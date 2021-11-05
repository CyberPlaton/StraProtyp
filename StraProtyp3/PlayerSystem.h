#pragma once

#include "GameObjectStorage.h" // Include all Components and GameObject2 Definitions.

/*
* Player system is used to create units, cities, improvement etc for player,
* as well as checking whether technology requirements for those are met.
* Basically every game mechanic which is related to player action or input
* is inside this class.
*/
class PlayerSystem
{
public:
	static PlayerSystem* get();
	static void del();


	// First time initialization with creation of players defined in a XML.
	bool Initialize(std::string filepath) { return false; }

private:
	static PlayerSystem* g_PlayerSystem;

	// All Players in Game.
	std::vector< Pointer< GameObject2 > > m_Players;

private:
	PlayerSystem() {}
};