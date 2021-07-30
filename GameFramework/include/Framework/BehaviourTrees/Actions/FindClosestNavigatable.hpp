#pragma once
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <Framework/Pathfinding/PathFindingGrid.hpp>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

using SquareGrid = Framework::Pathfinding::Grid<Framework::Pathfinding::SquareGridNode>;

// #define TRY_MULTITHREADING

namespace Framework::BT
{
	class FindClosestNavigatable : public Action
	{
		// Pathfinding
		static SquareGrid* m_Grid;
		Framework::Pathfinding::AStar* m_AStar;
		std::vector<Pathfinding::AStarCell*> m_FoundPath;

		// Find in background thread
#ifdef TRY_MULTITHREADING
		std::thread m_FindThread;
		std::mutex m_FindThreadMutex;
#endif
		GameObject* m_FoundClosest;
		BehaviourResult m_FindThreadResult;
		std::atomic_bool m_FindThreadFinished, m_FindThreadStarted;

		void ExecuteFinding(Vec2 startPos, std::vector<GameObject*> queryList, float cellSize);

	public:
		float Sight; // Radius around GameObject
		bool GetTargetFromContext;
		std::vector<std::string> TargetTags; // Tags to search for

		FindClosestNavigatable() :
			TargetTags(),
			m_FoundPath(),
			Sight(1000.0f),
			m_AStar(nullptr),
			m_FoundClosest(nullptr),
			GetTargetFromContext(false),
			m_FindThreadResult(BehaviourResult::Failure)

#ifdef TRY_MULTITHREADING
			, m_FindThread(),
			m_FindThreadMutex(),
			m_FindThreadStarted(false),
			m_FindThreadFinished(false)
#endif
		{ }

		void CopyGrid(SquareGrid* grid);

		virtual std::string GetName() override { return "FindClosestNavigatable"; }
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}