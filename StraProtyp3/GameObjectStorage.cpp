#include "GameObjectStorage.h"

IGameobjectStorage* GameobjectStorage::get()
{
	if (!IGameobjectStorage::g_IGameobjectStorage)
	{
		IGameobjectStorage::g_IGameobjectStorage = new GameobjectStorage();
	}

	return IGameobjectStorage::g_IGameobjectStorage;
}


void GameobjectStorage::del()
{
	using namespace std;

	if (IGameobjectStorage::g_IGameobjectStorage)
	{
		for (auto& object : IGameobjectStorage::g_IGameobjectStorage->GetStorage())
		{
			if (object.use_count() > 1)
			{
				cout << color(colors::RED);
				cout << "[GameobjectStorage::del] Gameobject has more than one Uses! Current count: " << object.use_count() << white << endl;
			}


			// Free the Gameobjects memory
			object.reset();
		}

		// Clear the Storage vector
		IGameobjectStorage::g_IGameobjectStorage->GetStorage().clear();


		delete IGameobjectStorage::g_IGameobjectStorage;
		IGameobjectStorage::g_IGameobjectStorage = nullptr;
	}
}
