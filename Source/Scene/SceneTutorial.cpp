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
	// �J���������ݒ�
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 20, 20),		// �J�������W
		DirectX::XMFLOAT3(0, 0, 0),			// �^�[�Q�b�g(�ݒ肵�Ă��Ӗ��Ȃ�)
		DirectX::XMFLOAT3(0, 1, 0)			// ������x�N�g��
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(cameraAngle), DirectX::XMConvertToRadians(180), 0 });
	Camera::Instance().cameraType = Camera::CAMERA::TARGET_PLAYER;

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
	StageMain* stageMain = new StageMain("Data/Fbx/stage/stage.model");
	stageManager.Register(stageMain);
	stageMain->SetScale(stageScale);

	bitBlockTransfer = std::make_unique<FullScreenQuad>();
	frameBuffer = std::make_unique<FrameBuffer>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF(), true);
	bloom = std::make_unique<Bloom>(Framework::Instance().GetScreenWidthF(), Framework::Instance().GetScreenHeightF());
	shadow = std::make_unique<Shadow>();


	waterBase = std::make_unique<TestAnimated>("Data/Fbx/WaterBase/WaterBase.model");
	waterBase->SetPosY(-100);

	//�v���C���[������
	PlayerManager& playerManager = PlayerManager::Instance();
	playerManager.Initialize();
	/*Player* player1 = new Player("Data/Fbx/Player_02/Player_02.model", false);
	player1->SetPos({ 0,0,0 });
	playerManager.Register(player1);

	Player* player2 = new Player("Data/Fbx/Player_02/Player_02.model", true);
	player2->SetPos({ 50.0f,0,0 });
	playerManager.Register(player2);*/

	//�v���~�e�B�u�̏�����
	primitive = std::make_unique<Primitive2D>();

	//�X�e�[�W�����[�h����
	//���݂���I�u�W�F�N�g���`����
	objects.insert(std::make_pair(eObjectType::Kesigomu, std::make_unique<Object3D>("Data/Fbx/Kesigomu/Kesigomu.model", eObjectType::Kesigomu)));
	objects.insert(std::make_pair(eObjectType::Pentate, std::make_unique<Object3D>("Data/Fbx/Pentate/Pentate.model", eObjectType::Pentate)));
	objects.insert(std::make_pair(eObjectType::Enpitu, std::make_unique<Object3D>("Data/Fbx/Enpitu/Enpitu.model", eObjectType::Enpitu)));
	objects.insert(std::make_pair(eObjectType::Tokei, std::make_unique<Object3D>("Data/Fbx/Tokei/Tokei.model", eObjectType::Tokei)));
	objects.insert(std::make_pair(eObjectType::Kikyapu, std::make_unique<Object3D>("Data/Fbx/Kikyapu/Kikyapu.model", eObjectType::Kikyapu)));
	objects.insert(std::make_pair(eObjectType::Kuripu, std::make_unique<Object3D>("Data/Fbx/Kuripu/Kuripu.model", eObjectType::Kuripu)));
	objects.insert(std::make_pair(eObjectType::Sunatokei, std::make_unique<Object3D>("Data/Fbx/Sunatokei/Sunatokei.model", eObjectType::Sunatokei)));

	//�v�f�̏�����
	tutorialState = 0;
	nextState = 0;
	textState = 0;
}

void SceneTutorial::Finalize()
{
	LightManager::Instance().Clear();

	StageManager::Instance().Clear();

	PlayerManager::Instance().Clear();

	//�U�����~�߂�
	if (InputManager::Instance().IsGamePadConnected())
		InputManager::Instance().SetVibration(0, 0.0f, 0.0f);
}

void SceneTutorial::Update()
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


	// �^�C�}�[�̒萔�o�b�t�@�̍X�V
	UpdateTimerConstnat();

	//�`���[�g���A���̃A�b�v�f�[�g
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

		//Particle::Instance().Render();

		// rasterizerState�̐ݒ�
		gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
		// depthStencilState�̐ݒ�
		gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_OFF);
		// blendState�̐ݒ�
		gfx->SetBlend(BLEND_STATE::ALPHA);

		waterBase->Render();
	}
	frameBuffer->DeActivate();

