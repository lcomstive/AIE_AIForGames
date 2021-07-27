#pragma once
#include <memory>
#include <Map.hpp>
#include <Animals/Animal.hpp>
#include <Animals/Predator.hpp>
#include <Framework/GameObject.hpp>
#include <Framework/GameObjects/Sprite.hpp>
#include <Framework/Pathfinding/PathFindingGrid.hpp>

using SquareGridNode = Framework::Pathfinding::SquareGridNode;
using PathfindingGrid = Framework::Pathfinding::Grid<SquareGridNode>;

class Game
{
	struct CreatureInfo
	{
		float Speed;
		Texture2D SpriteSheet;
		FoodClass FoodSource;
		Framework::Vec2 Size;
		unsigned int MaxFrames;
		Framework::Vec2 SpriteSize;
		Framework::Vec2 SpriteOffset;

		CreatureInfo(const Texture2D& texture);
		CreatureInfo(std::string filepath);
		~CreatureInfo();

	private:
		bool m_CreatedTexture;
	};

	std::vector<CreatureInfo> m_CreatureInfos;
	std::vector<Animal*> m_Creatures;

	const float MinZoom = 0.05f;
	const float MaxZoom = 2.50f;
	const float ZoomSpeed = 10.0f;

	const float GridCellSize = 25;

	Map m_Map;
	Camera2D m_Camera;
	Framework::GameObject* m_Root;
	Framework::GameObject* m_Background;
	std::unique_ptr<PathfindingGrid> m_PathfindingGrid;

	// Background Tiles
	Texture m_BackgroundSheet;
	Texture m_SlimeSpriteSheet;
	Texture m_SkeletonSpriteSheet;

	void CreateMap();
	void CreateCreatureInfos();

	Framework::GameObject* SpawnRandomCreature(Framework::Vec2 position, int index = -1);
	Framework::GameObject* AddBackgroundTileWaterEdge(unsigned int x, unsigned int y);
	Framework::GameObject* AddBackgroundTile(unsigned int x, unsigned int y, char c, int spriteIndex = -1);

public:
	Game();
	~Game();

	void Run();
	void Update();
	void DrawUI();

	void PrePhysicsUpdate();
	void PostPhysicsUpdate();

	static const float ThirstPerSecond;
	static const float HungerPerSecond;
	static const float HealthDecayPerSecond;
};