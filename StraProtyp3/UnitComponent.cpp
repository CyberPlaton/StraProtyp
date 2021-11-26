#include "UnitComponent.h"

UnitComponent::~UnitComponent()
{
	m_UnitName.clear();
	m_Profession.clear();
	m_IsInCity = 0;
}


void UnitComponent::SetName(std::string name)
{
	m_UnitName = name;
}

void UnitComponent::SetProfession(UnitProfession p)
{
	m_Profession = p;
}

void UnitComponent::SetIsInCity(bool val)
{
	m_IsInCity = val;
}


UnitProfession UnitComponent::GetProfession()
{
	return m_Profession;
}

bool UnitComponent::IsInCity()
{
	return m_IsInCity;
}



bool UnitComponent::SetStats(Pointer<GameObject2> nation)
{
	if (!nation->hasComponent(EComponentType::CT_Nation)) return false;

	auto n = nation->getComponent<NationComponent>("Nation");

	// Currently, directly setting the stats and chars according to Nations definition

	m_Race = n->GetNationRace();

	m_Health = n->GetStat("Health");
	m_Mana = n->GetStat("Mana");
	m_Fatigue = n->GetStat("Fatigue");
	m_Attack = n->GetStat("Attack");
	m_Defense = n->GetStat("Defense");
	m_Armor = n->GetStat("Armor");
	m_Agility = n->GetStat("Agility");
	m_Speed = n->GetStat("Speed");
	m_Strength = n->GetStat("Strength");
	m_Endurance = n->GetStat("Endurance");
	m_Intelligence = n->GetStat("Intelligence");
	m_Willpower = n->GetStat("Willpower");

	return true;
}