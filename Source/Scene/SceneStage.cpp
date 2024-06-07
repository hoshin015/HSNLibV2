#include <tchar.h>
#include <DirectXMath.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <Windows.h>
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
#include "../../Library/2D/Primitive2D.h"
// --- Scene ---
#include "SceneTest.h"
#include "SceneStage.h"
#include "SceneManager.h"
// --- Game ---
#include "../Game/Object/Stage/StageManager.h"
#include "../Game/Object/Stage/StageMain.h"
#include "../../Library/3D/DebugPrimitive.h"

#include "../Game/Object/Player/PlayerManager.h"

// STAGE CREATE
#define SC
 


void SceneStage::Initialize()
{
	// カメラ初期設定
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 20, 20),		// カメラ座標
		DirectX::XMFLOAT3(0, 0, 0),			// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);

#ifndef SC
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(cameraAngle), DirectX::XMConvertToRadians(180), 0 });
	Camera::Instance().cameraType = Camera::CAMERA::TARGET_PLAYER;
#else
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(45), DirectX::XMConvertToRadians(180), 0 });
	Camera::Instance().cameraType = Camera::CAMERA::FREE;
#endif

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
	//StageMain* stageMain = new StageMain("Data/Fbx/ExampleStage/ExampleStage.model");
	StageMain* stageMain = new StageMain("Data/Fbx/Stage/stage.fbx");

	stageManager.Register(stageMain);
	float scale = 0.35f;
	stageMain->SetScale(DirectX::XMFLOAT3{ scale, scale, scale });


	bitBlockTransfer = std::make_unique<FullScreenQuad>();
	frameBuffer = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF(), true);
	bloom = std::make_unique<Bloom>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF());
	shadow = std::make_unique<Shadow>();



	testStatic = std::make_unique<TestStatic>("Data/Fbx/Albino/Albino.model");
	testAnimated = std::make_unique<TestAnimated>("Data/Fbx/CatfishA/CatfishA.model");


	//存在するオブジェクトを定義する
	objects.insert(std::make_pair(eObjectType::Kesigomu,  std::make_unique<Object3D>("Data/Fbx/Kesigomu/Kesigomu.fbx",   eObjectType::Kesigomu)));
	objects.insert(std::make_pair(eObjectType::Pentate,   std::make_unique<Object3D>("Data/Fbx/Pentate/Pentate.fbx",     eObjectType::Pentate)));
	objects.insert(std::make_pair(eObjectType::Enpitu,    std::make_unique<Object3D>("Data/Fbx/Enpitu/Enpitu.fbx",       eObjectType::Enpitu)));
	objects.insert(std::make_pair(eObjectType::Tokei,     std::make_unique<Object3D>("Data/Fbx/Tokei/Tokei.fbx",         eObjectType::Tokei)));
	objects.insert(std::make_pair(eObjectType::Kikyapu,   std::make_unique<Object3D>("Data/Fbx/Kikyapu/Kikyapu.fbx",     eObjectType::Kikyapu)));
	objects.insert(std::make_pair(eObjectType::Kuripu,    std::make_unique<Object3D>("Data/Fbx/Kuripu/Kuripu.fbx",       eObjectType::Kuripu)));
	objects.insert(std::make_pair(eObjectType::Sunatokei, std::make_unique<Object3D>("Data/Fbx/Sunatokei/Sunatokei.fbx", eObjectType::Sunatokei)));
	objects.insert(std::make_pair(eObjectType::Goal,      std::make_unique<Object3D>("Data/Fbx/Goal/Stage.fbx",          eObjectType::Goal)));



	//プレイヤー初期化
	PlayerManager& playerManager = PlayerManager::Instance();
	//playerManager.SetRope("Data/Fbx/Enpitu/Enpitu.fbx");
	playerManager.Initialize();
	/*Player* player1 = new Player("Data/Fbx/Player_02/Player_02.model", false);
	playerManager.Register(player1);
	Player* player2 = new Player("Data/Fbx/Player_02/Player_02.model", true);
	player2->SetPosX(50.0f);
	playerManager.Register(player2);*/

	//sprTest = std::make_unique<Sprite>("Data/Texture/bomb/bomb.sprite");
	//sprTest2 = std::make_unique<Sprite>("Data/Texture/Icon.sprite");
	//sprTest2->SetPos({ 200, 100 });
	//sprTest3 = std::make_unique<Sprite>("Data/Texture/Nessie.sprite");
	//sprTest3->SetPos({ 500, 100 });
	//sprTest3->SetScale({ 0.2, 0.2 });
	//sprTest3->UpdateAnimation();

	//Particle::Instance().Initialize();
	///emitter1 = std::make_unique<Emitter>();
	//emitter1->position = { 0, 3,0 };
	//particle->Initialize();


	LoadFileStage("Data/Stage/Stage.txt");

}

