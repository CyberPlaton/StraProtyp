#pragma once

#include "ComponentSystem.h"
#include "GameObject.h"

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




private:
	std::string type;
};