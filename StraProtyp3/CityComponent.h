#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.


#include "ColorConsole.h"


struct ReligionColor
{
	ReligionColor() : a(0), r(0), g(0), b(0)
	{
	}
	ReligionColor(int r, int g, int b, int a) : a(a), r(r), g(g), b(b)
	{
	}
	~ReligionColor() = default;

	int a, r, g, b;
};


struct BuildingSlot
{
	BuildingSlot(float x, float y, int n, BuildingSlotType t)
	: m_Xpos(x), m_Ypos(y), m_SlotNumber(n), m_SlotType(t)
	{
	}

	~BuildingSlot()
	{
		m_Building.reset();
		m_Xpos = 0;
		m_Ypos = 0;
		m_SlotNumber = 0;
		m_SlotType.clear();
		m_IsUsed = 0;
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
	CityComponent(ComponentID name, Pointer<GameObject2> city)
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
	void AddReligion(ReligionID r, float v);
	void AddReligionPressure(ReligionID r, float v);
	void AddUnit(Pointer<GameObject2> u);
	void AssignBuildingToSlot(Pointer<GameObject2> b, int s, BuildingSlotType t);
	void AddRessource(RessourceID id, int amount);
	void AddPressureValue(ReligionID id, float v) { m_ReligionPressureValues[id] += v; }

	void SetName(std::string name);
	void SetMaxStorage(int n);
	void SetType(std::string type, std::string cityType, bool forest = false, bool hill = false, bool river = false, bool port = false);
	void SetFortificationLevel(CityFortificationLevel level);
	void SetPlayer(Pointer<GameObject2> p);
	void SetMajorReligion(ReligionID id) { m_MajorReligion = id; }
	void SetReligionColor(int r, int g, int b, int a) { m_ReligionColor.a = a; m_ReligionColor.r = r; m_ReligionColor.g = g; m_ReligionColor.b = b;}
	
	void IncreaseRessorce(RessourceID r, int a);
	void DereaseRessorce(RessourceID r, int a);
	bool HasRessourceAmount(RessourceID r, int a);


	CityFortificationLevel GetFortificationLevel();
	ReligionColor GetReligionColor() { return m_ReligionColor; }
	CityType GetType();
	std::string GetName();
	ReligionID GetMajorReligion();
	std::map< ReligionID, float >& GetReligionPressureValues() { return m_ReligionPressureValues; }
	std::vector< Pointer<GameObject2> >& GetUnits();
	std::vector< Pointer<GameObject2> >& GetBuildings();
	std::map< ReligionID, float >& GetReligions();
	std::map< RessourceID, int >& GetRessources();
	std::vector< Pointer<BuildingSlot> >& GetBuildingSlots();



private:

	ReligionColor m_ReligionColor;

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
