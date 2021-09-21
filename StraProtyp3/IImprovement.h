#pragma once

#include <vector>

#include "ComponentSystem.h"

#include "ITech.h"


/*
* Adding this component to a Gameobject makes it an Improvement.
*/
class IImprovementCmp : public Component
{
public:
	IImprovementCmp(const ComponentID& name) {
		this->name = name;
		type = "Improvement";

		init(type);
	}

	ComponentType getType() override { return this->type; }

	void addTechRequirement(const TechID& id) { techRequirements.push_back(id); }

	std::vector<TechID>& const getRequiredTech() { return techRequirements; }


private:
	std::string type;
	std::vector<TechID> techRequirements;
};
