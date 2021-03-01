#pragma once

#include<ostream>
#include<vector>
#include<string>

constexpr const static double DIST_SCALE = 1e6;
constexpr const static double MASS_SCALE = 1.989e30;
constexpr const static double G = 6.67e-11 * MASS_SCALE / DIST_SCALE / DIST_SCALE / DIST_SCALE;

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

// Used for generating plots in Jupyter Lab
void WritePositions(const std::string &file_name, const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z);