#pragma once

#include <stdexcept>

#include "IPlayer.h"

#include "tinyxml2.h"


class PlayerInstance : public IPlayer
{
public:
	PlayerInstance(const std::string& path);
	~PlayerInstance()
	{
		techs.clear();
	}

	PlayerTechnologies& getPlayerTechnologies() override final
	{
		return techs;
	}


	PlayerTechnologies techs;
};