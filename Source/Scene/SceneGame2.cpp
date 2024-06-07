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
	// �_�����ǉ�
	Light* light = new Light(LightType::Point);
	light->SetPosition({ 5, 5, 5 });
	light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	light->SetRange(3000.0f);
	LightManager::Instance().Register(light);

	LightManager::Instance().SetAmbientColor({ 0.01f,0.01f,0.01f,1.0f });
#endif
	// �X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new StageMain("Data/Fbx/stage/stage.model");
	stageManager.Register(stageMain);
	stageMain->SetScale(DirectX::XMFLOAT3{ stageScale, stageScale, stageScale });

	bitBlockTransfer = std::make_unique<FullScreenQuad>();
	frameBuffer = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF(), true);
	bloom = std::make_unique<Bloom>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF());
	shadow = std::make_unique<Shadow>();

	// --- �p�[�e�B�N�������� ---
	Particle::Instance().Initialize();

	// --- �G�~�b�^�[�o�^ ----
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


	//�v���C���[������
	PlayerManager& playerManager = PlayerManager::Instance();
	PlayerManager::Instance().Initialize();
	/*Player* player1 = new Player("Data/Fbx/Player_02/Player_02.model", false);
	player1->SetPos({ 0,0,0 });
	playerManager.Register(player1);

	Player* player2 = new Player("Data/Fbx/Player_02/Player_02.model", true);
	player2->SetPos({ 50.0f,0,0 });
	playerManager.Register(player2);*/


	//���݂���I�u�W�F�N�g���`����
	objects.insert(std::make_pair(eObjectType::Kesigomu, std::make_unique<Object3D>("Data/Fbx/Kesigomu/Kesigomu.fbx", eObjectType::Kesigomu)));
	objects.insert(std::make_pair(eObjectType::Pentate, std::make_unique<Object3D>("Data/Fbx/Pentate/Pentate.fbx", eObjectType::Pentate)));
	objects.insert(std::make_pair(eObjectType::Enpitu, std::make_unique<Object3D>("Data/Fbx/Enpitu/Enpitu.fbx", eObjectType::Enpitu)));
	objects.insert(std::make_pair(eObjectType::Tokei, std::make_unique<Object3D>("Data/Fbx/Tokei/Tokei.fbx", eObjectType::Tokei)));
	objects.insert(std::make_pair(eObjectType::Kikyapu, std::make_unique<Object3D>("Data/Fbx/Kikyapu/Kikyapu.fbx", eObjectType::Kikyapu)));
	objects.insert(std::make_pair(eObjectType::Kuripu, std::make_unique<Object3D>("Data/Fbx/Kuripu/Kuripu.fbx", eObjectType::Kuripu)));
	objects.insert(std::make_pair(eObjectType::Sunatokei, std::make_unique<Object3D>("Data/Fbx/Sunatokei/Sunatokei.fbx", eObjectType::Sunatokei)));
	objects.insert(std::make_pair(eObjectType::Goal, std::make_unique<Object3D>("Data/Fbx/Goal/Stage.fbx", eObjectType::Goal)));

	//�S�[��
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

	//�U�����~�߂�
	if (InputManager::Instance().IsGamePadConnected())
		InputManager::Instance().SetVibration(0, 0.0f, 0.0f);


	EmitterManager::Instance().Clear();
}

