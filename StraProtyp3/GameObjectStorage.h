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
#include "TechnologyComponent.h"

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


	bool DestroyGameobject(Pointer<GameObject2> entity) override;
	bool DestroyGameobject(const std::string& tag) override;

	// Construct a new Gameobject from preexisting prefab definition.
	// Returns a shared pointer,
	// where the instance itself is destructed on App shutdown.
	Pointer<GameObject2> Instantiate(const std::string& prefabName, float xpos, float ypos) override;

	// Try to get a reference to an existing Gameobject.
	Pointer<GameObject2> GetReference(const std::string& gameobjectTag) override;

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


	void AddPrefab(const std::string& prefabName, const std::string& filepath) override final
	{
		prefabStorage.emplace(prefabName, filepath);
	}

private:

	GameobjectStorageVec<GameObject2> gameObjectStorage;
	PrefabStorageMap prefabStorage;

private:


	bool _addUnitComponent(Pointer<UnitComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos);
	bool _addTransformComponent(Pointer<TransformComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos);
	bool _addRiverComponent(Pointer<RiverComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos);
	bool _addRessourceComponent(Pointer<RessourceComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addRenderableComponent(Pointer<RenderableComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addPlayerComponent(Pointer<PlayerComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addNavigatorComponent(Pointer<NavigatorComponent>cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addMountainComponent(Pointer<MountainComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos);
	bool _addMaptileComponent(Pointer<MaptileComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addHillComponent(Pointer<HillComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addForestComponent(Pointer<ForestComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos);
	bool _addCollisionBoxComponent(Pointer<CollisionBoxComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addCityComponent(Pointer<CityComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data, float xpos, float ypos);
	bool _addBuildingComponent(Pointer<BuildingComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addAnimatorComponent(Pointer<AnimatorComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);
	bool _addTechnologyComponent(Pointer<TechnologyComponent> cmp, Pointer<GameObject2> entity, tinyxml2::XMLElement* data);

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