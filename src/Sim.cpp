#include"Sim.h"

#include"Constants.h"

#include<cmath>

#include<SpiceUsr.h>

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
    Vec p, v;
    GetState(p, v, t);
    return p;
}

Vec StaticEntity::GetVelocity(Time t) const
{
    Vec p, v;
    GetState(p, v, t);
    return v;
}

void StaticEntity::Step(double dt)
{
    GetState(position, velocity, m_elapsed += Time::FromSeconds(dt));
}

void StaticEntity::GetState(Vec &p, Vec &v, Time t) const
{
    double state[6];
    double lt;
    // Retrieve ephemeris from NASA database
    spkezr_c(m_target.c_str(), t.Seconds(), FRAME.c_str(), CORRECTION.c_str(), m_observer.c_str(), state, &lt);

    p.x = state[0]; p.y = state[1]; p.z = state[2];
    v.x = state[3]; v.y = state[4]; v.z = state[5];

    p = p * 1000;
    v = v * 1000;
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