#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class MoveTowards : public Action
	{
	public:
		float Speed;
		unsigned int TargetID;
		bool GetValuesFromContext;

		MoveTowards() : TargetID(-1), Speed(10.0f), GetValuesFromContext(true) { }

		BehaviourResult Execute(GameObject* go) override;
	};
}