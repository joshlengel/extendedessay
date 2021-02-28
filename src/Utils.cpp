#include"Utils.h"

#include<cmath>
#include<fstream>
#include<iomanip>

#include<SpiceUsr.h>

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