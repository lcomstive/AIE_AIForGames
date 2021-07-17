#include <Framework/BehaviourTrees/Actions/CanSee.hpp>

using namespace Framework::BT;

BehaviourResult CanSee::Execute(GameObject* go)
{
	return BehaviourResult::Failure;
}