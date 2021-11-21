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


bool PlayerComponent::IsTechInAreaInResearch(TechArea area)
{
	return !(m_CurrentResearch[area].compare("none") == 0);
}


void PlayerComponent::SetResearchableTechnology(TechArea area, std::vector< TechID >& techs)
{
	m_ResearchableTechnology[area].clear();
	m_ResearchableTechnology[area] = std::move(techs);
}


bool PlayerComponent::CanChooseTechnologyForResearch()
{
	return (m_ResearchableTechnology["civics"].size() > 0 ||
			m_ResearchableTechnology["technical"].size() > 0 ||
			m_ResearchableTechnology["magick"].size() > 0 ||
			m_ResearchableTechnology["military"].size() > 0);
}