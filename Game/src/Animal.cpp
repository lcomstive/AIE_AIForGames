#include <Game.hpp>
#include <algorithm>
#include <Animal.hpp>

#include <Framework/BehaviourTrees/Actions/Move.hpp>
#include <Framework/BehaviourTrees/Actions/Wait.hpp>
#include <Framework/BehaviourTrees/Actions/CanSee.hpp>
#include <Framework/BehaviourTrees/Actions/FindPath.hpp>
#include <Framework/BehaviourTrees/Actions/SetValue.hpp>
#include <Framework/BehaviourTrees/Actions/LimitTime.hpp>
#include <Framework/BehaviourTrees/Actions/PlaySound.hpp>
#include <Framework/BehaviourTrees/Actions/MoveTowards.hpp>
#include <Framework/BehaviourTrees/Actions/CallFunction.hpp>
#include <Framework/BehaviourTrees/Actions/NavigatePath.hpp>
#include <Framework/BehaviourTrees/Actions/CanSeeTarget.hpp>

using namespace std;
using namespace Framework;
using namespace Framework::BT;
using namespace Framework::Pathfinding;

Animal::Animal(GameObject* parent) : AnimatedSprite(parent) { Init(); }
Animal::Animal(Texture texture, GameObject* parent) : AnimatedSprite(texture, parent) { Init(); }
Animal::Animal(string texturePath, GameObject* parent) : AnimatedSprite(texturePath, parent) { Init(); }

void Animal::Init()
{
	m_Health = 100.0f;
	m_Hunger = m_Thirst = 0.0f;
}

void Animal::OnUpdate()
{
	float deltaTime = GetFrameTime();
	m_Thirst += Game::ThirstPerSecond * deltaTime;
	m_Hunger += Game::HungerPerSecond * deltaTime;

	if (m_Thirst >= 1.0f)
		m_Health -= Game::HealthDecayPerSecond * deltaTime;
	if (m_Hunger >= 1.0f)
		m_Health -= Game::HealthDecayPerSecond * deltaTime;

	m_Health = clamp(m_Health, 0.0f, 100.0f);
	m_Thirst = clamp(m_Thirst, 0.0f, 1.0f);
	m_Hunger = clamp(m_Hunger, 0.0f, 1.0f);

	m_BehaviourTree->Update();
	Framework::AnimatedSprite::OnUpdate();

	// TODO: DEATH CHECK IN BEHAVIOUR TREE (for animation)
	if (m_Health <= 0.0f)
		Destroy();
}

void Animal::OnDraw()
{
	Framework::AnimatedSprite::OnDraw();

	Vec2 pos = GetPosition();
	DrawLineEx((Vec2)(pos - Vec2(7.5f, 5.0f)), (Vec2)(pos - Vec2(7.5f - (15 * m_Health / 100.0f), 5.0f)), 2.0f, RED);
	DrawLineEx((Vec2)(pos - Vec2(7.5f, 0.0f)), (Vec2)(pos - Vec2(7.5f - (15 * m_Thirst), 0.0f)), 2.0f, BLUE);
	DrawLineEx((Vec2)(pos - Vec2(7.5f, -5.0f)), (Vec2)(pos - Vec2(7.5f - (15 * m_Hunger), -5.0f)), 2.0f, GREEN);

#ifndef NDEBUG
	m_BehaviourTree->DebugDraw();
#endif
}

float Animal::GetSpeed() { return m_Speed; }
float Animal::GetHealth() { return m_Health; }
float Animal::GetThirst() { return m_Thirst; }
float Animal::GetHunger() { return m_Hunger; }
FoodClass Animal::GetFoodClass() { return m_FoodClass; }

void Animal::AddHealth(float value) { m_Health = clamp(m_Health + value, -1.0f, 100.0f); }
void Animal::AddThirst(float value) { m_Thirst = clamp(m_Thirst + value, 0.0f, 1.0f); }
void Animal::AddHunger(float value) { m_Hunger = clamp(m_Hunger + value, 0.0f, 1.0f); }

void Animal::SetSpeed(float value) { m_Speed = value; }
void Animal::SetHealth(float value) { m_Health = value; }
void Animal::SetThirst(float value) { m_Thirst = value; }
void Animal::SetHunger(float value) { m_Hunger = value; }
void Animal::SetFoodClass(FoodClass foodClass) { m_FoodClass = foodClass; }

void Animal::InitBehaviourTree(Grid<SquareGridNode>* grid)
{
	m_Grid = grid;
	m_BehaviourTree = make_unique<BehaviourTree>(this);

	CreateBehaviourCheckDeath();

	if (m_FoodClass == FoodClass::Herbivore)
	{
		AddTag("PassiveCreature");
		CreateBehaviourCheckPredator();
	}

	CreateBehaviourCheckWater();
	CreateBehaviourCheckFood();
	CreateBehaviourWander();
}

