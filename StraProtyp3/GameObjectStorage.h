#pragma once

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

#include "TestingComponent.h"
#include "SecondTestingComponent.h"



// For loading the Prefabs from XML File.
#include "tinyxml2.h"


class GameobjectStorage : public IGameobjectStorage
{
public:
	// Singleton Class.
	// Should be reachable Game wide.
	static IGameobjectStorage* get();
	static void del();


	// Construct a new Gameobject from preexisting prefab definition.
	// Returns a shared pointer,
	// where the instance itself is destructed on App shutdown.
	Pointer<GameObject2> Instantiate(const std::string& prefabName) override
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
			tag = root->FirstChildElement("Tag")->GetText();
			name = root->FirstChildElement("Name")->GetText();

			// Create a Gameobject.
			ptr = std::make_shared<GameObject2>(tag, name);

			// Get the defined components.
			XMLElement* components = root->FirstChildElement("Components");

			// Get first component
			XMLElement* cmp = components->FirstChildElement("Component");
			XMLElement* data = nullptr;


			bool panik = false;
			while (cmp)
			{
				ComponentType componentType;
				componentType = cmp->Attribute("name");


				// Here we make the "Deep-Copy",
				// where we more just create the new component and 
				if (componentType.compare("Transform") == 0)
				{

				}
				else if (componentType.compare("Forest") == 0)
				{

				}
				else if (componentType.compare("City") == 0)
				{

				}
				else if (componentType.compare("Unit") == 0)
				{

				}
				else if (componentType.compare("SecondTestComponent") == 0) // Testing Stuff.
				{
				}
				else if (componentType.compare("TestComponent") == 0) // Testing Stuff.
				{
				}
				else
				{
					cout << color(colors::RED);
					cout << "Unknown Component Type! Ignoring..." << white << endl;

					// An unknown component will not be included and just ignored...
					continue;
				}





				if (panik)
				{
					cout << color(colors::RED);
					cout << "Could not construct Gameobject \"" << name << "\"" << white << endl;

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
	// Returns a weak pointer,
	// where the reference count is not affected.
	Reference<GameObject2> GetReference(const std::string& gameobjectTag) override
	{
		// Return a weak reference to a shared pointer.
		// Thus not increasing the reference count.
		// The correct index of the Gameobject should be found first.
		return gameObjectStorage[0];
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
		Pointer<GameObject2> p = std::make_shared<GameObject2>("Gameobject", "Testing Gameobject");


		Pointer< SecondTestComponent > ptr = std::make_shared< SecondTestComponent >("Second Test Component");
		Pointer< TestComponent > ptr2 = std::make_shared< TestComponent >("Test Component");

		p->addComponent(ptr);
		p->addComponent(ptr2);



		return false;
	}

private:

	GameobjectStorageVec<GameObject2> gameObjectStorage;

	PrefabStorageMap prefabStorage;
};