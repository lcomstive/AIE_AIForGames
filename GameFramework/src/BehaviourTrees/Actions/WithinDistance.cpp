#include <Framework/BehaviourTrees/Actions/WithinDistance.hpp>

using namespace Framework::BT;

BehaviourResult WithinDistance::Execute(GameObject* go)
{
	if (GetTargetFromContext)
		TargetID = GetContext("Target", TargetID > 0 ? TargetID : (unsigned int)-1);

	if (TargetID == (unsigned int)-1)
		return BehaviourResult::Failure;

	GameObject* target = GameObject::FromID(TargetID);
	return go->GetPosition().Distance(target->GetPosition()) < MaxDistance ?
		BehaviourResult::Success : BehaviourResult::Failure;
}