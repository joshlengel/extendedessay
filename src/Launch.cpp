#include"Launch.h"

#include"Sim.h"
#include"Constants.h"

#include<cmath>

static double C2(double psi, double sqrt_psi) { return psi < 0? (1 - cosh(sqrt_psi)) / psi : (1 - cos(sqrt_psi)) / psi; }
static double C3(double psi, double sqrt_psi) { return psi < 0? (sinh(sqrt_psi) - sqrt_psi) / (sqrt_psi * psi) : (sqrt_psi - sin(sqrt_psi)) / (sqrt_psi * psi); }

static double abs(double d) { return d < 0? -d : d; }

// Use universal variables solution to lambert's problem for keplerian orbit
bool lambert_uni_variables(const Vec &r1, const Vec &r2, double dt, double mu, double tol, uint32_t M, Vec &v0, Vec &vf)
{
    double sqrt_mu = sqrt(mu);

    double r1_length = r1.Length();
    double r2_length = r2.Length();

    double gamma = Vec::Dot(r1, r2) / (r1_length * r2_length);
    //double beta = sqrt(1 - gamma * gamma);
    double A = sqrt(r1_length * r2_length * (1 + gamma));

    if (A == 0.0) return false;

    double psi = 0;
    double psi_l = -4.0 * M_PI;
    double psi_u = 4.0 * M_PI * M_PI;

    double c2 = 0.5;
    double c3 = 1.0 / 6.0;

    for (uint32_t i = 0; i < M; ++i)
    {
        double B = r1_length + r2_length + A * (psi * c3 - 1) / sqrt(c2);

        if (A > 0.0 && B < 0.0)
        {
            psi_l += M_PI;
            B = -B;
        }

        double chi_cubed = pow(B / c2, 1.5);
        double dt_ = (chi_cubed * c3 + A * sqrt(B)) / sqrt_mu;

        if (abs(dt - dt_) <= tol)
        {
            // Solved
            double f = 1 - B / r1_length;
            double g = A * sqrt(B / mu);
            double g_dot = 1 - B / r2_length;

            v0 = (r2 - r1 * f) / g;
            vf = (r2 * g_dot - r1) / g;

            return true;
        }
        
        if (dt_ <= dt)
            psi_l = psi;
        else
            psi_u = psi;
        
        psi = (psi_l + psi_u) * 0.5;
        double sqrt_psi = sqrt(abs(psi));
        c2 = C2(psi, sqrt_psi);
        c3 = C3(psi, sqrt_psi);
    }

    return false;
}

void LaunchData::Compute(SolarSystem *solar_system)
{
    Vec v0, vf;
    if (lambert_uni_variables(solar_system->earth.GetPosition(time_inject), solar_system->mars.GetPosition(time_exit), (time_exit - time_inject).Seconds(), Constants::G * solar_system->sun.mass, 1e-4, 200, v0, vf))
    {
        double v_inf_squared = (v0 - solar_system->earth.GetVelocity(time_inject)).LengthSquared();
        dv_1 = v_inf_squared;

       /* double v_or_squared = Constants::G * earth->mass / Rleo;
        double v_esc_squared = 2.0 * v_or_squared;

        double v_req = sqrt(v_esc_squared + v_inf_squared);

        dv_1 = v_req - sqrt(v_or_squared);*/
    }
}