void Animal::CreateBehaviourWander()
{
	auto sequence = m_BehaviourTree->Add<Sequence>();

#ifndef NDEBUG
	sequence->AddChild<Log>()->Message = "Behaviour - Wander";
#endif

	// Calculate random direction
	sequence->AddChild<CallFunction>()->Function = [](GameObject* go, CallFunction* caller)
	{
		auto cellSize = caller->GetContext<float>("CellSize", 1.0f);
		auto grid = caller->GetContext<Grid<SquareGridNode>*>("AStarGrid", nullptr);
		if (!grid || cellSize < 0)
			return false;
		auto position = go->GetPosition() / cellSize;

		// Ensure direction is traversable
		for (unsigned int i = 0; i < 4; i++)
		{
			bool moveX = rand() % 2 == 0;
			bool movePositive = rand() % 2 == 0;

			Vec2 direction = { (float)moveX, (float)!moveX };
			if (!movePositive)
				direction = direction * -1.0f;

			if (!grid->GetCell(position + direction)->Traversable)
				continue; // Path not traversable, try another random direction

			// Valid direction found
			caller->SetContext("Direction", direction);
			caller->SetContext("Speed", ((Animal*)go)->GetSpeed() / 2.0f); // Half speed for "leisurely" wanderings
			return true;
		}
		return false;
	};

	
	auto loop = sequence->AddChild<RepeatTime>();
	loop->SetTime(1.0f); // Repeat child execution for x seconds
	loop->SetChild<Move>()->GetValuesFromContext = true;

	// Reset speed to original value
	sequence->AddChild<CallFunction>()->Function = [](GameObject* go, CallFunction* caller) { caller->SetContext("Speed", ((Animal*)go)->GetSpeed()); return true; };

	sequence->AddChild<Wait>()->SetTime(0.5f);
}

void Animal::CreateBehaviourCheckFood()
{
	auto sequence = m_BehaviourTree->Add<Sequence>();

	sequence->AddChild<Conditional>()->Function = [=](GameObject* go, Conditional*)
	{
		return GetHunger() > GetThirst() && GetHunger() >= 0.5f;
	};

#ifndef NDEBUG
	sequence->AddChild<Log>()->Message = "Behaviour - Food";
#endif

	vector<string> foodTags = {};
	if (m_FoodClass == FoodClass::Herbivore || m_FoodClass == FoodClass::Omnivore)
		foodTags.emplace_back("HerbivoreFood");
	if (m_FoodClass == FoodClass::Carnivore || m_FoodClass == FoodClass::Omnivore)
	{
		foodTags.emplace_back("CarnivoreFood");
		foodTags.emplace_back("PassiveCreature");
	}
	
	auto repeat = sequence->AddChild<Repeat>();
	repeat->Condition = [](GameObject*, Repeat* caller) { return !caller->ContextExists("RepeatCount") || caller->ContextExists("Path"); };
	auto repeatSequence = repeat->SetChild<Sequence>();

	AddFindClosestNavigatable(repeatSequence, foodTags)->Sight = 10000.0f; // TODO: Change depending on creature?

	// Remove last node of path, so navigation is next to target
	repeatSequence->AddChild<CallFunction>()->Function = [](GameObject*, CallFunction* caller)
	{
		if (!caller->ContextExists("Path"))
			return false; // Cause node to return fail
		auto path = caller->GetContext<vector<AStarCell*>>("Path");
		if (!path.empty())
			path.erase(path.end() - 1);
		caller->SetContext("Path", path);
		return true;
	};

	// If the timer runs out, recalculate path to food (incase it got deleted or moved)
	auto limitTime = repeatSequence->AddChild<LimitTime>();
	limitTime->SetTime(1.5f);
	limitTime->SetChild<NavigatePath>()->Speed = m_Speed;

	// Reset thirst
	repeatSequence->AddChild<CallFunction>()->Function = [=](GameObject* go, CallFunction* caller)
	{
		unsigned int targetID = caller->GetContext("Target", -1);
		GameObject* target = GameObject::FromID(targetID);
		if (!target)
			return false;
		Animal* goAnimal = (Animal*)go;
		Animal* targetAnimal = dynamic_cast<Animal*>(target);
		if (targetAnimal)
		{
			goAnimal->AddHunger(targetAnimal->GetHealth() / -200.0f); // Regain hunger, half the health of target
			targetAnimal->SetHealth(0); // Destroy target

			return true;
		}
		else
			goAnimal->SetHunger(0.0f); // Source of food that is (probably) not alive

		return true;
	};
}

