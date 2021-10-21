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
	m_TechRequirements.push_back(techID);
}


void UnitComponent::AddRaceRequirement(const RaceID& raceID)
{
	m_RaceRequirements.push_back(raceID);
}

void UnitComponent::AddRessourceRequirement(Tuple< std::string, int > reqTuple)
{
	m_RessourceRequirements.push_back(reqTuple);
}

void UnitComponent::SetName(const std::string& name)
{
	m_UnitName = name;
}

void UnitComponent::SetProfession(const UnitProfession& p)
{
	m_Profession = p;
}

void UnitComponent::SetIsInCity(bool val)
{
	m_IsInCity = val;
}



std::vector< TechID >& UnitComponent::GetTechRequirements()
{
	return m_TechRequirements;
}

std::vector< Tuple<std::string, int > >& UnitComponent::GetRessourceRequirements()
{
	return m_RessourceRequirements;
}

std::vector< RaceID >& UnitComponent::GetRaceRequirements()
{
	return m_RaceRequirements;
}

UnitProfession UnitComponent::GetProfession()
{
	return m_Profession;
}

bool UnitComponent::IsInCity()
{
	return m_IsInCity;
}
