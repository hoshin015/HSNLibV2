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

	//数値の初期化
	goalPerformX = 1280 / SQUARE_SIZE;
	isFinishFirstPerform = false;
}

void SceneGameOver::Finalize()
{
	AudioManager::Instance().StopMusic(static_cast<int>(MUSIC_LABEL::BGM_GAMEOVER));
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

	FirstPerformRender();
}

//ゲームシーンでの演出を消すような感じ
void SceneGameOver::FirstPerformRender()
{
	goalPerformTimer += Timer::Instance().DeltaTime();

	//次の描画をするまでの時間
	const float NEXTDRAW_TIME = 0.15f;
	const int MAX_X = 1280 / SQUARE_SIZE;

	for (int x = 0; x < goalPerformX; x++)
	{
		//Yの初期値
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
		//Yの初期値
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