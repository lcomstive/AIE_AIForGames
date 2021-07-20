#include <Framework/BehaviourTrees/Actions/FindFirst.hpp>

using namespace std;
using namespace Framework::BT;

BehaviourResult FindFirst::Execute(GameObject* go)
{
	if (GetTagFromContext)
		Tag = GetContext<string>("TargetTag");

	if (Tag.empty())
		return BehaviourResult::Failure;

	auto gameObjects = GameObject::GetTag(Tag);
	if (gameObjects.empty())
		return BehaviourResult::Failure;

	SetContext("Found", gameObjects[0]->GetID());
	SetContext("Target", gameObjects[0]->GetID());
	return BehaviourResult::Success;
}