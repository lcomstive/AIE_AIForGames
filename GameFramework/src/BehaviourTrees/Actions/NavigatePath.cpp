#include <vector>
#include <Framework/Pathfinding/AStar.hpp>
#include <Framework/BehaviourTrees/Actions/NavigatePath.hpp>

using namespace std;
using namespace Framework::BT;
using namespace Framework::Pathfinding;

BehaviourResult NavigatePath::Execute(GameObject* go)
{
	if (!ContextExists("Path"))
		return BehaviourResult::Failure;

	m_Path = GetContext("Path", vector<AStarCell*>());
	m_GridSize = GetContext("CellSize", 1.0f);
	Speed = GetContext("Speed", Speed <= 0 ? 100.0f : Speed);

	if (m_Path.empty()) // No path present, or finished navigating
		return BehaviourResult::Success;

	Vec2 position = go->GetPosition();
	Vec2 targetPos = Vec2 { m_Path[0]->x, m_Path[0]->y } * m_GridSize + go->GetSize() / 2.0f;
	Vec2 difference = targetPos - position;
	float magnitude = difference.MagnitudeSqr();
	
	// Check for at next point in path
	if (magnitude < 1.0f)
	{
		m_Path.erase(m_Path.begin()); // Remove first element, navigate to next
		SetContext("Path", m_Path);

		if (m_Path.empty())
		{
			ClearContext("Path");
			return BehaviourResult::Success;
		}
		return BehaviourResult::Pending;
	}

	Vec2 direction = difference.Normalized();
	go->SetPosition(go->GetPosition() + direction * Speed * GetFrameTime());

	return BehaviourResult::Pending;
}

void NavigatePath::OnDebugDraw(GameObject* go)
{
	for(unsigned int i = 0; i < m_Path.size(); i++)
	{
		Vec2 pos = { m_Path[i]->x, m_Path[i]->y };
		pos *= m_GridSize;
		DrawCircle((int)pos.x, (int)pos.y, 5.0f, RED);
	}
}