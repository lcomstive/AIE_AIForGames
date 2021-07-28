#include <iostream>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

using namespace std;
using namespace Framework;
using namespace Framework::BT;

void BehaviourNode::ClearContext() { m_Context->clear(); }
void BehaviourNode::ClearContext(string name) { m_Context->erase(name); }
bool BehaviourNode::ContextExists(string name) { return m_Context->find(name) != m_Context->end(); }

void BehaviourNode::SetContext(string name, int value) { SetContext(name, (unsigned long long)value); }
void BehaviourNode::SetContext(string name, long value) { SetContext(name, (unsigned long long)value); }
void BehaviourNode::SetContext(string name, short value) { SetContext(name, (unsigned long long)value); }
void BehaviourNode::SetContext(string name, long long value) { SetContext(name, (unsigned long long)value); }
void BehaviourNode::SetContext(string name, unsigned int value) { SetContext(name, (unsigned long long)value); }
void BehaviourNode::SetContext(string name, unsigned long value) { SetContext(name, (unsigned long long)value); }
void BehaviourNode::SetContext(string name, unsigned short value) { SetContext(name, (unsigned long long)value); }
void BehaviourNode::SetContext(string name, unsigned long long value)
{
	if (!ContextExists(name))
		m_Context->emplace(name, ContextData());

	ContextData* data = &m_Context->at(name);
	data->Type = ContextDataType::Integer;
	data->Data = make_any<unsigned long long>(value);
}

void BehaviourNode::SetContext(string name, std::string value)
{
	if (!ContextExists(name))
		m_Context->emplace(name, ContextData());

	ContextData* data = &m_Context->at(name);
	data->Type = ContextDataType::String;
	data->Data = make_any<string>(value);
}

void BehaviourNode::SetContext(string name, float value) { SetContext(name, double(value)); }
void BehaviourNode::SetContext(string name, double value)
{
	if (!ContextExists(name))
		m_Context->emplace(name, ContextData());

	ContextData* data = &m_Context->at(name);
	data->Type = ContextDataType::Decimal;
	data->Data = make_any<double>(value);
}

/// --- EVALUATOR --- ///
BehaviourResult Evaluator::Execute(GameObject* go)
{
	if (!go || !Function)
		return BehaviourResult::Failure;
	if (Function(go, this))
		return m_True ? m_True->Execute(go) : BehaviourResult::Failure;
	else
		return m_False ? m_False->Execute(go) : BehaviourResult::Failure;
}

/// --- CONDITIONAL --- ///
BehaviourResult Conditional::Execute(GameObject* go)
{
	if (!go || !Function)
		return BehaviourResult::Failure;
	return Function(go, this) ? (m_Child ? m_Child->Execute(go) : BehaviourResult::Success) : BehaviourResult::Failure;
}

/// --- COMPOSITE --- ///
Composite::~Composite()
{
	for (BehaviourNode* child : m_Children)
		delete child;
	m_Children.clear();
	m_Children.shrink_to_fit();
}

void Composite::OnDebugDraw(GameObject* go)
{
	auto children = GetChildren();
	if (children.size() == 0)
		return;
	if (PendingChildIndex < 0 || PendingChildIndex >= children.size())
		PendingChildIndex = 0;
	GetChildren()[PendingChildIndex]->OnDebugDraw(go);
}

vector<BehaviourNode*>& Composite::GetChildren() { return m_Children; }

/// --- SEQUENCE --- ///
BehaviourResult Sequence::Execute(GameObject* go)
{
	auto children = GetChildren();
	if (!go || children.size() == 0)
		return BehaviourResult::Failure;
	if (PendingChildIndex < 0 || PendingChildIndex >= children.size())
		PendingChildIndex = 0;
	BehaviourNode* child = children[PendingChildIndex];

	while (child)
	{
		SetContext("SequenceIndex", PendingChildIndex);
		auto result = child->Execute(go);
		switch (result)
		{
		case BehaviourResult::Failure: PendingChildIndex = 0;
		case BehaviourResult::Pending: return result;
		case BehaviourResult::Success:
			if (++PendingChildIndex >= children.size())
			{
				child = nullptr;
				break;
			}
			child = children[PendingChildIndex];
			break;
		}
	}
	PendingChildIndex = 0;
	return BehaviourResult::Success;
}

