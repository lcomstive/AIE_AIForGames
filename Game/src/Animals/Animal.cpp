#include <Game.hpp>
#include <algorithm>
#include <Animals/Animal.hpp>

using namespace std;
using namespace Framework;

const float HealthDecayPerSecond = 50;

Animal::Animal(GameObject* parent) : Sprite(parent) { Init(); }
Animal::Animal(Texture texture, GameObject* parent) : Sprite(texture, parent) { Init(); }
Animal::Animal(string texturePath, GameObject* parent) : Sprite(texturePath, parent) { Init(); }

void Animal::Init()
{
	m_BehaviourTree = make_unique<BehaviourTree>(this);

	m_Health = 100.0f;
	m_Hunger = m_Thirst = 0.0f;
}

void Animal::OnUpdate()
{
	float deltaTime = GetFrameTime();
	m_Thirst += Game::ThirstPerSecond * deltaTime;
	m_Hunger += Game::HungerPerSecond * deltaTime;

	if (m_Thirst >= 1.0f)
		m_Health -= HealthDecayPerSecond * deltaTime;
	if (m_Hunger >= 1.0f)
		m_Health -= HealthDecayPerSecond * deltaTime;

	m_Health = clamp(m_Health, 0.0f, 100.0f);
	m_Thirst = clamp(m_Thirst, 0.0f, 1.0f);
	m_Hunger = clamp(m_Hunger, 0.0f, 1.0f);

	if(m_Health <= 0.0f)
		Destroy();

	if(m_Health < 100.0f)
		cout << "[" << GetID() << "] Health: " << m_Health << endl;

	m_BehaviourTree->Update();
	Framework::Sprite::OnUpdate();
}

void Animal::OnDraw()
{
	Framework::Sprite::OnDraw();
	m_BehaviourTree->DebugDraw();

	Vec2 pos = GetPosition();
	pos.x = -pos.x;
	DrawLineEx((Vec2)(pos + Vec2{ 7.5f,  5 }), (Vec2)(pos + Vec2{ 7.5f - (15 * m_Health / 100.0f),  5 }), 2.0f, RED);
	DrawLineEx((Vec2)(pos + Vec2{ 7.5f,  0 }), (Vec2)(pos + Vec2{ 7.5f - (15 * m_Thirst),  0 }), 2.0f, BLUE);
	DrawLineEx((Vec2)(pos + Vec2{ 7.5f, -5 }), (Vec2)(pos + Vec2{ 7.5f - (15 * m_Hunger), -5 }), 2.0f, GREEN);
}