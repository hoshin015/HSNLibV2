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
	stageMain->SetScale(stageScale);

	bitBlockTransfer = std::make_unique<FullScreenQuad>();
	frameBuffer = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF(), true);
	bloom = std::make_unique<Bloom>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF());
	shadow = std::make_unique<Shadow>();


	waterBase = std::make_unique<TestAnimated>("Data/Fbx/WaterBase/WaterBase.model");
	waterBase->SetPosY(-100);

	//プレイヤー初期化
	PlayerManager& playerManager = PlayerManager::Instance();
	playerManager.Initialize();
	/*Player* player1 = new Player("Data/Fbx/Player_02/Player_02.model", false);
	player1->SetPos({ 0,0,0 });
	playerManager.Register(player1);

	Player* player2 = new Player("Data/Fbx/Player_02/Player_02.model", true);
	player2->SetPos({ 50.0f,0,0 });
	playerManager.Register(player2);*/

	//プリミティブの初期化
	primitive = std::make_unique<Primitive2D>();

	//ステージをロードする
	//存在するオブジェクトを定義する
	objects.insert(std::make_pair(eObjectType::Kesigomu, std::make_unique<Object3D>("Data/Fbx/Kesigomu/Kesigomu.model", eObjectType::Kesigomu)));
	objects.insert(std::make_pair(eObjectType::Pentate, std::make_unique<Object3D>("Data/Fbx/Pentate/Pentate.model", eObjectType::Pentate)));
	objects.insert(std::make_pair(eObjectType::Enpitu, std::make_unique<Object3D>("Data/Fbx/Enpitu/Enpitu.model", eObjectType::Enpitu)));
	objects.insert(std::make_pair(eObjectType::Tokei, std::make_unique<Object3D>("Data/Fbx/Tokei/Tokei.model", eObjectType::Tokei)));
	objects.insert(std::make_pair(eObjectType::Kikyapu, std::make_unique<Object3D>("Data/Fbx/Kikyapu/Kikyapu.model", eObjectType::Kikyapu)));
	objects.insert(std::make_pair(eObjectType::Kuripu, std::make_unique<Object3D>("Data/Fbx/Kuripu/Kuripu.model", eObjectType::Kuripu)));
	objects.insert(std::make_pair(eObjectType::Sunatokei, std::make_unique<Object3D>("Data/Fbx/Sunatokei/Sunatokei.model", eObjectType::Sunatokei)));

	//要素の初期化
	tutorialState = 0;
	nextState = 0;
	textState = 0;
}

void SceneTutorial::Finalize()
{
	LightManager::Instance().Clear();

	StageManager::Instance().Clear();

	PlayerManager::Instance().Clear();

	//振動を止める
	if (InputManager::Instance().IsGamePadConnected())
		InputManager::Instance().SetVibration(0, 0.0f, 0.0f);
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

	//チュートリアルのアップデート
	UpdateTutorial();

	StageManager::Instance().Update();

	PlayerManager::Instance().Update();

	StageCollision();
	StageVsRope();

	//objects->Update();
	for (auto& object : objects) {
		object.second->Update();
	}

	collisions.clear();
	for (auto& object : objects) {
		for (auto& c : object.second->collisions) {
			collisions.emplace_back(c);
		}
	}

	waterBase->Update();
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

	// shadowMap
	{
		shadow->Clear();					// シャドウマップクリア
		shadow->UpdateShadowCasterBegin();	// シャドウマップ描画準備

		for (int i = 0; i < SHADOWMAP_COUNT; i++)
		{
			shadow->Activate(i);
			// 影を付けたいモデルはここで描画を行う(Render の引数に true をいれる)
			{
				// animated object
				shadow->SetAnimatedShader();
				StageManager::Instance().Render(true);
				PlayerManager::Instance().Render(true);

				// static object
				shadow->SetStaticShader();
				
				for (auto& object : objects) {
					object.second->Render(true);
				}


			}
			shadow->DeActivate();
		}

		// 通常描画用にテクスチャと定数バッファ更新
		shadow->SetShadowTextureAndConstants();
	}

	// rasterizerStateの設定
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilStateの設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendStateの設定
	gfx->SetBlend(BLEND_STATE::ALPHA);

	// 通常描画
	frameBuffer->Clear(gfx->GetBgColor());
	frameBuffer->Activate();
	{
		StageManager::Instance().Render();

		PlayerManager::Instance().Render();

		//testStatic->Render();
		//testAnimated->Render();

		//objects->Render();
		for (auto& object : objects) {
			object.second->Render();
		}

		LineRenderer::Instance().Render();
		DebugPrimitive::Instance().Render();

		// rasterizerStateの設定
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
		// depthStencilStateの設定
		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_OFF);
		// blendStateの設定
		gfx->SetBlend(BLEND_STATE::ALPHA);

		//Particle::Instance().Render();

		// rasterizerStateの設定
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
		// depthStencilStateの設定
		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_OFF);
		// blendStateの設定
		gfx->SetBlend(BLEND_STATE::ALPHA);

		waterBase->Render();
	}
	frameBuffer->DeActivate();

