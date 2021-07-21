#include <string>
#include <iostream>
#include <Game.hpp>
#include <raylib.h>
#include <Framework/PhysicsWorld.hpp>
#include <Framework/GameObjects/AnimatedSprite.hpp>

using namespace std;
using namespace Framework;

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
	InitWindow(1000, 1000, "Game");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

	PhysicsWorldArgs args;
	args.Gravity = { 0, 0 };
	PhysicsWorld::Init(args);

	m_Root = new GameObject("Root");

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

	unsigned int mapHeight = m_Map.GetHeight();
	for (unsigned int x = 0; x < m_Map.GetWidth(); x++)
	{
		for (unsigned int y = 0; y < m_Map.GetHeight(); y++)
		{
			char tileChar = m_Map.GetTileChar(x, mapHeight - y - 1); // Raylib inverts y axis, so get height - y
			auto& tile = m_Map.GetTileDef(tileChar);
			if (tile.SpritesheetOffsets.size() == 0)
				continue; // Empty

			auto foreground = AddBackgroundTile(x, y, tileChar);

			switch (tileChar)
			{
			default: break;
			case 'F':
			case 'B':
			case '-':
			{
				auto background = AddBackgroundTile(x, y, 'G'); // Add grass behind
				background->AddChild(foreground);
				foreground = background;
				break;
			}
			case 'E':
			{
				m_Root->AddChild(AddBackgroundTileWaterEdge(x, y));
				continue;
			}
			}

			m_Root->AddChild(foreground);
		}
	}

	// Camera
	m_Camera = Camera2D();
	m_Camera.zoom = 1.0f;
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
		ClearBackground(Color{ 10, 10, 10, 255 });

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

	tile->SetPosition({ x * GridCellSize, y * GridCellSize });
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