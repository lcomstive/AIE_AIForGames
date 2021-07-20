#include <Framework/GameObject.hpp>
#include <Framework/PhysicsWorld.hpp>

using namespace std;
using namespace Framework;

unordered_map<unsigned int, GameObject*> GameObject::m_IDs;
unordered_map<string, vector<GameObject*>> GameObject::m_GlobalTags;

unsigned int GameObject::GetNextID()
{
	unsigned int id = 0;
	while (m_IDs.find(++id) != m_IDs.end());
	return id;
}

GameObject::GameObject(GameObject* parent) : GameObject("GameObject", parent) { }
GameObject::GameObject(string name, GameObject* parent) :
	m_Name(name),
	m_Rotation(0),
	m_Size({ 1, 1 }),
	m_Position({ 0, 0 }),
	m_PhysicsBody(nullptr),
	m_DirtyTransform(false)
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

void GameObject::PrePhysicsUpdate()
{
	if (m_PhysicsBody)
	{
		if(m_DirtyTransform)
			m_PhysicsBody->SetTransform(m_Position, -m_Rotation);
		m_DirtyTransform = false;

		OnPrePhysicsUpdate();
	}

	for (auto pair : m_Children)
		pair.second->PrePhysicsUpdate();
}

void GameObject::PostPhysicsUpdate()
{
	if (m_PhysicsBody)
	{
		m_Rotation = -m_PhysicsBody->GetAngle();
		m_Position = m_PhysicsBody->GetPosition();

		OnPostPhysicsUpdate();
	}

	for (auto pair : m_Children)
		pair.second->PostPhysicsUpdate();
}

void GameObject::GeneratePhysicsBody(bool dynamic, float density, float friction)
{
	if (m_PhysicsBody)
		PhysicsWorld::GetBox2DWorld()->DestroyBody(m_PhysicsBody);
	
	// Create body
	b2BodyDef body;
	if(dynamic)
		body.type = b2_dynamicBody;
	body.angle = m_Rotation;
	body.userData.pointer = (uintptr_t)m_ID;
	body.position.Set(m_Position.x, m_Position.y);
	m_PhysicsBody = PhysicsWorld::GetBox2DWorld()->CreateBody(&body);

	// Define shape and attach
	b2PolygonShape box;
	box.SetAsBox(m_Size.x / 2.0f, m_Size.y / 2.0f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = density;
	fixture.friction = friction;
	fixture.userData.pointer = (uintptr_t)m_ID;

	m_PhysicsBody->CreateFixture(&fixture);
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

unsigned int GameObject::GetID() { return m_ID; }

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

GameObject* GameObject::FindChild(unsigned int id)
{
	for (auto& child : m_Children)
		if (child.first == id)
			return child.second;
	return nullptr;
}

Vec2& GameObject::GetPosition() { return m_Position; }
void GameObject::SetPosition(Vec2 position)
{
	m_Position = position;
	m_DirtyTransform = true;

	for (auto& child : m_Children)
		child.second->SetPosition(child.second->m_Position + m_Position);
}

Vec2& GameObject::GetSize() { return m_Size; }
void GameObject::SetSize(Vec2 size) { m_Size = size; }

float& GameObject::GetRotation() { return m_Rotation; }
void GameObject::SetRotation(float rotation)
{
	m_Rotation = rotation;
	m_DirtyTransform = true;

	for (auto& child : m_Children)
		child.second->SetRotation(child.second->m_Rotation + m_Rotation);
}

std::string& GameObject::GetName() { return m_Name; }
void GameObject::SetName(std::string name) { m_Name = name; }

Vec2 GameObject::GetForward()
{
	return Vec2
	{
		sin(m_Rotation),
		cos(m_Rotation)
	};
}

void GameObject::AddTag(std::string tag)
{
	if (m_GlobalTags.find(tag) == m_GlobalTags.end())
		m_GlobalTags.emplace(tag, vector<GameObject*>());
	m_GlobalTags[tag].push_back(this);
	m_Tags.emplace_back(tag);
}

void GameObject::RemoveTag(std::string tag)
{
	if (m_GlobalTags.find(tag) == m_GlobalTags.end())
		return; // Tag doesn't exist

	auto objects = m_GlobalTags[tag];
	for (int i = (int)objects.size() - 1; i >= 0; i--)
	{
		if (objects[i]->m_ID != m_ID)
			continue;
		m_GlobalTags[tag].erase(objects.begin() + i);
		break;
	}

	for (int i = (int)m_Tags.size() - 1; i >= 0; i--)
	{
		if (m_Tags[i].compare(tag) != 0)
			continue;
		m_Tags.erase(m_Tags.begin() + i);
		break;
	}
}

bool GameObject::HasTag(std::string inputTag)
{
	if (m_Tags.size() == 0)
		return false;
	for (auto& tag : m_Tags)
		if (tag.compare(inputTag) == 0)
			return true;
	return false;
}

b2Body* GameObject::GetPhysicsBody() { return m_PhysicsBody; }

GameObject* GameObject::FromID(unsigned int id)
{
	if (m_IDs.find(id) == m_IDs.end())
		return nullptr;
	return m_IDs[id];
}

vector<GameObject*> GameObject::GetAll()
{
	vector<GameObject*> gos;
	for (auto& pair : m_IDs)
		gos.emplace_back(pair.second);
	return gos;
}

vector<GameObject*> GameObject::GetTag(std::string tag)
{
	return m_GlobalTags.find(tag) == m_GlobalTags.end() ? vector<GameObject*>() : m_GlobalTags[tag];
}