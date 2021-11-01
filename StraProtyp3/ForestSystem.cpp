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
					neighbors += _secondaryNeighboringForestCount(i, j, world, "Normal");
					neighbors += _secondaryNeighboringForestCount(i, j, world, "Deep");

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



int ForestSystem::_secondaryNeighboringForestCount(int x, int y, GameworldMatrix& world, ForestType type)
{
	// We need to check 14 Maptiles around x and y.
	// We count them in floating point and return int cast.
	float count = 0.0f;
	float multiplier = 0.2178f;

	// Count the upper row of 5 Maptiles.
	if (x > 1 && y > 1) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x - 2, y - 2, world); m) // Left Up
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
			}
		}
	}
	if (x > 0 && y > 1) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x - 1, y - 2, world); m) // Left Up
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
			}
		}
	}
	if (y > 1) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x, y - 2, world); m) // Left Up
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
			}
		}
	}
	if (x < world.size() && y > 1) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x + 1, y - 2, world); m) // Left Up
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
			}
		}
	}
	if (x < world.size() - 1 && y > 1) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x + 2, y - 2, world); m) // Left Up
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
			}
		}
	}




	// Count the downward row of 5 Maptiles.
	if (x > 1 && y < world[0].size() - 1) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x - 2, y + 2, world); m)
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
			}
		}
	}
	if (x > 1 && y < world[0].size() - 1) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x - 1, y + 2, world); m)
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
			}
		}
	}
	if (y < world[0].size() - 1) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x, y + 2, world); m)
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
			}
		}
	}
	if (x < world.size() && y < world[0].size() - 1) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x + 1, y + 2, world); m)
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
			}
		}
	}if (x < world.size() - 1 && y < world[0].size() - 1) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x + 2, y + 2, world); m)
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
			}
		}
	}



	// Count the left Column of 2 Maptiles.
	if (x > 1) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x - 2, y, world); m)
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
			}
		}
	}
	if (x > 0) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x - 1, y, world); m)
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
			}
		}
	}


	// Count the right Column of 2 Maptiles.
	if (x < world[0].size() - 1) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x + 2, y, world); m)
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
			}
		}
	}
	if (x < world[0].size()) // Ensure array bounds.
	{
		if (auto m = _getForestInMaptile(x + 1, y, world); m)
		{
			auto ptr = m->getComponent<ForestComponent>("Forest");
			if (ptr->GetForestType().compare(type) == 0)
			{
				count += multiplier;
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
			forest->ResetLifetime();
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
			_changeForestType(entity, "Scarce");
		}
	}
	else if (forest->GetForestType().compare("Scarce") == 0)
	{
		auto life = forest->GetLifetime();
		auto maxLife = forest->GetMaxLifetime();

		// Forest becomes a Normal forest.
		if (life > maxLife && (neighborCount == 3 || neighborCount == 4))
		{
			// ...
			_changeForestType(entity, "Normal");
		}
		// Forest dies due to underpopulation.
		else if (life > maxLife && (neighborCount < 2 || neighborCount > 4))
		{
			// ...
			_changeForestType(entity, "Dying");
		}
		// Forest stayes the same, rejuvenating.
		else if (life > maxLife && neighborCount == 2)
		{
			// ...
			forest->ResetLifetime();
		}
	}
	else if (forest->GetForestType().compare("Deep") == 0)
	{
		auto life = forest->GetLifetime();
		auto maxLife = forest->GetMaxLifetime();

		if (forest->IsForestPermanent())
		{
			forest->ResetLifetime();
			return;
		}

		// Forest becomes Normal forest again.
		if (life > maxLife && neighborCount < 5)
		{
			// ...
			_changeForestType(entity, "Normal");
		}
		// Forest keeps on living in its state.
		else if (life > maxLife && neighborCount == 5)
		{
			// ...
			forest->ResetLifetime();
		}
		// Forest dies due to underpopulation.
		else if (life > maxLife && neighborCount > 5)
		{
			// ...
			_changeForestType(entity, "Scarce");
		}
	}
	else if (forest->GetForestType().compare("Dying") == 0)
	{
		auto life = forest->GetLifetime();
		auto maxLife = forest->GetMaxLifetime();

		// Forest rejuvenates.
		if (life > maxLife && neighborCount > 5)
		{
			// ...
			_changeForestType(entity, "Scarce");
		}
		// Forest seize to exist (delete)
		else if (life > maxLife && neighborCount < 6)
		{
			// ...
			world[x][y]->getComponent<MaptileComponent>("Maptile")->RemoveGameobject(entity);
		}
	}

}


bool ForestSystem::_isMaptileSuitableForForest(Pointer<GameObject2> maptile, int x, int y, GameworldMatrix& world)
{
	auto m = world[x][y]->getComponent<MaptileComponent>("Maptile");

	// Check whether maptile is suitable for a Forest.
	if (m->HasCity() ||
		m->HasFort() ||
		m->HasHill() ||
		m->HasMountain() ||
		m->HasImprovement())
	{
		return false;
	}

	return true;
}