#if 0
	// �u���[���������Ă̕`��
	bloom->Make(frameBuffer->shaderResourceViews[0].Get());
	bitBlockTransfer->blit(bloom->GetSrvAddress(), 0, 1, nullptr, nullptr);
#else
	// ���̂܂ܕ`��
	bitBlockTransfer->blit(frameBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1, nullptr, nullptr);
#endif	

	// �u���[���Ȃ�
	//sprTest->Render();
	//sprTest2->Render();
	//sprTest3->Render();

	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_OFF);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);

	DrawTutorialText();

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
				hit = true;
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

	return hit;
}

bool SceneTutorial::StageVsRope()
{
	//�v���C���[���擾
	std::vector<Player*> players = PlayerManager::Instance().GetPlayer();
	bool hit = false;
	
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

			hit = true;
		}
	}

	return hit;
}

void SceneTutorial::UpdateTutorial()
{
	PlayerManager& players = PlayerManager::Instance();
	InputManager& input = InputManager::Instance();

	//�e�L�X�g���X�V����
	
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
		//�n�߂̐���
	{
		//�v���C���[�����͂��󂯓���Ȃ��悤�ɂ���
		players.SetInputPlayerMove(false);
		//�v���C���[��Z�����֏���ɓ����Ȃ��悤�ɂ��A���x���ω����Ȃ��悤�ɂ���
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
		//�v���C���[�����ɍs�������邱�Ƃ̐���������
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
		//�v���C���[�̑���ɂ��Đ�������
	{
		//�v���C���[�����͂��󂯓����悤�ɕύX
		players.SetInputPlayerMove(true);
		updateText = false;

		//���������Ԃ��v��
		if (players.GetDoMove())
		{
			moveTimer += Timer::Instance().DeltaTime();
		}

		//�K�莞�Ԃ��߂�����
		if (moveTimer >= 2.5f)
		{
			//�e�L�X�g��i�߂�
			textState++;
			updateText = true;
			//�^�C�}�[���O�ɂ���
			moveTimer = 0;
			tutorialState = EXPLAIN_STATE::READYING;
			//nextState++;
		}

		break;
	}

	case LENGTH_STICK:
		//�_�̒����ɂ���ăv���C���[�̑��x���ω����邱�Ƃ��������
	{
		//�v���C���[�����͂��󂯓����悤�ɕύX
		players.SetInputPlayerMove(true);
		players.SetIsMoveZ(true);
		players.SetIsUpdateZ(true);
		//�e�L�X�g�̍X�V���~�߂�
		updateText = false;

		if (players.GetAccelerationZ() > 23.0f)
		{
			//�e�L�X�g��i�߂�
			textState++;
			updateText = true;
			//���̃X�e�[�g�֐i��
			tutorialState = EXPLAIN_STATE::READYING;
			//nextState++;
		}

		break;
	}

	case MAXLENGTH:
		//�_�̒������ő�l�𒴂����ꍇ�̐���
	{
		//�v���C���[�����͂��󂯓����悤�ɕύX
		players.SetInputPlayerMove(true);
		players.SetIsMoveZ(true);
		players.SetIsUpdateZ(true);
		//�e�L�X�g�̍X�V���~�߂�
		updateText = false;

		if (players.IsRopeOverLength())
		{
			//�e�L�X�g��i�߂�
			textState++;
			updateText = true;
			//���̃X�e�[�g�֐i��
			tutorialState = EXPLAIN_STATE::READYING;
			//nextState++;
		}

		break;
	}

	case OBJECT_READY:
		//��Q���̐��������邽�߂̏���
	{
		//�v���C���[�̍��W�������l�Ɉړ�����
		players.ResetToInterval();

		//��Q����������邽�߂̃X�e�[�W�����[�h����
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
	//�I�u�W�F�N�g�ɓ����������̏���
	{
		players.SetIsMoveZ(true);
		players.SetIsUpdateZ(true);
		//�e�L�X�g�̍X�V���~�߂�
		updateText = false;

		if (StageCollision())
		{
			//�e�L�X�g��i�߂�
			textState++;
			updateText = true;
			//���̃X�e�[�g�֐i��
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
			//�e�L�X�g��i�߂�
			textState++;
			updateText = true;
			//���̃X�e�[�g�֐i��
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
	//�e�L�X�g��\�����钆���l
	DirectX::XMFLOAT2 TEXTPOSITION = { 640, 660 };
	constexpr DirectX::XMFLOAT2 TEXT_BOX_POS = { 0,600 };
	constexpr DirectX::XMFLOAT2 TEXT_BOX_SIZE = { 1280,720 };
	
	constexpr DirectX::XMFLOAT2 ANOTHER_TEXTPOS = { 640,60 };
	constexpr DirectX::XMFLOAT2 ANOTHER_BOX_POS = { 0,0 };
	constexpr DirectX::XMFLOAT2 ANOTHER_BOX_SIZE = { 1280,100 };
	constexpr float ANOTHER_TEXTSIZE = 50;

	if (updateText)
	{
		//�e�L�X�g�����₷���悤�Ɍ����Â�����
		primitive->Render(TEXT_BOX_POS.x, TEXT_BOX_POS.y, TEXT_BOX_SIZE.x, TEXT_BOX_SIZE.y, 0.f, 0.f, 0.f, 0.5f, 0);
		//�e�L�X�g��\��
		text.Draw(TEXT_MESSEAGE[textState].text.c_str(), TEXTPOSITION, textSize, TEXT_ALIGN::MIDDLE);
	}

	//�ʂŐ������o��
	if (tutorialState == EXPLAIN_STATE::INPUT_MOVE)
	{
		primitive->Render(ANOTHER_BOX_POS.x, ANOTHER_BOX_POS.y, ANOTHER_BOX_SIZE.x, ANOTHER_BOX_SIZE.y, 0.f, 0.f, 0.f, 0.5f, 0);
		//�R���g���[���[���ڑ�����Ă��邩�ǂ����ŏo���e�L�X�g��ς���
		std::wstring t;
		if (InputManager::Instance().IsGamePadConnected())
			t = L"�F�E�X�e�B�b�N�A�ԁF���X�e�B�b�N";
		else
			t = L"�FWASD�A�ԁF�\���L�[";
		text.Draw(t.c_str(), ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
	if (tutorialState == EXPLAIN_STATE::LENGTH_STICK)
	{
		primitive->Render(ANOTHER_BOX_POS.x, ANOTHER_BOX_POS.y, ANOTHER_BOX_SIZE.x, ANOTHER_BOX_SIZE.y, 0.f, 0.f, 0.f, 0.5f, 0);
		text.Draw(L"�_��L�΂��āA���x���オ�邱�Ƃ��m�F����", ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
	if (tutorialState == EXPLAIN_STATE::MAXLENGTH)
	{
		primitive->Render(ANOTHER_BOX_POS.x, ANOTHER_BOX_POS.y, ANOTHER_BOX_SIZE.x, ANOTHER_BOX_SIZE.y, 0.f, 0.f, 0.f, 0.5f, 0);
		text.Draw(L"�_�̒����̌��E�𒴂��Ă݂�", ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
	if(tutorialState == EXPLAIN_STATE::HITOBJECT_ROPE)
	{
		primitive->Render(ANOTHER_BOX_POS.x, ANOTHER_BOX_POS.y, ANOTHER_BOX_SIZE.x, ANOTHER_BOX_SIZE.y, 0.f, 0.f, 0.f, 0.5f, 0);
		text.Draw(L"�_�ɏ�Q���𓖂Ă悤", ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
}
