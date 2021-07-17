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
		BT::Sequence m_RootNode;

		void DrawNode(BT::BehaviourNode* node, unsigned int x, unsigned int y, float zoom);

	public:
		BehaviourTree();
		~BehaviourTree();

		void Clear();
		void Update(GameObject* gameObject);

		template<typename T>
		T* Add() { return m_RootNode.AddChild<T>(); }

		void Draw(unsigned int nodeIndex, unsigned int x, unsigned int y, float zoom = 1.0f);
	};
}