#if 0
	// ブルーム処理しての描画
	bloom->Make(frameBuffer->shaderResourceViews[0].Get());
	bitBlockTransfer->blit(bloom->GetSrvAddress(), 0, 1, nullptr, nullptr);
#else
	// そのまま描画
	bitBlockTransfer->blit(frameBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1, nullptr, nullptr);
#endif	

	// ブルームなし
	//sprTest->Render();
	//sprTest2->Render();
	//sprTest3->Render();

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

	DrawMenuBar();
}

bool SceneTutorial::StageCollision()
{
	std::vector<Player*> players = PlayerManager::Instance().GetPlayer();

	bool hit = false;

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
				hit = true;
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

	return hit;
}

bool SceneTutorial::StageVsRope()
{
	//プレイヤーを取得
	std::vector<Player*> players = PlayerManager::Instance().GetPlayer();
	bool hit = false;
	
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

			hit = true;
		}
	}

	return hit;
}

void SceneTutorial::UpdateTutorial()
{
	PlayerManager& players = PlayerManager::Instance();
	InputManager& input = InputManager::Instance();

	//テキストを更新する
	
	if(input.IsGamePadConnected())
	{
		if (input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a) && updateText)
		{
			updateState = true;
			textState++;
		}
	}
	else
	{
		if (input.GetKeyPressed(DirectX::Keyboard::Enter) && updateText)
		{
			updateState = true;
			textState++;
		}
	}

	switch (tutorialState)
	{
	case READYING:
		//始めの説明
	{
		//プレイヤーが入力を受け入れないようにする
		players.SetInputPlayerMove(false);
		//プレイヤーがZ方向へ勝手に動かないようにし、速度も変化しないようにする
		players.SetIsMoveZ(false);
		players.SetIsUpdateZ(false);

		if (TEXT_MESSEAGE[(textState > 0) ? textState - 1 : textState].changeState && updateState)
		{
			nextState++;
			tutorialState = nextState;
		}
		break;
	}

	case MOVE_Z:
		//プレイヤーが奥に行き続けることの説明をする
	{
		players.SetIsMoveZ(true);
		players.SetIsUpdateZ(true);

		if (TEXT_MESSEAGE[textState].changeState)
		{
			//nextState++;
			//tutorialState = nextState;
			tutorialState = EXPLAIN_STATE::READYING;
			updateState = false;
		}

		break;
	}

	case INPUT_MOVE:
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
		if (moveTimer >= 2.5f)
		{
			//テキストを進める
			textState++;
			updateText = true;
			//タイマーを０にする
			moveTimer = 0;
			tutorialState = EXPLAIN_STATE::READYING;
			//nextState++;
		}

		break;
	}

	case LENGTH_STICK:
		//棒の長さによってプレイヤーの速度が変化することを説明する
	{
		//プレイヤーが入力を受け入れるように変更
		players.SetInputPlayerMove(true);
		players.SetIsMoveZ(true);
		players.SetIsUpdateZ(true);
		//テキストの更新を止める
		updateText = false;

		if (players.GetAccelerationZ() > 23.0f)
		{
			//テキストを進める
			textState++;
			updateText = true;
			//次のステートへ進む
			tutorialState = EXPLAIN_STATE::READYING;
			//nextState++;
		}

		break;
	}

	case MAXLENGTH:
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
			tutorialState = EXPLAIN_STATE::READYING;
			//nextState++;
		}

		break;
	}

	case OBJECT_READY:
		//障害物の説明をするための準備
	{
		//プレイヤーの座標を初期値に移動する
		players.ResetToInterval();

		//障害物を説明するためのステージをロードする
		std::ifstream file(fileName);

		if (file)
		{
			std::string str;

			for (auto& object : objects) {
				//object.second->transforms.clear();
				object.second->Clear();
			}

			while (std::getline(file, str)) {
				int objectType = 0;
				float _x = 0.0f, _y = 0.0f, _z = 0.0f;
				std::stringstream ss(str);
				ss >> objectType >> _x >> _y >> _z;

				objects.at(static_cast<eObjectType>(objectType))->Add(DirectX::XMFLOAT3{ _x, _y, _z });
			}
		}
		else
		{
			assert("StageFile Not Found");
		}
		tutorialState = EXPLAIN_STATE::READYING;
		updateState = false;

		break;
	}

	case HITOBJECT_PLAYER:
	//オブジェクトに当たった時の処理
	{
		players.SetIsMoveZ(true);
		players.SetIsUpdateZ(true);
		//テキストの更新を止める
		updateText = false;

		if (StageCollision())
		{
			//テキストを進める
			textState++;
			updateText = true;
			//次のステートへ進む
			tutorialState = EXPLAIN_STATE::READYING;
		}

		break;
	}

	case HITOBJECT_ROPE:
		players.SetInputPlayerMove(true);
		players.SetIsMoveZ(true);
		players.SetIsUpdateZ(true);
		updateText = false;

		if (PlayerManager::Instance().GetPlayer().at(0)->GetPosZ() < -800 && 
			PlayerManager::Instance().GetPlayer().at(1)->GetPosZ() < -800)
		{
			PlayerManager::Instance().GetPlayer().at(0)->SetPosZ(0.0f);
			PlayerManager::Instance().GetPlayer().at(1)->SetPosZ(0.0f);
		}

		StageCollision();

		if (StageVsRope())
		{
			//テキストを進める
			textState++;
			updateText = true;
			//次のステートへ進む
			tutorialState = EXPLAIN_STATE::READYING;
		}

		break;

	case END:
		SceneManager::Instance().ChangeScene(new SceneTitle);

		break;
	}
}

