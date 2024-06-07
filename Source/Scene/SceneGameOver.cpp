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
    // --- inputManager���� ---
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
}
