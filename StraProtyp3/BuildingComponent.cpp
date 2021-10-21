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
	m_TechRequirements.push_back(id);
}


void BuildingComponent::AddRaceRequirement(const RaceID& id)
{
	m_RaceRequirements.push_back(id);
}


void BuildingComponent::AddRessourceRequirement(const RessourceID& id, int a)
{
	Tuple<std::string, int > req;
	req.x = id;
	req.y = a;
	m_RessourceRequirements.push_back(req);
}


void BuildingComponent::AddProductionTuple(const RessourceID& id, int prAmount, int prTime, const RessourceID& ressourceNeed, int ressourceNeedAmount)
{
	// Store production.
	m_ProductionRessources[id].x = ressourceNeed;
	m_ProductionRessources[id].y = ressourceNeedAmount;

	// Store how much is produced.
	m_ProductionAmount.push_back(Tuple<std::string, int>(id, prAmount));

	// Store how long it is being produced.
	m_ProductionTime.push_back(Tuple<std::string, int>(id, prTime));
}


void BuildingComponent::AddRequiredProfession(const UnitProfession& id)
{
	m_RequiredProfessions.push_back(id);
}



void BuildingComponent::SetName(const std::string& name)
{
	m_Name = name;
}


void BuildingComponent::SetSlotType(const BuildingSlotType& t)
{
	m_BuildingSlotType = t;
}


void BuildingComponent::SetCity(Reference<GameObject2> c)
{
	m_City = c;
}


bool BuildingComponent::SetBuildingSlot(Reference<GameObject2> building, int slotNumber, const BuildingSlotType& t)
{
	auto ptr = building.lock();
	if (ptr)
	{
		if (ptr->hasComponent("City") == 0)
		{
			auto city = ptr->getComponent<CityComponent>("City");
		
			auto pCity = city.lock();
			if (pCity)
			{
				pCity->AssignBuildingToSlot(building, slotNumber, t);
				return true;
			}
		}
	}

	return false;
}


void BuildingComponent::SetProductionRessource(const RessourceID& id)
{
	for (auto& it : m_ProductionRessources)
	{
		if (it.first.compare(id) == 0)
		{
			// Set new production.
			m_CurrentProduction = id;
		}
	}
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
	return m_BuildingSlotType;
}


std::string BuildingComponent::GetName()
{
	return m_Name;
}


std::vector< UnitProfession >& BuildingComponent::GetRequiredProfession()
{
	return m_RequiredProfessions;
}


std::vector< TechID >& BuildingComponent::GetRequiredTech()
{
	return m_TechRequirements;
}


std::vector< RaceID >& BuildingComponent::GetRequiredRace()
{
	return m_RaceRequirements;
}


std::vector< Tuple< RessourceID, int > >& BuildingComponent::GetRequiredRessources()
{
	return m_RessourceRequirements;
}


std::map< RessourceID, Tuple<RessourceID, int> >& BuildingComponent::GetRessourceProduction()
{
	return m_ProductionRessources;
}


