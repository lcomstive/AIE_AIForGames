#pragma once
#include <string>
#include <vector>
#include <raylib.h>
#include <unordered_map>

namespace Framework
{
	class GameObject
	{
		static std::unordered_map<unsigned int, GameObject*> m_IDs;

		static unsigned int GetNextID();

		unsigned int m_ID;
		std::string m_Name;

		float m_Rotation;
		Vector2 m_Size;
		Vector2 m_Position;

		// Heirarchy
		GameObject* m_Parent;
		std::unordered_map<unsigned int, GameObject*> m_Children;

	public:
		GameObject(GameObject* parent = nullptr);
		GameObject(std::string name, GameObject* parent = nullptr);
		~GameObject();

		void Draw();
		void Update();

		virtual void OnDraw() { }
		virtual void OnUpdate() { }

		GameObject* GetParent();
		void SetParent(GameObject* parent = nullptr);

		std::vector<GameObject*> GetChildren();
		void AddChild(GameObject* child);
		void AddChildren(std::vector<GameObject*> children);
		void RemoveChild(GameObject* child);

		Vector2& GetPosition();
		void SetPosition(Vector2 position);

		Vector2& GetSize();
		void SetSize(Vector2 size);

		float& GetRotation();
		void SetRotation(float rotation);

		std::string& GetName();
		void SetName(std::string name);

		operator unsigned int() const { return m_ID; }
		operator std::string() const { return "GameObject[" + std::to_string(m_ID) + "]"; }
	};
}