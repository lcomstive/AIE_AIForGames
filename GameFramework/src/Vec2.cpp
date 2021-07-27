#include <Framework/Vec2.hpp>

using namespace std;
using namespace Framework;

Vec2::Vec2(float x, float y) : x(x), y(y) { }

Vec2 Vec2::Perpendicular() { return { x, -y }; }

float Vec2::Dot(Vec2& other) { return x * other.x + y * other.y; }

float Vec2::MagnitudeSqr() { return x * x + y * y; }
float Vec2::Magnitude() { return sqrt(MagnitudeSqr()); }

void Vec2::Normalize()
{
	float magnitude = Magnitude();
	x /= magnitude;
	y /= magnitude;
}

Vec2 Vec2::Normalized()
{
	Vec2 copy(*this);
	copy.Normalize();
	return copy;
}

float Vec2::Distance(Vec2& other) { return (*this - other).Magnitude(); }
float Vec2::Distance(Vec2& a, Vec2& b) { return (a - b).Magnitude(); }

float Vec2::DistanceSqr(Vec2& other) { return (*this - other).MagnitudeSqr(); }
float Vec2::DistanceSqr(Vec2& a, Vec2& b) { return (a - b).MagnitudeSqr(); }

float Vec2::Angle(Vec2& other) { return acos(Dot(other) / (Magnitude() * other.Magnitude())); }

Vec2 Vec2::Reflected(Vec2& normal) { return -2.0f * Dot(normal) * normal + *this; }

void Vec2::Reflect(Vec2& normal) { *this = Reflected(normal); }

Vec2 Vec2::Rotate(float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	float cx = x, cy = y;

	x = (cx * c) - (cy * s);
	y = (cx * s) + (cy * c);
	return *this;
}

Vec2 Vec2::RotateAround(Vec2& origin, float angle)
{
	float c = cos(angle);
	float s = sin(angle);
	float cx = x, cy = y;

	x = ((cx - origin.x) * c) - ((cy - origin.y) * s) + origin.x;
	y = ((cx - origin.x) * s) + ((cy - origin.y) * c) + origin.y;

	return *this;
}

/// --- OPERATORS --- ///
Vec2 Vec2::operator +(Vec2& other) { return { x + other.x, y + other.y }; }
Vec2 Vec2::operator -(Vec2& other) { return { x - other.x, y - other.y }; }
Vec2 Vec2::operator *(Vec2& other) { return { x * other.x, y * other.y }; }
Vec2 Vec2::operator /(Vec2& other) { return { x / other.x, y / other.y }; }
Vec2 Vec2::operator *(int v) { return { x * v, y * v }; }
Vec2 Vec2::operator /(int v) { return { x / v, y / v }; }
Vec2 Vec2::operator *(float v) { return { x * v, y * v }; }
Vec2 Vec2::operator /(float v) { return { x / v, y / v }; }
Vec2 Vec2::operator *(unsigned int v) { return { x * v, y * v }; }
Vec2 Vec2::operator /(unsigned int v) { return { x / v, y / v }; }
Vec2 Vec2::operator *(double v) { return { x * (float)v, y * (float)v }; }
Vec2 Vec2::operator /(double v) { return { x / (float)v, y / (float)v }; }

Vec2 Vec2::operator +=(Vec2& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

Vec2 Vec2::operator -=(Vec2& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

Vec2 Vec2::operator *=(Vec2& other)
{
	x *= other.x;
	y *= other.y;
	return *this;
}

Vec2 Vec2::operator /=(Vec2& other)
{
	x /= other.x;
	y /= other.y;
	return *this;
}

Vec2 Vec2::operator *=(float& v)
{
	x *= v;
	y *= v;
	return *this;
}

Vec2 Vec2::operator /=(float& v)
{
	x /= v;
	y /= v;
	return *this;
}

Vec2 Vec2::operator *=(int& v)
{
	x *= v;
	y *= v;
	return *this;
}

Vec2 Vec2::operator /=(int& v)
{
	x /= v;
	y /= v;
	return *this;
}

Vec2 Vec2::operator *=(double& v)
{
	x *= (float)v;
	y *= (float)v;
	return *this;
}

Vec2 Vec2::operator /=(double& v)
{
	x /= (float)v;
	y /= (float)v;
	return *this;
}

Vec2 Vec2::operator *=(unsigned int& v)
{
	x *= v;
	y *= v;
	return *this;
}

Vec2 Vec2::operator /=(unsigned int& v)
{
	x /= v;
	y /= v;
	return *this;
}

// Copy
Vec2& Vec2::operator=(const Vec2& other)
{
	x = other.x;
	y = other.y;
	return *this;
}

inline bool Vec2::operator ==(const Vec2& a) { return a.x == x && a.y == y; }
inline bool Vec2::operator !=(const Vec2& a) { return a.x == x && a.y == y; }

std::ostream& Framework::operator <<(std::ostream& os, const Vec2& v)
{
	os << "(" << v.x << ", " << v.y << ")";
	return os;
}
