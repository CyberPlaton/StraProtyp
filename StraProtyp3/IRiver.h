#pragma once

#include "ComponentSystem.h"
#include "GameObject.h"

/*
* Adding this component to a Gameobject makes it a River.
*/
class IRiverCmp : public Component
{
public:
	IRiverCmp(const ComponentID& name) {
		this->name = name;
		type = "River";

		init(type);
	}


	ComponentType getType() override { return this->type; }




private:
	std::string type;
};