#include "Rope.h"
#include "../../Library/3D/DebugPrimitive.h"
#include "../../Library/ImGui/ImGuiManager.h"

Rope::Rope(const char* filePath) : AnimatedObject(filePath)
{
	//���̃X�P�[��
	ropeScaleY = 0.168f / 20.0f;
	this->SetScaleY(ropeScaleY);
}

void Rope::Update()
{
	if (ropeLength > maxRopeLength)
		overRopeLength = true;

	float scaleY = ropeScaleY * ropeLength;
	this->SetScaleY(scaleY);

	// �A�j���[�V�����X�V
	UpdateAnimation();

	// �p���s��X�V
	UpdateTransform();
}

void Rope::Render(bool isShadow)
{
    model->Render(transform, &keyFrame, isShadow);
}

void Rope::DrawDebugImGui()
{
	ImGui::SliderFloat("MaxLopeLength", &maxRopeLength, 0, 100.0f);
}
