#pragma once

#include "ComponentSystem.h"
#include "GameObject.h"

/*
* Adding this component to a Gameobject makes it a Hill.
*/
class IHillCmp : public Component
{
public:
	IHillCmp(const ComponentID& name) {
		this->name = name;
		type = "Hill";

		init(type);
	}


	ComponentType getType() override { return this->type; }




private:
	std::string type;
};