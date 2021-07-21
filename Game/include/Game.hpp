#pragma once

#include <Map.hpp>
#include <Framework/GameObject.hpp>
#include <Framework/Pathfinding/PathFindingGrid.hpp>

using SquareGridNode = Framework::Pathfinding::SquareGridNode;

class Game
{
	const float MinZoom = 0.05f;
	const float MaxZoom = 2.50f;
	const float ZoomSpeed = 10.0f;

	const float GridCellSize = 25;

	Map m_Map;
	Camera2D m_Camera;
	Framework::GameObject* m_Root;
	Framework::Pathfinding::Grid<SquareGridNode> m_PathfindingGrid;

	// Background Tiles
	Texture m_BackgroundSheet;

	Framework::GameObject* AddBackgroundTile(unsigned int x, unsigned int y, char c, int spriteIndex = -1);
	Framework::GameObject* AddBackgroundTileWaterEdge(unsigned int x, unsigned int y);

public:
	Game();
	~Game();

	void Run();
	void Update();
	void DrawUI();

	void PrePhysicsUpdate();
	void PostPhysicsUpdate();
};