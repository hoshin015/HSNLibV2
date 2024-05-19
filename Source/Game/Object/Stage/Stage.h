#pragma once

#include "../Base/AnimatedObject.h"
#include "../../Library/Math/Collision.h"

// ステージ
class Stage : public AnimatedObject
{
public:
	Stage(const char* filePath) : AnimatedObject(filePath) {};
	virtual ~Stage() {}

	// 更新処理
	virtual void Update() = 0;

	// 描画処理
	virtual void Render(bool isShadow) = 0;

	// レイキャスト
	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;
};