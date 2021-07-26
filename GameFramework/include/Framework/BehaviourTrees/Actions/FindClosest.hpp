#pragma once
#include <string>
#include <vector>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class FindClosest : public Action
	{
	public:
		float Sight; // Radius around GameObject
		std::string TargetTag = "";
		bool GetTargetFromContext = false;

		virtual std::string GetName() override { return "FindClosest"; }
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}