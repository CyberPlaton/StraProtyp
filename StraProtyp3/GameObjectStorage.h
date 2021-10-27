#pragma once

#include <iostream>

// For giving Informational output to console if debugging.
#include "ColorConsole.h"

// Interface for imlpementation.
#include "IGameobjectStorage.h"


// Include the essential Gameobject interface.
// Every Entity in the game is a Gameobject with a number of Components (ECS).
#include "GameObject2.h"



// Include the essential components for the App.
// Every component which does exist in the game,
// must be included here, because the "GameobjectStorage" creates GOs and
// adds all the Components predefined in the prefab.
#include "BuildingComponent.h"
#include "UnitComponent.h"
#include "CityComponent.h"
#include "RiverComponent.h"
#include "MaptileComponent.h"
#include "PlayerComponent.h"
#include "MountainComponent.h"
#include "ImprovementComponent.h"
#include "ForestComponent.h"
#include "HillComponent.h"
#include "RessourceComponent.h"

#include "TestingComponent.h"
#include "SecondTestingComponent.h"


#include "NavigatorComponent.h"
#include "NavMesh.h"
#include "TransformComponent.h"
#include "RenderableComponent.h"
#include "CollisionBoxComponent.h"
#include "AnimatorComponent.h"


// For loading the Prefabs from XML File.
#include "tinyxml2.h"


class GameobjectStorage : public IGameobjectStorage
{
public:
	// Singleton Class.
	// Should be reachable Game wide.
	static IGameobjectStorage* get();
	static void del();


	bool DestroyGameobject(Pointer<GameObject2> entity) override
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


	bool DestroyGameobject(const std::string& tag) override
	{
		using namespace std;

		for (auto& entity : gameObjectStorage)
		{
			if (entity->getTag().compare(tag) == 0)
			{
				// Delete Gameobject.
				DestroyGameobject(entity);
			}
		}


		return false;
	}



	// Construct a new Gameobject from preexisting prefab definition.
	// Returns a shared pointer,
	// where the instance itself is destructed on App shutdown.
	Pointer<GameObject2> Instantiate(const std::string& prefabName, float xpos, float ypos) override
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
			cout << "Instantiate Gameobject \""<< name << "\"{"<< tag << "}" << white << endl;


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
					_addMountainComponent(mountain, ptr, cmp);
					mountain.reset();
				}
				else if (componentType.compare("River") == 0)
				{
					cout << color(colors::DARKGREEN);
					std::cout << "\tAdding River" << white << endl;


					auto river = std::make_shared<RiverComponent>(name + "River");
					ptr->addComponent(river);
					_addRiverComponent(river, ptr, cmp);
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



	// Try to get a reference to an existing Gameobject.
	Pointer<GameObject2> GetReference(const std::string& gameobjectTag) override
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



	GameobjectStorageVec<GameObject2>& GetStorage() override
	{
		return gameObjectStorage;
	}

	PrefabStorageMap& GetPrefabStorage() override
	{
		return prefabStorage;
	}


	// Try loading all prefabs defined in XML File.
	// The XML File itself should only specify prefab name with another
	// XML File path for loading it.
	bool LoadPrefabs(const std::string& file) override final
	{
		using namespace std;
		using namespace tinyxml2;

		tinyxml2::XMLDocument doc;


		XMLError result = doc.LoadFile(file.c_str());
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


		XMLElement* element = root->FirstChildElement("Prefab");
		while (element)
		{

			std::string name = element->Attribute("name");
			std::string path = element->Attribute("path");

			
			prefabStorage.emplace(name, path);


			element = element->NextSiblingElement("Prefab");
		}


		return true;
	}

private:

	GameobjectStorageVec<GameObject2> gameObjectStorage;
	PrefabStorageMap prefabStorage;



private:


	bool _addUnitComponent(Pointer<UnitComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos);
	bool _addTransformComponent(Pointer<TransformComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos);
	bool _addRiverComponent(Pointer<RiverComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addRessourceComponent(Pointer<RessourceComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addRenderableComponent(Pointer<RenderableComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addPlayerComponent(Pointer<PlayerComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addNavigatorComponent(Pointer<NavigatorComponent>cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addMountainComponent(Pointer<MountainComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addMaptileComponent(Pointer<MaptileComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addHillComponent(Pointer<HillComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addForestComponent(Pointer<ForestComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos);
	bool _addCollisionBoxComponent(Pointer<CollisionBoxComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addCityComponent(Pointer<CityComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos);
	bool _addBuildingComponent(Pointer<BuildingComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addAnimatorComponent(Pointer<AnimatorComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);


	Pointer<GameObject2> _getMaptileAtPosition(int x, int y)
	{
		for (auto& go : GameobjectStorage::get()->GetStorage())
		{
			if (go->hasComponent("Maptile"))
			{
				Pointer<TransformComponent> cmp = go->getComponent<TransformComponent>("Transform");
				if (cmp->xpos == x && cmp->ypos == y)
				{
					return go;
				}

			}
		}

		return nullptr;
	}
};



template < typename T >
std::vector< Pointer< T > > GetAllComponentsOfType(const std::string& t)
{
	std::vector< Pointer< T > > vec;

	for (auto e : GameobjectStorage::get()->GetStorage())
	{
		if (e->hasComponent(t))
		{
			vec.push_back(e->getComponent<T>(t));
		}
	}

	return vec;
}