#pragma once
#include <vector>
#include <memory>
#include <string>
#include <cassert>
#include <functional>
#include <unordered_map>
#include <Framework/GameObject.hpp>

namespace Framework { class BehaviourTree; } // Forward declaration for friending

namespace Framework::BT
{
	class Composite;
	class Decorator;
	class Evaluator;

	enum class BehaviourResult { Success, Failure, Pending };

	class BehaviourNode
	{
		friend Composite;
		friend Decorator;
		friend Evaluator;
		friend BehaviourTree;

		std::unordered_map<std::string, void*>* m_Context;
	public:
		BehaviourNode() : m_Context() {}
		BehaviourNode(const BehaviourNode& other) : m_Context(other.m_Context) { }
		~BehaviourNode() = default;

		virtual std::string GetName() { return "Node"; }
		virtual BehaviourResult Execute(GameObject* go) = 0;

		void ClearContext();
		void ClearContext(std::string name);
		bool ContextExists(std::string name);
		void SetContext(std::string name, void* data);

		template<typename T>
		void SetContext(std::string name, T data) { SetContext(name, (void*)data); }

		template<typename T>
		T GetContext(std::string name)
		{
			if (!ContextExists(name))
				return T();
			// Can break something if type is different, but using void* to store data so no way to check for error
			return (T)m_Context->at(name);
		}
	};
	typedef BehaviourNode Action;

	// Executes a function and uses the result to execute one of two children
	class Evaluator : public BehaviourNode
	{
		std::unique_ptr<BehaviourNode> m_True;
		std::unique_ptr<BehaviourNode> m_False;

		friend BehaviourTree;

	public:
		std::function<bool(GameObject* go, Evaluator* caller)> Function;

		template<typename T>
		T* SetResult(bool comparison)
		{
			bool isBehaviourType = std::is_base_of<BT::BehaviourNode, T>::value;
			assert(isBehaviourType);

			std::unique_ptr<BehaviourNode>* m_Child;
			if (comparison)
				m_Child = &m_True;
			else
				m_Child = &m_False;

			if (m_Child)
				m_Child->release();

			m_Child->reset(new T());
			m_Child->get()->m_Context = m_Context;
			return (T*)m_Child->get();
		}

		virtual BehaviourResult Execute(GameObject* go) override;
		virtual std::string GetName() override { return "Evaluator"; }
	};

	class Sequence;

	// Abstract node class with many children behaviours
	class Composite : public BehaviourNode
	{
		std::vector<BehaviourNode*> m_Children;

	protected:
		int PendingChildIndex = -1; // TODO: Find a better way of doing this?

	public:
		~Composite();

		std::vector<BehaviourNode*>& GetChildren();

		template<typename T>
		T* AddChild()
		{
#ifndef NDEBUG
			bool isBehaviourType = std::is_base_of<BT::BehaviourNode, T>::value;
			assert(isBehaviourType);
#endif

			m_Children.emplace_back(new T());
			T* child = (T*)m_Children[m_Children.size() - 1];
			((BehaviourNode*)child)->m_Context = m_Context;
			return child;
		}

		virtual BehaviourResult Execute(GameObject* go) = 0;
		virtual std::string GetName() override { return "Composite"; }
	};

	// AND node (runs child behaviours until one fails)
	class Sequence : public Composite
	{
	public:
		virtual BehaviourResult Execute(GameObject* go) override;
		virtual std::string GetName() override { return "Sequence"; }
	};

	// Random AND node (runs random child behaviours until one fails)
	class RandomSequence : public Composite
	{
	public:
		virtual BehaviourResult Execute(GameObject* go) override;
		virtual std::string GetName() override { return "RandomSequence"; }
	};

	// OR node (runs child behaviours until one succeeds)
	class Selector : public Composite
	{
	public:
		virtual BehaviourResult Execute(GameObject* go) override;
		virtual std::string GetName() override { return "Selector"; }
	};

	// Random OR node (runs random child behaviours until one succeeds)
	class RandomSelector : public Composite
	{
	public:
		virtual BehaviourResult Execute(GameObject* go) override;
		virtual std::string GetName() override { return "RandomSelector"; }
	};

	/// --- DECORATORS --- ///

	// Decorator node, always has one child
	class Decorator : public BehaviourNode
	{
		std::unique_ptr<BehaviourNode> m_Child = nullptr;
	public:
		BehaviourNode* GetChild();

		template<typename T>
		T* SetChild()
		{
#ifndef NDEBUG
			bool isBehaviourType = std::is_base_of<BT::BehaviourNode, T>::value;
			assert(isBehaviourType);
#endif

			if (m_Child)
				m_Child.release();

			m_Child.reset(new T());
			m_Child->m_Context = m_Context;
			return (T*)m_Child.get();
		}

		virtual BehaviourResult Execute(GameObject* go) = 0;
		virtual std::string GetName() override { return "Decorator"; }
	};

	// Inverts result of single child node
	class Inverse : public Decorator
	{
	public:
		virtual BehaviourResult Execute(GameObject* go) override;
		virtual std::string GetName() override { return "Inverse"; }
	};

	// Prints message when executed, useful for debugging
	class Log : public Decorator
	{
	public:
		std::string Message;

		virtual BehaviourResult Execute(GameObject* go) override;
		virtual std::string GetName() override { return "Log"; }
	};

	// Prints message when executed, useful for debugging
	class DynamicLog : public Decorator
	{
	public:
		std::function<std::string(GameObject* go, DynamicLog* caller)> Message;

		virtual BehaviourResult Execute(GameObject* go) override;
		virtual std::string GetName() override { return "DynamicLog"; }
	};

	// Always successful result
	class Succeeder : public Decorator
	{
	public:
		virtual BehaviourResult Execute(GameObject* go) override;
		virtual std::string GetName() override { return "Succeeder"; }
	};

	// Repeats execution until condition is false (while loop)
	class Repeat : public Decorator
	{
	public:
		std::function<bool(GameObject* go, Repeat* caller)> Condition;

		virtual BehaviourResult Execute(GameObject* go) override;
		virtual std::string GetName() override { return "Repeat"; }
	};

	// Repeats execution of child until count is reached
	class RepeatCount : public Decorator
	{
	public:
		unsigned int Repetitions = 1;

		virtual BehaviourResult Execute(GameObject* go) override;
		virtual std::string GetName() override { return "RepeatCount"; }
	};

	// Repeats execution of child until failure is returned from execution, then returns success
	class RepeatUntilFail : public Decorator
	{
	public:
		virtual BehaviourResult Execute(GameObject* go) override;
		virtual std::string GetName() override { return "RepeatUntilFail"; }
	};
}