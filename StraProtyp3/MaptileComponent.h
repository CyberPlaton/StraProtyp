#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.


class GameobjectStorage;
class MaptileComponent : public Component2
{
	friend class GameobjectStorage;
public:
	MaptileComponent(const ComponentID& name)
	{
		initialize("Maptile", name);
	}

	~MaptileComponent()
	{
		while (m_Gameobjects.size() > 0)
		{
			m_Gameobjects[0].reset();
			m_Gameobjects.erase(m_Gameobjects.begin());
		}
		m_MaptileType.clear();
	}

	
	// Function for general update of the component, intended to be called once per frame.
	// Currently the function removes and resets elsewhere deleted gameobjects
	// which are stored in this Maptile vector.
	void update() override final;
	void destroy() override final;

	void AddGameobject(Pointer<GameObject2> p);
	void RemoveGameobject(const GOTag& t);
	void RemoveGameobject(Pointer<GameObject2> p);
	void RemoveAllGameobjects();
	std::vector< Pointer<GameObject2> >& GetGameobjects(){return m_Gameobjects;}


	MaptileType GetMaptileType() { return m_MaptileType; }
	void SetMaptileType(const MaptileType& t) { m_MaptileType = t; }

	bool HasRoad();
	bool HasImprovement();
	bool HasForest();
	bool HasCity();
	bool HasFort();
	bool HasUnit();


	bool HasWaterAccess() { return m_HasWaterAccess; }
	bool HasHill() { return m_HasHill; }
	bool HasMountain() { return m_HasMountain; }
	bool HasRiver() { return m_HasRiver; }
	bool HasRessource() { return m_HasRessource; }
	

	void SetHasWaterAccess(bool v) { m_HasWaterAccess = v; }
	void SetHasHill(bool v) { m_HasHill = v; }
	void SetHasRiver(bool v) { m_HasRiver = v; }
	void SetHasMountain(bool v) { m_HasMountain = v; }
	void SetHasRessource(bool v) { m_HasRessource = v; }


private:

	MaptileType m_MaptileType;

	std::vector< Pointer<GameObject2> > m_Gameobjects;

	bool m_HasWaterAccess;
	bool m_HasHill;
	bool m_HasMountain;
	bool m_HasRiver;
	bool m_HasRessource;



private:

	bool _hasGameobjectWithComponent(const std::string& componentTag);
};