void SceneStage::Finalize()
{
	StageManager::Instance().Clear();
	LightManager::Instance().Clear();

	PlayerManager::Instance().Clear();
}

void SceneStage::Update()
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
#ifndef SC
	DirectX::XMFLOAT3 cameraTarget = PlayerManager::Instance().GetPositionCenter();
	cameraTarget += cameraOffset;
	Camera::Instance().SetTarget(cameraTarget);
#endif
	Camera::Instance().Update();


	// タイマーの定数バッファの更新
	UpdateTimerConstnat();

	// ステージ更新
	StageManager::Instance().Update();

	PlayerManager::Instance().Update();

	StageCollision();
	StageVsRope();

	testStatic->Update();
	testAnimated->Update();


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

	// シーンに配置されたオブジェクトの当たり判定の情報をコピーする
	collisions.clear();
	for (auto& object : objects) {
		for (auto& c : object.second->collisions) {
			collisions.emplace_back(c);
		}
	}

	//sprTest->SetAngle(sprTest->GetAngle() + 180 * Timer::Instance().DeltaTime());
	//sprTest->UpdateAnimation();

	//sprTest3->SetAngle(sprTest->GetAngle() + 180 * Timer::Instance().DeltaTime());

	//emitter1->Update();
	//Particle::Instance().Update();
}

void SceneStage::Render()
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

		//Particle::Instance().Render();

		

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
	//sprTest->Render();
	//sprTest2->Render();
	//sprTest3->Render();

	static Primitive2D rect;
	rect.Render(xx, xy, xw, xh, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	rect.Render(zx, zy, zw, zh, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

#if USE_IMGUI
	// --- デバッグGUI描画 ---
	DrawDebugGUI();

	LightManager::Instance().DrawDebugGui();
	bloom->DrawDebugGui();
	shadow->DrawDebugGui();

#if SHOW_PERFORMANCE
	// --- パフォーマンス描画 ---
	ImGuiManager::Instance().DisplayPerformanceStats();
#endif

	// --- imguiManager 描画 ---
	ImGuiManager::Instance().Render();
#endif
}

// スクリーン座標からワールド座標へ変換
inline DirectX::XMFLOAT3 ToWorldPosition(
	const DirectX::XMFLOAT3& position,
	const DirectX::XMFLOAT4X4& view,
	const DirectX::XMFLOAT4X4& projection)
{

	ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

	//ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	//変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
	DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);

	//ワールド座標に変換
	DirectX::XMVECTOR WorldPosition = DirectX::XMVector3Unproject(
		Position,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World
	);


	DirectX::XMFLOAT3 outPosition;
	DirectX::XMStoreFloat3(&outPosition, WorldPosition);

	return outPosition;
}

