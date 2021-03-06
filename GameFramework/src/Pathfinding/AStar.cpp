#include <math.h>
#include <cassert>
#include <algorithm>
#include <Framework/Pathfinding/AStar.hpp>

using namespace std;
using namespace Framework;
using namespace Framework::Pathfinding;

#define FINISH_MAX_ITERATIONS 1000

AStar::AStar(float heuristicModifier, std::function<float(AStarCell* cell, AStarCell* end)> heuristic)
	: m_Start(nullptr), m_End(nullptr)
{
	m_HeuristicModifier = heuristicModifier;
	if (heuristic)
		m_HeuristicFunc = heuristic;
}

// For sorting
bool Framework::Pathfinding::CompareAStarCells(AStarCell* a, AStarCell* b) { return a->FScore < b->FScore; }

float AStar::ManhattanHeuristic(AStarCell* cell, AStarCell* end) { return abs(cell->x - end->x) + abs(end->y - end->y); }
float AStar::EuclideanHeuristic(AStarCell* cell, AStarCell* end) { return sqrt(pow(cell->x - end->x, 2.0f) + pow(cell->y - end->y, 2.0f)); }

bool Find(const vector<AStarCell*>& set, AStarCell* value)
{
	for (size_t i = 0; i < set.size(); i++)
		if (set[i] == value)
			return true;
	return false;
}

void AStar::StartSearch(AStarCell* start, AStarCell* end)
{
	m_Finished = true;
	assert(start != nullptr);
	assert(end != nullptr);

	if (start == end)
		return; // No need to calculate path
	m_Finished = false;

	m_End = end;
	m_Start = start;

	m_Start->GScore = 0;
	// m_Start->HScore = Heuristic(m_Start);
	m_Start->FScore = m_Start->HScore;
	m_Start->Previous = nullptr;
	m_End->Previous = nullptr;

	m_OpenList = { m_Start };
	m_ClosedList.clear();
	m_CurrentPath.clear();
}

void AStar::Finish()
{
	int iterations = 0;
	while (!IsFinished())
	{
		Step();
		iterations++;

		if (iterations >= FINISH_MAX_ITERATIONS)
		{
			m_Finished = true;
			return; // Path could not be found
		}
	}
}

void AStar::Step()
{
	if (m_Finished)
		return;

	if (m_OpenList.size() == 0)
	{
		m_Finished = true;
		return;
	}
	if(m_OpenList.size() > 1)
		sort(m_OpenList.begin(), m_OpenList.end(), CompareAStarCells);

	auto current = m_OpenList[0];

	if (current == m_End)
	{
		m_Finished = true;
		m_CurrentPath.clear();
		while (current)
		{
			m_CurrentPath.insert(m_CurrentPath.begin(), current);
			current = current->Previous;
		}
		return;
	}

	m_OpenList.erase(m_OpenList.begin()); // Erase current from open list
	m_ClosedList.emplace_back(current);

	for (auto& connection : current->Neighbours)
	{
		if (!connection || !connection->Traversable)
			continue; // Invalid target
		if (Find(m_ClosedList, connection))
			continue; // Already checked target for pathing
		float gscore = current->GScore + connection->Cost;
		float hscore = 0;
		
		if (m_HeuristicFunc)
			hscore = m_HeuristicFunc(connection, m_End);
		else
			hscore = ManhattanHeuristic(connection, m_End);
		hscore *= m_HeuristicModifier;

		float fscore = gscore + hscore;

		// If neighbour has already been processed, check if fscore is less than last-processing's fscore
		if (Find(m_ClosedList, connection) && fscore >= connection->FScore)
			continue;

		connection->GScore = gscore;
		connection->HScore = hscore;
		connection->FScore = fscore;
		connection->Previous = current;

		if (fscore > m_LargestFScore)
			m_LargestFScore = fscore;
		if (fscore < m_SmallestFScore)
			m_SmallestFScore = fscore;

		// Haven't visited target yet, add to open list for processing
		if (!Find(m_OpenList, connection))
			m_OpenList.emplace_back(connection);
	}

	m_CurrentPath = { m_End };
}

bool AStar::IsFinished() { return m_Finished; }
float AStar::GetLargestFScore() { return m_LargestFScore; }
float AStar::GetSmallestFScore() { return m_SmallestFScore; }
vector<AStarCell*> AStar::GetPath() { return m_CurrentPath; }
bool AStar::IsPathValid() { return m_CurrentPath.size() > 1; }