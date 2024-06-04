#include <tchar.h>
#include <DirectXMath.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
// --- external ---
#include "../../External/ImGui/imgui.h"
// --- libarary ---
#include "../../Library/Framework.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Math/OperatorXMFloat3.h"
#include "../../Library/Timer.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/Effekseer/EffectManager.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/3D/LightManager.h"
#include "../../Library/Particle/Particle.h"
#include "../../Library/ErrorLogger.h"
#include "../../Library/Text/DispString.h"
// --- Scene ---
#include "SceneTutorial.h"
#include "SceneManager.h"
// --- Game ---
#include "../Game/Object/Stage/StageManager.h"
#include "../Game/Object/Stage/StageMain.h"
#include "../../Library/3D/DebugPrimitive.h"

#include "../Game/Object/Player/PlayerManager.h"
void SceneTutorial::Initialize()
{
	// カメラ初期設定
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 20, 20),		// カメラ座標
		DirectX::XMFLOAT3(0, 0, 0),			// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(cameraAngle), DirectX::XMConvertToRadians(180), 0 });
	Camera::Instance().cameraType = Camera::CAMERA::TARGET_PLAYER;

#if 1
	// ライト初期設定
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({ 0.2f, 0.2f, 0.2f, 1.0f });
#else
	// 点光源追加
	Light* light = new Light(LightType::Point);
	light->SetPosition({ 5, 5, 5 });
	light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	light->SetRange(30.0f);
	LightManager::Instance().Register(light);

	LightManager::Instance().SetAmbientColor({ 0.1f,0.1f,0.1f,1.0f });
#endif
	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain("Data/Fbx/stage/stage.model");
	stageManager.Register(stageMain);

	//プレイヤー初期化
	PlayerManager& playerManager = PlayerManager::Instance();
	PlayerManager::Instance().Initialize();
	Player* player1 = new Player("Data/Fbx/Player_02/Player_02.model", false);
	player1->SetPos({ 0,0,0 });
	playerManager.Register(player1);

	Player* player2 = new Player("Data/Fbx/Player_02/Player_02.model", true);
	player2->SetPos({ 50.0f,0,0 });
	playerManager.Register(player2);

	//ステージをロードする
	objects.insert(std::make_pair(eObjectType::Kesigomu, std::make_unique<Object3D>("Data/Fbx/Kesigomu/Kesigomu.fbx", eObjectType::Kesigomu)));
	objects.insert(std::make_pair(eObjectType::Pentate, std::make_unique<Object3D>("Data/Fbx/Pentate/Pentate.fbx", eObjectType::Pentate)));
	objects.insert(std::make_pair(eObjectType::Enpitu, std::make_unique<Object3D>("Data/Fbx/Enpitu/Enpitu.fbx", eObjectType::Enpitu)));
	objects.insert(std::make_pair(eObjectType::Tokei, std::make_unique<Object3D>("Data/Fbx/Tokei/Tokei.fbx", eObjectType::Tokei)));
}

void SceneTutorial::Finalize()
{
	LightManager::Instance().Clear();

	StageManager::Instance().Clear();

	PlayerManager::Instance().Clear();
}

void SceneTutorial::Update()
{

#if USE_IMGUI
	// --- imguiManager処理 ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager処理 ---
	InputManager::Instance().Update();

	// --- effectManager処理 ---
	EffectManager::Instance().Update();

	// --- カメラ処理 ---
	DirectX::XMFLOAT3 cameraTarget = PlayerManager::Instance().GetPositionCenter();
	cameraTarget += cameraOffset;
	Camera::Instance().SetTarget(cameraTarget);
	Camera::Instance().Update();


	// タイマーの定数バッファの更新
	UpdateTimerConstnat();

	UpdateTutorial();

	StageManager::Instance().Update();

	PlayerManager::Instance().Update();



	StageCollision();
	StageVsRope();

	//objects->Update();
	for (auto& object : objects) {
		object.second->Update();
	}

	// objectsのtranform情報をコピー
	/*transforms.clear();
	for (auto& object : objects) {
		for (auto& transform : object.second->transforms) {
			transforms.emplace_back(transform);
		}
	}*/

	collisions.clear();
	for (auto& object : objects) {
		for (auto& c : object.second->collisions) {
			collisions.emplace_back(c);
		}
	}
}

