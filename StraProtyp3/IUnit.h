#pragma once

#include <vector>

#include "ComponentSystem.h"

#include "ITech.h"

using RaceID = std::string;

/*
* Adding this component to a Gameobject makes it a Unit.
*/
class IUnitCmp : public Component
{
public:
	IUnitCmp(const ComponentID& name)	{
		this->name = name;
		type = "Unit";

		init(type);
	}

	ComponentType getType() override { return this->type; }

	void addTechRequirement(const TechID& id) { techRequirements.push_back(id); }
	void addRaceRequirement(const RaceID& id) { raceRequirements.push_back(id); }
	void addRessourceRequirement(std::pair<std::string, int> req) { ressourceRequirements.push_back(req); }


	std::vector<TechID>& const getRequiredTech() {return techRequirements;}
	std::vector<RaceID>& const getRequiredRace() { return raceRequirements; }
	std::vector<std::pair<std::string, int>>& const getRequiredRessources() { return ressourceRequirements; }


private:
	std::string type;
	std::vector<TechID> techRequirements;
	std::vector<std::pair<std::string, int>> ressourceRequirements;
	std::vector<RaceID> raceRequirements;
};