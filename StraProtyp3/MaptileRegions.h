#pragma once

#include "AVLTree.h"
#include "GameObject.h"

class MaptileRegionStorage;

using MaptileRegionID = int;


/*
* Definition of a single Region.
*/
class MaptileRegion
{
	friend class MaptileRegionStorage;

public:
	MaptileRegion();


	/*
	* Adding Maptile to region.
	*/
	void addMaptileToRegion(GameObject* go)
	{
#ifdef _DEBUG
		std::string msg = "Gameobject with Tag: \"" + go->getTag() + "\" and Name: \"" + go->getName() + "\" is not a maptile!";
		assert(go->hasComponent("Maptile") && msg.c_str());
#endif

		gameobjects.push_back(go);
	}



	/*
	* Remove maptile from vector, do not delete it from app.
	*/
	void removeMaptileFromRegion(GameObject* go)
	{
		for (int i = 0; i < gameobjects.size(); i++)
		{
			if (gameobjects[i]->getTag().compare(go->getTag()) == 0)
			{
				gameobjects.erase(gameobjects.begin() + i);
			}
		}
	}

private:
	MaptileRegionID id = -1;

	std::vector< GameObject* > gameobjects;
};



/*
* Storage for all Maptile Regions in game.
*/
class MaptileRegionStorage
{
private:
	static MaptileRegionID g_maptileRegionID;

public:

	static MaptileRegionStorage* get()
	{
		if (!g_MaptileRegionStorage)
		{
			g_MaptileRegionStorage = new MaptileRegionStorage();
		}

		return g_MaptileRegionStorage;
	}


	static void del()
	{
		if (g_MaptileRegionStorage)
		{
			delete g_MaptileRegionStorage;
			g_MaptileRegionStorage = nullptr;
		}
	}


	static MaptileRegionID getNextMaptileRegionId()
	{
		return ++g_maptileRegionID;
	}


	void addMaptileRegion(MaptileRegion* region)
	{
		maptileRegions.insert(region->id, region);
	}


	void removeMaptileRegion(MaptileRegionID id)
	{
		maptileRegions.remove(id);
	}


	MaptileRegion* getMaptileRegion(MaptileRegionID id)
	{
		return maptileRegions.search(id);
	}

private:
	MaptileRegionStorage()
	{
	}

	~MaptileRegionStorage()
	{
		maptileRegions.~AVLTree();
	}


	static MaptileRegionStorage* g_MaptileRegionStorage;

	AVLTree< MaptileRegion* > maptileRegions;

};