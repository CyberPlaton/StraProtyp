#include "main.h"


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
static GameObject* selected_gameobject = nullptr;
static int imnodes_tech_node_id = 1;
static int imnodes_tech_link_id = 1;
static int imnodes_tech_dependency_id = 1;
static int imnodes_tech_dependency_display_id = 100000;
static bool imnodes_tech_tree_initialized = false;




int main()
{
	App demo;
	if (demo.Construct(1024, 768, 1, 1))
		demo.Start();

	demo.shutDown();

	NavMesh::del();
	GameObjectStorage::del();
	ComponentStorage::del();
	GameWorldTime::del();


	ImNodes::SaveCurrentEditorStateToIniFile("tech_tree_graph.ini");
	ImGui::DestroyContext();
	ImNodes::DestroyContext();


	_CrtCheckMemory();
	_CrtDumpMemoryLeaks();

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

	_handleInput(); // Handle the users input.
		

	GameWorldTime::get()->update(); // Update Game World Time.


	stateMachine.update(fElapsedTime);


	return true;
}






bool App::OnUserCreate()
{
	using namespace std;

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


	m_GameLayer = CreateLayer();
	EnableLayer(m_GameLayer, true);
	SetLayerCustomRenderFunction(0, std::bind(&App::DrawUI, this));


	tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { DEFAULT_DECAL_SIZE_X, DEFAULT_DECAL_SIZE_Y });

	
	GameWorldTime::get()->setTimeSpeed(0.016);


	GameObjectCreator creator;

	// Show off new Forestry with Jungle.
	GameObject* go = creator.create("Data/Jungle_Maptile.xml", "Maptile", 0, 0);
	go = creator.create("Data/Jungle_Maptile.xml", "Maptile", 1, 0);
	go = creator.create("Data/Jungle_Maptile.xml", "Maptile", 0, 1);
	go = creator.create("Data/Jungle_Maptile.xml", "Maptile", 1, 1);
	go = creator.create("Data/Jungle_Maptile.xml", "Maptile", 2, 0);
	go = creator.create("Data/Jungle_Maptile.xml", "Maptile", 0, 2);



	go = creator.create("Data/Jungle_Deep.xml", "Forest", 0, 0);
	go = creator.create("Data/Jungle_Deep.xml", "Forest", 1, 0);
	go = creator.create("Data/Jungle_Normal.xml", "Forest", 0, 1);
	go = creator.create("Data/Jungle_Scarce.xml", "Forest", 1, 1);
	go = creator.create("Data/Jungle_Dying.xml", "Forest", 2, 0);
	go = creator.create("Data/Jungle_Dying.xml", "Forest", 0, 2);

	go = creator.create("Data/Spearman.xml", "Spearman_Unit", 0, 0);


	// Show off old Forestry with Savannah
	go = creator.create("Data/Savannah_Maptile.xml", "Maptile", 2, 2);
	go = creator.create("Data/Savannah_Maptile.xml", "Maptile", 3, 2);
	go = creator.create("Data/Savannah_Maptile.xml", "Maptile", 2, 3);
	go = creator.create("Data/Savannah_Maptile.xml", "Maptile", 3, 3);
	go = creator.create("Data/Savannah_Maptile.xml", "Maptile", 4, 2);
	go = creator.create("Data/Savannah_Maptile.xml", "Maptile", 2, 4);

	go = creator.create("Data/Savannah_Deep.xml", "Forest", 2, 2);
	go = creator.create("Data/Savannah_Deep.xml", "Forest", 3, 2);
	go = creator.create("Data/Savannah_Normal.xml", "Forest", 2, 3);
	go = creator.create("Data/Savannah_Scarce.xml", "Forest", 3, 3);
	go = creator.create("Data/Savannah_Dying.xml", "Forest", 4, 2);
	go = creator.create("Data/Savannah_Dying.xml", "Forest", 2, 4);

	go = creator.create("Data/Spearman.xml", "Spearman_Unit", 2, 2);


	// Show off old forestry with temperate
	go = creator.create("Data/Temperate_Maptile.xml", "Maptile", 4, 4);
	go = creator.create("Data/Temperate_Maptile.xml", "Maptile", 5, 4);
	go = creator.create("Data/Temperate_Maptile.xml", "Maptile", 4, 5);
	go = creator.create("Data/Temperate_Maptile.xml", "Maptile", 5, 5);
	go = creator.create("Data/Temperate_Maptile.xml", "Maptile", 6, 4);
	go = creator.create("Data/Temperate_Maptile.xml", "Maptile", 4, 6);


	go = creator.create("Data/Temperate_Deep.xml", "Forest", 4, 4);
	go = creator.create("Data/Temperate_Deep.xml", "Forest", 5, 4);
	go = creator.create("Data/Temperate_Normal.xml", "Forest", 4, 5);
	go = creator.create("Data/Temperate_Scarce.xml", "Forest", 5, 5);
	go = creator.create("Data/Temperate_Dying.xml", "Forest", 6, 4);
	go = creator.create("Data/Temperate_Dying.xml", "Forest", 4, 6);

	go = creator.create("Data/Spearman.xml", "Spearman_Unit", 4, 4);


	// Show off old forestry with tundra
	go = creator.create("Data/Tundra_Maptile.xml", "Maptile", 6, 6);
	go = creator.create("Data/Tundra_Maptile.xml", "Maptile", 7, 6);
	go = creator.create("Data/Tundra_Maptile.xml", "Maptile", 6, 7);
	go = creator.create("Data/Tundra_Maptile.xml", "Maptile", 7, 7);
	go = creator.create("Data/Tundra_Maptile.xml", "Maptile", 8, 6);
	go = creator.create("Data/Tundra_Maptile.xml", "Maptile", 6, 8);


	go = creator.create("Data/Tundra_Deep.xml", "Forest", 6, 6);
	go = creator.create("Data/Tundra_Deep.xml", "Forest", 7, 6);
	go = creator.create("Data/Tundra_Normal.xml", "Forest", 6, 7);
	go = creator.create("Data/Tundra_Scarce.xml", "Forest", 7, 7);
	go = creator.create("Data/Tundra_Dying.xml", "Forest", 8, 6);
	go = creator.create("Data/Tundra_Dying.xml", "Forest", 6, 8);

	go = creator.create("Data/Spearman.xml", "Spearman_Unit", 6, 6);


	go = creator.create("Data/Water_Shallow_Maptile.xml", "Maptile", 8, 8);
	go = creator.create("Data/Water_Shallow_Maptile.xml", "Maptile", 9, 8);
	go = creator.create("Data/Water_Shallow_Maptile.xml", "Maptile", 8, 9);
	go = creator.create("Data/Water_Deep_Maptile.xml", "Maptile", 9, 9);
	go = creator.create("Data/Snow_Maptile.xml", "Maptile", 10, 9);
	go = creator.create("Data/Snow_Maptile.xml", "Maptile", 9, 10);
	go = creator.create("Data/Snow_Maptile.xml", "Maptile", 10, 10);
	go = creator.create("Data/Snow_Maptile.xml", "Maptile", 10, 8);
	go = creator.create("Data/Ice_Maptile.xml", "Maptile", 11, 9);
	go = creator.create("Data/Ice_Maptile.xml", "Maptile", 9, 11);

	go = creator.create("Data/Snow_Scarce.xml", "Forest", 10, 9);
	go = creator.create("Data/Snow_Normal.xml", "Forest", 9, 10);
	go = creator.create("Data/Snow_Deep.xml", "Forest", 10, 10);
	go = creator.create("Data/Snow_Dying.xml", "Forest", 10, 8);


	go = creator.create("Data/City_Plain.xml", "City", 7, 6);


	NavMesh::get()->bake();


	PlayerInstance* player = new PlayerInstance("Data/Player/HumanPlayer.xml");
	players.push_back(player);


	// Seems to load correctly.


	go = nullptr;
	player = nullptr;
	return true;
}




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



