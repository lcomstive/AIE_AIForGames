#pragma once

#include <Framework/Pathfinding/AStar.hpp>
#include <Framework/GameObjects/AnimatedSprite.hpp>
#include <Framework/Pathfinding/PathFindingGrid.hpp>
#include <Framework/BehaviourTrees/BehaviourTree.hpp>

#define ASTAR_GRID_HEXAGON
#define ASTAR_GRID_TRIANGLE

class Game
{
	Camera2D m_Camera;
	Framework::GameObject* m_Root;

	Framework::BehaviourTree m_BehaviourTree;

	const bool Diagonals = true;
	const int GridWidth  = 180;
	const int GridHeight = 130;
	Framework::Pathfinding::AStar m_AStar;


#if defined(ASTAR_GRID_HEXAGON)
	Framework::Pathfinding::Grid<Framework::Pathfinding::HexGridNode>* m_AStarGrid;
#elif defined(ASTAR_GRID_TRIANGLE)
	Framework::Pathfinding::Grid<Framework::Pathfinding::TriangleGridNode>* m_AStarGrid;
#else
	Framework::Pathfinding::Grid<Framework::Pathfinding::SquareGridNode>* m_AStarGrid;
#endif

public:
	Game();
	~Game();

	void Run();
	void Update();
};