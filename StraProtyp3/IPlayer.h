#pragma once

#include <map>

/*
* PlayerTechnologies stores technology names which are researched by the player,
* where 1 means researched and 0 means not.
*/
using PlayerTechnologies = std::map<std::string, int>;


class IPlayer
{
public:


	virtual PlayerTechnologies& const getPlayerTechnologies() = 0;

};