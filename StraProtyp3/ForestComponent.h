#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.



class GameobjectStorage;
class ForestComponent : public Component2
{
	friend class GameobjectStorage;
public:
	ForestComponent(const ComponentID& name)
	{
		initialize("Forest", name);
	}
	~ForestComponent()
	{
	}


	ForestType GetForestType() { return m_ForestType; }
	void SetForestType(const ForestType& t) { m_ForestType = t; }

	ForestBiome GetForestBiome() { return m_ForestBiome; }
	void SetForestBiome(const ForestBiome& b) { m_ForestBiome = b; }


	void ResetLifetime() { m_Lifetime = 0; }
	void IncrementLifetime() { m_Lifetime++; }
	void DecrementLifetime() { m_Lifetime = m_Lifetime > 1 ? m_Lifetime - 1 : 1; }
	int GetLifetime() { return m_Lifetime; }

	int GetMaxLifetime() { return m_MaxLifetime; }
	void SetMaxLifetime(int n) { m_MaxLifetime = n; }

private:

	ForestBiome m_ForestBiome;
	ForestType m_ForestType;
	int m_Lifetime = 0;
	int m_MaxLifetime = 0;
};