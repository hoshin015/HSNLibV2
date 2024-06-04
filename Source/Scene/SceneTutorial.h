#pragma once

#include "Scene.h"
#include "SceneStage.h"
#include "../../Source/Game/Object/Player/PlayerManager.h"

#include <string>

class SceneTutorial : public Scene
{
private:
	enum EXPLAIN
	{
		FIRST = 0,
		MOVE_Z = 3,
		INPUT_MOVE = 10,
		LENGTH_STICK = 19,
		MAXLENGTH = 26,
	};

public:
	SceneTutorial() {}
	~SceneTutorial() {}

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

	void StageCollision();

	void StageVsRope();

	//チュートリアル中のアップデート
	void UpdateTutorial();

	//チュートリアル中のテキストの描画
	void DrawTutorialText();

private:
	//カメラを設定するための数値
	DirectX::XMFLOAT3 cameraOffset = { 0,350,400 };
	float cameraAngle = 45.0f;

	std::unordered_map<int, std::unique_ptr<Object3D>>  objects;
	std::vector<Object3D::Transform> transforms;

	std::vector<Object3D::Collision> collisions;


	//コントローラーの振動のタイマー
	float controllerTimer = 0.0f;

	//チュートリアルの状況を管理するステート
	int tutorialState = 0;
	//チュートリアル中のテキスト表示を管理するステート
	int textState = 0;
	//テキストを表示する中央値
	DirectX::XMFLOAT2 textPosition = { 640, 600 };
	//テキストのサイズ
	float textSize = 40;
	//チュートリアル中にEnterキーを押すとテキストを次へ送るかどうか
	bool updateText = true;
	
	//プレイヤーの操作がされた時間を管理する
	float moveTimer = 0;

private:
	//表示するテキストの内容
	const std::wstring textMessage[30] = {
		L"neck run の世界へようこそ！",
		L"このゲームの遊び方を教えるね",
		L"まずは、プレイヤーについて説明するよ",

		L"始めに、プレイヤーは常に奥へ行き続けるよ",
		L"このプレイヤーをゴールにたどり着かせることができたらゲームクリア！",

		L"それじゃあ、操作方法について教えるよ",
		L"プレイヤーが二人いるよね？",
		L"この二人はどちらも動かすことができるよ",
		L"青い方がWASDキー、赤い方が十字キーで動かせるんだ",
		L"早速動かしてみよう！",
		L"",
		L"完璧！",

		L"それじゃあ、今度はギミックについて説明するね？",
		L"二人の間に棒があるのが分かるかな？",
		L"この棒は、二人が離れることのできないようにしているんだ",
		L"二人の距離が開くほど、この棒も伸びていくよ",
		L"そして、棒が伸びれば伸びるほど、二人が奥に行く速度が上がるよ",
		L"操作が難しくなる半面、タイムの短縮が狙えるね",
		L"確認してみよう！",
		L"",
		L"分かったかな？",

		L"そうそう、この棒の長さには限界があるんだよ",
		L"二人の距離が開きすぎると、限界を迎えちゃうんだ",
		L"もしそうなると、二人は強制的に真ん中に引き寄せられるんだ",
		L"緊急回避にも使えるけど、棒が短くなるから、タイムは悪くなるかもね",
		L"それじゃあ、やってみよう！",
		L"",
		L"いい感じ！",

	};


};