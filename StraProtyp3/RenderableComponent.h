#pragma once
#include "Component2.h" // Include for extending the Component2 class
#include "GameObject2.h"
#include "IGameObjectStorage.h" // Include for GameobjectStorage Interface, mainly for Instantiation, Finding and Deleting functions
#include "Common.h" // Include Common Definitions and Datastructs.

#include "ColorConsole.h"

class GameobjectStorage;
class RenderableComponent : public Component2
{
	friend class GameobjectStorage;
public:
	RenderableComponent(const std::string& name)
	{
		initialize("Renderable", name);
	}
	~RenderableComponent()
	{
		width = 0;
		height = 0;
		render = 0;
		color.clear();
		renderingLayer.clear();
		decalName.clear();
	}


	float GetWidth() { return width; }
	float GetHeight() { return height; }
	bool IsRendering() { return render; }
	std::string GetColor() { return color; }
	std::string GetDecalName() { return decalName; }
	std::string GetRenderingLayer() { return renderingLayer; }

	float SetWidth(float w) { width = w; }
	float SetHeight(float h) { height = h; }
	bool SetIsRendering(bool v) { render = v; }
	std::string SetColor(std::string c) { color = c; }
	std::string SetDecalName(std::string d) { decalName = d; }
	std::string SetRenderingLayer(std::string l) { renderingLayer = l; }


private:

	float width = 0.0f;
	float height = 0.0f;

	std::string color = "none";
	std::string decalName = "none";
	std::string renderingLayer = "none";

	bool render = true;
};