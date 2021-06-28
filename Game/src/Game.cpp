#include <cstdlib> // rand()
#include <Game.hpp>
#include <Systems/AgentSystem.hpp>
#include <Components/AgentComponent.hpp>
#include <GameEngine/ResourceManager.hpp>
#include <Components/TargetComponent.hpp>
#include <GameEngine/CommonComponents.hpp>
#include <GameEngine/Systems/PhysicsSystem.hpp>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#pragma warning(push, 0) // Disable warnings from raylib & raygui
#include <raylib.h>
#include <raygui.h> // Immediate-mode GUI
#pragma warning(pop)

using namespace ECS;
using namespace Utilities;

const float BirbSize = 75;	// Scale
const string BirbSpriteSheet = "assets/Sprites/Birbs.png";
int BirbSpriteIndex = 1;

const float TargetSize = 50;	// Scale
const string TargetSpriteSheet = "assets/Sprites/Foods.png";

Game::Game(const ApplicationArgs& args) : Application(args)
{
	auto physicsSystem = GetWorld()->GetSystem<PhysicsSystem>();
	physicsSystem->SetGravity(Vector2{ 0, 0 });

	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

#ifndef NDEBUG
	physicsSystem->DrawDebugInfo = true;
#endif

	GetWorld()->AddSystem<AgentSystem>();

	// --- INPUT --- //
	InputSystem* input = GetInput();
	input->Map(KEY_ESCAPE, "KeyQuit", [&](DataStream) { Quit(); });

	// Pre-load assets on main thread
	PreloadAssets();

	input->Map(MOUSE_BUTTON_LEFT, "SpawnTarget", [&](const DataStream&) { CreateTarget(GetMousePosition()); }, InputBindingState::Down);

	CreateBirbs(10);

	GAME_LOG_DEBUG("Total entity count: " + to_string(GetWorld()->GetEntityCount()));
}

void Game::OnUpdate(float deltaTime)
{
	SetWindowTitle(("Game [entities: " + to_string(GetWorld()->GetEntityCount()) + "][DeltaTime: " + to_string(GetFrameTime()) + "]").c_str());
}

void Game::OnDrawGUI()
{

}

static unsigned int RandomIncrement = 0;
void Game::CreateTarget(Vector2 position)
{
	Entity target = GetWorld()->CreateEntity();
	target.AddComponent<TargetComponent>();

	auto transform = target.AddComponent<TransformComponent>();
	transform->Position = position;
	transform->Scale = { TargetSize, TargetSize};

	auto sprite = target.AddComponent<SpriteComponent>();
	sprite->Sprite = ResourceManager::LoadTexture(TargetSpriteSheet);

	const float TargetWidth = 64;
	const unsigned int TargetSprites = 37;
	const unsigned int TargetSpriteSheetWidth = 512;
	unsigned int randomSprite = rand() % TargetSprites;
	srand(RandomIncrement++);
	sprite->ReferenceSize =
	{
		randomSprite * TargetWidth,  // x offset
		0,  // y offset
		TargetWidth, // width
		TargetWidth  // height
	};

	auto physics = target.AddComponent<PhysicsBodyComponent>();
	physics->Trigger = true;
	physics->Static = true;

	m_Targets.push_back(target);

	EntityID targetID = target.ID;
	Events()->AddReceiver("PhysicsTrigger", [=](DataStream stream)
	{
		if (stream.read<EntityID>() != GetWorld()->ID())
			return; // Not for us?

		EntityID a = stream.read<EntityID>();
		EntityID b = stream.read<EntityID>();

		if (a == targetID)
			GetWorld()->DestroyEntity(targetID);
		if (b == targetID)			  
			GetWorld()->DestroyEntity(targetID);

		/* PLAY AUDIO WHEN TARGET DESTROYED

		AudioComponent* audio = nullptr;
		if (a == m_Player) audio = GetWorld()->AddComponent<AudioComponent>(b);
		else audio = GetWorld()->AddComponent<AudioComponent>(a);

		if (audio->Sound != InvalidResourceID)
			return; // Already has audio component

		audio->Sound = ObstacleHitSoundID;
		audio->EndAction = AudioEndAction::Remove;
		*/
	});
}

void Game::CreateBirbs(unsigned int count)
{
	const float ScreenPadding = 150.0f;
	Vector2 resolution = GetResolution();
	srand(time(0) + (RandomIncrement * RandomIncrement++));
	for (unsigned int i = 0; i < count; i++)
	{
		Entity birb = GetWorld()->CreateEntity();

		auto transform = birb.AddComponent<TransformComponent>();
		transform->Position =
		{
			ScreenPadding + (rand() % (int)resolution.x - ScreenPadding),
			ScreenPadding + (rand() % (int)resolution.y - ScreenPadding)
		};
		transform->Scale = { BirbSize, BirbSize };

		const float SpriteWidth = 48;
		const unsigned int SpritesPerLine = 12;
		const vector<unsigned int> BirbBeginFrames =
		{
			0,  // Light brown
			3,  // Brown
			6,  // Red,
			9,  // Yellow
			48, // White
			51, // Lime
			54, // Blue
			57, // Grey
		};

		auto sprite = birb.AddComponent<AnimatedSpriteComponent>();
		sprite->Sprite = ResourceManager::LoadTexture(BirbSpriteSheet);
		sprite->TimeBetweenFrames = 150;
		sprite->MaxFrames = 3;

		unsigned int selectedBirb = rand() % BirbBeginFrames.size();
		sprite->Frame = BirbBeginFrames[selectedBirb];
		sprite->ReferenceSize =
		{
			0,  // x
			0,  // y
			SpriteWidth, // Width
			SpriteWidth  // Height
		};

		birb.AddComponent<AgentComponent>();
		birb.AddComponent<PhysicsBodyComponent>();

		m_Birbs.push_back(birb);
	}
}

void Game::CreateRandomTargets(unsigned int count)
{
	const float ScreenPadding = 50.0f;
	Vector2 resolution = GetResolution();
	srand(time(0) + (RandomIncrement * RandomIncrement++));
	for (unsigned int i = 0; i < count; i++)
	{
		CreateTarget(
			{
				ScreenPadding + (rand() % (int)resolution.x - ScreenPadding),
				ScreenPadding + (rand() % (int)resolution.y - ScreenPadding)
			});
	}
}

void Game::PreloadAssets()
{
	ResourceManager::LoadTexture(BirbSpriteSheet);
	ResourceManager::LoadTexture(TargetSpriteSheet);
}