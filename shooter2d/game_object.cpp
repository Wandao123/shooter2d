#include <typeinfo>
#include "game_object.h"

using namespace Shooter;

void ObjectManager::Update()
{
	for (auto&& object : objectsList)
		if (object->IsEnabled())
			object->Update();
}

void ObjectManager::Draw()
{
	for (auto&& object : objectsList)
		if (object->IsEnabled())
			object->Draw();
}