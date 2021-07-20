#pragma once
#include <string>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class ValueExists : public Action
	{

	public:
		std::string m_Name;

		virtual std::string GetName() override { return "ValueExists"; }
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}