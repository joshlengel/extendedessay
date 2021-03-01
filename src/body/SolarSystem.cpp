#include"body/SolarSystem.h"
#include"mesh/MeshModel.h"
#include"camera/Camera.h"

#include<SpiceUsr.h>

SolarSystemBase::SolarSystemBase()
{
    furnsh_c("assets/solar_system.tm");
}

SolarSystemBase::~SolarSystemBase()
{
    unload_c("assets/solar_system.tm");
}

SolarSystem::SolarSystem(Time t, Camera &camera):
    m_camera(camera),
    m_sun_shader(),
    m_planet_shader(),
    m_sun_model(3),
    m_sun_body(GetBodyPosition("10", t), GetBodyVelocity("10", t), GetBodyMass("10", t), m_sun_model, m_sun_shader),
    m_earth_model(2),
    m_earth_body(GetBodyPosition("399", t), GetBodyVelocity("399", t), GetBodyMass("399", t), m_earth_model, m_planet_shader),
    m_mars_model(2),
    m_mars_body(GetBodyPosition("4", t), GetBodyVelocity("4", t), GetBodyMass("4", t), m_mars_model, m_planet_shader),
    m_time(t)
{
    m_sun_body.GetModel().SetScale(glm::vec3(6.9634e8f / DIST_SCALE));
    m_earth_body.GetModel().SetScale(glm::vec3(6.371e6f / DIST_SCALE));
    m_mars_body.GetModel().SetScale(glm::vec3(3.390e6f / DIST_SCALE));

    m_sun_shader.AttachShader(Shader::Load(ShaderType::VERTEX, "assets/shaders/sun.vert"));
    m_sun_shader.AttachShader(Shader::Load(ShaderType::FRAGMENT, "assets/shaders/sun.frag"));
    m_sun_shader.Make();

    m_sun_shader.DeclareUniform("model");
    m_sun_shader.DeclareUniform("view");
    m_sun_shader.DeclareUniform("projection");
    m_sun_shader.DeclareUniform("color");
    m_sun_shader.DeclareUniform("radius");
    m_sun_shader.DeclareUniform("glow_width");

    m_planet_shader.AttachShader(Shader::Load(ShaderType::VERTEX, "assets/shaders/planet.vert"));
    m_planet_shader.AttachShader(Shader::Load(ShaderType::FRAGMENT, "assets/shaders/planet.frag"));
    m_planet_shader.Make();

    m_planet_shader.DeclareUniform("model");
    m_planet_shader.DeclareUniform("view");
    m_planet_shader.DeclareUniform("projection");
    m_planet_shader.DeclareUniform("sunPosition");
    m_planet_shader.DeclareUniform("color");
}

SolarSystem::~SolarSystem()
{}

glm::dvec3 SolarSystem::GetBodyPosition(const std::string &name, Time t)
{
    SpiceDouble state[6];
    SpiceDouble lt;
    // Retrieve ephemeris from NASA database
    spkezr_c(name.c_str(), t.Seconds(), "J2000", "NONE", "SSB", state, &lt);

    return glm::dvec3(state[0], state[2], state[1]) * 1000.0 / DIST_SCALE; // Nasa uses km instead of m
}

glm::dvec3 SolarSystem::GetBodyVelocity(const std::string &name, Time t)
{
    SpiceDouble state[6];
    SpiceDouble lt;
    // Retrieve ephemeris from NASA database
    spkezr_c(name.c_str(), t.Seconds(), "J2000", "NONE", "SSB", state, &lt);

    return glm::dvec3(state[3], state[5], state[4]) * 1000.0 / DIST_SCALE; // Nasa uses km/s instead of m/s
}

double SolarSystem::GetBodyMass(const std::string &name, Time t)
{
    SpiceInt dim;
    SpiceDouble gm;
    bodvrd_c(name.c_str(), "GM", 1, &dim, &gm);

    return gm / (6.67e-11 * MASS_SCALE);
}

void SolarSystem::Update(double dt)
{
    m_time += Time::FromSeconds(dt);

    m_sun_body.SetPosition(GetBodyPosition("10", m_time));
    m_earth_body.SetPosition(GetBodyPosition("399", m_time));
    m_mars_body.SetPosition(GetBodyPosition("4", m_time));

    m_sun_body.SetVelocity(GetBodyVelocity("10", m_time));
    m_earth_body.SetVelocity(GetBodyVelocity("399", m_time));
    m_mars_body.SetVelocity(GetBodyVelocity("4", m_time));

    NoSolver solver;
    m_sun_body.Update(dt, solver);
    m_earth_body.Update(dt, solver);
    m_mars_body.Update(dt, solver);
    
}

void SolarSystem::Render() const
{
    glm::mat4 projection = m_camera.GetProjectionMatrix();
    glm::mat4 view = m_camera.GetViewMatrix();

    m_sun_shader.Bind();
    m_sun_shader.SetUniform("projection", projection);
    m_sun_shader.SetUniform("view", view);

    m_sun_shader.SetUniform("color", { 250.0f / 255.0f, 212.0f / 255.0f, 64.0f / 255.0f });
    m_sun_shader.SetUniform("radius", 1.0f);
    m_sun_shader.SetUniform("glow_width", 0.2f);
    m_sun_body.Render();

    m_planet_shader.Bind();
    m_planet_shader.SetUniform("projection", projection);
    m_planet_shader.SetUniform("view", view);
    m_planet_shader.SetUniform("sunPosition", glm::vec3(m_sun_body.GetPosition()));

    m_planet_shader.SetUniform("color", glm::vec3(30.0f / 255.0f, 156.0f / 255.0f, 42.0f / 255.0f));
    m_earth_body.Render();

    m_planet_shader.SetUniform("color", glm::vec3(163.0f / 255.0f, 50.0f / 255.0f, 5.0f / 255.0f));
    m_mars_body.Render();
}

const Body &SolarSystem::GetSun() const { return m_sun_body; }
const Body &SolarSystem::GetEarth() const { return m_earth_body; }
const Body &SolarSystem::GetMars() const { return m_mars_body; }