void ForestSystem::_checkForRandomForestGeneration(int x, int y, GameworldMatrix& world)
{
	using namespace std;

	// Maptile at {x, y} is guaranteed to not have a Forest.

	auto maptile = world[x][y]->getComponent<MaptileComponent>("Maptile");

	// Check whether maptile is suitable for a Forest.
	if (!_isMaptileSuitableForForest(world[x][y], x, y, world)) return;


	// Get the Biome of the Maptile
	std::string biome = maptile->GetMaptileType();

	
	// Get Neighbors count of current Maptile.
	int neighbors = _neighboringForestCount(x, y, world, "Normal");
	neighbors += _neighboringForestCount(x, y, world, "Deep");

	// Get the Base Probability for Forest.
	float baseProb = _getBaseProbability(biome);

	// Account for Neighbors.
	float neighboringMultiplier = _getNeighborMultiplier(biome, neighbors);
	baseProb += neighboringMultiplier;

	// Account for rivers.
	// ...



	if (Random::InRange(0.0f, 100.0f) <= baseProb)
	{
		// ...
		_createNewForestInMaptile(x, y, world);
		return;
	}

}



float ForestSystem::_getBaseProbability(const std::string& biome)
{
	for (auto p : m_ForestTypeDefinitions)
	{
		if (p->m_Biome.compare(biome) == 0)
		{
			return p->m_BaseProbability;
		}
	}
	
	return 0.0f;
}


float ForestSystem::_getNeighborMultiplier(const std::string& biome, int n)
{
	float neighboringMultiplier = 0.0f;
	for (auto p : m_ForestTypeDefinitions)
	{
		if (p->m_Biome.compare(biome) == 0)
		{
			neighboringMultiplier = n * p->m_NeighborIncrease;
		}
	}

	return neighboringMultiplier;
}


void ForestSystem::_createNewForestInMaptile(int x, int y, GameworldMatrix& world)
{
	// Do another Maptile suitability Check...
	if (!_isMaptileSuitableForForest(world[x][y], x, y, world)) return;


	auto maptile = world[x][y]->getComponent<MaptileComponent>("Maptile");

	Pointer<GameObject2> entity;

	if (maptile->GetMaptileType().compare("Temperate") == 0)
	{
		entity = GameobjectStorage::get()->Instantiate("Temperate_Scarce", x, y);
		_setForestMaxLifetime(entity->getComponent<ForestComponent>("Forest"), "Scarce", "Temperate");
	}
	else if (maptile->GetMaptileType().compare("Tundra") == 0)
	{
		entity = GameobjectStorage::get()->Instantiate("Tundra_Scarce", x, y);
		_setForestMaxLifetime(entity->getComponent<ForestComponent>("Forest"), "Scarce", "Tundra");
	}
	else if (maptile->GetMaptileType().compare("Snow") == 0)
	{
		entity = GameobjectStorage::get()->Instantiate("Snow_Scarce", x, y);
		_setForestMaxLifetime(entity->getComponent<ForestComponent>("Forest"), "Scarce", "Snow");
	}
	else if (maptile->GetMaptileType().compare("Savannah") == 0)
	{
		entity = GameobjectStorage::get()->Instantiate("Savannah_Scarce", x, y);
		_setForestMaxLifetime(entity->getComponent<ForestComponent>("Forest"), "Scarce", "Savannah");
	}
	else if (maptile->GetMaptileType().compare("Jungle") == 0)
	{
		entity = GameobjectStorage::get()->Instantiate("Jungle_Scarce", x, y);
		_setForestMaxLifetime(entity->getComponent<ForestComponent>("Forest"), "Scarce", "Jungle");
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
	}
	else if (t.compare("Normal") == 0)
	{
		decal += "normal";
	}
	else if (t.compare("Scarce") == 0)
	{
		decal += "scarce";
	}
	else if (t.compare("Dying") == 0)
	{
		decal += "dying";
	}


	_setForestMaxLifetime(forest, t, biome);
	forest->SetForestType(t);
	render->SetDecalName(decal);
	forest->ResetLifetime();
}


