#pragma once

#include<ostream>
#include<fstream>

#include<vector>
#include<map>
#include<unordered_map>

// Simple struct for representing 3D linear algebra calculations
struct Vec
{
    double x, y, z;

    Vec();
    Vec(double x, double y, double z);
    Vec(const Vec &v);

    Vec &operator=(const Vec &v);

    Vec operator+(const Vec &v) const;
    Vec operator-(const Vec &v) const;
    Vec operator*(double s) const;
    Vec operator-() const;

    // Dividing once and multiplying three times by the inverse is more efficient than dividing three times
    Vec operator/(double s) const;

    double LengthSquared() const;

    // Length operation (and consequently "Normalized") should be avoided due to costly sqrt function 
    double Length() const;
    Vec Normalized() const;

    static double Dot(const Vec &v1, const Vec &v2);
    static Vec Cross(const Vec &v1, const Vec &v2);
    static Vec Rotate(const Vec &v, const Vec &axis, double angle);
};

// Useful for outputting debug info

std::ostream &operator<<(std::ostream &os, const Vec &v);

// Struct representing a time (Mainly used for conversions)
struct Time
{
    Time();

    double Seconds() const;
    double Minutes() const;
    double Hours() const;
    double Days() const;
    std::string Str() const;

    static Time FromSeconds(double seconds);
    static Time FromMinutes(double minutes);
    static Time FromHours(double hours);
    static Time FromDays(double days);
    static Time FromStr(const std::string &date);

    Time operator+(const Time &t) const;
    Time operator-(const Time &t) const;
    Time operator*(double d) const;
    Time operator/(double d) const;
    Time &operator+=(const Time &t);
    Time &operator-=(const Time &t);
    Time &operator*=(double d);
    Time &operator/=(double d);

    bool operator>(const Time &t) const;
    bool operator<(const Time &t) const;
    bool operator>=(const Time &t) const;
    bool operator<=(const Time &t) const;
    bool operator==(const Time &t) const;

private:
    Time(double seconds);

    double m_seconds;
};

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