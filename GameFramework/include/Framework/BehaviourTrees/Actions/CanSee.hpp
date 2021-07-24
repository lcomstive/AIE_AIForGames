#pragma once
#include <string>
#include <vector>
#include <box2d/b2_world_callbacks.h>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

namespace Framework::BT
{
	class CanSee: public Action
	{
		class CanSeeRaycastCallback : public b2RayCastCallback
		{
		public:
			bool Started, Finished;
			std::string Tag;
			GameObject* Found;

			CanSeeRaycastCallback() : Started(false), Finished(false), Found(nullptr), Tag() { }

			float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override;
		};

		CanSeeRaycastCallback m_Callback;

	public:
		float SightRange = 100.0f;
		float FieldOfView = 60.0f;
		std::string TargetTag = "Player";

		bool GetValuesFromContext = false;

		virtual std::string GetName() override { return "CanSee"; }
		virtual BehaviourResult Execute(GameObject* go) override;
		virtual void OnDebugDraw(GameObject* go) override;

		static bool InFieldOfView(GameObject* a, GameObject* b, float fov);
	};
}