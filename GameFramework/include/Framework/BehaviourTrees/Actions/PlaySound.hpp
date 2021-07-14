#pragma once
#include <raylib.h>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class PlaySound : public Action
	{
		float m_PlayTime;

	public:
		Sound Sound;
		bool WaitForFinish;

		PlaySound();

		virtual std::string GetName() { return "PlaySound"; }
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}