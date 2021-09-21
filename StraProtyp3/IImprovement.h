#pragma once

#include <vector>

#include "ITech.h"


class IImprovement
{
public:

	virtual std::vector<TechID> getRequiredTech() const = 0;

};