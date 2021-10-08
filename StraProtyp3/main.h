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


#include "imnodes.h"
struct ImNodesLink
{
	ImNodesLink(int id, int start, int end) : id(id), start(start), end(end) {};
	int id, start, end;
};


struct ImNodesNode
{
	ImNodesNode(const std::string& name, int id,
		const std::string& area, const std::string& category) :name(name), id(id),
	area(area), subCategory(category) {};

	std::string name;
	std::string area;
	std::string subCategory;
	int id;
	std::map< std::string, int > dependencies; // Which are not technologies.
	std::map< std::string, int > techDependencies; // Which are for display only.
};

/*
*/
#include "ComponentSystem.h"
#include "Components.h"
#include "GameObject.h"
#include "GameObjectCreator.h"
#include "GameWorld.h"
#include "GameWorldTime.h"
#include "Random.h"
#include "IMaptile.h"
#include "MaptileRegions.h"
#include "FiniteStateMachine.h"


#include "TechInstance.h"
#include "PlayerInstance.h"


struct AppStateWorldMap;
struct AppStateCityView;

class App : public olc::PixelGameEngine
{
	friend class AppStateWorldMap;
	friend class AppStateCityView;


public:
	App() : pge_imgui(false)
	{
		sAppName = "APP";
	}

	void shutDown()
	{
		// Free Players
		while (players.size() > 0)
		{
			delete players[0];
			players.erase(players.begin());
		}

		// Free Decals
		for (auto& decal : decalNamesInDatabase)
		{
			decalDatabase.deleteNode(hasher(decal));
		}
		decalNamesInDatabase.clear();

		// Free Techtree
		while (techTree.size() > 0)
		{
			delete techTree[0];
			techTree.erase(techTree.begin());
		}

		techTreeNodes.clear();
		links.clear();
	}

public:

	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
	void DrawUI(void);
	std::vector< TechInstance* > getNextTechToChoose(IPlayer* player, ITech::TechArea area);
	StateMachine<App>& getStateMachine() { return stateMachine; }
	olc::TileTransformedView& getRenderer() { return tv; }
	
	GameObject* getCurrentViewedCity() { return currentViewedCity;}

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
	std::vector< std::string > decalNamesInDatabase;
	AVLTree2< olc::Decal* > decalDatabase;

	std::string lastSelectedGameobjectTag = "none";

	// A vector of all Technologies in game.
	// Currently for debug/display usage.
	std::vector< TechInstance* > techTree;
	std::vector< ImNodesNode > techTreeNodes;
	std::vector< ImNodesLink > links;


	// Players in game.
	std::vector< PlayerInstance* > players;


	GameObject* currentViewedCity = nullptr;


	StateMachine<App> stateMachine;



	// Font for rendering Text in game.
	olc::Font* font = nullptr;

private:

	// Helpers
	void _storeDecal(const std::string& name, olc::Decal* decal);
	olc::Decal* _getDecal(const std::string& name);

	//void _onImGui();
	void _handleInput();
	bool _loadDecalDatabase();
	bool _loadTechTreeDefinitions();
	bool _loadAppStateDefinitions();
	bool _loadGameobjectPathdefinitions();


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



struct AppStateWorldMap : public State<App>
{
	AppStateWorldMap(App* app): app(app){}

	void update(float) override final;

	void onEnter() override final;

	void onExit() override final;

	App* getOwner() override final
	{
		return app;
	}


private:
	App* app = nullptr;


	void _renderMaptile(GameObject* tile);
	void _computeVisibilityRect(olc::vi2d& upLeft, olc::vi2d& downRight);
	bool _isMaptileVisible(GameObject* go);
	void _renderGameworld();
	void _drawUI();
};



struct AppStateCityView : public State<App>
{
	AppStateCityView(App* app) : app(app) {}


	void update(float) override final;

	void onEnter() override final;

	void onExit() override final;

	App* getOwner() override final
	{
		return app;
	}


private:
	App* app = nullptr;



	// Render the basic layout for the city.
	void _renderCityBase(ICityCmp* city);


	// Draw the Background like Maptile, Forest or Hill,
	// River etc.
	void _renderCityBackground(ICityCmp* city);


	// Render the Cityground for Buildings and Walls.
	void _renderCityGroundAndWalls(ICityCmp* city);


	// Render the Buildings currently Build in the city.
	void _renderCityBuildings(ICityCmp* city);


	// Render Units in the city, working ones, citizens and garrisoned.
	void _renderCityUnits(ICityCmp* city);


	// Render needed UI elements for the city etc.
	void _renderCityOverlay(ICityCmp* city);

};



struct AppStateMainMenu : public State<App>
{
	AppStateMainMenu(App* app) : app(app) {}


	void update(float) override final;

	void onEnter() override final;

	void onExit() override final;

	App* getOwner() override final
	{
		return app;
	}


private:
	App* app = nullptr;
};