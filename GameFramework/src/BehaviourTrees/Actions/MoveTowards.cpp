#include <Framework/BehaviourTrees/Actions/MoveTowards.hpp>

using namespace Framework::BT;

BehaviourResult MoveTowards::Execute(GameObject* go)
{
	if (GetValuesFromContext)
	{
		TargetID = GetContext<unsigned int>("Target", -1);
		Speed = GetContext("Speed", Speed <= 0 ? 100.0f : Speed);
	}

	if (TargetID == (unsigned int)-1)
		return BehaviourResult::Failure;

	GameObject* target = GameObject::FromID(TargetID);
	Vec2 position = go->GetPosition();
	Vec2 targetVel = (target->GetPosition() - position).Normalized();

	/*
	float rotation = atan2(targetVel.x, targetVel.y);
	go->SetRotation(rotation * 50.0f);
	*/

	go->SetPosition(position + go->GetForward() * Speed * GetFrameTime());
	return BehaviourResult::Success;
}