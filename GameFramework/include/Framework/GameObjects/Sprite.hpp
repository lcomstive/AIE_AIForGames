#pragma once
#include <string>
#include <Framework/GameObject.hpp>

namespace Framework
{
	class Sprite : public GameObject
	{
		Texture m_Texture;

	protected:
		void DrawSprite(Rectangle view);

	public:
		Rectangle View;

		Sprite(GameObject * parent = nullptr);
		Sprite(Texture texture, GameObject * parent = nullptr);
		Sprite(std::string texturePath, GameObject * parent = nullptr);

		virtual void OnDraw() override;
		
		void SetTexture(std::string& path);
		void SetTexture(const Texture& texture);

		void SetView(
			float xOffset,
			float yOffset,
			float xSize,
			float ySize
		);
		void SetView(Vec2& offset, Vec2& size);

		Vec2 GetViewSize();
		Rectangle GetView();
		Vec2 GetViewOffset();

		Texture& GetTexture();
	};
}