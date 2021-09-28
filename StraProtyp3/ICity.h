#pragma once

#include <vector>
#include <map>

#include "ComponentSystem.h"

using RessourceID = std::string;


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
		BuildingSlot(int x, int y, int n, BuildingSlotType type)
		: xpos(x), ypos(y), slotNumber(n), slotType(type)
		{

		}

		int xpos;
		int ypos;
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


	/*
	* Set the building in a slot.
	*/
	bool setBuildingInSlot(GameObject* go, int slot, BuildingSlotType s)
	{
		if (!buildingSlots[slot].isUsed && 
			buildingSlots[slot].slotNumber == slot &&
			buildingSlots[slot].slotType.compare(s) == 0)
		{
			buildingSlots[slot].building = go;
			buildingSlots[slot].isUsed = true;

			return true;
		}

		return false;
	}



	void increaseRessource(const RessourceID& name, int amount);
	void decreaseRessource(const RessourceID& name, int amount);


private:
	std::string type;



	// Cities Building slots.
	std::vector< BuildingSlot > buildingSlots;


	// Stores the ressources of the city.
	std::map< RessourceID, int > cityRessources;
};