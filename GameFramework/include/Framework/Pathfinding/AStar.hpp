#pragma once
#include <set>
#include <vector>
#include <functional>

namespace Framework::Pathfinding
{
	struct AStarCell;
	struct AStarConnection
	{
		float Cost;
		AStarCell* Target;

		AStarConnection(AStarCell* target = nullptr, float cost = 1.0f) : Cost(cost), Target(target) { }
	};

	struct AStarCell
	{
		float x = 0, y = 0;
		bool Traversable = true;
		float GScore = 0, HScore = 0, FScore = 0;

		AStarCell* Previous = nullptr;
		std::vector<AStarConnection> Neighbours;

		AStarCell(float x = 0, float y = 0) : x(x), y(y) { }
	};

	bool CompareAStarCells(AStarCell* a, AStarCell* b);

	class AStar
	{
		AStarCell* m_End;
		AStarCell* m_Start;

		bool m_Finished = false;
		float m_LargestFScore = 0.0f;
		float m_HeuristicModifier = 1.0f;
		std::vector<AStarCell*> m_CurrentPath;

		std::vector<AStarCell*> m_OpenList;
		std::vector<AStarCell*> m_ClosedList;

		std::function<float(AStarCell* cell, AStarCell* end)> m_HeuristicFunc;

	public:
		AStar(float heuristicModifier = 1.0f, std::function<float(AStarCell* cell, AStarCell* end)> heuristic = nullptr);

		static float ManhattanHeuristic(AStarCell* cell, AStarCell* end);
		static float EuclideanHeuristic(AStarCell* cell, AStarCell* end);

		void StartSearch(AStarCell* start, AStarCell* end);

		void Step();
		bool IsFinished();
		float GetLargestFScore();
		std::vector<AStarCell*> GetPath();
	};
}