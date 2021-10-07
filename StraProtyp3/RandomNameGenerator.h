#pragma once

#include "ColorConsole.h"
#include "tinyxml2.h"

#include "Random.h"

/*
* "RandomNamesGenerator" generates names for units and buildings as they are created according to input data.
* 
* At this stage of development we simply acquire a random name from given ones.
*/
class RandomNamesGenerator
{
public:

	static RandomNamesGenerator* get()
	{
		if (!g_RandomNamesGenerator)
		{
			g_RandomNamesGenerator = new RandomNamesGenerator();
		}

		return g_RandomNamesGenerator;
	}


	static void del()
	{
		if (g_RandomNamesGenerator)
		{


			delete g_RandomNamesGenerator;
		}
	}


	// Currently we do not check for Race and other inputs.
	std::string getUnitName();
	std::string getBuildingName();

private:
	static RandomNamesGenerator* g_RandomNamesGenerator;


	std::string defaultEntityNamesPath = "Data/EntityNames.xml";


	RandomNamesGenerator() = default;
};