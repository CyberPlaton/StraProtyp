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

void BuildingComponent::AddTechRequirement(TechID id)
{
	m_TechRequirements.push_back(std::move(id));
}


void BuildingComponent::AddRaceRequirement(RaceID id)
{
	m_RaceRequirements.push_back(std::move(id));
}


void BuildingComponent::AddRessourceRequirement(RessourceID id, int a)
{
	Tuple<std::string, int > req;
	req.x = id;
	req.y = a;
	m_RessourceRequirements.push_back(req);
}


bool BuildingComponent::CanProduceRessource(RessourceID id)
{
	for (auto& p : m_ProductionRessources)
	{
		if (p.first.compare(id) == 0)
		{
			// Check whether this ressource does not need any other ressource
			// to be produced.
			if (p.second.x.compare("none") == 0) return true;
			else
			{
				// Check whether City has enough of the required ressources
				// to produce given ressource.

				Pointer<CityComponent> city = m_City->getComponent<CityComponent>("City");
				if (city->HasRessourceAmount(p.second.x, p.second.y))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}


	return false;
}


void BuildingComponent::AddProductionTuple(RessourceID id, int prAmount, int prTime, RessourceID ressourceNeed, int ressourceNeedAmount)
{
	// Store production.
	m_ProductionRessources[id].x = ressourceNeed;
	m_ProductionRessources[id].y = ressourceNeedAmount;

	// Store how much is produced.
	m_ProductionAmount.push_back(Tuple<std::string, int>(id, prAmount));

	// Store how long it is being produced.
	m_ProductionTime.push_back(Tuple<std::string, int>(id, prTime));
}


void BuildingComponent::AddRequiredProfession(UnitProfession id)
{
	m_RequiredProfessions.push_back(id);
}



void BuildingComponent::SetName(std::string name)
{
	m_Name = name;
}


void BuildingComponent::SetSlotType(BuildingSlotType t)
{
	m_BuildingSlotType = t;
}


void BuildingComponent::SetCity(Pointer<GameObject2> c)
{
	m_City = c;
}


bool BuildingComponent::SetBuildingSlot(Pointer<GameObject2> building, int slotNumber, BuildingSlotType t)
{
	if (building)
	{
		if (building->hasComponent("City") == 0)
		{
			auto city = building->getComponent<CityComponent>("City");
		
			if (city)
			{
				city->AssignBuildingToSlot(building, slotNumber, t);
				return true;
			}
		}
	}

	return false;
}


void BuildingComponent::SetProductionRessource(RessourceID id)
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
	if (m_Worker && m_City)
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


