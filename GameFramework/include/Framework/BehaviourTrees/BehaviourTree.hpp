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
		std::vector<std::unique_ptr<BT::BehaviourNode>> m_Nodes;
	public:
		void Clear();
		void Update(GameObject* gameObject);

		template<typename T>
		T* Add()
		{
			bool isBehaviourType = std::is_base_of<BT::BehaviourNode, T>::value;
			assert(isBehaviourType);

			m_Nodes.emplace_back(std::make_unique<T>());
			T* node = (T*)m_Nodes[m_Nodes.size() - 1].get();
			((BehaviourNode*)node)->m_Context = new std::unordered_map<std::string, void*>();
			return node;
		}
	};
}