#include "TechnologySystem.h"

TechnologySystem* TechnologySystem::g_TechnologySystem = nullptr;


bool TechnologySystem::Initialize(const std::string& filepath)
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

	// Load General Technologies.
	XMLElement* technologies = root->FirstChildElement("General");
	std::string defaultPath = technologies->Attribute("path");
	XMLElement* tech = technologies->FirstChildElement("Tech");
	while (tech)
	{
		std::string path = defaultPath + tech->Attribute("path");

		cout << color(colors::CYAN);
		cout << "Loading Technology \""<< tech->Attribute("name") << "\"{"<< path << "}" << white << endl;

		// Add Prefab.
		GameobjectStorage::get()->AddPrefab(tech->Attribute("name"), path);

		// Load Prefab.
		auto p = GameobjectStorage::get()->Instantiate(tech->Attribute("name"), -1, -1);


		if (!p)
		{
			cout << color(colors::RED);
			cout << "[TechnologySystem::Initialize] Fatal: Could not Initialize Technology System \""<< white << endl;
			return false;
		}


		m_Technologies.push_back(p);


		tech = tech->NextSiblingElement("Tech");
	}

	// Load Race Specific Technologies.
	// ...




	return true;
}


TechnologySystem* TechnologySystem::get()
{
	if (!g_TechnologySystem)
	{
		g_TechnologySystem = new TechnologySystem();
	}


	return g_TechnologySystem;
}


void TechnologySystem::del()
{
	if (g_TechnologySystem)
	{
		while (g_TechnologySystem->m_Technologies.size() > 0)
		{
			g_TechnologySystem->m_Technologies[0].reset();
			g_TechnologySystem->m_Technologies.erase(g_TechnologySystem->m_Technologies.begin());
		}


		delete g_TechnologySystem;
	}
}