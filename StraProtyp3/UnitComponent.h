#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "IGameObject.h" // Include for Gameobject Interface
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.


#include "NationComponent.h"


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

	void SetName(std::string name);
	void SetProfession(UnitProfession p);
	void SetIsInCity(bool val);


	UnitProfession GetProfession();
	bool IsInCity();


	// Characteristics for each unit.
	// General:
	// 	   These are determined by other Stats.
	// - Health (Strength, Endurance), Mana (Intelligence, Willpower), Fatigue (Agility, Speed, Endurance).
	// Special:
	// 	   These are determined partly by other Stats and
	// 	   partly by the Birthsign and National Characteristics (Race etc),
	// 	   and the actual Type of the unit, thus Training.
	// - Attack (Strength, Endurance), Defense (Agility, Endurance), Armor (Unit Dependent), Agility, Speed, Strength, Endurance, Intelligence, Willpower
	
	bool SetStats(Pointer<GameObject2> nation);
	
	int GetHealth() { return m_Health; }
	int GetMana() { return m_Mana; }
	int GetFatigue() { return m_Fatigue; }
	int GetAttack() { return m_Attack; }
	int GetDefense() { return m_Defense; }
	int GetArmor() { return m_Armor; }
	int GetAgility() { return m_Agility; }
	int GetSpeed() { return m_Speed; }
	int GetStrength() { return m_Strength; }
	int GetEndurance() { return m_Endurance; }
	int GetIntelligence() { return m_Intelligence; }
	int GetWillpower() { return m_Willpower; }

	void SetHealth(int v) { m_Health = v; }
	void SetMana(int v) { m_Mana = v; }
	void SetFatigue(int v) { m_Fatigue = v; }
	void SetAttack(int v) { m_Attack = v; }
	void SetDefense(int v) { m_Defense = v; }
	void SetArmor(int v) { m_Armor = v; }
	void SetAgility(int v) { m_Agility = v; }
	void SetSpeed(int v) { m_Speed = v; }
	void SetStrength(int v) { m_Strength = v; }
	void SetEndurance(int v) { m_Endurance = v; }
	void SetIntelligence(int v) { m_Intelligence = v; }
	void SetWillpower(int v) { m_Willpower = v; }

	RaceID GetRace() { return m_Race; }
	void SetRace(RaceID r) { m_Race = r; }

private:

	int m_Health = 0, m_Mana = 0, m_Fatigue = 0, // General Stats.
		m_Attack = 0, m_Defense = 0, m_Armor = 0, // Fighting Related Stats.
		m_Agility = 0, m_Speed = 0, m_Strength = 0, m_Endurance = 0, m_Intelligence = 0, m_Willpower = 0; // Characteristics.

	RaceID m_Race;

	// The display name of the unit, e.g. "Walter Frosch".
	std::string m_UnitName;

	// What Profession the unit has, e.g. "Swordsman" or "Builder".s
	UnitProfession m_Profession;

	// Whether the unit is currently in a city.
	bool m_IsInCity = false;
};