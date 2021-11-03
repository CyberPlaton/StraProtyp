#pragma once

#include "GameObjectStorage.h" // Include all Components and GameObject2 Definitions.


class TechnologySystem
{
public:
	static TechnologySystem* get();
	static void del();

	bool Initialize(std::string filepath);

private:
	static TechnologySystem* g_TechnologySystem;


	std::vector< Pointer< GameObject2 > > m_Technologies;


private:
	TechnologySystem(){}
};
