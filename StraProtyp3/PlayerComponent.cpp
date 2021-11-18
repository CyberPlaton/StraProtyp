#include "PlayerComponent.h"


bool PlayerComponent::Initialize(std::string filepath)
{
	return false;
}


void PlayerComponent::AddResearchPoints(TechArea area, int amount)
{
	m_ResearchPoints[area] += amount;
}


void PlayerComponent::RemoveResearchedTechnology(TechID id)
{
	auto it = std::find(m_ResearchedTechnologies.begin(), m_ResearchedTechnologies.end(), id);
	if (it != m_ResearchedTechnologies.end())
	{
		m_ResearchedTechnologies.erase(it);
	}
}