#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.

#include "ColorConsole.h"

class CollisionBoxComponent;
class NavigatorComponent;
class NavMesh;
class GameobjectStorage;


class TransformComponent : public Component2
{
	friend class CollisionBoxComponent;
	friend class NavigatorComponent;
	friend class NavMesh;
	friend class GameobjectStorage;

public:
	TransformComponent(const std::string& name)
	{
		initialize("Transform", name);
	}

	~TransformComponent()
	{
		xpos = 0;
		ypos = 0;
	}


	float GetXPos() { return xpos; }
	float GetYPos() { return ypos; }


	void SetPosition(float x, float y)
	{
		xpos = x;
		ypos = y;
	}

private:
	float xpos;
	float ypos;
};