#include "PlayerComponent.h"

bool PlayerComponent::CreateUnit(std::string unitName, float xpos, float ypos)
{
	// Create default unit defined in XML.
	auto unit = IGameobjectStorage::g_IGameobjectStorage->Instantiate(unitName, xpos, ypos);


	// Set the Stats and Charactiristics based on those of the controlled nation.
	if (!unit->getComponent<UnitComponent>("Unit")->SetStats(m_ControlledNation)) return false;


	// Finally alter Units Stats and Characterisitcs a last time based on
	// Astrology and Brithsign.
	// ...


	if (unit == nullptr)
	{
		return false;
	}
	else
	{
		m_Units.push_back(unit);
		return true;
	}
}



bool PlayerComponent::Initialize(std::string filepath)
{
	return false;
}