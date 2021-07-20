#include <Framework/PhysicsWorld.hpp>

#ifndef NDEBUG
#include <raylib.h>
#endif

using namespace Framework;

b2World* PhysicsWorld::m_World;
PhysicsWorldArgs PhysicsWorld::m_Args;
PhysicsWorld::PhysicsDebug* PhysicsWorld::m_Debug;

void PhysicsWorld::Init(PhysicsWorldArgs args)
{
	m_Args = args;
	m_World = new b2World(args.Gravity);
	m_Debug = new PhysicsDebug();

	m_Debug->AppendFlags(b2Draw::e_shapeBit);
	m_World->SetDebugDraw(m_Debug);
}
void PhysicsWorld::Destroy()
{
	delete m_World;
	delete m_Debug;
}

b2World* PhysicsWorld::GetBox2DWorld() { return m_World; }

void PhysicsWorld::Step()
{
	m_World->Step(
		m_Args.TimeStep,
		m_Args.VelocityIterations,
		m_Args.PositionIterations
	);
}

Color B2ToRaylibColor(const b2Color& c)
{
	return Color
	{
		(unsigned char)(c.r * 255),
		(unsigned char)(c.g * 255),
		(unsigned char)(c.b * 255),
		(unsigned char)(c.a * 255)
	};
}

// Debug Draw //
void PhysicsWorld::PhysicsDebug::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
#ifndef NDEBUG
	Color c = B2ToRaylibColor(color);
	for (int32 i = 0; i < vertexCount; i++)
	{
		const b2Vec2& nextVert = i < (vertexCount - 1) ? vertices[i + 1] : vertices[0];
		DrawLine((int)-vertices[i].x, (int)vertices[i].y, (int)-nextVert.x, (int)nextVert.y, c);
	}
#endif
}

void PhysicsWorld::PhysicsDebug::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
#ifndef NDEBUG
	Color c = B2ToRaylibColor(color);
	for (int32 i = 0; i < vertexCount; i++)
	{
		const b2Vec2& nextVert = i < (vertexCount - 1) ? vertices[i + 1] : vertices[0];
		DrawLine((int)-vertices[i].x, (int)vertices[i].y, (int)-nextVert.x, (int)nextVert.y, c);
	}
#endif
}

void PhysicsWorld::PhysicsDebug::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
#ifndef NDEBUG
	DrawCircleLines((int)-center.x, (int)center.y, radius, B2ToRaylibColor(color));
#endif
}

void PhysicsWorld::PhysicsDebug::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
#ifndef NDEBUG
	::DrawCircle((int)-center.x, (int)center.y, radius, B2ToRaylibColor(color));
#endif
}

void PhysicsWorld::PhysicsDebug::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
#ifndef NDEBUG
	DrawLine((int)-p1.x, (int)p1.y, (int)-p2.x, (int)p2.y, B2ToRaylibColor(color));
#endif
}

void PhysicsWorld::PhysicsDebug::DrawTransform(const b2Transform& transform)
{
#ifndef NDEBUG
	DrawLine((int)-transform.p.x, (int)transform.p.y, (int)-(transform.p.x + 5.0f), (int)transform.p.y, RED);
	DrawLine((int)-transform.p.x, (int)transform.p.y, (int)-transform.p.x, (int)(transform.p.y + 5.0f), BLUE);
#endif
}

void PhysicsWorld::PhysicsDebug::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
#ifndef NDEBUG
	::DrawCircle((int)p.x, (int)p.y, size, B2ToRaylibColor(color));
#endif
}