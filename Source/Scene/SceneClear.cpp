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

	//���l�̏�����
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
    // --- imguiManager���� ---
    ImGuiManager::Instance().Update();
#endif
    // --- inputManager���� ---
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

	//�N���A�摜�̕`��
    sprite->Render();

	//�ŏ��ɂ���`��
	FirstPerformRender();
	ScorePerformRender();
}

//�Q�[���V�[���ł̉��o�������悤�Ȋ���
void SceneClear::FirstPerformRender()
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

	//���Ԍo��
	scorePerformTimer += Timer::Instance().DeltaTime();


	std::wstringstream ss;
#if 1
	//��
	const int MINUTES_TIME = static_cast<int>(totalTimes.at(score.GetPlayNum()) / 60);
	//����2����
	const int MINUETES_TIME_SECOND = MINUTES_TIME / 10;
	//����1����
	const int MINUTES_TIME_FIRST  = MINUTES_TIME - MINUETES_TIME_SECOND * 10;
	//�b
	const int SECOND_TIME = static_cast<int>(totalTimes.at(score.GetPlayNum()) - MINUTES_TIME * 60);
	//�b��2����
	const int SECOND_TIME_SECOND = SECOND_TIME / 10;
	//�b��1����
	const int SECOND_TIME_FIRST = SECOND_TIME - SECOND_TIME_SECOND * 10;
#else
	//const int MINUTES_TIME = 1;
	//����2����
	const int MINUETES_TIME_SECOND = 1;
	//����1����
	const int MINUTES_TIME_FIRST = 2;
	//��
	//const int SECOND_TIME = static_cast<int>(totalTimes.at(score.GetPlayNum()) - MINUTES_TIME * 60);
	//�b��2����
	const int SECOND_TIME_SECOND = 3;
	//�b��1����
	const int SECOND_TIME_FIRST = 4;

#endif

	int randumNum = rand() % 10;
	const float SIZE = 100;
	const DirectX::XMFLOAT2 DRAWPOS_SCORE = { 400 + SIZE / 2 ,360 };

	switch (scorePerformCurrentState)
	{
	case 0:
		//�����������Â����A�ŏI�I�ɔ������ɂ���
	{
		scorePerformColorA += 0.5f * Timer::Instance().DeltaTime();
		drawPosRandumNum = { DRAWPOS_SCORE.x,DRAWPOS_SCORE.y };

		break;
	}

	case 1:
		//�����_���ݒ�
	{
		ss << randumNum;

		break;
	}

	case 2:
		//����2���ڂ�ݒ�
	{
		scorePerformString << MINUETES_TIME_SECOND;

		//���͂Ői�߂�
		scorePerformTimer = 0;
		scorePerformCurrentState = 1;
		//�����_���̐����o���ʒu�����炷
		drawPosRandumNum.x += SIZE / 2;

		break;
	}
	case 3:
		//����1���ڂ�ݒ�
	{
		scorePerformString << MINUTES_TIME_FIRST << ":";

		//���͂Ői�߂�
		scorePerformTimer = 0;
		scorePerformCurrentState = 1;
		//����ς���Ă��邽��
		drawPosRandumNum.x += (SIZE / 2) * 2;
	}

		break;
	case 4:
		//�b��2���ڂ�ݒ�
	{
		scorePerformString << SECOND_TIME_SECOND;

		//���͂Ői�߂�
		scorePerformTimer = 0;
		scorePerformCurrentState = 1;
		drawPosRandumNum.x += (SIZE / 2);
	}
		break;
		
	case 5:
		//����2���ڂ�ݒ�
	{
		scorePerformString << SECOND_TIME_FIRST;

	
		drawPosRandumNum.x += (SIZE / 2);
		//�I��
		scorePerformCurrentState = 100;
	}
		break;
	}

	//ss << MINUTES_TIME << ":" << SECOND_TIME;
	//�w�i���Â�����
	primitive2d->Render(0, 0, 1280, 720, 0, 0, 0, scorePerformColorA, 0);

	//�X�R�A�̕`��
	DispString::Instance().Draw(ss.str().c_str(), drawPosRandumNum, SIZE, TEXT_ALIGN::MIDDLE_LEFT);
	DispString::Instance().Draw(scorePerformString.str().c_str(), DRAWPOS_SCORE, SIZE, TEXT_ALIGN::MIDDLE_LEFT);
}
