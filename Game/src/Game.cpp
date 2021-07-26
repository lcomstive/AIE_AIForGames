#include <string>
#include <thread>
#include <iostream>
#include <Game.hpp>
#include <raylib.h>
#include <Framework/PhysicsWorld.hpp>
#include <Framework/GameObjects/AnimatedSprite.hpp>

#include <Framework/BehaviourTrees/Actions/Wait.hpp>
#include <Framework/BehaviourTrees/Actions/CanSee.hpp>
#include <Framework/BehaviourTrees/Actions/FindPath.hpp>
#include <Framework/BehaviourTrees/Actions/SetValue.hpp>
#include <Framework/BehaviourTrees/Actions/MoveTowards.hpp>
#include <Framework/BehaviourTrees/Actions/CallFunction.hpp>
#include <Framework/BehaviourTrees/Actions/NavigatePath.hpp>
#include <Framework/BehaviourTrees/Actions/CanSeeTarget.hpp>
#include <Framework/BehaviourTrees/Actions/FindClosestNavigatable.hpp>

using namespace std;
using namespace Framework;
using namespace Framework::BT;

const float Game::ThirstPerSecond = 0.02f;
const float Game::HungerPerSecond = 0.05f;
const float Game::HealthDecayPerSecond = 5.0f;

// Offsets of the backgroundsheet tileset
Vec2 BackgroundTileSize = { 16, 16 };

enum class WaterEdge : int
{
	Top			= 1,
	TopRight	= 0,
	TopLeft		= 2,

	Left		= 4,
	Right		= 3,

	Bottom		= 6,
	BottomLeft	= 7,
	BottomRight	= 5,

	JoinTopLeft = 10,
	JoinTopRight = 11,
	JoinBottomLeft = 8,
	JoinBottomRight = 9,
};

Game::Game()
{
	InitAudioDevice();
	InitWindow(1280, 720, "Game");
	SetWindowState(FLAG_WINDOW_RESIZABLE);

	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

	PhysicsWorldArgs args;
	args.Gravity = { 0, 0 };
	args.TimeStep = 1.0f / 100.0f;
	PhysicsWorld::Init(args);

	m_Root = new GameObject("Root");

	CreateMap();
	CreateCreatureInfos();

	// Camera
	m_Camera = Camera2D();
	m_Camera.zoom = 0.9f;
	m_Camera.target = { m_Map.GetWidth() * GridCellSize / 2.0f , m_Map.GetHeight() * GridCellSize / 2.0f };
}

Game::~Game()
{
	delete m_Root;
	CloseWindow();
	PhysicsWorld::Destroy();
}

void Game::Run()
{
	while (!WindowShouldClose())
	{
		m_Camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
		BeginDrawing();
		BeginMode2D(m_Camera);
		ClearBackground(Color{ 12, 17, 18, 100 });

		Update();

		PrePhysicsUpdate();
		m_Root->PrePhysicsUpdate();

		PhysicsWorld::Step();

		PostPhysicsUpdate();
		m_Root->PostPhysicsUpdate();

		m_Root->Update();
		m_Root->Draw();

		// Draw debug physics colliders
		PhysicsWorld::GetBox2DWorld()->DebugDraw();

#ifndef NDEBUG
		/// TEMP ///
		for (unsigned int x = 0; x < m_PathfindingGrid->GetWidth(); x++)
		{
			for (unsigned int y = 0; y < m_PathfindingGrid->GetHeight(); y++)
			{
				auto cell = m_PathfindingGrid->GetCell(x, y);
				if (!cell->Traversable)
					DrawRectangle(x * GridCellSize, y * GridCellSize, 10, 10, RED);
				DrawText(to_string((unsigned int)cell->Cost).c_str(), x * GridCellSize, y * GridCellSize, 10, PINK);
			}
		}
		/// TEMP ///
#endif

		EndMode2D();

		DrawUI();
		EndDrawing();
	}
}

