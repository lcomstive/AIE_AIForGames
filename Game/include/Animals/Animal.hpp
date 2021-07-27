#pragma once
#include <string>
#include <vector>
#include <Framework/GameObjects/AnimatedSprite.hpp>
#include <Framework/Pathfinding/PathFindingGrid.hpp>
#include <Framework/BehaviourTrees/BehaviourTree.hpp>
#include <Framework/BehaviourTrees/Actions/FindClosestNavigatable.hpp>

enum class FoodClass { Herbivore, Omnivore, Carnivore };

class Animal : public Framework::AnimatedSprite
{
	friend class Game;

	std::unique_ptr<Framework::BehaviourTree> m_BehaviourTree;
	Framework::Pathfinding::Grid<Framework::Pathfinding::SquareGridNode>* m_Grid;

	FoodClass m_FoodClass = FoodClass::Herbivore;
	float m_Health, m_Thirst, m_Hunger, m_Speed;

	void Init();

	void CreateBehaviourWander();
	void CreateBehaviourCheckFood();
	void CreateBehaviourCheckWater();
	void CreateBehaviourCheckPredator();
	Framework::BT::FindClosestNavigatable* AddFindClosestNavigatable
				(Framework::BT::Sequence* parent, std::vector<std::string> tags);

protected:
	Framework::BehaviourTree* GetBehaviourTree() { return m_BehaviourTree.get(); }

public:
	Animal(GameObject* parent = nullptr);
	Animal(Texture texture, GameObject* parent = nullptr);
	Animal(std::string texturePath, GameObject* parent = nullptr);

	void InitBehaviourTree(Framework::Pathfinding::Grid<Framework::Pathfinding::SquareGridNode>* grid);

	virtual void OnDraw() override;
	virtual void OnUpdate() override;

	float GetSpeed();
	float GetHealth();
	float GetThirst();
	float GetHunger();
	FoodClass GetFoodClass();

	void AddHealth(float value);
	void AddThirst(float value);
	void AddHunger(float value);

	void SetSpeed(float value);
	void SetHealth(float value);
	void SetThirst(float value);
	void SetHunger(float value);
	void SetFoodClass(FoodClass foodClass);
};