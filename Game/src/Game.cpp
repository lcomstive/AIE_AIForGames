#include <string>
#include <iostream>
#include <Game.hpp>
#include <raylib.h>
#include <Framework/PhysicsWorld.hpp>
#include <Framework/GameObjects/AnimatedSprite.hpp>

#include <Framework/BehaviourTrees/Actions/Wait.hpp>
#include <Framework/BehaviourTrees/Actions/CanSee.hpp>
#include <Framework/BehaviourTrees/Actions/MoveTowards.hpp>

using namespace std;
using namespace Framework;
using namespace Framework::BT;

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
	PhysicsWorld::Init(args);

	m_Root = new GameObject("Root");

	CreateMap();
	CreateCreatureInfos();

	// Camera
	m_Camera = Camera2D();
	m_Camera.zoom = 0.9f;
	m_Camera.rotation = 180.0f;
	m_Camera.target = { m_Map.GetWidth() * GridCellSize / -2.0f , m_Map.GetHeight() * GridCellSize / 2.0f };
}

Game::~Game()
{
	PhysicsWorld::Destroy();
	CloseWindow();

	delete m_Root;
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
#ifndef NDEBUG
		PhysicsWorld::GetBox2DWorld()->DebugDraw();
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
		m_Camera.target = { m_Camera.target.x + mouseDelta.x, m_Camera.target.y + mouseDelta.y };

	// Zoom in & out using mouse scroll
	m_Camera.zoom += GetMouseWheelMove() * GetFrameTime() * ZoomSpeed;
	m_Camera.zoom = max(min(m_Camera.zoom, MaxZoom), MinZoom); // Clamp

	// Spawn random creatures at mouse position
	auto mousePos = (Vec2)GetScreenToWorld2D(GetMousePosition(), m_Camera) / GridCellSize;
	mousePos.x = floorf(-mousePos.x);
	mousePos.y = floorf( mousePos.y);
	auto cell = m_PathfindingGrid->GetCell((unsigned int)mousePos.x, (unsigned int)mousePos.y);
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
		mousePos.x >= 0 && mousePos.x < m_Map.GetWidth() &&
		mousePos.y >= 0 && mousePos.y < m_Map.GetHeight() &&
		cell && cell->Traversable)
	{
		SpawnRandomCreature(mousePos * GridCellSize);
	}
}

void Game::PrePhysicsUpdate() { }
void Game::PostPhysicsUpdate() { }

void Game::DrawUI()
{
#ifndef NDEBUG
	DrawFPS(10, 10);
#endif
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
	unsigned int mapY = m_Map.GetHeight() - y - 1;

	WaterEdge edge = WaterEdge::Top;

	char tileLeft = m_Map.GetTileChar(x - 1, mapY);
	char tileRight = m_Map.GetTileChar(x + 1, mapY);
	char tileUp = m_Map.GetTileChar(x, mapY - 1);
	char tileDown = m_Map.GetTileChar(x, mapY + 1);

	// Check diagonals
	if (m_Map.GetTileChar(x - 1, mapY - 1) == 'W')
		edge = WaterEdge::BottomRight;
	if (m_Map.GetTileChar(x + 1, mapY - 1) == 'W')
		edge = WaterEdge::BottomLeft;
	if (m_Map.GetTileChar(x - 1, mapY + 1) == 'W')
		edge = WaterEdge::TopRight;
	if (m_Map.GetTileChar(x + 1, mapY + 1) == 'W')
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
	if (tileUp == 'E' && tileRight == 'E' && m_Map.GetTileChar(x + 1, mapY - 1) != 'W')
		edge = WaterEdge::JoinTopRight;
	if (tileUp == 'E' && tileLeft == 'E' && m_Map.GetTileChar(x - 1, mapY - 1) != 'W')
		edge = WaterEdge::JoinTopLeft;
	if (tileDown == 'E' && tileRight == 'E' && m_Map.GetTileChar(x + 1, mapY + 1) != 'W')
		edge = WaterEdge::JoinBottomRight;
	if (tileDown == 'E' && tileLeft == 'E' && m_Map.GetTileChar(x - 1, mapY + 1) != 'W')
		edge = WaterEdge::JoinBottomLeft;

	return AddBackgroundTile(x, y, 'E', (int)edge);
}

