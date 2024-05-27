#include "UserInterface.h"

// ������
void UserInterface::Initialize()
{
	pauseMenu.pauseText = std::make_unique<Sprite>("Data/Texture/UserInterface/Pause/pauseText.png");
	pauseMenu.pauseText->SetPivotPoint({ 100, pauseMenu.pauseText->GetSizeY() * 0.5f });
	pauseMenu.pauseText->SetPos({ 640, 360 });
}

// �`��
void UserInterface::Render()
{
	pauseMenu.pauseText->Render();
}


