#pragma once
// --- Scene ---
#include "Scene.h"
#include "SceneTitle.h"
#include "SceneTest.h"
#include "SceneModelEditor.h"
#include "SceneSpriteEditor.h"
#include "ScenePlayer.h"
#include "SceneStage.h"
#include "SceneGame1.h"
#include "SceneGame2.h"
#include "SceneGame3.h"
#include "SceneTutorial.h"

// シーンマネージャー
class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}

public:
	// 唯一のインスタンス取得
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}

	// 更新処理
	void Update();

	// 描画処理
	void Render();

	// シーンクリア
	void Clear();

	// シーン切り替え
	void ChangeScene(Scene* scene);

private:
	Scene* currentScene = nullptr;
	Scene* nextScene = nullptr;
};