#include <raylib.h>
#include <Framework/BehaviourTrees/BehaviourTree.hpp>

using namespace Framework;
using namespace Framework::BT;

BehaviourTree::BehaviourTree()
{
	m_RootNode.m_Context = new std::unordered_map<std::string, void*>();
}

BehaviourTree::~BehaviourTree() { Clear(); }

void BehaviourTree::Clear()
{
	/*
	for (auto& node : m_Nodes)
	{
		delete node->m_Context;
		node.release();
	}
	m_Nodes.clear();
	*/

	delete m_RootNode.m_Context;
}

void BehaviourTree::Update(GameObject* gameObject)
{
	/*
	for (auto& node : m_Nodes)
	{
		node->Execute(gameObject);
	}
	*/

	m_RootNode.Execute(gameObject);
}

void BehaviourTree::DrawNode(BehaviourNode* node, unsigned int x, unsigned int y, float zoom)
{
	if (!node)
		return;

	const float FontSize = 16.0f;

	int textLength = MeasureText(node->GetName().c_str(), (int)(FontSize * zoom)) + 20;
	DrawRectangle(x, y, textLength * zoom, (int)(FontSize * zoom * 1.6f), RAYWHITE);
	DrawText(node->GetName().c_str(), x + 10, y + 5, (int)(FontSize * zoom), BLACK);

	// Draw Evaluator
	Evaluator* evaluator = dynamic_cast<Evaluator*>(node);
	if (evaluator)
	{
		DrawNode(
			evaluator->m_True.get(),
			(unsigned int)(x - textLength * zoom * 2.0f),
			(unsigned int)(y + textLength * zoom * 2.0f),
			zoom
		);
		DrawNode(
			evaluator->m_False.get(),
			(unsigned int)(x + textLength * zoom * 2.0f),
			(unsigned int)(y + textLength * zoom * 2.0f),
			zoom
		);
	}

	// Draw Composite
	Composite* composite = dynamic_cast<Composite*>(node);
	if (composite)
	{
		auto children = composite->GetChildren();
		for (int i = 0; i < children.size(); i++)
			DrawNode(
				children[i],
				(x + textLength) * zoom * i,
				y + textLength * zoom * 2.0f,
				zoom
			);
	}

	// Draw Decorator
	Decorator* decorator = dynamic_cast<Decorator*>(node);
	if (decorator)
		DrawNode(
			decorator->GetChild(),
			x,
			y + textLength * zoom * 2.0f,
			zoom
		);
}

void BehaviourTree::Draw(unsigned int nodeIndex, unsigned int x, unsigned int y, float zoom)
{
	/*
	if (nodeIndex >= m_Nodes.size())
		nodeIndex = m_Nodes.size() - 1;
	DrawNode(m_Nodes[nodeIndex].get(), x, y, zoom);
	*/
 	DrawNode(&m_RootNode, x, y, zoom);
}