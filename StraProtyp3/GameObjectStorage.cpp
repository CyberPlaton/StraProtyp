#include "GameObjectStorage.h"

bool GameobjectStorage::DestroyGameobject(Pointer<GameObject2> entity)
{
	using namespace std;

	cout << color(colors::DARKYELLOW);
	cout << "[GameobjectStorage::DestroyGameobject] Destroy Entity \""<< entity->getName() <<"\"{"<< entity->getTag() << "} " << white << endl;

	// Delete Gameobject.
	if (entity.use_count() > 1)
	{
		cout << color(colors::DARKRED);
		cout << "\tGameobject \"" << entity->getName() << "\" has " << entity.use_count() << " \"Pointers\" in use" << white << endl;
	}
	else
	{
		cout << color(colors::DARKGREEN);
		cout << "\tGameobject \"" << entity->getName() << "\" has not \"Pointers\" in use" << white << endl;
	}




	/*
	// Remove entry from storage
	for (int index = 0; index < gameObjectStorage.size(); index++)
	{
		if (entity->getTag().compare(gameObjectStorage[index]->getTag()) == 0)
		{
			cout << "Storage Size: " << gameObjectStorage.size() << endl;
			cout << "Erase Entity at: " << index << endl;

			// Erase to be destroyed Gameobject from Storage.
			gameObjectStorage.erase(gameObjectStorage.begin() + index);
			break;
		}
		else if (gameObjectStorage[index] == nullptr)
		{
			cout << color(colors::RED);
			cout << "\t\tErasing Invalid Gameobject from Storage!" << white << endl;

			// While searching erase any invalid Gameobjects on our path.
			gameObjectStorage.erase(gameObjectStorage.begin() + index);

			index = index - 1;

			continue;
		}
	}
	*/

	entity.reset(); // Destroy Gameobject Pointer self
	return true;
}


bool GameobjectStorage::DestroyGameobject(std::string tag)
{
	using namespace std;

	/*
	for (auto& entity : gameObjectStorage)
	{
		if (entity->getTag().compare(tag) == 0)
		{
			// Delete Gameobject.
			return DestroyGameobject(entity);
		}
	}
	*/

	return DestroyGameobject(gameObjectStorage[_getHashFromGameobjectTag(tag)]);

	return false;
}


