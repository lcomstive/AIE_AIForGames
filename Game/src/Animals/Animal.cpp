#include <Game.hpp>
#include <Animals/Animal.hpp>

using namespace std;
using namespace Framework;

Animal::Animal(GameObject* parent) : AnimatedSprite(parent) { Init(); }
Animal::Animal(Texture texture, GameObject* parent) : AnimatedSprite(texture, parent) { Init(); }
Animal::Animal(string texturePath, GameObject* parent) : AnimatedSprite(texturePath, parent) { Init(); }

void Animal::Init()
{
	m_BehaviourTree = make_unique<BehaviourTree>(this);

	m_Health = 100.0f;
	m_Hunger = m_Thirst = 0.0f;
}

void Animal::OnUpdate()
{
	Framework::AnimatedSprite::OnUpdate();
	m_BehaviourTree->Update();

	m_Thirst += Game::ThirstPerSecond * GetFrameTime();
	m_Hunger += Game::HungerPerSecond * GetFrameTime();

	if (m_Thirst >= 1.0f)
		m_Health--;
	if (m_Hunger >= 1.0f)
		m_Health--;

	m_Health = min(max(m_Health, 0.0f), 100.0f);
	m_Thirst = min(max(m_Thirst, 0.0f), 1.0f);
	m_Hunger = min(max(m_Hunger, 0.0f), 1.0f);

	if(m_Health <= 0.0f)
		// TODO: Destroy();
}

void Animal::OnDraw()
{
	Framework::AnimatedSprite::OnDraw();
	m_BehaviourTree->DebugDraw();

	Vec2 pos = GetPosition();
	pos.x = -pos.x;
	DrawLineEx((Vec2)(pos + Vec2{ 7.5f,  5 }), (Vec2)(pos + Vec2{ -7.5f * m_Health / 100.0f,  5 }), 2.0f, RED);
	DrawLineEx((Vec2)(pos + Vec2{ 7.5f,  0 }), (Vec2)(pos + Vec2{ -7.5f * m_Thirst,  0 }), 2.0f, BLUE);
	DrawLineEx((Vec2)(pos + Vec2{ 7.5f, -5 }), (Vec2)(pos + Vec2{ -7.5f * m_Hunger, -5 }), 2.0f, GREEN);
}