#include "main.h"

size_t App::g_DecalInstanceID = 1;

/*
* IMGUI related variables, for easier iterating.
*/
static bool imgui_demo_window = false;
static bool gameobjects_window = true;
static bool imgui_has_focus = false;
static bool show_nav_mesh = false;
static bool show_nav_path = false;
static bool show_agent_action = false;
static bool show_collisions = false;
static bool show_tech_tree = false;

/*
* Helper, maybe GUI related vars.
*/
static Pointer<GameObject2> selected_ui_gameobject = nullptr;
static Pointer<GameObject2> selected_gameobject = nullptr;
static int imnodes_tech_node_id = 1;
static int imnodes_tech_link_id = 1;
static int imnodes_tech_dependency_id = 1;
static int imnodes_tech_dependency_display_id = 100000;
static bool imnodes_tech_tree_initialized = false;
static bool render_2d_grid = false;
static bool render_city_religions = false;
static bool show_city_religion_update_button = false;

/*
* Religion Helpers.
*/
static std::vector<std::string> availableColors;
static int nextColorIndex = 0;
static std::map< std::string, std::string > religionColors;
static std::map< std::string, double > worldReligionsStrengths;
static Timer religionStepTimer;
static bool religionContinuousUpdateStarted = false;
static bool religionContinuousUpdate = false;


/*
* Forest Helpers.
*/
static bool forestUpdateStarted = false;

int main()
{
	App demo;
	if (demo.Construct(1360, 768, 1, 1))
		demo.Start();

	demo.shutDown();

	NavMesh::del();
	GameWorldTime::del();
	JobSystem::del();
	ForestSystem::del();
	TechnologySystem::del();
	ReligionSystem::del();
	GameobjectStorage::del();

	ImNodes::SaveCurrentEditorStateToIniFile("tech_tree_graph.ini");
	ImGui::DestroyContext();
	ImNodes::DestroyContext();

#ifdef _DEBUG
	_CrtCheckMemory();
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}




void App::DrawUI(void)
{
	//This finishes the Dear ImGui and renders it to the screen
	pge_imgui.ImGui_ImplPGE_Render();
}


bool App::OnUserUpdate(float fElapsedTime)
{
	using namespace std;

	Clear(olc::BLACK);

	if (stateMachine.getCurrentState().compare("worldMap") == 0)
	{
		_updateVisibleRect(); // Update the Rect of the Worlmap which is visible for App owner.
	}

	_handleInput(); // Handle the users input.
		


	// Update all Maptile Components.
	for (auto& m : GetAllComponentsOfType<MaptileComponent>("Maptile"))
	{
		m->update();
	}


	stateMachine.update(fElapsedTime);

	SetDrawTarget((uint8_t)m_GameLayer);
	DrawStringDecal(olc::vf2d(1200, 50), "FPS: " + std::to_string(GetFPS()));
	font->DrawStringPropDecal(olc::vf2d(1200, 60), "FPS: " + std::to_string(GetFPS()), olc::CYAN, { 0.5f, 0.5f });

	return true;
}



bool App::_initGameworldMatrix()
{
	// Set the dimensions as given.
	gameWorldMatrix.resize(DEFAULT_MAPSIZE_X);
	for (int i = 0; i < DEFAULT_MAPSIZE_X; i++)
	{
		gameWorldMatrix[i].resize(DEFAULT_MAPSIZE_Y);
	}

	return true;
}