void Game::Update()
{
	// Simple camera drag controls
	auto mouseDelta = GetMouseDelta();
	mouseDelta.x /= m_Camera.zoom;
	mouseDelta.y /= m_Camera.zoom;
	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		m_Camera.target = { m_Camera.target.x - mouseDelta.x, m_Camera.target.y - mouseDelta.y };

	// Zoom in & out using mouse scroll
	m_Camera.zoom += GetMouseWheelMove() * GetFrameTime() * ZoomSpeed;
	m_Camera.zoom = max(min(m_Camera.zoom, MaxZoom), MinZoom); // Clamp

	// Spawn random creatures at mouse position
	Vec2 mousePos = (Vec2)GetScreenToWorld2D(GetMousePosition(), m_Camera) / GridCellSize;
	mousePos.x = floorf(mousePos.x);
	mousePos.y = floorf(mousePos.y);
	auto cell = m_PathfindingGrid->GetCell((unsigned int)mousePos.x, (unsigned int)mousePos.y);
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
		mousePos.x >= 0 && mousePos.x < m_Map.GetWidth()  &&
		mousePos.y >= 0 && mousePos.y < m_Map.GetHeight() &&
		cell && cell->Traversable)
		SpawnRandomCreature(mousePos * GridCellSize + Vec2(GridCellSize / 2.0f, GridCellSize / 2.0f));
}

void Game::PrePhysicsUpdate() { }
void Game::PostPhysicsUpdate() { }

void Game::DrawUI()
{
// #ifndef NDEBUG
	DrawFPS(10, 10);
// #endif
}

int main()
{
	Game game;
	game.Run();
	return 0;
}

GameObject* Game::AddBackgroundTile(unsigned int x, unsigned int y, char c, int spriteIndex)
{
	auto tileDef = m_Map.GetTileDef(c);
	Sprite* tile = new Sprite(m_BackgroundSheet);

	tile->SetSize({ (float)GridCellSize, (float)GridCellSize });

	// Check for valid sprite offset index, if invalid then generate random
	if (spriteIndex < 0 || spriteIndex >= tileDef.SpritesheetOffsets.size())
		spriteIndex = rand() % tileDef.SpritesheetOffsets.size();

	tile->SetView(
		tileDef.SpritesheetOffsets[spriteIndex],
		BackgroundTileSize
	);

	tile->SetPosition({ x * GridCellSize + GridCellSize / 2.0f, y * GridCellSize - GridCellSize / 2.0f });
	return tile;
}

GameObject* Game::AddBackgroundTileWaterEdge(unsigned int x, unsigned int y)
{
	WaterEdge edge = WaterEdge::Top;

	char tileLeft = m_Map.GetTileChar(x + 1, y);
	char tileRight = m_Map.GetTileChar(x - 1, y);
	char tileUp = m_Map.GetTileChar(x, y - 1);
	char tileDown = m_Map.GetTileChar(x, y + 1);

	// Check diagonals
	if (m_Map.GetTileChar(x + 1, y - 1) == 'W')
		edge = WaterEdge::BottomRight;
	if (m_Map.GetTileChar(x - 1, y - 1) == 'W')
		edge = WaterEdge::BottomLeft;
	if (m_Map.GetTileChar(x + 1, y + 1) == 'W')
		edge = WaterEdge::TopRight;
	if (m_Map.GetTileChar(x - 1, y + 1) == 'W')
		edge = WaterEdge::TopLeft;

	// Check immediate edges
	if (tileLeft == 'W')
		edge = WaterEdge::Right;
	if (tileRight == 'W') 
		edge = WaterEdge::Left;
	if (tileUp == 'W')
		edge = WaterEdge::Bottom;
	if (tileDown == 'W') 
		edge = WaterEdge::Top;

	// Check Joins
	if (tileUp == 'E' && tileRight == 'E' && m_Map.GetTileChar(x - 1, y - 1) != 'W')
		edge = WaterEdge::JoinTopRight;
	if (tileUp == 'E' && tileLeft == 'E' && m_Map.GetTileChar(x + 1, y - 1) != 'W')
		edge = WaterEdge::JoinTopLeft;
	if (tileDown == 'E' && tileRight == 'E' && m_Map.GetTileChar(x - 1, y + 1) != 'W')
		edge = WaterEdge::JoinBottomRight;
	if (tileDown == 'E' && tileLeft == 'E' && m_Map.GetTileChar(x + 1, y + 1) != 'W')
		edge = WaterEdge::JoinBottomLeft;

	return AddBackgroundTile(x, y, 'E', (int)edge);
}

