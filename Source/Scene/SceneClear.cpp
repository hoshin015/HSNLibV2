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
    // --- imguiManager処理 ---
    ImGuiManager::Instance().Update();
#endif
    // --- inputManager処理 ---
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
    gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_OFF);
    // blendStateの設定
    gfx->SetBlend(BLEND_STATE::ALPHA);

    sprite->Render();

    std::wstringstream ss;

    int minutesTime = static_cast<int>(Score::Instance().GetTotalTime().at(Score::Instance().GetPlayNum()) / 60);
    int secondTime = static_cast<int>(Score::Instance().GetTotalTime().at(Score::Instance().GetPlayNum()) - minutesTime * 60);
    ss << minutesTime << ":" <<secondTime;

    DispString::Instance().Draw(ss.str().c_str(), { 640,320 }, 40, TEXT_ALIGN::MIDDLE);
}
