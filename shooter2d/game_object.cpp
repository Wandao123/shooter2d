#include "game_object.h"

using namespace Shooter;

void ObjectManager::Draw() const
{
	for (auto&& object : objectsList)
		if (object->IsEnabled())
			object->Draw();
}

void ObjectManager::Update()
{
	for (auto&& object : objectsList)
		if (object->IsEnabled())
			object->Update();
}