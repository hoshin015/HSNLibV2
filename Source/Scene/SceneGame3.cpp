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
#include "SceneGame3.h"
#include "SceneManager.h"
// --- Game ---
#include "../Game/Object/Stage/StageManager.h"
#include "../Game/Object/Stage/StageMain.h"
#include "../../Library/3D/DebugPrimitive.h"

#include "../Game/Object/Player/PlayerManager.h"
#include "../../Library/Particle/EmitterManager.h"
void SceneGame3::Initialize()
{
	// �J���������ݒ�
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 20, 20),		// �J�������W
		DirectX::XMFLOAT3(0, 0, 0),			// �^�[�Q�b�g(�ݒ肵�Ă��Ӗ��Ȃ�)
		DirectX::XMFLOAT3(0, 1, 0)			// ������x�N�g��
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(cameraAngle), DirectX::XMConvertToRadians(180), 0 });
	Camera::Instance().cameraType = Camera::CAMERA::TARGET_PLAYER;

#if 0
	// ���C�g�����ݒ�
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({ 0.2f, 0.2f, 0.2f, 1.0f });
#else
	// �X�|�b�g�����ǉ�
	Light* light = new Light(LightType::Spot);
	light->SetPosition({ 5, 5, 5 });
	light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	light->SetRange(5000.0f);
	light->SetDirection({0, -0.25, -1});
	light->SetOuterCorn(0.95);
	light->SetInnerCorn(0.99);
	LightManager::Instance().Register(light);

	Light* light2 = new Light(LightType::Spot);
	light2->SetPosition({ 5, 5, 5 });
	light2->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	light2->SetRange(5000.0f);
	light2->SetDirection({ 0, -0.25, -1 });
	light->SetOuterCorn(0.95);
	light->SetInnerCorn(0.99);
	LightManager::Instance().Register(light2);

	// �_�����ǉ�
	Light* light3 = new Light(LightType::Point);
	light3->SetPosition({ 5, 5, 5 });
	light3->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	light3->SetRange(100.0f);
	LightManager::Instance().Register(light3);

	Light* light4 = new Light(LightType::Point);
	light4->SetPosition({ 5, 5, 5 });
	light4->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	light4->SetRange(100.0f);
	LightManager::Instance().Register(light4);

	LightManager::Instance().SetAmbientColor({ 0.01f,0.01f,0.01f,1.0f });
#endif
	// �X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain("Data/Fbx/stage/stage.model");
	stageManager.Register(stageMain);

	bitBlockTransfer = std::make_unique<FullScreenQuad>();
	frameBuffer = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF(), true);
	bloom = std::make_unique<Bloom>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF());
	shadow = std::make_unique<Shadow>();

	// --- �p�[�e�B�N�������� ---
	Particle::Instance().Initialize();

	// --- �G�~�b�^�[�o�^ ----
	Emitter* emitter0 = new Emitter();
	emitter0->position = { 0, 3, 3 };
	emitter0->rate = 20000;
	emitter0->duration = 1.1;
	emitter0->looping = false;
	emitter0->rateOverTime = 1;
	emitter0->startKind = 0;
	emitter0->startLifeTime = 1.0f;
	emitter0->startSize = 1.0f;
	emitter0->startColor = { 1.8,1.8,1.8,1 };
	EmitterManager::Instance().Register(emitter0);

	//�v���C���[������
	PlayerManager& playerManager = PlayerManager::Instance();
	PlayerManager::Instance().Initialize();
	/*Player* player1 = new Player("Data/Fbx/Player_02/Player_02.model", false);
	player1->SetPos({ 0,0,0 });
	playerManager.Register(player1);

	Player* player2 = new Player("Data/Fbx/Player_02/Player_02.model", true);
	player2->SetPos({ 50.0f,0,0 });
	playerManager.Register(player2);*/


	//�X�e�[�W�����[�h����
	objects.insert(std::make_pair(eObjectType::Kesigomu, std::make_unique<Object3D>("Data/Fbx/Kesigomu/Kesigomu.fbx", eObjectType::Kesigomu)));
	objects.insert(std::make_pair(eObjectType::Pentate, std::make_unique<Object3D>("Data/Fbx/Pentate/Pentate.fbx", eObjectType::Pentate)));
	objects.insert(std::make_pair(eObjectType::Enpitu, std::make_unique<Object3D>("Data/Fbx/Enpitu/Enpitu.fbx", eObjectType::Enpitu)));
	objects.insert(std::make_pair(eObjectType::Tokei, std::make_unique<Object3D>("Data/Fbx/Tokei/Tokei.fbx", eObjectType::Tokei)));
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
}

