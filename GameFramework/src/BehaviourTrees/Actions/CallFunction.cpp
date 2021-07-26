#include <Framework/BehaviourTrees/Actions/CallFunction.hpp>

using namespace std;
using namespace Framework::BT;

BehaviourResult CallFunction::Execute(GameObject* go)
{
	return Function && Function(go, this) ? BehaviourResult::Success : BehaviourResult::Failure;
}