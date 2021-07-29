#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <Framework/Vec2.hpp>

struct MapTileDefinition
{
	std::string Name;
	std::vector<Framework::Vec2> SpritesheetOffsets; // Many offsets, will be chosen at random
};

class Map
{
	static std::unordered_map<char, MapTileDefinition> m_TileDefinitions;

	unsigned int m_MaxWidth, m_MaxHeight;
	std::vector<std::vector<char>> m_Tiles;

public:
	Map();
	~Map();

	void Load(std::string filepath);
	void Unload();

	unsigned int GetWidth();
	unsigned int GetHeight();

	void SetTile(unsigned int x, unsigned int y, char c);
	void SetTile(unsigned int x, unsigned int y, char c, MapTileDefinition);

	char GetTileChar(unsigned int x, unsigned int y);
	MapTileDefinition& GetTile(unsigned int x, unsigned int y);

	static bool HasTileDef(char c);
	static void RemoveTileDef(char c);
	static MapTileDefinition& GetTileDef(char c);
	static void AddTileDef(char c, MapTileDefinition def);
};