#include "EmitterManager.h"

void EmitterManager::Update()
{
	for (auto emitter : emitters)
	{
		emitter->Update();
	}

	// íœ
	for (auto removeEmitter : removes)
	{
		emitters.erase(std::remove(emitters.begin(), emitters.end(), removeEmitter), emitters.end());
		delete removeEmitter;
	}
}

void EmitterManager::Register(Emitter* emitter)
{
	emitters.emplace_back(emitter);
}

void EmitterManager::Remove(Emitter* emitter)
{
	removes.insert(emitter);
}

void EmitterManager::Clear()
{
	for (auto emitter : emitters)
	{
		delete emitter;
	}
	emitters.clear();
	removes.clear();
}
