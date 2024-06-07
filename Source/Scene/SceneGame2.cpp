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
// --- Scene ---
#include "SceneGame2.h"
#include "SceneManager.h"
// --- Game ---
#include "../Game/Object/Stage/StageManager.h"
#include "../Game/Object/Stage/StageMain.h"
#include "../../Library/3D/DebugPrimitive.h"

#include "../../Source/Scene/Score/Score.h"
#include "../Game/Object/Player/PlayerManager.h"
#include "../../Library/Particle/EmitterManager.h"

void SceneGame2::Initialize()
{
	// カメラ初期設定
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 20, 20),		// カメラ座標
		DirectX::XMFLOAT3(0, 0, 0),			// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(cameraAngle), DirectX::XMConvertToRadians(180), 0 });
	Camera::Instance().cameraType = Camera::CAMERA::TARGET_PLAYER;

#if 0
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
	light->SetRange(3000.0f);
	LightManager::Instance().Register(light);

	LightManager::Instance().SetAmbientColor({ 0.01f,0.01f,0.01f,1.0f });
#endif
	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain("Data/Fbx/stage/stage.model");
	stageManager.Register(stageMain);
	stageMain->SetScale(DirectX::XMFLOAT3{ stageScale, stageScale, stageScale });

	bitBlockTransfer = std::make_unique<FullScreenQuad>();
	frameBuffer = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF(), true);
	bloom = std::make_unique<Bloom>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF());
	shadow = std::make_unique<Shadow>();

	// --- パーティクル初期化 ---
	Particle::Instance().Initialize();

	// --- エミッター登録 ----
	Emitter* emitter0 = new Emitter();
	emitter0->position = { 0, 3, 3 };
	emitter0->rate = 30000;
	emitter0->duration = 1.1;
	emitter0->looping = false;
	emitter0->rateOverTime = 1;
	emitter0->startKind = 0;
	emitter0->startLifeTime = 1.0f;
	emitter0->startSize = 1.5f;
	emitter0->startColor = { 2.5,2.5,2.5,1 };
	EmitterManager::Instance().Register(emitter0);


	//プレイヤー初期化
	PlayerManager& playerManager = PlayerManager::Instance();
	PlayerManager::Instance().Initialize();
	/*Player* player1 = new Player("Data/Fbx/Player_02/Player_02.model", false);
	player1->SetPos({ 0,0,0 });
	playerManager.Register(player1);

	Player* player2 = new Player("Data/Fbx/Player_02/Player_02.model", true);
	player2->SetPos({ 50.0f,0,0 });
	playerManager.Register(player2);*/


	//存在するオブジェクトを定義する
	objects.insert(std::make_pair(eObjectType::Kesigomu, std::make_unique<Object3D>("Data/Fbx/Kesigomu/Kesigomu.fbx", eObjectType::Kesigomu)));
	objects.insert(std::make_pair(eObjectType::Pentate, std::make_unique<Object3D>("Data/Fbx/Pentate/Pentate.fbx", eObjectType::Pentate)));
	objects.insert(std::make_pair(eObjectType::Enpitu, std::make_unique<Object3D>("Data/Fbx/Enpitu/Enpitu.fbx", eObjectType::Enpitu)));
	objects.insert(std::make_pair(eObjectType::Tokei, std::make_unique<Object3D>("Data/Fbx/Tokei/Tokei.fbx", eObjectType::Tokei)));
	objects.insert(std::make_pair(eObjectType::Kikyapu, std::make_unique<Object3D>("Data/Fbx/Kikyapu/Kikyapu.fbx", eObjectType::Kikyapu)));
	objects.insert(std::make_pair(eObjectType::Kuripu, std::make_unique<Object3D>("Data/Fbx/Kuripu/Kuripu.fbx", eObjectType::Kuripu)));
	objects.insert(std::make_pair(eObjectType::Sunatokei, std::make_unique<Object3D>("Data/Fbx/Sunatokei/Sunatokei.fbx", eObjectType::Sunatokei)));
	objects.insert(std::make_pair(eObjectType::Goal, std::make_unique<Object3D>("Data/Fbx/Goal/Stage.fbx", eObjectType::Goal)));

	//ゴール
	objects.insert(std::make_pair(eObjectType::Goal, std::make_unique<Object3D>("Data/Fbx/Goal/Stage.fbx", eObjectType::Goal)));
	std::ifstream file("Data/Stage/Stage.txt");

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

	Score::Instance().Initialize();
}

