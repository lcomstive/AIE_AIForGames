#pragma once
#include <box2d/b2_draw.h>
#include <box2d/b2_world.h>
#include <Framework/Vec2.hpp>

namespace Framework
{
	struct PhysicsWorldArgs
	{
		Vec2 Gravity = { 0, 0 };
		float TimeStep = 1.0f / 60.0f;
		unsigned int VelocityIterations = 6;
		unsigned int PositionIterations = 2;
	};

	class PhysicsWorld
	{
		static PhysicsWorldArgs m_Args;
		static b2World* m_World;

		class PhysicsDebug : public b2Draw
		{
			void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
			void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
			void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
			void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;
			void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
			void DrawTransform(const b2Transform& transform) override;
			void DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;
		};
		static PhysicsDebug* m_Debug;

	public:
		static void Init(PhysicsWorldArgs args = { });
		static void Destroy();

		static void Step();

		static b2World* GetBox2DWorld();
	};
}