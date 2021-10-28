#include "ForestSystem.h"

ForestSystem* ForestSystem::g_ForestSystem = nullptr;


void ForestSystem::Update(GameworldMatrix& world)
{

	for (int i = 0; i < world.size(); i++)
	{
		for (int j = 0; j < world[i].size(); j++)
		{
			if (world[i][j]->hasComponent("Forest"))
			{







			}
		}
	}


}


ForestSystem* ForestSystem::get()
{
	if (!g_ForestSystem)
	{
		g_ForestSystem = new ForestSystem();
	}


	return g_ForestSystem;
}


void ForestSystem::del()
{
	if (g_ForestSystem)
	{
		delete g_ForestSystem;
	}
}