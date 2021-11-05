#pragma once

#include "GameObjectStorage.h" // Include all Components and GameObject2 Definitions.


class NationSystem
{
public:
	static NationSystem* get();
	static void del();


	// Load Nation definitions from XML File.
	bool Initialize(std::string filepath);


	// Set player to control a nation from nations initialized.
	void AssignNationToPlayer(Pointer<GameObject2> player, NationID nation);



private:
	static NationSystem* g_NationSystem;

	std::vector< Pointer< GameObject2 > > m_Nations;

	std::string m_DefinitionPath = "Data/NationSystem.xml";

private:
	NationSystem() {}
};
