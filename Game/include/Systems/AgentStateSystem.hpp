#pragma once
#include <ECS/World.hpp>

class AgentStateSystem : ECS::System
{
public:
	void Update(float deltaTime) override;
};