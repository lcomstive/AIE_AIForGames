#pragma once
#include <string>
#include <vector>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class FindClosest : public Action
	{
	public:
		std::string TargetTag = "";
		bool GetTargetFromContext = false;

		virtual std::string GetName() override { return "FindClosest"; }
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}