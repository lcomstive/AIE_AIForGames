#include <raylib.h>
#include <Framework/BehaviourTrees/Actions/SetValue.hpp>

#ifndef NDEBUG
#include <iostream>
#endif

using namespace Framework::BT;

void SetValue::Set(std::string name, void* value)
{
	m_Name = name;
	m_Value = value;
}

std::string& SetValue::GetValueName() { return m_Name; }

BehaviourResult SetValue::Execute(GameObject* go)
{
	SetContext(m_Name, m_Value);
#ifndef NDEBUG
	std::cout << "Action SetValue: " << m_Name << "' = " << m_Value << std::endl;
#endif
	return BehaviourResult::Success;
}