Pointer<GameObject2> GameobjectStorage::Instantiate(std::string prefabName, float xpos, float ypos)
{
	using namespace std;
	using namespace tinyxml2;


	if (prefabStorage.find(prefabName) != prefabStorage.end())
	{
		Pointer<GameObject2> ptr;


		// Create an object defined from xml.
		tinyxml2::XMLDocument doc;


		XMLError result = doc.LoadFile(prefabStorage[prefabName].c_str());
		if (result != XMLError::XML_SUCCESS)
		{
			// Clean up.
			ptr.reset();
			doc.Clear();
			return nullptr;
		}


		XMLNode* root = doc.FirstChild();
		if (root == nullptr)
		{
			// Clean up.
			ptr.reset();
			doc.Clear();
			return nullptr;
		}


		// Create a Gameobject.
		std::string tag, name;
		size_t gameObjectNumber = ++IGameobjectStorage::g_GameobjectCount;
		tag = "GO_" + std::to_string(gameObjectNumber) + "_";
		tag += root->FirstChildElement("Tag")->GetText();
		name = root->FirstChildElement("Name")->GetText();

		ptr = std::make_shared<GameObject2>(tag, name, gameObjectNumber);

		cout << color(colors::GREEN);
		cout << "Instantiate Gameobject \"" << name << "\"{" << tag << "}" << white << endl;


		// Get the defined components.
		XMLElement* components = root->FirstChildElement("Components");

		// Get first component
		XMLElement* cmp = components->FirstChildElement("Component");
		bool panik = false;
		while (cmp)
		{
			ComponentType componentType;
			componentType = cmp->Attribute("name");


			// Here we make the "Deep-Copy",
			// where we more just create the new component and 
			if (componentType.compare("Transform") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Transform" << white << endl;

				auto transform = std::make_shared<TransformComponent>(name + "_Transform");
				ptr->addComponent(transform);
				_addTransformComponent(transform, ptr, cmp, xpos, ypos);
				transform.reset();

			}
			else if (componentType.compare("Forest") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Forest" << white << endl;


				auto forest = std::make_shared<ForestComponent>(name + "Forest");
				ptr->addComponent(forest);
				_addForestComponent(forest, ptr, cmp, xpos, ypos);
				forest.reset();

			}
			else if (componentType.compare("City") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding City" << white << endl;


				auto city = std::make_shared<CityComponent>(name + "City", ptr);
				ptr->addComponent(city);
				_addCityComponent(city, ptr, cmp, xpos, ypos);
				city.reset();

			}
			else if (componentType.compare("Unit") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Unit" << white << endl;


				auto unit = std::make_shared<UnitComponent>(name + "Unit");
				ptr->addComponent(unit);
				_addUnitComponent(unit, ptr, cmp, xpos, ypos);
				unit.reset();

			}
			else if (componentType.compare("Mountain") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Mountain" << white << endl;


				auto mountain = std::make_shared<MountainComponent>(name + "Mountain");
				ptr->addComponent(mountain);
				_addMountainComponent(mountain, ptr, cmp, xpos, ypos);
				mountain.reset();
			}
			else if (componentType.compare("River") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding River" << white << endl;


				auto river = std::make_shared<RiverComponent>(name + "River");
				ptr->addComponent(river);
				_addRiverComponent(river, ptr, cmp, xpos, ypos);
				river.reset();
			}
			else if (componentType.compare("Road") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Road" << white << endl;


				auto road = std::make_shared<RoadComponent>(name + "Road");
				ptr->addComponent(road);
				_addRoadComponent(road, ptr, cmp, xpos, ypos);
				road.reset();
			}
			else if (componentType.compare("Ressource") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Ressource" << white << endl;


				auto ressource = std::make_shared<RessourceComponent>(name + "Ressource");
				ptr->addComponent(ressource);
				_addRessourceComponent(ressource, ptr, cmp);
				ressource.reset();
			}
			else if (componentType.compare("Player") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Player" << white << endl;


				auto player = std::make_shared<PlayerComponent>(name + "Player");
				ptr->addComponent(player);
				_addPlayerComponent(player, ptr, cmp);
				player.reset();
			}
			else if (componentType.compare("Maptile") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Maptile" << white << endl;


				auto maptile = std::make_shared<MaptileComponent>(name + "Maptile");
				ptr->addComponent(maptile);
				_addMaptileComponent(maptile, ptr, cmp);
				maptile.reset();
			}
			else if (componentType.compare("Hill") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Hill" << white << endl;


				auto hill = std::make_shared<HillComponent>(name + "Hill");
				ptr->addComponent(hill);
				_addHillComponent(hill, ptr, cmp);
				hill.reset();
			}
			else if (componentType.compare("Building") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Building" << white << endl;


				auto building = std::make_shared<BuildingComponent>(name + "Building");
				ptr->addComponent(building);
				_addBuildingComponent(building, ptr, cmp);
				building.reset();
			}
			else if (componentType.compare("Animator") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Animator" << white << endl;


				auto anim = std::make_shared<AnimatorComponent>(name + "Animator");
				ptr->addComponent(anim);
				_addAnimatorComponent(anim, ptr, cmp);
				anim.reset();
			}
			else if (componentType.compare("CollisionBox") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding CollisionBox" << white << endl;


				auto collider = std::make_shared<CollisionBoxComponent>(name + "CollisionBox");
				ptr->addComponent(collider);
				_addCollisionBoxComponent(collider, ptr, cmp);
				collider.reset();
			}
			else if (componentType.compare("Navigator") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Navigator" << white << endl;


				auto navi = std::make_shared<NavigatorComponent>(name + "Navigator");
				ptr->addComponent(navi);
				_addNavigatorComponent(navi, ptr, cmp);
				navi.reset();
			}
			else if (componentType.compare("Renderable") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Renderable" << white << endl;


				auto render = std::make_shared<RenderableComponent>(name + "Renderable");
				ptr->addComponent(render);
				_addRenderableComponent(render, ptr, cmp);
				render.reset();
			}
			else if (componentType.compare("Technology") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Technology" << white << endl;

				auto tech = std::make_shared<TechnologyComponent>(name + "Technology");
				ptr->addComponent(tech);
				_addTechnologyComponent(tech, ptr, cmp);
				tech.reset();
			}
			else if (componentType.compare("Nation") == 0)
			{
				cout << color(colors::DARKGREEN);
				std::cout << "\tAdding Nation" << white << endl;

				auto nation = std::make_shared<NationComponent>(name + "Nation");
				ptr->addComponent(nation);
				_addNationComponent(nation, ptr, cmp);
				nation.reset();
			}
			else
			{
				std::cout << color(colors::RED);
				std::cout << "\tUnknown Component Type! Ignoring..." << white << endl;

				// An unknown component will not be included and just ignored...
			}





			if (panik)
			{
				std::cout << color(colors::RED);
				std::cout << "Could not construct Gameobject \"" << name << "\"" << white << endl;

				ptr.reset();
				return nullptr;
			}

			cmp = cmp->NextSiblingElement("Component");
		}



		if (!ptr)
		{
			cout << color(colors::RED);
			cout << "Could not Instantiate Prefab \""<< prefabName << "\" {"<< prefabStorage[prefabName]  << "}" << white << endl;
		}
		else
		{
			if (gameObjectStorage.size() <= gameObjectNumber)
			{
				// Doubling the size of the storage.
				gameObjectStorage.resize(gameObjectStorage.size() == 0 ? 1000 : gameObjectStorage.size() * 2);
			}

			gameObjectStorage[gameObjectNumber] = ptr;
			//gameObjectStorage.push_back(ptr);

		}

		doc.Clear();
		return ptr;
	}


	return nullptr;
}



