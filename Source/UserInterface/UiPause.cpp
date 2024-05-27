#include "UiPause.h"
#include "../../Library/Timer.h"
#include "../../Library/Easing.h"

UiPause::UiPause()
{
	pauseText = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/pauseText.png");
	pauseText->SetPivotPoint({ 100, pauseText->GetSizeY() * 0.5f });
	pauseText->SetPos({ 640, 360 });
}

void UiPause::Update()
{
	float deltaTime = Timer::Instance().DeltaTime();

	pauseTextTimer += deltaTime;
	float textPosX = Easing::InQuad(pauseTextTimer,pauseTextInTime) * 640;

	pauseText->SetAngle(pauseText->GetAngle() + 180 * deltaTime);
	pauseText->SetPosX(textPosX);

	if (pauseTextTimer >= pauseTextInTime) pauseTextTimer = 0.0f;
}

void UiPause::Render()
{
	pauseText->Render();
}
