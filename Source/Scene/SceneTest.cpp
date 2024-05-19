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
	// カメラ初期設定
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 20, 20),		// カメラ座標
		DirectX::XMFLOAT3(0, 0, 0),			// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(45), DirectX::XMConvertToRadians(180), 0 });
	Camera::Instance().cameraType = Camera::CAMERA::FREE;

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
	StageMain* stageMain = new StageMain("Data/Fbx/ExampleStage/ExampleStage.model");
	stageManager.Register(stageMain);


	bitBlockTransfer = std::make_unique<FullScreenQuad>();
	frameBuffer = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF(), true);
	bloom = std::make_unique<Bloom>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF());
	shadow = std::make_unique<Shadow>();



	testStatic = std::make_unique<TestStatic>("Data/Fbx/Monster/arakBarrak_v025.model");
	testAnimated = std::make_unique<TestAnimated>("Data/Fbx/Monster/arakBarrak_v025.model");
	sprTest = std::make_unique<Sprite>("Data/Texture/bomb/bomb.sprite");
	sprTest2 = std::make_unique<Sprite>("Data/Texture/Icon.sprite");
	sprTest2->SetPos({ 200, 100 });
	sprTest3 = std::make_unique<Sprite>("Data/Texture/Nessie.sprite");
	sprTest3->SetPos({ 500, 100 });
	sprTest3->SetScale({ 0.2, 0.2 });
	sprTest3->UpdateAnimation();
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
	testAnimated->Update();

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

	// shadowMap
	{
		// シャドウマップ分割エリア定義
		float splitAreaTable[] =
		{
			Camera::Instance().GetNearZ(),
			100.0f,
			300.0f,
			500.0f,
			Camera::Instance().GetFarZ()
		};
		// カメラのパラメータ取得
		DirectX::XMVECTOR cameraFront = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetFront()));
		DirectX::XMVECTOR cameraRight = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetRight()));
		DirectX::XMVECTOR cameraUp = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetUp()));
		DirectX::XMVECTOR cameraPos = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetEye()));

		// 定数バッファ更新
		shadow->UpdateConstants();	// ここで通常描画で使用する定数も一緒に更新している
		shadow->SetShadowShader();

		// 平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を生成
		DirectX::XMFLOAT3 dir = LightManager::Instance().GetLight(0)->GetDirection();
		DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&dir);
		LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);
		DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(
			LightPosition,
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		);

		// シャドウマップに描画したい範囲の射影行列を生成
		DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(1000, 1000, 0.1f, 1000.0f);
		DirectX::XMMATRIX viewProjection = V * P;
		///DirectX::XMStoreFloat4x4(&shadow->shadowConstants.lightViewProjection, viewProjection);	// ビュー　プロジェクション　変換行列をまとめる

		shadow->Clear();	// 画面クリア
		for (int i = 0; i < SHADOWMAP_COUNT; i++)
		{
			float nearDepth = splitAreaTable[i + 0];
			float farDepth = splitAreaTable[i + 1];

			shadow->Activate(i);

			// エリアを内包する視推台の8頂点を算出する
			DirectX::XMVECTOR vertex[8];
			{
				// エリア近平面の中心から上面までの距離を求める
				float nearY = tanf(Camera::Instance().GetFovY() / 2.0f) * nearDepth;
				// エリア近平面の中心から右面までの距離を求める
				float nearX = nearY * Camera::Instance().GetAspect();
				// エリア遠平面の中心から上面までの距離を求める
				float farY = tanf(Camera::Instance().GetFovY() / 2.0f) * farDepth;
				// エリア遠平面の中心から右面までの距離を求める
				float farX = farY * Camera::Instance().GetAspect();

				// エリア近平面の中心座標を求める
				DirectX::XMVECTOR nearPos = cameraPos + cameraFront * nearDepth;
				// エリア遠平面の中心座標を求める
				DirectX::XMVECTOR farPos = cameraPos + cameraFront * farDepth;

				// 8頂点を求める
				{
					// 近平面の右上
					vertex[0] = nearPos + cameraUp * nearY + cameraRight * nearX;
					// 近平面の左上
					vertex[1] = nearPos + cameraUp * nearY - cameraRight * nearX;
					// 近平面の右下
					vertex[2] = nearPos - cameraUp * nearY + cameraRight * nearX;
					// 近平面の左下
					vertex[3] = nearPos - cameraUp * nearY - cameraRight * nearX;
					// 遠平面の右上
					vertex[4] = farPos + cameraUp * farY + cameraRight * farX;
					// 遠平面の左上
					vertex[5] = farPos + cameraUp * farY - cameraRight * farX;
					// 遠平面の右下
					vertex[6] = farPos - cameraUp * farY + cameraRight * farX;
					// 遠平面の左下
					vertex[7] = farPos - cameraUp * farY - cameraRight * farX;
				}
			}
			// 8点を LightViewProjection 空間に変換して、最大値、最小値を求める
			DirectX::XMFLOAT3 vertexMin(FLT_MAX, FLT_MAX, FLT_MAX);
			DirectX::XMFLOAT3 vertexMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			for (auto& v : vertex)
			{
				DirectX::XMFLOAT3 p;
				DirectX::XMStoreFloat3(&p, DirectX::XMVector3TransformCoord(v, viewProjection));

				vertexMin.x = min(p.x, vertexMin.x);
				vertexMin.y = min(p.y, vertexMin.y);
				vertexMin.z = min(p.z, vertexMin.z);
				vertexMax.x = max(p.x, vertexMax.x);
				vertexMax.y = max(p.y, vertexMax.y);
				vertexMax.z = max(p.z, vertexMax.z);
			}

			// クロップ行列を求める
			float xScale = 2.0f / (vertexMax.x - vertexMin.x);
			float yScale = 2.0f / (vertexMax.y - vertexMin.y);
			float xOffset = -0.5f * (vertexMax.x + vertexMin.x) * xScale;
			float yOffset = -0.5f * (vertexMax.y + vertexMin.y) * yScale;
			DirectX::XMFLOAT4X4 clopMatrix;
			clopMatrix._11 = xScale;
			clopMatrix._12 = 0;
			clopMatrix._13 = 0;
			clopMatrix._14 = 0;
			clopMatrix._21 = 0;
			clopMatrix._22 = yScale;
			clopMatrix._23 = 0;
			clopMatrix._24 = 0;
			clopMatrix._31 = 0;
			clopMatrix._32 = 0;
			clopMatrix._33 = 1;
			clopMatrix._34 = 0;
			clopMatrix._41 = xOffset;
			clopMatrix._42 = yOffset;
			clopMatrix._43 = 0;
			clopMatrix._44 = 1;
			DirectX::XMMATRIX ClopMatrix = DirectX::XMLoadFloat4x4(&clopMatrix);

			// lightViewProjection 行列にクロップ行列を乗算
			DirectX::XMStoreFloat4x4(&shadow->shadowConstants.lightViewProjection, viewProjection * ClopMatrix);
			dc->UpdateSubresource(shadow->shadowConstant.Get(), 0, 0, &shadow->shadowConstants, 0, 0);
			dc->VSSetConstantBuffers(_shadowConstant, 1, shadow->shadowConstant.GetAddressOf());
			dc->PSSetConstantBuffers(_shadowConstant, 1, shadow->shadowConstant.GetAddressOf());

			// 影を付けたいモデルはここで描画を行う(Render の引数に true をいれる)
			{
				StageManager::Instance().Render(true);
				//testStatic->Render(true);
				testAnimated->Render(true);
			}
			shadow->DeActivate();
			shadow->SetShadowTexture();
		}
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

	// 通常描画
	frameBuffer->Clear(gfx->GetBgColor());
	frameBuffer->Activate();
	{
		StageManager::Instance().Render();

		testStatic->Render();
		testAnimated->Render();
	}
	frameBuffer->DeActivate();

	// ブルーム処理
	bloom->Make(frameBuffer->shaderResourceViews[0].Get());

	bitBlockTransfer->blit(bloom->GetSrvAddress(), 0, 1, nullptr, nullptr);

	// ブルームなし
	sprTest->Render();
	sprTest2->Render();
	sprTest3->Render();

#if USE_IMGUI
	// --- デバッグ描画 ---
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

// デバッグ描画
void SceneTest::DrawDebugGUI()
{
	// メニューバー描画
	DrawMenuBar();
}
