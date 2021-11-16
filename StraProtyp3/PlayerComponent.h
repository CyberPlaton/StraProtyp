#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.


#include "UnitComponent.h"


class GameobjectStorage;
class PlayerComponent : public Component2
{
	friend class GameobjectStorage;
public:
	PlayerComponent(ComponentID name)
	{
		initialize("Player", name);
	}
	~PlayerComponent()
	{
	}

	bool Initialize(std::string filepath);

	void AddUnit(Pointer< GameObject2 > u) { m_Units.push_back(u); }
	void SetNation(Pointer<GameObject2> n) { m_ControlledNation = n; }


	std::vector < TechID >& GetTechnologies() { return m_ResearchedTechnologies; }

private:

	Pointer<GameObject2> m_ControlledNation;
	std::vector< Pointer< GameObject2 > > m_Units;


	std::vector < TechID > m_ResearchedTechnologies; // The Technologies this player has researched
	std::map< TechArea, int > m_ResearchPoints; // How many points were already invested in a tech area
	std::map< TechArea, TechID > m_CurrentResearch; // What tech is being researched in each tech area
};
