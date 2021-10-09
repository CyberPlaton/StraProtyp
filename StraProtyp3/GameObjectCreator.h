#pragma once

#include "ColorConsole.h"
#include "WorldState.h"
#include "GameObject.h"
#include "Components.h"

/*
* Include Components relevant to the Game.
*/
#include "IBuilding.h"
#include "IUnit.h"
#include "IImprovement.h"
#include "ICity.h"
#include "IMaptile.h"
#include "IForest.h"
#include "IRessource.h"
#include "IMountain.h"
#include "IRiver.h"
#include "IHill.h"

/*
* Include Random Name Generator to create random names for units and buildings on the fly.
*/
#include "RandomNameGenerator.h"


#include "tinyxml2.h"
#include "nlohmann/json.hpp"


class GameObjectCreator
{
public:
	GameObjectCreator(){}

	GameObject* create(const std::string& filepath, const std::string& name, int xpos, int ypos)
	{ 
		using namespace std;

		if (filepath.find(".xml") != std::string::npos)
		{
			//cout << color(colors::CYAN);
			//cout << "[GameObjectCreator::create] Loading \""<< name << "\" from Path \""<< filepath <<"\"" << white << endl;

			return _createFromXML(filepath, name, xpos, ypos);
		}
		else
		{
			try
			{
				//cout << color(colors::CYAN);
				//cout << "[GameObjectCreator::create] Loading \"" << name << "\" from Gameobject Pathdefinitions" << white << endl;

				return _createFromXML(gameobjectPathDefinitions.at(filepath), name, xpos, ypos);
			}
			catch (...)
			{
				cout << color(colors::RED);
				cout << "[GameObjectCreator::create] Error loading \"" << name << "\", no Pathdefinition could be found" << white << endl;
				return nullptr;
			}
		}
	}


	static void createGameobjectPathDefinition(const std::string& name, const std::string& path)
	{
		gameobjectPathDefinitions.emplace(name, path);
	}

private:

	// Here we define with the name of a Gameobject the
	// according path to its XML file for easier creation.
	static std::map< std::string, std::string > gameobjectPathDefinitions;


