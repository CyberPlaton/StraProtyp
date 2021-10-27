#include "MaptileComponent.h"

void MaptileComponent::AddGameobject(Pointer<GameObject2> ref)
{
	m_Gameobjects.push_back(ref);
}


void MaptileComponent::update()
{
	for (int i = 0; i < m_Gameobjects.size(); i++)
	{

		// Search for Deleted gameobjects and remove them.
		// A Deleted Gameobjects Tag is Empty.
		if (m_Gameobjects[i]->getTag().compare("") == 0)
		{
			m_Gameobjects[i].reset();
			m_Gameobjects.erase(m_Gameobjects.begin() + i);
		}
	}
}


void MaptileComponent::RemoveGameobject(const GOTag& t)
{
	for (int i = 0; i < m_Gameobjects.size(); i++)
	{
		auto ptr = m_Gameobjects[i];
		if (ptr)
		{
			if (ptr->getTag().compare(t) == 0)
			{
				m_Gameobjects[i].reset();
				m_Gameobjects.erase(m_Gameobjects.begin() + i);
				return;
			}
			else if (ptr->getTag().compare("") == 0)
			{
				// Remove invalid or deleted Gameobjects.
				m_Gameobjects[i].reset();
				m_Gameobjects.erase(m_Gameobjects.begin() + i);
				continue;
			}
		}
		else
		{
			// Remove invalid Pointers.
			m_Gameobjects.erase(m_Gameobjects.begin() + i);
			continue;
		}
	}
}


void MaptileComponent::RemoveGameobject(Pointer<GameObject2> ptr)
{
	GOTag tag = ptr->getTag();
	RemoveGameobject(tag);
}


bool MaptileComponent::_hasGameobjectWithComponent(const std::string& componentTag)
{
	for (int i = 0; i< m_Gameobjects.size(); i++)
	{
		auto ptr = m_Gameobjects[i];
		if (ptr->hasComponent(componentTag))
		{
			return true;
		}
	}

	return false;
}


bool MaptileComponent::HasRoad()
{
	return _hasGameobjectWithComponent("Road");
}


bool MaptileComponent::HasImprovement()
{
	return _hasGameobjectWithComponent("Improvement");
}


bool MaptileComponent::HasForest()
{
	return _hasGameobjectWithComponent("Forest");
}


bool MaptileComponent::HasCity()
{
	return _hasGameobjectWithComponent("City");
}


bool MaptileComponent::HasFort()
{
	return _hasGameobjectWithComponent("City");
}


bool MaptileComponent::HasUnit()
{
	return _hasGameobjectWithComponent("Unit");
}

