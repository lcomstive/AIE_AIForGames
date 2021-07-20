#include <Framework/PhysicsWorld.hpp>
#include <Framework/BehaviourTrees/Actions/CanSeeTarget.hpp>

#ifndef NDEBUG
#include <raylib.h>
#endif

using namespace std;
using namespace Framework;
using namespace Framework::BT;

float CanSeeTarget::CanSeeTargetRaycastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
{
	CanSee = false;
	Finished = true;
	if (TargetID == (unsigned int)-1)
		return 0; // No ID set, terminate

	GameObject* go = GameObject::FromID((unsigned int)fixture->GetUserData().pointer);
	if (go->GetID() != TargetID)
		return 0; // Raycast didn't hit our target, terminate

	// Hit target
	CanSee = true;
	return fraction;
}

bool InFieldOfView(GameObject* a, GameObject* b, float fov)
{
	Vec2 forward = a->GetForward();
	Vec2 targetOffset = (b->GetPosition() - a->GetPosition()).Normalized();

	// test angle of forward and offset against FieldOfView
	return acos(forward.Dot(targetOffset)) < (fov / 2.0f);
}

BehaviourResult CanSeeTarget::Execute(GameObject* go)
{
	if (GetTargetFromContext)
	{
		SightRange = GetContext<float>("Sight", 100.0f);
		FieldOfView = GetContext<float>("FieldOfView", 60.0f);
		TargetID = GetContext<unsigned int>("Found", (unsigned int)-1);
	}

	auto pBody = go->GetPhysicsBody();
	if (!pBody || TargetID == (unsigned int)-1 || go->GetID() == TargetID)
		return BehaviourResult::Failure;

	if (!m_Callback.Started)
	{
		GameObject* target = GameObject::FromID(TargetID);
		float fovRads = FieldOfView * (PI / 180.0f);

		// Perform raycast
		Vec2 start = go->GetPosition();
		Vec2 end = target->GetPosition();

#ifndef NDEBUG
		// Draw viewcone
		Vec2 endFOV = go->GetForward() * SightRange;
		endFOV.Rotate(fovRads / 2.0f);
		DrawLine((int)-start.x, (int)start.y, (int)-(start.x + endFOV.x), (int)(start.y + endFOV.y), BLUE);
		DrawLine((int)-start.x, (int)start.y, (int)-(start.x - endFOV.x), (int)(start.y + endFOV.y), BLUE);
		DrawLine((int)-(start.x - endFOV.x), (int)(start.y + endFOV.y), (int)-(start.x + endFOV.x), (int)(start.y + endFOV.y), BLUE);
#endif

		if (!InFieldOfView(go, target, fovRads) ||
			start.Distance(end) > SightRange)
			return BehaviourResult::Failure;

		m_Callback.Started = true;
		m_Callback.CanSee = false;
		m_Callback.Finished = false;
		m_Callback.TargetID = TargetID;
		PhysicsWorld::GetBox2DWorld()->RayCast(&m_Callback, start, end);

#ifndef NDEBUG
		DrawLine((int)-start.x, (int)start.y, (int)-end.x, (int)end.y, RED);
#endif
	}

	if (!m_Callback.Finished)
		return BehaviourResult::Pending; // Currently raycasting

	m_Callback.Started = false;
	m_Callback.Finished = false;
	return m_Callback.CanSee ? BehaviourResult::Success : BehaviourResult::Failure;
}