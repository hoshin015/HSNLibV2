#include "UserInterface.h"
#include "../../Library/Timer.h"
#include "../../Library/Easing.h"

// ������
void UserInterface::Initialize()
{
}

// �X�V
void UserInterface::Update()
{
	uiPause.Update();
}

// �`��
void UserInterface::Render()
{
	uiPause.Render();
}


