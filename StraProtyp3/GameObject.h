#pragma once

#include <vector>
#include <string>

#include "AVLTree.h"
#include "ColorConsole.h"

#include "ComponentSystem.h"

#include "common/include/nlohmann/json.hpp"

/*
* The GOTag can be seen as the App unique identifier for a the GameObject.
* Thus we can identify each and we can easily search for each.
* 
* The tag is generated automatically on construction like for example "GO_16_Furniture".
*/
using GOTag = std::string;

/*
* THE GOName is some given or generated name for a gameobject, which is not necesseraly
* unique and can be duplicate, like the name "John" in a city.
*/
using GOName = std::string;


static std::hash<std::string> hasher;
class GameObject;


class GameObjectStorage
{
public:

	static GameObjectStorage* get();

	static void del();

	std::vector< GameObject* >& getStorage();

	void add(GameObject* go);
	void remove(GameObject* go);
	void remove(const GOTag& tag);


	/*
	* Returns the Gameobject with given tag or nullptr if none exists.
	*/
	GameObject* getGOByTag(const GOTag& tag);


	/*
	* Returns the Gameobject which has given tag as substring in its tag.
	* For example let "GO_12_Furniture" be the Gameobject tag,
	* then findAnyGOByTag( "Furniture" ) would return that Gameobject.
	*/
	GameObject* findAnyGOByTag(const GOTag& tag);


	/*
	* Returns the first Gameobject with given name.
	* Beware, as names are not enforced to be unique app wide.
	*/
	GameObject* getGOByName(const GOName& name);


	/*
	* Returns the first Gameobject which has given name as substring in its name.
	*/
	GameObject* findAnyGOByName(const GOName& name);


private:

	GameObjectStorage(){}

	static GameObjectStorage* g_GameObjectStorage;




	/*
	* Storing Gameobjects in 2 places for max efficiency.
	* Where "m_GameObjects" allows iterations and linear time search in names, substrings of tag etc.
	* And "m_GameObjectsTagOptimized" allows O(log n) insertion and search of Gameobjects based on their Tag,
	* or clearer the hash value of the Tag.
	*/
	std::vector< GameObject* > m_GameObjects;	
	AVLTree< GameObject* > m_GameObjectsTagOptimized;
};




class GameObject
{
public:
	/*
	* Construct a gameobject.
	* GOTag is a GUID for a GameObject.
	* GOName is just a name and can be duplicate.
	*/
	GameObject(const GOTag& tag, const GOName& name) : name(name)
	{
		this->tag = "GO_" + std::to_string(g_GameObjectCount) + "_" + tag;

		hash = hasher(this->tag);

		GameObjectStorage::get()->add(this);

		g_GameObjectCount++;
	}

	virtual ~GameObject()
	{
		name.clear();
		tag.clear();
		hash = 0;
		while (components.size() > 0) delete components[0];
		components.clear();
		g_GameObjectCount = 0;
	}


	void AddComponent(Component* c)
	{
		components.push_back(c);
	}


	void RemoveComponent(Component* c)
	{
		auto it = std::find(components.begin(), components.end(), c);

		if (it != components.end())
		{
			components.erase(it);
		}
	}

	Component* getComponent(const ComponentType& name)
	{
		for (auto& cmp : components)
		{
			// All components are unique for a GameObject,
			// so there cannot be e.g. 2 Transforms.. Thus we can search with ComponentType.

			if (cmp->getType().compare(name) == 0)
			{
				return cmp;
			}

			/*
			// Search for component adjusted to own name.
			if (cmp->name.compare(name) == 0)
			{
				return cmp;
			}
			*/
		}

		return nullptr;
	}


	template < typename T >
	T* getComponent(const std::string& name)
	{
		return static_cast<T*>(getComponent(name));
	}



	bool hasComponent(const ComponentType& name)
	{
		for (auto& cmp : components)
		{

			if (cmp->getType().compare(name) == 0)
			{
				return true;
			}

			/*
			// Search for component adjusted to own name.
			if (cmp->name.compare(name) == 0)
			{
				return true;
			}
			*/
		}

		return false;
	}

	void setPosition(int x, int y)
	{
		Component* cmp = this->getComponent("Transform");

		if (cmp != nullptr)
		{
			TransformCmp* t = static_cast<TransformCmp*>(cmp);
			t->setPosition(x, y);
		}
	}


	/*
	* Change tag and hash for gameobject. 
	* Those should automatically be adjusted in GOStorage,
	* as there are just the pointers to objects.
	*/
	void setTag(const std::string& tag)
	{
		this->tag = tag;
		//this->hash = hasher(this->tag);
	}

	std::string getTag() { return tag; }
	std::string getName() { return name; }


	std::string name;
	std::string tag;
	size_t hash;


	std::vector<Component*> components;

	static unsigned long long g_GameObjectCount;
};



