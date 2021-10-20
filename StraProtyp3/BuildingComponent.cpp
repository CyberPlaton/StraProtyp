#include "BuildingComponent.h"

BuildingComponent::~BuildingComponent()
{
	m_TechRequirements.clear();
	m_RaceRequirements.clear();
	m_RessourceRequirements.clear();
	m_RequiredProfessions.clear();
	m_ProductionAmount.clear();
	m_ProductionTime.clear();
	m_ProductionRessources.clear();

	m_Worker.reset();
	m_City.reset();

	m_CurrentProduction.clear();
	m_BuildingSlotType.clear();
	m_Name.clear();
	m_ProductionTurnsRemaining = 0;
	m_IsBeingWorked = 0;
	m_ProductionCommenced = 0;
}

void BuildingComponent::AddTechRequirement(const TechID& id)
{

}


void BuildingComponent::AddRaceRequirement(const RaceID& id)
{

}


void BuildingComponent::AddRessourceRequirement(const RessourceID& id, int a)
{

}


void BuildingComponent::AddProductionTuple(const RessourceID& id, int prAmount, int prTime, const RessourceID& ressourceNeed, int ressourceNeedAmount)
{

}


void BuildingComponent::AddRequiredProfession(const UnitProfession& id)
{

}



void BuildingComponent::SetName(const std::string& name)
{

}


void BuildingComponent::SetSlotType(const BuildingSlotType& t)
{

}


void BuildingComponent::SetCity(Reference<GameObject2> c)
{

}


bool BuildingComponent::SetBuildingSlot(Reference<GameObject2> building, int slotNumber, const BuildingSlotType& t)
{

}


void BuildingComponent::SetProductionRessource(const RessourceID& id)
{

}



bool BuildingComponent::ProduceRessource()
{
	// Perform action only if the Worker and City are valid Gameobjects.
	auto pWorker = m_Worker.lock();
	auto pCity = m_City.lock();
	if (pWorker && pCity)
	{
		if (m_IsBeingWorked)
		{

			// Check that Units Profession is valid and he can work on this Building


			// Check whether ressources for the Production are available


			// Check whether City has enough Storage


			// Check whether production has begun and enough turns has elapsed


			// Finally produce the ressource


			// Reset Stuff for next production


			return true;
		}
	}


	return false;
}




BuildingSlotType BuildingComponent::GetBuildingSlotType()
{

}


std::string BuildingComponent::GetName()
{

}


std::vector< UnitProfession >& BuildingComponent::GetRequiredProfession()
{

}


std::vector< TechID >& BuildingComponent::GetRequiredTech()
{

}


std::vector< RaceID >& BuildingComponent::GetRequiredRace()
{

}


std::vector< Tuple< RessourceID, int > >& BuildingComponent::GetRequiredRessources()
{

}


std::map< RessourceID, Tuple<RessourceID, int> >& BuildingComponent::GetRessourceProduction()
{

}


