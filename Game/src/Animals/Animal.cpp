#include <Animals/Animal.hpp>

using namespace std;
using namespace Framework;

Animal::Animal(GameObject* parent) : AnimatedSprite(parent)
{
	m_BehaviourTree = make_unique<BehaviourTree>(this);
}

Animal::Animal(Texture texture, GameObject* parent) : AnimatedSprite(texture, parent)
{
	m_BehaviourTree = make_unique<BehaviourTree>(this);
}

Animal::Animal(string texturePath, GameObject* parent) : AnimatedSprite(texturePath, parent)
{
	m_BehaviourTree = make_unique<BehaviourTree>(this);
}

void Animal::OnUpdate()
{
	Framework::AnimatedSprite::OnUpdate();
	m_BehaviourTree->Update();
}

void Animal::OnDraw()
{
	Framework::AnimatedSprite::OnDraw();
	m_BehaviourTree->DebugDraw();
}