bool App::OnUserCreate()
{
	using namespace std;


	GameobjectStorage::get(); // Initiaization
	GameobjectStorage::get()->LoadPrefabs("Data/PrefabDefinition.xml"); // Loading of all "Prefabs"


	// Define available colors for religions.
	availableColors.push_back("green");
	availableColors.push_back("dark_green");
	availableColors.push_back("dark_blue");
	availableColors.push_back("red");
	availableColors.push_back("dark_red");
	availableColors.push_back("yellow");
	availableColors.push_back("dark_yellow");
	availableColors.push_back("magenta");
	availableColors.push_back("dark_magenta");


	// Create Font
	font = new olc::Font("Data/Assets/Font/data-latin.png");

	// Init Random
	Random::Init();

	// Init ImNodes.
	ImNodes::CreateContext();
	ImNodes::SetNodeGridSpacePos(1, ImVec2(50.0f, 50.0f));
	ImNodes::LoadCurrentEditorStateFromIniFile("tech_tree_graph.ini");


	// Load Assets and Definitions etc.
	if (!_loadDecalDatabase()) return false;
	if (!_loadAppStateDefinitions()) return false;
	if (!_initGameworldMatrix()) return false;
	if (!TechnologySystem::get()->Initialize("Data/TechnologyDefinition.xml")) return false;
	if (!NationSystem::get()->Initialize("Data/NationSystem.xml")) return false;
	if (!UnitCreationSystem::get()->Initialize("Data/UnitCreationSystem.xml")) return false;


	m_GameLayer = CreateLayer();
	EnableLayer(m_GameLayer, true);
	SetLayerCustomRenderFunction(0, std::bind(&App::DrawUI, this));


	// Create Transformed View renderer with given Screen Dimension and given Tile Size (256, 256).
	tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { DEFAULT_DECAL_SIZE_X, DEFAULT_DECAL_SIZE_Y });

	// Initialize HID Manager
	g_HIDManager = new olc::hid::OLCHIDManager(new olc::hid::OLCMouse(this), new olc::hid::OLCKeyboard(this));

	
	GameWorldTime::get()->setTimeSpeed(0.016);



	for (int i = 0; i < gameWorldMatrix.size(); i++)
	{
		for (int j = 0; j < gameWorldMatrix[i].size(); j++)
		{
			if (j <= 3)
			{
				gameWorldMatrix[i][j] = GameobjectStorage::get()->Instantiate("Snow_Maptile", i, j);
			}
			else if (j <= 7)
			{
				gameWorldMatrix[i][j] = GameobjectStorage::get()->Instantiate("Tundra_Maptile", i, j);
			}
			else if (j <= 11)
			{
				gameWorldMatrix[i][j] = GameobjectStorage::get()->Instantiate("Temperate_Maptile", i, j);
			}
			else if (j <= 15)
			{
				gameWorldMatrix[i][j] = GameobjectStorage::get()->Instantiate("Savannah_Maptile", i, j);
			}
			else
			{
				gameWorldMatrix[i][j] = GameobjectStorage::get()->Instantiate("Jungle_Maptile", i, j);
			}
		}
	}


	auto vec = GetAllComponentsOfType<MaptileComponent>("Maptile");
	for (auto& m : vec)
	{
		while (m->GetGameobjects().size() > 0)
		{
			auto e = m->GetGameobjects()[0];
			m->RemoveGameobject(e);
		}
	}



	
	auto ptr = GameobjectStorage::get()->Instantiate("City_Plain", 3, 2);
	ptr->setName("Durotar");
	ReligionSystem::get()->CreateReligion(ptr, "Horde");
	ReligionSystem::get()->SetReligionColor("Horde", 255, 0, 50, 255);
	ptr.reset();

	ptr = GameobjectStorage::get()->Instantiate("Road", 3, 2);
	ptr = GameobjectStorage::get()->Instantiate("Road", 2, 2);
	ptr = GameobjectStorage::get()->Instantiate("Road", 1, 2);
	ptr = GameobjectStorage::get()->Instantiate("Road", 4, 2);
	ptr = GameobjectStorage::get()->Instantiate("Road", 5, 2);



	ptr = GameobjectStorage::get()->Instantiate("City_Plain", 8, 3);
	ptr->setName("Orgrimmar");
	ReligionSystem::get()->CreateReligion(ptr, "Lich King");
	ReligionSystem::get()->SetReligionColor("Lich King", 255, 150, 0, 255);
	ptr.reset();


	ptr = GameobjectStorage::get()->Instantiate("City_Plain", 5, 18);
	ptr->setName("Valenwood");
	ReligionSystem::get()->CreateReligion(ptr, "Forest Pact");
	ReligionSystem::get()->SetReligionColor("Forest Pact", 0, 200, 0, 255);
	ptr.reset();


	ptr = GameobjectStorage::get()->Instantiate("City_Plain", 25, 16);
	ptr->setName("Mournhold");
	ReligionSystem::get()->CreateReligion(ptr, "Forest Pact");
	ptr.reset();



	ptr = GameobjectStorage::get()->Instantiate("Snow_Deep", 5, 1);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();

	ptr = GameobjectStorage::get()->Instantiate("Snow_Deep", 5, 0);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();


	ptr = GameobjectStorage::get()->Instantiate("Snow_Deep", 7, 1);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();


	ptr = GameobjectStorage::get()->Instantiate("Snow_Deep", 6, 3);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();



	ptr = GameobjectStorage::get()->Instantiate("Tundra_Deep", 2, 4);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();


	ptr = GameobjectStorage::get()->Instantiate("Temperate_Deep", 3, 8);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();

	ptr = GameobjectStorage::get()->Instantiate("Temperate_Deep", 5, 9);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();

	ptr = GameobjectStorage::get()->Instantiate("Temperate_Deep", 4, 10);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);

	ptr = GameobjectStorage::get()->Instantiate("Temperate_Deep", 5, 11);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);


	ptr = GameobjectStorage::get()->Instantiate("Savannah_Deep", 19, 14);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();

	ptr = GameobjectStorage::get()->Instantiate("Savannah_Deep", 21, 14);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();

	ptr = GameobjectStorage::get()->Instantiate("Savannah_Deep", 20, 15);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();



	ptr = GameobjectStorage::get()->Instantiate("Jungle_Deep", 3, 18);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();


	ptr = GameobjectStorage::get()->Instantiate("Jungle_Deep", 1, 18);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();


	ptr = GameobjectStorage::get()->Instantiate("Jungle_Deep", 2, 19);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();


	ptr = GameobjectStorage::get()->Instantiate("Jungle_Deep", 4, 17);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();


	ptr = GameobjectStorage::get()->Instantiate("Jungle_Deep", 5, 16);
	ptr->getComponent<ForestComponent>("Forest")->SetIsForestPermanent(true);
	ptr.reset();


	ptr = GameobjectStorage::get()->Instantiate("Mountain_Snow", 4, 0);
	ptr.reset();

	ptr = GameobjectStorage::get()->Instantiate("Mountain_Sand", 5, 8);
	ptr.reset();

	ptr = GameobjectStorage::get()->Instantiate("Mountain_Savannah", 4, 9);
	ptr.reset();

	ptr = GameobjectStorage::get()->Instantiate("Mountain_Temperate", 18, 8);
	ptr.reset();

	ptr = GameobjectStorage::get()->Instantiate("Mountain_Tundra", 0, 4);
	ptr.reset();

	ptr = GameobjectStorage::get()->Instantiate("Mountain_Jungle", 9, 17);
	ptr.reset();



	ptr = GameobjectStorage::get()->Instantiate("River", 0, 0);
	ptr = GameobjectStorage::get()->Instantiate("River", 1, 0);
	ptr = GameobjectStorage::get()->Instantiate("River", 2, 0);
	ptr = GameobjectStorage::get()->Instantiate("River", 3, 0);
	ptr = GameobjectStorage::get()->Instantiate("River", 4, 0);
	ptr = GameobjectStorage::get()->Instantiate("River", 5, 0);


	ptr = GameobjectStorage::get()->Instantiate("River", 0, 7);
	ptr = GameobjectStorage::get()->Instantiate("River", 1, 7);
	ptr = GameobjectStorage::get()->Instantiate("River", 2, 7);
	ptr = GameobjectStorage::get()->Instantiate("River", 3, 7);
	ptr = GameobjectStorage::get()->Instantiate("River", 4, 7);
	ptr = GameobjectStorage::get()->Instantiate("River", 5, 7);
	ptr = GameobjectStorage::get()->Instantiate("River", 6, 7);
	ptr = GameobjectStorage::get()->Instantiate("River", 7, 7);
	ptr = GameobjectStorage::get()->Instantiate("River", 8, 7);
	ptr = GameobjectStorage::get()->Instantiate("River", 9, 7);
	ptr = GameobjectStorage::get()->Instantiate("River", 10, 7);
	ptr = GameobjectStorage::get()->Instantiate("River", 11, 7);
	ptr = GameobjectStorage::get()->Instantiate("River", 12, 7);

	ptr = GameobjectStorage::get()->Instantiate("River", 0, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 1, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 2, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 3, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 4, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 5, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 6, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 7, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 8, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 9, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 10, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 11, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 12, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 13, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 14, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 15, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 16, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 17, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 18, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 19, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 20, 11);
	ptr = GameobjectStorage::get()->Instantiate("River", 21, 11);

	ptr = GameobjectStorage::get()->Instantiate("River", 0, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 1, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 2, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 3, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 4, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 5, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 6, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 7, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 8, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 9, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 10, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 11, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 12, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 13, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 14, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 15, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 16, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 17, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 18, 15);
	ptr = GameobjectStorage::get()->Instantiate("River", 19, 15);


	NavMesh::get()->Bake();

	return true;
}



