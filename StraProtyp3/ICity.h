#pragma once

#include <vector>
#include <map>

#include "ComponentSystem.h"
#include "IPlayer.h"


using RessourceID = std::string;
using CityType = std::string;

/*
* Definition of which Type a Building Slot is.
* In the city there is a certain number of each slot type.
*/
using BuildingSlotType = std::string;


/*
* Adding this to a Gameobject makes it a city.
*/
class ICityCmp : public Component
{
public:

	/*
	* Defines a slot in the city where a building is drawn.
	*/
	struct BuildingSlot
	{
		BuildingSlot(float x, float y, int n, BuildingSlotType type)
		: xpos(x), ypos(y), slotNumber(n), slotType(type)
		{

		}

		float xpos;
		float ypos;
		int slotNumber;
		BuildingSlotType slotType;

		bool isUsed = false;
		GameObject* building = nullptr;
	};



public:
	ICityCmp(const ComponentID& name) {
		this->name = name;
		type = "City";

		init(type);
	}


	ComponentType getType() override { return this->type; }

	void pushSlot(const BuildingSlot& slot) { buildingSlots.push_back(slot); }

	void addUnit(GameObject* unit) { units.push_back(unit); }

	void setData(const std::string& entry, int amount)
	{
		data[entry] = amount;
	}

	void setMaxStorage(int n) { maxStorage = n; }

	void setCityType(const std::string& type) { cityType = type; }

	void setPlayer(IPlayer* p) { player = p; }


	/*
	* Set the building in a slot.
	*/
	bool assignBuildingToSlot(GameObject* go, int slot, BuildingSlotType s)
	{
		if (!buildingSlots[slot - 1].isUsed && 
			buildingSlots[slot - 1].slotNumber == slot &&
			buildingSlots[slot - 1].slotType.compare(s) == 0) // Sanity Checks.
		{
			buildingSlots[slot - 1].building = go;
			buildingSlots[slot - 1].isUsed = true;


			buildings.push_back(go);

			// Set the Position of the Gameobject 
			// According to Slot.
			TransformCmp* cmp = go->getComponent<TransformCmp>("Transform");
			cmp->xpos = buildingSlots[slot - 1].xpos;
			cmp->ypos = buildingSlots[slot - 1].ypos;

			return true;
		}

		return false;
	}



	void increaseRessource(const RessourceID& name, int amount)
	{
		cityRessources[name] += amount;
		if (cityRessources[name] > 100) cityRessources[name] = 100;
	}

	void decreaseRessource(const RessourceID& name, int amount)
	{
		cityRessources[name] -= amount;
		if (cityRessources[name] < 0) cityRessources[name] = 0;
	}

	bool hasRessourceAmount(const RessourceID& name, int amount)
	{
		return cityRessources[name] >= amount;
	}

	std::vector< GameObject* > getUnits() { return units; }
	std::vector< GameObject* > getBuildings(){ return buildings; }


private:
	std::string type;

	// To which player this city belongs.
	IPlayer* player = nullptr;


	// Whether a city or fort and
	// whether on a Plain, Forest, Hill etc.
	CityType cityType;


	// Cities Building slots.
	std::vector< BuildingSlot > buildingSlots;


	// Stores the ressources of the city.
	std::map< RessourceID, int > cityRessources;


	// Max storage is defined for each ressource.
	int maxStorage = -INT_MAX;


	// The units and buildings IN the city.
	// Buildings are added on creation and removed on destruction and
	// units are added on entering the city and removed on exiting the city.
	// Thus if the player is watching a city, only what is in it is being drawn,
	// plus the surrounding lands...
	std::vector< GameObject* > units;
	std::vector< GameObject* > buildings;


	// Central data storage for stuff like "Happiness" etc.
	std::map< std::string, int > data;
};