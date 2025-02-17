#pragma once
#include <memory>
#include "Scene.h"
#include "../../Library/2D/Sprite.h"
#include "../../Library/Graphics/FrameBuffer.h"
#include "../Game/TestStatic.h"


// テストシーン
class SceneTest : public Scene
{
public:
	SceneTest() {}
	~SceneTest() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Render() override;

private:
	// デバッグ描画
	void DrawDebugGUI();
private:
	std::unique_ptr<TestStatic> testStatic;
};