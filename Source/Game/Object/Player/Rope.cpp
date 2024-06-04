#include "Rope.h"
#include "../../Library/3D/DebugPrimitive.h"
#include "../../Library/ImGui/ImGuiManager.h"

Rope::Rope(const char* filePath) : AnimatedObject(filePath)
{
	//仮のスケール
	ropeScaleY = 0.168f / 20.0f;
	this->SetScaleY(ropeScaleY);
}

void Rope::Update()
{
	if (ropeLength > maxRopeLength)
		overRopeLength = true;

	float scaleY = ropeScaleY * ropeLength;
	this->SetScaleY(scaleY);

	// アニメーション更新
	UpdateAnimation();

	// 姿勢行列更新
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
