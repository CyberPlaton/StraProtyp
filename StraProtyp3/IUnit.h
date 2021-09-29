#pragma once

#include <vector>

#include "ComponentSystem.h"
#include "ITech.h"

using RaceID = std::string;
using UnitProfession = std::string;

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


	std::vector<TechID>& getRequiredTech() {return techRequirements;}
	std::vector<RaceID>& getRequiredRace() { return raceRequirements; }
	std::vector<std::pair<std::string, int>>& getRequiredRessources() { return ressourceRequirements; }

	UnitProfession getProfession() { return profession; }
	void setProfession(const UnitProfession& p) { profession = p; }

private:
	std::string type;
	std::vector<TechID> techRequirements;
	std::vector<std::pair<std::string, int>> ressourceRequirements;
	std::vector<RaceID> raceRequirements;



	UnitProfession profession;
};