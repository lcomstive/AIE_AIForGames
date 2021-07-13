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

	enum class BehaviourResult { Success, Failure, Pending };

	class BehaviourNode
	{
		friend Composite;
		friend Decorator;
		friend BehaviourTree;

		std::unordered_map<std::string, void*>* m_Context;
	public:
		BehaviourNode() = default;
		~BehaviourNode() = default;

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
				m_Child.release();

			m_Child.swap(new T());
			m_Child->m_Context = m_Context;
			return m_Child.get();
		}
	};

	// Abstract node class with many children behaviours
	class Composite : public BehaviourNode
	{
		std::vector<BehaviourNode*> m_Children;

	protected:
		int PendingChildIndex = -1; // TODO: Find a better way of doing this

	public:
		~Composite();

		std::vector<BehaviourNode*>& GetChildren();

		template<typename T>
		T* AddChild()
		{
			bool isBehaviourType = std::is_base_of<BT::BehaviourNode, T>::value;
			assert(isBehaviourType);

			m_Children.emplace_back(new T());
			T* child = m_Children[m_Children.size() - 1];
			((BehaviourNode*)child)->m_Context = m_Context;
			return child;
		}

		virtual BehaviourResult Execute(GameObject* go) = 0;
	};

	// AND node (runs child behaviours until one fails)
	class Sequence : public Composite
	{
	public:
		virtual BehaviourResult Execute(GameObject* go) override;
	};
	
	// OR node (runs child behaviours until one succeeds)
	class Selector : public Composite
	{
	public:
		virtual BehaviourResult Execute(GameObject* go) override;
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
			bool isBehaviourType = std::is_base_of<BT::BehaviourNode, T>::value;
			assert(isBehaviourType);

			if (m_Child)
				m_Child.release();

			m_Child.reset(new T());
			m_Child->m_Context = m_Context;
			return (T*)m_Child.get();
		}

		virtual BehaviourResult Execute(GameObject* go) = 0;
	};

	// Inverts result of single child node
	class Inverse : public Decorator
	{
	public:
		virtual BehaviourResult Execute(GameObject* go) override;
	};

	// Prints message when executed, useful for debugging
	class LogDecorator : public Decorator
	{
	public:
		std::string Message;

		virtual BehaviourResult Execute(GameObject* go) override;
	};
	
	// Prints message when executed, useful for debugging
	class DynamicLogDecorator : public Decorator
	{
	public:
		std::function<std::string(GameObject* go, DynamicLogDecorator* caller)> MessageGenerator;

		virtual BehaviourResult Execute(GameObject* go) override;
	};

	// Always successful result
	class Succeeder : public Decorator
	{
	public:
		virtual BehaviourResult Execute(GameObject* go) override;
	};

	// Repeats execution of child until count is reached
	class RepeatCount : public Decorator
	{
	public:
		unsigned int Repetitions = 1;

		virtual BehaviourResult Execute(GameObject* go) override;
	};

	// Repeats execution of child until failure is returned from execution, then returns success
	class RepeatUntilFail : public Decorator
	{
	public:
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}