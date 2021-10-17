#pragma once

#include "Component.h"
#include "IGameObject.h" // Include for Gameobject Interface
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions


class TestComponent : public Component2
{
public:
	TestComponent(const ComponentID& name)
	{
		initialize("TestComponent", name, ++IComponent::g_ComponentHash);
	}

	void update()
	{
		using namespace std;

		cout << "[TestComponent::update] Info:" << endl;
		cout << getComponentID() << endl;
		cout << getComponentType() << endl;
		cout << getComponentHashvalue() << endl;
	}


private:

};