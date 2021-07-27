#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class WithinDistance : public Action
	{

	public:
		float MaxDistance;
		unsigned int TargetID;
		bool GetTargetFromContext;

		WithinDistance() :
			MaxDistance(10),
			TargetID((unsigned int)-1),
			GetTargetFromContext(true)
		{ }

		BehaviourResult Execute(GameObject* go) override;
	};
}