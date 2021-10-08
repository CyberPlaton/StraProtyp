#include "RandomNameGenerator.h"

RandomNamesGenerator* RandomNamesGenerator::g_RandomNamesGenerator = nullptr;


std::string RandomNamesGenerator::getUnitName()
{
	using namespace std;
	using namespace tinyxml2;

	tinyxml2::XMLDocument doc;

	XMLError result = doc.LoadFile(defaultEntityNamesPath.c_str());
	if (result != XMLError::XML_SUCCESS) return "none";

	XMLNode* root = doc.FirstChild();
	if (root == nullptr) return "none";


	XMLElement* units = root->FirstChildElement("Units");
	XMLElement* race = units->FirstChildElement("Human");
	XMLElement* suffixes = race->FirstChildElement("Suffixes");
	XMLElement* prefixes = race->FirstChildElement("Prefixes");

	int suffCount, prefCount;
	suffCount = suffixes->IntAttribute("count");
	prefCount = prefixes->IntAttribute("count");


	// Generate random index.
	int randPrefix = Random::InRange(1, prefCount);
	int randSuffix = Random::InRange(1, suffCount);
	

	XMLElement* prefix = prefixes->FirstChildElement("Prefix");

	int i = 1;

	std::string returnName = "none";

	// Get prefix.
	while (prefix)
	{

		// Return Name at Index.
		if (i >= randPrefix)
		{
			returnName = prefix->GetText();
			break;
		}

		prefix = prefix->NextSiblingElement("Prefix");
		i++;
	}



	
	// Get suffix.
	XMLElement* suffix = suffixes->FirstChildElement("Suffix");

	i = 1;
	while (suffix)
	{

		// Return Name at Index.
		if (i >= randSuffix)
		{

			returnName += suffix->GetText();
			break;
		}

		suffix = suffix->NextSiblingElement("Suffix");
		i++;
	}



	return returnName;
}


std::string RandomNamesGenerator::getBuildingName()
{
	using namespace std;
	using namespace tinyxml2;

	tinyxml2::XMLDocument doc;

	XMLError result = doc.LoadFile(defaultEntityNamesPath.c_str());
	if (result != XMLError::XML_SUCCESS) return "none";

	XMLNode* root = doc.FirstChild();
	if (root == nullptr) return "none";


	XMLElement* buildings = root->FirstChildElement("Buildings");
	XMLElement* names = buildings->FirstChildElement("Names");
	XMLElement* name = names->FirstChildElement("Name");
	int namesCount = names->IntAttribute("count");

	// Generate random index.
	int rand = Random::InRange(1, namesCount);
	int i = 1;

	while (name)
	{

		// Return Name at Index.
		if (i >= rand)
		{

			return name->GetText();
		}

		name = name->NextSiblingElement("Name");
		i++;
	}


	return "none";
}