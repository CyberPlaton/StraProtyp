#include "ForestSystem.h"

ForestSystem* ForestSystem::g_ForestSystem = nullptr;


void ForestSystem::Update(GameworldMatrix& world)
{
	// Go through all maptiles.
	for (int i = 0; i < world.size(); i++)
	{
		for (int j = 0; j < world[i].size(); j++)
		{

			// Go through each entity on the maptile.
			for (auto entity : world[i][j]->getComponent<MaptileComponent>("Maptile")->GetGameobjects())
			{

				// Pick out the Forests.
				if (entity->hasComponent("Forest"))
				{

					auto forest = entity->getComponent<ForestComponent>("Forest");
					
					// Standard Forest rule checking

					// Increment Lifetime of forest.
					_incrementForestLifetime(forest);

					// Check for Forest type Transition.
					int neighbors = _neighboringForestCount(i, j, world, "Normal");
					neighbors += _neighboringForestCount(i, j, world, "Deep");
					_checkForForestTransition(entity, neighbors, i, j, world);
				}


				// Try generate a new Forest at given location.
				if (!_checkForForestGeneration(i, j, world))
				{
					// Check for Random Forest generation around existing Forests.
					_checkForRandomForestGeneration(i, j, world);
				}


			}
		}
	}


}



void ForestSystem::_incrementForestLifetime(Pointer<ForestComponent> forest)
{
	forest->IncrementLifetime();
}


Pointer<GameObject2> ForestSystem::_getForestInMaptile(int x, int y, GameworldMatrix& world)
{
	for (auto entity : world[x][y]->getComponent<MaptileComponent>("Maptile")->GetGameobjects())
	{
		if (entity->hasComponent("Forest"))
		{
			return entity;
		}
	}

	return nullptr;
}


int ForestSystem::_neighboringForestCount(int x, int y, GameworldMatrix& world, ForestType type)
{
	// Checking for 8 direct neighboring Maptiles around {x, y}.

	int count = 0;
	if (auto m = _getForestInMaptile(x - 1, y - 1, world); m) // Left Up
	{
		auto ptr = m->getComponent<ForestComponent>("Forest");
		if (ptr->GetForestType().compare(type) == 0)
		{
			count++;
		}
	}


	if (auto m = _getForestInMaptile(x - 1, y, world); m) // Left
	{
		auto ptr = m->getComponent<ForestComponent>("Forest");
		if (ptr->GetForestType().compare(type) == 0)
		{
			count++;
		}
	}


	if (auto m = _getForestInMaptile(x, y - 1, world); m) // Up
	{
		auto ptr = m->getComponent<ForestComponent>("Forest");
		if (ptr->GetForestType().compare(type) == 0)
		{
			count++;
		}
	}


	if (auto m = _getForestInMaptile(x + 1, y + 1, world); m) // Right Down
	{
		auto ptr = m->getComponent<ForestComponent>("Forest");
		if (ptr->GetForestType().compare(type) == 0)
		{
			count++;
		}
	}


	if (auto m = _getForestInMaptile(x + 1, y, world); m) // Right
	{
		auto ptr = m->getComponent<ForestComponent>("Forest");
		if (ptr->GetForestType().compare(type) == 0)
		{
			count++;
		}
	}


	if (auto m = _getForestInMaptile(x, y + 1, world); m) // Down
	{
		auto ptr = m->getComponent<ForestComponent>("Forest");
		if (ptr->GetForestType().compare(type) == 0)
		{
			count++;
		}
	}


	if (auto m = _getForestInMaptile(x - 1, y + 1, world); m) // Left Down
	{
		auto ptr = m->getComponent<ForestComponent>("Forest");
		if (ptr->GetForestType().compare(type) == 0)
		{
			count++;
		}
	}


	if (auto m = _getForestInMaptile(x + 1, y - 1, world); m) // Right Up
	{
		auto ptr = m->getComponent<ForestComponent>("Forest");
		if (ptr->GetForestType().compare(type) == 0)
		{
			count++;
		}
	}


	return count;
}