void SceneGame3::Finalize()
{
	LightManager::Instance().Clear();

	StageManager::Instance().Clear();

	PlayerManager::Instance().Clear();

	//�U�����~�߂�
	if (InputManager::Instance().IsGamePadConnected())
		InputManager::Instance().SetVibration(0, 0.0f, 0.0f);

	EmitterManager::Instance().Clear();
}

void SceneGame3::Update()
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
	DirectX::XMFLOAT3 cameraTarget = PlayerManager::Instance().GetPositionCenter();
	cameraTarget += cameraOffset;
	Camera::Instance().SetTarget(cameraTarget);
	Camera::Instance().Update();

	// �������W�X�V
	DirectX::XMFLOAT3 SpotLPos = PlayerManager::Instance().GetPlayer().at(0)->GetPos();
	SpotLPos.y += 50;
	LightManager::Instance().GetLight(0)->SetPosition(SpotLPos);

	DirectX::XMFLOAT3 SpotLPos2 = PlayerManager::Instance().GetPlayer().at(1)->GetPos();
	SpotLPos2.y += 50;
	LightManager::Instance().GetLight(1)->SetPosition(SpotLPos2);

	DirectX::XMFLOAT3 PointLPos3 = PlayerManager::Instance().GetPlayer().at(0)->GetPos();
	PointLPos3.y += 75;
	PointLPos3.z += 10;
	LightManager::Instance().GetLight(2)->SetPosition(PointLPos3);

	DirectX::XMFLOAT3 PointLPos4 = PlayerManager::Instance().GetPlayer().at(1)->GetPos();
	PointLPos4.y += 75;
	PointLPos4.z += 10;
	LightManager::Instance().GetLight(3)->SetPosition(PointLPos4);


	// �^�C�}�[�̒萔�o�b�t�@�̍X�V
	UpdateTimerConstnat();
	StageManager::Instance().Update();

	PlayerManager::Instance().Update();

	StageCollision();
	StageVsRope();

	//objects->Update();
	for (auto& object : objects) {
		object.second->Update();
	}

	// objects��tranform�����R�s�[
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

	// --- �p�[�e�B�N���X�V ---
	EmitterManager::Instance().Update();
	Particle::Instance().Update();
}

void SceneGame3::Render()
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

	// �J�����̒萔�o�b�t�@�̍X�V
	Camera::Instance().UpdateCameraConstant();
	// ���C�g�̒萔�o�b�t�@�̍X�V
	LightManager::Instance().UpdateConstants();


	// shadowMap
	{
		shadow->Clear();					// �V���h�E�}�b�v�N���A
		shadow->UpdateShadowCasterBegin();	// �V���h�E�}�b�v�`�揀��

		for (int i = 0; i < SHADOWMAP_COUNT; i++)
		{
			shadow->Activate(i);
			// �e��t���������f���͂����ŕ`����s��(Render �̈����� true �������)
			{
				// animated object
				shadow->SetAnimatedShader();
				StageManager::Instance().Render(true);
				PlayerManager::Instance().Render(true);
			
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

		// �ʏ�`��p�Ƀe�N�X�`���ƒ萔�o�b�t�@�X�V
		shadow->SetShadowTextureAndConstants();
	}

	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);

	// �ʏ�`��
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

		// rasterizerState�̐ݒ�
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
		// depthStencilState�̐ݒ�
		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_OFF);
		// blendState�̐ݒ�
		gfx->SetBlend(BLEND_STATE::ALPHA);

		Particle::Instance().Render();

		// rasterizerState�̐ݒ�
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
		// depthStencilState�̐ݒ�
		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_OFF);
		// blendState�̐ݒ�
		gfx->SetBlend(BLEND_STATE::ALPHA);
	}
	frameBuffer->DeActivate();

