#include <Map.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace Framework;

unordered_map<char, MapTileDefinition> Map::m_TileDefinitions;

Map::Map()
{
	if (!HasTileDef('\0')) // Default/Empty tiles
	{
		AddTileDef('\0', { "Default", vector<Vec2>() });
		AddTileDef(' ', { "Default", vector<Vec2>() });
	}
}

Map::~Map() { Unload(); }

void Map::Load(string filepath)
{
	ifstream file(filepath);
	if (!file)
	{
		cout << "Couldn't open map '" << filepath << "'" << endl;
		return; // Unable to open
	}

	string line;
	m_Tiles.clear();
	unsigned int y = 0;
	m_MaxWidth = m_MaxHeight = 1;
	while (getline(file, line))
	{
		if (line.empty() || line[0] == '!')
			continue; // Comment

		m_Tiles.emplace_back(vector<char>());
		for (size_t x = 0; x < line.size(); x++)
		{
			if (line[x] == '!') // Comment
				break;

			m_Tiles[y].emplace_back(line[x]);

			if (x >= m_MaxWidth)
				m_MaxWidth = (x + 1);
		}

		y++;
		m_MaxHeight++;
	}
	file.close();

	cout << "Loaded map '" << filepath << "'" << endl;
}

void Map::Unload()
{
	for (size_t x = 0; x < m_Tiles.size(); x++)
		m_Tiles[x].clear();
	m_Tiles.clear();
}

unsigned int Map::GetWidth() { return m_MaxWidth; }
unsigned int Map::GetHeight() { return m_MaxHeight; }

void Map::SetTile(unsigned int x, unsigned int y, char c, MapTileDefinition def)
{
	AddTileDef(c, def);
	SetTile(x, y, c);
}

void Map::SetTile(unsigned int x, unsigned int y, char c)
{
	if (y >= m_Tiles.size() || x >= m_Tiles[y].size())
		return; // Out of bounds

	m_Tiles[y][x] = c;
}

char Map::GetTileChar(unsigned int x, unsigned int y)
{
	if (y >= m_Tiles.size() || x >= m_Tiles[y].size())
		return '\0'; // Out of bounds
	return m_Tiles[y][x];
}

MapTileDefinition& Map::GetTile(unsigned int x, unsigned int y)
{
	assert(y < m_Tiles.size() && x < m_Tiles[y].size());
	return GetTileDef(GetTileChar(x, y));
}

MapTileDefinition& Map::GetTileDef(char c)
{
	assert(HasTileDef(c));
	return m_TileDefinitions[c];
}

bool Map::HasTileDef(char c) { return m_TileDefinitions.find(c) != m_TileDefinitions.end(); }

void Map::AddTileDef(char c, MapTileDefinition def)
{
	if (HasTileDef(c))
		m_TileDefinitions[c] = def;
	else
		m_TileDefinitions.emplace(c, def);
}

void Map::RemoveTileDef(char c)
{
	if (HasTileDef(c))
		m_TileDefinitions.erase(c);
}