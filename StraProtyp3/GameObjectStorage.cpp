#include "GameObjectStorage.h"

bool GameobjectStorage::DestroyGameobject(Pointer<GameObject2> entity)
{
	using namespace std;

	// Delete Gameobject.
	if (entity.use_count() > 1)
	{
		cout << color(colors::RED);
		cout << "Gameobject \"" << entity->getName() << "\" has " << entity.use_count() << " \"Pointers\" in use..." << white << endl;
	}
	else
	{
		cout << color(colors::GREEN);
		cout << "Gameobject \"" << entity->getName() << "\" fully deleted" << white << endl;
	}

	// Remove entry from storage
	for (int index = 0; index < gameObjectStorage.size(); index++)
	{
		if (entity->getTag().compare(gameObjectStorage[index]->getTag()) == 0)
		{
			gameObjectStorage.erase(gameObjectStorage.begin() + index);
			break;
		}
	}

	entity->~GameObject2(); // Destroy Components and their data
	entity.reset(); // Destroy Gameobject Pointer self


	return true;
}


bool GameobjectStorage::DestroyGameobject(const std::string& tag)
{
	using namespace std;

	for (auto& entity : gameObjectStorage)
	{
		if (entity->getTag().compare(tag) == 0)
		{
			// Delete Gameobject.
			return DestroyGameobject(entity);
		}
	}


	return false;
}

Pointer<GameObject2> GameobjectStorage::Instantiate(const std::string& prefabName, float xpos, float ypos)
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


		std::string tag, name;
		size_t gameObjectNumber = ++IGameobjectStorage::g_GameobjectCount;
		tag = "GO_" + std::to_string(gameObjectNumber) + "_";
		tag += root->FirstChildElement("Tag")->GetText();
		name = root->FirstChildElement("Name")->GetText();



		cout << color(colors::GREEN);
		cout << "Instantiate Gameobject \"" << name << "\"{" << tag << "}" << white << endl;


		// Create a Gameobject.
		ptr = std::make_shared<GameObject2>(tag, name);

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

		gameObjectStorage.push_back(ptr);
		doc.Clear();
		return ptr;
	}


	return nullptr;
}



Pointer<GameObject2> GameobjectStorage::GetReference(const std::string& gameobjectTag)
{
	// Return a weak reference to a shared pointer.
	// Thus not increasing the reference count.
	// The correct index of the Gameobject should be found first.
	for (auto& entity : gameObjectStorage)
	{
		if (entity->getTag().compare(gameobjectTag) == 0)
		{
			return entity;
		}
	}

	return nullptr;
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
		for (auto& object : IGameobjectStorage::g_IGameobjectStorage->GetStorage())
		{
			if (object.use_count() > 1)
			{
				cout << color(colors::RED);
				cout << "[GameobjectStorage::del] Gameobject \""<< object->getName() << "\" has more than one Uses! Current count: " << object.use_count() << white << endl;
			}


			// Free the Gameobjects memory
			object.reset();
		}

		// Clear the Storage vector and map.
		IGameobjectStorage::g_IGameobjectStorage->GetStorage().clear();
		IGameobjectStorage::g_IGameobjectStorage->GetPrefabStorage().clear();

		delete IGameobjectStorage::g_IGameobjectStorage;
		IGameobjectStorage::g_IGameobjectStorage = nullptr;
	}
}



bool GameobjectStorage::_addUnitComponent(Pointer<UnitComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos)
{
	using namespace tinyxml2;

	cmp->SetProfession(data->FirstChildElement("Profession")->GetText());

	XMLElement* e = data->FirstChildElement("Requirements");
	XMLElement* req = e->FirstChildElement("Requirement");
	while (req)
	{
		std::string reqType = req->Attribute("type");
		

		if (reqType.compare("tech") == 0)
		{
			cmp->AddTechRequirement(req->GetText());
		}
		else if(reqType.compare("race") == 0)
		{
			cmp->AddRaceRequirement(req->GetText());
		}
		else if (reqType.compare("ressource") == 0)
		{
			Tuple<std::string, int> reqTuple;
			reqTuple.x = req->GetText();
			reqTuple.y = req->IntAttribute("amount");
			cmp->AddRessourceRequirement(reqTuple);
		}


		req = req->NextSiblingElement("Requirement");
	}



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

