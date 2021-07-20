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

		enum class ContextDataType { Integer, Decimal, String, Other };
		struct ContextData
		{
			ContextDataType Type;

			union
			{
				void*				Other;
				double				Decimal;
				std::string			String;
				unsigned long long	Integer;
			};

			ContextData() : Type(ContextDataType::Other), Other(nullptr) { }
			ContextData(const ContextData& other) : Type(other.Type), Other(other.Other) { }
			~ContextData() { }
		};

		std::unordered_map<std::string, ContextData>* m_Context;

	public:
		BehaviourNode() : m_Context() {}
		BehaviourNode(const BehaviourNode& other) : m_Context(other.m_Context) { }
		~BehaviourNode() = default;

		virtual std::string GetName() { return "Node"; }
		virtual BehaviourResult Execute(GameObject* go) = 0;

		void ClearContext();
		void ClearContext(std::string name);
		bool ContextExists(std::string name);

		template<typename T>
		void SetContext(std::string name, T value)
		{
			if (!ContextExists(name))
				m_Context->emplace(name, ContextData());

			ContextData* data = &m_Context->at(name);
			data->Type = ContextDataType::Other;
			data->Other = value;
		}

		void SetContext(std::string name, int value);
		void SetContext(std::string name, long value);
		void SetContext(std::string name, short value);
		void SetContext(std::string name, float value);
		void SetContext(std::string name, double value);
		void SetContext(std::string name, long long value);
		void SetContext(std::string name, std::string value);
		void SetContext(std::string name, unsigned int value);
		void SetContext(std::string name, unsigned long value);
		void SetContext(std::string name, unsigned short value);
		void SetContext(std::string name, unsigned long long value);

		template<typename T>
		T GetContext(std::string name)
		{
			if (!ContextExists(name))
				return T();
			// Can break something if type is different, but using void* to store data so no way to check for error
			return (T)m_Context->at(name).Other;
		}

		template<typename T>
		T GetContext(std::string name, T defaultValue)
		{
			if (!ContextExists(name))
				return defaultValue;
			// Can break something if type is different, but using void* to store data so no way to check for error
			return (T)m_Context->at(name).Other;
		}

		template<>
		int GetContext(std::string name) { return (int)GetContext<unsigned long long>(name); }
		template<>
		int GetContext(std::string name, int defaultValue) { return (int)GetContext<unsigned long long>(name, defaultValue); }

		template<>
		long GetContext(std::string name) { return (long)GetContext<unsigned long long>(name); }
		template<>
		long GetContext(std::string name, long defaultValue) { return (long)GetContext<unsigned long long>(name, defaultValue); }

		template<>
		short GetContext(std::string name) { return (short)GetContext<unsigned long long>(name); }
		template<>
		short GetContext(std::string name, short defaultValue) { return (short)GetContext<unsigned long long>(name, defaultValue); }

		template<>
		long long GetContext(std::string name) { return (short)GetContext<unsigned long long>(name); }
		template<>
		long long GetContext(std::string name, long long defaultValue) { return (short)GetContext<unsigned long long>(name, defaultValue); }

		template<>
		unsigned int GetContext(std::string name) { return (short)GetContext<unsigned long long>(name); }
		template<>
		unsigned int GetContext(std::string name, unsigned int defaultValue) { return (short)GetContext<unsigned long long>(name, defaultValue); }

		template<>
		unsigned long GetContext(std::string name) { return (short)GetContext<unsigned long long>(name); }
		template<>
		unsigned long GetContext(std::string name, unsigned long defaultValue) { return (short)GetContext<unsigned long long>(name, defaultValue); }

		template<>
		unsigned short GetContext(std::string name) { return (short)GetContext<unsigned long long>(name); }
		template<>
		unsigned short GetContext(std::string name, unsigned short defaultValue) { return (short)GetContext<unsigned long long>(name, defaultValue); }

		template<>
		unsigned long long GetContext(std::string name) { return GetContext<unsigned long long>(name, 0); }
		template<>
		unsigned long long GetContext(std::string name, unsigned long long defaultValue)
		{
			if (!ContextExists(name))
				return defaultValue;
			ContextData& data = m_Context->at(name);
			assert(data.Type == ContextDataType::Integer);
			return data.Integer;
		}

		template<>
		float GetContext(std::string name) { return (float)GetContext<double>(name); }
		template<>
		float GetContext(std::string name, float defaultValue) { return (float)GetContext<double>(name, defaultValue); }

		template<>
		double GetContext(std::string name) { return GetContext<double>(name, 0); }
		template<>
		double GetContext(std::string name, double defaultValue)
		{
			if (!ContextExists(name))
				return defaultValue;
			ContextData& data = m_Context->at(name);
			assert(data.Type == ContextDataType::Decimal);
			return data.Decimal;
		}

		template<>
		std::string GetContext(std::string name) { return GetContext(name, ""); }
		template<>
		std::string GetContext(std::string name, std::string defaultValue)
		{
			if (!ContextExists(name))
				return defaultValue;
			ContextData& data = m_Context->at(name);
			assert(data.Type == ContextDataType::String);
			return data.String;
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