/*
std::vector< TechInstance* > App::getNextTechToChoose(IPlayer* player, ITech::TechArea area)
{
	using namespace std;

	std::vector< TechInstance* > researchable;
	std::map< TechInstance*, float > researchable_probability_distr;
	std::vector< TechInstance* > return_vec;
	std::vector< TechInstance* > specAreaTechs;

	// First Get all defined area technologies available..
	for (auto& tech : techTree)
	{
		// .. for specified area AND for the basis.
		// As the basis is researched there will be no more of them and
		// only specified area will be left.
		if (tech->getTechArea() == area || 
			tech->getTechArea() == ITech::TA_BASIC)
		{
			specAreaTechs.push_back(tech);
		}
	}


	// Get all Technologies which are available for the player to research from given area.
	for (auto& tech : specAreaTechs)
	{
		if (tech->checkWhetherAvailableForPlayer(player))
		{
			researchable.push_back(tech);
		}
	}


	// Before executing the algorithm, check whether we have more than 3 Technologies to choose from,
	// else return them directly.
	if (researchable.size() <= 3)
	{
		return researchable;
	}



	// Compute Probability of Sample Space, which is the 
	// "base value" plus "path value" for all researchable techs.
	float sample_space_value = 0.0f;
	for (auto& tech : researchable)
	{
		sample_space_value += tech->getAccumulatedWeight(techTree, player);
	}

	cout << color(colors::RED);
	cout << "Sample Space Probability Value: " << sample_space_value << white << endl;

	// Compute the probability value for each technology according to the Sample Space.
	float percent = 0.0f;
	float sum = 0.0f;
	for (auto& tech : researchable)
	{
		float weighted_prob = tech->getAccumulatedWeight(techTree, player) / sample_space_value;
		researchable_probability_distr.emplace(tech, weighted_prob);


		cout << color(colors::YELLOW);
		cout << "\"" << tech->getID() << "\" : {\"" << weighted_prob << "\"} = " << weighted_prob * 100.0f << "%" << endl;

		sum += weighted_prob;
		percent += weighted_prob * 100.0f;
	}
	cout << "Sum: \"" << sum << "\" = " << percent << "%" << white << endl;


	// Choose with given probability values three Technologies.

	// Find Technology which is closest over the value.
	while (return_vec.size() < 3)
	{
		// Get a random throw
		float random = Random::InRange(0.0f, 1.0f);
		float cumulative_probability = 0.0f;


		float currentMin = (float)INT_MAX;
		TechInstance* tech_to_be_added = nullptr;
		for (auto& tech : researchable_probability_distr)
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
		for (auto& tech : return_vec)
		{
			if (tech->getID().compare(tech_to_be_added->getID()) == 0)
			{
				tech_already_appended = true;
				break;
			}
		}

		if (!tech_already_appended)
		{
			return_vec.push_back(tech_to_be_added);
		}
	}


	// For now return only first three researchable techs.
	return return_vec;
}
*/


