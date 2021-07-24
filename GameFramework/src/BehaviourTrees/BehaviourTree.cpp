#include <raylib.h>
#include <Framework/BehaviourTrees/BehaviourTree.hpp>

using namespace Framework;
using namespace Framework::BT;

BehaviourTree::BehaviourTree(GameObject* parent) : m_Parent(parent)
{
	m_RootNode.m_Context = new std::unordered_map<std::string, BehaviourNode::ContextData>();
}

BehaviourTree::~BehaviourTree() { Clear(); }

void BehaviourTree::Clear() { delete m_RootNode.m_Context; }
void BehaviourTree::Update() { m_RootNode.Execute(m_Parent); }
void BehaviourTree::DebugDraw() { m_RootNode.OnDebugDraw(m_Parent); }