void App::_handleInput()
{
	using namespace std;


	olc::vi2d point = tv.ScreenToWorld({ GetMouseX(), GetMouseY() });
	int mousex = point.x;
	int mousey = point.y;


	// Handle Movement and Paning Input only for WorldMap,
	// where we use the Tiling Transformation.
	if (stateMachine.getCurrentState().compare("worldMap") == 0)
	{
		olc::vi2d topLeft = tv.GetTileUnderScreenPos({ 0, 0 });
		olc::vi2d bottomDown = tv.GetBottomRightTile();
		olc::vi2d middle = { bottomDown.x / 2, bottomDown.y / 2 };

		cout << color(colors::RED);
		cout << "MousePos {" << mousex << "," << mousey << "}" << endl;
		cout << "Tile TopLeft " << topLeft.x << "," << topLeft.y << "}" << endl;
		cout << "Tile BottRight " << bottomDown.x << "," << bottomDown.y << "}" << endl;
		cout << "Tile Middle " << middle.x << "," << middle.y << "}" << white << endl;


		// Do not allow capturing input to imgui and app at same time.
		if (!imgui_has_focus)
		{
			if (GetKey(olc::Key::TAB).bPressed)
			{
				imgui_demo_window = (imgui_demo_window == false) ? true : false;
				gameobjects_window = (gameobjects_window == false) ? true : false;
			}


			if (GetMouse(2).bPressed) tv.StartPan(GetMousePos());
			if (GetMouse(2).bHeld) tv.UpdatePan(GetMousePos());
			if (GetMouse(2).bReleased) tv.EndPan(GetMousePos());
			if (GetMouseWheel() > 0) tv.ZoomAtScreenPos(2.0f, GetMousePos());
			if (GetMouseWheel() < 0) tv.ZoomAtScreenPos(0.5f, GetMousePos());
		}
	}



	if (stateMachine.getCurrentState().compare("worldMap") == 0)
	{

		// Check for RMB Press on a city...
		if (GetMouse(1).bPressed && stateMachine.getCurrentState().compare("worldMap") == 0)
		{
			for (auto& go : GameObjectStorage::get()->getStorage())
			{
				TransformCmp* transform = go->getComponent<TransformCmp>("Transform");
				CollisionBoxCmp* box = go->getComponent<CollisionBoxCmp>("CollisionBox");


				if (mousex >= transform->xpos && mousex <= transform->xpos + box->width &&
					mousey >= transform->ypos && mousey <= transform->ypos + box->height)
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
}



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




bool App::_loadDecalDatabase()
{
	std::string default_path = "Data/Assets/Map/";

	olc::Sprite* sprite = nullptr;
	olc::Decal* decal = nullptr;

	// Loading Tundra
	sprite = new olc::Sprite(default_path + "forest_tundra_normal.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_tundra_normal", decal);

	sprite = new olc::Sprite(default_path + "forest_tundra_scarce.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_tundra_scarce", decal);

	sprite = new olc::Sprite(default_path + "forest_tundra_deep.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_tundra_deep", decal);

	sprite = new olc::Sprite(default_path + "forest_tundra_dying.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_tundra_dying", decal);


	// Loading Jungle
	sprite = new olc::Sprite(default_path + "forest_jungle_deep.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_jungle_deep", decal);

	sprite = new olc::Sprite(default_path + "forest_jungle_normal.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_jungle_normal", decal);

	sprite = new olc::Sprite(default_path + "forest_jungle_scarce.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_jungle_scarce", decal);

	sprite = new olc::Sprite(default_path + "forest_jungle_dying.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_jungle_dying", decal);


	// Loading Temperate
	sprite = new olc::Sprite(default_path + "forest_temperate_dying.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_temperate_dying", decal);


	sprite = new olc::Sprite(default_path + "forest_temperate_deep.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_temperate_deep", decal);


	sprite = new olc::Sprite(default_path + "forest_temperate_normal.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_temperate_normal", decal);


	sprite = new olc::Sprite(default_path + "forest_temperate_scarce.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_temperate_scarce", decal);


	// Loading Savanah
	sprite = new olc::Sprite(default_path + "forest_savannah_scarce.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_savannah_scarce", decal);

	sprite = new olc::Sprite(default_path + "forest_savannah_normal.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_savannah_normal", decal);

	sprite = new olc::Sprite(default_path + "forest_savannah_deep.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_savannah_deep", decal);

	sprite = new olc::Sprite(default_path + "forest_savannah_dying.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_savannah_dying", decal);


	// Load Snow
	sprite = new olc::Sprite(default_path + "forest_snow_scarce.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_snow_scarce", decal);

	sprite = new olc::Sprite(default_path + "forest_snow_normal.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_snow_normal", decal);

	sprite = new olc::Sprite(default_path + "forest_snow_deep.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_snow_deep", decal);

	sprite = new olc::Sprite(default_path + "forest_snow_dying.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("forest_snow_dying", decal);


	// Maptiles.
	sprite = new olc::Sprite(default_path + "jungle.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("jungle", decal);

	sprite = new olc::Sprite(default_path + "sand.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("sand", decal);

	sprite = new olc::Sprite(default_path + "savannah.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("savannah", decal);

	sprite = new olc::Sprite(default_path + "snow.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("snow", decal);

	sprite = new olc::Sprite(default_path + "temperate.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("temperate", decal);

	sprite = new olc::Sprite(default_path + "tundra.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("tundra", decal);

	sprite = new olc::Sprite(default_path + "ice.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("ice", decal);

	sprite = new olc::Sprite(default_path + "water_deep.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("water_deep", decal);

	sprite = new olc::Sprite(default_path + "water_shallow.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("water_shallow", decal);


	// Rivers.
	sprite = new olc::Sprite(default_path + "river_down_left.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("river_down_left", decal);

	sprite = new olc::Sprite(default_path + "river_down_right.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("river_down_right", decal);

	sprite = new olc::Sprite(default_path + "river_left_right.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("river_left_right", decal);

	sprite = new olc::Sprite(default_path + "river_left_right_down.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("river_left_right_down", decal);

	sprite = new olc::Sprite(default_path + "river_left_up_down.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("river_left_up_down", decal);

	sprite = new olc::Sprite(default_path + "river_up_down.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("river_up_down", decal);

	sprite = new olc::Sprite(default_path + "river_up_left.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("river_up_left", decal);

	sprite = new olc::Sprite(default_path + "river_up_right.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("river_up_right", decal);

	sprite = new olc::Sprite(default_path + "river_up_right_down.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("river_up_right_down", decal);

	sprite = new olc::Sprite(default_path + "river_up_right_left.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("river_up_right_left", decal);

	sprite = new olc::Sprite(default_path + "river_up_right_left_down.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("river_up_right_left_down", decal);


	// Hills/Mountains.
	sprite = new olc::Sprite(default_path + "hills.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("hills", decal);

	sprite = new olc::Sprite(default_path + "highmountain.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("highmountain", decal);


	// Units.
	default_path = "Data/Assets/Unit/";
	sprite = new olc::Sprite(default_path + "aristocrat.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("aristocrat", decal);

	sprite = new olc::Sprite(default_path + "assassin.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("assassin", decal);

	sprite = new olc::Sprite(default_path + "assassin_2.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("assassin_2", decal);

	sprite = new olc::Sprite(default_path + "battlemage.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("battlemage", decal);

	sprite = new olc::Sprite(default_path + "battlemage_2.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("battlemage_2", decal);

	sprite = new olc::Sprite(default_path + "civic_researcher.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("civic_researcher", decal);

	sprite = new olc::Sprite(default_path + "crossbowman.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("crossbowman", decal);

	sprite = new olc::Sprite(default_path + "fighter.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("fighter", decal);

	sprite = new olc::Sprite(default_path + "heavy_maceman.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("heavy_maceman", decal);

	sprite = new olc::Sprite(default_path + "heavy_spearman_iron.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("heavy_spearman_iron", decal);

	sprite = new olc::Sprite(default_path + "heavy_swordman_bronze.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("heavy_swordman_bronze", decal);

	sprite = new olc::Sprite(default_path + "heavy_swordman_iron.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("heavy_swordman_iron", decal);

	sprite = new olc::Sprite(default_path + "high_senator.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("high_senator", decal);

	sprite = new olc::Sprite(default_path + "knight_bronze.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("knight_bronze", decal);

	sprite = new olc::Sprite(default_path + "light_archer.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("light_archer", decal);

	sprite = new olc::Sprite(default_path + "light_cavalry.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("light_cavalry", decal);

	sprite = new olc::Sprite(default_path + "local_merchant.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("local_merchant", decal);

	sprite = new olc::Sprite(default_path + "local_merchant.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("local_merchant", decal);

	sprite = new olc::Sprite(default_path + "longswordsman.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("longswordsman", decal);

	sprite = new olc::Sprite(default_path + "mage.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("mage", decal);

	sprite = new olc::Sprite(default_path + "magick_researcher.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("magick_researcher", decal);

	sprite = new olc::Sprite(default_path + "merchant.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("merchant", decal);

	sprite = new olc::Sprite(default_path + "merchant.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("merchant", decal);

	sprite = new olc::Sprite(default_path + "merchant.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("merchant", decal);

	sprite = new olc::Sprite(default_path + "mounted_knight.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("mounted_knight", decal);

	sprite = new olc::Sprite(default_path + "mounted_scout.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("mounted_scout", decal);

	sprite = new olc::Sprite(default_path + "paladin_adamantium.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("paladin_adamantium", decal);

	sprite = new olc::Sprite(default_path + "paladin_malachite.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("paladin_malachite", decal);

	sprite = new olc::Sprite(default_path + "senator.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("senator", decal);

	sprite = new olc::Sprite(default_path + "sorcerer.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("sorcerer", decal);

	sprite = new olc::Sprite(default_path + "spearman.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("spearman", decal);

	sprite = new olc::Sprite(default_path + "spy.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("spy", decal);



	// Ressources.
	default_path = "Data/Assets/Ressource/";
	sprite = new olc::Sprite(default_path + "jungle_small_hunter.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("jungle_small_hunter", decal);

	sprite = new olc::Sprite(default_path + "savannah_small_hunter.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("savannah_small_hunter", decal);

	sprite = new olc::Sprite(default_path + "jungle_mushroom_eater.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("jungle_mushroom_eater", decal);

	sprite = new olc::Sprite(default_path + "wheat.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("wheat", decal);



	// Buildings
	default_path = "Data/Assets/Building/";
	sprite = new olc::Sprite(default_path + "tavern.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("tavern", decal);

	sprite = new olc::Sprite(default_path + "weaponsmiths_workshop.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("weaponsmiths_workshop", decal);


	// Cities
	default_path = "Data/Assets/City/";
	sprite = new olc::Sprite(default_path + "city_plain.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("city_plain", decal);

	sprite = new olc::Sprite(default_path + "fort_plain.png");
	decal = new olc::Decal(sprite);
	decalDatabase.emplace("fort_plain", decal);


	return true;
}




bool App::_loadGameobjectPathdefinitions()
{
	GameObjectCreator::createGameobjectPathDefinition("Spearman", "Data/Spearman.xml");
	GameObjectCreator::createGameobjectPathDefinition("Tavern", "Data/Tavern.xml");
	GameObjectCreator::createGameobjectPathDefinition("Weapon Smiths Workshop", "Data/Weaponsmith.xml");


	return true;
}




void App::renderLayer(const std::string& layerName)
{
	for (auto& go : GameObjectStorage::get()->getStorage())
	{
		if (go->hasComponent("Renderable"))
		{
			RendererableCmp* render = go->getComponent<RendererableCmp>("Renderable");
			if (render->render && render->renderingLayer.compare(layerName) == 0)
			{
				TransformCmp* transform = go->getComponent<TransformCmp>("Transform");

				olc::Decal* decal = decalDatabase[render->decalName];

				tv.DrawDecal(olc::vf2d(transform->xpos, transform->ypos), decal);
			}
		}
	}
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
	cout << color(colors::MAGENTA);
	cout << "[AppStateCityView] update" << white << endl;

	app->SetDrawTarget((uint8_t)app->m_GameLayer);

	ICityCmp* city = app->getCurrentViewedCity()->getComponent<ICityCmp>("City");
	if (city)
	{
		_renderCityBase(city);
		_renderCityBackground(city);
		_renderCityGroundAndWalls(city);
		_renderCityBuildings(city);
		_renderCityUnits(city);
		_renderCityOverlay(city);
	}
}


// Render the basic layout for the city.
void AppStateCityView::_renderCityBase(ICityCmp* city)
{
	// Red Circle Filling Whole Screen Almost -> Available Space.
	app->FillRect(olc::vi2d(5, 5), olc::vi2d(1000, 750), olc::DARK_RED);


	// City Screen.
	app->FillRect(olc::vi2d(5, 5), olc::vi2d(600, 750), olc::DARK_CYAN);

	// City Worldmap Screen.
	app->FillRect(olc::vi2d(605, 5), olc::vi2d(400, 750), olc::DARK_MAGENTA);

	// City UI Screen
	app->FillRect(olc::vi2d(5, 5), olc::vi2d(600, 100), olc::GREEN);

	// Maptile Indication.
	app->FillRect(olc::vi2d(5, 100), olc::vi2d(600, 200), olc::YELLOW);

	// City Ground Indication.
	app->FillRect(olc::vi2d(5, 300), olc::vi2d(600, 455), olc::GREY);

	// Selected Unit/Building etc.
	app->FillRect(olc::vi2d(5, 740), olc::vi2d(600, 15), olc::RED);
}


// Draw the Background like Maptile, Forest or Hill,
// River etc.
void AppStateCityView::_renderCityBackground(ICityCmp* city)
{

}




// Render the Cityground for Buildings and Walls.
void AppStateCityView::_renderCityGroundAndWalls(ICityCmp* city)
{

}




// Render the Buildings currently Build in the city.
void AppStateCityView::_renderCityBuildings(ICityCmp* city)
{
	for (auto& go : city->getBuildings())
	{
		if (go->hasComponent("Renderable"))
		{
			RendererableCmp* render = go->getComponent<RendererableCmp>("Renderable");
			if (render->render && render->renderingLayer.compare("building") == 0)
			{
				TransformCmp* transform = go->getComponent<TransformCmp>("Transform");

				olc::Decal* decal = app->decalDatabase[render->decalName];

				app->DrawDecal(olc::vf2d(transform->xpos, transform->ypos), decal, olc::vf2d(0.25f, 0.25f));
			}
		}
	}
}




// Render Units in the city, working ones, citizens and garrisoned.
void AppStateCityView::_renderCityUnits(ICityCmp* city)
{
	for (auto& go : city->getUnits())
	{
		if (go->hasComponent("Renderable"))
		{
			RendererableCmp* render = go->getComponent<RendererableCmp>("Renderable");
			if (render->render && render->renderingLayer.compare("unit") == 0)
			{
				TransformCmp* transform = go->getComponent<TransformCmp>("Transform");

				olc::Decal* decal = app->decalDatabase[render->decalName];

				app->DrawDecal(olc::vf2d(transform->xpos, transform->ypos), decal, olc::vf2d(0.25f, 0.25f));
			}
		}
	}
}




// Render needed UI elements for the city etc.
void AppStateCityView::_renderCityOverlay(ICityCmp* city)
{

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

void AppStateWorldMap::update(float)
{
	using namespace std;
	cout << color(colors::MAGENTA);
	cout << "[AppStateWorldMap] update" << white << endl;


	// Do the standard update.
	olc::TileTransformedView tv = app->getRenderer();


	// Update all agents.
	for (auto& go : GameObjectStorage::get()->getStorage())
	{
		// Update Navigator, for each entity which can move, let it move.
		if (go->hasComponent("Navigator"))
		{
			go->getComponent<NavigatorCmp>("Navigator")->update(GameWorldTime::get()->getTimeSpeed());
		}
	}


	// Application rendering.
	olc::vi2d topleft = tv.GetTopLeftTile().max({ 0, 0 });
	olc::vi2d bottomright = tv.GetBottomRightTile().min({ DEFAULT_DECAL_SIZE_X, DEFAULT_DECAL_SIZE_Y });
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



	// Draw General Layered
	app->renderLayer("maptile");
	app->renderLayer("mountain");
	app->renderLayer("forest");
	app->renderLayer("river");
	app->renderLayer("city");
	app->renderLayer("unit");
	app->renderLayer("overlay");


	if (selected_gameobject)
	{
		TransformCmp* tr = static_cast<TransformCmp*> (selected_gameobject->getComponent("Transform"));
		RendererableCmp* rc = static_cast<RendererableCmp*> (selected_gameobject->getComponent("Renderable"));

		if (tr != nullptr && rc != nullptr)
		{
			olc::vf2d p = { tr->xpos + rc->width / 2.0f - 0.3f, tr->ypos + rc->height / 2.0f };
			tv.DrawStringDecal(p, selected_gameobject->tag, olc::RED, olc::vf2d(0.5f, 0.5f));
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


	if (show_nav_mesh)
	{
		// Draw Nodes
		std::vector<std::vector<int>> nodes = NavMesh::get()->getNavGraph();
		Graph* graph = NavMesh::get()->getGraph();
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


	if (show_nav_path)
	{
		// Draw waypoints of selected entity,
		// if it has one.
		if (selected_gameobject)
		{
			if (selected_gameobject->hasComponent("Navigator"))
			{
				NavigatorCmp* nav = selected_gameobject->getComponent<NavigatorCmp>("Navigator");


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



	// Draw ImGui.
	_drawUI();
}

void AppStateWorldMap::_drawUI()
{
	using namespace std;

	app->SetDrawTarget((uint8_t)app->m_GameLayer);

	selected_gameobject = nullptr;


	// CHECK WHETHER IMGUI IS FOCUSED
	if (ImGui::IsAnyItemFocused() || ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
	{
		imgui_has_focus = true;
	}
	else
	{
		imgui_has_focus = false;
	}


	// DEMO
	if (imgui_demo_window)
	{
		ImGui::ShowDemoWindow(&imgui_demo_window);
	}


	// MAIN MENU BAR
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{



			if (ImGui::BeginMenu("Time Speed"))
			{

				float speed = GameWorldTime::get()->getTimeSpeed();
				ImGui::SliderFloat("Timespeed", &speed, 0.0f, 2.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
				GameWorldTime::get()->setTimeSpeed(speed);

				ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("Collider"))
			{
				if (ImGui::MenuItem("Collision Display"))
				{
					show_collisions = (show_collisions == true) ? false : true;
				}
				ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("Navigator"))
			{

				if (ImGui::MenuItem("NavMesh"))
				{
					show_nav_mesh = (show_nav_mesh == true) ? false : true;
				}
				if (ImGui::MenuItem("MovePoints"))
				{
					show_nav_path = (show_nav_path == true) ? false : true;
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Editor"))
			{
				if (ImGui::MenuItem("New"))
				{

				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Technology Tree"))
			{
				if (ImGui::MenuItem("Display"))
				{
					show_tech_tree = (show_tech_tree == true) ? false : true;
				}
				ImGui::EndMenu();
			}


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}




	// GAMEOBJECTS WINDOW
	int go_count = GameObjectStorage::get()->getStorage().size();
	ImGui::SetNextWindowPos(ImVec2(1.0f, 15.0f), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(350.0f, 600.0f), ImGuiCond_Appearing);
	if (ImGui::Begin("GameObjects", &gameobjects_window))
	{
		for (auto& go : GameObjectStorage::get()->getStorage())
		{
			// Show GO
			std::string tag_name = go->getTag() + " \"" + go->getName() + "\"";
			bool ret = ImGui::CollapsingHeader(tag_name.c_str());

			// Check whether we are hovering over the current displayed GO.
			if (ImGui::IsItemHovered())
			{
				selected_gameobject = go;
			}

			// Show the components of Selected GO.
			if (ret)
			{
				// Show CMPs
				for (auto& cmp : go->components)
				{
					if (ImGui::TreeNode(cmp->name.c_str()))
					{
						if (cmp->getType().find("Transform") != std::string::npos)
						{
							int v[2];
							v[0] = static_cast<TransformCmp*>(cmp)->xpos;
							v[1] = static_cast<TransformCmp*>(cmp)->ypos;

							if (ImGui::SliderInt2("Position", v, -DEFAULT_MAPSIZE_X, DEFAULT_MAPSIZE_X))
							{
								static_cast<TransformCmp*>(cmp)->xpos = v[0];
								static_cast<TransformCmp*>(cmp)->ypos = v[1];
							}
						}


						if (cmp->getType().find("Renderable") != std::string::npos)
						{
							RendererableCmp* rc = static_cast<RendererableCmp*>(cmp);

							ImGui::Text(rc->color.c_str());

							ImGui::SameLine();

							ImGui::Checkbox("Render", &rc->render);

							ImGui::Text("Decal \"%s\"", rc->decalName.c_str());

							ImGui::Text("Layer \"%s\"", rc->renderingLayer.c_str());
						}


						if (cmp->getType().find("Unit") != std::string::npos)
						{
							IUnitCmp* uc = static_cast<IUnitCmp*>(cmp);

							if (ImGui::TreeNode("Requirements"))
							{
								if (ImGui::TreeNode("Tech"))
								{
									// Get the requirements.
									std::vector<TechID>& req = uc->getRequiredTech();

									for (auto& r : req)
									{
										ImGui::Text("\"%s\"", r.c_str());
									}

									ImGui::TreePop();
								}

								if (ImGui::TreeNode("Ressources"))
								{
									std::vector<std::pair<std::string, int>>& req = uc->getRequiredRessources();

									for (auto& r : req)
									{
										ImGui::Text("%d \"%s\"", r.second, r.first.c_str());
									}

									ImGui::TreePop();
								}

								if (ImGui::TreeNode("Race"))
								{
									std::vector<RaceID>& req = uc->getRequiredRace();

									for (auto& r : req)
									{
										ImGui::Text("\"%s\"", r.c_str());
									}

									ImGui::TreePop();
								}


								ImGui::TreePop();
							}


						}

						if (cmp->getType().find("Building") != std::string::npos)
						{

						}

						if (cmp->getType().find("Improvement") != std::string::npos)
						{

						}



						ImGui::TreePop();
					}

					ImGui::Separator();
				}
			}
		}
	}
	ImGui::End();



	if (ImGui::Begin("DecalDatabase"))
	{
		for (auto& decal : app->decalDatabase)
		{
			if (ImGui::ImageButton((ImTextureID)decal.second->id, ImVec2(64, 64), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 1)))
			{
				GameObjectStorage::get()->getGOByName("Spearman_Unit")->getComponent<RendererableCmp>("Renderable")->decalName = decal.first;
			}

			ImGui::SameLine();

			ImGui::Text(decal.first.c_str());
		}
	}
	ImGui::End();

	if (show_tech_tree)
	{
		using namespace std;

		if (!imnodes_tech_tree_initialized)
		{
			for (auto& node : app->techTree)
			{
				app->techTreeNodes.push_back(ImNodesNode(node->getID(), imnodes_tech_node_id++, ITech::getTechAreaAsText(node->getTechArea()), node->getSubcategory()));
			}


			for (auto& node : app->techTree)
			{
				// Get technology dependency
				for (auto& req : node->checks)
				{

					if (req.first->type().compare("string") == 0)
					{
						std::string techReq = req.first->as<std::string>();

						// Dont create a link if the requirement is a building etc.
						if (ITech::getCheckAreaAsText(req.second.second).compare("player_tech_check") == 0)
						{
							// Create a link.
							int startid;
							int endid;

							// To...
							for (auto& n : app->techTreeNodes)
							{
								if (n.name.compare(node->getID()) == 0)
								{
									startid = n.id << 8;

									// Store Dependency name for later display.
									n.techDependencies.emplace(techReq, imnodes_tech_dependency_display_id++);
								}
							}


							// From...
							for (auto& n : app->techTreeNodes)
							{
								if (n.name.compare(techReq) == 0)
								{
									endid = n.id << 24;
								}
							}


							app->links.push_back(ImNodesLink(imnodes_tech_link_id++, startid, endid));
						}
						else
						{
							for (auto& n : app->techTreeNodes)
							{
								if (n.name.compare(node->getID()) == 0)
								{
									n.dependencies.emplace(techReq, imnodes_tech_dependency_id++);
								}
							}
						}
					}
				}
			}


			imnodes_tech_tree_initialized = true;
		}


		// Render Tree With Nodes.
		ImGui::SetNextWindowPos(ImVec2(1.0f, 15.0f), ImGuiCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(600.0f, 600.0f), ImGuiCond_Appearing);
		ImGui::Begin("Technology Tree", &show_tech_tree, ImGuiWindowFlags_None);

		ImNodes::BeginNodeEditor();


		for (auto& tech : app->techTreeNodes)
		{
			// For Technologies which were already researched by current player,
			// we can color them differently like this:
			// Note to call "ImNodes::PopColorStyle();" after "ImNodes::EndNode();"
			//ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(255, 109, 191, 255));

			// Colorize the techs after the researcharea and subcategory.
			ImNodes::PushStyleVar(ImNodesStyleVar_NodeBorderThickness, 1.0f);
			if (tech.area.compare("military") == 0)
			{
				ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(200, 0, 0, 255)); // Lighter Red
				ImNodes::PushColorStyle(ImNodesCol_NodeBackground, IM_COL32(150, 0, 0, 255)); // Dark Red
				ImNodes::PushColorStyle(ImNodesCol_NodeOutline, IM_COL32(0, 0, 64, 255));
			}
			else if (tech.area.compare("civics") == 0)
			{
				ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(30, 175, 0, 255)); // Lighter Green
				ImNodes::PushColorStyle(ImNodesCol_NodeBackground, IM_COL32(20, 125, 0, 255)); // Dark Green
				ImNodes::PushColorStyle(ImNodesCol_NodeOutline, IM_COL32(0, 0, 64, 255));
			}
			else if (tech.area.compare("technical") == 0)
			{
				ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(170, 70, 0, 255)); // Lighter Brown
				ImNodes::PushColorStyle(ImNodesCol_NodeBackground, IM_COL32(125, 60, 0, 255)); // Dark Brown
				ImNodes::PushColorStyle(ImNodesCol_NodeOutline, IM_COL32(0, 0, 64, 255));

			}
			else if (tech.area.compare("magick") == 0)
			{
				ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(170, 0, 230, 255)); // Lighter Purple
				ImNodes::PushColorStyle(ImNodesCol_NodeBackground, IM_COL32(125, 0, 170, 255)); // Dark Purple
				ImNodes::PushColorStyle(ImNodesCol_NodeOutline, IM_COL32(0, 0, 64, 255));
			}



			for (auto& playerTech : app->players[0]->techs)
			{
				if (playerTech.first.compare(tech.name) == 0)
				{
					if (playerTech.second == 1)
					{
						ImNodes::PushStyleVar(ImNodesStyleVar_NodeBorderThickness, 5.0f);
						ImNodes::PushColorStyle(ImNodesCol_NodeOutline, IM_COL32(250, 255, 0, 255));
						ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(255, 255, 255, 255));
					}
				}
			}


			ImNodes::BeginNode(tech.id);
			ImNodes::BeginNodeTitleBar();
			ImGui::TextUnformatted(tech.name.c_str());
			ImNodes::EndNodeTitleBar();

			ImNodes::BeginInputAttribute(tech.id << 8);
			ImGui::TextUnformatted("Need");
			ImNodes::EndInputAttribute();


			for (auto& dep : tech.dependencies) // Buildings and other.
			{
				ImNodes::BeginInputAttribute(dep.second);
				ImGui::TextUnformatted(dep.first.c_str());
				ImNodes::EndInputAttribute();
			}
			for (auto& dep : tech.techDependencies) // Technologies for display.
			{
				ImNodes::BeginInputAttribute(dep.second);
				ImGui::TextUnformatted(dep.first.c_str());
				ImNodes::EndInputAttribute();
			}




			ImNodes::BeginOutputAttribute(tech.id << 24);
			ImGui::Indent(ImGui::CalcTextSize("Need").x + ImGui::CalcTextSize(tech.name.c_str()).x);
			ImGui::TextUnformatted("Unlocks");
			ImNodes::EndOutputAttribute();


			// Draw Icon
			ImGui::ImageButton((ImTextureID)app->decalDatabase["assassin"]->id, ImVec2(32, 32), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 1));


			ImNodes::EndNode();


			ImNodes::PopColorStyle();
			ImNodes::PopColorStyle();
			ImNodes::PopColorStyle();
			ImNodes::PopStyleVar();
		}


		ImNodes::PushColorStyle(ImNodesCol_LinkSelected, IM_COL32(230, 230, 255, 255));
		for (auto& link : app->links)
		{
			ImNodes::Link(link.id, link.start, link.end);
		}
		ImNodes::PopColorStyle();


		ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight);
		ImNodes::EndNodeEditor();

		ImGui::End();

	}



	static bool research_test = true;
	// Draw Testing For Technology Tree.
	if (ImGui::Begin("Testing Technology Research", &research_test))
	{
		static std::vector< TechInstance* > vec;

		if (ImGui::Button("Get Next Military Techs"))
		{

			vec = app->getNextTechToChoose(app->players[0], ITech::TA_MILITARY);
		}
		if (ImGui::Button("Get Next Magick Techs"))
		{

			vec = app->getNextTechToChoose(app->players[0], ITech::TA_MAGIC);
		}
		if (ImGui::Button("Get Next Civics Techs"))
		{

			vec = app->getNextTechToChoose(app->players[0], ITech::TA_CIVICS);
		}
		if (ImGui::Button("Get Next Technical Techs"))
		{

			vec = app->getNextTechToChoose(app->players[0], ITech::TA_TECHNICAL);
		}


		for (auto& tech : vec)
		{
			if (ImGui::Button(tech->getID().c_str()))
			{
				// Let player instantly research...
				app->players[0]->techs[tech->getID()] = 1;
			}
		}
	}
	ImGui::End();
}

void AppStateWorldMap::onEnter()
{
	using namespace std;
	cout << color(colors::MAGENTA);
	cout << "[AppStateMainMenu] onEnter" << white << endl;
}

void AppStateWorldMap::onExit()
{
	using namespace std;
	cout << color(colors::MAGENTA);
	cout << "[AppStateMainMenu] onExit" << white << endl;
}