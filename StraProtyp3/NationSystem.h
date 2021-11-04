#pragma once

#include "GameObjectStorage.h" // Include all Components and GameObject2 Definitions.


class NationSystem
{
public:
	static NationSystem* get();
	static void del();


	// Load Nation definitions from XML File.
	bool Initialize(std::string filepath);


private:
	static NationSystem* g_NationSystem;

	std::vector< Pointer< GameObject2 > > m_Nations;

	std::string m_DefinitionPath = "Data/NationSystem.xml";

private:
	NationSystem() {}
};
