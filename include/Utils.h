#pragma once

#include<ostream>
#include<vector>
#include<string>

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

void WritePositions(const std::string &file_name, const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z);