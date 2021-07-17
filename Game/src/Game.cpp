#include <string>
#include <iostream>
#include <Game.hpp>
#include <raylib.h>
#include <Framework/PhysicsWorld.hpp>

using namespace std;
using namespace Framework;

#ifdef BTREE_TEST
#include <Framework/BehaviourTrees/Actions/Wait.hpp>
#include <Framework/BehaviourTrees/Actions/SetValue.hpp>
#include <Framework/BehaviourTrees/Actions/PlaySound.hpp>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

using namespace Framework::BT;
#endif

#ifdef ASTAR_TEST
using namespace Framework::Pathfinding;
#endif

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

	// Chicken
	AnimatedSprite* chicken = new AnimatedSprite("./assets/Sprites/Chicken/Idle (32x34).png");
	chicken->SetSize(Vector2{ 100, 100 });
	chicken->View.width = 32;
	chicken->View.height = 34;
	chicken->MaxFrames = 13;
	chicken->SetTimeBetweenFrames(0.1f);
	chicken->SetPosition({ -400, 0 });

	chicken->GeneratePhysicsBody(true, 100, 0.0f);

	m_Root->AddChild(chicken);

	m_Camera = Camera2D();
	m_Camera.zoom = 1.0f;
	m_Camera.target = { 0, 0 };

	/// TEST BEHAVIOUR TREE ///
#ifdef BTREE_TEST
	auto eval = m_BehaviourTree.Add<Evaluator>();
	eval->Function = [](GameObject* go, Evaluator* caller) { return !caller->GetContext<bool>("HasWaited"); };

	auto sequenceTrue = eval->SetResult<Sequence>(true);
	sequenceTrue->AddChild<Log>()->Message = "Waiting...";
	sequenceTrue->AddChild<Wait>()->SetTime(1.0f); // Seconds
	sequenceTrue->AddChild<BT::PlaySound>()->Sound = LoadSound("./assets/Monke_1.wav");
	sequenceTrue->AddChild<Log>()->Message = "Finished!";
	sequenceTrue->AddChild<SetValue>()->Set("HasWaited", true);

	auto sequenceFalse = eval->SetResult<Sequence>(false);
	sequenceFalse->AddChild<Log>()->Message = "Finished waiting, resetting...";
	sequenceFalse->AddChild<Wait>()->SetTime(0.5f);
	sequenceFalse->AddChild<SetValue>()->Set("HasWaited", false);
#endif

#ifdef ASTAR_TEST
#if defined(ASTAR_GRID_HEXAGON)
	m_AStarGrid = new Grid<HexGridNode>(GridWidth, GridHeight);
#elif defined(ASTAR_GRID_TRIANGLE)
	m_AStarGrid = new Grid<TriangleGridNode>(GridWidth, GridHeight);
#else
	m_AStarGrid = new Grid<SquareGridNode>(GridWidth, GridHeight);
#endif

	for (int i = 0; i < (GridWidth * GridHeight) / 4; i++)
	{
		int x = rand() % GridWidth;
		int y = rand() % GridHeight;
		m_AStarGrid->GetCell(x, y)->Traversable = false;
	}

	unsigned int endX, endY;
	do
	{
	endX = rand() % GridWidth;
	endY = rand() % GridHeight;
	} while (!m_AStarGrid->GetCell(endX, endY)->Traversable);

	m_AStarGrid->RefreshNodes();

	m_AStar = AStar(10, AStar::EuclideanHeuristic);
	m_AStar.StartSearch(m_AStarGrid->GetCell(GridWidth / 2, GridHeight / 2), m_AStarGrid->GetCell(endX, endY));
#endif
}

Game::~Game()
{
	PhysicsWorld::Destroy();

	CloseWindow();
#ifdef ASTAR_TEST
	delete m_AStarGrid;
#endif
	delete m_Root;
}

