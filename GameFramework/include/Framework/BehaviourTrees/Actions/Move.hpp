#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class Move : public Action
	{
	public:
		float Speed;
		Vec2 Direction;
		bool GetValuesFromContext;

		Move() : Speed(10.0f), Direction(0, 0), GetValuesFromContext(false) { }

		BehaviourResult Execute(GameObject* go) override;
	};
}