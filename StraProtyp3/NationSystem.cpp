#include "NationSystem.h"

NationSystem* NationSystem::g_NationSystem = nullptr;


NationSystem* NationSystem::get()
{
	if (!g_NationSystem)
	{
		g_NationSystem = new NationSystem();
	}

	return g_NationSystem;
}



void NationSystem::del()
{
	if (g_NationSystem)
	{
		while (g_NationSystem->m_Nations.size() > 0)
		{
			g_NationSystem->m_Nations[0].reset();
			g_NationSystem->m_Nations.erase(g_NationSystem->m_Nations.begin());
		}

		g_NationSystem->m_DefinitionPath.clear();

		delete g_NationSystem;
		g_NationSystem = 0;
	}
}



bool NationSystem::Initialize(std::string filepath)
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

	XMLElement* nationDefintions = root->FirstChildElement("NationDefinitions");
	XMLElement* nation = nationDefintions->FirstChildElement("Nation");
	while (nation)
	{

		auto pNation = GameobjectStorage::get()->Instantiate("DefaultNation", -1, -1);
		auto pNationCmp = pNation->getComponent<NationComponent>("Nation");


		pNationCmp->SetNationName(nation->Attribute("name"));
		pNationCmp->SetNationRace(nation->Attribute("race"));


		// Get the Characteristics.
		XMLElement* characteristics = nation->FirstChildElement("Characteristics");
		XMLElement* chars = characteristics->FirstChildElement("Char");
		while (chars)
		{

			pNationCmp->SetCharacteristic(chars->Attribute("name"), chars->FloatText());
			chars = chars->NextSiblingElement("Char");
		}



		// Get the Stats.
		XMLElement* nationStats = nation->FirstChildElement("NationalStats");
		XMLElement* stat = nationStats->FirstChildElement("Stat");
		while (stat)
		{
			pNationCmp->SetStat(stat->Attribute("name"), stat->IntText());
			stat = stat->NextSiblingElement("Stat");
		}



		// Get the Technologies.
		XMLElement* nationTech = nation->FirstChildElement("ResearchedTechnology");
		XMLElement* tech = nationTech->FirstChildElement("Tech");
		while (tech)
		{
			pNationCmp->AddResearchedTechnology(tech->Attribute("name"));

			tech = tech->NextSiblingElement("Tech");
		}


		m_Nations.push_back(pNation);


		nation = nation->NextSiblingElement("Nation");
	}



	g_NationSystem->m_DefinitionPath = filepath;
	return true;
}


void NationSystem::AssignNationToPlayer(Pointer<GameObject2> player, NationID nation)
{
	for (int i = 0; i < m_Nations.size(); i++)
	{
		if (auto cmp = m_Nations[i]->getComponent<NationComponent>("Nation"); cmp)
		{
			if (cmp->GetNationName().compare(nation) == 0)
			{
				return;
			}
		}
	}
}