#include "IComponent.h"

size_t IComponent::g_ComponentHash = 0;

std::string EComponentTypeString(EComponentType type)
{
	switch (type)
	{
	case CT_Transform:
		return "Transform";
	
	case CT_Animator:
		return "Animator";

	case CT_Building:
		return "Building";

	case CT_City:
		return "City";

	case CT_CollisionBox:
		return "CollisionBox";

	case CT_Forest:
		return "Forest";

	case CT_Hill:
		return "Hill";

	case CT_Improvement:
		return "Improvement";

	case CT_Maptile:
		return "Maptile";

	case CT_Mountain:
		return "Mountain";

	case CT_Nation:
		return "Nation";

	case CT_Navigator:
		return "Navigator";

	case CT_Player:
		return "Player";

	case CT_Renderable:
		return "Renderable";

	case CT_Ressource:
		return "Ressource";

	case CT_River:
		return "River";

	case CT_Road:
		return "Road";

	case CT_Technology:
		return "Technology";

	case CT_Unit:
		return "Unit";


	default:
		return "Unknown";
	}
}