void ForestSystem::_checkForForestTransition(Pointer<GameObject2> entity, int neighborCount, int x, int y, GameworldMatrix& world)
{
	auto forest = entity->getComponent<ForestComponent>("Forest");


	if (forest->GetForestType().compare("Normal") == 0)
	{
		auto life = forest->GetLifetime();
		auto maxLife = forest->GetMaxLifetime();

		// Forest keeps on living in its state.
		if (life > maxLife && neighborCount == 3)
		{
			forest->DecrementLifetime();
		}
		// Forest becomes Deep.
		else if (life > maxLife && neighborCount > 3)
		{
			// ..
			_changeForestType(entity, "Deep");
		}
		// Forest dies due to underpopulation.
		else if (life > maxLife && neighborCount < 3)
		{
			// ...
			_changeForestType(entity, "Dying");
		}
	}
	else if (forest->GetForestType().compare("Scarce") == 0)
	{
		auto life = forest->GetLifetime();
		auto maxLife = forest->GetMaxLifetime();

		// Forest becomes a Normal forest.
		if (life > maxLife && neighborCount >= 3)
		{
			// ...
			_changeForestType(entity, "Normal");
		}
		// Forest dies due to underpopulation.
		else if (life > maxLife && neighborCount < 3)
		{
			// ...
			_changeForestType(entity, "Dying");
		}
	}
	else if (forest->GetForestType().compare("Deep") == 0)
	{
		auto life = forest->GetLifetime();
		auto maxLife = forest->GetMaxLifetime();

		// Forest becomes Normal forest again.
		if (life > maxLife && neighborCount == 3)
		{
			// ...
			_changeForestType(entity, "Normal");
		}
		// Forest keeps on living in its state.
		else if (life > maxLife && neighborCount > 3)
		{
			// ...
		}
		// Forest dies due to underpopulation.
		else if (life > maxLife && neighborCount < 3)
		{
			// ...
			_changeForestType(entity, "Dying");
		}
	}
	else if (forest->GetForestType().compare("Dying") == 0)
	{
		auto life = forest->GetLifetime();
		auto maxLife = forest->GetMaxLifetime();

		// Forest rejuvenates.
		if (life > maxLife && neighborCount == 3)
		{
			// ...
			_changeForestType(entity, "Normal");
		}
		// Forest seize to exist (delete)
		else if (life > maxLife && neighborCount > 3)
		{
			// ...
			world[x][y]->getComponent<MaptileComponent>("Maptile")->RemoveGameobject(entity);
		}
		// Forest seize to exist (delete)
		else if (life > maxLife && neighborCount < 3)
		{
			// ...
			world[x][y]->getComponent<MaptileComponent>("Maptile")->RemoveGameobject(entity);
		}
	}

}


void ForestSystem::_checkForRandomForestGeneration(int x, int y, GameworldMatrix& world)
{
	// Maptile at {x, y} is guaranteed to not have a Forest.

	// Rules:
	// 1) 1% Probability of creating a new Scarce Forest around any number of Normal Forests.
	// 2) 5% Probability of creating a new Scarce Forest around any number of Deep Forests.
	// ...

	int neighbors = _neighboringForestCount(x, y, world, "Normal");
	if (neighbors > 0 && Random::InRange(0.0f, 100.0f) <= 1.0f)
	{
		// ...
	}


	neighbors = _neighboringForestCount(x, y, world, "Deep");
	if (neighbors > 0 && Random::InRange(0.0f, 100.0f) <= 5.0f)
	{
		// ...
	}


}



bool ForestSystem::_checkForForestGeneration(int x, int y, GameworldMatrix& world)
{
	// Maptile at {x, y} is guaranteed to not have a Forest.

	// Rules:
	// 1) Exactly 3 Normal Forests around: New Scarce Forest
	// 2) More than 1 Deep Forests around: New Scarce Forest.

	int neighbors = _neighboringForestCount(x, y, world, "Normal");
	if (neighbors == 3)
	{
		// ...
		return true;
	}


	neighbors = _neighboringForestCount(x, y, world, "Deep");
	if (neighbors > 1)
	{
		// ...
		return true;
	}


	return false;
}


void ForestSystem::_changeForestType(Pointer<GameObject2> entity, const ForestType& t)
{
	auto forest = entity->getComponent<ForestComponent>("Forest");
	auto render = entity->getComponent<RenderableComponent>("Renderable");
	std::string decal = "forest_";


	// Change the type and set the decal,
	// Reset the lifetime and set max lifetime accordingly.
	ForestBiome biome = forest->GetForestBiome();
	if (biome.compare("Jungle") == 0)
	{
		decal += "jungle_";
	}
	else if (biome.compare("Tundra") == 0)
	{
		decal += "tundra_";
	}
	else if (biome.compare("Temperate") == 0)
	{
		decal += "temperate_";
	}
	else if (biome.compare("Savannah") == 0)
	{
		decal += "savannah_";
	}
	else if (biome.compare("Snow") == 0)
	{
		decal += "snow_";
	}



	if (t.compare("Deep") == 0)
	{
		decal += "deep";
		forest->SetMaxLifetime(100);
	}
	else if (t.compare("Normal") == 0)
	{
		decal += "normal";
		forest->SetMaxLifetime(50);
	}
	else if (t.compare("Scarce") == 0)
	{
		decal += "scarce";
		forest->SetMaxLifetime(25);
	}
	else if (t.compare("Dying") == 0)
	{
		decal += "dying";
		forest->SetMaxLifetime(25);
	}


	forest->SetForestType(t);
	render->SetDecalName(decal);
	forest->ResetLifetime();
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