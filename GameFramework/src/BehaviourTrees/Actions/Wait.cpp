#include <raylib.h>
#include <Framework/BehaviourTrees/Actions/Wait.hpp>

using namespace Framework::BT;

Wait::Wait() : m_TimeLeft(1.0f), m_WaitTime(1.0f) { }

float& Wait::GetTimeLeft() { return m_TimeLeft; }
float& Wait::GetWaitTime() { return m_WaitTime; }

void Wait::SetTime(float time)
{
	m_TimeLeft = m_WaitTime = time;
}

BehaviourResult Wait::Execute(GameObject* go)
{
	m_TimeLeft -= GetFrameTime();
	return m_TimeLeft > 0 ? BehaviourResult::Pending : BehaviourResult::Success;
}