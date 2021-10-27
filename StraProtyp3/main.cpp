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

int main()
{
	App demo;
	if (demo.Construct(1360, 768, 1, 1))
		demo.Start();


	demo.shutDown();


	NavMesh::del();
	GameWorldTime::del();


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
		

	GameWorldTime::get()->update(); // Update Game World Time.


	// Update all Maptile Components.
	for (auto ptr : GameobjectStorage::get()->GetStorage())
	{
		if (ptr->hasComponent("Maptile"))
		{
			Pointer<MaptileComponent> m = ptr->getComponent<MaptileComponent>("Maptile");
			m->update();
		}
	}



	stateMachine.update(fElapsedTime);



	SetDrawTarget((uint8_t)m_GameLayer);
	DrawStringDecal(olc::vf2d(1200, 50), "FPS: " + std::to_string(GetFPS()));
	font->DrawStringDecal(olc::vf2d(1200, 50), "FPS: " + std::to_string(GetFPS()), olc::WHITE, {0.5f, 0.5f});

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
	if (!_loadTechTreeDefinitions()) return false;
	if (!_loadAppStateDefinitions()) return false;
	if (!_loadGameobjectPathdefinitions()) return false;
	if (!_initGameworldMatrix()) return false;




	m_GameLayer = CreateLayer();
	EnableLayer(m_GameLayer, true);
	SetLayerCustomRenderFunction(0, std::bind(&App::DrawUI, this));


	// Create Transformed View renderer with given Screen Dimension and given Tile Size (256, 256).
	tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { DEFAULT_DECAL_SIZE_X, DEFAULT_DECAL_SIZE_Y });

	
	GameWorldTime::get()->setTimeSpeed(0.016);



	for (int i = 0; i < gameWorldMatrix.size(); i++)
	{
		for (int j = 0; j < gameWorldMatrix[i].size(); j++)
		{
			gameWorldMatrix[i][j] = GameobjectStorage::get()->Instantiate("Jungle_Maptile", i, j);
		
			auto forest = GameobjectStorage::get()->Instantiate("Jungle_Normal", i, j);
			auto spearman = GameobjectStorage::get()->Instantiate("Spearman", i, j);
		}
	}

	for (int i = 0; i < gameWorldMatrix.size(); i++)
	{
		for (int j = 0; j < gameWorldMatrix[i].size(); j++)
		{
			auto ptr = gameWorldMatrix[i][j];

			// Remove all entities from maptile.
			Pointer<MaptileComponent> m = ptr->getComponent<MaptileComponent>("Maptile");
			while(m->GetGameobjects().size() > 0)
			{
				auto e = m->GetGameobjects()[0];
				m->RemoveGameobject(e);
			}
		}
	}

	




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


	olc::vf2d point = tv.ScreenToWorld({ GetMouseX(), GetMouseY() });
	float mousex = point.x;
	float mousey = point.y;


	// Handle Movement and Paning Input only for WorldMap,
	// where we use the Tiling Transformation.
	if (stateMachine.getCurrentState().compare("worldMap") == 0)
	{
		olc::vi2d topLeft = tv.GetTileUnderScreenPos({ 0, 0 });
		olc::vi2d bottomDown = tv.GetBottomRightTile();
		olc::vi2d middle = { bottomDown.x / 2, bottomDown.y / 2 };

		/*
		cout << color(colors::RED);
		cout << "MousePos {" << mousex << "," << mousey << "}" << endl;
		cout << "TopLeft Maptile {" << topLeft.x << "," << topLeft.y << "}" << endl;
		cout << "BottomRight Maptile {" << bottomDown.x << "," << bottomDown.y << "}" << endl;
		cout << "Middle Maptile {" << middle.x << "," << middle.y << "}" << white << endl;
		*/

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


bool App::_loadTechTreeDefinitions()
{
	return true;
}

/*
bool App::_loadTechTreeDefinitions()
{
	std::string default_path = "Data/Tech/";

	TechInstance* tech = new TechInstance(default_path + "Hunting.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "AdvancedArmorForging.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "AdvancedWeaponForging.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "ArmorForging.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "WeaponForging.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "Bowmaking.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "ExpertBowmaking.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "Honor.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "Tactics.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "WarfareTheory.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "WarriorCode.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "AdvancedClothing.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "Alchemy.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "AnimalHusbandry.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "AnimalTaiming.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "ClayWorking.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "CropRotation.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "Irrigation.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "LeatherWorking.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "MagicalPropertiesOfStones.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "MagicalStaffs.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "ManaStoneBounding.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "Masonry.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "IronWorking.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "BronzeWorking.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "Mysticism.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "PlantCultivation.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "PlantKnowledge.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "PotteryMaking.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "StoneWorking.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "ToolMaking.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "WoodWorking.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "AdamantiumSmelting.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "MalachiteSmelting.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "AlcoholMaking.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "SkumaDistillation.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "HorseRiding.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "SteelMaking.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "BrickBurning.xml");
	techTree.push_back(tech);

	tech = new TechInstance(default_path + "Barber.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "EconomyTheory.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "EconomyTheoryII.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "EconomyTheoryIII.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "ElderCircle.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "Philosophy.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "PoliticalTheory.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "PoliticalTheoryII.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "Scissors.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "Writing.xml");
	techTree.push_back(tech);
	tech = new TechInstance(default_path + "Trade.xml");
	techTree.push_back(tech);


	return true;
}
*/

/*
void App::_storeDecal(const std::string& name, olc::Decal* decal)
{
	int hash = hasher(name);
	decalDatabase.insert(hash, decal);
	decalNamesInDatabase.push_back(name);
}


olc::Decal* App::_getDecal(const std::string& name)
{
	int hash = hasher(name);
	return decalDatabase.findStoredData(hash);
}
*/

void App::_storeDecal(const std::string& name, size_t id, Pointer<olc::Decal> decal, Pointer<olc::Sprite> sprite)
{
	sprites.push_back(sprite);
	decalIDMap[name] = id;
	decalDatabase.insert(id, decal);
}


Pointer<olc::Decal> App::_getDecal(const std::string& name)
{
	size_t id = decalIDMap[name];
	return decalDatabase.findStoredData(id);
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




bool App::_loadGameobjectPathdefinitions()
{
	/*
	GameObjectCreator::createGameobjectPathDefinition("Spearman", "Data/Spearman.xml");
	GameObjectCreator::createGameobjectPathDefinition("Tavern", "Data/Tavern.xml");
	GameObjectCreator::createGameobjectPathDefinition("Weapon Smiths Workshop", "Data/Weaponsmith.xml");
	*/
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
	// 2 - hill, mountain or forest
	// 3 - river
	// 4 - ressource
	// 5 - improvement
	// 6 - road
	// 7 - city or fort
	// 8 - unit

	using namespace std;


	std::map< int, Pointer<GameObject2> > drawOrder;
	for (int i = 1; i < 9; i++) drawOrder.emplace(i, nullptr);

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
				if (render->GetRenderingLayer().compare("forest") == 0) // And Mountain, Hill
				{
					drawOrder[2] = go;
				}
				else if (render->GetRenderingLayer().compare("river") == 0)
				{
					drawOrder[3] = go;
				}
				else if (render->GetRenderingLayer().compare("ressource") == 0)
				{
					drawOrder[4] = go;
				}
				else if (render->GetRenderingLayer().compare("improvement") == 0)
				{
					drawOrder[5] = go;
				}
				else if (render->GetRenderingLayer().compare("road") == 0)
				{
					drawOrder[6] = go;
				}
				else if (render->GetRenderingLayer().compare("city") == 0)
				{
					drawOrder[7] = go;
				}
				else if (render->GetRenderingLayer().compare("unit") == 0)
				{
					drawOrder[8] = go;
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


	/*
	std::map< int, GameObject* > drawOrder;
	for (int i = 1; i < 9; i++) drawOrder.emplace(i, nullptr);

	IMaptileCmp* maptile = tile->getComponent<IMaptileCmp>("Maptile");

	// Render Maptile itself as first.
	drawOrder[1] = tile;


	for (auto& tag : maptile->getGameobjects())
	{
		// Get the gameobject on maptile.
		GameObject* go = GameObjectStorage::get()->getGOByTag(tag);


		// Store gameobject in correct slot for ordered rendering.
		if (go->hasComponent("Renderable"))
		{

			// Do not render units in city as outside.
			if (go->hasComponent("Unit") && go->getComponent<IUnitCmp>("Unit")->isInCity() == true) continue;


			RendererableCmp* render = go->getComponent<RendererableCmp>("Renderable");


			if (render->render)
			{
				if (render->renderingLayer.compare("forest") == 0) // And Mountain, Hill
				{
					drawOrder[2] = go;
				}
				else if (render->renderingLayer.compare("river") == 0)
				{
					drawOrder[3] = go;
				}
				else if (render->renderingLayer.compare("ressource") == 0)
				{
					drawOrder[4] = go;
				}
				else if (render->renderingLayer.compare("improvement") == 0)
				{
					drawOrder[5] = go;
				}
				else if (render->renderingLayer.compare("road") == 0)
				{
					drawOrder[6] = go;
				}
				else if (render->renderingLayer.compare("city") == 0)
				{
					drawOrder[7] = go;
				}
				else if (render->renderingLayer.compare("unit") == 0)
				{
					drawOrder[8] = go;
				}
			}
		}
	}


	for (int i = 1; i < 9; i++)
	{
		// Render the Gameobjects in correct order.
		GameObject* go = drawOrder[i];
		if (go)
		{
			TransformCmp* transform = go->getComponent<TransformCmp>("Transform");
			RendererableCmp* render = go->getComponent<RendererableCmp>("Renderable");

			olc::Decal* decal = app->_getDecal(render->decalName);

			app->tv.DrawDecal(olc::vf2d(transform->xpos, transform->ypos), decal);
		}
	}
	*/
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

	/*
	if (!isSelected) selected_gameobject = nullptr;

	if (selected_gameobject)
	{
		TransformCmp* tr = static_cast<TransformCmp*> (selected_gameobject->getComponent("Transform"));
		RendererableCmp* rc = static_cast<RendererableCmp*> (selected_gameobject->getComponent("Renderable"));

		if (tr != nullptr && rc != nullptr)
		{
			olc::vf2d p = { tr->xpos + rc->width / 2.0f - 0.3f, tr->ypos + rc->height / 2.0f };
			
			olc::vf2d screenPoint = tv.WorldToScreen(p);
			app->font->DrawStringDecal(screenPoint, selected_gameobject->name, olc::RED, { 0.75f, 0.75f });
		}
	}
	*/
}

void AppStateWorldMap::_drawUI()
{

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