#include "SceneGameOver.h"
#include "SceneManager.h"
#include "SceneTitle.h"
#include "Score/Score.h"

#include "../../Library/Graphics/Graphics.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/Audio/AudioManager.h"

void SceneGameOver::Initialize()
{
    sprite = std::make_unique<Sprite>("Data/Texture/GameOver.png");
	primitive2d = std::make_unique<Primitive2D>();

	//���l�̏�����
	goalPerformX = 1280 / SQUARE_SIZE;
	isFinishFirstPerform = false;
}

void SceneGameOver::Finalize()
{
	AudioManager::Instance().StopMusic(static_cast<int>(MUSIC_LABEL::BGM_GAMEOVER));
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

	FirstPerformRender();
}

//�Q�[���V�[���ł̉��o�������悤�Ȋ���
void SceneGameOver::FirstPerformRender()
{
	goalPerformTimer += Timer::Instance().DeltaTime();

	//���̕`�������܂ł̎���
	const float NEXTDRAW_TIME = 0.15f;
	const int MAX_X = 1280 / SQUARE_SIZE;

	for (int x = 0; x < goalPerformX; x++)
	{
		//Y�̏����l
		const int INITIAL_Y = (x % 2 == 0) ? 1 : 0;
		for (int y = INITIAL_Y; y < 720 / SQUARE_SIZE; y += 2)
		{
			const float PERFORM_X = x * SQUARE_SIZE;
			const float PERFORM_Y = y * SQUARE_SIZE;

			primitive2d->Render(
				PERFORM_X,
				PERFORM_Y,
				SQUARE_SIZE,
				SQUARE_SIZE,
				0, 0, 0, 1, 0);
		}
	}

	int i = 0;
	for (int x = MAX_X; i < goalPerformX; x--)
	{
		//Y�̏����l
		const int INITIAL_Y = (x % 2 == 0) ? 0 : 1;
		for (int y = INITIAL_Y; y < 720 / SQUARE_SIZE; y += 2)
		{
			const float PERFORM_X = x * SQUARE_SIZE;
			const float PERFORM_Y = y * SQUARE_SIZE;

			primitive2d->Render(
				PERFORM_X,
				PERFORM_Y,
				SQUARE_SIZE,
				SQUARE_SIZE,
				0, 0, 0, 1, 0);
		}
		i++;
	}

	if (goalPerformTimer > NEXTDRAW_TIME)
	{
		goalPerformTimer = 0;
		goalPerformX--;
	}

	if (goalPerformX < 0)
		isFinishFirstPerform = true;
}