#pragma once
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

	public:
		static void Init(PhysicsWorldArgs args = { });
		static void Destroy();

		static void Step();

		static b2World* GetBox2DWorld();
	};
}