#include <Framework/GameObjects/Sprite.hpp>

using namespace std;
using namespace Framework;

Sprite::Sprite(GameObject* parent) : GameObject(parent), m_Texture({}), View({ }) { }
Sprite::Sprite(Texture texture, GameObject* parent) : GameObject(parent) { SetTexture(texture); }
Sprite::Sprite(string texturePath, GameObject* parent) : GameObject(parent) { SetTexture(texturePath); }

void Sprite::DrawSprite(Rectangle view)
{
	if (!m_Texture.width || !m_Texture.height)
		return; // Texture invalid

	Vector2& size = GetSize();
	Vector2& position = GetPosition();
	DrawTexturePro(
		m_Texture,
		view, // Source Rect
		// Dest Rect
		{	
			position.x,
			position.y,
			size.x,
			size.y
		},
		// Origin
		{ size.x / 2.0f, size.y / 2.0f },
		GetRotation(),
		RAYWHITE
	);
}

void Sprite::OnDraw() { DrawSprite(View); }

void Sprite::SetTexture(const Texture& texture)
{
	m_Texture = texture;
	View = { 0, 0, (float)m_Texture.width, (float)m_Texture.height };
}
void Sprite::SetTexture(std::string& path) { SetTexture(LoadTexture(path.c_str())); }

Texture& Sprite::GetTexture() { return m_Texture; }

void Sprite::SetView(
		float xOffset,
		float yOffset,
		float xSize,
		float ySize
	)
{ View = { xOffset, yOffset, xSize, ySize }; }

void Sprite::SetView(Vector2& offset, Vector2& size) { View = { offset.x, offset.y, size.x, size.y }; }