#include"Utils.h"

#include<cmath>
#include<fstream>
#include<iomanip>

#include<SpiceUsr.h>

// -------------------------- Vec -----------------------------
Vec::Vec():
    x(0.0), y(0.0), z(0.0) {}

Vec::Vec(double x, double y, double z):
    x(x), y(y), z(z) {}

Vec::Vec(const Vec &v):
    x(v.x), y(v.y), z(v.z) {}

Vec &Vec::operator=(const Vec &v) { x=v.x; y=v.y; z=v.z; return *this; }

Vec Vec::operator+(const Vec &v) const { return { x+v.x, y+v.y, z+v.z }; }
Vec Vec::operator-(const Vec &v) const { return { x-v.x, y-v.y, z-v.z }; }
Vec Vec::operator*(double s)     const { return {   x*s,   y*s,   z*s }; }
Vec Vec::operator-()             const { return {    -x,    -y,    -z }; }

// Dividing once and multiplying three times by the inverse is more efficient than dividing three times
Vec Vec::operator/(double s) const { double inv_s = 1.0 / s; return { x*inv_s, y*inv_s, z*inv_s }; }

double Vec::LengthSquared() const { return x*x + y*y + z*z; }

double Vec::Length() const { return sqrt(x*x + y*y + z*z); }
Vec Vec::Normalized() const { return *this / Length(); }

double Vec::Dot(const Vec &v1, const Vec &v2) { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }
Vec Vec::Cross(const Vec &v1, const Vec &v2) { return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x }; }

Vec Vec::Rotate(const Vec &v, const Vec &axis, double angle)
{
    double sa = sin(angle);
    double ca = cos(angle);

    double mat[9] =
    {
        ca + axis.x * axis.x * (1.0f - ca), axis.x * axis.y * (1.0f - ca) - axis.z * sa, axis.x * axis.z * (1.0f - ca) + axis.y * sa,
        axis.y * axis.x * (1.0f - ca) + axis.z * sa, ca + axis.y * axis.y * (1.0f - ca), axis.y * axis.z * (1.0f - ca) - axis.x * sa,
        axis.z * axis.x * (1.0f - ca) - axis.y * sa, axis.z * axis.y * (1.0f - ca) + axis.x * sa, ca + axis.z * axis.z * (1.0f - ca)
    };

    return
    {
        v.x * mat[0] + v.y * mat[1] + v.z * mat[2],
        v.x * mat[3] + v.y * mat[4] + v.z * mat[5],
        v.x * mat[6] + v.y * mat[7] + v.z * mat[8],
    };
}

std::ostream &operator<<(std::ostream &os, const Vec &v) { return os << '(' << v.x << ", " << v.y << ", " << v.z << ')'; }

// ------------------------- Time ----------------------------
Time::Time(): m_seconds(0.0) {}
Time::Time(double seconds): m_seconds(seconds) {}

double Time::Seconds() const { return m_seconds; }
double Time::Minutes() const { return m_seconds / 60.0; }
double Time::Hours()   const { return m_seconds / 3600.0; }
double Time::Days()    const { return m_seconds / 86400.0; }

std::string Time::Str() const { char buff[30]; et2utc_c(m_seconds, "ISOC", 2, 30, buff); return buff; }

Time Time::FromSeconds(double seconds) { return seconds; }
Time Time::FromMinutes(double minutes) { return minutes * 60.0; }
Time Time::FromHours(double hours)     { return hours * 3600.0; }
Time Time::FromDays(double days)       { return days * 86400.0; }

// str2et_c conveniently returns J2000 date in seconds
Time Time::FromStr(const std::string &date) { double t; str2et_c(date.c_str(), &t); return t; }

Time Time::operator+(const Time &t) const { return m_seconds + t.m_seconds; }
Time Time::operator-(const Time &t) const { return m_seconds - t.m_seconds; }
Time Time::operator*(double d) const { return m_seconds * d; }
Time Time::operator/(double d) const { return m_seconds / d; }
Time &Time::operator+=(const Time &t)     { return *this = m_seconds + t.m_seconds; }
Time &Time::operator-=(const Time &t)     { return *this = m_seconds - t.m_seconds; }
Time &Time::operator*=(double d) { return *this = m_seconds * d; }
Time &Time::operator/=(double d) { return *this = m_seconds / d; }

bool Time::operator>(const Time &t)  const { return m_seconds > t.m_seconds; }
bool Time::operator<(const Time &t)  const { return m_seconds < t.m_seconds; }
bool Time::operator>=(const Time &t) const { return m_seconds >= t.m_seconds; }
bool Time::operator<=(const Time &t) const { return m_seconds <= t.m_seconds; }
bool Time::operator==(const Time &t) const { return m_seconds == t.m_seconds; }

void WritePositions(const std::string &file_name, const std::vector<double> &x, const std::vector<double> &y, const std::vector<double> &z)
{
    std::ofstream file(file_name);
    file << std::setprecision(13);

    file << "{\n\t\"x\":[" << x[0];
    
    for (auto itr = x.begin() + 1; itr != x.end(); ++itr)
        file << "," << *itr;
    
    file << "],\n\t\"y\":[" << y[0];

    for (auto itr = y.begin() + 1; itr != y.end(); ++itr)
        file << "," << *itr;

    file << "],\n\t\"z\":[" << z[0];

    for (auto itr = z.begin() + 1; itr != z.end(); ++itr)
        file << "," << *itr;
    
    file << "]\n}";
}