void App::_handleInput()
{
	using namespace std;

	g_HIDManager->update();
	olc::vf2d point = tv.ScreenToWorld({ GetMouseX(), GetMouseY() });
	float mousex = point.x;
	float mousey = point.y;


	// Handle Movement and Paning Input only for WorldMap,
	// where we use the Tiling Transformation.
	if (stateMachine.getCurrentState().compare("worldMap") == 0)
	{

		/*
		cout << color(colors::RED);
		cout << "MousePos {" << mousex << "," << mousey << "}" << endl;
		cout << "TopLeft Maptile {" << topLeft.x << "," << topLeft.y << "}" << endl;
		cout << "BottomRight Maptile {" << bottomDown.x << "," << bottomDown.y << "}" << endl;
		cout << "Middle Maptile {" << middle.x << "," << middle.y << "}" << white << endl;
		*/

		olc::vi2d topLeft = tv.GetTileUnderScreenPos({ 0, 0 });
		olc::vi2d bottomDown = tv.GetBottomRightTile();
		olc::vi2d middle = { bottomDown.x / 2, bottomDown.y / 2 };

		// Is Mouse pressed on Forest.
		if (GetMouse(0).bPressed)
		{
			auto maptile = gameWorldMatrix[mousex][mousey]->getComponent<MaptileComponent>("Maptile");
			if (maptile->HasForest())
			{
				for (auto e : maptile->GetGameobjects())
				{
					if (e->hasComponent("Forest"))
					{
						selected_gameobject = e;
						break;
					}
				}
			}
			else if (maptile->HasCity())
			{
				for (auto e : maptile->GetGameobjects())
				{
					if (e->hasComponent("City"))
					{
						selected_gameobject = e;
						break;
					}
				}
			}
			else
			{
				selected_gameobject.reset();
			}
		}

		if (selected_gameobject)
		{
			if (GetKey(olc::Key::DEL).bPressed)
			{
				Pointer<TransformComponent> transform = selected_gameobject->getComponent<TransformComponent>("Transform");
				auto maptile = gameWorldMatrix[(int)transform->GetXPos()][(int)transform->GetYPos()]->getComponent<MaptileComponent>("Maptile");
			

				maptile->RemoveGameobject(selected_gameobject);
				selected_gameobject.reset();
			}
		}


		// Do not allow capturing input to imgui and app at same time.
		if (!imgui_has_focus)
		{
			if (GetKey(olc::Key::TAB).bPressed)
			{
				imgui_demo_window = (imgui_demo_window == false) ? true : false;
				gameobjects_window = (gameobjects_window == false) ? true : false;
			}


			if (GetMouse(2).bPressed)
			{
				isPanning = true;
				tv.StartPan(GetMousePos());
			}
			if (GetMouse(2).bHeld) tv.UpdatePan(GetMousePos());
			if (GetMouse(2).bReleased)
			{
				isPanning = false;
				tv.EndPan(GetMousePos());
			}
			if (GetMouseWheel() > 0) tv.ZoomAtScreenPos(2.0f, GetMousePos());
			if (GetMouseWheel() < 0) tv.ZoomAtScreenPos(0.5f, GetMousePos());

			if (!isPanning)
			{
				// WASD Movement,
				// for when we do not use the Mouse for panning.
				if (GetKey(olc::SHIFT).bHeld)
				{
					cameraSpeed = 4;
				}
				else
				{
					cameraSpeed = 1;
				}
				if (GetKey(olc::W).bPressed || GetKey(olc::S).bPressed ||
					GetKey(olc::A).bPressed || GetKey(olc::D).bPressed)
				{
					camerax = tv.GetTileUnderScreenPos({ 0, 0 }).x;
					cameray = tv.GetTileUnderScreenPos({ 0, 0 }).y;
					tv.StartPan({ camerax, cameray });
				}
				if (GetKey(olc::W).bHeld || GetKey(olc::S).bHeld ||
					GetKey(olc::A).bHeld || GetKey(olc::D).bHeld)
				{
					if (GetKey(olc::W).bHeld)
					{
						cameray += 3 * cameraSpeed;
						tv.UpdatePan({ camerax, cameray });
					}
					if (GetKey(olc::S).bHeld)
					{
						cameray -= 3 * cameraSpeed;
						tv.UpdatePan({ camerax, cameray });
					}
					if (GetKey(olc::A).bHeld)
					{
						camerax += 3 * cameraSpeed;
						tv.UpdatePan({ camerax, cameray });
					}
					if (GetKey(olc::D).bHeld)
					{
						camerax -= 3 * cameraSpeed;
						tv.UpdatePan({ camerax, cameray });
					}
				}
				else
				{
					tv.UpdatePan({ camerax, cameray });
				}
			}
		}
	}

	
	if (stateMachine.getCurrentState().compare("worldMap") == 0)
	{
		if (GetKey(olc::SPACE).bHeld)
		{
			UpdateGameSystems(gameWorldMatrix);
		}


		if (GetKey(olc::TAB).bPressed)
		{
			ForestSystem::get()->ReloadDefinition();
		}


		if (GetKey(olc::R).bPressed)
		{
			render_city_religions = render_city_religions == true ? false : true;
		}
	}
	

	/*
	if (stateMachine.getCurrentState().compare("worldMap") == 0)
	{

		// TODO Rework
		// Check for RMB Press on a city...
		
		if (GetMouse(1).bPressed && stateMachine.getCurrentState().compare("worldMap") == 0)
		{
			for (auto& go : GameObjectStorage::get()->getStorage())
			{
				TransformCmp* transform = go->getComponent<TransformCmp>("Transform");
				CollisionBoxCmp* box = go->getComponent<CollisionBoxCmp>("CollisionBox");


				if (mousex > transform->xpos && mousex < transform->xpos + box->width &&
					mousey > transform->ypos && mousey < transform->ypos + box->height)
				{

					// Is the Gameobject a city.
					if (go->hasComponent("City"))
					{
						currentViewedCity = go;
						stateMachine.setState("cityView");
						break;
					}
					else
					{
						currentViewedCity = nullptr;
					}
				}
			}
		}
		
	}
	else if (stateMachine.getCurrentState().compare("cityView") == 0)
	{
		// Check for escaping from cityview...
		if (GetKey(olc::ESCAPE).bPressed && currentViewedCity)
		{
			stateMachine.setState("worldMap");
			currentViewedCity = nullptr;
		}
	}
	*/
}



void App::_storeDecal(std::string name, size_t id, Pointer<olc::Decal> decal, Pointer<olc::Sprite> sprite)
{
	sprites.push_back(sprite);
	decalIDMap[name] = id;

	if (m_DecalDatabase.size() < id)
	{
		m_DecalDatabase.resize(id);
	}

	m_DecalDatabase[id - 1] = decal;
}


Pointer<olc::Decal> App::_getDecal(std::string name)
{
	size_t id = decalIDMap[name];	
	return m_DecalDatabase[id - 1];
}


bool App::_loadDecalDatabase()
{
	using namespace tinyxml2;
	using namespace std;
	tinyxml2::XMLDocument doc;

	XMLError result = doc.LoadFile("Data/DecalDefinition.xml");
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

	Pointer<olc::Sprite> pSprite;
	Pointer<olc::Decal> pDecal;


	XMLElement* decals = root->FirstChildElement("Decals");
	while (decals)
	{
		pSprite.reset();
		pDecal.reset();

		std::string default_path = decals->Attribute("path");

		XMLElement* decal = decals->FirstChildElement("Decal");
		while (decal)
		{

			std::string path = default_path + decal->GetText();

			std::string decalName = decal->GetText();
			size_t pos = decalName.find(".png");
			if (pos != std::string::npos)
			{
				decalName.erase(pos, decalName.length());
			}


			pSprite = std::make_shared<olc::Sprite>(path);

			if (pSprite->width == 0 && pSprite->height == 0 && pSprite->pColData.size() == 0)
			{
				cout << color(colors::RED);
				cout << "Failed loading Sprite \""<< decalName << "\"{"<< path << "}" << white << endl;
			}


			pDecal = std::make_shared<olc::Decal>(pSprite.get());

			_storeDecal(decalName, ++App::g_DecalInstanceID, pDecal, pSprite);


			decal = decal->NextSiblingElement("Decal");
		}

		decals = decals->NextSiblingElement("Decals");
	}

	return true;
}




bool App::_loadAppStateDefinitions()
{
	stateMachine.storeStateDefinition("worldMap", new AppStateWorldMap(this));
	stateMachine.storeStateDefinition("cityView", new AppStateCityView(this));
	stateMachine.storeStateDefinition("mainMenu", new AppStateMainMenu(this));

	// Set initial state.
	stateMachine.setInitialState("worldMap");

	return true;
}



void AppStateMainMenu::update(float) 
{
	using namespace std;
	cout << color(colors::MAGENTA);
	cout << "[AppStateMainMenu] update" << white << endl;
}


void AppStateMainMenu::onEnter() 
{
	using namespace std;
	cout << color(colors::MAGENTA);
	cout << "[AppStateMainMenu] onEnter" << white << endl;
}

void AppStateMainMenu::onExit() 
{
	using namespace std;
	cout << color(colors::MAGENTA);
	cout << "[AppStateMainMenu] onExit" << white << endl;
}