Pointer<GameObject2> GameobjectStorage::GetReference(std::string gameobjectTag)
{
	// Return a weak reference to a shared pointer.
	// Thus not increasing the reference count.
	// The correct index of the Gameobject should be found first.
	//for (auto& entity : gameObjectStorage)
	//{
	//	if (entity->getTag().compare(gameobjectTag) == 0)
	//	{
	//		return entity;
	//	}
	//}

	// Our Tag has the form "GO_#_Spearman", where # is the hash of the Gameobject.
	// Thus we easily can retrieve the index in the Storage:
	return gameObjectStorage[_getHashFromGameobjectTag(gameobjectTag)];
}


size_t GameobjectStorage::_getHashFromGameobjectTag(std::string tag)
{
	std::string hash;

	for (int i = 0; i < tag.size(); i++)
	{
		// Skipping "GO_"
		if (i < 3) continue;

		// Get the number
		if (std::isdigit(tag[i]))
		{
			hash += tag[i];
		}
	}

	return std::stoi(hash);
}



IGameobjectStorage* GameobjectStorage::get()
{
	if (!IGameobjectStorage::g_IGameobjectStorage)
	{
		IGameobjectStorage::g_IGameobjectStorage = new GameobjectStorage();
	}

	return IGameobjectStorage::g_IGameobjectStorage;
}


void GameobjectStorage::del()
{
	using namespace std;

	if (IGameobjectStorage::g_IGameobjectStorage)
	{
		/*
		while (IGameobjectStorage::g_IGameobjectStorage->GetStorage().size() > 0)
		{
			if (IGameobjectStorage::g_IGameobjectStorage->GetStorage()[0].use_count() > 1)
			{
				cout << color(colors::RED);
				cout << "[GameobjectStorage::del] Gameobject \"" << IGameobjectStorage::g_IGameobjectStorage->GetStorage()[0]->getName() << "\" has more than one Uses! Current count: " << IGameobjectStorage::g_IGameobjectStorage->GetStorage()[0].use_count() << white << endl;
			}


			IGameobjectStorage::g_IGameobjectStorage->DestroyGameobject(IGameobjectStorage::g_IGameobjectStorage->GetStorage()[0]);
		}
		*/


		// Reset all Gameobjects.
		for (int i = 0; i < IGameobjectStorage::g_IGameobjectStorage->GetStorage().size(); i++)
		{
			if (IGameobjectStorage::g_IGameobjectStorage->GetStorage()[i] == nullptr) continue;


			if (IGameobjectStorage::g_IGameobjectStorage->GetStorage()[i].use_count() > 1)
			{
				cout << color(colors::RED);
				cout << "[GameobjectStorage::del] Gameobject \"" << IGameobjectStorage::g_IGameobjectStorage->GetStorage()[i]->getName() << "\" has more than one Uses! Current count: " << IGameobjectStorage::g_IGameobjectStorage->GetStorage()[i].use_count() << white << endl;
			}


			IGameobjectStorage::g_IGameobjectStorage->DestroyGameobject(IGameobjectStorage::g_IGameobjectStorage->GetStorage()[i]);
		}


		// Clear the Storage vector and map.
		IGameobjectStorage::g_IGameobjectStorage->GetStorage().clear();
		IGameobjectStorage::g_IGameobjectStorage->GetPrefabStorage().clear();

		delete IGameobjectStorage::g_IGameobjectStorage;
		IGameobjectStorage::g_IGameobjectStorage = nullptr;
	}
}


