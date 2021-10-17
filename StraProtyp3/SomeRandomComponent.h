#pragma once

#include "ComponentSystem.h"


class SomeRandomComponent : public Component
{
public:
	SomeRandomComponent(const ComponentID& name) {
		this->name = name;
		type = "SomeRandomComponent";

		init(type);
	}


	ComponentType getType() override { return this->type; }



	void update()
	{
		// This component has to create another gameobject in the world periodically.
		Pointer<GameObject> city = GameobjectStorage::get()->Instantiate("City_Human_Tiny");	
	
		// And maybe do some stuff with a distinct gameobject.
		Reference< GameObject > unit = GameobjectStorage::get()->GetReference("Some_Random_Dude");
	}


private:
	std::string type;
};