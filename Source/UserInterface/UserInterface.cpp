#include "UserInterface.h"
#include "../../Library/Timer.h"
#include "../../Library/Easing.h"

// 初期化
void UserInterface::Initialize()
{
}

// 更新
void UserInterface::Update()
{
	uiPause.Update();
}

// 描画
void UserInterface::Render()
{
	uiPause.Render();
}


