#include <Framework/BehaviourTrees/BehaviourTree.hpp>

using namespace Framework;
using namespace Framework::BT;

void BehaviourTree::Clear()
{
	for (auto& node : m_Nodes)
	{
		delete node->m_Context;
		node.release();
	}
	m_Nodes.clear();
}

void BehaviourTree::Update(GameObject* gameObject)
{
	for(auto& node : m_Nodes)
		node->Execute(gameObject);
}
