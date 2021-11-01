#pragma once

/*
* OpenGL, GLFW
*/
#include"common/include/GL/glew.h"
#pragma comment(lib, "glew32.lib")

#include"common/include/GLFW/glfw3.h"
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "opengl32.lib")

/*
* OLC
*/
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#define OLC_PGEX_TRANSFORMEDVIEW
#include "olcPGEX_TransformedView.h"
#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include "common/include/imgui_impl_pge.h"
#define OLC_PGEX_FONT
#include "olcPGEX_Font.h"
#define OLC_PGEX_CUSTOMFONT
#include "olcPGEX_CustomFont.h"


/*
* ImGui includes for Technology Tree.
*/
#include "imnodes.h"
struct ImNodesLink
{
	ImNodesLink(int id, int start, int end) : id(id), start(start), end(end) {};
	~ImNodesLink()
	{
		id = 0;
		start = 0;
		end = 0;
	}

	int id, start, end;
};


struct ImNodesNode
{
	ImNodesNode(const std::string& name, int id,
		const std::string& area, const std::string& category) :name(name), id(id),
	area(area), subCategory(category) {};

	~ImNodesNode()
	{
		name.clear();
		area.clear();
		subCategory.clear();
		id = 0;
		while (dependencies.size() > 0)
		{
			dependencies.erase(dependencies.begin());
		}

		while (techDependencies.size() > 0)
		{
			techDependencies.erase(techDependencies.begin());
		}
	}

	std::string name;
	std::string area;
	std::string subCategory;
	int id;
	std::map< std::string, int > dependencies; // Which are not technologies.
	std::map< std::string, int > techDependencies; // Which are for display only.
};




/*
* Common includes.
*/
#include "GameWorldTime.h"
#include "Random.h"
#include "FiniteStateMachine.h"

#include "AVLTree.h"

#include "TechInstance.h"
#include "PlayerInstance.h"


#include "GameObjectStorage.h"
#include "JobSystem.h"


#include "ForestSystem.h"
#include "ReligionSystem.h"


struct AppStateWorldMap;
struct AppStateCityView;


using GameworldMatrix = std::vector< std::vector< Pointer<GameObject2> >>;


void UpdateForestSystem(GameworldMatrix& world);
void UpdateReligionSystem(GameworldMatrix& world);


class App : public olc::PixelGameEngine
{
	friend class AppStateWorldMap;
	friend class AppStateCityView;

	static size_t g_DecalInstanceID;

public:
	App() : pge_imgui(false)
	{
		sAppName = "APP";
	}

	void shutDown()
	{
		for (auto n : decalIDMap)
		{
			decalDatabase.findStoredData(n.second).reset();
		}


		while (sprites.size() > 0)
		{
			sprites[0].reset();
			sprites.erase(sprites.begin());
		}

		while (decalNamesInDatabase.size() > 0)
		{
			decalNamesInDatabase[0].clear();
			decalNamesInDatabase.erase(decalNamesInDatabase.begin());
		}

		while (decalIDMap.size() > 0)
		{
			decalIDMap.erase(decalIDMap.begin());
		}

		while (techTreeNodes.size() > 0)
		{
			techTreeNodes[0].~ImNodesNode();
			techTreeNodes.erase(techTreeNodes.begin());
		}

		while (links.size() > 0)
		{
			links[0].~ImNodesLink();
			links.erase(links.begin());
		}

		for (int i = 0; i < gameWorldMatrix.size(); i++)
		{
			for (int j = 0; j < gameWorldMatrix[i].size(); j++)
			{
				gameWorldMatrix[i][j].reset();
			}
		}

		delete font;
		font = nullptr;
	}

public:

	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
	void DrawUI(void);
	//std::vector< TechInstance* > getNextTechToChoose();
	StateMachine<Pointer<App>>& getStateMachine() { return stateMachine; }
	olc::TileTransformedView& getRenderer() { return tv; }
	
	Pointer<GameObject2> getCurrentViewedCity() { return currentViewedCity;}

private:

	olc::TileTransformedView tv;
	olc::imgui::PGE_ImGUI pge_imgui;
	int m_GameLayer;

	// Camera Movement
	bool isPanning = false;
	int cameraSpeed = 1;
	int camerax = 0;
	int cameray = 0;

	//std::map< std::string, olc::Decal* > decalDatabase;
	std::vector< Pointer<olc::Sprite> > sprites;
	std::vector< std::string > decalNamesInDatabase;
	AVLTree2< Pointer<olc::Decal> > decalDatabase;
	std::map< std::string, size_t > decalIDMap;

