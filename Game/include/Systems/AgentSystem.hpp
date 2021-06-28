#pragma once
#include <ECS/world.hpp>

class AgentSystem : public ECS::System
{
public:
	float MaxMoveSpeed = 500.0f;

	void Update(float deltaTime) override;
};