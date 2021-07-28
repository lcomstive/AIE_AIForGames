#include <Framework/BehaviourTrees/Actions/FindClosestNavigatable.hpp>

using namespace std;
using namespace Framework;
using namespace Framework::BT;
using namespace Framework::Pathfinding;

Grid<SquareGridNode>* FindClosestNavigatable::m_Grid = nullptr;

void FindClosestNavigatable::CopyGrid(SquareGrid* grid)
{
	if (!m_Grid)
	{
		cout << "CREATING NEW GRID" << endl;
		m_Grid = new Grid<SquareGridNode>(grid->GetWidth(), grid->GetHeight());
	}
	SetContext("AStarGrid", m_Grid);

	if (!m_AStar)
	{
		m_AStar = new AStar();
		SetContext("AStar", m_AStar);
	}

	for (unsigned int x = 0; x < grid->GetWidth(); x++)
	{
		for (unsigned int y = 1; y < grid->GetHeight(); y++)
		{
			auto* newCell = m_Grid->GetCell(x, y);
			auto* oldCell =   grid->GetCell(x, y);
			newCell->Traversable = oldCell->Traversable;
			newCell->Cost = oldCell->Cost;
		}
	}

	m_Grid->RefreshNodes();
}

void FindClosestNavigatable::ExecuteFinding(Vec2 position, vector<GameObject*> queryList, float cellSize)
{
	// Reset
	m_FindThreadStarted.store(true);
	m_FindThreadFinished.store(false);
	m_FoundClosest = nullptr;
	m_FoundPath = vector<AStarCell*>();

	float smallestFScore = 99999;
	float closestDistance = 99999;

	Vec2 startPos = position / cellSize;
	for (unsigned int i = 0; i < queryList.size(); i++)
	{
		Vec2 endPos = queryList[i]->GetPosition();
		float distance = endPos.Distance(position);
		if (distance >= closestDistance || distance >= Sight)
			continue;
		endPos /= cellSize;

		if (startPos.x == endPos.x && startPos.y == endPos.y)
		{
			// Already at target
#if TRY_MULTITHREADING
			lock_guard<mutex> lock(m_FindThreadMutex);
#endif
			m_FindThreadFinished.store(true);
			m_FindThreadResult = BehaviourResult::Success;
			return;
		}

		m_AStar->StartSearch(
			m_Grid->GetCell((unsigned int)startPos.x, (unsigned int)startPos.y),
			m_Grid->GetCell((unsigned int)  endPos.x, (unsigned int)  endPos.y)
		);

		{
#if TRY_MULTITHREADING
			lock_guard<mutex> lock(m_FindThreadMutex);
#endif
			m_AStar->Finish();
		}

		if (!m_AStar->IsPathValid() || m_AStar->GetSmallestFScore() > smallestFScore)
			continue;

		m_FoundClosest = queryList[i];
		closestDistance = distance;
		m_FoundPath = m_AStar->GetPath();
		smallestFScore = m_AStar->GetSmallestFScore();
	}

	m_FindThreadStarted.store(false);
	m_FindThreadFinished.store(true);
#if TRY_MULTITHREADING
	lock_guard<mutex> lock(m_FindThreadMutex);
#endif

	m_FindThreadResult = m_FoundClosest ? BehaviourResult::Success : BehaviourResult::Failure;
}

BehaviourResult FindClosestNavigatable::Execute(GameObject* go)
{
	if (!m_Grid || !m_AStar)
	{
		m_AStar = GetContext<AStar*>("AStar", nullptr);
		m_Grid = GetContext<SquareGrid*>("AStarGrid", nullptr);

		if (!m_AStar || !m_Grid) // CopyGrid was never called
			return BehaviourResult::Failure;
	}

	if (!m_FindThreadStarted.load() && !m_FindThreadFinished.load())
	{
		if (GetTargetFromContext)
		{
			Sight = GetContext<float>("Sight", 10000.0f);
			TargetTags = GetContext("TargetTags", vector<string>());
			if (ContextExists("TargetTag"))
				TargetTags.emplace_back(GetContext<string>("TargetTarget"));
		}

		vector<GameObject*> queryList = TargetTags.empty() ? GameObject::GetAll() : vector<GameObject*>();

		for (string& tag : TargetTags)
		{
			vector<GameObject*> tagged = GameObject::GetTag(tag);
			queryList.insert(queryList.end(), tagged.begin(), tagged.end());
		}

		if (queryList.size() == 0 || !m_Grid)
			return BehaviourResult::Failure;

#ifndef TRY_MULTITHREADING
		ExecuteFinding(go->GetPosition(), queryList, GetContext<float>("CellSize", 1.0f));
#else
		Vec2 startPos = go->GetPosition();
		float cellSize = GetContext<float>("CellSize", 1.0f);
		m_FindThread = thread([=]() { ExecuteFinding(startPos, queryList, cellSize); });
#endif
	}

	// Check if still processing
	if (!m_FindThreadFinished.load())
		return BehaviourResult::Pending;

	// Reset
#ifdef TRY_MULTITHREADING
	lock_guard<mutex> guard(m_FindThreadMutex);
	m_FindThread.join();
#endif
	
	m_FindThreadStarted.store(false);
	m_FindThreadFinished.store(false);

	if (m_FindThreadResult == BehaviourResult::Success)
	{
		SetContext("Path", m_FoundPath);
		SetContext("Target", m_FoundClosest->GetID());
		SetContext("Found", m_FoundClosest->GetID());
	}

	return m_FindThreadResult;
}