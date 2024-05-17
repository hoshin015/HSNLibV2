#include "TestAnimated.h"

void TestAnimated::Update()
{
	// アニメーション更新
	UpdateAnimation();

	// 姿勢行列更新
	UpdateTransform();
}

void TestAnimated::Render()
{
	model->Render(transform, &keyFrame);
}
