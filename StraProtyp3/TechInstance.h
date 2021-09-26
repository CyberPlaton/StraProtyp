#pragma once

#include <map>
#include <stdexcept>

#include "Any.h"
#include "ITech.h"
#include "ColorConsole.h"

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
		using namespace std;

		PlayerTechnologies& playerTechs = player->getPlayerTechnologies();

		bool requirementFulfilled = true;

		// Firstly, check whether player has already researched this technology,
		// if so, return false.
		if (playerTechs[this->getID()] == 1) return false;



		for (auto& req : checks)
		{
			if (req.first->type().compare("string") == 0)
			{
				// Try find requirement in player technologies.
				if (req.second.second == CheckArea::CA_TECH)
				{
					if (req.second.first == CheckType::CT_TRUE)
					{
						// Technology must be researched to be fulfilled.
						// Find Tech
						std::string reqTech = req.first->as<std::string>();
						auto it = playerTechs.find(reqTech);
						if (it != playerTechs.end())
						{
							// Check whether researched
							if (playerTechs[reqTech] == 1)
							{
								requirementFulfilled &= true;
							}
							else
							{
								return false;
							}
						}
						else
						{
							cout << color(colors::RED);
							cout << "Required Technology \""<< reqTech << "\" not found!" << white << endl;
							return false; 
						}
					}
					else
					{
						// Technology must NOT be researched to be fulfilled.
						// Find Tech
						std::string reqTech = req.first->as<std::string>();
						auto it = playerTechs.find(reqTech);
						if (it != playerTechs.end())
						{
							// Check whether researched
							if (playerTechs[reqTech] == 0)
							{
								requirementFulfilled &= true;
							}
							else
							{
								return false;
							}
						}
					}
				}
			}

		}


		return requirementFulfilled;
	}



	TechArea getTechArea()  override final
	{
		return techArea;
	}

	int getResearchPoints()  override final
	{
		return researchPoints;
	}

	TechSubcategory getSubcategory() override final
	{
		return subCategory;
	}

	float getBaseweight() override final
	{
		return baseWeight;
	}


	float getAccumulatedWeight(std::vector< TechInstance* > techs, IPlayer* player) override final
	{
		// Here we could use the player to check for special buildings that boost this technologies´
		// TechSubcategory...



		// We first have to Check,
		// whether this technology another parent technology has.
		for (auto& req : checks)
		{
			if (req.first->type().compare("string") == 0)
			{
				if (req.second.second == CheckArea::CA_TECH)
				{
					if (req.second.first == CheckType::CT_TRUE)
					{
						// If we are here, it means it has one.
						std::string parent = req.first->as<std::string>();


						// Return baseweight of this tech plus accumulated weight of the parent (recursion)
						ITech* t = nullptr;
						for (auto& it : techs)
						{
							if (it->getID().compare(parent) == 0)
							{
								t = it;
								break;
							}
						}

						return this->getBaseweight() + t->getAccumulatedWeight(techs, player);
					}
				}
			}
		}


		// If this Technology does not have any requirements for another technology,
		// than this is a parent tech, so return just the Baseweight (recursion end)
		return this->getBaseweight();
	}


public:
	TechID id;
	int researchPoints = -1;
	TechArea techArea;
	TechSubcategory subCategory;
	float baseWeight = 0.0f;

	// Map of checks.
	std::map< Any*, CheckDefinition> checks;
};