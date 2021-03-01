#pragma once

#include"Utils.h"
#include"mesh/MeshModel.h"
#include"render/Shader.h"
#include"body/Body.h"

#include<vector>

class Camera;

class SolarSystemBase
{
public:
    SolarSystemBase();
    virtual ~SolarSystemBase();
};

class SolarSystem : public virtual SolarSystemBase, public virtual Renderable
{
public:
    SolarSystem(Time t, Camera &camera);
    ~SolarSystem();

    void Update(double dt);
    virtual void Render() const override;

    const Body &GetSun() const;
    const Body &GetEarth() const;
    const Body &GetMars() const;

private:
    static glm::dvec3 GetBodyPosition(const std::string &name, Time t);
    static glm::dvec3 GetBodyVelocity(const std::string &name, Time t);
    static double GetBodyMass(const std::string &name, Time t);

    Camera &m_camera;

    ShaderProgram m_sun_shader;
    ShaderProgram m_planet_shader;

    IcoModel m_sun_model;
    Body m_sun_body;

    IcoModel m_earth_model;
    Body m_earth_body;

    IcoModel m_mars_model;
    Body m_mars_body;

    Time m_time;
};