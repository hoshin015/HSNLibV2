#include "Rope.h"
#include "../../Library/3D/DebugPrimitive.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Audio/AudioManager.h"
#include "PlayerManager.h"

Rope::Rope(const char* filePath) : AnimatedObject(filePath)
{
	//���̃X�P�[��
	ropeScaleY = 0.14f / 20.0f;
	//this->SetScaleY(ropeScaleY);
}

void Rope::Update()
{
	if (PlayerManager::Instance().GetDoMove())
	{
		if(!AudioManager::Instance().IsInUseMusic(static_cast<int>(MUSIC_LABEL::SE_STRETCHSTICK)))
		{
			AudioManager::Instance().PlayMusic(static_cast<int>(MUSIC_LABEL::SE_STRETCHSTICK));
		}
	}

	oldRopeLength = currentRopeLength;

	if (currentRopeLength > maxRopeLength)
		overRopeLength = true;

	//�ő�l��8���𒴂�����
	if (currentRopeLength > maxRopeLength * 0.8f)
		color = { 1.0f,0.25f,0.25f,1.0f };
	else
		color = { 0.25f,0.25f,1.0f,1.0f };

	float scaleX = ropeScaleY * currentRopeLength;
	this->SetScaleX(scaleX);

	// �A�j���[�V�����X�V
	UpdateAnimation();

	// �p���s��X�V
	UpdateTransform();
}

void Rope::Render(bool isShadow)
{
    model->Render(transform, &keyFrame, isShadow, color);
}

void Rope::DrawDebugImGui()
{
	ImGui::SliderFloat("MaxLopeLength", &maxRopeLength, 0, 100.0f);
	ImGui::ColorEdit4("RopeColor", &color.x);
}