void SceneTutorial::DrawTutorialText()
{
	DispString& text = DispString::Instance();
	//テキストを表示する中央値
	DirectX::XMFLOAT2 TEXTPOSITION = { 640, 660 };
	constexpr DirectX::XMFLOAT2 TEXT_BOX_POS = { 0,600 };
	constexpr DirectX::XMFLOAT2 TEXT_BOX_SIZE = { 1280,720 };
	
	constexpr DirectX::XMFLOAT2 ANOTHER_TEXTPOS = { 640,60 };
	constexpr DirectX::XMFLOAT2 ANOTHER_BOX_POS = { 0,0 };
	constexpr DirectX::XMFLOAT2 ANOTHER_BOX_SIZE = { 1280,100 };
	constexpr float ANOTHER_TEXTSIZE = 50;

	if (updateText)
	{
		//テキストが見やすいように後ろを暗くする
		primitive->Render(TEXT_BOX_POS.x, TEXT_BOX_POS.y, TEXT_BOX_SIZE.x, TEXT_BOX_SIZE.y, 0.f, 0.f, 0.f, 0.5f, 0);
		//テキストを表示
		text.Draw(TEXT_MESSEAGE[textState].text.c_str(), TEXTPOSITION, textSize, TEXT_ALIGN::MIDDLE);
	}

	//個別で説明を出す
	if (tutorialState == EXPLAIN_STATE::INPUT_MOVE)
	{
		primitive->Render(ANOTHER_BOX_POS.x, ANOTHER_BOX_POS.y, ANOTHER_BOX_SIZE.x, ANOTHER_BOX_SIZE.y, 0.f, 0.f, 0.f, 0.5f, 0);
		//コントローラーが接続されているかどうかで出すテキストを変える
		std::wstring t;
		if (InputManager::Instance().IsGamePadConnected())
			t = L"青：右スティック、赤：左スティック";
		else
			t = L"青：WASD、赤：十字キー";
		text.Draw(t.c_str(), ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
	if (tutorialState == EXPLAIN_STATE::LENGTH_STICK)
	{
		primitive->Render(ANOTHER_BOX_POS.x, ANOTHER_BOX_POS.y, ANOTHER_BOX_SIZE.x, ANOTHER_BOX_SIZE.y, 0.f, 0.f, 0.f, 0.5f, 0);
		text.Draw(L"棒を伸ばして、速度が上がることを確認する", ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
	if (tutorialState == EXPLAIN_STATE::MAXLENGTH)
	{
		primitive->Render(ANOTHER_BOX_POS.x, ANOTHER_BOX_POS.y, ANOTHER_BOX_SIZE.x, ANOTHER_BOX_SIZE.y, 0.f, 0.f, 0.f, 0.5f, 0);
		text.Draw(L"棒の長さの限界を超えてみる", ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
	if(tutorialState == EXPLAIN_STATE::HITOBJECT_ROPE)
	{
		primitive->Render(ANOTHER_BOX_POS.x, ANOTHER_BOX_POS.y, ANOTHER_BOX_SIZE.x, ANOTHER_BOX_SIZE.y, 0.f, 0.f, 0.f, 0.5f, 0);
		text.Draw(L"棒に障害物を当てよう", ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
}
