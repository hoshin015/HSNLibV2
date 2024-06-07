#include "SceneClear.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "Score/Score.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/Text/DispString.h"


void SceneClear::Initialize()
{
    sprite = std::make_unique<Sprite>("Data/Texture/GameClear.png");
}

void SceneClear::Finalize()
{
}

void SceneClear::Update()
{
#if USE_IMGUI
    // --- imguiManager���� ---
    ImGuiManager::Instance().Update();
#endif
    // --- inputManager���� ---
    InputManager& input = InputManager::Instance();
    input.Update();

    Camera::Instance().Update();

    if(input.IsGamePadConnected())
    {
        if (input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::menu))
        {
            SceneManager::Instance().ChangeScene(new SceneTitle);
        }
    }
    else
    {
        if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter))
        {
            SceneManager::Instance().ChangeScene(new SceneTitle);
        }
    }
}

void SceneClear::Render()
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
    gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_OFF);
    // blendState�̐ݒ�
    gfx->SetBlend(BLEND_STATE::ALPHA);

    sprite->Render();

    std::wstringstream ss;

    int minutesTime = static_cast<int>(Score::Instance().GetTotalTime().at(Score::Instance().GetPlayNum()) / 60);
    int secondTime = static_cast<int>(Score::Instance().GetTotalTime().at(Score::Instance().GetPlayNum()) - minutesTime * 60);
    ss << minutesTime << ":" <<secondTime;

    DispString::Instance().Draw(ss.str().c_str(), { 640,320 }, 40, TEXT_ALIGN::MIDDLE);
}
