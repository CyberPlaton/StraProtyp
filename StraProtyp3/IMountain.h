#pragma once

#include "ComponentSystem.h"
#include "GameObject.h"

/*
* Adding this component to a Gameobject makes it a Mountain.
*/
class IMountainCmp : public Component
{
public:
	IMountainCmp(const ComponentID& name) {
		this->name = name;
		type = "Mountain";

		init(type);
	}


	ComponentType getType() override { return this->type; }




private:
	std::string type;
};