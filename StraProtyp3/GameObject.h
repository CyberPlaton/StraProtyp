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



	std::map< GOTag, size_t > goTagHashMap;

	/*
	* Storing Gameobjects in 2 places for max efficiency.
	* Where "m_GameObjects" allows iterations and linear time search in names, substrings of tag etc.
	* And "m_GameObjectsTagOptimized" allows O(log n) insertion and search of Gameobjects based on their Tag,
	* or clearer the hash value of the Tag.
	*/
	std::vector< GameObject* > m_GameObjects;	
	AVLTree2< GameObject* > m_GameObjectsTagOptimized;
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
		g_GameObjectCount++;

		this->tag = "GO_" + std::to_string(g_GameObjectCount) +"_" + tag;

		hash = hasher(this->tag);

		GameObjectStorage::get()->add(this);
	}

	virtual ~GameObject()
	{
		name.clear();
		tag.clear();
		hash = 0;
		g_GameObjectCount = 0;
	}


	void AddComponent(Component* c)
	{
		components.insert(c->getComponentLocalHashValue(), c);
	}


	void RemoveComponent(Component* c)
	{
		components.deleteNode(c->getComponentLocalHashValue());
	}

	Component* getComponent(const ComponentType& name)
	{
		size_t localHash = hasher(name);
		return components.findStoredData(localHash);
	}


	template < typename T >
	T* getComponent(const std::string& name)
	{
		return static_cast<T*>(getComponent(name));
	}



	bool hasComponent(const ComponentType& name)
	{
		size_t localHash = hasher(name);
		if (components.searchTree(localHash) != nullptr)
		{
			return true;
		}
		else
		{
			return false;
		}
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
		this->hash = hasher(this->tag);
	}


	std::string getTag() { return tag; }
	std::string getName() { return name; }


	std::string name;
	std::string tag;
	size_t hash;


	AVLTree2< Component* > components;

	static unsigned long long g_GameObjectCount;
};