GameObject* Game::SpawnRandomCreature(Vec2& position)
{
	auto creatureInfo = m_CreatureInfos[rand() % m_CreatureInfos.size()];
	Animal* creature = new Animal(creatureInfo.SpriteSheet);

	creature->AddTag("Creature");
	creature->SetSize(creatureInfo.Size);
	creature->SetTexture(creatureInfo.SpriteSheet);
	creature->SetView(creatureInfo.SpriteOffset, creatureInfo.SpriteSize);
	creature->SetPosition(position + Vec2{ GridCellSize / 2.0f, GridCellSize / 2.0f });

	creature->GeneratePhysicsBody();
	
	m_Root->AddChild(creature);
	m_Creatures.push_back(creature);

	// Behaviour Tree
	auto canSee = creature->GetBehaviourTree()->Add<CanSee>();
	canSee->TargetTag = "Creature";
	canSee->SightRange = 150.0f;
	canSee->FieldOfView = 50.0f;

	auto moveTowards = creature->GetBehaviourTree()->Add<MoveTowards>();
	moveTowards->Speed = 50.0f;
	moveTowards->GetValuesFromContext = true;

#ifndef NDEBUG
	cout << "Created creature @ " << creature->GetPosition() << endl;
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

	// Background
	m_BackgroundSheet = LoadTexture("./assets/Sprites/Kenney/Roguelike/Spritesheet/roguelikeSheet_transparent.png");

	unsigned int mapWidth = m_Map.GetWidth();
	unsigned int mapHeight = m_Map.GetHeight();

	// Create A* grid
	m_PathfindingGrid = make_unique<PathfindingGrid>(mapWidth, mapHeight);

	auto background = new GameObject("Background");
	for (unsigned int x = 0; x < mapWidth; x++)
	{
		for (unsigned int y = 0; y < mapHeight; y++)
		{
			char tileChar = m_Map.GetTileChar(x, mapHeight - y - 1); // Raylib inverts y axis, so get height - y
			auto& tile = m_Map.GetTileDef(tileChar);
			if (tile.SpritesheetOffsets.size() == 0)
				continue; // Empty

			// For setting pathfinding properties
			auto cell = m_PathfindingGrid->GetCell(x, mapHeight - y - 1);

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

				if (tileChar == '-' || tileChar == 'B') // Raycast hittable tiles
					foreground->GeneratePhysicsBody(false);

				cell->Traversable = false;
				break;
			}
			case 'E':
			{
				cell->Traversable = false;
				delete foreground;

				foreground = AddBackgroundTileWaterEdge(x, y);
				background->AddChild(foreground);
				continue;
			}
			case 'W':
			{
				cell->Traversable = false;
				break;
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
	Texture texture = LoadTexture("./assets/Sprites/Onocentaur/Colorful.png");

	CreatureInfo info(texture);
	info.Size = { GridCellSize, GridCellSize };
	info.SpriteSize = { 16, 16 };

	for (unsigned int i = 0; i < 25; i++)
	{
		info.SpriteOffset = { 0, i * 16.0f };
		m_CreatureInfos.push_back(info);
	}
}

Game::CreatureInfo::CreatureInfo(string filepath) : CreatureInfo(LoadTexture(filepath.c_str())) { m_CreatedTexture = true; }
Game::CreatureInfo::CreatureInfo(Texture& texture) :
	Size(1, 1),
	SpriteSize(1, 1),
	SpriteOffset(0, 0),
	SpriteSheet(texture),
	m_CreatedTexture(false)
	{ }

Game::CreatureInfo::~CreatureInfo()
{
	if(m_CreatedTexture)
		UnloadTexture(SpriteSheet);
}