void AppStateCityView::update(float)
{
	using namespace std;
	/*
	cout << color(colors::MAGENTA);
	cout << "[AppStateCityView] update" << white << endl;
	*/

	app->SetDrawTarget((uint8_t)app->m_GameLayer);


	Pointer<CityComponent> city = app->getCurrentViewedCity()->getComponent<CityComponent>("City");
	if (city)
	{
		_renderCityBase(app->getCurrentViewedCity());
		_renderCityBackground(app->getCurrentViewedCity());
		_renderCityGroundAndWalls(app->getCurrentViewedCity());
		_renderCityBuildings(app->getCurrentViewedCity());
		_renderCityUnits(app->getCurrentViewedCity());
		_renderCityOverlay(app->getCurrentViewedCity());
	}
}


// Render the basic layout for the city.
void AppStateCityView::_renderCityBase(Pointer<GameObject2> city)
{
	// Red Circle Filling Whole Screen Almost -> Available Space.
	app->FillRectDecal(olc::vi2d(5, 5), olc::vi2d(1360, 750), olc::DARK_RED);

	// City Screen.
	app->FillRectDecal(olc::vi2d(5, 5), olc::vi2d(700, 750), olc::DARK_CYAN);


	// City Worldmap Screen, showing the city and surrounding maptiles around city.
	app->FillRectDecal(olc::vi2d(705, 5), olc::vi2d(650, 750), olc::DARK_MAGENTA);

	// City UI Screen
	app->FillRectDecal(olc::vi2d(5, 5), olc::vi2d(700, 200), olc::GREEN);

	// Maptile Indication, with the maptile and forest, hills or river if any.
	app->FillRectDecal(olc::vi2d(5, 200), olc::vi2d(700, 200), olc::YELLOW);
	app->DrawDecal(olc::vf2d(5, 200), app->_getDecal("bg_maptile_temperate").get(), olc::vf2d(1, 1));
	app->DrawDecal(olc::vf2d(5, 200), app->_getDecal("temperate_forest_bg").get(), olc::vf2d(1, 1));


	// City Ground Indication.
	//app->FillRectDecal(olc::vi2d(5, 200), olc::vi2d(700, 555), olc::GREY);
	app->DrawDecal(olc::vf2d(5, 200), app->_getDecal("city_bg_temperate_plain").get(), olc::vf2d(1, 1));


	// Selected Unit/Building etc.
	app->FillRectDecal(olc::vi2d(5, 740), olc::vi2d(700, 15), olc::RED);
}


// Draw the Background like Maptile, Forest or Hill,
// River etc.
void AppStateCityView::_renderCityBackground(Pointer<GameObject2> city)
{

}




// Render the Cityground for Buildings and Walls.
void AppStateCityView::_renderCityGroundAndWalls(Pointer<GameObject2> city)
{

}




// Render the Buildings currently Build in the city.
void AppStateCityView::_renderCityBuildings(Pointer<GameObject2> city)
{
	/*
	for (auto& go : city->getBuildings())
	{
		if (go->hasComponent("Renderable"))
		{
			RendererableCmp* render = go->getComponent<RendererableCmp>("Renderable");
			if (render->render && render->renderingLayer.compare("building") == 0)
			{
				TransformCmp* transform = go->getComponent<TransformCmp>("Transform");

				olc::Decal* decal = app->_getDecal(render->decalName);

				app->DrawDecal(olc::vf2d(transform->xpos, transform->ypos), decal, olc::vf2d(0.25f, 0.25f));
			}
		}
	}
	*/
}




// Render Units in the city, working ones, citizens and garrisoned.
void AppStateCityView::_renderCityUnits(Pointer<GameObject2> city)
{
	/*
	for (auto& go : city->getUnits())
	{
		if (go->hasComponent("Renderable"))
		{
			RendererableCmp* render = go->getComponent<RendererableCmp>("Renderable");
			if (render->render && render->renderingLayer.compare("unit") == 0)
			{
				TransformCmp* transform = go->getComponent<TransformCmp>("Transform");

				olc::Decal* decal = app->_getDecal(render->decalName);

				app->DrawDecal(olc::vf2d(transform->xpos, transform->ypos), decal, olc::vf2d(0.25f, 0.25f));
			}
		}
	}
	*/
}




// Render needed UI elements for the city etc.
void AppStateCityView::_renderCityOverlay(Pointer<GameObject2> city)
{

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("World Map"))
		{
			app->stateMachine.setState("worldMap");


			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
}





void AppStateCityView::onEnter()
{
	using namespace std;
	cout << color(colors::MAGENTA);
	cout << "[AppStateCityView] onEnter" << white << endl;
}

void AppStateCityView::onExit()
{
	using namespace std;
	cout << color(colors::MAGENTA);
	cout << "[AppStateCityView] onExit" << white << endl;
}



void AppStateWorldMap::_updateGameworldMatrix(GameworldMatrix& world)
{
	for (int i = 0; i < world.size(); i++)
	{
		for (int j = 0; j < world[i].size(); j++)
		{

			if (world[i][j])
			{
				// Search for Deleted gameobjects and remove them.
				// A Deleted Gameobjects Tag is Empty.
				if (world[i][j]->getTag().compare("") == 0)
				{
					world[i][j].reset();
				}
			}
		}
	}
}


void AppStateWorldMap::_renderGameworld()
{
	using namespace std;

	_updateGameworldMatrix(app->gameWorldMatrix);

	// Iterate through visible maptiles and render them.
	olc::vi2d upLeft = app->visiblePointLeftUp;
	olc::vi2d downRight = app->visiblePointDownRight;

	for (int x = upLeft.x; x < downRight.x; x++)
	{
		for (int y = upLeft.y; y < downRight.y; y++)
		{
			if (app->gameWorldMatrix[x][y])
			{
				_renderMaptile(app->gameWorldMatrix[x][y]);
			}
			else
			{
				cout << color(colors::MAGENTA);
				cout << "Maptile {" << x << "," << y << "} is Destroyed" << white << endl;
			}
		}
	}
}


