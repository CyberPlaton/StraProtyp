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

/*
* Helper, maybe GUI related vars.
*/
static GameObject* selected_gameobject = nullptr;


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
	renderLayer("maptile");
	renderLayer("mountain");
	renderLayer("forest");
	renderLayer("river");
	renderLayer("city");
	renderLayer("unit");
	renderLayer("overlay");


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


	// For Rendering IMGUI.
	_onImGui();
	
	return true;
}






bool App::OnUserCreate()
{
	using namespace std;

	// Load Assets
	if (!_loadDecalDatabase()) return false;

	m_GameLayer = CreateLayer();
	EnableLayer(m_GameLayer, true);
	SetLayerCustomRenderFunction(0, std::bind(&App::DrawUI, this));



	tv = olc::TileTransformedView({ ScreenWidth(), ScreenHeight() }, { DEFAULT_DECAL_SIZE_X, DEFAULT_DECAL_SIZE_Y });

	
	GameWorldTime::get()->setTimeSpeed(0.016);


	GameObjectCreator creator;
	GameObject* go = creator.create("Data/Jungle_Maptile.xml", "Maptile", 0, 0);
	go = creator.create("Data/Jungle_Scarce.xml", "Forest", 0, 0);
	go = creator.create("Data/Mountain.xml", "Mountain", 0, 0);
	go = creator.create("Data/Spearman.xml", "Spearman_Unit", 0, 0);

	NavMesh::get()->bake();


	TechInstance tech("Data/Tech/ExampleTech.xml");

	
	return true;
}






int main()
{
	App demo;
	if (demo.Construct(648, 512, 2, 2))
		demo.Start();


	NavMesh::del();
	GameObjectStorage::del();

	_CrtCheckMemory();
	_CrtDumpMemoryLeaks();

	return 0;
}


void App::_onImGui()
{
	using namespace std;

	SetDrawTarget((uint8_t)m_GameLayer);

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
			std::string tag_name = go->getTag() + " \"" + go->getName() +"\"";
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
		for (auto& decal : decalDatabase)
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
}



void App::_handleInput()
{
	using namespace std;

	// Do not allow capturing input to imgui and app at same time.
	if (!imgui_has_focus)
	{
		if (GetKey(olc::Key::TAB).bPressed)
		{
			imgui_demo_window = (imgui_demo_window == false) ? true : false;
			gameobjects_window = (gameobjects_window == false) ? true : false;
		}


		if (GetMouse(1).bPressed) tv.StartPan(GetMousePos());
		if (GetMouse(1).bHeld) tv.UpdatePan(GetMousePos());
		if (GetMouse(1).bReleased) tv.EndPan(GetMousePos());
		if (GetMouseWheel() > 0) tv.ZoomAtScreenPos(2.0f, GetMousePos());
		if (GetMouseWheel() < 0) tv.ZoomAtScreenPos(0.5f, GetMousePos());
	}
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