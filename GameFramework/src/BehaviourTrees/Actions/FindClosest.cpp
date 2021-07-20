#include <Framework/BehaviourTrees/Actions/FindClosest.hpp>

using namespace std;
using namespace Framework::BT;

BehaviourResult FindClosest::Execute(GameObject* go)
{
	if (GetTargetFromContext)
		TargetTag = GetContext<string>("TargetTag");

	vector<GameObject*> queryList = TargetTag.empty() ? GameObject::GetAll() : GameObject::GetTag(TargetTag);

	if (queryList.size() == 0)
		return BehaviourResult::Failure;

	Vec2 position = go->GetPosition();
	GameObject* closest = queryList[0];
	float closestDistance = closest->GetPosition().Distance(position);

	for (unsigned int i = 1; i < queryList.size(); i++)
	{
		float distance = queryList[i]->GetPosition().Distance(position);
		if (distance >= closestDistance)
			continue;
		closest = queryList[i];
		closestDistance = distance;
	}

	SetContext("Found", closest->GetID());
	return BehaviourResult::Success;
}