#pragma once

#include <vector>

#include "ITech.h"


class IBuilding
{
public:

	virtual std::vector<TechID> getRequiredTech() const = 0;

};