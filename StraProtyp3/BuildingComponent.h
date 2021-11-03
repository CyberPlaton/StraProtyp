#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.

#include "CityComponent.h"


class GameobjectStorage;
class BuildingComponent : public Component2
{
	friend class GameobjectStorage;

public:
	BuildingComponent(ComponentID name)
	{
		initialize("Building", name);
	}
	~BuildingComponent();


	void AddTechRequirement(TechID id);
	void AddRaceRequirement(RaceID id);
	void AddRessourceRequirement(RessourceID id, int a);
	void AddProductionTuple(RessourceID id, int prAmount, int prTime, RessourceID ressourceNeed, int ressourceNeedAmount);
	void AddRequiredProfession(UnitProfession id);


	void SetName(std::string name);
	void SetSlotType(BuildingSlotType t);
	void SetCity(Pointer<GameObject2> c);
	bool SetBuildingSlot(Pointer<GameObject2> building, int slotNumber, BuildingSlotType t);
	void SetProductionRessource(RessourceID id);


	bool ProduceRessource();
	bool CanProduceRessource(RessourceID id);


	BuildingSlotType GetBuildingSlotType();
	std::string GetName();
	std::vector< UnitProfession >& GetRequiredProfession();
	std::vector< TechID >& GetRequiredTech();
	std::vector< RaceID >& GetRequiredRace();
	std::vector< Tuple< RessourceID, int > >& GetRequiredRessources();
	std::map< RessourceID, Tuple<RessourceID, int> >& GetRessourceProduction();


private:

	BuildingSlotType m_BuildingSlotType;
	bool m_IsBeingWorked;
	int m_ProductionTurnsRemaining;
	bool m_ProductionCommenced;

	RessourceID m_CurrentProduction;
	
	std::map< RessourceID, Tuple<RessourceID, int> > m_ProductionRessources;
	std::vector< Tuple<RessourceID, int > > m_ProductionTime;
	std::vector< Tuple<RessourceID, int > > m_ProductionAmount;
	std::vector< UnitProfession > m_RequiredProfessions;

	Pointer< GameObject2 > m_City;
	Pointer< GameObject2 > m_Worker;


	std::vector< TechID > m_TechRequirements;
	std::vector< RaceID > m_RaceRequirements;
	std::vector< Tuple< RessourceID, int > > m_RessourceRequirements;

	std::string m_Name;
};