#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class FindFirst : public Action
	{
	public:
		std::string Tag;
		bool GetTagFromContext;

		FindFirst() : Tag(), GetTagFromContext(false) { }

		BehaviourResult Execute(GameObject* go) override;
	};
}