GameObject* Game::SpawnRandomCreature(Vec2 position)
{
	auto creatureInfo = m_CreatureInfos[rand() % m_CreatureInfos.size()];
	Animal* creature = new Animal(creatureInfo.SpriteSheet);
	creature->Frame = 0;
	creature->MaxFrames = creatureInfo.MaxFrames;
	creature->SetTimeBetweenFrames(0.1f);

	creature->AddTag("Creature");
	creature->SetSize(creatureInfo.Size);
	creature->SetView(creatureInfo.SpriteOffset, creatureInfo.SpriteSize);
	creature->SetPosition(position);

	creature->GeneratePhysicsBody();

	creature->SetFoodClass(creatureInfo.FoodSource);
	FoodClass foodClass = creatureInfo.FoodSource;
	if (foodClass == FoodClass::Carnivore || foodClass == FoodClass::Omnivore)
		creature->AddTag("Predator");
	else
		creature->AddTag("PassiveCreature");
	
	m_Root->AddChild(creature);
	m_Creatures.push_back(creature);

	creature->GetBehaviourTree()->Root()->SetContext("CellSize", GridCellSize);

	// Behaviour Tree
	/*
	auto canSee = creature->GetBehaviourTree()->Add<CanSee>();
	canSee->TargetTag = "Creature";
	canSee->SightRange = 150.0f;
	canSee->FieldOfView = 50.0f;

	auto findPath = creature->GetBehaviourTree()->Add<FindPath>();
	findPath->CopyGrid(m_PathfindingGrid.get());
	findPath->StepsPerUpdate = 1;

	auto moveTowards = creature->GetBehaviourTree()->Add<NavigatePath>();
	moveTowards->Speed = 50.0f;
	*/

	// Check for predators and flee
	if (foodClass == FoodClass::Herbivore)
	{
		auto predatorCheck = creature->GetBehaviourTree()->Add<Sequence>();
		auto predatorFoundClosest = predatorCheck->AddChild<FindClosestNavigatable>();
		predatorFoundClosest->TargetTags = { "Predator" };
		predatorFoundClosest->Sight = 50.0f;
		predatorFoundClosest->CopyGrid(m_PathfindingGrid.get());
		// Check that predator was found
		predatorCheck->AddChild<Conditional>()->Function = [](GameObject* go, Conditional* caller) { return caller->ContextExists("Found"); };
		predatorCheck->AddChild<Log>()->Message = "FLEE!!";
		// TODO: Implement fleeing behaviour
	}

	// If thirsty, find closest water and navigate to it
	auto waterCheck = creature->GetBehaviourTree()->Add<Sequence>();
	waterCheck->AddChild<Conditional>()->Function = [](GameObject* go, Conditional* caller) { return ((Animal*)go)->GetThirst() >= 0.65f; };
	waterCheck->AddChild<Log>()->Message = "Checking for water source...";
	auto waterFindClosest = waterCheck->AddChild<FindClosestNavigatable>();
	waterFindClosest->TargetTags = { "WaterSource" };
	waterFindClosest->Sight = 10000.0f;
	waterFindClosest->CopyGrid(m_PathfindingGrid.get());
	waterCheck->AddChild<CallFunction>()->Function = [](GameObject* go, CallFunction* caller)
	{
		if (!caller->ContextExists("Path"))
			return false;
		auto path = caller->GetContext<vector<Pathfinding::AStarCell*>>("Path");
		cout << "Path: " << path.size() << endl;
		if (!path.empty())
			path.erase(path.end() - 1);
		caller->SetContext("Path", path);
		cout << "Path: " << path.size() << endl;
		return true;
	};
	waterCheck->AddChild<Log>()->Message = "Found path to water source, navigating...";
	auto waterNavPath = waterCheck->AddChild<NavigatePath>();
	waterNavPath->UpdatePathEachFrame = true;
	waterNavPath->Speed = creatureInfo.Speed;
	waterCheck->AddChild<Log>()->Message = "Navigated to water source!";
	waterCheck->AddChild<CallFunction>()->Function = [](GameObject* go, CallFunction* caller) { ((Animal*)go)->SetThirst(0.0f); return true; };
	waterCheck->AddChild<Log>()->Message = "Drunk water!";
	
	// If hungry, find closest food source and navigate to it
	auto hungerCheck = creature->GetBehaviourTree()->Add<Sequence>();
	hungerCheck->AddChild<Conditional>()->Function = [](GameObject* go, Conditional* caller) { return ((Animal*)go)->GetHunger() >= 0.4f; };
	hungerCheck->AddChild<Log>()->Message = "Checking for hunger source...";
	auto hungerFindClosest = hungerCheck->AddChild<FindClosestNavigatable>();

	hungerFindClosest->TargetTags = {};
	if(foodClass == FoodClass::Herbivore || foodClass == FoodClass::Omnivore)
		hungerFindClosest->TargetTags.emplace_back("HerbivoreFood");
	if (foodClass == FoodClass::Carnivore || foodClass == FoodClass::Omnivore)
	{
		hungerFindClosest->TargetTags.emplace_back("CarnivoreFood");
		hungerFindClosest->TargetTags.emplace_back("PassiveCreature");
	}

	hungerFindClosest->Sight = 10000.0f;
	hungerFindClosest->CopyGrid(m_PathfindingGrid.get());
	hungerCheck->AddChild<CallFunction>()->Function = [](GameObject* go, CallFunction* caller)
	{
		if (!caller->ContextExists("Path"))
			return false;
		auto path = caller->GetContext<vector<Pathfinding::AStarCell*>>("Path");
		cout << "Path: " << path.size() << endl;
		if (!path.empty())
			path.erase(path.end() - 1);
		caller->SetContext("Path", path);
		cout << "Path: " << path.size() << endl;
		return true;
	};
	hungerCheck->AddChild<Log>()->Message = "Found path to hunger source, navigating...";
	auto hungerNavPath = hungerCheck->AddChild<NavigatePath>();
	hungerNavPath->UpdatePathEachFrame = true;
	hungerNavPath->Speed = creatureInfo.Speed;
	hungerCheck->AddChild<Log>()->Message = "Navigated to hunger source!";
	hungerCheck->AddChild<CallFunction>()->Function = [](GameObject* go, CallFunction* caller)
	{
		unsigned int targetID = caller->GetContext("Target", -1);
		GameObject* target = GameObject::FromID(targetID);
		if (!target)
			return false;
		Animal* goAnimal = (Animal*)go;
		Animal* targetAnimal = dynamic_cast<Animal*>(target);
		if (targetAnimal)
		{
			targetAnimal->SetHealth(targetAnimal->GetHealth() - 25);
			goAnimal->SetHunger(goAnimal->GetHunger() - 0.2f);
			return true;
		}
		else
			goAnimal->SetHunger(0.0f); // Source of food that is (probably) not alive
	};
	hungerCheck->AddChild<Log>()->Message = "Ate food!";

#ifndef NDEBUG
	cout << "Created creature @ " << creature->GetPosition() << " " << (creature->GetPosition() / GridCellSize);
	switch (foodClass)
	{
	default:
	case FoodClass::Omnivore: cout << " [Omnivore]" << endl; break;
	case FoodClass::Herbivore: cout << " [Herbivore]" << endl; break;
	case FoodClass::Carnivore: cout << " [Carnivore]" << endl; break;
	}
#endif
	return creature;
}

