#pragma once
#include <raylib.h>
#include <ECS/entity.hpp>

enum class AgentState
{
	Wandering,
	Pursuing
};

struct AgentComponent
{
	Vector2 Velocity = { 0, 0 };
};