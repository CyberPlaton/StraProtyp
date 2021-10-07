#pragma once

#include <vector>
#include <map>

#include "ComponentSystem.h"
#include "ITech.h"
#include "IUnit.h"
#include "ICity.h"


using RaceID = std::string;
using RessourceID = std::string;

/*
* Adding this component to a Gameobject makes it a Building.
* 
* A Building is always in a city.
* A Building can "produce" other Gameobjects, mainly
* Units and Ressources ( could be just a string and value ).
*/
class IBuildingCmp : public Component
{
public:
	IBuildingCmp(const ComponentID& name) {
		this->name = name;
		type = "Building";

		init(type);
	}

	ComponentType getType() override { return this->type; }

	void addTechRequirement(const TechID& id) { techRequirements.push_back(id); }
	
	void addRaceRequirement(const RaceID& id) { raceRequirements.push_back(id); }
	
	void addRessourceRequirement(const RessourceID& ressName, int amount) { ressourceRequirements.emplace(ressName, amount); }

	void setBuildingName(const std::string& name) { buildingName = name; }
	std::string getBuildingName() { return buildingName; }

	void addProductionTuple(const RessourceID& prodRessource, int prodAmount, int prodTime, const RessourceID& needRessource, int needAmount)
	{
		// Store production.
		productionRessources[prodRessource].emplace(needRessource, needAmount);

		// Store how much is produced.
		productionAmount[prodRessource] = prodAmount;

		// Store how long it is being produced.
		productionTime[prodRessource] = prodTime;
	}

	void setSlotType(const BuildingSlotType& type) { slotType = type; }
	void addRequiredProfession(const UnitProfession& prof) { requiredProfessions.push_back(prof); }

	std::vector<TechID>& getRequiredTech() { return techRequirements; }
	std::vector<RaceID>& getRequiredRace() { return raceRequirements; }
	std::map<RessourceID, int>& getRequiredRessources() { return ressourceRequirements; }


	// City related functionality.
	
	// Set the city in which this building is in.
	void setCity(GameObject* go) { city = go; }
	

	// Set the slot number in which this building is in.
	// The slot of the city defines where in the city the building is drawn.
	// Those slots are predefined for each city type ( small, large... and forts ).
	bool setBuildingSlot(GameObject* self, int i, BuildingSlotType s)
	{
		if (city)
		{
			if (city->hasComponent("City"))
			{
				ICityCmp* cmp = city->getComponent<ICityCmp>("City");

				if (cmp->assignBuildingToSlot(self, i, s))
				{
					return true;
				}
			}
		}

		return false;
	}


	// Let the building produce its ressource,
	// which is currently set as the production ressource.
	// Returns true if the ressource was successfully produced.
	// Note that the ressource can be a Gameobject, a Unit...
	bool produceRessource()
	{
		if (isBeingWorked && worker)
		{
			// Check for correct profession of unit and produced ressource ( Building ).
			UnitProfession uProf = worker->getComponent<IUnitCmp>("Unit")->getProfession();

			auto it = std::find(requiredProfessions.begin(), requiredProfessions.end(), uProf);
			if (it == requiredProfessions.end())
			{
				return false;
			}



			// Check for whether needed ressources for the production are available.
			for (auto& req : productionRessources[currentProduction])
			{
				// Check availability of each ressource in city.
				if (!city->getComponent<ICityCmp>("City")->hasRessourceAmount(req.first, req.second))
				{
					return false;
				}
			}


			// Check whether city has enough storage.
			// TODO



			// Check whether enough production time has elapsed.
			if (productionCommenced)
			{
				if (productionTurnsRemaining > 0)
				{
					productionTurnsRemaining--;
					return false;
				}
			}
			else
			{
				productionCommenced = true;
				productionTurnsRemaining = productionTime[currentProduction];
				productionTurnsRemaining--;
			}

			if (productionTurnsRemaining > 0) return false;




			// Produce ressource.
			city->getComponent<ICityCmp>("City")->increaseRessource(currentProduction, productionAmount[currentProduction]);




			// Reset for next production.
			productionTurnsRemaining = INT_MAX;
			productionCommenced = false;


			return true;
		}

		return false;
	}



	// Set the ressource for the building to produce.
	// Returns true if the RessourceID is valid and the building is able to produce that ressource.
	// Note that the RessourceID can be a Gameobject, a Unit...
	bool setProductionRessource(const RessourceID& id)
	{
		for (auto& it : productionRessources)
		{
			if (it.first.compare(id) == 0)
			{
				// Set new production
				currentProduction = id;
				return true;
			}
		}

		return false;
	}
	


	BuildingSlotType getBuildingSlotType() { return slotType; }




private:
	std::string type;

	std::string buildingName;

	// Requirements to build this Building.
	std::vector<TechID> techRequirements;
	std::map<RessourceID, int> ressourceRequirements;
	std::vector<RaceID> raceRequirements;



	// City to which this building belongs.
	GameObject* city = nullptr;


	// The required professions to work on this building.
	std::vector< UnitProfession > requiredProfessions;


	// Defines ressources which can be produced with a set of required ressources
	// for it to be produced.
	// E.g. { "Iron Weapons" : {"Iron Bars", 5} }
	// Or   { "Iron Bars" :	   {"Iron Ore", 2
	//							"Wood", 1 }
	std::map< RessourceID, std::map< RessourceID, int > > productionRessources;


	// Defines how much of a ressource is being produced.
	std::map< RessourceID, int > productionAmount;


	// Defines how many turns a ressource needs to be produced.
	std::map< RessourceID, int > productionTime;


	// What is currently being produced.
	RessourceID currentProduction;


	// How long is left until a ressource is produced.
	int productionTurnsRemaining = INT_MAX;
	bool productionCommenced = false;

	// Which slot type this building is defined for.
	BuildingSlotType slotType;


	// Indicate whether this building is being worked by a Unit and
	// thus can produce ressorces.
	bool isBeingWorked = false;


	// Gameobject which is working on this building.
	// The pointer provided is for giving the unit experience points, check for 
	// required profession, level etc etc.
	GameObject* worker = nullptr;
};