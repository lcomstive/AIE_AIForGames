#include <Game.hpp>
#include <algorithm>
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
	float deltaTime = GetFrameTime();
	m_Thirst += Game::ThirstPerSecond * deltaTime;
	m_Hunger += Game::HungerPerSecond * deltaTime;

	if (m_Thirst >= 1.0f)
		m_Health -= Game::HealthDecayPerSecond * deltaTime;
	if (m_Hunger >= 1.0f)
		m_Health -= Game::HealthDecayPerSecond * deltaTime;

	m_Health = clamp(m_Health, 0.0f, 100.0f);
	m_Thirst = clamp(m_Thirst, 0.0f, 1.0f);
	m_Hunger = clamp(m_Hunger, 0.0f, 1.0f);

	m_BehaviourTree->Update();
	Framework::AnimatedSprite::OnUpdate();

	if (m_Health <= 0.0f)
		Destroy();
}

void Animal::OnDraw()
{
	Framework::AnimatedSprite::OnDraw();

	Vec2 pos = GetPosition();
	DrawLineEx((Vec2)(pos - Vec2(7.5f,  5.0f)), (Vec2)(pos - Vec2(7.5f - (15 * m_Health / 100.0f),  5.0f)), 2.0f, RED);
	DrawLineEx((Vec2)(pos - Vec2(7.5f,  0.0f)), (Vec2)(pos - Vec2(7.5f - (15 * m_Thirst         ),  0.0f)), 2.0f, BLUE);
	DrawLineEx((Vec2)(pos - Vec2(7.5f, -5.0f)), (Vec2)(pos - Vec2(7.5f - (15 * m_Hunger         ), -5.0f)), 2.0f, GREEN);

#ifndef NDEBUG
	m_BehaviourTree->DebugDraw();
#endif
}

float Animal::GetHealth() { return m_Health; }
float Animal::GetThirst() { return m_Thirst; }
float Animal::GetHunger() { return m_Hunger; }
FoodClass Animal::GetFoodClass() { return m_FoodClass; }

void Animal::SetHealth(float value) { m_Health = value; }
void Animal::SetThirst(float value) { m_Thirst = value; }
void Animal::SetHunger(float value) { m_Hunger = value; }
void Animal::SetFoodClass(FoodClass foodClass) { m_FoodClass = foodClass; }