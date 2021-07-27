#pragma once
#include <string>
#include <iostream>

#pragma warning(push, 0) // Disable warnings
#include <raylib.h>
#include <box2d/b2_math.h>
#pragma warning(pop) // Restore warnings

namespace Framework
{
	struct Vec2
	{
		float x, y;

		Vec2(float x = 0, float y = 0);

		float MagnitudeSqr();
		float Magnitude();

		void Normalize();
		Vec2 Normalized();

		float Distance(Vec2& other);
		static float Distance(Vec2& a, Vec2& b);

		float DistanceSqr(Vec2& other);
		static float DistanceSqr(Vec2& a, Vec2& b);

		Vec2 Perpendicular();
		float Dot(Vec2& other);
		float Angle(Vec2& other);

		void Reflect(Vec2& normal);
		Vec2 Reflected(Vec2& normal);

		Vec2 Rotate(float angle);
		Vec2 RotateAround(Vec2& origin, float angle);

		/// --- OPERATORS --- ///
		Vec2 operator +(Vec2& other);
		Vec2 operator -(Vec2& other);
		Vec2 operator *(Vec2& other);
		Vec2 operator /(Vec2& other);
		Vec2 operator *(int v);
		Vec2 operator /(int v);
		Vec2 operator *(float v);
		Vec2 operator /(float v);
		Vec2 operator *(double v);
		Vec2 operator /(double v);
		Vec2 operator *(unsigned int v);
		Vec2 operator /(unsigned int v);

		Vec2 operator +=(Vec2& other);
		Vec2 operator -=(Vec2& other);
		Vec2 operator *=(Vec2& other);
		Vec2 operator /=(Vec2& other);

		Vec2 operator *=(int& v);
		Vec2 operator /=(int& v);
		Vec2 operator *=(float& v);
		Vec2 operator /=(float& v);
		Vec2 operator *=(double & v);
		Vec2 operator /=(double & v);
		Vec2 operator *=(unsigned int& v);
		Vec2 operator /=(unsigned int& v);

		// Copy
		Vec2& operator=(const Vec2& other);

		inline bool operator ==(const Vec2& a);
		inline bool operator !=(const Vec2& a);

		/// --- RAYLIB VECTOR --- ///
		Vec2(const Vector2& v) : x(v.x), y(v.y) { }
		operator Vector2() const { return { x, y }; } // Implicit operator

		/// --- BOX2D VECTOR --- ///
		Vec2(b2Vec2& v) : x(v.x), y(v.y) { }
		Vec2(const b2Vec2& v) : x(v.x), y(v.y) { }
		operator b2Vec2() const { return { x, y }; } // Implicit operator

		/// --- STRING --- ///
		operator std::string() const { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }
	};

	// Operator to string (e.g. for std::cout)
	std::ostream& operator <<(std::ostream& os, const Vec2& v);
}