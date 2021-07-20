#pragma once

#include <Framework/GameObjects/AnimatedSprite.hpp>

#define ASTAR_TEST
// #define BTREE_TEST

#ifdef ASTAR_TEST
#include <Framework/Pathfinding/AStar.hpp>
#include <Framework/Pathfinding/PathFindingGrid.hpp>

// #define ASTAR_GRID_HEXAGON
// #define ASTAR_GRID_TRIANGLE // UNTESTED
#endif

#ifdef BTREE_TEST
#include <Framework/BehaviourTrees/BehaviourTree.hpp>
#endif

class Game
{
	const float MinZoom = 0.05f;
	const float MaxZoom = 2.50f;
	const float ZoomSpeed = 10.0f;

	Camera2D m_Camera;
	Framework::GameObject* m_Root;

#ifdef BTREE_TEST
	Framework::BehaviourTree m_BehaviourTree;
#endif

#ifdef ASTAR_TEST
#ifdef ASTAR_GRID_HEXAGON
	const int GridWidth  = 50;
	const int GridHeight = 50;
#else
	const int GridWidth  = 1000;
	const int GridHeight = 1000;
#endif
	const int CellSize = 50;
	const int CellPadding = 0;

	Framework::Pathfinding::AStar m_AStar;

#if defined(ASTAR_GRID_HEXAGON)
	Framework::Pathfinding::Grid<Framework::Pathfinding::HexGridNode>* m_AStarGrid;
#elif defined(ASTAR_GRID_TRIANGLE)
	Framework::Pathfinding::Grid<Framework::Pathfinding::TriangleGridNode>* m_AStarGrid;
#else
	Framework::Pathfinding::Grid<Framework::Pathfinding::SquareGridNode>* m_AStarGrid;
#endif
#endif

public:
	Game();
	~Game();

	void Run();
	void Update();

	void PrePhysicsUpdate();
	void PostPhysicsUpdate();
};