#pragma once
#include <string>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class SetValue : public Action
	{
		std::string m_Name;
		void* m_Value;
	public:
		template<typename T>
		T Get() { return (T)m_Data; }

		std::string& GetValueName();

		void Set(std::string name, void* value);

		template<typename T>
		void Set(std::string name, T value) { Set(name, (void*)value); }

		virtual std::string GetName() override { return "SetValue"; }
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}