#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.

class GameobjectStorage;

class RessourceComponent : public Component2
{
	friend class GameobjectStorage;
public:
	RessourceComponent(ComponentID name)
	{
		initialize("Ressource", name);
	}
	~RessourceComponent()
	{
	}

};