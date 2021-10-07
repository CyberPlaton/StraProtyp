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
	XMLElement* names = units->FirstChildElement("Names");
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