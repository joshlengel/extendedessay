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
    double Years() const;
    double J2000() const;

    static Time FromSeconds(double seconds);
    static Time FromMinutes(double minutes);
    static Time FromHours(double hours);
    static Time FromDays(double days);
    static Time FromYears(double years);

    static Time FromDate(int32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second = 0);

    Time operator+(const Time &t) const;
    Time operator-(const Time &t) const;
    Time &operator+=(const Time &t);
    Time &operator-=(const Time &t);

    bool operator>(const Time &t) const;
    bool operator<(const Time &t) const;
    bool operator>=(const Time &t) const;
    bool operator<=(const Time &t) const;
    bool operator==(const Time &t) const;

private:
    Time(double j2000);

    double m_j2000;
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
    StaticEntity(const std::string &data_path, const std::string &csv_path);

    const Vec &GetPosition(Time t) const;
    const Vec &GetVelocity(Time t) const;

    void Step(double dt) override;

private:
    struct State
    {
        Vec position;
        Vec velocity;
    };

    Time m_prev_time;
    Time m_time_step;
    Time m_first_time;
    Time m_last_time;
    Time m_elapsed_time;
    uint32_t m_index;

    std::vector<State> m_states;

    static void ParseLine(const std::string &line, double &jdtdb_time, Vec &position, Vec &velocity);
};

struct SolarSystem
{
    Entity *sun;
    Entity *mercury;
    Entity *venus;
    Entity *earth;
    Entity *moon;
    Entity *mars;
    Entity *jupyter;
    Entity *saturn;
    Entity *uranus;
    Entity *neptune;
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

    Simulation();

    void Step(Time dt);
};