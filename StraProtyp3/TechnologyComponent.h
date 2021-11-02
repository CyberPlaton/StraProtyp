#pragma once

#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.

#include "Any.h"

class GameobjectStorage;


class TechnologyComponent : public Component2
{
	friend class GameobjectStorage;


	struct Requirement
	{
		Requirement() = default;
		Requirement(Pointer<Any> value, const std::string& checkArea, const std::string& checkType)
			: m_CheckArea(checkArea), m_CheckType(checkType)
		{
			m_Value = value;
		}
		~Requirement()
		{
			m_Value.reset();
			m_CheckArea.clear();
			m_CheckType.clear();
		}

		// For example "tundra", "human" or "Heavy Armor"...
		Pointer<Any> m_Value;

		// For example "player_city_check" or "player_race_check"...
		std::string m_CheckArea;

		// For example "true" or "greater"...
		std::string m_CheckType;
	};


public:
	TechnologyComponent(const ComponentID& name)
	{
		initialize("Technology", name);
	}
	~TechnologyComponent()
	{
	}


	TechID GetTechID();
	TechArea GetTechArea();
	TechSubcategory GetTechSubcategory();
	float GetBaseWeight();
	int GetResearchPoints();


	void SetTechID(const TechID& v) { m_TechName = v; }
	void SetTechArea(const TechArea& v) { m_TechArea = v; }
	void SetTechSubcategory(const TechSubcategory& v) { m_TechSubcategory = v; }
	void SetBaseWeight(float v) { m_BaseWeight = v; }
	void SetResearchPoints(int v){m_ResearchPoints = v;}
	void AddRequirement(Pointer<Requirement> r) { m_Requirements.push_back(r); }

private:

	TechID m_TechName;
	TechArea m_TechArea;
	TechSubcategory m_TechSubcategory;
	float m_BaseWeight = 0.0f;
	int m_ResearchPoints = 0;


	// All requirements to research this Technology.
	std::vector< Pointer< Requirement > > m_Requirements;
};
