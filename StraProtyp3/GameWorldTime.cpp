#include "GameWorldTime.h"

GameWorldTime* GameWorldTime::g_GameWorldTime = nullptr;


GameWorldTime* GameWorldTime::get()
{
	if (!g_GameWorldTime)
	{
		g_GameWorldTime = new GameWorldTime();
	}

	return g_GameWorldTime;
}

void GameWorldTime::del()
{
	if (g_GameWorldTime)
	{
		g_GameWorldTime->timeSpeed = 0.0;

		g_GameWorldTime->internal_minute_counter = 0.0;

		g_GameWorldTime->minutes = 0;
		g_GameWorldTime->hours = 0;

		g_GameWorldTime->day = 0;
		g_GameWorldTime->week = 0;
		g_GameWorldTime->month = 0;
		g_GameWorldTime->year = 0;

		delete g_GameWorldTime;
	}
}