void AppStateWorldMap::_renderMaptile(Pointer<GameObject2> tile)
{
	// Rendering Order:
	// 1 - maptile itself.
	// 2 - hill, mountain
	// 3 - river
	// 4 - forest
	// 5 - ressource
	// 6 - improvement
	// 7 - road
	// 8 - city or fort
	// 9 - unit

	using namespace std;


	std::map< int, Pointer<GameObject2> > drawOrder;
	for (int i = 1; i < 10; i++) drawOrder.emplace(i, nullptr);

	Pointer<MaptileComponent> maptile = tile->getComponent<MaptileComponent>("Maptile");

	// Render Maptile itself as first.
	drawOrder[1] = tile;


	for (auto go : maptile->GetGameobjects())
	{
		// Store gameobject in correct slot for ordered rendering.
		if (go->hasComponent("Renderable"))
		{

			// Do not render units in city as outside.
			if (go->hasComponent("Unit") && go->getComponent<UnitComponent>("Unit")->IsInCity() == true) continue;


			Pointer<RenderableComponent> render = go->getComponent<RenderableComponent>("Renderable");

			if (render->IsRendering())
			{
				if (render->GetRenderingLayer().compare("forest") == 0) // And Mountain, Hill.
				{
					drawOrder[4] = go;
				}
				else if (render->GetRenderingLayer().compare("mountain") == 0)
				{
					drawOrder[2] = go;
				}
				else if (render->GetRenderingLayer().compare("river") == 0)
				{
					drawOrder[3] = go;
				}
				else if (render->GetRenderingLayer().compare("ressource") == 0)
				{
					drawOrder[5] = go;
				}
				else if (render->GetRenderingLayer().compare("improvement") == 0)
				{
					drawOrder[6] = go;
				}
				else if (render->GetRenderingLayer().compare("road") == 0)
				{
					drawOrder[7] = go;
				}
				else if (render->GetRenderingLayer().compare("city") == 0)
				{
					drawOrder[8] = go;
				}
				else if (render->GetRenderingLayer().compare("unit") == 0)
				{
					drawOrder[9] = go;
				}
			}
		}
	}


	for (int i = 1; i < 9; i++)
	{
		// Render the Gameobjects in correct order.
		Pointer<GameObject2> go = drawOrder[i];
		if (go)
		{
			Pointer<TransformComponent> transform = go->getComponent<TransformComponent>("Transform");
			Pointer<RenderableComponent>  render = go->getComponent<RenderableComponent>("Renderable");

			Pointer<olc::Decal> decal = app->_getDecal(render->GetDecalName());

			app->tv.DrawDecal(olc::vf2d(transform->GetXPos(), transform->GetYPos()), decal.get());
		}
	}


	// Render some text about a Forest.
	// Will crash if we select a Mountain or Hill...
	if (drawOrder[4] && drawOrder[4]->hasComponent("Forest"))
	{
		auto forest = drawOrder[4]->getComponent<ForestComponent>("Forest");
		auto transform = drawOrder[4]->getComponent<TransformComponent>("Transform");
		auto render = drawOrder[4]->getComponent<RenderableComponent>("Renderable");

		std::string type, biome;
		int life, maxLife;

		type = forest->GetForestType();
		biome = forest->GetForestBiome();
		life = forest->GetLifetime();
		maxLife = forest->GetMaxLifetime();

		std::string text = "\"" + type + "\",\n\"" + biome + "\",\n\"" + std::to_string(life) + "\"/\"" + std::to_string(maxLife) + "\"";

		olc::vf2d p = { transform->GetXPos() + render->GetWidth() / 2.0f - 0.3f, transform->GetYPos() + render->GetHeight() / 2.0f };
		olc::vf2d screenPoint = app->tv.WorldToScreen(p);
		app->font->DrawStringDecal(screenPoint, text, olc::RED, { 0.4f, 0.4f });
	}



	// Render Religion related text above city.
	if (drawOrder[8] && drawOrder[8]->hasComponent("City"))
	{
		auto c = drawOrder[8]->getComponent<CityComponent>("City");
		auto transform = drawOrder[8]->getComponent<TransformComponent>("Transform");
		auto render = drawOrder[8]->getComponent<RenderableComponent>("Renderable");

		std::string text = c->GetName();

		olc::vf2d p = { transform->GetXPos() + render->GetWidth() / 2.0f - 0.3f, transform->GetYPos() + render->GetHeight() / 2.0f };
		olc::vf2d screenPoint = app->tv.WorldToScreen(p);
		app->font->DrawStringDecal(screenPoint, text, olc::RED, { 0.5f, 0.5f });
	
	}
}


void App::_updateVisibleRect()
{
	visiblePointLeftUp = tv.GetTileUnderScreenPos({ 0, 0 });
	if (visiblePointLeftUp.x < 0) visiblePointLeftUp.x = 0;
	if (visiblePointLeftUp.y < 0) visiblePointLeftUp.y = 0;
	visiblePointDownRight = tv.GetBottomRightTile();
	if (visiblePointDownRight.x > DEFAULT_MAPSIZE_X) visiblePointDownRight.x = DEFAULT_MAPSIZE_X;
	if (visiblePointDownRight.y > DEFAULT_MAPSIZE_Y) visiblePointDownRight.y = DEFAULT_MAPSIZE_Y;
}


