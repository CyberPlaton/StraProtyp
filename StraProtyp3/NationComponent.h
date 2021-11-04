#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.


#include "ColorConsole.h"



class GameobjectStorage;
class NationComponent : public Component2
{
	friend class GameobjectStorage;
public:
	NationComponent(ComponentID name)
	{
		initialize("Nation", name);

		m_NationCharacteristics["Spiritual"] = 0.0f;
		m_NationCharacteristics["Materialistic"] = 0.0f;
		m_NationCharacteristics["Technologic"] = 0.0f;
		m_NationCharacteristics["Primitive"] = 0.0f;
		m_NationCharacteristics["Economic"] = 0.0f;
		m_NationCharacteristics["Non-Economic"] = 0.0f;
		m_NationCharacteristics["Isolationist"] = 0.0f;
		m_NationCharacteristics["Expansive"] = 0.0f;
		m_NationCharacteristics["Industrial"] = 0.0f;
		m_NationCharacteristics["Agricultural"] = 0.0f;
		m_NationCharacteristics["Warfaring"] = 0.0f;
		m_NationCharacteristics["Pacifist"] = 0.0f;

		m_NationStats["Health"] = 0;
		m_NationStats["Mana"] = 0;
		m_NationStats["Fatigue"] = 0;
		m_NationStats["Attack"] = 0;
		m_NationStats["Defense"] = 0;
		m_NationStats["Armor"] = 0;
		m_NationStats["Strength"] = 0;
		m_NationStats["Speed"] = 0;
		m_NationStats["Agility"] = 0;
		m_NationStats["Endurance"] = 0;
		m_NationStats["Intelligence"] = 0;
		m_NationStats["Willpower"] = 0;
	}


	~NationComponent()
	{
	}


	float GetCharacteristic(NationCharacteristic c) { return m_NationCharacteristics[c]; }
	void SetCharacteristic(NationCharacteristic c, float v){ m_NationCharacteristics[c] = v; }
	
	int GetStat(std::string s) { return m_NationStats[s]; }
	void SetStat(std::string s, int n){ m_NationStats[s] = n; }

	int GetNeighborCount() { return m_NationNeighbors; }
	
	RaceID GetNationRace() { return m_NationRace; }
	void SetNationRace(RaceID r) { m_NationRace = r; }

	NationID GetNationName() { return m_NationName; }
	void SetNationName(NationID n) { m_NationName = n; }


	void AddResearchedTechnology(TechID id) { m_ResearchedTechnology.push_back(id); }

private:

	NationID m_NationName;
	RaceID m_NationRace;
	int m_NationNeighbors = 0;


	// Distinctive characteristics for a nation, which to a certain degree
	// determine the characteristics of each Unit belonging to that Nation.
	// A nation is not the same as a race.
	std::map< NationCharacteristic, float > m_NationCharacteristics;

	std::map< std::string, int > m_NationStats;


	std::vector< TechID > m_ResearchedTechnology;

private:

};