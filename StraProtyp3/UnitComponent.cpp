#include "UnitComponent.h"

UnitComponent::~UnitComponent()
{
	m_TechRequirements.clear();
	m_RessourceRequirements.clear();
	m_RaceRequirements.clear();
	m_UnitName.clear();
	m_Profession.clear();
	m_IsInCity = 0;
}

void UnitComponent::AddTechRequirement(const TechID& techID)
{

}


void UnitComponent::AddRaceRequirement(const RaceID& raceID)
{

}

void UnitComponent::AddRessourceRequirement(Tuple< std::string, int > reqTuple)
{

}

void UnitComponent::SetName(const std::string& name)
{

}

void UnitComponent::SetProfession(const UnitProfession& p)
{

}

void UnitComponent::SetIsInCity(bool val)
{

}



std::vector< TechID >& UnitComponent::GetTechRequirements()
{

}

std::vector< Tuple<std::string, int > >& UnitComponent::GetRessourceRequirements()
{

}

std::vector< RaceID >& UnitComponent::GetRaceRequirements()
{

}

UnitProfession UnitComponent::GetProfession()
{

}

bool UnitComponent::IsInCity()
{

}