bool GameobjectStorage::_addNationComponent(Pointer<NationComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
	return true;
}


bool GameobjectStorage::_addUnitComponent(Pointer<UnitComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos)
{
	using namespace tinyxml2;

	cmp->SetProfession(data->FirstChildElement("Profession")->GetText());

	// Add Unit to Maptile.
	Pointer<GameObject2> pGO = _getMaptileAtPosition(xpos, ypos);
	Pointer<MaptileComponent> maptile = pGO->getComponent<MaptileComponent>("Maptile");
	maptile->AddGameobject(entity);

	return true;
}

bool GameobjectStorage::_addTransformComponent(Pointer<TransformComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos)
{
	cmp->SetPosition(xpos, ypos);
	return true;
}

bool GameobjectStorage::_addRiverComponent(Pointer<RiverComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos)
{

	// Add River to Maptile.
	Pointer<GameObject2> pGO = _getMaptileAtPosition(xpos, ypos);
	Pointer<MaptileComponent> maptile = pGO->getComponent<MaptileComponent>("Maptile");
	maptile->AddGameobject(entity);


	maptile->SetHasRiver(true);

	return true;
}

bool GameobjectStorage::_addRoadComponent(Pointer<RoadComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos)
{
	// Add Road to Maptile.
	Pointer<GameObject2> pGO = _getMaptileAtPosition(xpos, ypos);
	Pointer<MaptileComponent> maptile = pGO->getComponent<MaptileComponent>("Maptile");
	maptile->AddGameobject(entity);

	return true;
}


bool GameobjectStorage::_addRessourceComponent(Pointer<RessourceComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
	return true;
}

bool GameobjectStorage::_addRenderableComponent(Pointer<RenderableComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
	cmp->color = data->FirstChildElement("Color")->GetText();
	cmp->decalName = data->FirstChildElement("Decal")->GetText();
	cmp->renderingLayer = data->FirstChildElement("Layer")->GetText();
	cmp->width = data->FirstChildElement("Width")->FloatText();
	cmp->height = data->FirstChildElement("Height")->FloatText();
	return true;
}

bool GameobjectStorage::_addPlayerComponent(Pointer<PlayerComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
	return true;
}


