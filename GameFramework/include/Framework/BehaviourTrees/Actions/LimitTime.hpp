#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class LimitTime : public Decorator
	{
		float m_TimeLeft;
		float m_MaxTime;

	public:
		LimitTime() : m_TimeLeft(-1) { SetTime(10.0f); }

		BehaviourResult Execute(GameObject* go) override;

		// Sets the maximum time the child can execute for, in seconds
		void SetTime(float time);
	};
}