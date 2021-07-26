#pragma once
#include <vector>
#include <cassert>
#include <Framework/Vec2.hpp>
#include <Framework/Pathfinding/AStar.hpp>

// #define SQUARE_GRID_NODE_DIAGONAL

namespace Framework::Pathfinding
{
	template<typename T>
	class Grid;

	struct GridNode
	{
		AStarCell Cell;

		virtual unsigned int NumberSides() = 0;
		virtual void CalculateNeighbours(void* grid) = 0;
	};

	struct SquareGridNode : public GridNode
	{
		unsigned int NumberSides() { return 4; }
		virtual void CalculateNeighbours(void* grid) override;
	};
	
	struct TriangleGridNode : public GridNode
	{
		bool OrientedUpwards;

		unsigned int NumberSides() { return 3; }
		virtual void CalculateNeighbours(void* grid) override;
	};

	struct HexGridNode : public GridNode
	{
		unsigned int NumberSides() { return 6; }
		virtual void CalculateNeighbours(void* grid) override;
	};

	template<typename T>
	class Grid
	{
		T** m_Grid;
		unsigned int m_Width, m_Height;

	public:
		Grid(unsigned int width, unsigned int height) : m_Width(width), m_Height(height)
		{
			bool isNode = std::is_base_of<GridNode, T>::value;
			assert(isNode);

			// Create grid
			m_Grid = new T * [m_Width];
			for (unsigned int x = 0; x < m_Width; x++)
			{
				m_Grid[x] = new T[m_Height];
				for (unsigned int y = 0; y < m_Height; y++)
				{
					m_Grid[x][y] = T();

					GridNode* node = (GridNode*)&m_Grid[x][y];
					node->Cell = AStarCell((float)x, (float)y);
				}
			}
		}

		void RefreshNodes()
		{
			for (unsigned int x = 0; x < m_Width; x++)
			{
				for (unsigned int y = 0; y < m_Height; y++)
				{
					GridNode* node = (GridNode*)&m_Grid[x][y];
					node->CalculateNeighbours(this);
					node->Cell.FScore = node->Cell.GScore = node->Cell.HScore = 0;
					node->Cell.Previous = nullptr;
				}
			}
		}

		~Grid()
		{
			for (unsigned int x = 0; x < m_Width; x++)
				delete[] m_Grid[x];
			delete[] m_Grid;
		}

		T* GetNode(unsigned int x, unsigned int y)
		{
			if (x >= m_Width)  x = m_Width  - 1;
			if (y >= m_Height) y = m_Height - 1;

			return &m_Grid[x][y];
		}

		AStarCell* GetCell(unsigned int x, unsigned int y)
		{
			if (x >= m_Width)  x = m_Width  - 1;
			if (y >= m_Height) y = m_Height - 1;

			return &((GridNode*)&m_Grid[x][y])->Cell;
		}
		
		T* GetNode(Vec2 pos)
		{
			if (pos.x >= m_Width)  pos.x = m_Width  - 1.0f;
			if (pos.y >= m_Height) pos.y = m_Height - 1.0f;

			return &m_Grid[(unsigned int)pos.x][(unsigned int)pos.y];
		}

		AStarCell* GetCell(Vec2 pos)
		{
			if (pos.x >= m_Width)  pos.x = m_Width - 1.0f;
			if (pos.y >= m_Height) pos.y = m_Height - 1.0f;

			return &((GridNode*)&m_Grid[(unsigned int)pos.x][(unsigned int)pos.y])->Cell;
		}

		unsigned int GetWidth() { return m_Width; }
		unsigned int GetHeight() { return m_Height; }
	};
}