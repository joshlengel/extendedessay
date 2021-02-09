#include"Launch.h"

#include"Sim.h"
#include"Constants.h"
#include"Json.h"

#include<cmath>
#include<fstream>
#include<iomanip>
#include<iostream>

double C2(double psi)
{
    if (psi >= 0.0)
    {
        double sp = sqrt(psi);
        return (1 - cos(sp)) / psi;
    }
    else
    {
        double sp = sqrt(-psi);
        return (1 - cosh(sp)) / psi;
    }
}

double C3(double psi)
{
    if (psi >= 0.0)
    {
        double sp = sqrt(psi);
        return (sp - sin(sp)) / (psi * sp);
    }
    else
    {
        double sp = sqrt(-psi);
        return (sp - sinh(sp)) / (psi * sp);
    }
    
}

bool lambert_solve(const Vec &r1, const Vec &r2, double tof, double mu, uint32_t max_itr, double tol, Vec &v1, Vec &v2)
{
    double R1 = r1.Length();
    double R2 = r2.Length();
    
    double gamma = Vec::Dot(r1, r2) / (R1 * R2);
    double A = sqrt(R1 * R2 * (1.0 + gamma));

    if (A == 0.0) return false;

    double psi = 0.0;
    double psi_l = -4.0 * M_PI * M_PI;
    double psi_u =  4.0 * M_PI * M_PI;

    double c2 = 0.5;
    double c3 = 1.0 / 6.0;

    for (uint32_t i = 0; i < max_itr; ++i)
    {
        double B = R1 + R2 + A * (psi * c3 - 1.0) / sqrt(c2);

/*
        if (A > 0.0)
        {
            while (B < 0.0)
            {
                psi_l = psi;
                psi = 0.8 * (1.0 - R1 * R2 * sqrt(C2(psi)) / A) / C3(psi);
                B = R1 + R2 + A * (psi * C3(psi) - 1.0) / sqrt(C2(psi));
            }
        }
*/
        if (A > 0.0 && B < 0.0)
        {
            psi_l += M_PI;
            B = -B;
        }

        double chi = sqrt(B / c2);
        double chi3 = chi * chi * chi;

        double tof_new = (chi3 * c3 + A * sqrt(B)) / sqrt(mu);

        if (fabs(tof_new - tof) < tol)
        {
            // Convergence

            double f = 1.0 - B / R1;
            double g = A * sqrt(B / mu);
            double g_dot = 1.0 - B / R2;

            v1 = (r2 - r1 * f) / g;
            v2 = (r2 * g_dot - r1) / g;

            return true;
        }

        if (tof_new <= tof)
            psi_l = psi;
        else
            psi_u = psi;
        
        psi = (psi_l + psi_u) * 0.5;
        c2 = C2(psi);
        c3 = C3(psi);
    }

    return false;
}

void LaunchData::Compute(SolarSystem *solar_system)
{
    Vec v0, vf;
    if (lambert_solve(solar_system->earth.GetPosition(time_inject), solar_system->mars.GetPosition(time_exit), (time_exit - time_inject).Seconds(), Constants::G * solar_system->sun.mass, 400, 1e-8, v0, vf))
    {
        Vec v0i = v0.Normalized();
        Vec ve = v0i * Vec::Dot(v0i, solar_system->earth.GetVelocity(time_inject));
        double v_inf_e_squared = (v0 - ve).LengthSquared();
        c3 = v_inf_e_squared;
        
        double v1 = sqrt(2.0 * Constants::G * solar_system->earth.mass / Rleo + v_inf_e_squared);
        dv_1 = v1 - sqrt(Constants::G * solar_system->earth.mass / Rleo);

        Vec vfi = vf.Normalized();
        Vec vm = vfi * Vec::Dot(vfi, solar_system->mars.GetVelocity(time_exit));
        double v_inf_m_squared = (vf - vm).LengthSquared();
        v_inf = sqrt(v_inf_m_squared);

        double v2 = sqrt(2.0 * Constants::G * solar_system->mars.mass / Rlmo + v_inf_m_squared);
        dv_2 = v2 - sqrt(Constants::G * solar_system->mars.mass / Rlmo);
    }
    else
    {
        c3 = 1e20;
        v_inf = 1e20;
        dv_1 = 1e20;
        dv_2 = 1e20;
    }
}

void GeneratePorkChop(const std::string &out, SolarSystem *solar_system, const LaunchData &launch_data)
{
    Time inject_span = Time::FromDays(30);
    Time inject_start = launch_data.time_inject - inject_span;
    Time inject_end = launch_data.time_inject + inject_span;

    Time exit_span = Time::FromDays(100);
    Time exit_start = launch_data.time_exit - exit_span;
    Time exit_end = launch_data.time_exit + exit_span;

    uint32_t precision = 1000;
    Time step_x = inject_span * 2.0 / precision;
    Time step_y = exit_span * 2.0 / precision;

    std::vector<double> x;
    std::vector<double> y;
    std::vector<double> z;
    x.reserve(precision * precision);
    y.reserve(precision * precision);
    z.reserve(precision * precision);

    LaunchData ld = launch_data;

    Time tx = inject_start;
    
    for (uint32_t i = 0; i < precision; ++i)
    {
        Time ty = exit_start;

        for (uint32_t j = 0; j < precision; ++j)
        {
            ld.time_inject = tx;
            ld.time_exit = ty;
            ld.Compute(solar_system);

            x.push_back(tx.Days());
            y.push_back(ty.Days());
            z.push_back(ld.dv_1);

            ty += step_y;
        }

        tx += step_x;
    }

    /*
    std::ofstream file(out);

    file << std::setprecision(13)
        << "{\n\t\"x\":[";
    
    for (uint32_t i = 0; i < x.size() - 1; ++i)
        file << x[i] << ",";
    
    file << x.back() << "],\n\t\"y\":[";

    for (uint32_t i = 0; i < y.size() - 1; ++i)
        file << y[i] << ",";
    
    file << y.back() << "],\n\t\"z\":[";

    for (uint32_t i = 0; i < z.size() - 1; ++i)
        file << z[i] << ",";
    
    file << z.back() << "]\n}";*/

    std::ofstream file(out);
    file << std::setprecision(13);

    JsonWriter writer(file);
    writer.Begin();

    writer.Write("x");
    JsonArray x_obj(writer);
    x_obj.Begin();
    x_obj.WritePrimitiveArray(x.begin(), x.end());
    x_obj.End();

    writer.Write("y");
    JsonArray y_obj(writer);
    y_obj.Begin();
    y_obj.WritePrimitiveArray(y.begin(), y.end());
    y_obj.End();

    writer.Write("z");
    JsonArray z_obj(writer);
    z_obj.Begin();
    z_obj.WritePrimitiveArray(z.begin(), z.end());
    z_obj.End();

    writer.End();
}