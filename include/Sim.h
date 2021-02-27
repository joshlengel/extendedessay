#pragma once

#include"Utils.h"

#include<ostream>
#include<fstream>

#include<vector>
#include<map>
#include<unordered_map>

// Struct representing a physical body (Not necessarily a rigid)
struct Entity
{
    // Quantities representing the state of the object
    Vec position;
    Vec momentum;
    Vec force;

    double mass;

    bool do_gravity;

    // Derived quantities
    Vec velocity;
    Vec acceleration;

    // Used to speed up calculations by avoiding division
    double inv_mass;

    Entity(const Vec &position, const Vec &velocity, double mass);

    virtual void Step(double dt);
};

// Entity which has already been numerically integrated
struct StaticEntity : Entity
{
    StaticEntity(const std::string &target, const std::string &observer);
    StaticEntity(const StaticEntity &entitiy);
    void Init(Time t);

    Vec GetPosition(Time t) const;
    Vec GetVelocity(Time t) const;

    void Step(double dt) override;
    void Reset(Time t);

private:
    std::string m_target;
    std::string m_observer;

    inline static const std::string FRAME = "J2000";
    inline static const std::string CORRECTION = "NONE";

    Time m_elapsed;

    void GetState(Vec &position, Vec &velocity, Time t) const;
};

// Used for executing statements before planets initialize
struct SolarSystem_Base { protected: SolarSystem_Base(const std::string &kernel_path); SolarSystem_Base(); };

struct SolarSystem : public SolarSystem_Base
{
    StaticEntity sun;
    StaticEntity mercury;
    StaticEntity venus;
    StaticEntity earth;
    StaticEntity moon;
    StaticEntity mars;
    StaticEntity jupyter;
    StaticEntity saturn;
    StaticEntity uranus;
    StaticEntity neptune;

    SolarSystem(const std::string &kernel_path);
    SolarSystem(const SolarSystem &system);
    ~SolarSystem();

    void Init(Time t);
    void Reset(Time t);

private:
    std::string m_kernel_path;
};

struct Simulation
{
    // All of the bodies being simulated
    std::vector<Entity*> entities;

    // Time points with associated function calls to represent launch phases
    struct LaunchPhase
    {
        void(*callback)(Simulation &sim, void *data);
        void *data;
    };
    
    std::map<Time, LaunchPhase> launch_phases;

    Time elapsed_time;

    void Step(Time dt);
    void Reset(Time t);
};