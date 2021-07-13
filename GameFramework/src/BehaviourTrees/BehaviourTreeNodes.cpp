#include <iostream>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

using namespace std;
using namespace Framework;
using namespace Framework::BT;

void BehaviourNode::ClearContext() { m_Context->clear(); }
void BehaviourNode::ClearContext(string name) { m_Context->erase(name); }
void BehaviourNode::SetContext(string name, void* data)
{
	if (ContextExists(name))
		m_Context->at(name) = data;
	else
		m_Context->emplace(name, data);
}
bool BehaviourNode::ContextExists(string name) { return m_Context->find(name) != m_Context->end(); }

/// --- COMPOSITE --- ///
Composite::~Composite()
{
	for (BehaviourNode* child : m_Children)
		delete child;
	m_Children.clear();
}
vector<BehaviourNode*>& Composite::GetChildren() { return m_Children; }

/// --- SEQUENCE --- ///
BehaviourResult Sequence::Execute(GameObject* go)
{
	auto children = GetChildren();
	if (!go || children.size() == 0)
		return BehaviourResult::Failure;
	BehaviourNode* child = children[PendingChildIndex >= 0 ? PendingChildIndex : 0];

	while (child)
	{
		auto result = child->Execute(go);
		switch (result)
		{
		case BehaviourResult::Failure:
		case BehaviourResult::Pending: return result;
		case BehaviourResult::Success:
			if (PendingChildIndex >= children.size() - 1)
				return result;
			child = children[++PendingChildIndex];
			break;
		}
	}
	return BehaviourResult::Success;
}

/// --- SELECTOR --- ///
BehaviourResult Selector::Execute(GameObject* go)
{
	auto children = GetChildren();
	if (!go || children.size() == 0)
		return BehaviourResult::Failure;
	BehaviourNode* child = children[PendingChildIndex >= 0 ? PendingChildIndex : 0];

	while (child)
	{
		auto result = child->Execute(go);
		switch (result)
		{
		case BehaviourResult::Pending: return result;
		case BehaviourResult::Success: PendingChildIndex = -1; return result;
		case BehaviourResult::Failure:
			if (PendingChildIndex >= children.size() - 1)
				return result;
			child = children[++PendingChildIndex];
			break;
		}
	}

	return BehaviourResult::Failure;
}

/// --- DECORATOR --- ///
BehaviourNode* Decorator::GetChild() { return m_Child ? m_Child.get() : nullptr; }

/// --- INVERSE DECORATOR --- ///
BehaviourResult Inverse::Execute(GameObject* go)
{
	auto child = GetChild();
	if (!go || !child)
		return BehaviourResult::Failure;
	auto result = child->Execute(go);
	switch (result)
	{
	default:
	case BehaviourResult::Success: return BehaviourResult::Failure;
	case BehaviourResult::Failure: return BehaviourResult::Success;
	}
}

/// --- LOG DECORATOR --- ///
BehaviourResult LogDecorator::Execute(GameObject* go)
{
	auto child = GetChild();
	cout << "[Node] " << Message << endl;
	return (go && child) ? child->Execute(go) : BehaviourResult::Failure;
}

/// --- DYNAMIC LOG DECORATOR --- ///
BehaviourResult DynamicLogDecorator::Execute(GameObject* go)
{
	auto child = GetChild();
	if(MessageGenerator)
		cout << "[Node] " << MessageGenerator(go, this) << endl;
	return (go && child) ? child->Execute(go) : BehaviourResult::Failure;
}

/// --- SUCCEEDER --- ///
BehaviourResult Succeeder::Execute(GameObject* go)
{
	auto child = GetChild();
	if (go && child)
		child->Execute(go);
	return BehaviourResult::Success;
}

/// --- REPEAT COUNT --- ///
BehaviourResult RepeatCount::Execute(GameObject* go)
{
	auto child = GetChild();
	BehaviourResult result = BehaviourResult::Failure;
	if (!go || !child)
		return result;

	unsigned int repeats = 0;
	SetContext("RepeatCount", repeats);
	for (unsigned int i = 0; i < Repetitions; i++)
	{
		result = child->Execute(go);
		SetContext("RepeatCount", ++repeats);
	}
	return result;
}

/// --- REPEAT UNTIL FAIL --- ///
BehaviourResult RepeatUntilFail::Execute(GameObject* go)
{
	auto child = GetChild();
	if (!go || !child)
		return BehaviourResult::Failure;
	BehaviourResult result;
	unsigned int repeats = 0;
	
	SetContext("RepeatCount", repeats);
	while ((result = child->Execute(go)) != BehaviourResult::Failure)
		SetContext("RepeatCount", ++repeats);
	return BehaviourResult::Success;
}