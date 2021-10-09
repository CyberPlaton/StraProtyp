#pragma once

#include <string>
#include <map>
#include <vector>
#include <typeinfo>


#define DEFAULT_DECAL_SIZE_X 256
#define DEFAULT_DECAL_SIZE_Y 256
#define DEFAULT_MAPSIZE_X 75
#define DEFAULT_MAPSIZE_Y 64

static std::hash<std::string> hasher;

class Component;
using ComponentMap = std::map<std::string, std::vector<Component*>>;


/*
* The ComponentID is how the user names it, this can be anything, even a unique name for each component,
* for example for the TransformCmp of some Agent named "Ralf" we can assign the ComponentID as := "GO_98_Ralf_Transform".
*/
using ComponentID = std::string;

/*
* The ComponentType is more of a runtime  type information and describes what kind of class the component it,
* thus we can know to what we can cast it. For now it is a string, but will be enumed or hashed...
*/
using ComponentType = std::string;







/*
* General storage for all components with types.
* Usage, e.g. for iteration of all components with some type.
*/
class ComponentStorage
{
public:

	static ComponentStorage* get();

	static void del();

	/*
	* Each comonent adds itself on creation.
	*/
	void add(Component* c, const ComponentType& type);

	/*
	* Each component removes itself on destr.
	*/
	void remove(Component* c);

	/*
	* Iteratable for a set of components of same type.
	*/
	template < class T >
	std::vector<T*> getAllOfType(const ComponentType& type)
	{
		if (componentMap.contains(type))
		{
			std::vector<T*> r;

			for (auto& c : componentMap.at(type))
			{
				r.push_back(static_cast<T*>(c));
			}

			return r;
		}
	}

private:

	ComponentStorage() {}

	static ComponentStorage* g_ComponentStorageStorage;

	ComponentMap componentMap;
};




class Component
{
private:

	static unsigned long long g_ComponentID;

public:

	virtual ~Component(){}


	bool init(const ComponentType& type)
	{
		// Create a Numerical Hash Value to self.
		std::string typeName = type + "_" + std::to_string(g_ComponentID);
		globalHash = hasher(typeName);

		// Store the ID of self.
		id = g_ComponentID;

		// Create Object Local Hashvalue.
		localHash = hasher(type);


		// Store self in storage.
		ComponentStorage::get()->add(this, type);


		g_ComponentID++;

		return true;
	}


	virtual ComponentType getType() = 0;
	size_t getComponentGlobalHashValue() { return globalHash; }
	size_t getComponentLocalHashValue() { return localHash; }


	unsigned long long id;
	ComponentID name;

	// The Global hashvalue is app wide unique for the component.
	size_t globalHash;
	
	// The Local hashvalue is unique for the gameobject it is attached to.
	size_t localHash;
};





class TransformCmp : public Component
{
public:
	TransformCmp(const ComponentID& name) :xpos(0), ypos(0)
	{
		this->name = name;
		type = "Transform";

		init(type);
	}


	~TransformCmp()
	{
		xpos = 0;
		ypos = 0;
		type.clear();
	}

	void setPosition(int x, int y)
	{
		this->xpos = x;
		this->ypos = y;
	}

	ComponentType getType() override { return this->type; }

	ComponentType type;
	int xpos;
	int ypos;
};



class RendererableCmp : public Component
{
public:
	RendererableCmp(const ComponentID& name, float width, float height, const std::string& color, const std::string& decalName, const std::string& drawLayer):
		width(width), height(height), color(color), decalName(decalName), renderingLayer(drawLayer)
	{
		this->name = name;
		type = "Renderable";

		init(type);
	}

	~RendererableCmp()
	{
		width = 0.0f;
		height = 0.0f;
		color.clear();
		decalName.clear();
		renderingLayer.clear();
	}


	ComponentType getType() override { return this->type; }




	ComponentType type;

	/*
	* Specify the dimension of it.
	* Where to draw is derived from transform.
	*/
	float width = 0.0f;
	float height = 0.0f;

	std::string color = "none";
	std::string decalName = "none";
	std::string renderingLayer = "none";

	bool render = true;
};