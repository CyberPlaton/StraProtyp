#include "TechnologySystem.h"

TechnologySystem* TechnologySystem::g_TechnologySystem = nullptr;


bool TechnologySystem::Initialize(std::string filepath)
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


void TechnologySystem::SetTechnologyResearched(Pointer< GameObject2 > player, TechID id, bool status)
{
	auto p = player->getComponent<PlayerComponent>("Player");
	if (status)
	{
		p->AddResearchedTechnology(id);
	}
	else
	{
		p->RemoveResearchedTechnology(id);
	}
}


void TechnologySystem::VisualizeTechnologyTree(Pointer< GameObject2 > player)
{
	ImGui::Begin("Technology Tree");


	if (!m_TechTreeInitialized)
	{
		for (int i = 0; i < m_Technologies.size(); i++)
		{
			auto tech = m_Technologies[i]->getComponent<TechnologyComponent>("Technology");

			std::string tech_name = tech->GetTechID();
			std::string tech_area = tech->GetTechArea();
			std::string tech_category = tech->GetTechSubcategory();

			m_TechNodes.push_back(new Node(tech_name, m_TechNodeID++, tech_area, tech_category));
		}

		m_TechTreeInitialized = true;
	}


	auto researched_techs = player->getComponent<PlayerComponent>("Player")->GetTechnologies();


	ImNodes::BeginNodeEditor();


	for (int i = 0; i < m_TechNodes.size(); i++)
	{
		bool researched = false;

		researched = _hasResearchedTech(m_TechNodes[i]->name, researched_techs);


		if (researched)
		{
			// Change the Color of the Node if player has researched it.
			ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(0, 100, 0, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(189, 183, 107, 255));
			ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(0, 200, 0, 255));
		}


		ImNodes::BeginNode(m_TechNodes[i]->id);

		// After adjusting the Node position to a final spot,
		// add call to function "ImNodes::SetNodeDraggable( m_TechNodes[i]->id, false );"
		// so that they become static and unmovable.


		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted(m_TechNodes[i]->name.c_str());
		ImNodes::EndNodeTitleBar();


		ImNodes::EndNode();


		if (researched)
		{
			ImNodes::PopColorStyle();
			ImNodes::PopColorStyle();
			ImNodes::PopColorStyle();
		}
	}


	ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight);
	ImNodes::EndNodeEditor();


	ImGui::End();
}




void TechnologySystem::ChooseNextTechnology(Pointer< GameObject2 > player, TechArea area)
{
	std::vector< TechID > to_choose_from; // For displaying the tech from which to choose.
	std::vector< Pointer< GameObject2 > > researchable; // Tech which can be researched by the player.
	std::map< TechID, float > researchable_probability_distribution; // Prob of each Tech.

	// Get all technologies in specified Area and in
	// the Base area not belonging to any area.
	for (int i = 0; i < m_Technologies.size(); i++)
	{
		if (m_Technologies[i]->getComponent<TechnologyComponent>("Technology")->GetTechArea().compare(area) == 0 ||
			m_Technologies[i]->getComponent<TechnologyComponent>("Technology")->GetTechArea().compare("base") == 0)
		{
			//researchable.push_back(m_Technologies[i]);
						
			// Get all technologies which are available for the player to be researched
			// in the specified area and in the base area.
			if (_checkWhetherAvailableForPlayer(player, m_Technologies[i]))
			{
				researchable.push_back(m_Technologies[i]);
			}
		}
	}


	// Before executing the algorithm, check whether we have more than 3 Technologies to choose from,
	// else return them directly.
	if (researchable.size() <= 3)
	{
		std::vector< TechID > r;
		for (auto& tech : researchable)
		{
			r.push_back(tech->getComponent<TechnologyComponent>("Technology")->GetTechID());
		}

		player->getComponent< PlayerComponent >("Player")->SetResearchableTechnology(area, r);
		return;
	}


	// Compute Probability of Sample Space, which is the 
	// "base value" plus "path value" for all researchable techs.
	float sample_space_value = 0.0f;
	for (auto& tech : researchable)
	{
		sample_space_value += _getAccumulatedPathProbabilityWeightForTech(player, tech);
	}



	// Compute the probability value for each technology according to the Sample Space.
	float percent = 0.0f;
	float sum = 0.0f;
	for (auto& tech : researchable)
	{
		float weighted_prob = _getAccumulatedPathProbabilityWeightForTech(player, tech) / sample_space_value;
		researchable_probability_distribution.emplace(tech->getComponent<TechnologyComponent>("Technology")->GetTechID(), weighted_prob);

		sum += weighted_prob;
		percent += weighted_prob * 100.0f;
	}


	// Choose with given probability values three Technologies.
	// Find Technology which is closest over the value.
	while (to_choose_from.size() < 3)
	{
		// Get a random throw
		float random = Random::InRange(0.0f, 1.0f);
		float cumulative_probability = 0.0f;


		float currentMin = (float)INT_MAX;
		TechID tech_to_be_added;
		for (auto& tech : researchable_probability_distribution)
		{
			// Increase cumulative Probability.
			cumulative_probability += tech.second;

			if (cumulative_probability > random)
			{
				if (tech.second < currentMin)
				{
					tech_to_be_added = tech.first;
					currentMin = tech.second;
					break;
				}
			}
		}

		// Check whether we already appended the technology.
		bool tech_already_appended = false;
		for (auto& tech : to_choose_from)
		{
			if (tech.compare(tech_to_be_added) == 0)
			{
				tech_already_appended = true;
				break;
			}
		}

		if (!tech_already_appended)
		{
			to_choose_from.push_back(tech_to_be_added);
		}
	}


	// Give the player his technologies to choose from later.
	player->getComponent< PlayerComponent >("Player")->SetResearchableTechnology(area, to_choose_from); 
}



