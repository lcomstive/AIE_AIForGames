#pragma once
#include <vector>
#include <GameEngine/Application.hpp>

class Game : public Application
{
	std::vector<ECS::Entity> m_Birbs;
	std::vector<ECS::Entity> m_Targets;

public:
	Game(const ApplicationArgs& args = { });

protected:
	void OnDrawGUI() override;
	void OnUpdate(float deltaTime) override;

private:
	void PreloadAssets();

	void CreateTarget(Vector2 position);
	void CreateBirbs(unsigned int count = 2);
	void CreateRandomTargets(unsigned int count = 5);
};