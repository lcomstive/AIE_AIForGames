#pragma once
#include <string>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class CanSee: public Action
	{
	public:
		float FieldOfView;

		virtual std::string GetName() override { return "CanSee"; }
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}