#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.


#include "ColorConsole.h"



struct BuildingSlot
{
	BuildingSlot(float x, float y, int n, BuildingSlotType t)
	: m_Xpos(x), m_Ypos(y), m_SlotNumber(n), m_SlotType(t)
	{
	}

	float m_Xpos = 0.0f;
	float m_Ypos = 0.0f;
	int m_SlotNumber = 0;
	BuildingSlotType m_SlotType;
	bool m_IsUsed = false;
	Pointer<GameObject2> m_Building;
};


class GameobjectStorage;
class CityComponent : public Component2
{
	friend class GameobjectStorage;

public:
	CityComponent(const ComponentID& name, Pointer<GameObject2> city)
	{
		m_City.swap(city);
		initialize("City", name);
	}
	~CityComponent();


	// The Function updates the current religion status of the city based on
	// the last turn pressure values accumulated.
	// Where "game" is the Gameobject containing the Game State
	// together with the Gameworldmatrix.
	void UpdateReligion(Pointer<GameObject2> game);


	void AddBuldingSlot(Pointer<BuildingSlot> s);
	void AddReligion(const ReligionID& r, float v);
	void AddReligionPressure(const ReligionID& r, float v);
	void AddUnit(Pointer<GameObject2> u);
	void AssignBuildingToSlot(Pointer<GameObject2> b, int s, const BuildingSlotType& t);
	void AddRessource(const RessourceID& id, int amount);

	void SetName(const std::string& name);
	void SetMaxStorage(int n);
	void SetType(const std::string& type, const std::string& cityType, bool forest = false, bool hill = false, bool river = false, bool port = false);
	void SetFortificationLevel(const CityFortificationLevel& level);
	void SetPlayer(Pointer<GameObject2> p);



	void IncreaseRessorce(const RessourceID& r, int a);
	void DereaseRessorce(const RessourceID& r, int a);
	bool HasRessourceAmount(const RessourceID& r, int a);



	CityFortificationLevel GetFortificationLevel();
	CityType GetType();
	std::string GetName();
	ReligionID GetMajorReligion();
	std::vector< Pointer<GameObject2> >& GetUnits();
	std::vector< Pointer<GameObject2> >& GetBuildings();
	std::map< ReligionID, float >& GetReligions();
	std::map< RessourceID, int >& GetRessources();
	std::vector< Pointer<BuildingSlot> >& GetBuildingSlots();



private:

	Pointer<GameObject2> m_Player;

	Pointer<GameObject2> m_City;
	CityFortificationLevel m_CityFortificationLevel;
	CityType m_CityType;
	std::string m_CityName;


	std::vector< Pointer<GameObject2> > m_Units;
	std::vector< Pointer<GameObject2> > m_Buildings;

	
	std::map< ReligionID, float > m_Religions;
	std::map< ReligionID, float > m_ReligionPressureValues;
	ReligionID m_MajorReligion;


	std::map< RessourceID, int > m_Ressources;
	int m_MaxStorage;
	std::vector< Pointer<BuildingSlot> > m_BuildingSlots;
};
