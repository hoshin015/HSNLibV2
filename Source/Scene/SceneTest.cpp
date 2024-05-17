#include <tchar.h>
#include <DirectXMath.h>
#include "SceneTest.h"
#include "SceneManager.h"
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
#include "../Game/Object/Stage/StageManager.h"
#include "../Game/Object/Stage/StageMain.h"


void SceneTest::Initialize()
{
	testStatic = std::make_unique<TestStatic>("Data/Fbx/Jummo/Jummo.model");
	sprTest = std::make_unique<Sprite>("Data/Texture/bomb/bomb.sprite");
	sprTest2 = std::make_unique<Sprite>("Data/Texture/Icon.sprite");
	sprTest2->SetPos({ 200, 100 });
	sprTest3 = std::make_unique<Sprite>("Data/Texture/Nessie.sprite");
	sprTest3->SetPos({ 500, 100 });
	sprTest3->SetScale({ 0.2, 0.2 });
	sprTest3->UpdateAnimation();

	// カメラ初期設定
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 5, 20),		// カメラ座標
		DirectX::XMFLOAT3(0, 0, 0),			// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);
	//Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(-45), DirectX::XMConvertToRadians(45), 0 });
	Camera::Instance().cameraType = Camera::CAMERA::FREE;

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
	light->SetRange(30.0f);
	LightManager::Instance().Register(light);

	LightManager::Instance().SetAmbientColor({ 0,0,0,1.0f });
#endif

	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain("Data/Fbx/ExampleStage/ExampleStage.model");
	stageManager.Register(stageMain);
}

void SceneTest::Finalize()
{
	StageManager::Instance().Clear();
	LightManager::Instance().Clear();
}

void SceneTest::Update()
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
	Camera::Instance().Update();



	// ステージ更新
	StageManager::Instance().Update();

	testStatic->Update();

	sprTest->SetAngle(sprTest->GetAngle() + 180 * Timer::Instance().DeltaTime());
	sprTest->UpdateAnimation();

	sprTest3->SetAngle(sprTest->GetAngle() + 180 * Timer::Instance().DeltaTime());

}

void SceneTest::Render()
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

	// rasterizerStateの設定
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilStateの設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendStateの設定
	gfx->SetBlend(BLEND_STATE::ALPHA);


	// カメラの定数バッファの更新
	Camera::Instance().UpdateCameraConstant();

	// ライトの定数バッファの更新
	LightManager::Instance().UpdateConstants();



	StageManager::Instance().Render();

	testStatic->Render();
	sprTest->Render();
	sprTest2->Render();
	sprTest3->Render();



#if USE_IMGUI
	// --- デバッグ描画 ---
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

// デバッグ描画
void SceneTest::DrawDebugGUI()
{
	// メニューバー描画
	DrawMenuBar();
}
