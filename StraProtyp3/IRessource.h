#pragma once

#include "ComponentSystem.h"
#include "GameObject.h"

#include "IUnit.h"

/*
* Adding this component to a Gameobject makes it a Ressource.
*/
class IRessourceCmp : public Component
{
public:
	IRessourceCmp(const ComponentID& name) {
		this->name = name;
		type = "Ressource";

		init(type);
	}


	ComponentType getType() override { return this->type; }

	// Get all the required professions that can work this ressource
	// with which we can retrieve possible production.
	std::map< RessourceID, UnitProfession > getRequiredProfessions() { return requiredProfessions; }


	// Get the production with amount which is possible to be produced by given profession.
	std::map< RessourceID, int > getPossibleProductionForProfession(const UnitProfession& prof)
	{
		std::map< RessourceID, int > production;

		for (auto& prod: productionRessources)
		{
			for (auto& profession : requiredProfessions)
			{
				if (prod.first.compare(profession.first) == 0)
				{
					if (profession.second.compare(prof) == 0)
					{
						production.emplace(prod.first, prod.second);
					}
				}
			}
		}

		return production;
	}


	// Define a Production with
	// what is being produced, how long does it need to be produced and how much is being produced.
	void addProductionTuple(const RessourceID& prodRessource, int prodAmount, int prodTime)
	{
		// Store production.
		productionRessources.emplace(prodRessource, prodAmount);

		// Store how long it is being produced.
		productionTime[prodRessource] = prodTime;
	}


	// Get or Set the name of the Ressource.
	void setRessourceName(const std::string& name) { ressourceName = name; }
	std::string getRessourceName() { return ressourceName; }


	// Add a profession as a required one to work this RessourceObject. 
	void addRequiredProfession(const RessourceID& prodRessource, const UnitProfession& prof)
	{
		requiredProfessions.emplace(prodRessource, prof);
	}


	// Set the city to which this RessourceObject belongs to,
	// meaning for which it produces ressources.
	void setCity(GameObject* go) { city = go; }


	// Set the ressource for the RessourceObject to produce.
	// Returns true if the RessourceID is valid and the RessourceObject is able to produce that ressource.
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

			bool reqProfFound = false;
			for (auto& prof : requiredProfessions)
			{
				if (prof.second.compare(uProf) == 0)
				{
					reqProfFound = true;
					break;
				}
			}


			if (!reqProfFound)
			{
				return false;
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
			city->getComponent<ICityCmp>("City")->increaseRessource(currentProduction, productionRessources[currentProduction]);


			// Reset for next production.
			productionTurnsRemaining = INT_MAX;
			productionCommenced = false;


			return true;
		}

		return false;
	}


private:
	std::string type;
	std::string ressourceName;


	// City to which this ressource belongs.
	GameObject* city = nullptr;


	// The required professions to work on this ressource,
	// where the index matches the "productionRessources" index.
	std::map< RessourceID, UnitProfession > requiredProfessions;


	// Defines ressources which can be produced with amount.
	// E.g. { "Clay" : 2}
	// Or   { "Wood" : 1 }
	std::map< RessourceID, int > productionRessources;


	// Defines how many turns a ressource needs to be produced.
	std::map< RessourceID, int > productionTime;


	// What is currently being produced.
	RessourceID currentProduction;


	// How long is left until a ressource is produced.
	int productionTurnsRemaining = INT_MAX;
	bool productionCommenced = false;


	// Indicate whether this building is being worked by a Unit and
	// thus can produce ressorces.
	bool isBeingWorked = false;


	// Gameobject which is working on this building.
	// The pointer provided is for giving the unit experience points, check for 
	// required profession, level etc etc.
	GameObject* worker = nullptr;
};