#include <Framework/BehaviourTrees/Actions/LimitTime.hpp>

using namespace std;
using namespace Framework::BT;

BehaviourResult LimitTime::Execute(GameObject* go)
{
	auto child = GetChild();
	if (!child)
		return BehaviourResult::Failure;

	if (m_TimeLeft < 0)
		m_TimeLeft = m_MaxTime;

	m_TimeLeft -= GetFrameTime();
	if (m_TimeLeft <= 0)
	{
		// Child took too long
		m_TimeLeft = -1.0f;
		return BehaviourResult::Failure;
	}

	auto result = child->Execute(go);
	return result;
}

void LimitTime::SetTime(float time) { m_TimeLeft = m_MaxTime = time; }
