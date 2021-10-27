#include "GameObjectStorage.h"


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
				cout << "[GameobjectStorage::del] Gameobject has more than one Uses! Current count: " << object.use_count() << white << endl;
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

bool GameobjectStorage::_addRiverComponent(Pointer<RiverComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
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

bool GameobjectStorage::_addNavigatorComponent(Pointer<NavigatorComponent>cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
	return true;
}

bool GameobjectStorage::_addMountainComponent(Pointer<MountainComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data)
{
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

