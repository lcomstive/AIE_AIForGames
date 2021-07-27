#pragma once
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <Framework/Pathfinding/PathFindingGrid.hpp>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

using SquareGrid = Framework::Pathfinding::Grid<Framework::Pathfinding::SquareGridNode>;

namespace Framework::BT
{
	class FindClosestNavigatable : public Action
	{
		// Pathfinding
		SquareGrid* m_Grid;
		Framework::Pathfinding::AStar* m_AStar;

		// Find in background thread
		std::thread m_FindThread;
		GameObject* m_FoundClosest;
		std::mutex m_FindThreadMutex;
		BehaviourResult m_FindThreadResult;
		std::vector<Pathfinding::AStarCell*> m_FoundPath;
		std::atomic_bool m_FindThreadFinished, m_FindThreadStarted;

		void ExecuteFinding(Vec2 startPos, const std::vector<GameObject*> queryList, float cellSize);

	public:
		float Sight; // Radius around GameObject
		bool GetTargetFromContext;
		std::vector<std::string> TargetTags; // Tags to search for

		FindClosestNavigatable() :
			TargetTags(),
			m_FoundPath(),
			Sight(1000.0f),
			m_FindThread(),
			m_Grid(nullptr),
			m_AStar(nullptr),
			m_FindThreadMutex(),
			m_FoundClosest(nullptr),
			m_FindThreadStarted(false),
			m_FindThreadFinished(false),
			GetTargetFromContext(false),
			m_FindThreadResult(BehaviourResult::Failure)
		{ }

		void CopyGrid(SquareGrid* grid);

		virtual std::string GetName() override { return "FindClosestNavigatable"; }
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}