#include <raylib.h>
#include <iostream>
#include <Framework/BehaviourTrees/Actions/PlaySound.hpp>

using namespace Framework::BT;

PlaySound::PlaySound() : WaitForFinish(true), Sound({}), m_PlayTime(99999) { }

BehaviourResult PlaySound::Execute(GameObject* go)
{
	if (Sound.sampleCount == 0 || !Sound.stream.buffer)
		return BehaviourResult::Failure;

	if (!IsSoundPlaying(Sound) && m_PlayTime == 99999)
	{
		::PlaySound(Sound); // Raylib function
		m_PlayTime = (float)Sound.sampleCount / (Sound.stream.sampleRate * Sound.stream.channels);
		return BehaviourResult::Pending;
	}

	if (!WaitForFinish)
		return BehaviourResult::Success;

	m_PlayTime -= GetFrameTime();
	return m_PlayTime > 0 ? BehaviourResult::Pending : BehaviourResult::Success;
}