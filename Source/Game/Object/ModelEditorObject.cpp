#include "ModelEditorObject.h"

void ModelEditorObject::Update()
{
	// �A�j���[�V�����X�V
	UpdateAnimation();

	// �p���s��X�V
	UpdateTransform();
}

void ModelEditorObject::Render()
{
	model->Render(transform, &keyFrame);
}
