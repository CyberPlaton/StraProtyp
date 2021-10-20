#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "IGameObject.h" // Include for Gameobject Interface
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions


class TestComponent : public Component2
{
public:
	TestComponent(const ComponentID& name)
	{
		initialize("TestComponent", name);
	}
	~TestComponent()
	{

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