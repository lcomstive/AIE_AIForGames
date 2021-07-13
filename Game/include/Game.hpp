#pragma once

#include <Framework/GameObjects/AnimatedSprite.hpp>
#include <Framework/BehaviourTrees/BehaviourTree.hpp>

class Game
{
	Camera2D m_Camera;
	Framework::GameObject* m_Root;

	Framework::BehaviourTree m_BehaviourTree;

public:
	Game();
	~Game();

	void Run();
	void Update();
};