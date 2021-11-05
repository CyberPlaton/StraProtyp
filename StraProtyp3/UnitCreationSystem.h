#pragma once

#include "GameObjectStorage.h" // Include all Components and GameObject2 Definitions.


class UnitCreationSystem
{
	struct UnitRequirement
	{
		UnitRequirement() {}

		UnitProfession prof;

		std::vector< Tuple<std::string, int > > m_RessourceRequirements;
		std::vector< RaceID > m_RaceRequirements;
		std::vector< TechID > m_TechRequirements;
	};

public:
	static UnitCreationSystem* get();
	static void del();


	// Load available unit and link to prefabs,
	// further extract the requirements for tech, race and ressources
	// from the Units and store them in here.
	bool Initialize(std::string filepath);
	bool ReloadDefinition();


	// Create a new unit in a city.
	// From the Player we get the Nation for adjusting Stats etc.
	// From the City we get the ressources.
	// From the unitName we can check whether requirements are met and
	// lastly we set Units position as given.
	bool CreateUnit(Pointer<GameObject2> player, Pointer<GameObject2> city,
					UnitProfession unitName, float xpos, float ypos);


	// Create a neutral Unit without faction and without any Checks at a world position.
	bool CreateIndependentUnit(UnitProfession unitName, float xpos, float ypos);
	

	// Create a unit without any Checks in a city for a given player.
	bool CreateUnitWithoutCheckInCity(Pointer<GameObject2> player, Pointer<GameObject2> city,
									UnitProfession unitName, float xpos, float ypos);


	// Create a unit without any Checks outside of the city for a given player.
	bool CreateUnitWithoutCheckOutsideOfCity(Pointer<GameObject2> player,
											UnitProfession unitName, float xpos, float ypos);


private:
	static UnitCreationSystem* g_UnitCreationSystem;

	std::string m_DefintionPath;


	std::vector< Pointer<UnitRequirement> > m_UnitRequirements;

private:
	UnitCreationSystem(){}



	void _extractUnitRequirements(std::string filepath);
};