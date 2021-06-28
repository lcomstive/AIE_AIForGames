#include <Systems/AgentSystem.hpp>
#include <Components/AgentComponent.hpp>
#include <Components/TargetComponent.hpp>
#include <ECS/components/transform-component.hpp>
#include <GameEngine/Components/PhysicsBodyComponent.hpp>
#include <Physics/PhysicsObject.hpp>

using namespace ECS;
using namespace Utilities;

pair<unsigned int, TransformComponent*> GetClosestTarget(vector<pair<Entity, TransformComponent*>>& targets, Vector2 agent)
{
	if (targets.size() == 0)
		return { {}, nullptr };

	float closestDistance = Vector2Distance(targets[0].second->Position, agent);
	unsigned int closestDistanceIndex = 0;
	for (size_t i = 0; i < targets.size(); i++)
	{
		float distance = Vector2Distance(targets[i].second->Position, agent);
		if (distance >= closestDistance)
			continue;

		closestDistance = distance;
		closestDistanceIndex = i;
	}
	return { closestDistanceIndex, targets[closestDistanceIndex].second };
}

void AgentSystem::Update(float deltaTime)
{
	auto pairs = GetWorld()->GetComponents<AgentComponent, TransformComponent>();
	auto targetEntities = GetWorld()->GetEntities<TargetComponent, TransformComponent>();

	vector<pair<Entity, TransformComponent*>> targets;
	for (auto& targetEntity : targetEntities)
		targets.push_back(make_pair(targetEntity, targetEntity.GetComponent<TransformComponent>()));

	for(auto& pair : pairs)
	{
		auto agent = pair.second.first;
		auto physicsBody = GetWorld()->GetComponent<PhysicsBodyComponent>(pair.first);
		if (!physicsBody)
			continue;
		TransformComponent* transform = pair.second.second;

		if (targets.size() > 0)
		{
			// Seeks
			auto [targetIndex, targetTransform] = GetClosestTarget(targets, transform->Position);
			targets.erase(targets.begin() + targetIndex);

			Vector2 desiredVelocity = Vector2Normalize(Vector2Subtract(targetTransform->Position, transform->Position));
			desiredVelocity *= MaxMoveSpeed;

			Vector2 steeringForce = Vector2Subtract(desiredVelocity, agent->Velocity);

			agent->Velocity = Vector2Add(agent->Velocity, steeringForce);
			transform->Position += agent->Velocity * deltaTime;
		}
	}
}