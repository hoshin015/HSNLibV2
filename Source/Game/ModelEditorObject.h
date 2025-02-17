#pragma once
#include "AnimatedObject.h"

class ModelEditorObject : public AnimatedObject
{
public:
	ModelEditorObject(const char* filePath) : AnimatedObject(filePath) {}
	~ModelEditorObject() override {}

	void Update() override;
	void Render() override;
};