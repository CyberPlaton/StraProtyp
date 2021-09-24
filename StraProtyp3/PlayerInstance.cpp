#include"PlayerInstance.h"

PlayerInstance::PlayerInstance(const std::string& path)
{
	using namespace tinyxml2;
	using namespace std;

	// Create an object defined from xml.
	tinyxml2::XMLDocument doc;


	XMLError result = doc.LoadFile(path.c_str());
	if (result != XMLError::XML_SUCCESS) throw std::runtime_error("Could not load file!");


	XMLNode* root = doc.FirstChild();
	if (root == nullptr) throw std::runtime_error("Could not load file!");


	std::string raceName = root->FirstChildElement("Race")->GetText();


	XMLElement* techs = root->FirstChildElement("Technologies");
	XMLElement* tech = techs->FirstChildElement("Tech");

	while (tech)
	{
		std::string techName = tech->GetText();
		bool researched = tech->BoolAttribute("researched", false);
		int r = researched == true ? 1 : 0;


		this->techs.emplace(techName, r);


		tech = tech->NextSiblingElement("Tech");
	}
}
