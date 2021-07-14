#pragma once
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class Wait : public Action
	{
		float m_TimeLeft;
		float m_WaitTime;

	public:
		Wait();

		float& GetTimeLeft();
		float& GetWaitTime();
		void SetTime(float time);

		virtual std::string GetName() { return "Wait"; }
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}