void ForestSystem::_setForestMaxLifetime(Pointer<ForestComponent> forest, const ForestType& t, const ForestBiome& b)
{
	using namespace std;

	for (auto ptr : m_ForestTypeDefinitions)
	{
		if (ptr->m_Biome.compare(b) == 0)
		{

			int lifetime = ptr->m_MaxLifetime[t];
			forest->SetMaxLifetime(lifetime);
			return;
		}
	}
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
		while (g_ForestSystem->m_ForestTypeDefinitions.size() > 0)
		{
			g_ForestSystem->m_ForestTypeDefinitions[0].reset();
			g_ForestSystem->m_ForestTypeDefinitions.erase(g_ForestSystem->m_ForestTypeDefinitions.begin());
		}


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


	XMLElement* jungle = root->FirstChildElement("Jungle");
	XMLElement* temperate = root->FirstChildElement("Temperate");
	XMLElement* snow = root->FirstChildElement("Snow");
	XMLElement* savannah = root->FirstChildElement("Savannah");
	XMLElement* tundra = root->FirstChildElement("Tundra");


	// JUNGLE
	auto ptr = std::make_shared<ForestTypeDefinition>();
	ptr->m_Biome = "Jungle";
	ptr->m_BaseProbability = jungle->FirstChildElement("MaptileBaseProbability")->FloatText();
	ptr->m_NeighborIncrease = jungle->FirstChildElement("NeighborProbabilityIncrease")->FloatText();
	ptr->m_RiverIncrease = jungle->FirstChildElement("RiverProbabilityIncrease")->FloatText();

	XMLElement* lifetime = jungle->FirstChildElement("MaxLifetime");
	XMLElement* def = lifetime->FirstChildElement("Def");
	while (def)
	{
		ptr->m_MaxLifetime.emplace(def->Attribute("type"), def->IntText());

		def = def->NextSiblingElement("Def");
	}

	m_ForestTypeDefinitions.push_back(ptr);



	// TEMPERATE
	ptr = std::make_shared<ForestTypeDefinition>();
	ptr->m_Biome = "Temperate";
	ptr->m_BaseProbability = temperate->FirstChildElement("MaptileBaseProbability")->FloatText();
	ptr->m_NeighborIncrease = temperate->FirstChildElement("NeighborProbabilityIncrease")->FloatText();
	ptr->m_RiverIncrease = temperate->FirstChildElement("RiverProbabilityIncrease")->FloatText();

	lifetime = temperate->FirstChildElement("MaxLifetime");
	def = lifetime->FirstChildElement("Def");
	while (def)
	{
		ptr->m_MaxLifetime.emplace(def->Attribute("type"), def->IntText());

		def = def->NextSiblingElement("Def");
	}

	m_ForestTypeDefinitions.push_back(ptr);




	// SAVANNAH
	ptr = std::make_shared<ForestTypeDefinition>();
	ptr->m_Biome = "Savannah";
	ptr->m_BaseProbability = savannah->FirstChildElement("MaptileBaseProbability")->FloatText();
	ptr->m_NeighborIncrease = savannah->FirstChildElement("NeighborProbabilityIncrease")->FloatText();
	ptr->m_RiverIncrease = savannah->FirstChildElement("RiverProbabilityIncrease")->FloatText();

	lifetime = savannah->FirstChildElement("MaxLifetime");
	def = lifetime->FirstChildElement("Def");
	while (def)
	{
		ptr->m_MaxLifetime.emplace(def->Attribute("type"), def->IntText());

		def = def->NextSiblingElement("Def");
	}

	m_ForestTypeDefinitions.push_back(ptr);





	// SNOW
	ptr = std::make_shared<ForestTypeDefinition>();
	ptr->m_Biome = "Snow";
	ptr->m_BaseProbability = snow->FirstChildElement("MaptileBaseProbability")->FloatText();
	ptr->m_NeighborIncrease = snow->FirstChildElement("NeighborProbabilityIncrease")->FloatText();
	ptr->m_RiverIncrease = snow->FirstChildElement("RiverProbabilityIncrease")->FloatText();

	lifetime = snow->FirstChildElement("MaxLifetime");
	def = lifetime->FirstChildElement("Def");
	while (def)
	{
		ptr->m_MaxLifetime.emplace(def->Attribute("type"), def->IntText());

		def = def->NextSiblingElement("Def");
	}

	m_ForestTypeDefinitions.push_back(ptr);




	// TUNDRA
	ptr = std::make_shared<ForestTypeDefinition>();
	ptr->m_Biome = "Tundra";
	ptr->m_BaseProbability = tundra->FirstChildElement("MaptileBaseProbability")->FloatText();
	ptr->m_NeighborIncrease = tundra->FirstChildElement("NeighborProbabilityIncrease")->FloatText();
	ptr->m_RiverIncrease = tundra->FirstChildElement("RiverProbabilityIncrease")->FloatText();

	lifetime = tundra->FirstChildElement("MaxLifetime");
	def = lifetime->FirstChildElement("Def");
	while (def)
	{
		ptr->m_MaxLifetime.emplace(def->Attribute("type"), def->IntText());

		def = def->NextSiblingElement("Def");
	}

	m_ForestTypeDefinitions.push_back(ptr);


	m_DefinitionFilepath = filepath;
	return true;
}


bool ForestSystem::ReloadDefinition()
{
	using namespace std;

	while (m_ForestTypeDefinitions.size() > 0)
	{
		m_ForestTypeDefinitions[0].reset();
		m_ForestTypeDefinitions.erase(m_ForestTypeDefinitions.begin());
	}

	if (Initialize(m_DefinitionFilepath))
	{

		cout << color(colors::GREEN);
		cout << "[ForestSystem::ReloadDefinition] Success" << white << endl;
		return true;
	}
}