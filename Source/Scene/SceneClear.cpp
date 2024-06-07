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
	primitive2d = std::make_unique<Primitive2D>();

	//数値の初期化
	goalPerformX = 1280 / SQUARE_SIZE;
	onScorePerform = false;
	scorePerformCurrentState = 0;
	scorePerformNextState = 0;
	scorePerformTimer = 0;
	scorePerformString.clear();
	scorePerformColorA = 0.0f;
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

	if (isFinishFirstPerform)
	{
		if (input.IsGamePadConnected())
		{
			if (input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a))
			{
				onScorePerform = true;
				//SceneManager::Instance().ChangeScene(new SceneTitle);
			}
		}
		else
		{
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter))
			{
				onScorePerform = true;
				//SceneManager::Instance().ChangeScene(new SceneTitle);
			}
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

	//クリア画像の描画
    sprite->Render();

	//最初にする描画
	FirstPerformRender();
	ScorePerformRender();
}

//ゲームシーンでの演出を消すような感じ
void SceneClear::FirstPerformRender()
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

void SceneClear::ScorePerformRender()
{
	Score& score = Score::Instance();
	std::vector<float> totalTimes = score.GetTotalTime();

	if (!onScorePerform)
		return;

	if (scorePerformTimer > 1.0f && scorePerformColorA > 0.5f)
	{
		scorePerformNextState++;
		scorePerformCurrentState = scorePerformNextState;
		scorePerformTimer = 0;
	}

	//時間経過
	scorePerformTimer += Timer::Instance().DeltaTime();


	std::wstringstream ss;
#if 1
	//分
	const int MINUTES_TIME = static_cast<int>(totalTimes.at(score.GetPlayNum()) / 60);
	//分の2桁目
	const int MINUETES_TIME_SECOND = MINUTES_TIME / 10;
	//分の1桁目
	const int MINUTES_TIME_FIRST  = MINUTES_TIME - MINUETES_TIME_SECOND * 10;
	//秒
	const int SECOND_TIME = static_cast<int>(totalTimes.at(score.GetPlayNum()) - MINUTES_TIME * 60);
	//秒の2桁目
	const int SECOND_TIME_SECOND = SECOND_TIME / 10;
	//秒の1桁目
	const int SECOND_TIME_FIRST = SECOND_TIME - SECOND_TIME_SECOND * 10;
#else
	//const int MINUTES_TIME = 1;
	//分の2桁目
	const int MINUETES_TIME_SECOND = 1;
	//分の1桁目
	const int MINUTES_TIME_FIRST = 2;
	//分
	//const int SECOND_TIME = static_cast<int>(totalTimes.at(score.GetPlayNum()) - MINUTES_TIME * 60);
	//秒の2桁目
	const int SECOND_TIME_SECOND = 3;
	//秒の1桁目
	const int SECOND_TIME_FIRST = 4;

#endif

	int randumNum = rand() % 10;
	const float SIZE = 100;
	const DirectX::XMFLOAT2 DRAWPOS_SCORE = { 400 + SIZE / 2 ,360 };

	switch (scorePerformCurrentState)
	{
	case 0:
		//裏を少しずつ暗くし、最終的に半透明にする
	{
		scorePerformColorA += 0.5f * Timer::Instance().DeltaTime();
		drawPosRandumNum = { DRAWPOS_SCORE.x,DRAWPOS_SCORE.y };

		break;
	}

	case 1:
		//ランダム設定
	{
		ss << randumNum;

		break;
	}

	case 2:
		//分の2桁目を設定
	{
		scorePerformString << MINUETES_TIME_SECOND;

		//自力で進める
		scorePerformTimer = 0;
		scorePerformCurrentState = 1;
		//ランダムの数を出す位置をずらす
		drawPosRandumNum.x += SIZE / 2;

		break;
	}
	case 3:
		//分の1桁目を設定
	{
		scorePerformString << MINUTES_TIME_FIRST << ":";

		//自力で進める
		scorePerformTimer = 0;
		scorePerformCurrentState = 1;
		//二つ分変わっているため
		drawPosRandumNum.x += (SIZE / 2) * 2;
	}

		break;
	case 4:
		//秒の2桁目を設定
	{
		scorePerformString << SECOND_TIME_SECOND;

		//自力で進める
		scorePerformTimer = 0;
		scorePerformCurrentState = 1;
		drawPosRandumNum.x += (SIZE / 2);
	}
		break;
		
	case 5:
		//分の2桁目を設定
	{
		scorePerformString << SECOND_TIME_FIRST;

	
		drawPosRandumNum.x += (SIZE / 2);
		//終了
		scorePerformCurrentState = 100;
	}
		break;
	}

	//ss << MINUTES_TIME << ":" << SECOND_TIME;
	//背景を暗くする
	primitive2d->Render(0, 0, 1280, 720, 0, 0, 0, scorePerformColorA, 0);

	//スコアの描画
	DispString::Instance().Draw(ss.str().c_str(), drawPosRandumNum, SIZE, TEXT_ALIGN::MIDDLE_LEFT);
	DispString::Instance().Draw(scorePerformString.str().c_str(), DRAWPOS_SCORE, SIZE, TEXT_ALIGN::MIDDLE_LEFT);
}
