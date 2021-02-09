#include"Sim.h"

#include"Constants.h"

#include<cmath>
#include<regex>
#include<sstream>
#include<algorithm>

// -------------------------- Vec -----------------------------
Vec::Vec():
    x(0.0), y(0.0), z(0.0) {}

Vec::Vec(double x, double y, double z):
    x(x), y(y), z(z) {}

Vec::Vec(const Vec &v):
    x(v.x), y(v.y), z(v.z) {}

Vec &Vec::operator=(const Vec &v) { x=v.x; y=v.y; z=v.z; return *this; }

Vec Vec::operator+(const Vec &v) const { return { x+v.x, y+v.y, z+v.z }; }
Vec Vec::operator-(const Vec &v) const { return { x-v.x, y-v.y, z-v.z }; }
Vec Vec::operator*(double s)     const { return {   x*s,   y*s,   z*s }; }
Vec Vec::operator-()             const { return {    -x,    -y,    -z }; }

// Dividing once and multiplying three times by the inverse is more efficient than dividing three times
Vec Vec::operator/(double s) const { double inv_s = 1.0 / s; return { x*inv_s, y*inv_s, z*inv_s }; }

double Vec::LengthSquared() const { return x*x + y*y + z*z; }

double Vec::Length() const { return sqrt(x*x + y*y + z*z); }
Vec Vec::Normalized() const { return *this / Length(); }

double Vec::Dot(const Vec &v1, const Vec &v2) { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }

std::ostream &operator<<(std::ostream &os, const Vec &v) { return os << '(' << v.x << ", " << v.y << ", " << v.z << ')'; }

// ------------------------- Time ----------------------------
Time::Time(): m_j2000(0.0) {}
Time::Time(double j2000): m_j2000(j2000) {}

double Time::Seconds() const { return m_j2000 * 86400.0; }
double Time::Minutes() const { return m_j2000 * 1440.0; }
double Time::Hours()   const { return m_j2000 * 24.0; }
double Time::Days()    const { return m_j2000; }
double Time::Years()   const { return m_j2000 / 365.2422; }

Time Time::FromSeconds(double seconds) { return seconds / 86400.0; }
Time Time::FromMinutes(double minutes) { return minutes / 1440.0; }
Time Time::FromHours(double hours)     { return hours / 24.0; }
Time Time::FromDays(double days)       { return days; }
Time Time::FromYears(double years)     { return years / 365.2422; }

Time Time::FromDate(int32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second)
{
    if (month < 3)
    {
        --year;
        month += 12;
    }

    int32_t A = static_cast<int32_t>(year / 100.0);
    int32_t B = 2 - A + static_cast<int32_t>(A / 4.0);
    
    return Time(365.25 * (year + 4716) + 30.6001 * (month + 1) + day + B - 1524.5);
}

Time Time::operator+(const Time &t) const { return m_j2000 + t.m_j2000; }
Time Time::operator-(const Time &t) const { return m_j2000 - t.m_j2000; }
Time &Time::operator+=(const Time &t)     { return *this = m_j2000 + t.m_j2000; }
Time &Time::operator-=(const Time &t)     { return *this = m_j2000 - t.m_j2000; }

bool Time::operator>(const Time &t)  const { return m_j2000 > t.m_j2000; }
bool Time::operator<(const Time &t)  const { return m_j2000 < t.m_j2000; }
bool Time::operator>=(const Time &t) const { return m_j2000 >= t.m_j2000; }
bool Time::operator<=(const Time &t) const { return m_j2000 <= t.m_j2000; }
bool Time::operator==(const Time &t) const { return m_j2000 == t.m_j2000; }

// --------------------------- Entity --------------------------
Entity::Entity(const Vec &position, const Vec &velocity, double mass):
    position(position),
    momentum(velocity * mass),
    force(),
    mass(mass),
    do_gravity(true),

    velocity(velocity),
    acceleration(),
    inv_mass(1.0 / mass)
{}

void Entity::Step(double dt)
{
    inv_mass = 1.0 / mass;

    // F = ma
    acceleration = force / mass;
    
    // Euler integration
    
    // dp/dt = F, p = mv
    momentum = momentum + force * dt;
    velocity = momentum * inv_mass;

    // ds/dt = v
    position = position + velocity * dt;
}