void SceneTutorial::Render()
{
	// 必要なポインタ取得
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// renderTargetの設定
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// imGuiBufferを使用しない場合はこっちを記述する
	{
		// renderTargetのクリア
		dc->ClearRenderTargetView(gfx->GetRTV(), gfx->GetBgColor());
		// depthStencilViewのクリア
		dc->ClearDepthStencilView(gfx->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	// カメラの定数バッファの更新
	Camera::Instance().UpdateCameraConstant();
	// ライトの定数バッファの更新
	LightManager::Instance().UpdateConstants();

	// rasterizerStateの設定
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilStateの設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendStateの設定
	gfx->SetBlend(BLEND_STATE::ALPHA);

	StageManager::Instance().Render();

	//プレイヤーの描画
	PlayerManager::Instance().Render();
	//ステージ描画
	for (auto& object : objects) {
		object.second->Render();
	}

	//線の描画
	LineRenderer::Instance().Render();
	DebugPrimitive::Instance().Render();

	// rasterizerStateの設定
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
	// depthStencilStateの設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_OFF);
	// blendStateの設定
	gfx->SetBlend(BLEND_STATE::ALPHA);

	DrawTutorialText();

#if USE_IMGUI
	// --- デバッグGUI描画 ---
	DrawDebugGUI();

	LightManager::Instance().DrawDebugGui();

#if SHOW_PERFORMANCE
	// --- パフォーマンス描画 ---
	ImGuiManager::Instance().DisplayPerformanceStats();
#endif

	// --- imguiManager 描画 ---
	ImGuiManager::Instance().Render();
#endif
}

void SceneTutorial::DrawDebugGUI()
{
	PlayerManager::Instance().DrawDebugImGui();

	ImGui::Begin("Camera");

	ImGui::SliderFloat3("CameraOffset", &cameraOffset.x, 0, 500);
	ImGui::SliderInt("textState", &textState, 0, 30);
	ImGui::End();
}

void SceneTutorial::StageCollision()
{
	std::vector<Player*> players = PlayerManager::Instance().GetPlayer();

	for (Player* player : players)
	{
		for (const Object3D::Collision collision : collisions)
		{
			DirectX::XMFLOAT3 outPos;
			//当たり判定
			if (Collision::IntersectSphereVsSphere(player->GetPos(), player->GetRadius(), collision.pos, collision.radius, outPos))
			{
				//ヒット時の処理
				player->HitModel(outPos, PlayerManager::Instance().GetHitPower(), PlayerManager::Instance().GetHitDownSpeed());

				//コントローラーを振動させる
				if (InputManager::Instance().IsGamePadConnected())
				{
					InputManager::Instance().SetVibration(0, 0.25f, 0.25f);
					//振動させる時間を設定
					controllerTimer = 1.0f;
				}
			}
			else
			{
				//コントローラーの振動を止める
				if (InputManager::Instance().IsGamePadConnected() && controllerTimer <= 0)
					InputManager::Instance().SetVibration(0, 0.0f, 0.0f);
			}
		}
	}

	//コントローラーの振動のタイマーを減らす
	if (controllerTimer > 0)
		controllerTimer -= Timer::Instance().DeltaTime();
}

void SceneTutorial::StageVsRope()
{
	//プレイヤーを取得
	std::vector<Player*> players = PlayerManager::Instance().GetPlayer();

	DirectX::XMFLOAT3 rayPos[2] = { {0,0,0},{0,0,0} };
	int i = 0;
	for (Player* player : players)
	{
		//プレイヤーの首辺りの座標を取る
		rayPos[i] = player->GetPos();
		rayPos[i].y += PlayerManager::Instance().GetRopeHeight();
		i++;
	}


	for (Object3D::Collision collision : collisions)
	{
		//紐の場所よりそのコリジョンの高さが小さければcontinue
		if (collision.pos.y + collision.radius < rayPos[0].y && collision.pos.y + collision.radius < rayPos[1].y)
			continue;
		DirectX::XMFLOAT3 outPos;
		//当たり判定
		if (PlayerManager::Instance().IntersectSphereVsLine(collision.pos, collision.radius, rayPos[0], rayPos[1]))
		{
			for (Player* player : PlayerManager::Instance().GetPlayer())
			{
				//死亡処理
				player->SetDeath();
			}
			SceneManager::Instance().ChangeScene(new SceneTitle);
		}
	}

}

void SceneTutorial::UpdateTutorial()
{
	PlayerManager& players = PlayerManager::Instance();

	//テキストを更新する
	if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter) && updateText)
		textState++;

	switch (tutorialState)
	{
	case 0:
		//始めの説明
	{
		//プレイヤーが入力を受け入れないようにする
		players.SetInputPlayerMove(false);
		//プレイヤーがZ方向へ勝手に動かないようにし、速度も変化しないようにする
		players.SetIsMoveZ(false);
		players.SetIsUpdateZ(false);

		if (textState >= EXPLAIN::MOVE_Z)
			tutorialState++;
		break;
	}

	case 1:
		//プレイヤーが奥に行き続けることの説明をする
	{
		players.SetIsMoveZ(true);
		players.SetIsUpdateZ(true);

		if (textState >= EXPLAIN::INPUT_MOVE)
			tutorialState++;

		break;
	}

	case 2:
		//プレイヤーの操作について説明する
	{
		//プレイヤーが入力を受け入れるように変更
		players.SetInputPlayerMove(true);
		updateText = false;

		//動いた時間を計測
		if (players.GetDoMove())
		{
			moveTimer += Timer::Instance().DeltaTime();
		}

		//規定時間を過ぎたら
		if (moveTimer >= 3.0f)
		{
			//テキストを進める
			textState++;
			updateText = true;
			//タイマーを０にする
			moveTimer = 0;
			tutorialState++;
		}

		break;
	}

	case 3:
	//移動説明終了時の処理
	{
		//プレイヤーが入力を受け入れないようにする
		players.SetInputPlayerMove(false);
		//プレイヤーがZ方向へ勝手に動かないようにし、速度も変化しないようにする
		players.SetIsMoveZ(false);
		players.SetIsUpdateZ(false);

		if (textState >= LENGTH_STICK)
			tutorialState++;
		break;
	}

	case 4:
	//棒の長さによってプレイヤーの速度が変化することを説明する
	{
		//プレイヤーが入力を受け入れるように変更
		players.SetInputPlayerMove(true);
		players.SetIsMoveZ(true);
		players.SetIsUpdateZ(true);
		//テキストの更新を止める
		updateText = false;

		if (players.GetAccelerationZ() > 11.0f)
		{
			//テキストを進める
			textState++;
			updateText = true;
			//次のステートへ進む
			tutorialState++;
		}

		break;
	}

	case 5:
	//プレイヤー速度変化の説明終了処理
	{
		//プレイヤーが入力を受け入れないようにする
		players.SetInputPlayerMove(false);
		//プレイヤーがZ方向へ勝手に動かないようにし、速度も変化しないようにする
		players.SetIsMoveZ(false);
		players.SetIsUpdateZ(false);

		if (textState >= EXPLAIN::MAXLENGTH)
			tutorialState++;
		break;
	}

	case 6:
	//棒の長さが最大値を超えた場合の説明
	{
		//プレイヤーが入力を受け入れるように変更
		players.SetInputPlayerMove(true);
		players.SetIsMoveZ(true);
		players.SetIsUpdateZ(true);
		//テキストの更新を止める
		updateText = false;

		if (players.IsRopeOverLength())
		{
			//テキストを進める
			textState++;
			updateText = true;
			//次のステートへ進む
			tutorialState++;
		}

		break;
	}

	case 7:
	//棒の最大値を超えた時の説明終了処理
	{
		//プレイヤーが入力を受け入れないようにする
		players.SetInputPlayerMove(false);
		//プレイヤーがZ方向へ勝手に動かないようにし、速度も変化しないようにする
		players.SetIsMoveZ(false);
		players.SetIsUpdateZ(false);

		break;
	}
	}
}