	std::string lastSelectedGameobjectTag = "none";

	// A vector of all Technologies in game.
	// Currently for debug/display usage.
	//std::vector< TechInstance* > techTree;
	std::vector< ImNodesNode > techTreeNodes;
	std::vector< ImNodesLink > links;


	// Players in game.
	//std::vector< PlayerInstance* > players;


	// The city the player associated with this App is currenlty viewing.
	Pointer<GameObject2> currentViewedCity = nullptr;

	// The State machine associated with this App.
	StateMachine<Pointer<App>> stateMachine;

	// The Gameworld represented as a matrix.
	// In here are stored maptile Gameobject according to their position in 
	// the gameworld. Accessing a maptile at coordinates {x=10, y=32}
	// is done with gameWorldMatrix[10][32].
	GameworldMatrix gameWorldMatrix;
	olc::vi2d visiblePointLeftUp;
	olc::vi2d visiblePointDownRight;


	// Font for rendering Text in game.
	olc::Font* font = nullptr;

private:

	// Helpers
	void _storeDecal(const std::string& name, size_t id, Pointer<olc::Decal> decal, Pointer<olc::Sprite> sprite);
	Pointer<olc::Decal> _getDecal(const std::string& name);

	//void _onImGui();
	void _handleInput();
	void _updateVisibleRect();
	bool _loadDecalDatabase();
	bool _loadTechTreeDefinitions();
	bool _loadAppStateDefinitions();
	bool _initGameworldMatrix();



	olc::Pixel _getColorFromString(const std::string& c)
	{
		if (c.compare("grey") == 0) return olc::GREY;
		else if (c.compare("dark_grey") == 0)return olc::DARK_GREY;

		else if (c.compare("green") == 0) return olc::GREEN;
		else if (c.compare("dark_green") == 0) return olc::DARK_GREEN;

		else if (c.compare("blue") == 0) return olc::BLUE;
		else if (c.compare("dark_blue") == 0) return olc::DARK_BLUE;

		else if (c.compare("red") == 0) return olc::RED;
		else if (c.compare("dark_red") == 0) return olc::DARK_RED;

		else if (c.compare("yellow") == 0) return olc::YELLOW;
		else if (c.compare("dark_yellow") == 0) return olc::DARK_YELLOW;

		else if (c.compare("magenta") == 0) return olc::MAGENTA;
		else if (c.compare("dark_magenta") == 0) return olc::DARK_MAGENTA;
		
		else return olc::WHITE;
	}
};



struct AppStateWorldMap : public State<Pointer<App>>
{
	AppStateWorldMap(Pointer<App> app): app(app){}
	~AppStateWorldMap()
	{
		app.reset();
	}


	void update(float) override final;

	void onEnter() override final;

	void onExit() override final;

	Pointer<App> getOwner() override final
	{
		return app;
	}


private:
	Pointer<App> app = nullptr;

	void _updateGameworldMatrix(GameworldMatrix& world);
	void _renderMaptile(Pointer<GameObject2> tile);
	void _renderGameworld();
	void _drawUI();
};



struct AppStateCityView : public State<Pointer<App>>
{
	AppStateCityView(Pointer<App> app) : app(app) {}
	~AppStateCityView()
	{
		app.reset();
	}

	void update(float) override final;

	void onEnter() override final;

	void onExit() override final;

	Pointer<App> getOwner() override final
	{
		return app;
	}


private:
	Pointer<App> app = nullptr;



	// Render the basic layout for the city.
	void _renderCityBase(Pointer<GameObject2> city);


	// Draw the Background like Maptile, Forest or Hill,
	// River etc.
	void _renderCityBackground(Pointer<GameObject2> city);


	// Render the Cityground for Buildings and Walls.
	void _renderCityGroundAndWalls(Pointer<GameObject2> city);


	// Render the Buildings currently Build in the city.
	void _renderCityBuildings(Pointer<GameObject2> city);


	// Render Units in the city, working ones, citizens and garrisoned.
	void _renderCityUnits(Pointer<GameObject2> city);


	// Render needed UI elements for the city etc.
	void _renderCityOverlay(Pointer<GameObject2> city);

};



struct AppStateMainMenu : public State<Pointer<App>>
{
	AppStateMainMenu(Pointer<App> app) : app(app) {}
	~AppStateMainMenu()
	{
		app.reset();
	}

	void update(float) override final;

	void onEnter() override final;

	void onExit() override final;

	Pointer<App> getOwner() override final
	{
		return app;
	}


private:
	Pointer<App> app = nullptr;
};