void AppStateWorldMap::update(float)
{
	using namespace std;
	/*
	cout << color(colors::MAGENTA);
	cout << "[AppStateWorldMap] update" << white << endl;
	*/

	// Do the standard update.
	olc::TileTransformedView tv = app->getRenderer();


	if (render_2d_grid)
	{

		// Application rendering.
		olc::vi2d topleft = tv.GetTopLeftTile().max({ 0, 0 });
		olc::vi2d bottomright = tv.GetBottomRightTile().min({ DEFAULT_MAPSIZE_X, DEFAULT_MAPSIZE_Y });
		olc::vi2d tile;


		// Draw Grid.
		for (tile.y = topleft.y; tile.y < bottomright.y; tile.y++)
		{
			for (tile.x = topleft.x; tile.x < bottomright.x; tile.x++)
			{
				tv.DrawLine(tile, tile + olc::vf2d(0.0f, 1.0f), olc::VERY_DARK_GREY);
				tv.DrawLine(tile, tile + olc::vf2d(1.0f, 0.0f), olc::VERY_DARK_GREY);
			}
		}


	}


	// Draw General Layered
	_renderGameworld();




	/*
	// Dra UI Stuff for debugging.
	if (selected_ui_gameobject)
	{
		TransformCmp* tr = static_cast<TransformCmp*> (selected_ui_gameobject->getComponent("Transform"));
		RendererableCmp* rc = static_cast<RendererableCmp*> (selected_ui_gameobject->getComponent("Renderable"));

		if (tr != nullptr && rc != nullptr)
		{
			olc::vf2d p = { tr->xpos + rc->width / 2.0f - 0.3f, tr->ypos + rc->height / 2.0f };
			tv.DrawStringDecal(p, selected_ui_gameobject->tag, olc::RED, olc::vf2d(0.5f, 0.5f));

			olc::vf2d screenPoint = tv.WorldToScreen(p);
			app->font->DrawStringDecal(screenPoint, selected_ui_gameobject->tag, olc::RED, {0.75f, 0.75f});
		}
	}


	if (show_collisions)
	{
		for (auto& coll : ComponentStorage::get()->getAllOfType<CollisionBoxCmp>("CollisionBox")) // Update collision detection.
		{
			for (auto& go : GameObjectStorage::get()->getStorage())
			{
				// Do not draw collision with self
				if (coll->this_agent->getTag().compare(go->getTag()) == 0) continue;

				// Resolve collision with another object
				if (coll->resolve(go))
				{
					TransformCmp* tr = static_cast<TransformCmp*>(coll->this_agent->getComponent("Transform"));
					CollisionBoxCmp* c = static_cast<CollisionBoxCmp*>(coll->this_agent->getComponent("CollisionBox"));

					tv.DrawRect(olc::vf2d(tr->xpos - 0.1f, tr->ypos - 0.1f), olc::vf2d(c->width + 0.1f, c->height + 0.1f), olc::DARK_RED);
				}
			}
		}
	}
	*/

	if (show_nav_mesh)
	{
		// Draw Nodes
		std::vector<std::vector<float>> nodes = NavMesh::get()->GetNavGraph();
		Pointer<Graph> graph = NavMesh::get()->GetGraph();
		for (int i = 0; i < nodes.size(); i++)
		{
			for (int j = 0; j < nodes[i].size(); j++)
			{
				if (nodes[i][j] == 1)
				{
					tv.DrawCircle(olc::vf2d(i + 0.5f, j + 0.5f), 0.1f, olc::WHITE);
				}
			}
		}

		// Draw Edges
		for (auto& e : graph->edges)
		{
			float a, b, x, y;

			a = edge_from_x(e);
			b = edge_from_y(e);
			x = edge_to_x(e);
			y = edge_to_y(e);

			tv.DrawLine(olc::vf2d(a + 0.5f, b + 0.5f), olc::vf2d(x + 0.5f, y + 0.5f), olc::YELLOW);
		}
	}

	/*
	if (show_nav_path)
	{
		// Draw waypoints of selected entity,
		// if it has one.
		if (selected_ui_gameobject)
		{
			if (selected_ui_gameobject->hasComponent("Navigator"))
			{
				NavigatorCmp* nav = selected_ui_gameobject->getComponent<NavigatorCmp>("Navigator");


				for (int i = 0; i < nav->movementPoints.size(); i++)
				{
					if (i == 0)
					{
						// Draw source with special color.
						tv.DrawCircle(olc::vf2d(node_x(nav->movementPoints[i]) + 0.5f, node_y(nav->movementPoints[i]) + 0.5f), 0.1f, olc::RED);
						continue;
					}

					if (i + 1 == nav->movementPoints.size())
					{
						// Draw destination with special color.
						tv.DrawCircle(olc::vf2d(node_x(nav->movementPoints[i]) + 0.5f, node_y(nav->movementPoints[i]) + 0.5f), 0.1f, olc::GREEN);
						continue;
					}

					tv.DrawCircle(olc::vf2d(node_x(nav->movementPoints[i]) + 0.5f, node_y(nav->movementPoints[i]) + 0.5f), 0.1f, olc::MAGENTA);
				}
			}
		}
	}
	*/

	// Draw ImGui.
	_drawUI();


	/*
	// Have we selected some entity on the Screen.
	static bool isSelected = false;
	if (app->stateMachine.getCurrentState().compare("worldMap") == 0 && app->GetMouse(0).bPressed)
	{
		isSelected = false;

		olc::vf2d p = tv.ScreenToWorld(app->GetMousePos());

		for (auto& go : GameObjectStorage::get()->getStorage())
		{
			if (go->hasComponent("Transform") && go->hasComponent("Renderable"))
			{
				TransformCmp* goTr = nullptr;
				RendererableCmp* goRen = nullptr;

				if (selected_gameobject)
				{
					if (selected_gameobject->getTag().compare(go->getTag()) == 0) continue;
				}


				goTr = go->getComponent<TransformCmp>("Transform");
				goRen = go->getComponent<RendererableCmp>("Renderable");

				if (p.x >= goTr->xpos && p.x <= goTr->xpos + goRen->width &&
					p.y >= goTr->ypos && p.y <= goTr->ypos + goRen->height)
				{
					// Collision.
					selected_gameobject = go;
					isSelected = true;
					break;
				}
			}
		}
	}
	*/

	

	if (selected_gameobject)
	{
		Pointer<TransformComponent> tr = selected_gameobject->getComponent<TransformComponent>("Transform");
		Pointer<RenderableComponent> rc = selected_gameobject->getComponent<RenderableComponent>("Renderable");

		if (tr != nullptr && rc != nullptr)
		{
			olc::vf2d p = { tr->GetXPos() + rc->GetWidth() / 2.0f - 0.3f, tr->GetYPos() + rc->GetHeight() / 2.0f };
			
			olc::vf2d screenPoint = tv.WorldToScreen(p);
			app->font->DrawStringDecal(screenPoint, selected_gameobject->getName(), olc::MAGENTA, { 0.75f, 0.75f });
		}



		if (selected_gameobject->hasComponent("City"))
		{

			auto c = selected_gameobject->getComponent<CityComponent>("City");
			auto transform = selected_gameobject->getComponent<TransformComponent>("Transform");
			auto render = selected_gameobject->getComponent<RenderableComponent>("Renderable");

			std::map< ReligionID, float > religions = c->GetReligions();

			std::string text;
			for (auto r : religions)
			{
				text += r.first + " : " + std::to_string(r.second) +" \n";
			}

			olc::vf2d p = { transform->GetXPos() + render->GetWidth(), transform->GetYPos() + render->GetHeight()};
			olc::vf2d screenPoint = app->tv.WorldToScreen(p);
			app->font->DrawStringDecal(screenPoint, text, olc::RED, { 0.5f, 0.5f });
		}

	}
	
}

