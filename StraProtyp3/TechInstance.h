#pragma once

#include <map>
#include <stdexcept>

#include "Any.h"
#include "ITech.h"

using CheckDefinition = std::pair<ITech::CheckType, ITech::CheckArea>;

class TechInstance : public ITech
{
public:
	TechInstance(const std::string& path);

	TechID getID() const override final
	{
		return id;
	}


	bool checkWhetherAvailableForPlayer(IPlayer* player) override final
	{
		return false;
	}



	TechArea getTechArea()  override final
	{
		return techArea;
	}

	int getResearchPoints()  override final
	{
		return researchPoints;
	}

public:
	TechID id;
	int researchPoints = -1;
	TechArea techArea;


	// Map of checks.
	std::map< Any*, CheckDefinition> checks;
};