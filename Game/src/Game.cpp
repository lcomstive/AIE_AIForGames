#include <Game.hpp>
#include <string>
#include <raylib.h>
#include <Framework/BehaviourTrees/BehaviourTreeNodes.hpp>

using namespace std;
using namespace Framework;
using namespace Framework::BT;

Game::Game()
{
	InitWindow(800, 600, "Game");
	SetWindowState(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

	m_Root = new GameObject("Root");

	Texture t = LoadTexture("./assets/Sprites/Foods.png");
	for (float i = -10; i < 10; i++)
	{
		for (float j = -10; j < 10; j++)
		{
			AnimatedSprite* sprite = new AnimatedSprite(t);
			sprite->SetSize(Vector2{ 100, 100 });
			sprite->View.width = sprite->View.height = 64;
			sprite->MaxFrames = 37;
			sprite->SetTimeBetweenFrames(0.5f);

			sprite->SetPosition({ i * 80, j * 80 });

			sprite->Frame = (unsigned int)(i + j);

			m_Root->AddChild(sprite);
		}
	}

	m_Camera = Camera2D();
	m_Camera.zoom = 1.0f;
	m_Camera.target = { 0, 0 };

	auto repeat = m_BehaviourTree.Add<RepeatCount>();
	repeat->Repetitions = 10;
	auto log = repeat->SetChild<DynamicLogDecorator>();
	log->MessageGenerator = [](GameObject* go, DynamicLogDecorator* caller)
	{
		return "Test message [" + to_string(caller->GetContext<unsigned int>("RepeatCount")) + "] '" + go->GetName() + "'";
	};

	m_BehaviourTree.Update(m_Root);
}

Game::~Game()
{
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
		ClearBackground(Color { 10, 10, 10, 255 });

		Update();

		m_Root->Update();
		m_Root->Draw();

#ifndef NDEBUG
		DrawFPS(10, 10);
#endif

		EndMode2D();
		EndDrawing();
	}
}

void Game::Update()
{
	auto children = m_Root->GetChildren();
	for (auto child : children)
		child->SetRotation(child->GetRotation() + 1.0f);
}

void main()
{
	Game game;
	game.Run();
}