BehaviourResult RandomSequence::Execute(GameObject* go)
{
	auto children = GetChildren();
	if (!go || children.size() == 0)
		return BehaviourResult::Failure;
	if (PendingChildIndex < 0)
		PendingChildIndex = rand() % children.size();
	BehaviourNode* child = children[PendingChildIndex];

	unsigned int sequenceIndex = 0;
	while (child)
	{
		SetContext("SequenceIndex", sequenceIndex++);
		auto result = child->Execute(go);
		children.erase(children.begin() + PendingChildIndex);
		switch (result)
		{
		case BehaviourResult::Failure: PendingChildIndex = 0;
		case BehaviourResult::Pending: return result;
		case BehaviourResult::Success:
			if (children.size() == 0)
				return result;
			PendingChildIndex = rand() % children.size();
			child = children[PendingChildIndex];
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
	if (PendingChildIndex < 0 || PendingChildIndex >= children.size())
		PendingChildIndex = 0;
	BehaviourNode* child = children[PendingChildIndex];

	while (child)
	{
		SetContext("SelectorIndex", PendingChildIndex);
		auto result = child->Execute(go);
		switch (result)
		{
		case BehaviourResult::Pending: return result;
		case BehaviourResult::Success: PendingChildIndex = -1; return result;
		case BehaviourResult::Failure:
			if (++PendingChildIndex >= children.size())
			{
				child = nullptr;
				break;
			}
			child = children[PendingChildIndex];
			break;
		}
	}

	PendingChildIndex = 0;
	return BehaviourResult::Failure;
}

BehaviourResult RandomSelector::Execute(GameObject* go)
{
	auto children = GetChildren();
	if (!go || children.size() == 0)
		return BehaviourResult::Failure;
	if (PendingChildIndex < 0)
		PendingChildIndex = rand() % children.size();
	BehaviourNode* child = children[PendingChildIndex];
	children.erase(children.begin() + PendingChildIndex);

	unsigned int selectorIndex = 0;
	while (child)
	{
		SetContext("SelectorIndex", selectorIndex++);
		auto result = child->Execute(go);
		children.erase(children.begin() + PendingChildIndex);
		switch (result)
		{
		case BehaviourResult::Pending: return result;
		case BehaviourResult::Success: PendingChildIndex = -1; return result;
		case BehaviourResult::Failure:
			if (children.size() == 0)
				return result;
			PendingChildIndex = rand() % children.size();
			child = children[PendingChildIndex];
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
BehaviourResult Log::Execute(GameObject* go)
{
	cout << "[Node] " << Message << endl;

	auto child = GetChild();
	if (go && child)
		child->Execute(go);
	return BehaviourResult::Success;
}

/// --- DYNAMIC LOG DECORATOR --- ///
BehaviourResult DynamicLog::Execute(GameObject* go)
{
	if(Message)
		cout << "[Node] " << Message(go, this) << endl;

	auto child = GetChild();
	if (go && child)
		child->Execute(go);
	return BehaviourResult::Success;
}

/// --- SUCCEEDER --- ///
BehaviourResult Succeeder::Execute(GameObject* go)
{
	auto child = GetChild();
	auto result = BehaviourResult::Success;
	if (go && child)
		result = child->Execute(go);
	return result == BehaviourResult::Pending ? result : BehaviourResult::Success;
}

/// --- REPEAT --- ///
BehaviourResult Repeat::Execute(GameObject* go)
{
	auto child = GetChild();
	BehaviourResult result = BehaviourResult::Failure;
	if (!go || !child || !Condition)
		return result;

	bool condition = Condition(go, this);
	if(condition)
		result = child->Execute(go);

	if (SingleFrame)
	{
		while (Condition(go, this))
		{
			result = child->Execute(go);
			SetContext("RepeatCount", ++m_Repetitions);
		}
		m_Repetitions = 0;
		return result;
	}
	else
		SetContext("RepeatCount", ++m_Repetitions);

	if (!condition)
	{
		m_Repetitions = 0;
		ClearContext("RepeatCount");
	}
	return condition ? BehaviourResult::Pending : BehaviourResult::Failure;
}

/// --- REPEAT TIME --- ///
void RepeatTime::SetTime(float value) { m_TimeLeft = m_MaxTime = value; }

BehaviourResult RepeatTime::Execute(GameObject* go)
{
	auto child = GetChild();
	if (!child)
		return BehaviourResult::Failure;

	m_TimeLeft -= GetFrameTime();
	if (m_TimeLeft > 0)
	{
		child->Execute(go);
		return BehaviourResult::Pending;
	}
	m_TimeLeft = m_MaxTime;
	return BehaviourResult::Success;
}


/// --- REPEAT COUNT --- ///
BehaviourResult RepeatCount::Execute(GameObject* go)
{
	auto child = GetChild();
	BehaviourResult result = BehaviourResult::Failure;
	if (!go || !child)
		return result;

	SetContext("RepeatCount", m_Repetitions);
	for (unsigned int i = 0; i < (SingleFrame ? Repetitions : 1u); i++)
	{
		result = child->Execute(go);
		SetContext("RepeatCount", ++m_Repetitions);
	}

	if (m_Repetitions >= Repetitions)
		m_Repetitions = 0;
	else
		return BehaviourResult::Pending;

	return result;
}

/// --- REPEAT UNTIL FAIL --- ///
BehaviourResult RepeatUntilFail::Execute(GameObject* go)
{
	auto child = GetChild();
	if (!go || !child)
		return BehaviourResult::Failure;
	BehaviourResult result;
	
	SetContext("RepeatCount", m_Repetitions);

	if (SingleFrame)
	{
		while ((result = child->Execute(go)) != BehaviourResult::Failure)
			SetContext("RepeatCount", ++m_Repetitions);
		return BehaviourResult::Success;
	}

	result = child->Execute(go);
	if (result == BehaviourResult::Failure)
	{
		m_Repetitions = 0;
		ClearContext("RepeatCount");
		cout << "[RepeatUntilFail] Failed!" << endl;
		return result;
	}
	return BehaviourResult::Pending;
}