void Game::Run()
{
#if ASTAR_TEST
	int AStarStepsPerFrame = 100;
	const float AStarTimeBetweenSteps = 0.0f;
	float nextStep = AStarTimeBetweenSteps;
#endif
	while (!WindowShouldClose())
	{
		m_Camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
		BeginDrawing();
		BeginMode2D(m_Camera);
		ClearBackground(Color{ 10, 10, 10, 255 });

#ifdef BTREE_TEST
		m_BehaviourTree.Update(m_Root);
#endif

		Update();

		m_Root->PrePhysicsUpdate();
		PhysicsWorld::Step();
		m_Root->PostPhysicsUpdate();

		m_Root->Update();
		m_Root->Draw();

#ifdef ASTAR_TEST
		if (!m_AStar.IsFinished())
		{
			nextStep -= GetFrameTime();
			if (nextStep <= 0.0f)
			{
				for(int i = 0; i < AStarStepsPerFrame; i++)
					m_AStar.Step();
				nextStep = AStarTimeBetweenSteps;
			}
		}

		auto foundPath = m_AStar.GetPath();
		float largestScore = m_AStar.GetLargestFScore();
		for (int x = 0; x < GridWidth; x++)
		{
			for (int y = 0; y < GridHeight; y++)
			{
				auto cell = m_AStarGrid->GetCell(x, y);
				Color color = { 20, 20, 20, 255 };
				int score = (cell->FScore / largestScore) * color.r * 2.0f;
				color.r += min(score, (int)color.r);

				if (cell->Traversable)
					continue;
					color = DARKGRAY;

#if defined(ASTAR_GRID_HEXAGON)
				float finalX = ((x + (y % 2 == 0 ? 0.5f : 0)) - GridWidth / 2.0f) * (CellSize + CellPadding);
				float finalY = (y - GridHeight / 2) * (CellSize * 0.9f + CellPadding);
				DrawPoly(
					Vector2
					{
						finalX,
						finalY
					},
					6,
					CellSize * 0.6f,
					0.0f,
					color
				);
				DrawText(
					("(" + to_string(x) + ", " + to_string(y) + ")").c_str(),
					finalX - CellSize / 4.0f,
					finalY - CellSize / 10.0f,
					11,
					BLACK
				);
#elif defined(ASTAR_GRID_TRIANGLE)

#else
				DrawRectangle(
					(x - GridWidth / 2) * (CellSize + CellPadding),
					(y - GridHeight / 2) * (CellSize + CellPadding),
					CellSize,
					CellSize,
					color
				);
#endif

			}
		}

#if defined(ASTAR_GRID_HEXAGON)

#elif defined(ASTAR_GRID_TRIANGLE)

#else
		if(m_AStar.IsFinished())
			for (auto pathNode : foundPath)
				DrawRectangle(
					(pathNode->x - GridWidth / 2) * (CellSize + CellPadding),
					(pathNode->y - GridHeight / 2) * (CellSize + CellPadding),
					CellSize,
					CellSize,
					RED
				);
#endif
#endif

		EndMode2D();

		// DRAW UI
// #ifndef NDEBUG
		DrawFPS(10, 10);
		DrawText(("Zoom: " + to_string(m_Camera.zoom)).c_str(), 10, 30, 11, GREEN);
// #endif

#if ASTAR_TEST
		DrawText(("A* Steps Per Frame: " + to_string(AStarStepsPerFrame)).c_str(), 10, 35, 11, GREEN);

		if (IsKeyPressed(KEY_UP))
			AStarStepsPerFrame *= 2;
		if (IsKeyPressed(KEY_DOWN) && AStarStepsPerFrame > 1)
			AStarStepsPerFrame /= 2;
#endif

		EndDrawing();
	}
}

void Game::Update()
{
	// Simple camera drag controls
	auto mouseDelta = GetMouseDelta();
	mouseDelta.x /= m_Camera.zoom;
	mouseDelta.y /= m_Camera.zoom;
	if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		m_Camera.target = { m_Camera.target.x - mouseDelta.x, m_Camera.target.y - mouseDelta.y };

#if ASTAR_TEST
	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		Vector2 pos = GetScreenToWorld2D(GetMousePosition(), m_Camera);
		pos.x += (GridWidth  / 2) * (CellSize + CellPadding);
		pos.y += (GridHeight / 2) * (CellSize + CellPadding);
		unsigned int cellX = pos.x / (CellSize + CellPadding);
		unsigned int cellY = pos.y / (CellSize + CellPadding);
		auto cell = m_AStarGrid->GetCell(cellX, cellY);
		if (cellX > 0 && cellX < GridWidth &&
			cellY > 0 && cellY < GridHeight &&
			cell->Traversable)
		{
			m_AStarGrid->RefreshNodes();
			m_AStar.StartSearch(m_AStarGrid->GetCell(GridWidth / 2, GridHeight / 2), cell);
		}
	}
#endif

	// Zoom in & out using mouse scroll

	m_Camera.zoom += GetMouseWheelMove() * GetFrameTime() * ZoomSpeed;
	m_Camera.zoom = max(min(m_Camera.zoom, MaxZoom), MinZoom); // Clamp

	if(IsKeyDown(KEY_SPACE))
		m_Root->GetChildren()[0]->GetPhysicsBody()->ApplyForce({ 10, 10 }, { 0, 0 }, true);
}

void main()
{
	Game game;
	game.Run();
}