void AppStateWorldMap::_drawUI()
{

	if (render_city_religions)
	{
		std::vector< Pointer<GameObject2> > cities;

		for (int i = 0; i < app->gameWorldMatrix.size(); i++)
		{
			for (int j = 0; j < app->gameWorldMatrix[i].size(); j++)
			{
				Pointer<MaptileComponent> m = app->gameWorldMatrix[i][j]->getComponent<MaptileComponent>("Maptile");
				if (m->HasCity())
				{
					Pointer<GameObject2> c;
					for (auto ptr : m->GetGameobjects())
					{
						if (ptr->hasComponent("City")) c = ptr; break;
					}

					auto city = c->getComponent<CityComponent>("City");
					auto cityTransform = c->getComponent<TransformComponent>("Transform");

					olc::Pixel color;
					color.r = ReligionSystem::get()->GetReligionColor(city->GetMajorReligion()).r;
					color.g = ReligionSystem::get()->GetReligionColor(city->GetMajorReligion()).g;
					color.b = ReligionSystem::get()->GetReligionColor(city->GetMajorReligion()).b;
					color.a = 100;

					float strength = city->GetReligions()[city->GetMajorReligion()];
					float div = 4.0f;

					olc::vf2d p = { cityTransform->GetXPos() - (strength / div) / 2.0f, cityTransform->GetYPos() - (strength / div) / 2.0f };

					app->tv.DrawDecal(p, app->_getDecal("circle").get(), { strength / div, strength / div }, color);

					// Store for later drawing of distances.
					cities.push_back(c);
				}
			}
		}



		// Go through all cities on maptile,
		// and render their distance from each other.
		for (int i = 0; i < cities.size(); i++)
		{
			auto city = cities[i]->getComponent<CityComponent>("City");
			auto cityTransform = cities[i]->getComponent<TransformComponent>("Transform");


			for (int j = 0; j < cities.size(); j++)
			{
				// Dont draw a line to self.
				if (i == j) continue;

				auto otherCityTransform = cities[j]->getComponent<TransformComponent>("Transform");

				olc::vf2d from = { cityTransform->GetXPos(), cityTransform->GetYPos() };
				olc::vf2d to = { otherCityTransform->GetXPos(), otherCityTransform->GetYPos() };


				// Draw the distance above the line.
				float d = std::abs(from.x - to.x) + std::abs(from.y - to.y); // Manhatten

				// Get color indicating whether in range.
				olc::Pixel color = (d > 30.0) ? olc::DARK_RED : olc::GREEN;

				// Draw the line.
				app->DrawLineDecalTransformed(from, to, color);

				// Compute middle point of the line.
				olc::vf2d m = {std::abs(to.x - from.x) / 2.0f, std::abs(to.y - from.y) / 2.0f};

				// Get the screen space coordinates for the middle point.
				olc::vf2d p = app->tv.WorldToScreen(m);

				// Draw the Distance at given cooordinates.
				// TODO: Not drawing correctly.
				app->font->DrawStringDecal(m, std::to_string(d), olc::BLACK, {0.4f, 0.4f});
			}
		}


	}

}


void AppStateWorldMap::onEnter()
{
	using namespace std;
	cout << color(colors::MAGENTA);
	cout << "[AppStateWorldMap] onEnter" << white << endl;
}

void AppStateWorldMap::onExit()
{
	using namespace std;
	cout << color(colors::MAGENTA);
	cout << "[AppStateWorldMap] onExit" << white << endl;
}

void UpdateGameSystems(GameworldMatrix& world)
{
	UpdateForestSystem(world);
	UpdateReligionSystem(world);
}

void UpdateForestSystem(GameworldMatrix& world)
{
	ForestSystem::get()->Update(world);
}

void UpdateReligionSystem(GameworldMatrix& world)
{
	ReligionSystem::get()->UpdateReligions(world);
}



void App::DrawCircleDecal(const olc::vf2d& pos, const olc::vf2d& scale, const olc::vf2d& center, const olc::Pixel& tint)
{
	olc::vf2d p = {pos.x - center.x, pos.y - center.y};
	DrawDecal(p, _getDecal("circle").get(), scale, tint);
}

void App::DrawElipseDecal(const olc::vf2d& pos, const olc::vf2d& scale, const olc::vf2d& center, const olc::Pixel& tint)
{
	olc::vf2d p = { pos.x - center.x, pos.y - center.y };
	DrawDecal(p, _getDecal("elipse").get(), scale, tint);
}

void App::DrawRectangleDecal(const olc::vf2d& pos, const olc::vf2d& scale, const olc::vf2d& center, const olc::Pixel& tint)
{
	olc::vf2d p = { pos.x - center.x, pos.y - center.y };
	DrawDecal(p, _getDecal("rectangle").get(), scale, tint);
}


void App::DrawTriangleDecal(const olc::vf2d& pos, const olc::vf2d& scale, const olc::vf2d& center, const olc::Pixel& tint)
{
	olc::vf2d p = { pos.x - center.x, pos.y - center.y };
	DrawDecal(p, _getDecal("triangle").get(), scale, tint);
}


void App::DrawCircleDecalTransformed(const olc::vf2d& pos, const olc::vf2d& scale, const olc::Pixel& tint)
{
	tv.DrawDecal(pos, _getDecal("circle").get(), scale, tint);
}


void App::DrawElipseDecalTransformed(const olc::vf2d& pos, const olc::vf2d& scale, const olc::Pixel& tint)
{
	tv.DrawDecal(pos, _getDecal("elipse").get(), scale, tint);
}


void App::DrawRectangleDecalTransformed(const olc::vf2d& pos, const olc::vf2d& scale, const olc::Pixel& tint)
{
	tv.DrawDecal(pos, _getDecal("rectangle").get(), scale, tint);
}


void App::DrawTriangleDecalTransformed(const olc::vf2d& pos, const olc::vf2d& scale, const olc::Pixel& tint)
{
	tv.DrawDecal(pos, _getDecal("triangle").get(), scale, tint);
}


void App::DrawLineDecalTransformed(const olc::vf2d& from, const olc::vf2d& to, const olc::Pixel& tint)
{
	olc::vf2d p1 = tv.WorldToScreen(from);
	olc::vf2d p2 = tv.WorldToScreen(to);
	DrawLineDecal(p1, p2, tint);
}

