#include <Framework/GameObject.hpp>

using namespace std;
using namespace Framework;

unordered_map<unsigned int, GameObject*> GameObject::m_IDs;

unsigned int GameObject::GetNextID()
{
	unsigned int id = 0;
	while (m_IDs.find(++id) != m_IDs.end());
	return id;
}

GameObject::GameObject(GameObject* parent) : GameObject("GameObject", parent) { }
GameObject::GameObject(string name, GameObject* parent)
	: m_Name(name), m_Rotation(0), m_Position({ 0, 0 }), m_Size({ 1, 1 })
{
	SetParent(parent);
	m_ID = GetNextID();
	m_IDs.emplace(m_ID, this);
}

GameObject::~GameObject()
{
	m_IDs.erase(m_ID);

	for (auto& pair : m_Children)
		delete pair.second;
}

void GameObject::Update()
{
	OnUpdate();
	for (auto& pair : m_Children)
		pair.second->Update();
}

void GameObject::Draw()
{
	OnDraw();
	for (auto& pair : m_Children)
		pair.second->Draw();
}

/// --- GETTERS & SETTERS --- ///
GameObject* GameObject::GetParent() { return m_Parent; }
void GameObject::SetParent(GameObject* parent)
{
	if (!parent)
		return;
	m_Parent = parent;
	m_Parent->AddChild(this);
}

vector<GameObject*> GameObject::GetChildren()
{
	vector<GameObject*> children;
	for (auto& pair : m_Children)
		children.emplace_back(pair.second);
	return children;
}

void GameObject::AddChild(GameObject* child)
{
	if (!child)
		return;
	child->m_Parent = this;
	m_Children.emplace(child->m_ID, child);
}

void GameObject::AddChildren(vector<GameObject*> children)
{
	for (GameObject* child : children)
		AddChild(child);
}

void GameObject::RemoveChild(GameObject* child)
{
	if (!child)
		return;
	if (m_Children.find(child->m_ID) != m_Children.end())
		m_Children.erase(child->m_ID);
}

Vector2& GameObject::GetPosition() { return m_Position; }
void GameObject::SetPosition(Vector2 position) { m_Position = position; }

Vector2& GameObject::GetSize() { return m_Size; }
void GameObject::SetSize(Vector2 size) { m_Size = size; }

float& GameObject::GetRotation() { return m_Rotation; }
void GameObject::SetRotation(float rotation) { m_Rotation = rotation; }

std::string& GameObject::GetName() { return m_Name; }
void GameObject::SetName(std::string name) { m_Name = name; }