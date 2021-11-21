#pragma once

#include "GameObjectStorage.h" // Include all Components and GameObject2 Definitions.

#include "Random.h" // For randomly choosing 3 technologies for player.

#include "ColorConsole.h" // For printing to console which technologies can or cannot be researched, their requirements etc.

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


	// For given player visualize the technology tree with all available technologies
	// and those already researched by him.
	void VisualizeTechnologyTree(Pointer< GameObject2 > player);
	

	// Let given player choose next technology to be researched.
	void ChooseNextTechnology(Pointer< GameObject2 > player, TechArea area);


	// Add a tech as researched by given player or 
	// remove an already researched tech from the players ResearchedTechnologies vector.
	// 
	// Leave "status" as true to to add newly researched tech, to remove one, set "status = false".
	void SetTechnologyResearched(Pointer< GameObject2 > player, TechID id, bool status = true);

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



private:

	bool _hasResearchedTech(TechID id, std::vector< TechID >& techs);

	
	// Check whether a technology is available for player to be researched,
	// currently we only check technological requirements.
	bool _checkWhetherAvailableForPlayer(Pointer< GameObject2> player, Pointer< GameObject2 > tech);


	float _getAccumulatedPathProbabilityWeightForTech(Pointer< GameObject2> player, Pointer< GameObject2 > tech);


};
