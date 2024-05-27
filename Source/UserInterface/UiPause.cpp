#include "UiPause.h"
#include "../../Library/Timer.h"
#include "../../Library/Easing.h"
#include "../../Library/Input/InputManager.h"


void UiPause::Initialize()
{
	state = UiPauseState::Hidden;

	pauseText = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/pauseText.sprite");
	pauseText->UpdateAnimation();

	pauseImg1 = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/pauseImg1.sprite");
	pauseImg1->UpdateAnimation();
}

bool UiPause::Update()
{
	switch (state)
	{
	case UiPauseState::Hidden:
		{
			// ポーズフラグがtrueになったら遷移
			if(InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter))
			{
				isPause = true;
				state = UiPauseState::Showing;
			}
		}
		break;
	case UiPauseState::Showing:
		{
			pauseTimer += Timer::Instance().DeltaTime();

			// 表示中にポーズ解除されたら非表示遷移状態に遷移
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter))
			{
				isPause = false;
				state = UiPauseState::Hide;
				break;
			}

			pauseText->SetPos(GetNowParamVec(pauseTimer, pauseTextPos));
			pauseText->SetColorA(GetNowParam(pauseTimer , pauseTextAlpha));



			float img1PosX = Easing::OutQuad(pauseTimer, pauseTotalInTime, pauseImg1VisiblePos.x, pauseImg1HiddenPos.x);
			float img1PosY = Easing::OutQuad(pauseTimer, pauseTotalInTime, pauseImg1VisiblePos.y, pauseImg1HiddenPos.y);
			pauseImg1->SetPos({ img1PosX, img1PosY });

			// 表示完了したら遷移
			if(pauseTimer > pauseTotalInTime)
			{
				pauseTimer = pauseTotalInTime;
				state = UiPauseState::Visible;
			}
		}
		break;
	case UiPauseState::Visible:
		{
			// ポーズフラグがfalseになったら遷移
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter))
			{
				isPause = false;
				state = UiPauseState::Hide;
			}
		}
		break;
	case UiPauseState::Hide:
		{
			pauseTimer -= Timer::Instance().DeltaTime();

			// 表示中にポーズ解除されたら非表示遷移状態に遷移
			if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::Enter))
			{
				isPause = true;
				state = UiPauseState::Showing;
				break;
			}

			pauseText->SetPos(GetNowParamVec(pauseTimer, pauseTextPos));
			pauseText->SetColorA(GetNowParam(pauseTimer, pauseTextAlpha));

			float img1PosX = Easing::OutQuad(pauseTimer, pauseTotalOutTime, pauseImg1VisiblePos.x, pauseImg1HiddenPos.x);
			float img1PosY = Easing::OutQuad(pauseTimer, pauseTotalOutTime, pauseImg1VisiblePos.y, pauseImg1HiddenPos.y);
			pauseImg1->SetPos({ img1PosX, img1PosY });

			// 非表示完了したら遷移
			if (pauseTimer < 0.0)
			{
				pauseTimer = 0.0f;
				state = UiPauseState::Hidden;
				break;
			}
		}
		break;
	}

	//pauseText->SetAngle(pauseText->GetAngle() + 180 * Timer::Instance().DeltaTime());
	pauseImg1->UpdateAnimation();


	return isPause;
}

// 描画
void UiPause::Render()
{
	//if (!isPause) return;

	pauseText->Render();
	pauseImg1->Render();
}

// イージングによる現在のパラメータ取得
float UiPause::GetNowParam(float time, float startTime, float endTime, float startValue, float endValue)
{
	return Easing::OutQuad(time - startTime, endTime - startTime, endValue, startValue);
}

// イージングによる現在のパラメータ取得
float UiPause::GetNowParam(float time, UiEasingValue uiEasingValue)
{
	if (pauseTimer < uiEasingValue.startTime)
	{
		return uiEasingValue.startValue;
	}
	if (pauseTimer > uiEasingValue.endTime)
	{
		return uiEasingValue.endValue;
	}

	return Easing::OutQuad(time - uiEasingValue.startTime, uiEasingValue.endTime - uiEasingValue.startTime, uiEasingValue.endValue, uiEasingValue.startValue);
}

DirectX::XMFLOAT2 UiPause::GetNowParamVec(float time, UiEasingValueVec uiEasingValueVec)
{
	if (pauseTimer < uiEasingValueVec.startTime)
	{
		return uiEasingValueVec.startValueVec;
	}
	if(pauseTimer > uiEasingValueVec.endTime)
	{
		return uiEasingValueVec.endValueVec;
	}


	DirectX::XMFLOAT2 vec2 =
	{
		Easing::OutQuad(time - uiEasingValueVec.startTime, uiEasingValueVec.endTime - uiEasingValueVec.startTime, uiEasingValueVec.endValueVec.x, uiEasingValueVec.startValueVec.x),
		Easing::OutQuad(time - uiEasingValueVec.startTime, uiEasingValueVec.endTime - uiEasingValueVec.startTime, uiEasingValueVec.endValueVec.y, uiEasingValueVec.startValueVec.y)
	};

	return vec2;
}
