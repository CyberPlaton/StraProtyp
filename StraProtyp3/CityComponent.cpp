#include "CityComponent.h"

CityComponent::~CityComponent()
{
	while (m_Units.size() > 0)
	{
		m_Units[0].reset();
		m_Units.erase(m_Units.begin());
	}

	while (m_Buildings.size() > 0)
	{
		m_Buildings[0].reset();
		m_Buildings.erase(m_Buildings.begin());
	}

	while (m_BuildingSlots.size() > 0)
	{
		m_BuildingSlots[0].reset();
		m_BuildingSlots.erase(m_BuildingSlots.begin());
	}

	m_Player.reset();
	m_City.reset();
	m_Units.clear();
	m_Buildings.clear();
	m_Religions.clear();
	m_ReligionPressureValues.clear();
	m_BuildingSlots.clear();
	m_MaxStorage = 0;
	m_CityName.clear();
	m_CityType.clear();
	m_CityFortificationLevel.clear();
	m_ReligionColor.~ReligionColor();
}

void CityComponent::AddRessource(RessourceID id, int amount)
{
	m_Ressources[id] += amount;
}

void CityComponent::UpdateReligion(Pointer<GameObject2> game)
{
	// Perform an Update if the Gameobject Game is still alive and valid.
	if (game)
	{
		// Update Pressure Values


		// Normalize Religion Values


		// Update Major Religion


		// For each Maptile within predeefined manhatten Range:
		//		Add Pressure Value according to Formula.
	}
}

void CityComponent::AddBuldingSlot(Pointer<BuildingSlot> s)
{
	m_BuildingSlots.push_back(s);
}

void CityComponent::AddReligion(ReligionID r, float v)
{
	m_Religions.emplace(r, v);
}

void CityComponent::AddReligionPressure(ReligionID r, float v)
{
	m_ReligionPressureValues[r] = v;
}

void CityComponent::AddUnit(Pointer<GameObject2> u)
{
	m_Units.push_back(u);
}

void CityComponent::AssignBuildingToSlot(Pointer<GameObject2> b, int s, BuildingSlotType t)
{
	auto pSlot = m_BuildingSlots[s - 1];

	if (pSlot->m_IsUsed == false &&
		pSlot->m_SlotNumber == s &&
		pSlot->m_SlotType.compare(t) == 0)
	{

		pSlot->m_Building = b;
		pSlot->m_IsUsed = true;

		m_Buildings.push_back(b);


		// Assign Position of Gameobject according to Slot Position.	

	}
}



void CityComponent::SetName(std::string name)
{
	m_CityName = name;
}

void CityComponent::SetMaxStorage(int n)
{
	m_MaxStorage = n;
}

void CityComponent::SetType(std::string type, std::string cityType, bool forest, bool hill, bool river, bool port)
{
	using namespace std;

	std::string cType = cityType;

	// Set city as forest or hill city.
	// They are mutualy exclusive.
	if (forest)
	{
		cType += "_forest";
	}
	else if (hill)
	{
		cType += "_hill";
	}

	// Check whether city is on river
	if (river)
	{
		cType += "_river";
	}

	// Check whether city has water access
	if (port)
	{
		cType += "_port";
	}


	// Set correct underlying maptile type
	if (type.compare("Temperate") == 0)
	{
		cType += "_temperate";
	}
	else if (type.compare("Snow") == 0)
	{
		cType += "_snow";

	}
	else if (type.compare("Tundra") == 0)
	{
		cType += "_tundra";

	}
	else if (type.compare("Savannah") == 0)
	{
		cType += "_savannah";

	}
	else if (type.compare("Jungle") == 0)
	{
		cType += "_jungle";

	}
	else
	{
		cout << color(colors::RED);
		cout << "Undefined Maptile \"" << type << "\", defining as default Temperate..." << white << endl;
		cType += "Temperate";
	}

	m_CityType = cType;
}

void CityComponent::SetFortificationLevel(CityFortificationLevel level)
{
	m_CityFortificationLevel = level;
}

void CityComponent::SetPlayer(Pointer<GameObject2> p)
{
	if (p)
	{
		m_Player.swap(p);
	}
}




void CityComponent::IncreaseRessorce(RessourceID r, int a)
{
	m_Ressources[r] += a;
}

void CityComponent::DereaseRessorce(RessourceID r, int a)
{
	m_Ressources[r] -= a;
}

bool CityComponent::HasRessourceAmount(RessourceID r, int a)
{
	return m_Ressources[r] >= a;
}




CityFortificationLevel CityComponent::GetFortificationLevel()
{
	return m_CityFortificationLevel;
}

CityType CityComponent::GetType()
{
	return m_CityType;
}

std::string CityComponent::GetName()
{
	return m_CityName;
}

ReligionID CityComponent::GetMajorReligion()
{
	return m_MajorReligion;
}

std::vector< Pointer<GameObject2> >& CityComponent::GetUnits()
{
	return m_Units;
}

std::vector< Pointer<GameObject2> >& CityComponent::GetBuildings()
{
	return m_Buildings;
}

std::map< ReligionID, float >& CityComponent::GetReligions()
{
	return m_Religions;
}

std::map< RessourceID, int >& CityComponent::GetRessources()
{
	return m_Ressources;
}

std::vector< Pointer<BuildingSlot> >& CityComponent::GetBuildingSlots()
{
	return m_BuildingSlots;
}
