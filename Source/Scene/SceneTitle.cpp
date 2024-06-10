#include <tchar.h>
#include <DirectXMath.h>

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
// --- Scene ---
#include "SceneTitle.h"
#include "SceneManager.h"
// --- UserInterface ---
#include "../../Library/Audio/AudioManager.h"
#include "../UserInterface/UiMain.h"

void SceneTitle::Initialize()
{
	Framework* frameWork = &Framework::Instance();
	imGuiFrameBuffer = std::make_unique<FrameBuffer>(frameWork->GetScreenWidthF(), frameWork->GetScreenHeightF());

	UiMain::Instance().Initialize();

	AudioManager::Instance().PlayMusic(static_cast<int>(MUSIC_LABEL::MAIN_BGM), true);
	AudioManager::Instance().SetMusicVolume(static_cast<int>(MUSIC_LABEL::MAIN_BGM), 0.5f);
}

void SceneTitle::Finalize()
{
	AudioManager::Instance().StopMusic(static_cast<int>(MUSIC_LABEL::MAIN_BGM));
}

void SceneTitle::Update()
{
#if USE_IMGUI
	// --- imguiManager���� ---
	ImGuiManager::Instance().Update();
#endif
	// --- inputManager���� ---
	InputManager::Instance().Update();

	// --- effectManager���� ---
	EffectManager::Instance().Update();

	UiMain::Instance().Update();

	if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::F1))
	{
		SceneManager::Instance().ChangeScene(new SceneTest);
	}

#if USE_IMGUI
#endif
}

void SceneTitle::Render()
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

	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);	


	UiMain::Instance().Render();

#if USE_IMGUI
	// --- �f�o�b�O�`�� ---
	DrawDebugGUI();

#if SHOW_PERFORMANCE
	// --- �p�t�H�[�}���X�`�� ---
	ImGuiManager::Instance().DisplayPerformanceStats();
#endif

	// --- imguiManager �`�� ---
	ImGuiManager::Instance().Render();
#endif
}

// �f�o�b�O�`��
void SceneTitle::DrawDebugGUI()
{
	// ���j���[�o�[�`��
	DrawMenuBar();
}
