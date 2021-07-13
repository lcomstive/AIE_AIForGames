#include <Framework/GameObjects/AnimatedSprite.hpp>
#include <iostream>

using namespace std;
using namespace Framework;

AnimatedSprite::AnimatedSprite(GameObject* parent)
	: Sprite(parent), Frame(0), m_TimeUntilNextFrame(10000.0f) { }
AnimatedSprite::AnimatedSprite(Texture texture, GameObject* parent)
	: Sprite(texture, parent), Frame(0), MaxFrames(1), m_TimeUntilNextFrame(10000.0f) { }
AnimatedSprite::AnimatedSprite(string texturePath, GameObject* parent)
	: Sprite(texturePath, parent), Frame(0), MaxFrames(1), m_TimeUntilNextFrame(10000.0f) { }

void AnimatedSprite::OnUpdate()
{
	float delta = GetFrameTime();
	m_TimeUntilNextFrame -= delta;
	Frame %= MaxFrames;
	if (m_TimeUntilNextFrame > 0.001f)
		return;
	Frame = (Frame + 1) % MaxFrames;
	m_TimeUntilNextFrame = m_TimeBetweenFrames;
}

void AnimatedSprite::OnDraw()
{
	Texture& texture = GetTexture();
	Rectangle view = View;
	view.x += view.width * Frame;
	while (view.x >= texture.width)
	{
		view.x -= texture.width;
		view.y += view.height;
	}

	DrawSprite(view);
}

float& AnimatedSprite::GetTimeBetweenFrames() { return m_TimeBetweenFrames; }

void AnimatedSprite::SetTimeBetweenFrames(float time)
{
	m_TimeBetweenFrames = m_TimeUntilNextFrame = time;
}