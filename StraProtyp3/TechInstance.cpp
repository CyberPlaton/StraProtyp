#include "TechInstance.h"

TechInstance::TechInstance(const std::string& path)
{
	using namespace tinyxml2;
	using namespace std;

	// Create an object defined from xml.
	tinyxml2::XMLDocument doc;


	XMLError result = doc.LoadFile(path.c_str());
	if (result != XMLError::XML_SUCCESS) throw std::runtime_error("Could not load file!");


	XMLNode* root = doc.FirstChild();
	if (root == nullptr) throw std::runtime_error("Could not load file!");

	// Get basic definition
	XMLElement* def = root->FirstChildElement("Definition");
	std::string name = def->FirstChildElement("Name")->GetText();
	std::string area = def->FirstChildElement("ResearchArea")->GetText();
	int p = stod(def->FirstChildElement("ResearchPoints")->GetText());

	id = name;
	techArea = ITech::getTechAreaFromText(area);
	researchPoints = p;
	baseWeight = def->FirstChildElement("BaseWeight")->FloatText(0.001f);
	subCategory = def->FirstChildElement("SubCategory")->GetText();

	// Get requirements
	XMLElement* reqs = root->FirstChildElement("Requirements");
	XMLElement* req = reqs->FirstChildElement("Req");


	while (req)
	{
		std::string area = req->Attribute("area");
		std::string type = req->Attribute("type");

		CheckType checkType = ITech::getCheckTypeFromText(type);
		CheckArea checkArea = ITech::getCheckAreaFromText(area);
		CheckDefinition checkDef = { checkType, checkArea };

		int iV;
		std::string what;

		switch (checkType)
		{
		case CheckType::CT_EQUAL:
		case CheckType::CT_GREATER:
		case CheckType::CT_SMALLER:
			iV = req->IntText(INT_MAX);

			checks.emplace(std::make_pair(new Any(iV, "int"), checkDef));
			break;

		case CheckType::CT_TRUE:
		case CheckType::CT_FALSE:
			what = req->GetText();

			// BUG:
			// If we have same "checkDef" it Overrides it!!!
			checks.emplace(std::make_pair(new Any(what, "string"), checkDef));
			break;
		}


		req = req->NextSiblingElement("Req");
	}
}