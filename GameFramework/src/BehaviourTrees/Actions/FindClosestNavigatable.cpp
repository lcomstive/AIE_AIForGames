#include <Framework/BehaviourTrees/Actions/FindClosestNavigatable.hpp>

using namespace std;
using namespace Framework::BT;
using namespace Framework::Pathfinding;

void FindClosestNavigatable::CopyGrid(SquareGrid* grid)
{
	if (m_Grid)
		delete m_Grid;

	m_Grid = new Grid<SquareGridNode>(grid->GetWidth(), grid->GetHeight());
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

BehaviourResult FindClosestNavigatable::Execute(GameObject* go)
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

	Vec2 position = go->GetPosition();
	GameObject* closest = nullptr;
	vector<AStarCell*> closestPath;
	float smallestFScore = 99999;
	float closestDistance = 99999;

	float cellSize = GetContext("CellSize", 1.0f);
	Vec2 startPos = go->GetPosition() / cellSize;
	cout << "StartPos " << startPos << endl;

	for (unsigned int i = 0; i < queryList.size(); i++)
	{
		Vec2 endPos = queryList[i]->GetPosition();
		float distance = endPos.Distance(position);
		if (distance >= closestDistance || distance >= Sight)
			continue;
		endPos /= cellSize;

		if (startPos.x == endPos.x && startPos.y == endPos.y)
		{
			SetContext("Path", vector<AStarCell*>());
			return BehaviourResult::Success; // Already at target
		}

		m_AStar.StartSearch(
			m_Grid->GetCell((unsigned int)startPos.x, (unsigned int)startPos.y),
			m_Grid->GetCell((unsigned int)  endPos.x, (unsigned int)  endPos.y)
		);

		for (unsigned int loop = 0; loop < 500; loop++)
		{
			m_AStar.Step();
			if (m_AStar.IsFinished())
				break;
		}

		if (!m_AStar.IsPathValid() || m_AStar.GetSmallestFScore() > smallestFScore)
			continue;

		closest = queryList[i];
		closestDistance = distance;
		closestPath = m_AStar.GetPath();
		smallestFScore = m_AStar.GetSmallestFScore();
	}

	if (!closest)
	{
		cout << "No valid paths {" << go->GetID() << "}" << endl;
		return BehaviourResult::Failure;
	}
	SetContext("NewPath", true);
	SetContext("Path", closestPath);
	SetContext("Target", closest->GetID());
	SetContext("Found", closest->GetID());
	return BehaviourResult::Success;
}