#include "UnitCreationSystem.h"

UnitCreationSystem* UnitCreationSystem::g_UnitCreationSystem = nullptr;


bool UnitCreationSystem::CreateUnit(Pointer<GameObject2> player, Pointer<GameObject2> city,
	UnitProfession unitName, float xpos, float ypos)
{
	return false;
}


bool UnitCreationSystem::CreateIndependentUnit(UnitProfession unitName, float xpos, float ypos)
{
	return false;
}


bool UnitCreationSystem::CreateUnitWithoutCheckInCity(Pointer<GameObject2> player, Pointer<GameObject2> city,
	UnitProfession unitName, float xpos, float ypos)
{
	return false;
}


bool UnitCreationSystem::CreateUnitWithoutCheckOutsideOfCity(Pointer<GameObject2> player,
	UnitProfession unitName, float xpos, float ypos)
{
	return false;
}



bool UnitCreationSystem::Initialize(std::string filepath)
{
	using namespace tinyxml2;
	using namespace std;

	// Create an object defined from xml.
	tinyxml2::XMLDocument doc;


	XMLError result = doc.LoadFile(filepath.c_str());
	if (result != XMLError::XML_SUCCESS)
	{
		// Clean up.
		doc.Clear();
		return false;
	}

	XMLNode* root = doc.FirstChild();
	if (root == nullptr)
	{
		// Clean up.
		doc.Clear();
		return false;
	}

	XMLElement* units = root->FirstChildElement("Units");
	XMLElement* unit = units->FirstChildElement("Unit");
	while (unit)
	{
		std::string path = std::string(units->Attribute("path")) + std::string(unit->Attribute("path"));

		_extractUnitRequirements(path);

		unit = unit->NextSiblingElement("Unit");
	}


	m_DefintionPath = filepath;
	return true;
}


void UnitCreationSystem::_extractUnitRequirements(std::string filepath)
{
	using namespace tinyxml2;
	using namespace std;

	// Create an object defined from xml.
	tinyxml2::XMLDocument doc;


	XMLError result = doc.LoadFile(filepath.c_str());
	if (result != XMLError::XML_SUCCESS)
	{
		// Clean up.
		doc.Clear();
		return;
	}

	XMLNode* root = doc.FirstChild();
	if (root == nullptr)
	{
		// Clean up.
		doc.Clear();
		return;
	}


	XMLElement* components = root->FirstChildElement("Components");
	XMLElement* data = nullptr;
	XMLElement* component = components->FirstChildElement("Component");
	while (component)
	{
		
		std::string name = component->Attribute("name");
		if (name.compare("Unit") == 0)
		{
			data = component;
			break;
		}


		component = component->NextSiblingElement("Component");
	}

	Pointer<UnitRequirement> cmp = std::make_shared<UnitRequirement>();
	XMLElement* e = data->FirstChildElement("Requirements");
	XMLElement* req = e->FirstChildElement("Req");
	cmp->prof = data->FirstChildElement("Profession")->GetText();
	while (req)
	{
		std::string reqType = req->Attribute("type");


		if (reqType.compare("tech") == 0)
		{
			cmp->m_TechRequirements.push_back(req->GetText());
		}
		else if (reqType.compare("race") == 0)
		{
			cmp->m_RaceRequirements.push_back(req->GetText());
		}
		else if (reqType.compare("ressource") == 0)
		{
			Tuple<std::string, int> reqTuple;
			reqTuple.x = req->GetText();
			reqTuple.y = req->IntAttribute("amount");

			cmp->m_RessourceRequirements.push_back(reqTuple);
		}


		req = req->NextSiblingElement("Req");
	}


	m_UnitRequirements.push_back(cmp);
}


bool UnitCreationSystem::ReloadDefinition()
{
	return Initialize(m_DefintionPath);
}


UnitCreationSystem* UnitCreationSystem::get()
{
	if (!g_UnitCreationSystem)
	{
		g_UnitCreationSystem = new UnitCreationSystem();
	}

	return g_UnitCreationSystem;
}


void UnitCreationSystem::del()
{
	if (g_UnitCreationSystem)
	{
		delete g_UnitCreationSystem;
		g_UnitCreationSystem = nullptr;
	}
}