void SceneTutorial::DrawTutorialText()
{
	DispString& text = DispString::Instance();
	
	constexpr DirectX::XMFLOAT2 ANOTHER_TEXTPOS = { 640,100 };
	constexpr float ANOTHER_TEXTSIZE = 50;

	if(updateText)
		text.Draw(textMessage[textState].c_str(), textPosition, textSize, TEXT_ALIGN::MIDDLE);

	//個別で説明を出す
	if (textState == EXPLAIN::INPUT_MOVE)
	{
		text.Draw(L"青：WASD、赤：十字キー", ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
	if (textState == EXPLAIN::LENGTH_STICK)
	{
		text.Draw(L"棒を伸ばして、速度が上がることを確認する", ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
	if (textState == EXPLAIN::MAXLENGTH)
	{
		text.Draw(L"棒の長さの限界を超えてみる", ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
	/*switch (textState)
	{
	case 0:
		break;

	case 1:
		text.Draw(L"このゲームについて説明するよ！", textPosition, textSize, TEXT_ALIGN::MIDDLE);
		break;

	case 2:
		text.Draw(L"まずは、プレイヤーについて説明するよ" , textPosition, textSize, TEXT_ALIGN::MIDDLE);
		break
	
	;case 3:
		text.Draw(L"まずは、プレイヤーについて説明するね" , textPosition, textSize, TEXT_ALIGN::MIDDLE);
		break;
	
	}*/
}