// --------------------- StaticEntity -------------------------
StaticEntity::StaticEntity(const std::string &data_path, const std::string &csv_path):
    Entity(Vec(), Vec(), 0.0),
    m_prev_time(),
    m_time_step(),
    m_first_time(),
    m_last_time(),
    m_elapsed_time(),
    m_index(0),
    m_states()
{
    do_gravity = false;

    std::ifstream in(csv_path);
    if (!in)
    {
        std::stringstream err_msg;
        err_msg << "Error finding csv file '" << csv_path << "'";
        throw std::runtime_error(err_msg.str());
    }

    // Parse necessary data:
    std::ifstream data(data_path);

    if (!data)
    {
        std::stringstream err_msg;
        err_msg << "Error finding data file '" << data_path << "'";
        throw std::runtime_error(err_msg.str());
    }

    std::regex gm_str("GM.*?=\\s*([0-9\\.]*?)\\s*"); // Find GM data in csv file (This may vary so the regex expression could be wrong)
    std::smatch gm_match;

    std::string line;
    while (std::getline(data, line))
    {
        if (std::regex_search(line.cbegin(), line.cend(), gm_match, gm_str))
        {
            double gm = std::strtod(gm_match[1].first.base(), nullptr);
            mass = gm / Constants::G * 1e9; // gm has units of km^3 / s^2, but we need m^3 / s^2
        }
    }

    data.close();

    // Start and end keys
    std::regex start_ephemerides("^\\$\\$SOE");
    std::regex end_ephemerides("^\\$\\$EOE");
    
    while (std::getline(in, line))
    {
        if (std::regex_match(line, start_ephemerides))
        {
            break;
        }
    }

    bool check_first_time = true;
    bool check_timestep = true;

    double jdtdb_time;
    State state;

    while (std::getline(in, line))
    {
        if (std::regex_match(line, end_ephemerides))
        {
            m_last_time = Time::FromDays(jdtdb_time);
            break;
        }

        ParseLine(line, jdtdb_time, state.position, state.velocity);

        if (check_timestep && !check_first_time)
        {
            check_timestep = false;
            m_time_step = Time::FromDays(jdtdb_time) - m_first_time;
        }

        if (check_first_time)
        {
            check_first_time = false;
            m_first_time = Time::FromDays(jdtdb_time);
        }

        m_states.push_back(state);
    }

    const State &initial = m_states[0];
    position = initial.position;
    velocity = initial.velocity;
}

const Vec &StaticEntity::GetPosition(Time t) const
{
    return m_states[static_cast<uint32_t>(std::clamp((t - m_first_time).Days(), 0.0, m_last_time.Days()) / m_time_step.Days())].position;
}

const Vec &StaticEntity::GetVelocity(Time t) const
{
    return m_states[static_cast<uint32_t>(std::clamp((t - m_first_time).Days(), 0.0, m_last_time.Days()) / m_time_step.Days())].velocity;
}

void StaticEntity::Step(double dt)
{
    m_elapsed_time += Time::FromSeconds(dt);
    Time next_time = m_prev_time + m_time_step;

    if (m_elapsed_time > next_time && m_elapsed_time < m_last_time)
    {
        m_prev_time = next_time;
        next_time += m_time_step;
        ++m_index;
    }   

    const State &s1 = m_states[m_index    ];
    const State &s2 = m_states[m_index + 1];

    double w = (m_elapsed_time - m_prev_time).Days() / (next_time - m_prev_time).Days();

    position = (s2.position - s1.position) * w + s1.position;
    velocity = (s2.velocity - s1.velocity) * w + s1.velocity;
}

void StaticEntity::ParseLine(const std::string &line, double &jdtdb_time, Vec &position, Vec &velocity)
{
    auto itr = line.begin();

    auto NextValue = [&itr](const std::string &line)
    {
        while (itr != line.end() && *itr != ',')
        {
            ++itr;
        }

        if (itr == line.end() || ++itr == line.end()) return false;

        ++itr; // jump comma and following space
        return true;
    };

    uint32_t index = 0;
    std::string::const_iterator values[8];

    do
    {
        values[index++] = itr;
    }
    while (NextValue(line));
    
    jdtdb_time = std::strtod(values[0].base(), nullptr);
    position.x = std::strtod(values[2].base(), nullptr) * 1000;
    position.y = std::strtod(values[3].base(), nullptr) * 1000;
    position.z = std::strtod(values[4].base(), nullptr) * 1000;
    velocity.x = std::strtod(values[5].base(), nullptr) * 1000;
    velocity.y = std::strtod(values[6].base(), nullptr) * 1000;
    velocity.z = std::strtod(values[7].base(), nullptr) * 1000;
}

// ----------------------- Simulation ------------------------
Simulation::Simulation():
    entities(),
    launch_phases(),
    elapsed_time()
{}

void Simulation::Step(Time dt)
{
    // Range for activating launch phases
    
    Time t_low = elapsed_time;
    Time t_high = elapsed_time += dt;

    for (auto itr : launch_phases)
    {
        Time time = itr.first;

        if (time >= t_low && time < t_high)
        {
            // Activate launch phase
            LaunchPhase launch_phase = itr.second;
            launch_phase.callback(*this, launch_phase.data);
        }
    }

    for (Entity *entity : entities)
    {
        entity->Step(dt.Seconds());

        new (&entity->force) Vec; // Clear forces
    }

    // Apply gravity (Iterating the following way assures that every entity pair is only simulated once)
    for (size_t i = 0; i < entities.size(); ++i)
    for (size_t j = i+1; j < entities.size(); ++j)
    {
        Entity *e1 = entities[i];
        Entity *e2 = entities[j];

        if (!e1->do_gravity && !e2->do_gravity) continue; // No need to check two static objects

        Vec e12 = e2->position - e1->position;
        // Costly operation but unavoidable
        Vec normal = e12.Normalized();

        // Fg = G * m1 * m2 / r^2
        double Fg = Constants::G * e1->mass * e2->mass / e12.LengthSquared();

        Vec gravity = normal * Fg;
        e1->force = e1->force + gravity;
        e2->force = e2->force - gravity;
    }
}