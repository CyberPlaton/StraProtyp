#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "IGameObject.h" // Include for Gameobject Interface
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.



struct BuildingSlot
{
	BuildingSlot(float x, float y, int n, BuildingSlotType t)
	: m_Xpos(x), m_Ypos(y), m_SlotNumber(n), m_SlotType(t)
	{
	}


	float m_Xpos;
	float m_Ypos;
	int m_SlotNumber;
	BuildingSlotType m_SlotType;
	bool m_IsUsed;
	Reference<GameObject2> m_Building;
};



class CityComponent : public Component2
{
public:
	CityComponent(const ComponentID& name, Reference<GameObject2> city)
	{
		m_City.swap(city);
		initialize("City", name);
	}
	~CityComponent()
	{

	}


	// The Function updates the current religion status of the city based on
	// the last turn pressure values accumulated.
	// Where "game" is the Gameobject containing the Game State
	// together with the Gameworldmatrix.
	void UpdateReligion(Reference<GameObject2> game)
	{
		// Perform an Update if the Gameobject Game is still alive and valid.
		auto ptr = game.lock();
		if (ptr)
		{
			// Update Pressure Values


			// Normalize Religion Values


			// Update Major Religion


			// For each Maptile within predeefined manhatten Range:
			//		Add Pressure Value according to Formula.
		}
	}


	void AddBuldingSlot(const BuildingSlot& s){}
	void AddReligion(const ReligionID& r, float v){}
	void AddReligionPressure(const ReligionID& r, float v){}
	void AddUnit(Reference<GameObject2> u){}
	void AssignBuildingToSlot(Reference<GameObject2> b, int s, const BuildingSlotType& t) {}


	void SetName(const std::string& name) {}
	void SetMaxStorage(int n){}
	void SetType(){}
	void SetFortificationLevel(const CityFortificationLevel& level){}
	void SetPlayer(Reference<GameObject2> p){}



	void IncreaseRessorce(const RessourceID& r, int a){}
	void DereaseRessorce(const RessourceID& r, int a) {}
	bool HasRessourceAmount(const RessourceID& r, int a){}



	CityFortificationLevel GetFortificationLevel(){}
	CityType GetType() {}
	std::string GetName() {}
	ReligionID GetMajorReligion(){}
	std::vector< Reference<GameObject2> >& GetUnits(){}
	std::vector< Reference<GameObject2> >& GetBuildings() {}
	std::map< ReligionID, float >& GetReligions(){}
	std::map< RessourceID, int >& GetRessources(){}
	std::vector< Pointer<BuildingSlot> >& GetBuildingSlots(){}



private:

	Reference<GameObject2> m_Player;

	Reference<GameObject2> m_City;
	CityFortificationLevel m_CityFortificationLevel;
	CityType m_CityType;


	std::vector< Reference<GameObject2> > m_Units;
	std::vector< Reference<GameObject2> > m_Buildings;

	
	std::map< ReligionID, float > m_Religions;
	std::map< ReligionID, float > m_ReligionPressureValues;
	ReligionID m_MajorReligion;


	std::map< RessourceID, int > m_Ressources;
	std::vector< Pointer<BuildingSlot> > m_BuildingSlots;
};
