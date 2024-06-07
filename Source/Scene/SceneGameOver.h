#pragma once

#include "Scene.h"
#include "../../Library/2D/Sprite.h"
#include <memory>

class SceneGameOver : public Scene
{
public:
	SceneGameOver() {}
	~SceneGameOver() {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Render() override;

private:
	std::unique_ptr<Sprite> sprite;

};