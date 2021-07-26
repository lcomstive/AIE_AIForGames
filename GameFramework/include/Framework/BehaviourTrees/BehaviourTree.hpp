#pragma once
#include <vector>
#include <memory>
#include <cassert>
#include <unordered_map>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework
{
	class BehaviourTree
	{
		BT::Selector m_RootNode;
		GameObject* m_Parent;

	public:
		BehaviourTree(GameObject* parent);
		~BehaviourTree();

		void Clear();
		void Update();
		void DebugDraw();

		template<typename T>
		T* Add() { return m_RootNode.AddChild<T>(); }

		BT::Selector* Root() { return &m_RootNode; }
	};
}