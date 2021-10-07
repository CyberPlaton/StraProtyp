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



	// Functions to be implemented.
	bool hasRiver() { return false; }
	bool hasHill() { return false; }
	bool hasMountain() { return false; }
	bool hasRoad() { return false; }
	bool hasImprovement() { return false; }
	bool hasForest() { return false; }
	bool hasCity() { return false; }
	bool hasFort() { return false; }
	bool hasUnit() { return false; }
	bool hasWaterAccess() { return false; }



private:
	std::string type;
	MType maptileType;
	std::vector< GOTag > gameobjects;


	MaptileRegionID associatedRegion = -1;
};