#if 1
	// �u���[���������Ă̕`��
	bloom->Make(frameBuffer->shaderResourceViews[0].Get());
	bitBlockTransfer->blit(bloom->GetSrvAddress(), 0, 1, nullptr, nullptr);
#else
	// ���̂܂ܕ`��
	bitBlockTransfer->blit(frameBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1, nullptr, nullptr);
#endif	

	// �u���[���Ȃ�

#if USE_IMGUI
	// --- �f�o�b�OGUI�`�� ---
	DrawDebugGUI();

	LightManager::Instance().DrawDebugGui();

#if SHOW_PERFORMANCE
	// --- �p�t�H�[�}���X�`�� ---
	ImGuiManager::Instance().DisplayPerformanceStats();
#endif

	// --- imguiManager �`�� ---
	ImGuiManager::Instance().Render();
#endif
}

void SceneGame3::DrawDebugGUI()
{
	PlayerManager::Instance().DrawDebugImGui();

	ImGui::Begin("Camera");

	ImGui::SliderFloat3("CameraOffset", &cameraOffset.x, 0, 500);
	ImGui::End();

	DrawMenuBar();
}

void SceneGame3::StageCollision()
{
	std::vector<Player*> players = PlayerManager::Instance().GetPlayer();

	for (Player* player : players)
	{
		for (const Object3D::Collision collision : collisions)
		{
			DirectX::XMFLOAT3 outPos;
			//�����蔻��
			if (Collision::IntersectSphereVsSphere(player->GetPos(), player->GetRadius(), collision.pos, collision.radius, outPos))
			{
				//�q�b�g���̏���
				player->HitModel(outPos, PlayerManager::Instance().GetHitPower(), PlayerManager::Instance().GetHitDownSpeed());

				//�R���g���[���[��U��������
				if (InputManager::Instance().IsGamePadConnected())
				{
					InputManager::Instance().SetVibration(0, 0.25f, 0.25f);
					//�U�������鎞�Ԃ�ݒ�
					controllerTimer = 1.0f;
				}
			}
			else
			{
				//�R���g���[���[�̐U�����~�߂�
				if (InputManager::Instance().IsGamePadConnected() && controllerTimer <= 0)
					InputManager::Instance().SetVibration(0, 0.0f, 0.0f);
			}
		}
	}

	//�R���g���[���[�̐U���̃^�C�}�[�����炷
	if (controllerTimer > 0)
		controllerTimer -= Timer::Instance().DeltaTime();
}

void SceneGame3::StageVsRope()
{
	//�v���C���[���擾
	std::vector<Player*> players = PlayerManager::Instance().GetPlayer();

	DirectX::XMFLOAT3 rayPos[2] = { {0,0,0},{0,0,0} };
	int i = 0;
	for (Player* player : players)
	{
		//�v���C���[�̎�ӂ�̍��W�����
		rayPos[i] = player->GetPos();
		rayPos[i].y += PlayerManager::Instance().GetRopeHeight();
		i++;
	}


	for (Object3D::Collision collision : collisions)
	{
		//�R�̏ꏊ��肻�̃R���W�����̍��������������continue
		if (collision.pos.y + collision.radius < rayPos[0].y && collision.pos.y + collision.radius < rayPos[1].y)
			continue;
		DirectX::XMFLOAT3 outPos;
		//�����蔻��
		if (PlayerManager::Instance().IntersectSphereVsLine(collision.pos, collision.radius, rayPos[0], rayPos[1]))
		{
			for (Player* player : PlayerManager::Instance().GetPlayer())
			{
				//���S����
				player->SetDeath();
			}
			SceneManager::Instance().ChangeScene(new SceneTitle);
		}
	}

}
