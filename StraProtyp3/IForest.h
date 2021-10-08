#pragma once

#include "ComponentSystem.h"
#include "GameObject.h"

/*
* Adding this component to a Gameobject makes it a Forest.
*/
class IForestCmp : public Component
{
public:
	IForestCmp(const ComponentID& name) {
		this->name = name;
		type = "Forest";

		init(type);
	}


	ComponentType getType() override { return this->type; }




private:
	std::string type;
};