bool TechnologySystem::_hasResearchedTech(TechID id, std::vector< TechID >& techs)
{
	auto it = std::find(techs.begin(), techs.end(), id);
	return it != techs.end();
}



float TechnologySystem::_getAccumulatedPathProbabilityWeightForTech(Pointer< GameObject2> player, Pointer< GameObject2 > tech)
{
	// Here we could use the player to check for special buildings that boost this technologies´
	// TechSubcategory...


	// We first have to Check,
	// whether this technology has another parent technology.
	auto t = tech->getComponent< TechnologyComponent >("Technology");
	auto requirements = t->GetRequirements();

	for (int i = 0; i < requirements.size(); i++)
	{
		auto req = requirements[i];

		if (req->m_CheckArea.compare("player_tech_check") == 0)
		{

			if (req->m_CheckType.compare("true") == 0)
			{

				// If we are here, then this technology has a parent technology.
				Pointer< GameObject2 > parent;
				for (int j = 0; j < m_Technologies.size(); j++)
				{
					if (m_Technologies[j]->getComponent< TechnologyComponent >("Technology")->GetTechID().compare(req->m_Value->as<std::string>()) == 0)
					{
						// Found the parent.
						parent = m_Technologies[j];
						break;
					}
				}

				// Recursive accumulating of weight.
				return t->GetBaseWeight() + _getAccumulatedPathProbabilityWeightForTech(player, parent);
			}
		}
	}

	// Recursion end if no parent.
	return t->GetBaseWeight();
}



bool TechnologySystem::_checkWhetherAvailableForPlayer(Pointer< GameObject2> player, Pointer< GameObject2 > tech)
{
	using namespace std;

	auto p = player->getComponent< PlayerComponent >("Player");
	auto t = tech->getComponent< TechnologyComponent >("Technology");

	// Print out general information about the player.
	cout << color(colors::MAGENTA);
	printf("[TechnologySystem::_checkWhetherAvailableForPlayer] Player: \"%s\"\tTech: \"%s\" \n", player->getName().c_str(), tech->getName().c_str());
	cout << white;

	cout << color(colors::CYAN);
	printf("Researched Technologies: \n");
	cout << white;
	for (int i = 0; i < p->GetTechnologies().size(); i++)
	{
		cout << color(colors::WHITE);
		printf("\t%d.) \"%s\"", i + 1, p->GetTechnologies()[i].c_str());
		cout << white << endl;
	}


	std::vector< TechID >& playerTechs = p->GetTechnologies();


	// Firstly, check whether player has already researched this technology,
	// if so, return false.
	if (std::find(playerTechs.begin(), playerTechs.end(), t->GetTechID()) != playerTechs.end())
	{
		cout << color(colors::GREEN);
		cout << "\t\t OK (Technology already researched)" << white << endl;
		return false;
	}



	cout << color(colors::CYAN);
	printf("\nRequirements: \n");
	cout << white;

	auto requirements = t->GetRequirements();
	bool requirementFulfilled = true;

	for (int i = 0; i < requirements.size(); i++)
	{
		auto req = requirements[i];

		if (req->m_CheckArea.compare("player_tech_check") == 0)
		{
			if (req->m_CheckType.compare("true") == 0)
			{
				cout << color(colors::DARKMAGENTA);
				printf("\t%s(%s): \"%s\"", req->m_CheckArea.c_str(), req->m_CheckType.c_str(), req->m_Value->as<std::string>().c_str());
				cout << white;

				// Check whether player has researched needed technology.
				if (std::find(playerTechs.begin(), playerTechs.end(), req->m_Value->as<std::string>()) != playerTechs.end())
				{
					requirementFulfilled &= true;
				}
				else
				{
					requirementFulfilled &= false;
				}
			}
			else
			{
				// Check whether player has NOT researched needed technology.
				if (std::find(playerTechs.begin(), playerTechs.end(), req->m_Value->as<std::string>()) == playerTechs.end())
				{
					requirementFulfilled &= true;
				}
				else
				{
					requirementFulfilled &= false;
				}
			}
			


			if (!requirementFulfilled)
			{
				cout << color(colors::RED);
				cout << "\t\t X" << white << endl;
			}
			else
			{
				cout << color(colors::GREEN);
				cout << "\t\t OK" << white << endl;
			}


		}
	}


	if (!requirementFulfilled)
	{
		cout << color(colors::RED);
		cout << "\t\t\t\t\t\t Can not be researched" << white << endl;
	}
	else
	{
		cout << color(colors::GREEN);
		cout << "\t\t\t\t\t\t Can be researched" << white << endl;
	}

	cout << endl << endl;
	return requirementFulfilled;
}