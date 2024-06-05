#pragma once

#include "Scene.h"
#include "SceneStage.h"
#include "../../Source/Game/Object/Player/PlayerManager.h"

#include <string>
#include <map>

class SceneTutorial : public Scene
{
private:
	/*enum EXPLAIN
	{
		FIRST = 0,
		MOVE_Z = 3,
		INPUT_MOVE = 10,
		LENGTH_STICK = 19,
		MAXLENGTH = 26,
		OBJECT_READY = 29,
	};*/

	enum EXPLAIN_STATE
	{
		READYING = 0,
		MOVE_Z,
		INPUT_MOVE,
		LENGTH_STICK,
		MAXLENGTH,
		OBJECT_READY,
		HITOBJECT_PLAYER,
		HITOBJECT_ROPE,
		END,
	};

	struct TextMessage
	{
		std::wstring text;
		bool changeState = false;
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

	bool StageCollision();

	bool StageVsRope();

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
	int nextState = 0;
	//チュートリアル中のテキスト表示を管理するステート
	int textState = 0;
	//テキストを表示する中央値
	DirectX::XMFLOAT2 textPosition = { 640, 600 };
	//テキストのサイズ
	float textSize = 40;
	//チュートリアル中にEnterキーを押すとテキストを次へ送るかどうか
	bool updateText = true;
	bool updateState = true;
	
	//プレイヤーの操作がされた時間を管理する
	float moveTimer = 0;

	std::unique_ptr<FrameBuffer> frameBuffer;
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	std::unique_ptr<Bloom> bloom;
	std::unique_ptr<Shadow> shadow;

private:
	//ロードするステージのテキストファイル名
	const std::string fileName = "Data/Stage/TutorialStage.txt";

	//表示するテキストの内容
	/*const std::wstring textMessage[30] = {
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

		L"よし、次は障害物について教えるね！",


	};*/

	const TextMessage TEXT_MESSEAGE[50]
	{
		{L"neck run の世界へようこそ！"},
		{L"このゲームの遊び方を教えるね"},
		{L"まずは、プレイヤーについて説明するよ"},
																		
		{L"始めに、プレイヤーは常に奥へ行き続けるよ",true} ,
		{L"このプレイヤーをゴールにたどり着かせることができたらゲームクリア！",},

		{L"それじゃあ、操作方法について教えるよ"},
		{L"プレイヤーが二人いるよね？"},
		{L"この二人はどちらも動かすことができるよ"} ,
		{L"青い方がWASDキー、赤い方が十字キーで動かせるんだ"},
		{L"早速動かしてみよう！",	true},
		{L"" },
		{L"完璧！"},

		{L"それじゃあ、今度はギミックについて説明するね？" },
		{L"二人の間に棒があるのが分かるかな？"},
		{L"この棒は、二人が離れることのできないようにしているんだ"} ,
		{L"二人の距離が開くほど、この棒も伸びていくよ"},
		{L"そして、棒が伸びれば伸びるほど、二人が奥に行く速度が上がるよ"},
		{L"操作が難しくなる半面、タイムの短縮が狙えるね",},
		{L"確認してみよう！",true},
		{L""} ,
		{L"分かったかな？"} ,

		{L"そうそう、この棒の長さには限界があるんだよ"}  ,
		{L"二人の距離が開きすぎると、限界を迎えちゃうんだ" }	  ,
		{L"もしそうなると、二人は強制的に真ん中に引き寄せられるんだ"}	  ,
		{L"緊急回避にも使えるけど、棒が短くなるから、タイムは悪くなるかもね"}	  ,
		{L"それじゃあ、やってみよう！",true},
		{L""},
		{L"いい感じ！"} ,

		{L"よし、次は障害物について教えるね！"},
		{L"ちょっと移動するよ",true},

		{L"前にある障害物が見えるかな？"},
		{L"二人はあれらを躱しながら、進まないといけないんだ"},
		{L"当たると・・・",true},
		{L""},

		{L"こんな風に後ろに下がって、進むのが少し遅くなるんだ"},
		{L"極力、避けて進んだ方がいいね"},

		{L"最後に、障害物が棒に当たった時の説明をするね"},
		{L"奥にある長い障害物があるのが分かるかな？"},
		{L"あの長さだと、棒に当たっちゃうんだ"},
		{L"もし棒に当たっちゃうと、ゲームオーバーになるんだ"},
		{L"なにがあっても絶対に棒に当てたらないようにしないと駄目だね"},
		{L"ただ、今回はわざと当ててみようか",true},

		{L"チュートリアルは終わりだよ"},
		{L"やってくれてありがとう！",true},
	};


};