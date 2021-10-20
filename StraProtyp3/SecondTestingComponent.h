#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "IGameObject.h" // Include for Gameobject Interface
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions


class SecondTestComponent : public Component2
{
public:
	SecondTestComponent(const ComponentID& name)
	{
		initialize("SecondTestComponent", name);
	}
	~SecondTestComponent()
	{
		// Custom destructor for the Component.
		someData.clear();
		someOtherData = 0;
	}

	void update(std::vector< std::vector< Reference<GameObject2> > >& world)
	{
		using namespace std;


		for (int i = 0; i < world.size(); i++)
		{
			for (int j = 0; j < world[i].size(); j++)
			{
				if (auto ptr = world[i][j].lock())
				{
					cout << ptr->getName() << endl;
				}
			}
		}

	}

	std::string getData() { return someData; }
	size_t getOtherData() { return someOtherData; }

private:

	std::string someData;
	size_t someOtherData;
};