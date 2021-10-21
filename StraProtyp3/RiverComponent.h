#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.

class RiverComponent : public Component2
{
public:
	RiverComponent(const ComponentID& name)
	{
		initialize("River", name);
	}
	~RiverComponent()
	{
	}

};