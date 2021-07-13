#pragma once
#include <Framework/GameObjects/Sprite.hpp>

namespace Framework
{
	class AnimatedSprite : public Sprite
	{
		float m_TimeUntilNextFrame;
		float m_TimeBetweenFrames = 0.25f;

	public:
		unsigned int Frame;
		unsigned int MaxFrames;

		AnimatedSprite(GameObject* parent = nullptr);
		AnimatedSprite(Texture texture, GameObject* parent = nullptr);
		AnimatedSprite(std::string texturePath, GameObject* parent = nullptr);

		virtual void OnUpdate() override;
		virtual void OnDraw() override;

		float& GetTimeBetweenFrames();
		void SetTimeBetweenFrames(float time);
	};
}