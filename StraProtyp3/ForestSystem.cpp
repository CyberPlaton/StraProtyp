#include "ForestSystem.h"

ForestSystem* ForestSystem::g_ForestSystem = nullptr;


void ForestSystem::Update(GameworldMatrix& world)
{
	// Go through all maptiles.
	for (int i = 0; i < world.size(); i++)
	{
		for (int j = 0; j < world[i].size(); j++)
		{
			bool doesForestExist = false;
			// Go through each entity on the maptile.
			for (auto entity : world[i][j]->getComponent<MaptileComponent>("Maptile")->GetGameobjects())
			{
				// Pick out the Forests.
				if (entity->hasComponent("Forest"))
				{
					doesForestExist = true;

					auto forest = entity->getComponent<ForestComponent>("Forest");
					
					// Standard Forest rule checking

					// Increment Lifetime of forest.
					_incrementForestLifetime(forest);

					// Check for Forest type Transition.
					int neighbors = _neighboringForestCount(i, j, world, "Normal");
					neighbors += _neighboringForestCount(i, j, world, "Deep");
					_checkForForestTransition(entity, neighbors, i, j, world);
				}
			}


			if (!doesForestExist)
			{
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
	if (x < 0 || y < 0 || x >= world.size() || y >= world[0].size()) return nullptr;


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

	if (x > 0 && y > 0) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x - 1, y - 1, world); m) // Left Up
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count++;
			}
		}
	}

	if (x > 0)
	{
		if (auto m = _getForestInMaptile(x - 1, y, world); m) // Left
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count++;
			}
		}
	}


	if (y > 0)
	{
		if (auto m = _getForestInMaptile(x, y - 1, world); m) // Up
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count++;
			}
		}
	}

	if (x < world.size() && y < world[0].size())
	{
		if (auto m = _getForestInMaptile(x + 1, y + 1, world); m) // Right Down
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count++;
			}
		}
	}


	if (x < world.size())
	{
		if (auto m = _getForestInMaptile(x + 1, y, world); m) // Right
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count++;
			}
		}
	}


	if (y < world[0].size())
	{
		if (auto m = _getForestInMaptile(x, y + 1, world); m) // Down
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count++;
			}
		}
	}


	if (x > 0 && y < world[0].size())
	{
		if (auto m = _getForestInMaptile(x - 1, y + 1, world); m) // Left Down
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count++;
			}
		}
	}

	if (x < world.size() && y > 0)
	{
		if (auto m = _getForestInMaptile(x + 1, y - 1, world); m) // Right Up
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count++;
			}
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
		else if (life > maxLife && neighborCount == 4)
		{
			// ..
			_changeForestType(entity, "Deep");
		}
		// Forest dies on overpopulation.
		else if (life > maxLife && neighborCount > 4)
		{
			// ..
			_changeForestType(entity, "Dying");
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
		if (life > maxLife && neighborCount >= 2)
		{
			// ...
			_changeForestType(entity, "Normal");
		}
		// Forest dies due to underpopulation.
		else if (life > maxLife && neighborCount < 2)
		{
			// ...
			_changeForestType(entity, "Dying");
		}
	}
	else if (forest->GetForestType().compare("Deep") == 0)
	{
		auto life = forest->GetLifetime();
		auto maxLife = forest->GetMaxLifetime();

		if (forest->IsForestPermanent())
		{
			forest->ResetLifetime();
		}

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
			forest->DecrementLifetime();
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
		_createNewForestInMaptile(x, y, world);
		return;
	}


	neighbors = _neighboringForestCount(x, y, world, "Deep");
	if (neighbors > 0 && Random::InRange(0.0f, 100.0f) <= 5.0f)
	{
		// ...
		_createNewForestInMaptile(x, y, world);
		return;
	}


}



void ForestSystem::_createNewForestInMaptile(int x, int y, GameworldMatrix& world)
{
	auto maptile = world[x][y]->getComponent<MaptileComponent>("Maptile");

	if (maptile->GetMaptileType().compare("temperate") == 0)
	{
		GameobjectStorage::get()->Instantiate("Temperate_Scarce", x, y);
	}
	else if (maptile->GetMaptileType().compare("tundra") == 0)
	{
		GameobjectStorage::get()->Instantiate("Tundra_Scarce", x, y);
	}
	else if (maptile->GetMaptileType().compare("snow") == 0)
	{
		GameobjectStorage::get()->Instantiate("Snow_Scarce", x, y);
	}
	else if (maptile->GetMaptileType().compare("savannah") == 0)
	{
		GameobjectStorage::get()->Instantiate("Savannah_Scarce", x, y);
	}
	else if (maptile->GetMaptileType().compare("jungle") == 0)
	{
		GameobjectStorage::get()->Instantiate("Jungle_Scarce", x, y);
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
		return false;
	}


	neighbors = _neighboringForestCount(x, y, world, "Deep");
	if (neighbors > 1)
	{
		// ...
		return false;
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
		forest->SetMaxLifetime(m_DeepForestDefaultMaxLifetime);
	}
	else if (t.compare("Normal") == 0)
	{
		decal += "normal";
		forest->SetMaxLifetime(m_NormalForestDefaultMaxLifetime);
	}
	else if (t.compare("Scarce") == 0)
	{
		decal += "scarce";
		forest->SetMaxLifetime(m_ScarceForestDefaultMaxLifetime);
	}
	else if (t.compare("Dying") == 0)
	{
		decal += "dying";
		forest->SetMaxLifetime(m_DyingForestDefaultMaxLifetime);
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
		if (!g_ForestSystem->Initialize("Data/ForestSystem.xml"))
		{
			delete g_ForestSystem;
			g_ForestSystem = NULL;
		}
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



bool ForestSystem::Initialize(const std::string& filepath)
{
	using namespace tinyxml2;

	// Create an object defined from xml.
	tinyxml2::XMLDocument doc;


	XMLError result = doc.LoadFile(filepath.c_str());
	if (result != XMLError::XML_SUCCESS)
	{
		// Clean up.
		doc.Clear();
		return false;
	}

	XMLNode* root = doc.FirstChild();
	if (root == nullptr)
	{
		// Clean up.
		doc.Clear();
		return false;
	}




	return true;
}