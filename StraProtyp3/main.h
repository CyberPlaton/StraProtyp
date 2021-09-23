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


#include "imnodes.h"
struct ImNodesLink
{
	ImNodesLink(int id, int start, int end) : id(id), start(start), end(end) {};
	int id, start, end;
};


struct ImNodesNode
{
	ImNodesNode(const std::string& name, int id) :name(name), id(id) {};

	std::string name;
	int id;
	std::map< std::string, int > dependencies; // Which are not technologies.
};

/*
*/
#include "ComponentSystem.h"
#include "Components.h"
#include "GameObject.h"
#include "GameObjectCreator.h"
#include "GameWorld.h"
#include "GameWorldTime.h"

#include "TechInstance.h"




class App : public olc::PixelGameEngine
{
public:
	App() : pge_imgui(false)
	{
		sAppName = "APP";
	}
public:

	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
	void DrawUI(void);

private:

	olc::TileTransformedView tv;
	olc::imgui::PGE_ImGUI pge_imgui;
	int m_GameLayer;


	std::map< std::string, olc::Decal* > decalDatabase;


	// A vector of all Technologies in game.
	// Currently for debug/display usage.
	std::vector< TechInstance* > techTree;
	std::vector< ImNodesNode > techTreeNodes;
	std::vector< ImNodesLink > links;
private:

	void _onImGui();
	void _handleInput();


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



	bool _loadDecalDatabase();
	bool _loadTechTreeDefinitions();

	/*
	* The names for rendering layers are defined for each renderable component in XML.
	*/
	void renderLayer(const std::string& layerName);
};