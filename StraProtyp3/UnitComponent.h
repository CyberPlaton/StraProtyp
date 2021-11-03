#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "IGameObject.h" // Include for Gameobject Interface
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.

class GameobjectStorage;
class UnitComponent : public Component2
{
	friend class GameobjectStorage;
public:
	UnitComponent(ComponentID name)
	{
		initialize("Unit", name);
	}
	~UnitComponent();

	void AddTechRequirement(TechID techID);
	void AddRaceRequirement(RaceID raceID);
	void AddRessourceRequirement(Tuple< std::string, int > reqTuple);
	void SetName(std::string name);
	void SetProfession(UnitProfession p);
	void SetIsInCity(bool val);


	std::vector< TechID >& GetTechRequirements();
	std::vector< Tuple<std::string, int > >& GetRessourceRequirements();
	std::vector< RaceID >& GetRaceRequirements();
	UnitProfession GetProfession();
	bool IsInCity();


private:

	// What technologies have to be unlocked by the Player in order
	// to produce this unit.
	std::vector< TechID > m_TechRequirements;

	// What ressources needs to be invested in order to produce this unit.
	std::vector< Tuple<std::string, int > > m_RessourceRequirements;

	// What races are able to produce this unit.
	std::vector< RaceID > m_RaceRequirements;

	// The display name of the unit, e.g. "Walter Frosch".
	std::string m_UnitName;

	// What Profession the unit has, e.g. "Swordsman" or "Builder".s
	UnitProfession m_Profession;

	// Whether the unit is currently in a city.
	bool m_IsInCity = false;
};