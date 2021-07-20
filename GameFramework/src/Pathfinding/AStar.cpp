#include <math.h>
#include <cassert>
#include <algorithm>
#include <Framework/Pathfinding/AStar.hpp>

using namespace std;
using namespace Framework;
using namespace Framework::Pathfinding;

AStar::AStar(float heuristicModifier, std::function<float(AStarCell* cell, AStarCell* end)> heuristic)
{
	m_HeuristicModifier = heuristicModifier;
	if (heuristic)
		m_HeuristicFunc = heuristic;
}

// For sorting
bool Framework::Pathfinding::CompareAStarCells(AStarCell* a, AStarCell* b) { return a->FScore < b->FScore; }

float AStar::ManhattanHeuristic(AStarCell* cell, AStarCell* end) { return abs(cell->x - end->x) + abs(end->y - end->y); }
float AStar::EuclideanHeuristic(AStarCell* cell, AStarCell* end) { return sqrt(pow(cell->x - end->x, 2) + pow(cell->y - end->y, 2)); }

bool Find(vector<AStarCell*> set, AStarCell* value)
{
	return find_if(set.begin(), set.end(), [=](AStarCell* cell)
		{
			return value == cell;
		}) != set.end();
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

void AStar::Step()
{
	if (m_Finished)
		return;

	if (m_OpenList.size() == 0)
	{
		m_Finished = true;
		return;
	}
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
		if (!connection.Target)
			continue; // Invalid target
		if (Find(m_ClosedList, connection.Target))
			continue; // Already checked target for pathing
		float gscore = current->GScore + max(connection.Cost, 0.0f);
		float hscore = 0;
		
		if (m_HeuristicFunc)
			hscore = m_HeuristicFunc(connection.Target, m_End);
		else
			hscore = ManhattanHeuristic(connection.Target, m_End);
		hscore *= m_HeuristicModifier;

		float fscore = gscore + hscore;

		if (Find(m_ClosedList, connection.Target) && fscore >= connection.Target->FScore)
			continue;

		connection.Target->GScore = gscore;
		connection.Target->HScore = hscore;
		connection.Target->FScore = fscore;
		connection.Target->Previous = current;

		if (fscore > m_LargestFScore)
			m_LargestFScore = fscore;

		// Haven't visited target yet, add to open list for processing
		if (!Find(m_OpenList, connection.Target))
			m_OpenList.emplace_back(connection.Target);
	}

	m_CurrentPath = { m_End };
}

bool AStar::IsFinished() { return m_Finished; }
float AStar::GetLargestFScore() { return m_LargestFScore; }
vector<AStarCell*> AStar::GetPath() { return m_CurrentPath; }