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
#include "SceneTest.h"
#include "SceneStage.h"
#include "SceneManager.h"
// --- Game ---
#include "../Game/Object/Stage/StageManager.h"
#include "../Game/Object/Stage/StageMain.h"
#include "../../Library/3D/DebugPrimitive.h"

#include "../Game/Object/Player/PlayerManager.h"


void SceneStage::Initialize()
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



	testStatic = std::make_unique<TestStatic>("Data/Fbx/Albino/Albino.model");
	testAnimated = std::make_unique<TestAnimated>("Data/Fbx/CatfishA/CatfishA.model");

	//objects = std::make_unique<Object3D>("Data/Fbx/Albino/Albino.model");
	//objects = std::make_unique<Object3D>("Data/Fbx/Kesigou/Kesigou.fbx");

	objects.insert(std::make_pair(eObjectType::Kesigomu, std::make_unique<Object3D>("Data/Fbx/Kesigomu/Kesigomu.fbx", eObjectType::Kesigomu)));
	objects.insert(std::make_pair(eObjectType::Pentate, std::make_unique<Object3D>("Data/Fbx/Pentate/Pentate.fbx", eObjectType::Pentate)));
	objects.insert(std::make_pair(eObjectType::Enpitu, std::make_unique<Object3D>("Data/Fbx/Enpitu/Enpitu.fbx", eObjectType::Enpitu)));
	objects.insert(std::make_pair(eObjectType::Tokei, std::make_unique<Object3D>("Data/Fbx/Tokei/Tokei.fbx", eObjectType::Tokei)));

	//�v���C���[������
	PlayerManager& playerManager = PlayerManager::Instance();
	Player* player1 = new Player("Data/Fbx/Player/Player.model", false);
	playerManager.Register(player1);
	Player* player2 = new Player("Data/Fbx/Player/Player.model", true);
	player2->SetPosX(50.0f);
	playerManager.Register(player2);
	
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
	// --- imguiManager���� ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager���� ---
	InputManager::Instance().Update();


	// --- effectManager���� ---
	EffectManager::Instance().Update();

	// --- �J�������� ---
	Camera::Instance().Update();


	// �^�C�}�[�̒萔�o�b�t�@�̍X�V
	UpdateTimerConstnat();

	// �X�e�[�W�X�V
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

	//sprTest->SetAngle(sprTest->GetAngle() + 180 * Timer::Instance().DeltaTime());
	//sprTest->UpdateAnimation();

	//sprTest3->SetAngle(sprTest->GetAngle() + 180 * Timer::Instance().DeltaTime());

	//emitter1->Update();
	//Particle::Instance().Update();
}

void SceneStage::Render()
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

		//Particle::Instance().Render();

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
	//sprTest->Render();
	//sprTest2->Render();
	//sprTest3->Render();

#if USE_IMGUI
	// --- �f�o�b�OGUI�`�� ---
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
void SceneStage::DrawDebugGUI()
{
	// ���j���[�o�[�`��
	DrawMenuBar();

	if (ImGui::Begin("Debug Menu")) {

		//objects->UpdateImGui();
		for (auto& object : objects) {
			object.second->UpdateImGui(objectNames[object.first]);
		}

		ImGui::InputText("Save File", filename, IM_ARRAYSIZE(filename));

		if (ImGui::Button("Save")) {
			if (!SaveFileStage(filename)) {
				ErrorLogger::Log("�ۑ��Ɏ��s���܂����B");
			}
		}

		if (ImGui::Button("Load")) {
			if (!LoadFileStage(filename)) {
				ErrorLogger::Log("�ǂݍ��݂Ɏ��s���܂����B");
			}
		}

		if (ImGui::Button("Clear")) {
			ClearStage();
		}

		ImGui::End();
	}

	PlayerManager::Instance().DrawDebugImGui();
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
			if (Collision::IntersectSphereVsSphere(player->GetPos(), player->GetRadius(), collision.pos, collision.radius, outPos))
			{
				player->HitModel(outPos, PlayerManager::Instance().GetHitPower(), PlayerManager::Instance().GetHitDownSpeed());
			}
		}
	}
}

void SceneStage::StageVsRope()
{
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
		if (collision.pos.y + collision.radius < rayPos[0].y && collision.pos.y + collision.radius < rayPos[1].y)
			continue;
		DirectX::XMFLOAT3 outPos;
		if (PlayerManager::Instance().IntersectSphereVsLine(collision.pos,collision.radius,rayPos[0],rayPos[1]))
		{
			for (Player* player : PlayerManager::Instance().GetPlayer())
			{
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

	//�X�e�[�W������������
	ClearStage();

	std::string str;

	while (std::getline(file, str)) {
		int objectType = 0;
		float _x = 0.0f, _y = 0.0f, _z = 0.0f;
		std::stringstream ss(str);
		ss >> objectType >>  _x >> _y >> _z;

		objects.at(static_cast<eObjectType>(objectType))->Add(DirectX::XMFLOAT3{ _x, _y, _z  });
	
	}

	return true;
}
