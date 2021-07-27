#include <iostream>
#include <Framework/BehaviourTrees/Actions/Move.hpp>

using namespace Framework::BT;

BehaviourResult Move::Execute(GameObject* go)
{
	if (GetValuesFromContext)
	{
		Speed = GetContext("Speed", Speed);
		Direction = GetContext("Direction", Direction);
	}

	go->SetPosition(go->GetPosition() + Direction * Speed * GetFrameTime());
	return BehaviourResult::Success;
}