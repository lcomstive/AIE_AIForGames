#include <vector>
#include <Framework/Vec2.hpp>
#include <Framework/Pathfinding/AStar.hpp>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class NavigatePath : public Action
	{
		float m_GridSize;
		std::vector<Framework::Pathfinding::AStarCell*> m_Path;

	public:
		float Speed;
		bool UpdatePathEachFrame = false;

		NavigatePath() : Speed(10.0f), m_GridSize(1.0f), m_Path() { }

		BehaviourResult Execute(GameObject* go) override;
		void OnDebugDraw(GameObject* go) override;
	};
}