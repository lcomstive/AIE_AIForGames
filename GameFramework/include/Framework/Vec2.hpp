#pragma once

#include <iostream>
#include <raylib.h>
#include <box2d/b2_math.h>

namespace Framework
{
	struct Vec2
	{
		float x, y;

		Vec2(float x = 0, float y = 0) : x(x), y(y) { }

		Vec2 Perpendicular() { return { x, -y }; }

		float Dot(Vec2& other) { return x * other.x + y * other.y; }

		float MagnitudeSqr() { return x * x + y * y; }
		float Magnitude() { return sqrt(MagnitudeSqr()); }

		void Normalize()
		{
			float magnitude = Magnitude();
			x /= magnitude;
			y /= magnitude;
		}

		Vec2 Normalized()
		{
			Vec2 copy(*this);
			copy.Normalize();
			return copy;
		}

		float Distance(Vec2& other) { return (*this - other).Magnitude(); }
		static float Distance(Vec2& a, Vec2& b) { return (a - b).Magnitude(); }

		float DistanceSqr(Vec2& other) { return (*this - other).MagnitudeSqr(); }
		static float DistanceSqr(Vec2& a, Vec2& b) { return (a - b).MagnitudeSqr(); }

		float Angle(Vec2& other) { return acos(Dot(other) / (Magnitude() * other.Magnitude())); }

		Vec2 Reflected(Vec2& normal) { return -2.0f * Dot(normal) * normal + *this; }

		void Reflect(Vec2& normal) { *this = Reflected(normal); }

		Vec2 Rotate(float angle)
		{
			float c = cos(angle);
			float s = sin(angle);
			float cx = x, cy = y;

			x = (cx * c) - (cy * s);
			y = (cx * s) + (cy * c);
			return *this;
		}

		Vec2 RotateAround(Vec2& origin, float angle)
		{
			float c = cos(angle);
			float s = sin(angle);
			float cx = x, cy = y;

			x = ((cx - origin.x) * c) - ((cy - origin.y) * s) + origin.x;
			y = ((cx - origin.x) * s) + ((cy - origin.y) * c) + origin.y;
			return *this;
		}

		/// --- OPERATORS --- ///
		Vec2 operator +(Vec2& other) { return { x + other.x, y + other.y }; }
		Vec2 operator -(Vec2& other) { return { x - other.x, y - other.y }; }
		Vec2 operator *(Vec2& other) { return { x * other.x, y * other.y }; }
		Vec2 operator /(Vec2& other) { return { x / other.x, y / other.y }; }
		Vec2 operator *(float v) { return { x * v, y * v }; }
		Vec2 operator /(float v) { return { x / v, y / v }; }

		Vec2 operator +=(Vec2& other)
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		Vec2 operator -=(Vec2& other)
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		Vec2 operator *=(Vec2& other)
		{
			x *= other.x;
			y *= other.y;
			return *this;
		}

		Vec2 operator /=(Vec2& other)
		{
			x /= other.x;
			y /= other.y;
			return *this;
		}
		
		Vec2 operator *=(float& v)
		{
			x *= v;
			y *= v;
			return *this;
		}

		Vec2 operator /=(float& v)
		{
			x /= v;
			y /= v;
			return *this;
		}

		// Copy
		Vec2& operator=(const Vec2& other)
		{
			x = other.x;
			y = other.y;
			return *this;
		}

		std::ostream& operator <<(std::ostream& os)
		{
			os << "(" << x << ", " << y << ")";
			return os;
		}

		inline bool operator ==(const Vec2& a) { return a.x == x && a.y == y; }
		inline bool operator !=(const Vec2& a) { return a.x == x && a.y == y; }

		/// --- RAYLIB VECTOR --- ///
		Vec2(Vector2& v) : x(v.x), y(v.y) { }
		operator Vector2() const { return { x, y }; } // Implicit operator

		/// --- BOX2D VECTOR --- ///
		Vec2(b2Vec2& v) : x(v.x), y(v.y) { }
		Vec2(const b2Vec2& v) : x(v.x), y(v.y) { }
		operator b2Vec2() const { return { x, y }; } // Implicit operator
	};
}