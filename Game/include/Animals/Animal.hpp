#pragma once
#include <Framework/GameObjects/AnimatedSprite.hpp>
#include <Framework/BehaviourTrees/BehaviourTree.hpp>

class Animal : public Framework::AnimatedSprite
{
	std::unique_ptr<Framework::BehaviourTree> m_BehaviourTree;
	friend class Game;

protected:
	Framework::BehaviourTree* GetBehaviourTree() { return m_BehaviourTree.get(); }

public:
	Animal(GameObject* parent = nullptr);
	Animal(Texture texture, GameObject* parent = nullptr);
	Animal(std::string texturePath, GameObject* parent = nullptr);

	virtual void OnDraw() override;
	virtual void OnUpdate() override;
};