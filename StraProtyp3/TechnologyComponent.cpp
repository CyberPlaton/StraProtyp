#include "TechnologyComponent.h"

TechID TechnologyComponent::GetTechID()
{
	return m_TechName;
}


TechArea TechnologyComponent::GetTechArea()
{
	return m_TechArea;
}


TechSubcategory TechnologyComponent::GetTechSubcategory()
{
	return m_TechSubcategory;
}


float TechnologyComponent::GetBaseWeight()
{
	return m_BaseWeight;
}


int TechnologyComponent::GetResearchPoints()
{
	return m_ResearchPoints;
}
