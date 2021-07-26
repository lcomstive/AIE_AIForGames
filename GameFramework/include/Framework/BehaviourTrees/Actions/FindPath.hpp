#pragma once
#include <memory>
#include <Framework/Pathfinding/AStar.hpp>
#include <Framework/Pathfinding/PathFindingGrid.hpp>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

using SquareGrid = Framework::Pathfinding::Grid<Framework::Pathfinding::SquareGridNode>;

namespace Framework::BT
{
	class FindPath : public Action
	{
		bool m_Started = false;

		SquareGrid* m_Grid;
		Framework::Pathfinding::AStar m_AStar;

	public:
		unsigned int StepsPerUpdate = 10;

		void CopyGrid(SquareGrid* grid);

		virtual std::string GetName() { return "FindPath"; }
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}