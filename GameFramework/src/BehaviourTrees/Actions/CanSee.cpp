#include <Framework/PhysicsWorld.hpp>
#include <Framework/BehaviourTrees/Actions/CanSee.hpp>

#ifndef NDEBUG
#include <raylib.h>
#endif

using namespace std;
using namespace Framework;
using namespace Framework::BT;

float CanSee::CanSeeRaycastCallback::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
{
	Found = nullptr;
	Finished = true;
	if (Tag.empty())
		return 0; // Terminate

	GameObject* go = GameObject::FromID((unsigned int)fixture->GetUserData().pointer);
	if (!go->HasTag(Tag))
		return 0; // Raycast didn't hit our target, terminate

	// Hit target
	Found = go;
	return fraction;
}

bool CanSee::InFieldOfView(GameObject* a, GameObject* b, float fov)
{
	Vec2 forward = a->GetForward();
	Vec2 targetOffset = (b->GetPosition() - a->GetPosition()).Normalized();

	// test angle of forward and offset against FieldOfView
	return acos(forward.Dot(targetOffset)) < (fov / 2.0f);
}

BehaviourResult CanSee::Execute(GameObject* go)
{
	if (GetValuesFromContext)
	{
		SightRange = GetContext<float>("CanSee_Sight", SightRange);
		FieldOfView = GetContext<float>("CanSee_FOV", FieldOfView);
		TargetTag = GetContext<string>("TargetTag", TargetTag);
	}

	auto pBody = go->GetPhysicsBody();
	if(!pBody || TargetTag.empty())
		return BehaviourResult::Failure;

	if (!m_Callback.Started)
	{
		Vec2 currentPos = go->GetPosition();
		float fovRads = FieldOfView * (PI / 180.0f);

#ifndef NDEBUG
		Vec2 leftFOV = go->GetForward() * SightRange;
		leftFOV.Rotate(-fovRads / 2.0f);
		Vec2 rightFOV = go->GetForward() * SightRange;
		rightFOV.Rotate(fovRads / 2.0f);
		DrawLine((int)-currentPos.x, (int)currentPos.y, (int)-(currentPos.x + leftFOV.x), (int)(currentPos.y + leftFOV.y), BLUE);
		DrawLine((int)-currentPos.x, (int)currentPos.y, (int)-(currentPos.x + rightFOV.x), (int)(currentPos.y + rightFOV.y), BLUE);
		DrawLine((int)-(currentPos.x + leftFOV.x), (int)(currentPos.y + leftFOV.y), (int)-(currentPos.x + rightFOV.x), (int)(currentPos.y + rightFOV.y), BLUE);
#endif

		// Get closest object
		float closestDistance = 0.0f;
		GameObject* closestGO = nullptr;

		vector<GameObject*> queryObjects = GameObject::GetTag(TargetTag);
		for (int i = (int)queryObjects.size() - 1; i >= 0; i--)
		{
			if (queryObjects[i]->GetID() == go->GetID())
				continue;
			float distance = currentPos.Distance(queryObjects[i]->GetPosition());
			if (distance > SightRange)
				continue; // Out of sight range

			// Test if object is in front, within field of view
			if (!InFieldOfView(go, queryObjects[i], fovRads))
				continue;

			if (!closestGO || closestDistance > distance)
			{
				closestGO = queryObjects[i];
				closestDistance = distance;
				continue;
			}
		}

		if (!closestGO)
			return BehaviourResult::Failure;

		// Perform raycast
		Vec2 start = go->GetPosition();
		Vec2 end = closestGO->GetPosition();

		m_Callback.Found = nullptr;
		m_Callback.Tag = TargetTag;
		m_Callback.Started = true;
		m_Callback.Finished = false;
		PhysicsWorld::GetBox2DWorld()->RayCast(&m_Callback, start, end);

#ifndef NDEBUG
		DrawLine((int)-start.x, (int)start.y, (int)-end.x, (int)end.y, RED);
#endif
	}

	if (!m_Callback.Finished)
		return BehaviourResult::Pending; // Currently raycasting

#ifndef NDEBUG
	if (m_Callback.Found)
	{
		Vec2 start = go->GetPosition();
		Vec2 end = m_Callback.Found->GetPosition();
		DrawLine((int)-start.x, (int)start.y, (int)-end.x, (int)end.y, GREEN);
	}
#endif

	m_Callback.Started = false;
	m_Callback.Finished = false;
	SetContext("Target", m_Callback.Found ? m_Callback.Found->GetID() : (unsigned int)-1);
	SetContext("Found", m_Callback.Found ? m_Callback.Found->GetID() : (unsigned int)-1);
	return m_Callback.Found == nullptr ? BehaviourResult::Failure : BehaviourResult::Success;
}