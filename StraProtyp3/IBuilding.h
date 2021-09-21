#pragma once

#include <vector>

#include "ComponentSystem.h"

#include "ITech.h"


/*
* Adding this component to a Gameobject makes it a Building.
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

	std::vector<TechID>& const getRequiredTech() { return techRequirements; }


private:
	std::string type;
	std::vector<TechID> techRequirements;

};