void SceneGame2::Finalize()
{
	LightManager::Instance().Clear();

	StageManager::Instance().Clear();

	PlayerManager::Instance().Clear();

	//振動を止める
	if (InputManager::Instance().IsGamePadConnected())
		InputManager::Instance().SetVibration(0, 0.0f, 0.0f);


	EmitterManager::Instance().Clear();
}

void SceneGame2::Update()
{

#if USE_IMGUI
	// --- imguiManager処理 ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager処理 ---
	InputManager::Instance().Update();

	// --- effectManager処理 ---
	EffectManager::Instance().Update();

	

	// 点光源座標更新
	DirectX::XMFLOAT3 pointLPos = PlayerManager::Instance().GetPositionCenter();
	pointLPos.y += 100;
	pointLPos.z += 100;
	LightManager::Instance().GetLight(0)->SetPosition(pointLPos);

	// --- カメラ処理 ---
	CameraUpdate();

	if(isFinishCamera)
		Score::Instance().Update();

	// タイマーの定数バッファの更新
	UpdateTimerConstnat();
	StageManager::Instance().Update();

	PlayerManager::Instance().Update();

	StageCollision();
	StageVsRope();
	GoalCheack();

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

	// --- パーティクル更新 ---
	EmitterManager::Instance().Update();
	Particle::Instance().Update();
}

void SceneGame2::Render()
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
				//testAnimated->Render(true);



				// static object
				shadow->SetStaticShader();
				//testStatic->Render(true);


				//objects->Render(true);
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

		Particle::Instance().Render();

		// rasterizerStateの設定
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
		// depthStencilStateの設定
		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_OFF);
		// blendStateの設定
		gfx->SetBlend(BLEND_STATE::ALPHA);
	}
	frameBuffer->DeActivate();

#if 01
	// ブルーム処理しての描画
	bloom->Make(frameBuffer->shaderResourceViews[0].Get());
	bitBlockTransfer->blit(bloom->GetSrvAddress(), 0, 1, nullptr, nullptr);
#else
	// そのまま描画
	bitBlockTransfer->blit(frameBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1, nullptr, nullptr);
#endif	

	// ブルームなし

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

void SceneGame2::DrawDebugGUI()
{
	PlayerManager::Instance().DrawDebugImGui();

	DrawMenuBar();

}

void SceneGame2::StageCollision()
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

void SceneGame2::StageVsRope()
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

void SceneGame2::LerpCameraTarget(DirectX::XMFLOAT3 target, float factor)
{
	DirectX::XMFLOAT3 cameraPosition = { 0,0,0 };
	DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR CurrentTarget = DirectX::XMLoadFloat3(&cameraTarget);

	CurrentTarget = DirectX::XMVectorLerp(CurrentTarget, Target, factor);
	DirectX::XMStoreFloat3(&cameraTarget, CurrentTarget);

	Camera::Instance().SetTarget(cameraTarget);
}


