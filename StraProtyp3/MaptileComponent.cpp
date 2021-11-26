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


void MaptileComponent::destroy()
{
	for (int i = 0; i < m_Gameobjects.size(); i++)
	{
		m_Gameobjects[i].reset();
	}

	m_MaptileType.clear();

}


void MaptileComponent::RemoveGameobject(GOTag t)
{
	for (int i = 0; i < m_Gameobjects.size(); i++)
	{
		auto ptr = m_Gameobjects[i];
		if (ptr)
		{

			if (ptr->getTag().compare(t) == 0)
			{
				// Erase the Gameobject from this Maptile Storage.
				m_Gameobjects[i].reset();
				m_Gameobjects.erase(m_Gameobjects.begin() + i);

				// Ask for deletion from Program.
				IGameobjectStorage::g_IGameobjectStorage->DestroyGameobject(ptr);
				return;
			}
			else if (ptr->getTag().compare("") == 0)
			{
				// Remove invalid or deleted Gameobjects.
				IGameobjectStorage::g_IGameobjectStorage->DestroyGameobject(ptr);
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


void MaptileComponent::RemoveAllGameobjects()
{
	while (GetGameobjects().size() > 0)
	{
		auto e = GetGameobjects()[0];
		RemoveGameobject(e);
	}
}


bool MaptileComponent::_hasGameobjectWithComponent(EComponentType type)
{
	for (int i = 0; i< m_Gameobjects.size(); i++)
	{
		auto ptr = m_Gameobjects[i];
		if (ptr->hasComponent(type))
		{
			return true;
		}
	}

	return false;
}


bool MaptileComponent::HasRoad()
{
	return _hasGameobjectWithComponent(EComponentType::CT_Road);
}


bool MaptileComponent::HasImprovement()
{
	return _hasGameobjectWithComponent(EComponentType::CT_Improvement);
}


bool MaptileComponent::HasForest()
{
	return _hasGameobjectWithComponent(EComponentType::CT_Forest);
}


bool MaptileComponent::HasCity()
{
	return _hasGameobjectWithComponent(EComponentType::CT_City);
}


bool MaptileComponent::HasFort()
{
	return _hasGameobjectWithComponent(EComponentType::CT_City);
}


bool MaptileComponent::HasUnit()
{
	return _hasGameobjectWithComponent(EComponentType::CT_Unit);
}