	GameObject* _createFromXML(const std::string& xml_filepath, const std::string& name, int xpos, int ypos)
	{
		using namespace std;
		using namespace tinyxml2;
		
		// Create an object defined from xml.
		tinyxml2::XMLDocument doc;


		XMLError result = doc.LoadFile(xml_filepath.c_str());
		if (result != XMLError::XML_SUCCESS) return nullptr;


		XMLNode* root = doc.FirstChild();
		if (root == nullptr) return nullptr;


		// Get tag and name.
		std::string gameobjectName = root->FirstChildElement("Name")->GetText();

		// Create the gameobject
		GameObject* gameobject = new GameObject(name, gameobjectName);

		/*
		* Get the components defining the agent.
		*/
		XMLElement* components = root->FirstChildElement("Components");

		// Get first component
		XMLElement* cmp = components->FirstChildElement("Component");
		XMLElement* data = nullptr;

		bool panik = false;
		while (cmp)
		{
			// Get the component name
			std::string cmp_name = cmp->Attribute("name");


			// Set data based on the component.
			if (cmp_name.compare("Renderable") == 0)
			{
				float w, h;
				std::string color, decal, layer;

				data = cmp->FirstChildElement("Width");
				w = stod(data->GetText());

				data = cmp->FirstChildElement("Height");
				h = stod(data->GetText());

				data = cmp->FirstChildElement("Color");
				color = data->GetText();

				data = cmp->FirstChildElement("Decal");
				decal = data->GetText();

				data = cmp->FirstChildElement("Layer");
				layer = data->GetText();


				gameobject->AddComponent(new RendererableCmp("Renderable", w, h, color, decal, layer));
			}
			else if (cmp_name.compare("Transform") == 0)
			{
				gameobject->AddComponent(new TransformCmp("Transform"));
				gameobject->getComponent<TransformCmp>("Transform")->setPosition(xpos, ypos);
			}
			else if (cmp_name.compare("Navigator") == 0)
			{
				gameobject->AddComponent(new NavigatorCmp("Navigator", gameobject));
			}
			else if (cmp_name.compare("CollisionBox") == 0)
			{
				float w, h;
				data = cmp->FirstChildElement("Width");
				w = stod(data->GetText());

				data = cmp->FirstChildElement("Height");
				h = stod(data->GetText());

				gameobject->AddComponent(new CollisionBoxCmp("CollisionBox", w, h, gameobject));
			}
			else if (cmp_name.compare("WorldState") == 0)
			{
				gameobject->AddComponent(new WorldStateCmp("WorldState"));
			}
			else if (cmp_name.compare("Animator") == 0)
			{
				gameobject->AddComponent(new AnimatorCmp("Animator", gameobject));
			}
			else if (cmp_name.compare("WalkableBuilding") == 0)
			{
				int doorx = (int)stod(cmp->Attribute("doorx"));
				int doory = (int)stod(cmp->Attribute("doory"));

				gameobject->AddComponent(new WalkableBuildingCmp("WalkableBuilding", gameobject , doorx, doory));
			}
			else if (cmp_name.compare("Forest") == 0)
			{
				gameobject->AddComponent(new IForestCmp("Forest"));

				// Add forest to Maptile
				GameObject* maptileGO = _getMaptileAtPosition(xpos, ypos);
				IMaptileCmp* maptile = maptileGO->getComponent<IMaptileCmp>("Maptile");
				maptile->addGameobject(gameobject->getTag());

			}
			else if (cmp_name.compare("Maptile") == 0)
			{
				IMaptileCmp* tile = new IMaptileCmp("Maptile");
				IMaptileCmp::MType type = IMaptileCmp::getMaptileTypeFromString(cmp->Attribute("type"));
				tile->setMaptileType(type);

				gameobject->AddComponent(tile);
			}
			else if (cmp_name.compare("Building") == 0)
			{
				// Get Building Tag and Name.
				std::string buildName = gameobjectName;
				std::string buildTag = "Building_" + buildName;

				IBuildingCmp* building = new IBuildingCmp(buildTag);

				// Setting the Building name as some specific ingame name based on race and randomness.
				std::string randName = RandomNamesGenerator::get()->getBuildingName();
				std::string finalName = " The \""+ randName + "\" " + buildName;
				building->setBuildingName(finalName);
				

				gameobject->AddComponent(building);



				// Check for requirements for the unit and add them.
				XMLElement* reqs = cmp->FirstChildElement("Requirements");
				XMLElement* req = reqs->FirstChildElement("Req");
				std::string req_type;

				while (req)
				{
					req_type = req->Attribute("type");

					if (req_type.compare("tech") == 0)
					{
						building->addTechRequirement(req->GetText());
					}
					else if (req_type.compare("race") == 0)
					{
						building->addRaceRequirement(req->GetText());
					}
					else if (req_type.compare("ressource") == 0)
					{
						int amount = stod(req->Attribute("amount"));
						building->addRessourceRequirement(req->GetText(), amount);
					}
					else if (req_type.compare("production") == 0)
					{
						// Get the production tuples.
						XMLElement* def = req->FirstChildElement("Def");
						while (def)
						{
							
							// Production.
							XMLElement* prod = def->FirstChildElement("Prod");
							std::string prodRess = prod->GetText();
							int prodAmount = prod->IntAttribute("amount", INT_MAX);
							int prodTime = prod->IntAttribute("time", INT_MAX);


							// Need. Note that we can have a variable number of needs.
							XMLElement* need = def->FirstChildElement("Need");
							while (need && need->IntAttribute("amount", 0) > 0)
							{

								std::string reqRess = need->GetText();
								int reqAmount = need->IntAttribute("amount", INT_MAX);



								// Calling this function n Times adds n requirements,
								// where previous definitions are not override or duplicated.
								building->addProductionTuple(prodRess, prodAmount, prodTime, reqRess, reqAmount);

								need = need->NextSiblingElement("Need");
							}


							def = def->NextSiblingElement("Def");
						}
					}
					else if (req_type.compare("slot") == 0)
					{
						building->setSlotType(req->GetText());
					}
					else if (req_type.compare("worker_prof") == 0)
					{
						// Add valid professions.
						XMLElement* prof = req->FirstChildElement("Prof");
						while (prof)
						{
							building->addRequiredProfession(prof->GetText());

							prof = prof->NextSiblingElement("Prof");
						}
					}


					req = req->NextSiblingElement("Req");
				}
			}
			else if (cmp_name.compare("Unit") == 0)
			{
				// Get Unit Name and Tag
				std::string unitName = gameobjectName;
				std::string unitTag = "Unit_" + unitName;

				IUnitCmp* unit = new IUnitCmp(unitTag);

				// Get a random name for the Unit.
				std::string randName = RandomNamesGenerator::get()->getUnitName();
				std::string finalName = randName + " the \"" + unitName +"\"";
				unit->setUnitName(finalName); // The Unit name is the displaying Name of the in game Citizen.

				gameobject->AddComponent(unit);


				// Add unit to Maptile
				GameObject* maptileGO = _getMaptileAtPosition(xpos, ypos);
				IMaptileCmp* maptile = maptileGO->getComponent<IMaptileCmp>("Maptile");
				maptile->addGameobject(gameobject->getTag());


				// Get the profession.
				unit->setProfession(cmp->FirstChildElement("Profession")->GetText());


				// Check for requirements for the unit and add them.
				XMLElement* reqs = cmp->FirstChildElement("Requirements");
				XMLElement* req = reqs->FirstChildElement("Req");
				std::string req_type;

				while (req)
				{
					req_type = req->Attribute("type");

					if (req_type.compare("tech") == 0)
					{
						unit->addTechRequirement(req->GetText());
					}
					else if (req_type.compare("race") == 0)
					{
						unit->addRaceRequirement(req->GetText());
					}
					else if (req_type.compare("ressource") == 0)
					{
						int amount = stod(req->Attribute("amount"));
						unit->addRessourceRequirement({ req->GetText(), amount});
					}


					req = req->NextSiblingElement("Req");
				}

			}
			else if (cmp_name.compare("Improvement") == 0)
			{
				gameobject->AddComponent(new IImprovementCmp("Improvement"));

				// Add Improvement to Maptile.
				GameObject* maptileGO = _getMaptileAtPosition(xpos, ypos);
				IMaptileCmp* maptile = maptileGO->getComponent<IMaptileCmp>("Maptile");
				maptile->addGameobject(gameobject->getTag());

			}
			else if (cmp_name.compare("Ressource") == 0)
			{
			gameobject->AddComponent(new IRessourceCmp("Ressource"));

			// Add Ressource to Maptile.
			GameObject* maptileGO = _getMaptileAtPosition(xpos, ypos);
			IMaptileCmp* maptile = maptileGO->getComponent<IMaptileCmp>("Maptile");
			maptile->addGameobject(gameobject->getTag());

			}
			else if (cmp_name.compare("River") == 0)
			{
			gameobject->AddComponent(new IRiverCmp("River"));

			// Add Ressource to Maptile.
			GameObject* maptileGO = _getMaptileAtPosition(xpos, ypos);
			IMaptileCmp* maptile = maptileGO->getComponent<IMaptileCmp>("Maptile");
			maptile->addGameobject(gameobject->getTag());

			}
			else if (cmp_name.compare("Mountain") == 0)
			{
			gameobject->AddComponent(new IMountainCmp("Mountain"));

			// Add Ressource to Maptile.
			GameObject* maptileGO = _getMaptileAtPosition(xpos, ypos);
			IMaptileCmp* maptile = maptileGO->getComponent<IMaptileCmp>("Maptile");
			maptile->addGameobject(gameobject->getTag());

			}
			else if (cmp_name.compare("Hill") == 0)
			{
			gameobject->AddComponent(new IHillCmp("Hill"));

			// Add Ressource to Maptile.
			GameObject* maptileGO = _getMaptileAtPosition(xpos, ypos);
			IMaptileCmp* maptile = maptileGO->getComponent<IMaptileCmp>("Maptile");
			maptile->addGameobject(gameobject->getTag());

			}
			else if (cmp_name.compare("City") == 0)
			{
				ICityCmp* city = new ICityCmp("City");
				gameobject->AddComponent(city);

				// Citytype
				std::string cType = cmp->FirstChildElement("CityType")->GetText();
				GameObject* maptileGO = _getMaptileAtPosition(xpos, ypos);

				IMaptileCmp* maptile = maptileGO->getComponent<IMaptileCmp>("Maptile");
				std::string maptileType = IMaptileCmp::getMaptileTypeAsString(maptile->getMaptileType());

				// Add Improvement to Maptile.
				maptile->addGameobject(gameobject->getTag());

				
				bool forest = false, river = false, hill = false, port = false;
				if (maptile->hasForest()) forest = true;
				if (maptile->hasHill()) hill = true;
				if (maptile->hasRiver()) river = true;
				if (maptile->hasWaterAccess()) port = true;

				city->setCityType(maptileType, cType, forest, hill, river, port);				

				// Walls
				std::string fortLevel = cmp->FirstChildElement("Fortification")->GetText();
				city->setCityFortificationLevel(fortLevel);


				// Storage
				int maxStorage = cmp->FirstChildElement("MaxStorage")->IntText(-INT_MAX);
				city->setMaxStorage(maxStorage);


				// Data
				XMLElement* data = cmp->FirstChildElement("Data");
				XMLElement* entry = data->FirstChildElement("Entry");
				while (entry)
				{
					
					std::string name = entry->Attribute("name");
					int amount = entry->IntAttribute("amount", -INT_MAX);


					city->setData(name, amount);
					

					entry = entry->NextSiblingElement("Entry");
				}


				// Building Slots.
				data = cmp->FirstChildElement("BuildingSlots");
				entry = data->FirstChildElement("Slot");
				while (entry)
				{

					int slotNum = entry->IntAttribute("number", -INT_MAX);
					float slotXpos = entry->FloatAttribute("xpos", (float)-INT_MAX);
					float slotYpos = entry->FloatAttribute("ypos", (float)-INT_MAX);
					std::string type = entry->Attribute("type");


					// Create a Slot.
					ICityCmp::BuildingSlot slot(slotXpos, slotYpos, slotNum, type);


					// Add Slot.
					city->pushSlot(slot);


					entry = entry->NextSiblingElement("Slot");
				}


				// Starting Stuff for the city.

				// Starting Ressources.
				data = cmp->FirstChildElement("Ressources");
				entry = data->FirstChildElement("Entry");
				while(entry)
				{

					city->increaseRessource(entry->Attribute("name"), entry->IntAttribute("amount", -INT_MAX));

					entry = entry->NextSiblingElement("Entry");
				}


				// Starting Units.
				data = cmp->FirstChildElement("Units");
				entry = data->FirstChildElement("Entry");
				while (entry)
				{
					int amount = entry->IntAttribute("amount", -INT_MAX);
					std::string unitName = entry->Attribute("name");

					for (int i = 0; i < amount; i++)
					{
						// Set the initial position of the Gameobject as invalid.
						// As its correct position will be defined by the City.
						GameObject* object = create(unitName, unitName, 0, 0);
						if (object)
						{
							city->addUnit(object);
						}
					}

					
					entry = entry->NextSiblingElement("Entry");
				}


				// Starting Buildings.
				data = cmp->FirstChildElement("Buildings");
				entry = data->FirstChildElement("Entry");
				while (entry)
				{
					std::string name = entry->Attribute("name");
					int slot = entry->IntAttribute("slot", -INT_MAX);
					BuildingSlotType slotType = entry->Attribute("slotType");


					// Create a Building Gameobject with invalid initial Positions.
					GameObject* object = create(name, name, 0, 0);
					if (object)
					{
						if (!city->assignBuildingToSlot(object, slot, slotType))
						{
							delete object;
							object = nullptr;
						}
					}


					entry = entry->NextSiblingElement("Entry");
				}
			}



			if (panik)
			{
				cout << color(colors::RED);
				cout << "Could not construct Gameobject \"" << name << "\"" << white << endl;

				delete gameobject;
				gameobject = nullptr;
				break;
			}


			cmp = cmp->NextSiblingElement("Component");
		}


		doc.Clear();
		return gameobject;
	}