void Game::CreateMap()
{
	m_Map.Load("./assets/default.map");

	m_Map.AddTileDef('G',
		{
			"Grass",
			{
				{ 85,  0 },
				{ 85, 17 }
			}
		});
	m_Map.AddTileDef('W',
		{
			"Water",
			{
				{  0, 0 },
				{ 17, 0 }
			}
		});
	m_Map.AddTileDef('E',
		{
			"Water Edge",
			{
				{ 34,  0 }, // Top-Left
				{ 51,  0 }, // Top
				{ 68,  0 }, // Top-Right
				{ 34, 17 }, // Left
				{ 68, 17 }, // Right
				{ 34, 34 }, // Bottom-Left
				{ 51, 34 }, // Bottom
				{ 68, 34 }, // Bottom-Right
				{  0, 17}, // Joint Bottom-Right
				{ 17, 17 }, // Joint Bottom-Left
				{  0, 34 }, // Joint Top-Right
				{ 17, 34 }, // Joint Top-Left
			}
		});
	m_Map.AddTileDef('F',
		{
			"Flower",
			{
				{ 493, 153 },	// Blue
				{ 476, 153 },	// Red
				{ 510, 153 },	// Purple
				{ 527, 153 }	// White
			}
		});
	m_Map.AddTileDef('B',
		{
			"Bush",
			{
				{ 323, 153 },
				{ 408, 153 },
				{ 408, 170 },
				{ 408, 187 }
			}
		});
	m_Map.AddTileDef('-',
		{
			"Barrier",
			{ { 697, 170 } }
		});

	// Pre-load textures
	m_SlimeSpriteSheet = LoadTexture("./assets/Sprites/PixelNauta/slime-sheet.png");
	m_SkeletonSpriteSheet = LoadTexture("./assets/Sprites/JesseM/Skeleton Idle.png");
	m_BackgroundSheet = LoadTexture("./assets/Sprites/Kenney/Roguelike/Spritesheet/roguelikeSheet_transparent.png");

	unsigned int mapWidth = m_Map.GetWidth();
	unsigned int mapHeight = m_Map.GetHeight();

	// Create A* grid
	m_PathfindingGrid = make_unique<PathfindingGrid>(mapWidth, mapHeight);

	const unordered_map<char, string> CellTags =
	{
		{ 'E', "WaterSource"   },
		{ 'F', "HerbivoreFood" },
		{ 'B', "HerbivoreFood" }
	};

	auto background = new GameObject("Background");
	for (unsigned int x = 0; x < mapWidth; x++)
	{
		for (unsigned int y = 0; y < mapHeight; y++)
		{
			char tileChar = m_Map.GetTileChar(x, y);
			auto& tile = m_Map.GetTileDef(tileChar);
			if (tile.SpritesheetOffsets.size() == 0)
				continue; // Empty

			// For setting pathfinding properties
			auto cell = m_PathfindingGrid->GetCell(x, y - 1);
			cell->Traversable = true;

			// Create tile
			auto foreground = AddBackgroundTile(x, y, tileChar);

			switch (tileChar)
			{
			default: break;
			case 'F':
			case 'B':
			case '-':
			{
				auto backgroundTile = AddBackgroundTile(x, y, 'G'); // Add grass behind
				backgroundTile->AddChild(foreground);
				foreground = backgroundTile;

				if (tileChar == '-')
				{
					cell->Traversable = false;
					foreground->GeneratePhysicsBody(false); // Raycast hittable tiles
				}
				else
					cell->Cost = 3;

				if (CellTags.find(tileChar) != CellTags.end())
					foreground->AddTag(CellTags.at(tileChar));
				break;
			}
			case 'E':
			{
				delete foreground;

				foreground = AddBackgroundTileWaterEdge(x, y);
				background->AddChild(foreground);

				if (CellTags.find(tileChar) != CellTags.end())
					foreground->AddTag(CellTags.at(tileChar));
				continue;
			}
			case 'W':
			{
				cell->Traversable = false;
				break;

				if (CellTags.find(tileChar) != CellTags.end())
					foreground->AddTag(CellTags.at(tileChar));
			}
			}

			background->AddChild(foreground);
		}
	}
	m_Root->AddChild(background);
}

