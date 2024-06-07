#include "Rope.h"
#include "../../Library/3D/DebugPrimitive.h"
#include "../../Library/ImGui/ImGuiManager.h"

Rope::Rope(const char* filePath) : AnimatedObject(filePath)
{
	//仮のスケール
	ropeScaleY = 0.14f / 20.0f;
	//this->SetScaleY(ropeScaleY);
}

void Rope::Update()
{
	if (ropeLength > maxRopeLength)
		overRopeLength = true;

	//最大値の8割を超えたら
	if (ropeLength > maxRopeLength * 0.8f)
		color = { 1.0f,0.25f,0.25f,1.0f };
	else
		color = { 0.25f,0.25f,1.0f,1.0f };

	float scaleX = ropeScaleY * ropeLength;
	this->SetScaleX(scaleX);

	// アニメーション更新
	UpdateAnimation();

	// 姿勢行列更新
	UpdateTransform();
}

void Rope::Render(bool isShadow)
{
    model->Render(transform, &keyFrame, isShadow, color);
}

void Rope::DrawDebugImGui()
{
	ImGui::SliderFloat("MaxLopeLength", &maxRopeLength, 0, 100.0f);
}
