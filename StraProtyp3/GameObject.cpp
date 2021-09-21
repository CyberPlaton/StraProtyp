#include "GameObject.h"

GameObjectStorage* GameObjectStorage::g_GameObjectStorage = nullptr;
unsigned long long GameObject::g_GameObjectCount = 0;


GameObjectStorage* GameObjectStorage::get()
{
	if (!g_GameObjectStorage)
	{
		g_GameObjectStorage = new GameObjectStorage();
	}

	return g_GameObjectStorage;
}

void GameObjectStorage::del()
{
	if (g_GameObjectStorage)
	{
		g_GameObjectStorage->m_GameObjects.clear();
		g_GameObjectStorage->m_GameObjectsTagOptimized.~AVLTree();
		delete g_GameObjectStorage;
	}
}


void GameObjectStorage::remove(const GOTag& tag)
{
	size_t tag_hash = hasher(tag);
	for (int i = 0; i < m_GameObjects.size(); i++)
	{
		if (m_GameObjects[i]->hash == tag_hash) m_GameObjects.erase(m_GameObjects.begin() + i);
	}

	m_GameObjectsTagOptimized.remove(tag_hash);
}


void GameObjectStorage::remove(GameObject* go)
{
	remove(go->getTag());
}

std::vector< GameObject* >& GameObjectStorage::getStorage()
{
	return g_GameObjectStorage->m_GameObjects;
}

void GameObjectStorage::add(GameObject* go)
{
	m_GameObjects.push_back(go);
	m_GameObjectsTagOptimized.insert(go->hash, go);
}


GameObject* GameObjectStorage::getGOByTag(const GOTag& tag)
{
	size_t tag_hash = hasher(tag);
	GameObject* go = m_GameObjectsTagOptimized.search(tag_hash);
	return go;
}


GameObject* GameObjectStorage::findAnyGOByTag(const GOTag& tag)
{
	for (auto& go : m_GameObjects)
	{
		if (go->getTag().find(tag) != std::string::npos)
		{
			return go;
		}
	}

	return nullptr;
}



GameObject* GameObjectStorage::findAnyGOByName(const GOName& name)
{
	for (auto& go : m_GameObjects)
	{
		if (go->getName().find(name) != std::string::npos)
		{
			return go;
		}
	}

	return nullptr;
}


GameObject* GameObjectStorage::getGOByName(const GOName& name)
{
	for (auto& go : m_GameObjects)
	{
		if (go->getName().compare(name) == 0)
		{
			return go;
		}
	}

	return nullptr;
}