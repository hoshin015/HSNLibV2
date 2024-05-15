#include "StageMain.h"


// 更新処理
void StageMain::Update()
{
	// 姿勢行列更新
	UpdateTransform();
}

// 描画処理
void StageMain::Render()
{
	model->Render(instanceNum, &transform);
}

// レイキャスト
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model->GetModelResource(), hit);
}