// デバッグ描画
void SceneStage::DrawDebugGUI()
{
	Graphics* gfx = &Graphics::Instance();
	Camera* camera = &Camera::Instance();
	InputManager* input = &InputManager::Instance();

	//操作するオブジェクトを選ぶ
	static Object3D::Transform* workObject = nullptr;
	static int workNumber = 0;
	if (input->GetMousePressed(MOUSEBUTTON_STATE::leftButton)) {
		int i = 0;
		for (auto& collision : collisions) {

			DirectX::XMFLOAT3 click;
			click.x = static_cast<float>(input->GetCursorPosXFloat());
			click.y = static_cast<float>(input->GetCursorPosYFloat());
			click.z = 0.0f;

			DirectX::XMFLOAT3 worldScreenPosition =
				ToWorldPosition(click, camera->GetView(), camera->GetProjection());

			click.z = 1.0f;
			DirectX::XMFLOAT3 worldPosition =
				ToWorldPosition(click, camera->GetView(), camera->GetProjection());

			//DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&worldScreenPosition);
			//DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&worldPosition);
			//DirectX::XMVECTOR Ray = DirectX::XMVectorSubtract(v1, v0);
		
			//コリジョンと当たり判定
			if (PlayerManager::IntersectSphereVsLine(collision.pos, collision.radius, worldScreenPosition, worldPosition)) {
				//当たったオブジェクトを操作する
				//MessageBox(nullptr, L"当たり", nullptr, NULL);
				workObject = nullptr;
				workObject = collision.parent;
				workNumber = i;
			}

			i++;
		}
	}

	//オブジェクトの操作
	if (workObject != nullptr)
	{

		auto HitRect = [](float x, float y, float w, float h, float x2, float y2, float w2, float h2)->bool {
			if (x > x2 + w2) return false;
			if (x + w < w2) return false;
			if (y > y2 + h2) return false;
			if (y + h < y2) return false;

			return true;
		};

		float mx = input->GetCursorPosXFloat();
		float my = input->GetCursorPosYFloat();
		float mw = 2.0f, mh = 2.0f;
		static float ox, oy, owp;
		static float* wp = nullptr;

		static bool isMoveObj;

		if (input->GetMousePress(MOUSEBUTTON_STATE::leftButton) && !isMoveObj)
		{
			if (HitRect(xx, xy, xw, xh, mx, my, mw, mh))
			{
				isMoveObj = true;
				wp = &workObject->pos.x;
				owp = *wp;
				ox = mx, oy = my;
			}

			if (HitRect(zx, zy, zw, zh, mx, my, mw, mh))
			{
				isMoveObj = true;
				wp = &workObject->pos.z;
				owp = *wp;
				ox = mx, oy = my;
			}
		}

		if (input->GetMouseReleased(MOUSEBUTTON_STATE::leftButton))
		{
			isMoveObj = false;
		}

		if (isMoveObj)
		{
			*wp = owp + ((oy - my) * 2.0f);
		}
	}
	
	// メニューバー描画
	DrawMenuBar();

	if (ImGui::Begin("Debug Menu")) {

		//objects->UpdateImGui();
		//for (auto& object : objects) {
		//	object.second->UpdateImGui(objectNames[object.first]);
		//}

		//オブジェクトを追加する
		if (ImGui::Button("Add Pentate")) {
			objects.at(eObjectType::Pentate)->Add(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
			workObject = nullptr;
		}
		if (ImGui::Button("Add Kesigomu")) {
			objects.at(eObjectType::Kesigomu)->Add(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
			workObject = nullptr;
		}
		if (ImGui::Button("Add Enpitu")) {
			objects.at(eObjectType::Enpitu)->Add(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
			workObject = nullptr;
		}
		if (ImGui::Button("Add Tokei")) {
			objects.at(eObjectType::Tokei)->Add(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
			workObject = nullptr;
		}
		if (ImGui::Button("Add Kikyapu")) {
			objects.at(eObjectType::Kikyapu)->Add(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
			workObject = nullptr;
		}
		if (ImGui::Button("Add Kuripu")) {
			objects.at(eObjectType::Kuripu)->Add(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
			workObject = nullptr;
		}
		if (ImGui::Button("Add Sunatokei")) {
			objects.at(eObjectType::Sunatokei)->Add(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
			workObject = nullptr;
		}
		if (ImGui::Button("Add Goal")) {
			objects.at(eObjectType::Goal)->Add(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
			workObject = nullptr;
		}

		ImGui::Text("\n\n\n");

		//オブジェクトを操作する
		ImGui::Text("Work Object\n");
		if (workObject != nullptr) {
			
			float* d[]{ &workObject->pos.x, &workObject->pos.y, &workObject->pos.z };
			ImGui::SliderFloat3("Position Slider", *d, -100.0f, 100.0f);
			ImGui::InputFloat3("Position Input", *d);
		
			if (ImGui::Button("Remove")) {
				for (auto& obj : objects) {
					int i = 0;
					for (auto& t : obj.second->transforms){
						if (workObject == &t) {
							obj.second->transforms.erase(obj.second->transforms.begin() + i);
							obj.second->Remove();
							workObject = nullptr;
							//break;
						}
						i++;
					}
				}
			}
		}

		ImGui::Text("\n\n\n");

		//ステージ情報を保存するファイル名
		ImGui::InputText("Save File", filename, IM_ARRAYSIZE(filename));

		//ステージ情報を保存
		if (ImGui::Button("Save")) {
			if (MessageBox(nullptr, L"ステージを保存しますか？", L"ステージの保存", MB_YESNO) == IDYES)
			{
				if (SaveFileStage(filename)) {
					MessageBox(nullptr, L"保存に成功しました。", L"ステージの保存", MB_ICONINFORMATION);
				}
				else {
					ErrorLogger::Log("保存に失敗しました。");
				}
			}
		}
		//ステージ情報をロード
		if (ImGui::Button("Load")) {
			if (!LoadFileStage(filename)) {
				ErrorLogger::Log("読み込みに失敗しました。");
			}
		}
		//ステージを初期化
		if (ImGui::Button("Clear")) {
			ClearStage();
		}

		ImGui::End();
	}

	PlayerManager::Instance().DrawDebugImGui();

	if (ImGui::Begin("CameraParameter"))
	{
		ImGui::SliderFloat3("CameraOffset", &cameraOffset.x, -200, 200);
	}
	ImGui::End();
}

bool SceneStage::SaveFileStage(const char* filename)
{
	std::ofstream file(filename);

	for (auto& object : objects) {
		for (auto& transform : object.second->transforms) {
			int objectType = static_cast<int>(object.first);
			file << objectType << " " << transform.pos.x << " " << transform.pos.y << " " << transform.pos.z << std::endl;
		}
	}

	return true;
}

void  SceneStage::ClearStage() 
{
	for (auto& object : objects) {
		//object.second->transforms.clear();
		object.second->Clear();
	}
}

void SceneStage::StageCollision()
{
	std::vector<Player*> players = PlayerManager::Instance().GetPlayer();

	for (Player* player : players)
	{
		for (Object3D::Collision collision : collisions)
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

void SceneStage::StageVsRope()
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
		if (PlayerManager::Instance().IntersectSphereVsLine(collision.pos,collision.radius,rayPos[0],rayPos[1]))
		{
			for (Player* player : PlayerManager::Instance().GetPlayer())
			{
				//死亡処理
				player->SetDeath();
			}
		}
	}
	
}

bool SceneStage::LoadFileStage(const char* filename)
{
	std::ifstream file(filename);
	if (!file) {
		return false;
	}

	//ステージを初期化する
	ClearStage();

	std::string str{};

	while (std::getline(file, str)) {
		int objectType = 0;
		float _x = 0.0f, _y = 0.0f, _z = 0.0f;
		std::stringstream ss(str);
		ss >> objectType >>  _x >> _y >> _z;

		objects.at(static_cast<eObjectType>(objectType))->Add(DirectX::XMFLOAT3{ _x, _y, _z  });
	}

	return true;
}
