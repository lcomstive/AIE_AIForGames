#pragma once
#include <string>
#include <vector>
#include <Framework/Pathfinding/PathFindingGrid.hpp>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

using SquareGrid = Framework::Pathfinding::Grid<Framework::Pathfinding::SquareGridNode>;

namespace Framework::BT
{
	class FindClosestNavigatable : public Action
	{
		SquareGrid* m_Grid;
		Framework::Pathfinding::AStar m_AStar;

	public:
		float Sight; // Radius around GameObject
		std::vector<std::string> TargetTags; // Tags to search for
		bool GetTargetFromContext = false;

		void CopyGrid(SquareGrid* grid);

		virtual std::string GetName() override { return "FindClosestNavigatable"; }
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}