/// CREATURE INFO ///
void Game::CreateCreatureInfos()
{
	/// --- SLIME --- ///
	CreatureInfo info(m_SlimeSpriteSheet);
	info.Size = { GridCellSize, GridCellSize };
	info.SpriteSize = { 32, 32 };
	info.MaxFrames = 4;
	info.Speed = 15.0f;
	info.SpriteOffset = { 0, 5 * 32 };
	info.FoodSource = FoodClass::Herbivore;
	m_CreatureInfos.emplace_back(info);

	/// --- SKELETON --- ///
	/*
	info = CreatureInfo(m_SkeletonSpriteSheet);
	info.Size = { GridCellSize, GridCellSize };
	info.SpriteSize = { 24, 32 };
	info.MaxFrames = 11;
	info.Speed = 25.0f;
	info.FoodSource = FoodClass::Carnivore; // They eat slimes! *surprised pikachu*
	m_CreatureInfos.emplace_back(info);
	*/
}

Game::CreatureInfo::CreatureInfo(string filepath) : CreatureInfo(LoadTexture(filepath.c_str())) { m_CreatedTexture = true; }
Game::CreatureInfo::CreatureInfo(Texture& texture) :
	Speed(10),
	Size(1, 1),
	MaxFrames(1),
	SpriteSize(1, 1),
	SpriteOffset(0, 0),
	SpriteSheet(texture),
	m_CreatedTexture(false),
	FoodSource(FoodClass::Herbivore)
	{ }

Game::CreatureInfo::~CreatureInfo()
{
	if(m_CreatedTexture)
		UnloadTexture(SpriteSheet);
}