	GameObject* _createFromJson(const std::string& json_filepath, const std::string& name, int xpos, int ypos)
	{
		// Not implemented
		return nullptr;
	}


	GameObject* _getMaptileAtPosition(int x, int y)
	{
		for (auto& go : GameObjectStorage::get()->getStorage())
		{
			if (go->hasComponent("Maptile"))
			{

				TransformCmp* cmp = go->getComponent<TransformCmp>("Transform");
				if (cmp->xpos == x && cmp->ypos == y)
				{
					return go;
				}

			}
		}

		return nullptr;
	}

	/*
	GameObject* _createFromJson(const std::string& json_filepath, const std::string& name, int xpos, int ypos)
	{
		using namespace std;
		using namespace nlohmann;

		ifstream fin(json_filepath);
		if (fin.is_open() == false) return nullptr;

		json in;
		fin >> in;


		std::string type = in.at("GameObjectType").get<std::string>();


		if (type.compare("Building") == 0)
		{
			GameObject* building = new GameObject("Building", name);

			building->AddComponent(new TransformCmp(building->tag + "_Transform"));

			building->setPosition(xpos, ypos);
			 

			building->AddComponent(new RendererableCmp(building->tag + "_Renderable", in.at("Width").get<int>(), in.at("Height").get<int>(), "grey"));

			
			for (auto& e : in.at("Layout"))
			{
				std::string jsonpath = "GOAP/Gameobjects/" + e.at("Name").get<std::string>() + ".json";

				// Specify the name for the child.
				std::string child_name = name + " " + e.at("Name").get<std::string>();

				GameObject* obj = create(jsonpath, child_name, 0, 0); // Create children without specific position.

				// Children Objects do have position entry.
				int obj_xpos = e.at("Position")[0].get<int>();
				int obj_ypos = e.at("Position")[1].get<int>();

				obj->setPosition(xpos + obj_xpos, ypos + obj_ypos); // Set the relative position.
			}


			cout << color(colors::YELLOW);
			cout << "[BUILDING] NEW BUILDING: ";
			cout << "Tag \"";
			cout << color(colors::GREEN);
			cout << building->getTag() << "\"";

			cout << color(colors::YELLOW);
			cout << "Name ";
			cout << color(colors::GREEN);
			cout << building->getName() << "\"" << white << endl;

			return building;
		}
		else if (type.compare("NPC") == 0)
		{
			
			Agent* npc = new Agent("NPC", name);
			npc->awake();

			npc->AddComponent(new TransformCmp(npc->tag + "_Transform"));

			npc->setPosition(xpos, ypos);

			npc->AddComponent(new RendererableCmp(npc->tag + "_Renderable", 0.8f, 0.8f, "green"));
		

			// An NPC is initialized at some position,
			// he can have some objects in his inventory and
			// he can possess some objects (In order to possess them, they MUST be created first).

			for (auto& e : in.at("Ownership"))
			{
				GameObject* owned_thing = GameObjectStorage::get()->getGOByName(e.at("Name").get<std::string>());
			

				if(!owned_thing)
				{
					cout << "[OWNERSHIP] Object not found:  " << e.at("Name").get<std::string>() << endl;
				}
				else
				{
					cout << "[OWNERSHIP] Object found:  \"" << e.at("Name").get<std::string>() << "\"";
					cout << ", \"" << name << "\" now owns it!" << endl;

					npc->agentOwnedObjects.push_back(owned_thing);

					// Furthermore, ownership extends on all initial gameobjects inside the building,
					// if owned_object is a building.
					if (owned_thing->getTag().find("Building") != std::string::npos)
					{
						// Get all GOs, which are INSIDE it,
						// we can easily get those by parent-child naming convention.
						for (auto& go : GameObjectStorage::get()->getStorage())
						{
							// Dont pushback the buildings themselves a second time.
							if (go->getName().compare(owned_thing->getName()) == 0) continue;


							if (go->getName().find(owned_thing->getName()) != std::string::npos)
							{
								npc->agentOwnedObjects.push_back(go);

								cout << "[OWNERSHIP] Object found:  \"" << go->getName() << "\"";
								cout << ", \"" << name << "\" now owns it!" << endl;
							}
						}
					}
				}
			}


			cout << color(colors::YELLOW);
			cout << "[NPC] NEW NPC: ";
			cout << "Tag \"";
			cout << color(colors::GREEN);
			cout << npc->getTag() << "\"";

			cout << color(colors::YELLOW);
			cout << "Name ";
			cout << color(colors::GREEN);
			cout << npc->getName() << "\"" << white << endl;

			return npc;
		}
		else if (type.compare("Furniture") == 0)
		{
			GameObject* furniture = new GameObject("Furniture", name);

			furniture->AddComponent(new TransformCmp(furniture->tag + "_Transform"));
			
			// Position of the furniture is defined from the parent gameobject, e.g. a house.

			furniture->AddComponent(new RendererableCmp(furniture->tag + "_Renderable", in.at("Width").get<int>(), in.at("Height").get<int>(), "dark_grey"));



			cout << color(colors::YELLOW);
			cout << "[FURNITURE] NEW FURNITURE: ";
			cout << "Tag \"";
			cout << color(colors::GREEN);
			cout << furniture->getTag() << "\"";

			cout << color(colors::YELLOW);
			cout << "Name ";
			cout << color(colors::GREEN);
			cout << furniture->getName() << "\"" << white << endl;


			return furniture;
		}

		return nullptr;
	}
	*/
};