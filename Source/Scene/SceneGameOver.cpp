#include "SceneGameOver.h"
#include "SceneManager.h"
#include "SceneTitle.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/3D/Camera.h"

void SceneGameOver::Initialize()
{
    sprite = std::make_unique<Sprite>("Data/Texture/GameOver.png");
}

void SceneGameOver::Finalize()
{
}

void SceneGameOver::Update()
{
    // --- inputManager処理 ---
    InputManager& input = InputManager::Instance();
    input.Update();

    Camera::Instance().Update();

    if (input.IsGamePadConnected())
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

void SceneGameOver::Render()
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
}