void Animal::CreateBehaviourCheckDeath()
{
	/* 
	
	Doesn't work consistently, might comment out for submission as not enough time to properly implement
	

	auto condition = m_BehaviourTree->Add<Conditional>();
	condition->Function = [](GameObject* go, Conditional*) { return ((Animal*)go)->GetHealth() <= 0.0f; };

	auto sequence = condition->SetChild<Sequence>();

#ifndef NDEBUG
	sequence->AddChild<Log>()->Message = "Behaviour - DEATH";
#endif

	// sequence->AddChild<SetValue>()->Set("AnimationState", "Death");
#ifndef NDEBUG
	sequence->AddChild<Log>()->Message = "Behaviour - DEATH SETVALUE";
#endif
// #ifndef NDEBUG
	sequence->AddChild<Conditional>()->Function = [](GameObject* go, Conditional* conditional) { return rand() % 10 + 1 < 2; }; // 20% chance of true (I think?)
	auto sound = sequence->AddChild<BT::PlaySound>();
	sound->Sound = LoadSound("./assets/Sounds/Unacceptable.mp3");
	SetAudioStreamVolume(sound->Sound.stream, 0.5f);
	sequence->AddChild<Log>()->Message = "Behaviour - DEATH SOUND";
	sound->WaitForFinish = true;
// #endif

	// sequence->AddChild<Wait>()->SetTime(0.25f);
	sequence->AddChild<CallFunction>()->Function = [](GameObject* go, CallFunction*) { go->Destroy(); return true; };

	*/
}

void Animal::CreateBehaviourCheckWater()
{
	auto sequence = m_BehaviourTree->Add<Sequence>();
	sequence->AddChild<Conditional>()->Function = [=](GameObject* go, Conditional*)
	{
		return GetThirst() > GetHunger() && GetThirst() >= 0.4f;
	};
#ifndef NDEBUG
	sequence->AddChild<Log>()->Message = "Behaviour - Water";
#endif

	AddFindClosestNavigatable(sequence, { "WaterSource" })->Sight = 10000.0f; // TODO: Change depending on creature?

	// Remove last node of path, so navigation is just outside of water's edge
	sequence->AddChild<CallFunction>()->Function = [](GameObject*, CallFunction* caller)
	{
		if (!caller->ContextExists("Path"))
			return false; // Cause node to return fail
		auto path = caller->GetContext<vector<AStarCell*>>("Path");
		if (!path.empty())
			path.erase(path.end() - 1);
		caller->SetContext("Path", path);
		return true;
	};

	sequence->AddChild<NavigatePath>()->Speed = m_Speed;

	// Reset thirst
	sequence->AddChild<CallFunction>()->Function = [=](GameObject* go, CallFunction*)
	{
		SetThirst(0);
		return true;
	};
}

void Animal::CreateBehaviourCheckPredator()
{
	auto sequence = m_BehaviourTree->Add<Sequence>();

	AddFindClosestNavigatable(sequence, { "Predator" })->Sight = 250.0f;
#ifndef NDEBUG
	sequence->AddChild<Log>()->Message = "Behaviour - Predator";
	sequence->AddChild<Log>()->Message = "{" + to_string(GetID()) + "} should flee!";
#endif

	// Calculate random direction
	sequence->AddChild<CallFunction>()->Function = [](GameObject* go, CallFunction* caller)
	{
		auto cellSize = caller->GetContext<float>("CellSize", 1.0f);
		auto grid = caller->GetContext<Grid<SquareGridNode>*>("AStarGrid", nullptr);
		auto predatorID = caller->GetContext("Target", (unsigned int)-1);
		GameObject* predator = GameObject::FromID(predatorID);
		if (!grid || cellSize < 0 || !predator)
			return false;
		auto position = go->GetPosition() / cellSize;

		Vec2 direction = (go->GetPosition() - predator->GetPosition()).Normalized();
		caller->SetContext("Direction", direction);
		caller->SetContext("Speed", ((Animal*)go)->GetSpeed());

		return false;
	};


	auto loop = sequence->AddChild<RepeatTime>();
	loop->SetTime(1.0f); // Repeat child execution for x seconds
	loop->SetChild<Move>()->GetValuesFromContext = true;
}

FindClosestNavigatable* Animal::AddFindClosestNavigatable(Sequence* parent, vector<string> tags)
{
	auto findClosest = parent->AddChild<FindClosestNavigatable>();
	findClosest->TargetTags = tags; // Tag to find
	findClosest->Sight = 100.0f; // TODO: Sight depends on creature?
	findClosest->CopyGrid(m_Grid);
	return findClosest;
}