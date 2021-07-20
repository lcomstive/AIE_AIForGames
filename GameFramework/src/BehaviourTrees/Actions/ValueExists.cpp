#include <Framework/BehaviourTrees/Actions/ValueExists.hpp>

using namespace Framework::BT;

BehaviourResult ValueExists::Execute(GameObject* go)
{
	return !m_Name.empty() && ContextExists(m_Name) ? BehaviourResult::Success : BehaviourResult::Failure;
}