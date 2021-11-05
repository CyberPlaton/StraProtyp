#include "PlayerSystem.h"

PlayerSystem* PlayerSystem::g_PlayerSystem = nullptr;


PlayerSystem* PlayerSystem::get()
{
	if (!g_PlayerSystem)
	{
		g_PlayerSystem = new PlayerSystem();
	}

	return g_PlayerSystem;
}


void PlayerSystem::del()
{
	if (g_PlayerSystem)
	{
		delete g_PlayerSystem;
		g_PlayerSystem = nullptr;
	}
}
