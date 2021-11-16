#pragma once

#include "GameObjectStorage.h" // Include all Components and GameObject2 Definitions.


/*
* ImGui includes for Technology Tree,
* so we can Build the UI.
*/
#include"common/include/GL/glew.h"
#pragma comment(lib, "glew32.lib")

#include "common/include/imgui_impl_pge.h"
#include "imnodes.h"


class TechnologySystem
{
	// Tech Tree visualization for a line between 2 nodes.
	struct Link
	{
		Link(int id, int start, int end)
		: id(id), start(start), end(end) {};

		int id, start, end;
	};


	// Tech Tree visualization for a technology as a node.
	struct Node
	{
		Node(std::string name, int id, std::string area, std::string category) 
		: name(name), id(id), area(area), subCategory(category) {};


		std::string name;
		std::string area;
		std::string subCategory;
		int id;
		std::map< std::string, int > dependencies; // Which are not technologies.
		std::map< std::string, int > techDependencies; // Which are for display only.
	};



public:
	static TechnologySystem* get();
	static void del();

	bool Initialize(std::string filepath);

	void VisualizeTechnologyTree(Pointer< GameObject2 > player);

private:
	static TechnologySystem* g_TechnologySystem;

	// All in game technologies.
	std::vector< Pointer< GameObject2 > > m_Technologies;
	
	std::vector< Node* > m_TechNodes;
	std::vector< Link* > m_TechLinks;
	bool m_TechTreeInitialized = false;
	size_t m_TechNodeID = 0;

private:
	TechnologySystem(){}
};
