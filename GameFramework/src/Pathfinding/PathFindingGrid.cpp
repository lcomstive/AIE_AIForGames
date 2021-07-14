#include <Framework/Pathfinding/PathFindingGrid.hpp>

using namespace std;
using namespace Framework;
using namespace Framework::Pathfinding;

/// --- GRID NODES --- ///
void SquareGridNodeAddNeighbour(AStarCell* cell, int offsetX, int offsetY, Grid<SquareGridNode>* grid)
{
	int finalX = cell->x + offsetX;
	int finalY = cell->y + offsetY;
	if (finalX < 0 || finalY < 0 ||
		finalX >= grid->GetWidth() ||
		finalY >= grid->GetHeight())
		return;

	auto newCell = grid->GetCell(finalX, finalY);
	if (!newCell->Traversable)
		return;

	// Check diagonal
#ifdef SQUARE_GRID_NODE_DIAGONAL
	if (offsetX != 0 && offsetY != 0)
	{
		if (!grid->GetCell(finalX, cell->y)->Traversable ||
			!grid->GetCell(cell->x, finalY)->Traversable)
			return;
	}
#endif

	cell->Neighbours.emplace_back(AStarConnection(newCell));
}

void SquareGridNode::CalculateNeighbours(void* gridPtr)
{
	auto grid = (Grid<SquareGridNode>*)gridPtr;
	Cell.Neighbours.clear();

	// Left
	SquareGridNodeAddNeighbour(&Cell,  1,  0, grid); // Right
	SquareGridNodeAddNeighbour(&Cell, -1,  0, grid); // Left
	SquareGridNodeAddNeighbour(&Cell,  0,  1, grid); // Up
	SquareGridNodeAddNeighbour(&Cell,  0, -1, grid); // Down

#ifdef SQUARE_GRID_NODE_DIAGONAL
	SquareGridNodeAddNeighbour(&Cell,  1,  1, grid); // Up Right
	SquareGridNodeAddNeighbour(&Cell, -1,  1, grid); // Up Left
	SquareGridNodeAddNeighbour(&Cell,  1, -1, grid); // Down Right
	SquareGridNodeAddNeighbour(&Cell, -1, -1, grid); // Down Left
#endif
}

void TriangleGridNode::CalculateNeighbours(void* gridPtr)
{
	auto grid = (Grid<TriangleGridNode>*)gridPtr;
	Cell.Neighbours.clear();

	// Left
	if (Cell.x > 0) Cell.Neighbours.emplace_back(grid->GetCell(Cell.x - 1, Cell.y));

	// Right
	if (Cell.x < grid->GetWidth()) Cell.Neighbours.emplace_back(grid->GetCell(Cell.x + 1, Cell.y));

	// Up or Down
	int yOffset = OrientedUpwards ? -1 : 1;
	if ((yOffset < 0 && Cell.y > 0) ||
		(yOffset > 0 && Cell.y < grid->GetHeight()))
		Cell.Neighbours.emplace_back(grid->GetCell(Cell.x, Cell.y + yOffset));
}

void HexGridNodeAddNeighbour(AStarCell* cell, int offsetX, int offsetY, Grid<HexGridNode>* grid)
{
	if ((int)cell->y % 2 != 0)
		offsetX -= 1;

	int finalX = cell->x + offsetX;
	int finalY = cell->y + offsetY;
	if (finalX < 0 || finalY < 0 ||
		finalX >= grid->GetWidth() ||
		finalY >= grid->GetHeight())
		return;

	auto newCell = grid->GetCell(finalX, finalY);
	if (newCell->Traversable)
		cell->Neighbours.emplace_back(AStarConnection(newCell));
}

void HexGridNode::CalculateNeighbours(void* gridPtr)
{
	auto grid = (Grid<HexGridNode>*)gridPtr;
	Cell.Neighbours.clear();

	HexGridNodeAddNeighbour(&Cell,  0,  1, grid); // NW
	HexGridNodeAddNeighbour(&Cell, -1,  0, grid); //  W
	HexGridNodeAddNeighbour(&Cell,  0, -1, grid); // SW
	HexGridNodeAddNeighbour(&Cell,  1, -1, grid); // SE
	HexGridNodeAddNeighbour(&Cell,  1,  0, grid); //  E
	HexGridNodeAddNeighbour(&Cell,  1,  1, grid); // NE
}