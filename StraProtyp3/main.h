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
#define OLC_PGEX_HIDMANAGER
#include "olcPGEX_HIDManager.h"


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
#include "TechnologySystem.h"
#include "NationSystem.h"
#include "UnitCreationSystem.h"
#include "PlayerSystem.h"


struct AppStateWorldMap;
struct AppStateCityView;


using GameworldMatrix = std::vector< std::vector< Pointer<GameObject2> >>;


void UpdateGameSystems(GameworldMatrix& world);
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
		while (m_DecalDatabase.size() > 0)
		{
			m_DecalDatabase[0].reset();
			m_DecalDatabase.erase(m_DecalDatabase.begin());
		}
		decalIDMap.clear();

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

		stateMachine.shutDown();
		currentViewedCity.reset();
	}

public:

	/*
	* Custom rendering functions,
	* example usage:
	* 
	* 	DrawCircleDecal({ 250.1f, 250.1f }, { 1.0f, 1.0f }, { DEFAULT_DECAL_SIZE_X / 2, DEFAULT_DECAL_SIZE_Y / 2 }, olc::BLUE);
	*	DrawElipseDecal({ 200.1f, 550.1f }, { 0.25f, 0.56f }, { DEFAULT_DECAL_SIZE_X / 2, DEFAULT_DECAL_SIZE_Y / 2 }, olc::CYAN);
	*	DrawRectangleDecal({ 100.1f, 150.1f }, { 0.25f, 0.56f }, { DEFAULT_DECAL_SIZE_X / 2, DEFAULT_DECAL_SIZE_Y / 2 }, olc::RED);
	*	DrawTriangleDecal({ 200.1f, 550.1f }, { 0.25f, 0.1f }, { DEFAULT_DECAL_SIZE_X / 2, DEFAULT_DECAL_SIZE_Y / 2 }, olc::GREEN);
	*	DrawLineDecal(olc::vf2d(-10.0f, 0.0f), olc::vf2d(900.0f, 710.0f), olc::DARK_MAGENTA);
	*
	*	DrawCircleDecalTransformed({ 0, 0 }, { 1.0f, 1.0f }, olc::DARK_GREEN);
	*	DrawElipseDecalTransformed({ 1, 1 }, { 1.0f, 1.0f }, olc::DARK_CYAN);
	*	DrawRectangleDecalTransformed({ 1, 0 }, { 1.0f, 1.0f }, olc::DARK_MAGENTA);
	*	DrawTriangleDecalTransformed({ 0, 1 }, { 1.0f, 1.0f }, olc::MAGENTA);
	*	DrawLineDecalTransformed({ 1, 1 }, { 19, 19 }, olc::RED);
	* 
	*/
	void DrawCircleDecal(const olc::vf2d& pos, const olc::vf2d& scale, const olc::vf2d& center, const olc::Pixel& tint);
	void DrawElipseDecal(const olc::vf2d& pos, const olc::vf2d& scale, const olc::vf2d& center, const olc::Pixel& tint);
	void DrawRectangleDecal(const olc::vf2d& pos, const olc::vf2d& scale, const olc::vf2d& center, const olc::Pixel& tint);
	void DrawTriangleDecal(const olc::vf2d& pos, const olc::vf2d& scale, const olc::vf2d& center, const olc::Pixel& tint);

	void DrawCircleDecalTransformed(const olc::vf2d& pos, const olc::vf2d& scale, const olc::Pixel& tint);
	void DrawElipseDecalTransformed(const olc::vf2d& pos, const olc::vf2d& scale, const olc::Pixel& tint);
	void DrawRectangleDecalTransformed(const olc::vf2d& pos, const olc::vf2d& scale, const olc::Pixel& tint);
	void DrawTriangleDecalTransformed(const olc::vf2d& pos, const olc::vf2d& scale, const olc::Pixel& tint);
	void DrawLineDecalTransformed(const olc::vf2d& from, const olc::vf2d& to, const olc::Pixel& tint);



	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
	void DrawUI(void);
	StateMachine<App*>& getStateMachine() { return stateMachine; }
	olc::TileTransformedView& getRenderer() { return tv; }
	
	Pointer<GameObject2> getCurrentViewedCity() { return currentViewedCity;}

private:

	olc::hid::OLCHIDManager* g_HIDManager = nullptr;

	olc::TileTransformedView tv;
	olc::imgui::PGE_ImGUI pge_imgui;
	int m_GameLayer;

	// Camera Movement
	bool isPanning = false;
	int cameraSpeed = 1;
	int camerax = 0;
	int cameray = 0;


	std::vector< Pointer<olc::Sprite> > sprites;
	std::vector< std::string > decalNamesInDatabase;
	std::vector< Pointer< olc::Decal > > m_DecalDatabase;
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
	StateMachine<App*> stateMachine;

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
	void _storeDecal(std::string name, size_t id, Pointer<olc::Decal> decal, Pointer<olc::Sprite> sprite);
	Pointer<olc::Decal> _getDecal(std::string name);

	void _handleInput();
	void _updateVisibleRect();
	bool _loadDecalDatabase();
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



struct AppStateWorldMap : public State<App*>
{
	AppStateWorldMap(App* app): app(app){}
	~AppStateWorldMap()
	{
	}

	void shutDown() override final
	{
		app = nullptr;
		delete this;
	}

	void update(float) override final;

	void onEnter() override final;

	void onExit() override final;

	App* getOwner() override final
	{
		return app;
	}


private:
	App* app = nullptr;

	void _updateGameworldMatrix(GameworldMatrix& world);
	void _renderMaptile(Pointer<GameObject2> tile);
	void _renderGameworld();
	void _drawUI();
};



struct AppStateCityView : public State<App*>
{
	AppStateCityView(App* app) : app(app) {}
	~AppStateCityView()
	{
	}

	void shutDown() override final
	{
		app = nullptr;
		delete this;
	}

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



struct AppStateMainMenu : public State< App* >
{
	AppStateMainMenu(App* app) : app(app) {}
	~AppStateMainMenu()
	{
	}

	void shutDown() override final
	{
		app = nullptr;
		delete this;
	}

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