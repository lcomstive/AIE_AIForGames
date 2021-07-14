#include <string>
#include <iostream>
#include <Game.hpp>
#include <raylib.h>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

using namespace std;
using namespace Framework;
using namespace Framework::BT;
using namespace Framework::Pathfinding;

Game::Game()
{
	InitWindow(1000, 1000, "Game");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

	m_Root = new GameObject("Root");

	// Chicken
	AnimatedSprite* chicken = new AnimatedSprite("./assets/Sprites/Chicken/Idle (32x34).png");
	chicken->SetSize(Vector2{ 100, 100 });
	chicken->View.width = 32;
	chicken->View.height = 34;
	chicken->MaxFrames = 13;
	chicken->SetTimeBetweenFrames(0.1f);
	chicken->SetPosition({ -400, 0 });

	m_Root->AddChild(chicken);

	m_Camera = Camera2D();
	m_Camera.zoom = 1.0f;
	m_Camera.target = { 0, 0 };

	/// TEST BEHAVIOUR TREE ///
	/*
	auto sequence = m_BehaviourTree.Add<RandomSequence>();

	auto eval = sequence->AddChild<Evaluator>();
	eval->Function = [](GameObject* go, Evaluator* caller) { return caller->GetContext<unsigned int>("SequenceIndex") > 5; };
	auto dynamicLog = eval->SetResult<DynamicLogDecorator>(true);
	dynamicLog->Message = [](GameObject* go, DynamicLogDecorator* caller) { return "Evaluated true!"; };
	dynamicLog->SetChild<Succeeder>();
	dynamicLog = eval->SetResult<DynamicLogDecorator>(false);
	dynamicLog->Message = [](GameObject* go, DynamicLogDecorator* caller) { return "Evaluated false!"; };
	dynamicLog->SetChild<Succeeder>();

	for (int i = 0; i < 10; i++)
	{
		auto log = sequence->AddChild<DynamicLogDecorator>();
		log->Message = [=](GameObject* go, DynamicLogDecorator* caller) { return "Test message [" + to_string(i) + "]"; };
		log->SetChild<Succeeder>();
	}

	m_BehaviourTree.Update(m_Root); // Should be called every Update instead
	*/

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

	m_AStar = AStar(5, AStar::EuclideanHeuristic);
	m_AStar.StartSearch(m_AStarGrid->GetCell(0, 0), m_AStarGrid->GetCell(endX, endY));
}

Game::~Game()
{
	CloseWindow();
	delete m_AStarGrid;
	delete m_Root;
}

void Game::Run()
{
	const float AStarStepTime = 0.0f;
	float nextStep = AStarStepTime;
	while (!WindowShouldClose())
	{
		m_Camera.offset = { GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f };
		BeginDrawing();
		BeginMode2D(m_Camera);
		ClearBackground(Color { 10, 10, 10, 255 });

		Update();

		m_Root->Update();
		m_Root->Draw();

		if (!m_AStar.IsFinished())
		{
			nextStep -= GetFrameTime();
			if (nextStep <= 0.0f)
			{
				m_AStar.Step();
				nextStep = AStarStepTime;
			}
		}

		const int CellSize = 10;
		const int CellPadding = 0;
		auto foundPath = m_AStar.GetPath();
		float largestScore = m_AStar.GetLargestFScore();
		for (int x = 0; x < GridWidth; x++)
		{
			for (int y = 0; y < GridHeight; y++)
			{
				Color color = { 20, 20, 20, 255 };
				int score = (m_AStarGrid->GetCell(x, y)->FScore / largestScore) * color.r * 2.0f;
				color.r += min(score, (int)color.r);

				if (!m_AStarGrid->GetCell(x, y)->Traversable)
					color = DARKGRAY;

				if (find(foundPath.begin(), foundPath.end(), m_AStarGrid->GetCell(x, y)) != foundPath.end())
					color = RED;

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
				/*
				DrawText(
					("(" + to_string(x) + ", " + to_string(y) + ")").c_str(),
					finalX - CellSize / 4.0f,
					finalY - CellSize / 10.0f,
					11,
					BLACK
				);
				*/
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

#ifndef NDEBUG
		DrawFPS(-GetScreenWidth() / 2 + 10, -GetScreenHeight() / 2 + 10);
#endif

		EndMode2D();
		EndDrawing();
	}
}

void Game::Update()
{
	/*
	auto children = m_Root->GetChildren();
	for (auto child : children)
		child->SetRotation(child->GetRotation() + 1.0f);
	*/
}

void main()
{
	Game game;
	game.Run();
}