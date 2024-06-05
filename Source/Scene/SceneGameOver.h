#pragma once

#include "Scene.h"
#include "../../Library/2D/Sprite.h"
#include <memory>

class SceneGameOver : public Scene
{
public:
	SceneGameOver() {}
	~SceneGameOver() {}

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