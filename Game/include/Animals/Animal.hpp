#pragma once
#include <Framework/GameObjects/AnimatedSprite.hpp>
#include <Framework/BehaviourTrees/BehaviourTree.hpp>

class Animal : public Framework::Sprite
{
	std::unique_ptr<Framework::BehaviourTree> m_BehaviourTree;
	friend class Game;

	float m_Health, m_Thirst, m_Hunger;

	void Init();
protected:
	Framework::BehaviourTree* GetBehaviourTree() { return m_BehaviourTree.get(); }

public:
	Animal(GameObject* parent = nullptr);
	Animal(Texture texture, GameObject* parent = nullptr);
	Animal(std::string texturePath, GameObject* parent = nullptr);

	virtual void OnDraw() override;
	virtual void OnUpdate() override;
};