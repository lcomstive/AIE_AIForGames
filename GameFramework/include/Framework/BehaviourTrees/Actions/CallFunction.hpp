#pragma once
#include <functional>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class CallFunction : public Action
	{
	public:
		std::function<bool(GameObject*, CallFunction*)> Function;

		virtual std::string GetName() override { return "CallFunction"; }
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}