bool GameobjectStorage::_addTechnologyComponent(Pointer<TechnologyComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
	using namespace tinyxml2;
	using namespace std;

	// Get definition.
	XMLElement* definition = data->FirstChildElement("Definition");
	cmp->SetTechID(definition->FirstChildElement("Name")->GetText());
	cmp->SetTechArea(definition->FirstChildElement("ResearchArea")->GetText());
	cmp->SetResearchPoints(definition->FirstChildElement("ResearchPoints")->IntText());
	cmp->SetBaseWeight(definition->FirstChildElement("BaseWeight")->FloatText());
	cmp->SetTechSubcategory(definition->FirstChildElement("SubCategory")->GetText());


	// Get the requirements.
	XMLElement* requirements = data->FirstChildElement("Requirements");
	XMLElement* req = requirements->FirstChildElement("Req");
	while (req)
	{
		Pointer<TechnologyComponent::Requirement> requirement = std::make_shared<TechnologyComponent::Requirement>();
		Pointer<Any> value = std::make_shared<Any>();

		std::string checkArea = req->Attribute("area");
		std::string checkType = req->Attribute("type");


		if (checkArea.compare("player_start_biome_check") == 0 ||
			checkArea.compare("player_tech_check") == 0 ||
			checkArea.compare("player_unit_check") == 0 ||
			checkArea.compare("player_building_check") == 0 ||
			checkArea.compare("player_fort_check") == 0 ||
			checkArea.compare("player_race_check") == 0)
		{
			// CheckType is a boolean: True or False.
			value->set<std::string>(req->GetText(), "string");
		}
		else if (checkArea.compare("player_pop_check") == 0 ||
			checkArea.compare("player_city_check") == 0)
		{
			// CheckType is a comparison(String): Smaller, Greater or Equal a certain number.
			value->set<int>(req->IntText(), "int");
		}
		else
		{
			cout << color(colors::RED);
			cout << "[GameobjectStorage::_addTechnologyComponent] Unknown Checkarea \""<< checkArea << "\"\n" << white << endl;;
		}

		requirement->m_CheckArea = checkArea;
		requirement->m_CheckType = checkType;
		requirement->m_Value = value;


		if (requirement)
		{
			// Store the requirement for the Technology.
			cmp->AddRequirement(requirement);
		}
		else
		{
			cout << color(colors::RED);
			cout << "[GameobjectStorage::_addTechnologyComponent] Could not create Requirement\n" << white << endl;;
		}


		req = req->NextSiblingElement("Req");
	}


	// Get the effects.

	return true;
}

bool GameobjectStorage::_addNavigatorComponent(Pointer<NavigatorComponent>cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
	return true;
}

bool GameobjectStorage::_addMountainComponent(Pointer<MountainComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos)
{
	// Add Mountain to Maptile.
	Pointer<GameObject2> pGO = _getMaptileAtPosition(xpos, ypos);
	Pointer<MaptileComponent> maptile = pGO->getComponent<MaptileComponent>("Maptile");
	maptile->AddGameobject(entity);


	maptile->SetHasMountain(true);

	return true;
}

bool GameobjectStorage::_addMaptileComponent(Pointer<MaptileComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
	cmp->SetMaptileType(data->Attribute("type"));
	return true;
}

bool GameobjectStorage::_addHillComponent(Pointer<HillComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
	return true;
}

bool GameobjectStorage::_addForestComponent(Pointer<ForestComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos)
{
	// Add Forest to Maptile.
	Pointer<GameObject2> pGO = _getMaptileAtPosition(xpos, ypos);
	Pointer<MaptileComponent> maptile = pGO->getComponent<MaptileComponent>("Maptile");
	maptile->AddGameobject(entity);


	cmp->SetForestType(data->FirstChildElement("Type")->GetText());
	cmp->SetForestBiome(data->FirstChildElement("Biome")->GetText());
	cmp->SetMaxLifetime(data->FirstChildElement("MaxLifetime")->IntText());
	cmp->ResetLifetime();
	return true;
}

bool GameobjectStorage::_addCollisionBoxComponent(Pointer<CollisionBoxComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
	cmp->width = data->FirstChildElement("Width")->FloatText();
	cmp->height = data->FirstChildElement("Height")->FloatText();
	return true;
}

