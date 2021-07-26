#include <Framework/BehaviourTrees/Actions/FindPath.hpp>

using namespace std;
using namespace Framework;
using namespace Framework::BT;
using namespace Framework::Pathfinding;

void FindPath::CopyGrid(SquareGrid* grid)
{
	if (m_Grid)
		delete m_Grid;

	m_Grid = new Grid<SquareGridNode>(grid->GetWidth(), grid->GetHeight());
	for (unsigned int x = 0; x < grid->GetWidth(); x++)
		for (unsigned int y = 0; y < grid->GetHeight(); y++)
			m_Grid->GetCell(x, y)->Traversable = grid->GetCell(x, y)->Traversable;

	m_Grid->RefreshNodes();
}

BehaviourResult FindPath::Execute(GameObject* go)
{
	if (!m_Started)
	{
		float cellSize = GetContext("CellSize", 1.0f);
		unsigned int targetID = GetContext<unsigned int>("Target", -1);
		GameObject* target = GameObject::FromID(targetID);

		Vec2 startPos = go->GetPosition() / cellSize;
		startPos.x = floorf(startPos.x);
		startPos.y = floorf(startPos.y);

		Vec2 endPos = target->GetPosition() / cellSize;
		endPos.x = floorf(endPos.x);
		endPos.y = floorf(endPos.y);

		cout << "{" << go->GetID() << "}" << go->GetPosition() << startPos << " is pathfinding to {" << targetID << "}" << endPos << target->GetPosition() << endl;
		if (!target)
			return BehaviourResult::Failure;
		auto start = m_Grid->GetCell((unsigned int)startPos.x, (unsigned int)startPos.y);
		auto end = m_Grid->GetCell((unsigned int)endPos.x, (unsigned int)endPos.y);
		m_AStar.StartSearch(start, end);

		m_Started = true;
		cout << "RECALCULATING A*" << endl;
		return BehaviourResult::Pending;
	}

	if (!m_AStar.IsFinished())
	{
		for (unsigned int i = 0; i < StepsPerUpdate; i++)
			m_AStar.Step();

		SetContext("Path", m_AStar.GetPath());

		cout << "STEPPED A*" << endl;
		return BehaviourResult::Pending;
	}

	m_Started = false; // Finished
	return BehaviourResult::Success;
}