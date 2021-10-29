#pragma once

#include "GameObjectStorage.h" // Include all Components and GameObject2 Definitions.
#include "Random.h" // Random number generator.


using GameworldMatrix = std::vector< std::vector< Pointer<GameObject2> >>;

class ForestSystem
{
public:
	static ForestSystem* get();
	static void del();

	

	void Update(GameworldMatrix& world);


private:
	static ForestSystem* g_ForestSystem;
	

	
private:

	ForestSystem()
	{
	}

	void _incrementForestLifetime(Pointer<ForestComponent> forest);

	// Return number of Forest with type of Normal or Deep directly
	// around x and y coords.
	int _neighboringForestCount(int x, int y, GameworldMatrix& world, ForestType type);

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
	void _changeForestType(Pointer<GameObject2> entity, const ForestType& t);


	// Return the Forest Gameobject on given Maptile.
	// If none exists a nullptr is returned.
	Pointer<GameObject2> _getForestInMaptile(int x, int y, GameworldMatrix& world);
};