bool GameobjectStorage::_addCityComponent(Pointer<CityComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos)
{
	using namespace tinyxml2;

	cmp->SetFortificationLevel(data->FirstChildElement("Fortification")->GetText());
	cmp->SetMaxStorage(data->FirstChildElement("MaxStorage")->IntText());

	// Get more data and set city type accordingly.
	std::string type = data->FirstChildElement("CityType")->GetText();
	Pointer<GameObject2> pGO = _getMaptileAtPosition(xpos, ypos);

	Pointer<MaptileComponent> maptile = pGO->getComponent<MaptileComponent>("Maptile");
	if (!maptile) return false;

	std::string maptileType = maptile->GetMaptileType();


	bool forest = false, river = false, hill = false, port = false;
	if (maptile->HasForest()) forest = true;
	if (maptile->HasHill()) hill = true;
	if (maptile->HasRiver()) river = true;
	if (maptile->HasWaterAccess()) port = true;

	cmp->SetType(maptileType, type, forest, hill, river, port);


	// Add City to Maptile.
	maptile->AddGameobject(entity);


	// Set Building Slots.
	XMLElement* buildingSlots = data->FirstChildElement("BuildingSlots");
	XMLElement* slot = buildingSlots->FirstChildElement("Slot");
	while (slot)
	{
		Pointer<BuildingSlot> buildSlot = std::make_shared<BuildingSlot>(slot->FloatAttribute("xpos"), slot->FloatAttribute("ypos"), slot->IntAttribute("number"), slot->Attribute("type"));
		cmp->AddBuldingSlot(buildSlot);

		slot = slot->NextSiblingElement("Slot");
	}


	// Set initial Ressources.
	XMLElement* ressources = data->FirstChildElement("Ressources");
	XMLElement* e = ressources->FirstChildElement("Entry");
	while (e)
	{

		cmp->AddRessource(e->Attribute("name"), e->IntAttribute("amount"));

		e = e->NextSiblingElement("Entry");
	}


	// Set initial Units.
	XMLElement* units = data->FirstChildElement("Units");
	e = units->FirstChildElement("Entry");
	while (e)
	{
		int amount = e->IntAttribute("amount");
		
		for (int i = 1; i <= amount; i++)
		{
			Pointer<GameObject2> pUnit = Instantiate(e->Attribute("name"), xpos, ypos);
			pUnit->getComponent<UnitComponent>("Unit")->SetIsInCity(true);
		}

		e = e->NextSiblingElement("Entry");
	}

	// Set initial Buildings.
	XMLElement* buildings = data->FirstChildElement("Buildings");
	e = buildings->FirstChildElement("Entry");
	while (e)
	{
		Pointer<GameObject2> pBuilding = Instantiate(e->Attribute("name"), xpos, ypos);
		
		cmp->AssignBuildingToSlot(pBuilding, e->IntAttribute("slot"), e->Attribute("slotType"));

		e = e->NextSiblingElement("Entry");
	}

	return true;
}

bool GameobjectStorage::_addBuildingComponent(Pointer<BuildingComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
	using namespace tinyxml2;

	XMLElement* requirements = data->FirstChildElement("Requirements");
	XMLElement* req = requirements->FirstChildElement("Req");
	while (req)
	{
		std::string reqType = req->Attribute("type");

		if (reqType.compare("tech") == 0)
		{
			cmp->AddTechRequirement(req->GetText());
		}
		else if(reqType.compare("ressource") == 0)
		{
			cmp->AddRessourceRequirement(req->GetText(), req->IntAttribute("amount"));
		}
		else if (reqType.compare("race") == 0)
		{
			cmp->AddRaceRequirement(req->GetText());
		}
		else if (reqType.compare("slot") == 0)
		{
			cmp->SetSlotType(req->GetText());
		}
		else if (reqType.compare("worker_prof") == 0)
		{
			XMLElement* prof = req->FirstChildElement("Prof");
			while (prof)
			{
				cmp->AddRequiredProfession(prof->GetText());

				prof = prof->NextSiblingElement("Prof");
			}
		}
		else if (reqType.compare("production") == 0)
		{
			XMLElement* def = req->FirstChildElement("Def");
			while (def)
			{
				// Produced Ressource
				XMLElement* prod = def->FirstChildElement("Prod");

				// Need to produce Ressource.
				XMLElement* need = def->FirstChildElement("Need");
				while (need)
				{

					// Is any other ressource required to produce given ressource
					if (need->IntAttribute("amount") > 0)
					{
						cmp->AddProductionTuple(prod->GetText(), prod->IntAttribute("amount"),
							prod->IntAttribute("time"), need->GetText(), need->IntAttribute("amount"));
					}
					else
					{
						cmp->AddProductionTuple(prod->GetText(), prod->IntAttribute("amount"),
							prod->IntAttribute("time"), "none", 0);
					}

					need = need->NextSiblingElement("Need");
				}
				

				def = def->NextSiblingElement("Def");
			}
		}


		req = req->NextSiblingElement("Req");
	}

	return true;
}

bool GameobjectStorage::_addAnimatorComponent(Pointer<AnimatorComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
	return true;
}

