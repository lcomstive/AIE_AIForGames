#pragma once
#include <Framework/GameObjects/AnimatedSprite.hpp>
#include <Framework/BehaviourTrees/BehaviourTree.hpp>

enum class FoodClass { Herbivore, Omnivore, Carnivore };

class Animal : public Framework::AnimatedSprite
{
	std::unique_ptr<Framework::BehaviourTree> m_BehaviourTree;
	friend class Game;

	FoodClass m_FoodClass = FoodClass::Herbivore;
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

	float GetHealth();
	float GetThirst();
	float GetHunger();
	FoodClass GetFoodClass();

	void SetHealth(float value);
	void SetThirst(float value);
	void SetHunger(float value);
	void SetFoodClass(FoodClass foodClass);
};