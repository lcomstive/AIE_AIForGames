#pragma once
#include <set>
#include <vector>
#include <functional>

namespace Framework::Pathfinding
{
	struct AStarCell
	{
		float x = 0, y = 0;
		float Cost = 1.0f;
		bool Traversable = true;
		float GScore = 0, HScore = 0, FScore = 0;

		AStarCell* Previous = nullptr;
		std::vector<AStarCell*> Neighbours;

		AStarCell(float x = 0, float y = 0) : x(x), y(y) { }
	};

	bool CompareAStarCells(AStarCell* a, AStarCell* b);

	class AStar
	{
		AStarCell* m_End;
		AStarCell* m_Start;

		bool m_Finished = false;
		float m_HeuristicModifier = 1.0f;
		std::vector<AStarCell*> m_CurrentPath;
		float m_LargestFScore = 0.0f, m_SmallestFScore = 0.0f;

		std::vector<AStarCell*> m_OpenList;
		std::vector<AStarCell*> m_ClosedList;

		std::function<float(AStarCell* cell, AStarCell* end)> m_HeuristicFunc;

	public:
		AStar(float heuristicModifier = 1.0f, std::function<float(AStarCell* cell, AStarCell* end)> heuristic = nullptr);

		static float ManhattanHeuristic(AStarCell* cell, AStarCell* end);
		static float EuclideanHeuristic(AStarCell* cell, AStarCell* end);

		void StartSearch(AStarCell* start, AStarCell* end);

		void Step();
		void Finish();
		bool IsFinished();
		bool IsPathValid();
		float GetLargestFScore();
		float GetSmallestFScore();
		std::vector<AStarCell*> GetPath();
	};
}