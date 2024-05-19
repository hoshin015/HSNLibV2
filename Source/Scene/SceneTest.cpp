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
	// �J���������ݒ�
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 20, 20),		// �J�������W
		DirectX::XMFLOAT3(0, 0, 0),			// �^�[�Q�b�g(�ݒ肵�Ă��Ӗ��Ȃ�)
		DirectX::XMFLOAT3(0, 1, 0)			// ������x�N�g��
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(45), DirectX::XMConvertToRadians(180), 0 });
	Camera::Instance().cameraType = Camera::CAMERA::FREE;

#if 1
	// ���C�g�����ݒ�
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({ 0.2f, 0.2f, 0.2f, 1.0f });
#else
	// �_�����ǉ�
	Light* light = new Light(LightType::Point);
	light->SetPosition({ 5, 5, 5 });
	light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	light->SetRange(30.0f);
	LightManager::Instance().Register(light);

	LightManager::Instance().SetAmbientColor({ 0.1f,0.1f,0.1f,1.0f });
#endif


	// �X�e�[�W������
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
	// --- imguiManager���� ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager���� ---
	InputManager::Instance().Update();


	// --- effectManager���� ---
	EffectManager::Instance().Update();

	// --- �J�������� ---
	Camera::Instance().Update();



	// �X�e�[�W�X�V
	StageManager::Instance().Update();

	testStatic->Update();
	testAnimated->Update();

	sprTest->SetAngle(sprTest->GetAngle() + 180 * Timer::Instance().DeltaTime());
	sprTest->UpdateAnimation();

	sprTest3->SetAngle(sprTest->GetAngle() + 180 * Timer::Instance().DeltaTime());

}

