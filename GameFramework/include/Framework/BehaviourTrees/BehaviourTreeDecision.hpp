#pragma once
#include <Framework/GameObject.hpp>

namespace Framework::BT
{
	class Decision
	{
	public:
		virtual void Execute(GameObject* go) = 0;
	};

	class BoolDecision : public Decision
	{
	public:
		BoolDecision* True;
		BoolDecision* False;
	};
}