void SceneGame2::Update()
{

#if USE_IMGUI
	// --- imguiManager���� ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager���� ---
	InputManager::Instance().Update();

	// --- effectManager���� ---
	EffectManager::Instance().Update();

	

	// �_�������W�X�V
	DirectX::XMFLOAT3 pointLPos = PlayerManager::Instance().GetPositionCenter();
	pointLPos.y += 100;
	pointLPos.z += 100;
	LightManager::Instance().GetLight(0)->SetPosition(pointLPos);

	// --- �J�������� ---
	CameraUpdate();

	if(isFinishCamera)
		Score::Instance().Update();

	// �^�C�}�[�̒萔�o�b�t�@�̍X�V
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

void SceneGame2::Render()
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

#if 01
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

void SceneGame2::StageVsRope()
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
	//�J�����̏����ʒu
	const float INITIAL_CAMERA_Z = -8000 * stageScale;
	const float INITIAL_CAMERA_Y = 150;
	//�J�����̃v���C���̊p�x
	const float PLAING_ANGLE = 30.0f;
	//���`�⊮�̌W��
	const float FACTOR_Y = 0.1f;
	const float FACTOR_Z = 0.01f;
	//�J�����������痈�����ɏI���������
	const float FINISH_LENGTH = 0.5f;

	//�ʏ�̃J�����̃^�[�Q�b�g
	const DirectX::XMFLOAT3 CAMERA_TARGET = PlayerManager::Instance().GetPositionCenter() + cameraOffset;
	//���o�̎��Ɏg����ԏ��߂̃^�[�Q�b�g
	const DirectX::XMFLOAT3 FIRST_TARGET = { CAMERA_TARGET.x,CAMERA_TARGET.y + INITIAL_CAMERA_Y,CAMERA_TARGET.z };

	const DirectX::XMFLOAT3 OLD_CAMERA_TARGET = cameraTarget;

	switch (cameraState)
	{
	case 0:
		//�����ݒ�
		//�v���C���[�������Ȃ��悤�ɂ���
		PlayerManager::Instance().SetInputPlayerMove(false);
		PlayerManager::Instance().SetIsMoveZ(false);
		PlayerManager::Instance().SetIsUpdateZ(false);

		//�J�����̃^�[�Q�b�g�̏����l��ݒ�
		cameraTarget = CAMERA_TARGET;
		//�J�������{������΂߉��ɂ���悤�ɂ���
		cameraTarget.z += INITIAL_CAMERA_Z;
		cameraTarget.y += INITIAL_CAMERA_Y;

		Camera::Instance().SetTarget(cameraTarget);
		cameraState++;

		//�����ݒ�Ȃ̂ł��̂܂ܐi�߂�
		/*fall through*/

	case 1:
		//�J�������������O�ɗ���
		LerpCameraTarget(FIRST_TARGET, FACTOR_Z);

		Camera::Instance().Update();

		//�O��̈ړ��������Ƃ̍������ȉ���������I��
		if (cameraTarget.z - OLD_CAMERA_TARGET.z > -0.1f &&
			cameraTarget.z - OLD_CAMERA_TARGET.z < 0.1f)
			cameraState++;

		break;

	case 2:
		//�J�������ォ��{���̈ʒu�֗���
		LerpCameraTarget(CAMERA_TARGET, FACTOR_Y);
		//�J�����̃A���O����ύX
		cameraAngle = cameraAngle + (FACTOR_Y / 2.0f) * (PLAING_ANGLE - cameraAngle);

		//�ݒ�
		Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(cameraAngle),DirectX::XMConvertToRadians(180), 0 });
		Camera::Instance().Update();

		//�O��̈ړ��������Ƃ̍������ȉ���������I��
		if (cameraTarget.y - OLD_CAMERA_TARGET.y > -0.1f &&
			cameraTarget.y - OLD_CAMERA_TARGET.y < 0.1f)
			cameraState++;

		break;

	case 3:
		//�v���C���[�������悤�ɂ���
		PlayerManager::Instance().SetInputPlayerMove(true);
		PlayerManager::Instance().SetIsMoveZ(true);
		PlayerManager::Instance().SetIsUpdateZ(true);
		isFinishCamera = true;
		cameraState++;
		break;

	default:
		//�ʏ폈��
		cameraTarget = CAMERA_TARGET;
		Camera::Instance().SetTarget(cameraTarget);
		Camera::Instance().Update();

		break;
	}

#elif 0
	//�v���C���[�̏����p�x
	const float PLAYER_ANGLE = 90;
	cameraTimer += Timer::Instance().DeltaTime();
	float angle = (PLAYER_ANGLE + cameraTimer * 360 / CAMERA_LAPTIME) * DirectX::XM_PI / 180.0f;

	if (cameraTimer < CAMERA_LAPTIME)
	{
		//�J�������������1������
		CameraRendition(PlayerManager::Instance().GetPositionCenter(), 500, 500, angle);
		//�v���C���[�������Ȃ��悤�ɂ���
		PlayerManager::Instance().SetInputPlayerMove(false);
		PlayerManager::Instance().SetIsMoveZ(false);
		PlayerManager::Instance().SetIsUpdateZ(false);
	}
	else
	{
		PlayerManager::Instance().SetInputPlayerMove(true);
		PlayerManager::Instance().SetIsMoveZ(true);
		PlayerManager::Instance().SetIsUpdateZ(true);
		// --- �J�������� ---
		DirectX::XMFLOAT3 cameraTarget = PlayerManager::Instance().GetPositionCenter();
		cameraTarget += cameraOffset;
		Camera::Instance().SetTarget(cameraTarget);
		Camera::Instance().Update();
	}
#elif 0
	// --- �J�������� ---
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
			//�S�[����z���W
			float goleZ = object.second->GetTransform(i).pos.z;

			//�v���C���[�̗������S�[���𒴂�����N���A
			if (manager.GetPlayer().at(0)->GetPosZ() < goleZ &&
				manager.GetPlayer().at(1)->GetPosZ() < goleZ)
			{
				SceneManager::Instance().ChangeScene(new SceneClear);
			}
		}
	}
}
