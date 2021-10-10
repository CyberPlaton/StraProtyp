#pragma once

#include <vector>

#include "ComponentSystem.h"
#include "GameObject.h"

#include "MaptileRegions.h"

/*
* Adding this component to a Gameobject makes it a Maptile.
*/
class IMaptileCmp : public Component
{
public:
	enum MType
	{
		MT_SAVANNAH,
		MT_SAND,
		MT_SNOW,
		MT_ICE,
		MT_TEMPERATE,
		MT_TUNDRA,
		MT_WATER_DEEP,
		MT_WATER_SHALLOW,
		MT_JUNGLE
	};


	static std::string getMaptileTypeAsString(IMaptileCmp::MType type);
	static MType getMaptileTypeFromString(const std::string& type);

public:
	IMaptileCmp(const ComponentID& name) {
		this->name = name;
		type = "Maptile";

		init(type);
	}

	/*
	* Component related Type.
	*/
	ComponentType getType() override { return this->type; }
	

	/*
	* Whether it is a Temperate etc. maptile.
	*/
	MType getMaptileType() { return maptileType; }

	void setMaptileType(MType type)
	{
		maptileType = type;
	}

	/*
	* Get the Gameobject Tags on this Maptile.
	* The Instances can be easily and fast retrieved by calling GameObjectStorage.
	*/
	std::vector< GOTag >& getGameobjects()
	{
		return gameobjects;
	}


	void addGameobject(const GOTag& tag)
	{
		gameobjects.push_back(tag);
	}


	void removeGameobject(const GOTag& tag)
	{
		for (int i = 0; i < gameobjects.size(); i++)
		{
			if (gameobjects[i].compare(tag) == 0)
			{
				gameobjects.erase(gameobjects.begin() + i);
				return;
			}
		}
	}



	// Whether on the maptile is a river.
	// This is something constant and does not need to be changed.
	bool hasRiver() { return bhasRiver; }
	void setHasRiver(bool value) { bhasRiver = value; }

	// Whether on the maptile is a hill.
	// This is something constant and does not need to be changed.
	bool hasHill() { return bHasHill; }
	void setHasHill(bool value) { bHasHill = value; }


	// Whether on the maptile is a mountain.
	// This is something constant and does not need to be changed.
	bool hasMountain() { return bHasMountain; }
	void setHasMountain(bool value) { bHasMountain = value; }


	// Whether on the maptile is a road.
	// Roads can be built dynamically by the player, thus this needs to be updated.
	//
	// Runtime: Amortized O( logn ), with n:= Gameobjects in the game.
	// Note: Same runtime is expected for all further querying functions.
	bool hasRoad()
	{
		for (auto& tag : gameobjects)
		{
			GameObject* object = GameObjectStorage::get()->getGOByTag(tag);
			if (object->hasComponent("Road"))
			{
				return true;
			}
		}

		return false;
	}

	
	// Whether on the maptile is an improvement.
	// Improtovements can be built dynamically by the player, thus this needs to be updated.
	bool hasImprovement()
	{
		for (auto& tag : gameobjects)
		{
			GameObject* object = GameObjectStorage::get()->getGOByTag(tag);
			if (object->hasComponent("Improvement"))
			{
				return true;
			}
		}

		return false;
	}
	
	// Whether on the maptile is a forest.
	// Forests are being created and destroyed dynamically, thus this needs to be updated.
	bool hasForest()
	{
		for (auto& tag : gameobjects)
		{
			GameObject* object = GameObjectStorage::get()->getGOByTag(tag);
			if (object->hasComponent("Forest"))
			{
				return true;
			}
		}

		return false;
	}
	
	// Whether on the maptile is a city.
	// Cities can be built dynamically by the player, thus this needs to be updated.
	bool hasCity() 
	{
		for (auto& tag : gameobjects)
		{
			GameObject* object = GameObjectStorage::get()->getGOByTag(tag);
			if (object->hasComponent("City"))
			{
				return true;
			}
		}

		return false;
	}

	// Whether on the maptile is a fort.
	// Forts can be built dynamically by the player, thus this needs to be updated.
	bool hasFort()
	{
		for (auto& tag : gameobjects)
		{
			GameObject* object = GameObjectStorage::get()->getGOByTag(tag);
			if (object->hasComponent("City"))
			{
				return true;
			}
		}

		return false;
	}
	
	// Whether on the maptile is a unit.
	// Obviously needs to be updated.
	bool hasUnit() 
	{
		for (auto& tag : gameobjects)
		{
			GameObject* object = GameObjectStorage::get()->getGOByTag(tag);
			if (object->hasComponent("Unit"))
			{
				return true;
			}
		}

		return false;
	}
	
	// Whether a neighboring maptile is water maptile.
	// This is something constant and does not need to be changed.
	bool hasWaterAccess() { return bHasWaterAccess; }
	void setHasWaterAccess(bool value) { bHasWaterAccess = value; }

	// Whether a ressource is on the maptile, where we do not count the maptile as ressource but special Gameobjects.
	// This is something constant and does not need to be changed.
	bool hasRessource() { return bHasRessource; }
	void setHasRessource(bool value) { bHasRessource = value; }


private:
	std::string type;
	MType maptileType;
	std::vector< GOTag > gameobjects;

	// The ID of the region to which this maptile belongs.
	MaptileRegionID associatedRegion = -1;

	bool bHasWaterAccess = false;
	bool bHasMountain = false;
	bool bHasHill = false;
	bool bhasRiver = false;
	bool bHasRessource = false;
};