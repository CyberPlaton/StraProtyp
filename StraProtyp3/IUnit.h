#pragma once

#include <vector>

#include "ITech.h"


class IUnit
{
public:

	virtual std::vector<TechID> getRequiredTech() const = 0;

};