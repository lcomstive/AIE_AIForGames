#include <Framework/PhysicsWorld.hpp>

using namespace Framework;

b2World* PhysicsWorld::m_World;
PhysicsWorldArgs PhysicsWorld::m_Args;

void PhysicsWorld::Init(PhysicsWorldArgs args)
{
	m_Args = args;
	m_World = new b2World(args.Gravity);
}
void PhysicsWorld::Destroy() { delete m_World; }

b2World* PhysicsWorld::GetBox2DWorld() { return m_World; }

void PhysicsWorld::Step()
{
	m_World->Step(
		m_Args.TimeStep,
		m_Args.VelocityIterations,
		m_Args.PositionIterations
	);
}