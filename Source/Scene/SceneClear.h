#pragma once

#include "Scene.h"
#include "../../Library/2D/Sprite.h"
#include <memory>

class SceneClear : public Scene
{
public:
    SceneClear(){}
    ~SceneClear(){}

	// ‰Šú‰»
	void Initialize() override;

	// I—¹‰»
	void Finalize() override;

	// XV
	void Update() override;

	// •`‰æ
	void Render() override;

private:
    std::unique_ptr<Sprite> sprite;

};