void SceneGame2::CameraUpdate()
{

#if 1
	//カメラの初期位置
	const float INITIAL_CAMERA_Z = -8000 * stageScale;
	const float INITIAL_CAMERA_Y = 150;
	//カメラのプレイ時の角度
	const float PLAING_ANGLE = 30.0f;
	//線形補完の係数
	const float FACTOR_Y = 0.1f;
	const float FACTOR_Z = 0.01f;
	//カメラが奥から来た時に終了する条件
	const float FINISH_LENGTH = 0.5f;

	//通常のカメラのターゲット
	const DirectX::XMFLOAT3 CAMERA_TARGET = PlayerManager::Instance().GetPositionCenter() + cameraOffset;
	//演出の時に使う一番初めのターゲット
	const DirectX::XMFLOAT3 FIRST_TARGET = { CAMERA_TARGET.x,CAMERA_TARGET.y + INITIAL_CAMERA_Y,CAMERA_TARGET.z };

	const DirectX::XMFLOAT3 OLD_CAMERA_TARGET = cameraTarget;

	switch (cameraState)
	{
	case 0:
		//初期設定
		//プレイヤーが動かないようにする
		PlayerManager::Instance().SetInputPlayerMove(false);
		PlayerManager::Instance().SetIsMoveZ(false);
		PlayerManager::Instance().SetIsUpdateZ(false);

		//カメラのターゲットの初期値を設定
		cameraTarget = CAMERA_TARGET;
		//カメラが本来より上斜め奥にあるようにする
		cameraTarget.z += INITIAL_CAMERA_Z;
		cameraTarget.y += INITIAL_CAMERA_Y;

		Camera::Instance().SetTarget(cameraTarget);
		cameraState++;

		//初期設定なのでそのまま進める
		/*fall through*/

	case 1:
		//カメラが奥から手前に来る
		LerpCameraTarget(FIRST_TARGET, FACTOR_Z);

		Camera::Instance().Update();

		//前回の移動した時との差が一定以下だったら終了
		if (cameraTarget.z - OLD_CAMERA_TARGET.z > -0.1f &&
			cameraTarget.z - OLD_CAMERA_TARGET.z < 0.1f)
			cameraState++;

		break;

	case 2:
		//カメラが上から本来の位置へ来る
		LerpCameraTarget(CAMERA_TARGET, FACTOR_Y);
		//カメラのアングルを変更
		cameraAngle = cameraAngle + (FACTOR_Y / 2.0f) * (PLAING_ANGLE - cameraAngle);

		//設定
		Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(cameraAngle),DirectX::XMConvertToRadians(180), 0 });
		Camera::Instance().Update();

		//前回の移動した時との差が一定以下だったら終了
		if (cameraTarget.y - OLD_CAMERA_TARGET.y > -0.1f &&
			cameraTarget.y - OLD_CAMERA_TARGET.y < 0.1f)
			cameraState++;

		break;

	case 3:
		//プレイヤーが動くようにする
		PlayerManager::Instance().SetInputPlayerMove(true);
		PlayerManager::Instance().SetIsMoveZ(true);
		PlayerManager::Instance().SetIsUpdateZ(true);
		isFinishCamera = true;
		cameraState++;
		break;

	default:
		//通常処理
		cameraTarget = CAMERA_TARGET;
		Camera::Instance().SetTarget(cameraTarget);
		Camera::Instance().Update();

		break;
	}

#elif 0
	//プレイヤーの初期角度
	const float PLAYER_ANGLE = 90;
	cameraTimer += Timer::Instance().DeltaTime();
	float angle = (PLAYER_ANGLE + cameraTimer * 360 / CAMERA_LAPTIME) * DirectX::XM_PI / 180.0f;

	if (cameraTimer < CAMERA_LAPTIME)
	{
		//カメラがぐるっと1周する
		CameraRendition(PlayerManager::Instance().GetPositionCenter(), 500, 500, angle);
		//プレイヤーが動かないようにする
		PlayerManager::Instance().SetInputPlayerMove(false);
		PlayerManager::Instance().SetIsMoveZ(false);
		PlayerManager::Instance().SetIsUpdateZ(false);
	}
	else
	{
		PlayerManager::Instance().SetInputPlayerMove(true);
		PlayerManager::Instance().SetIsMoveZ(true);
		PlayerManager::Instance().SetIsUpdateZ(true);
		// --- カメラ処理 ---
		DirectX::XMFLOAT3 cameraTarget = PlayerManager::Instance().GetPositionCenter();
		cameraTarget += cameraOffset;
		Camera::Instance().SetTarget(cameraTarget);
		Camera::Instance().Update();
	}
#elif 0
	// --- カメラ処理 ---
	DirectX::XMFLOAT3 cameraTarget = PlayerManager::Instance().GetPositionCenter();
	cameraTarget += cameraOffset;
	Camera::Instance().SetTarget(cameraTarget);
	Camera::Instance().Update();
#endif
}


void SceneGame2::GoalCheack()
{
	PlayerManager& manager = PlayerManager::Instance();

	for (auto& object : objects) {
		if (object.first != eObjectType::Goal)
			continue;

		for (int i = 0; i < object.second->GetActive(); i++)
		{
			//ゴールのz座標
			float goleZ = object.second->GetTransform(i).pos.z;

			//プレイヤーの両方がゴールを超えたらクリア
			if (manager.GetPlayer().at(0)->GetPosZ() < goleZ &&
				manager.GetPlayer().at(1)->GetPosZ() < goleZ)
			{
				SceneManager::Instance().ChangeScene(new SceneClear);
			}
		}
	}
}
