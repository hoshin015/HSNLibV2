#include "StageMain.h"


// �X�V����
void StageMain::Update()
{
	// �p���s��X�V
	UpdateTransform();
}

// �`�揈��
void StageMain::Render()
{
	model->Render(instanceNum, &transform);
}

// ���C�L���X�g
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model->GetModelResource(), hit);
}
