#include"Sim.h"

#include"Constants.h"

#include<cmath>

#include<SpiceUsr.h>

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
Vec Vec::Cross(const Vec &v1, const Vec &v2) { return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x }; }

Vec Vec::Rotate(const Vec &v, const Vec &axis, double angle)
{
    double sa = sin(angle);
    double ca = cos(angle);

    double mat[9] =
    {
        ca + axis.x * axis.x * (1.0f - ca), axis.x * axis.y * (1.0f - ca) - axis.z * sa, axis.x * axis.z * (1.0f - ca) + axis.y * sa,
        axis.y * axis.x * (1.0f - ca) + axis.z * sa, ca + axis.y * axis.y * (1.0f - ca), axis.y * axis.z * (1.0f - ca) - axis.x * sa,
        axis.z * axis.x * (1.0f - ca) - axis.y * sa, axis.z * axis.y * (1.0f - ca) + axis.x * sa, ca + axis.z * axis.z * (1.0f - ca)
    };

    return
    {
        v.x * mat[0] + v.y * mat[1] + v.z * mat[2],
        v.x * mat[3] + v.y * mat[4] + v.z * mat[5],
        v.x * mat[6] + v.y * mat[7] + v.z * mat[8],
    };
}

std::ostream &operator<<(std::ostream &os, const Vec &v) { return os << '(' << v.x << ", " << v.y << ", " << v.z << ')'; }

// ------------------------- Time ----------------------------
Time::Time(): m_seconds(0.0) {}
Time::Time(double seconds): m_seconds(seconds) {}

double Time::Seconds() const { return m_seconds; }
double Time::Minutes() const { return m_seconds / 60.0; }
double Time::Hours()   const { return m_seconds / 3600.0; }
double Time::Days()    const { return m_seconds / 86400.0; }

std::string Time::Str() const { char buff[30]; et2utc_c(m_seconds, "ISOC", 2, 30, buff); return buff; }

Time Time::FromSeconds(double seconds) { return seconds; }
Time Time::FromMinutes(double minutes) { return minutes * 60.0; }
Time Time::FromHours(double hours)     { return hours * 3600.0; }
Time Time::FromDays(double days)       { return days * 86400.0; }

// str2et_c conveniently returns J2000 date in seconds
Time Time::FromStr(const std::string &date) { double t; str2et_c(date.c_str(), &t); return t; }

Time Time::operator+(const Time &t) const { return m_seconds + t.m_seconds; }
Time Time::operator-(const Time &t) const { return m_seconds - t.m_seconds; }
Time Time::operator*(double d) const { return m_seconds * d; }
Time Time::operator/(double d) const { return m_seconds / d; }
Time &Time::operator+=(const Time &t)     { return *this = m_seconds + t.m_seconds; }
Time &Time::operator-=(const Time &t)     { return *this = m_seconds - t.m_seconds; }
Time &Time::operator*=(double d) { return *this = m_seconds * d; }
Time &Time::operator/=(double d) { return *this = m_seconds / d; }

bool Time::operator>(const Time &t)  const { return m_seconds > t.m_seconds; }
bool Time::operator<(const Time &t)  const { return m_seconds < t.m_seconds; }
bool Time::operator>=(const Time &t) const { return m_seconds >= t.m_seconds; }
bool Time::operator<=(const Time &t) const { return m_seconds <= t.m_seconds; }
bool Time::operator==(const Time &t) const { return m_seconds == t.m_seconds; }

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
StaticEntity::StaticEntity(const std::string &target, const std::string &observer):
    Entity(Vec(), Vec(), 0.0),
    m_target(target),
    m_observer(observer),
    m_elapsed()
{
    do_gravity = false;

    int dim = 1;
    double gm;
    bodvrd_c(target.c_str(), "GM", 1, &dim, &gm);
    mass = gm / Constants::G * 1e9;
}

StaticEntity::StaticEntity(const StaticEntity &entity):
    Entity(Vec(), Vec(), 0.0),
    m_target(entity.m_target),
    m_observer(entity.m_observer),
    m_elapsed()
{
    do_gravity = entity.do_gravity;
    mass = entity.mass;
}

void StaticEntity::Init(Time t)
{
    m_elapsed = t;
    GetState(position, velocity, t);
}

Vec StaticEntity::GetPosition(Time t) const
{
    Vec position, velocity;
    GetState(position, velocity, t);
    return position;
}

Vec StaticEntity::GetVelocity(Time t) const
{
    Vec position, velocity;
    GetState(position, velocity, t);
    return velocity;
}

void StaticEntity::Step(double dt)
{
    GetState(position, velocity, m_elapsed += Time::FromSeconds(dt));
}

void StaticEntity::GetState(Vec &position, Vec &velocity, Time t) const
{
    double state[6];
    double lt;
    // Retrieve ephemeris from NASA database
    spkezr_c(m_target.c_str(), t.Seconds(), FRAME.c_str(), CORRECTION.c_str(), m_observer.c_str(), state, &lt);

    position.x = state[0]; position.y = state[1]; position.z = state[2];
    velocity.x = state[3]; velocity.y = state[4]; velocity.z = state[5];

    position = position * 1000;
    velocity = velocity * 1000;
}

void StaticEntity::Reset(Time t)
{
    Init(t);
}

SolarSystem_Base::SolarSystem_Base(const std::string &kernel_path)
{
    furnsh_c(kernel_path.c_str());
}

SolarSystem_Base::SolarSystem_Base()
{}

SolarSystem::SolarSystem(const std::string &kernel_path):
    SolarSystem_Base(kernel_path),
    sun    ("10",  "SSB"),
    mercury("1",   "SSB"),
    venus  ("2",   "SSB"),
    earth  ("399", "SSB"),
    moon   ("301", "SSB"),
    mars   ("4",   "SSB"),
    jupyter("5",   "SSB"),
    saturn ("6",   "SSB"),
    uranus ("7",   "SSB"),
    neptune("8",   "SSB"),
    m_kernel_path(kernel_path)
{}

SolarSystem::SolarSystem(const SolarSystem &system):
    SolarSystem_Base(),
    sun    (system.sun    ),
    mercury(system.mercury),
    venus  (system.venus  ),
    earth  (system.earth  ),
    moon   (system.moon   ),
    mars   (system.mars   ),
    jupyter(system.jupyter),
    saturn (system.saturn ),
    uranus (system.uranus ),
    neptune(system.neptune),
    m_kernel_path(system.m_kernel_path)
{}

SolarSystem::~SolarSystem()
{
    //unload_c(m_kernel_path.c_str());
}

void SolarSystem::Init(Time t)
{
    sun    .Init(t);
    mercury.Init(t);
    venus  .Init(t);
    earth  .Init(t);
    moon   .Init(t);
    mars   .Init(t);
    jupyter.Init(t);
    saturn .Init(t);
    uranus .Init(t);
    neptune.Init(t);
}

void SolarSystem::Reset(Time t)
{
    sun    .Reset(t);
    mercury.Reset(t);
    venus  .Reset(t);
    earth  .Reset(t);
    moon   .Reset(t);
    mars   .Reset(t);
    jupyter.Reset(t);
    saturn .Reset(t);
    uranus .Reset(t);
    neptune.Reset(t);
}

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

void Simulation::Reset(Time t)
{
    elapsed_time = t;
}