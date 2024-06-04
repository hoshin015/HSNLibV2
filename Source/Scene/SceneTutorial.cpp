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

	//�v���C���[������
	PlayerManager& playerManager = PlayerManager::Instance();
	PlayerManager::Instance().Initialize();
	Player* player1 = new Player("Data/Fbx/Player_02/Player_02.model", false);
	player1->SetPos({ 0,0,0 });
	playerManager.Register(player1);

	Player* player2 = new Player("Data/Fbx/Player_02/Player_02.model", true);
	player2->SetPos({ 50.0f,0,0 });
	playerManager.Register(player2);

	//�X�e�[�W�����[�h����
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

	UpdateTutorial();

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

	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);

	StageManager::Instance().Render();

	//�v���C���[�̕`��
	PlayerManager::Instance().Render();
	//�X�e�[�W�`��
	for (auto& object : objects) {
		object.second->Render();
	}

	//���̕`��
	LineRenderer::Instance().Render();
	DebugPrimitive::Instance().Render();

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
}

void SceneTutorial::StageCollision()
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

void SceneTutorial::StageVsRope()
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

void SceneTutorial::UpdateTutorial()
{
	PlayerManager& players = PlayerManager::Instance();

	//�e�L�X�g���X�V����
	if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter) && updateText)
		textState++;

	switch (tutorialState)
	{
	case 0:
		//�n�߂̐���
	{
		//�v���C���[�����͂��󂯓���Ȃ��悤�ɂ���
		players.SetInputPlayerMove(false);
		//�v���C���[��Z�����֏���ɓ����Ȃ��悤�ɂ��A���x���ω����Ȃ��悤�ɂ���
		players.SetIsMoveZ(false);
		players.SetIsUpdateZ(false);

		if (textState >= EXPLAIN::MOVE_Z)
			tutorialState++;
		break;
	}

	case 1:
		//�v���C���[�����ɍs�������邱�Ƃ̐���������
	{
		players.SetIsMoveZ(true);
		players.SetIsUpdateZ(true);

		if (textState >= EXPLAIN::INPUT_MOVE)
			tutorialState++;

		break;
	}

	case 2:
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
		if (moveTimer >= 3.0f)
		{
			//�e�L�X�g��i�߂�
			textState++;
			updateText = true;
			//�^�C�}�[���O�ɂ���
			moveTimer = 0;
			tutorialState++;
		}

		break;
	}

	case 3:
	//�ړ������I�����̏���
	{
		//�v���C���[�����͂��󂯓���Ȃ��悤�ɂ���
		players.SetInputPlayerMove(false);
		//�v���C���[��Z�����֏���ɓ����Ȃ��悤�ɂ��A���x���ω����Ȃ��悤�ɂ���
		players.SetIsMoveZ(false);
		players.SetIsUpdateZ(false);

		if (textState >= LENGTH_STICK)
			tutorialState++;
		break;
	}

	case 4:
	//�_�̒����ɂ���ăv���C���[�̑��x���ω����邱�Ƃ��������
	{
		//�v���C���[�����͂��󂯓����悤�ɕύX
		players.SetInputPlayerMove(true);
		players.SetIsMoveZ(true);
		players.SetIsUpdateZ(true);
		//�e�L�X�g�̍X�V���~�߂�
		updateText = false;

		if (players.GetAccelerationZ() > 11.0f)
		{
			//�e�L�X�g��i�߂�
			textState++;
			updateText = true;
			//���̃X�e�[�g�֐i��
			tutorialState++;
		}

		break;
	}

	case 5:
	//�v���C���[���x�ω��̐����I������
	{
		//�v���C���[�����͂��󂯓���Ȃ��悤�ɂ���
		players.SetInputPlayerMove(false);
		//�v���C���[��Z�����֏���ɓ����Ȃ��悤�ɂ��A���x���ω����Ȃ��悤�ɂ���
		players.SetIsMoveZ(false);
		players.SetIsUpdateZ(false);

		if (textState >= EXPLAIN::MAXLENGTH)
			tutorialState++;
		break;
	}

	case 6:
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
			tutorialState++;
		}

		break;
	}

	case 7:
	//�_�̍ő�l�𒴂������̐����I������
	{
		//�v���C���[�����͂��󂯓���Ȃ��悤�ɂ���
		players.SetInputPlayerMove(false);
		//�v���C���[��Z�����֏���ɓ����Ȃ��悤�ɂ��A���x���ω����Ȃ��悤�ɂ���
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

	//�ʂŐ������o��
	if (textState == EXPLAIN::INPUT_MOVE)
	{
		text.Draw(L"�FWASD�A�ԁF�\���L�[", ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
	if (textState == EXPLAIN::LENGTH_STICK)
	{
		text.Draw(L"�_��L�΂��āA���x���オ�邱�Ƃ��m�F����", ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
	if (textState == EXPLAIN::MAXLENGTH)
	{
		text.Draw(L"�_�̒����̌��E�𒴂��Ă݂�", ANOTHER_TEXTPOS, ANOTHER_TEXTSIZE, TEXT_ALIGN::MIDDLE);
	}
	/*switch (textState)
	{
	case 0:
		break;

	case 1:
		text.Draw(L"���̃Q�[���ɂ��Đ��������I", textPosition, textSize, TEXT_ALIGN::MIDDLE);
		break;

	case 2:
		text.Draw(L"�܂��́A�v���C���[�ɂ��Đ��������" , textPosition, textSize, TEXT_ALIGN::MIDDLE);
		break
	
	;case 3:
		text.Draw(L"�܂��́A�v���C���[�ɂ��Đ��������" , textPosition, textSize, TEXT_ALIGN::MIDDLE);
		break;
	
	}*/
}
