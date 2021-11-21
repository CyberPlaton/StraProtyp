#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.


#include "UnitComponent.h" // Include for access to all units belonging to this player
#include "CityComponent.h" // Include for access to all cities/forts belonging to this player


class GameobjectStorage;
class PlayerComponent : public Component2
{
	friend class GameobjectStorage;
public:
	PlayerComponent(ComponentID name)
	{
		initialize("Player", name);


		m_ResearchPoints["civics"] = 0;
		m_ResearchPoints["technical"] = 0;
		m_ResearchPoints["magick"] = 0;
		m_ResearchPoints["military"] = 0;

		m_CurrentResearch["civics"] = "none";
		m_CurrentResearch["technical"] = "none";
		m_CurrentResearch["magick"] = "none";
		m_CurrentResearch["military"] = "none";
	}
	~PlayerComponent()
	{
	}

	bool Initialize(std::string filepath);

	void AddUnit(Pointer< GameObject2 > u) { m_Units.push_back(u); }
	
	void SetNation(Pointer<GameObject2> n) { m_ControlledNation = n; }


	void AddResearchedTechnology(TechID id) { m_ResearchedTechnologies.push_back(id); }
	void RemoveResearchedTechnology(TechID id);
	void AddResearchPoints(TechArea area, int amount);
	std::vector < TechID >& GetTechnologies() { return m_ResearchedTechnologies; }
	std::map< TechArea, std::vector< TechID > >& GetResearchableTechnology() { return m_ResearchableTechnology; }
	bool IsTechInAreaInResearch(TechArea area);
	void SetResearchableTechnology(TechArea area, std::vector< TechID >& techs);
	bool CanChooseTechnologyForResearch();
	void SetCurrentResearch(TechArea area, TechID id) { m_CurrentResearch[area] = id; }
	std::map< TechArea, TechID >& GetCurrentResearch() { return m_CurrentResearch; }

private:
	
	// The Nation entity controlled by this Player.
	Pointer<GameObject2> m_ControlledNation;

	// All Units of this player.
	std::vector< Pointer< GameObject2 > > m_Units;

	// All cities and forts of this player.
	std::vector< Pointer< GameObject2 > > m_Cities;

	// The Technologies this player has researched
	std::vector < TechID > m_ResearchedTechnologies;

	
	//For each TechArea the player can choose one Tech to be researched,
	//so it is possible to have 4 simultanious techs researching, thus the maps.
	std::map< TechArea, int > m_ResearchPoints; // How many points were already invested in a tech area
	std::map< TechArea, TechID > m_CurrentResearch; // What tech is being researched in each tech area


	// For each technology area we store the next 3 technologies to choose from
	// for researching.
	// After the player has choosen one to research or the research in an area
	// is on the way, then that vector is empty and shouldnt be used.
	std::map< TechArea, std::vector< TechID > > m_ResearchableTechnology;
};
