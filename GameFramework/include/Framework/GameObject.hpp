#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <box2d/box2d.h>
#include <Framework/Vec2.hpp>

namespace Framework
{
	class GameObject
	{
		static std::unordered_map<unsigned int, GameObject*> m_IDs;
		static std::unordered_map<std::string, std::vector<GameObject*>> m_GlobalTags;

		static unsigned int GetNextID();

		unsigned int m_ID;
		std::string m_Name;
		bool m_ShouldDelete = false;

		float m_Rotation;
		Vec2 m_Size;
		Vec2 m_Position;
		bool m_DirtyTransform;
		std::vector<std::string> m_Tags;

		// Physics
		b2Body* m_PhysicsBody;

		// Heirarchy
		GameObject* m_Parent;
		std::unordered_map<unsigned int, GameObject*> m_Children;

	public:
		GameObject(GameObject* parent = nullptr);
		GameObject(std::string name, GameObject* parent = nullptr);
		~GameObject();

		void Draw();
		void Update();
		void Destroy();
		void PrePhysicsUpdate();
		void PostPhysicsUpdate();

		void GeneratePhysicsBody(
			bool dynamic = true, // Whether the physics body will move
			float density = 1.0f,
			float friction = 0.3f
		);

		/// --- VIRTUALS --- ///

		// Called when a drawing a frame to screen
		virtual void OnDraw() { }

		// Called once per frame
		virtual void OnUpdate() { }

		// Called before the physics calculations are done
		virtual void OnPrePhysicsUpdate() { }

		// Called after the physics calculations are done
		virtual void OnPostPhysicsUpdate() { }

		/// --- GETTERS & SETTERS --- ///

		// Gets parent
		GameObject* GetParent();

		// Sets parent
		void SetParent(GameObject* parent = nullptr);

		unsigned int GetID();

		std::vector<GameObject*> GetChildren();
		void AddChild(GameObject* child);
		void AddChildren(std::vector<GameObject*> children);
		void RemoveChild(GameObject* child);
		GameObject* FindChild(unsigned int id);

		Vec2& GetPosition();
		void SetPosition(Vec2 position);

		Vec2& GetSize();
		void SetSize(Vec2 size);

		float& GetRotation();
		void SetRotation(float rotation);

		std::string& GetName();
		void SetName(std::string name);

		Vec2 GetForward();
		b2Body* GetPhysicsBody();

		void AddTag(std::string tag);
		void RemoveTag(std::string tag);
		bool HasTag(std::string tag);

		static GameObject* FromID(unsigned int id);

		static std::vector<GameObject*> GetAll();
		static std::vector<GameObject*> GetTag(std::string tag);

		operator unsigned int() const { return m_ID; }
		operator std::string() const { return "GameObject[" + std::to_string(m_ID) + "]"; }
	};
}