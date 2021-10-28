#pragma once

#include "GameObjectStorage.h"

using GameworldMatrix = std::vector< std::vector< Pointer<GameObject2> >>;

class ForestSystem
{
public:
	static ForestSystem* get();
	static void del();

	

	void Update(GameworldMatrix& world);


private:
	static ForestSystem* g_ForestSystem;
	
	ForestSystem()
	{
	}


};