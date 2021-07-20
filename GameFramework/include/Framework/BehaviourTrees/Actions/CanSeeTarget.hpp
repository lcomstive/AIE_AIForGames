#pragma once
#include <string>
#include <vector>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class CanSeeTarget : public Action
	{
		class CanSeeTargetRaycastCallback : public b2RayCastCallback
		{
		public:
			bool CanSee;
			unsigned int TargetID;
			bool Started, Finished;

			CanSeeTargetRaycastCallback() : CanSee(false), Started(false), Finished(false), TargetID(-1) { }

			float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
		};

		CanSeeTargetRaycastCallback  m_Callback;

	public:
		float SightRange = 100.0f;
		float FieldOfView = 60.0f;
		unsigned int TargetID = (unsigned int)-1;

		bool GetTargetFromContext = false;

		virtual std::string GetName() override { return "CanSeeTarget"; }
		virtual BehaviourResult Execute(GameObject* go) override;
	};
}