void SceneTest::Render()
{
	// �K�v�ȃ|�C���^�擾
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// renderTarget�̐ݒ�
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// imGuiBuffer���g�p���Ȃ��ꍇ�͂��������L�q����
	{
		// renderTarget�̃N���A
		dc->ClearRenderTargetView(gfx->GetRTV(), gfx->GetBgColor());
		// depthStencilView�̃N���A
		dc->ClearDepthStencilView(gfx->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	// shadowMap
	{
		// �V���h�E�}�b�v�����G���A��`
		float splitAreaTable[] =
		{
			Camera::Instance().GetNearZ(),
			100.0f,
			300.0f,
			500.0f,
			Camera::Instance().GetFarZ()
		};
		// �J�����̃p�����[�^�擾
		DirectX::XMVECTOR cameraFront = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetFront()));
		DirectX::XMVECTOR cameraRight = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetRight()));
		DirectX::XMVECTOR cameraUp = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetUp()));
		DirectX::XMVECTOR cameraPos = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Camera::Instance().GetEye()));

		// �萔�o�b�t�@�X�V
		shadow->UpdateConstants();	// �����Œʏ�`��Ŏg�p����萔���ꏏ�ɍX�V���Ă���
		shadow->SetShadowShader();

		// ���s��������J�����ʒu���쐬���A�������猴�_�̈ʒu������悤�Ɏ����s��𐶐�
		DirectX::XMFLOAT3 dir = LightManager::Instance().GetLight(0)->GetDirection();
		DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&dir);
		LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);
		DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(
			LightPosition,
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		);

		// �V���h�E�}�b�v�ɕ`�悵�����͈͂̎ˉe�s��𐶐�
		DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(1000, 1000, 0.1f, 1000.0f);
		DirectX::XMMATRIX viewProjection = V * P;
		///DirectX::XMStoreFloat4x4(&shadow->shadowConstants.lightViewProjection, viewProjection);	// �r���[�@�v���W�F�N�V�����@�ϊ��s����܂Ƃ߂�

		shadow->Clear();	// ��ʃN���A
		for (int i = 0; i < SHADOWMAP_COUNT; i++)
		{
			float nearDepth = splitAreaTable[i + 0];
			float farDepth = splitAreaTable[i + 1];

			shadow->Activate(i);

			// �G���A�����鎋�����8���_���Z�o����
			DirectX::XMVECTOR vertex[8];
			{
				// �G���A�ߕ��ʂ̒��S�����ʂ܂ł̋��������߂�
				float nearY = tanf(Camera::Instance().GetFovY() / 2.0f) * nearDepth;
				// �G���A�ߕ��ʂ̒��S����E�ʂ܂ł̋��������߂�
				float nearX = nearY * Camera::Instance().GetAspect();
				// �G���A�����ʂ̒��S�����ʂ܂ł̋��������߂�
				float farY = tanf(Camera::Instance().GetFovY() / 2.0f) * farDepth;
				// �G���A�����ʂ̒��S����E�ʂ܂ł̋��������߂�
				float farX = farY * Camera::Instance().GetAspect();

				// �G���A�ߕ��ʂ̒��S���W�����߂�
				DirectX::XMVECTOR nearPos = cameraPos + cameraFront * nearDepth;
				// �G���A�����ʂ̒��S���W�����߂�
				DirectX::XMVECTOR farPos = cameraPos + cameraFront * farDepth;

				// 8���_�����߂�
				{
					// �ߕ��ʂ̉E��
					vertex[0] = nearPos + cameraUp * nearY + cameraRight * nearX;
					// �ߕ��ʂ̍���
					vertex[1] = nearPos + cameraUp * nearY - cameraRight * nearX;
					// �ߕ��ʂ̉E��
					vertex[2] = nearPos - cameraUp * nearY + cameraRight * nearX;
					// �ߕ��ʂ̍���
					vertex[3] = nearPos - cameraUp * nearY - cameraRight * nearX;
					// �����ʂ̉E��
					vertex[4] = farPos + cameraUp * farY + cameraRight * farX;
					// �����ʂ̍���
					vertex[5] = farPos + cameraUp * farY - cameraRight * farX;
					// �����ʂ̉E��
					vertex[6] = farPos - cameraUp * farY + cameraRight * farX;
					// �����ʂ̍���
					vertex[7] = farPos - cameraUp * farY - cameraRight * farX;
				}
			}
			// 8�_�� LightViewProjection ��Ԃɕϊ����āA�ő�l�A�ŏ��l�����߂�
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

			// �N���b�v�s������߂�
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

			// lightViewProjection �s��ɃN���b�v�s�����Z
			DirectX::XMStoreFloat4x4(&shadow->shadowConstants.lightViewProjection, viewProjection * ClopMatrix);
			dc->UpdateSubresource(shadow->shadowConstant.Get(), 0, 0, &shadow->shadowConstants, 0, 0);
			dc->VSSetConstantBuffers(_shadowConstant, 1, shadow->shadowConstant.GetAddressOf());
			dc->PSSetConstantBuffers(_shadowConstant, 1, shadow->shadowConstant.GetAddressOf());

			// �e��t���������f���͂����ŕ`����s��(Render �̈����� true �������)
			{
				StageManager::Instance().Render(true);
				//testStatic->Render(true);
				testAnimated->Render(true);
			}
			shadow->DeActivate();
			shadow->SetShadowTexture();
		}
	}

	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);


	// �J�����̒萔�o�b�t�@�̍X�V
	Camera::Instance().UpdateCameraConstant();
	// ���C�g�̒萔�o�b�t�@�̍X�V
	LightManager::Instance().UpdateConstants();

	// �ʏ�`��
	frameBuffer->Clear(gfx->GetBgColor());
	frameBuffer->Activate();
	{
		StageManager::Instance().Render();

		testStatic->Render();
		testAnimated->Render();
	}
	frameBuffer->DeActivate();

	// �u���[������
	bloom->Make(frameBuffer->shaderResourceViews[0].Get());

	bitBlockTransfer->blit(bloom->GetSrvAddress(), 0, 1, nullptr, nullptr);

	// �u���[���Ȃ�
	sprTest->Render();
	sprTest2->Render();
	sprTest3->Render();

#if USE_IMGUI
	// --- �f�o�b�O�`�� ---
	DrawDebugGUI();

	LightManager::Instance().DrawDebugGui();
	bloom->DrawDebugGui();
	shadow->DrawDebugGui();

#if SHOW_PERFORMANCE
	// --- �p�t�H�[�}���X�`�� ---
	ImGuiManager::Instance().DisplayPerformanceStats();
#endif

	// --- imguiManager �`�� ---
	ImGuiManager::Instance().Render();
#endif
}

// �f�o�b�O�`��
void SceneTest::DrawDebugGUI()
{
	// ���j���[�o�[�`��
	DrawMenuBar();
}
