#pragma once

#include "GameObjectStorage.h" // Include all Components and GameObject2 Definitions.
#include "Random.h" // Random number generator.


using GameworldMatrix = std::vector< std::vector< Pointer<GameObject2> >>;


class ForestSystem
{
	struct ForestTypeDefinition
	{
		ForestBiome m_Biome;
		std::map< ForestType, int > m_MaxLifetime;
		float m_BaseProbability = 0.0f;
		float m_NeighborIncrease = 0.0f;
		float m_RiverIncrease = 0.0f;
	};


public:
	static ForestSystem* get();
	static void del();


	bool Initialize(std::string filepath);
	void Update(GameworldMatrix& world);
	bool ReloadDefinition();

private:
	static ForestSystem* g_ForestSystem;
	
	std::string m_DefinitionFilepath;
	std::vector< Pointer< ForestTypeDefinition > > m_ForestTypeDefinitions;
		
private:

	ForestSystem()
	{
	}

	// Check whether the given maptile is per definition suitable for a new Forest of any type.
	bool _isMaptileSuitableForForest(Pointer<GameObject2> maptile, int x, int y, GameworldMatrix& world);


	// Return the new-forest-probability-multiplier of a river in given biome.
	float _getRiverProbabilityMultiplier(std::string biome);


	// Return the base probability value to generate a Forest in given Biome.
	float _getBaseProbability(std::string biome);


	// Return the probability multiplier for n neighboring Normal/Deep forests
	// for given biome.
	float _getNeighborMultiplier(std::string biome, int n);


	void _incrementForestLifetime(Pointer<ForestComponent> forest);


	// Return number of Forest with given type directly
	// around x and y coords.
	int _neighboringForestCount(int x, int y, GameworldMatrix& world, ForestType type);


	// Return number of Forest with given type 1 Tile away around
	// x and y coords.
	int _secondaryNeighboringForestCount(int x, int y, GameworldMatrix& world, ForestType type);


	// Execute logic for Transitioning of Forest types and
	// deleting forests which have died.
	void _checkForForestTransition(Pointer<GameObject2> entity, int neighborCount, int x, int y, GameworldMatrix& world);


	// Execute logic for randomly generating a new Forest at given coords.
	void _checkForRandomForestGeneration(int x, int y, GameworldMatrix& world);


	// Execute logic for standard generating a new Forest at given coords.
	// Returns True if a new Forest was generating.
	bool _checkForForestGeneration(int x, int y, GameworldMatrix& world);


	// Change the Type of the forest.
	// Resetting the Lifetime, setting max lifetime and decal accordingly.
	void _changeForestType(Pointer<GameObject2> entity, ForestType t);


	// Return the Forest Gameobject on given Maptile.
	// If none exists a nullptr is returned.
	Pointer<GameObject2> _getForestInMaptile(int x, int y, GameworldMatrix& world);


	void _createNewForestInMaptile(int x, int y, GameworldMatrix& world);


	// Setting max Lifetime of a forest according to its type and biome.
	void _setForestMaxLifetime(Pointer<ForestComponent> forest, ForestType t, ForestBiome b);

};