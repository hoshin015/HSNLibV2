#include "TestAnimated.h"

void TestAnimated::Update()
{
	// �A�j���[�V�����X�V
	UpdateAnimation();

	// �p���s��X�V
	UpdateTransform();
}

void TestAnimated::Render()
{
	model->Render(transform, &keyFrame);
}
