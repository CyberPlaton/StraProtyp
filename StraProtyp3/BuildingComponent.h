#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.

#include "CityComponent.h"


class BuildingComponent : public Component2
{
public:
	BuildingComponent(const ComponentID& name)
	{
		initialize("Building", name);
	}
	~BuildingComponent();


	void AddTechRequirement(const TechID& id);
	void AddRaceRequirement(const RaceID& id);
	void AddRessourceRequirement(const RessourceID& id, int a);
	void AddProductionTuple(const RessourceID& id, int prAmount, int prTime, const RessourceID& ressourceNeed, int ressourceNeedAmount);
	void AddRequiredProfession(const UnitProfession& id);


	void SetName(const std::string& name);
	void SetSlotType(const BuildingSlotType& t);
	void SetCity(Reference<GameObject2> c);
	bool SetBuildingSlot(Reference<GameObject2> building, int slotNumber, const BuildingSlotType& t);
	void SetProductionRessource(const RessourceID& id);


	bool ProduceRessource();


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

	Reference< GameObject2 > m_City;
	Reference< GameObject2 > m_Worker;


	std::vector< TechID > m_TechRequirements;
	std::vector< RaceID > m_RaceRequirements;
	std::vector< Tuple< RessourceID, int > > m_RessourceRequirements;

	std::string m_Name;
};