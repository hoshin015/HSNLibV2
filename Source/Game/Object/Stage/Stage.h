#pragma once

#include "../Base/StaticObject.h"
#include "../../Library/Math/Collision.h"

// ステージ
class Stage : public StaticObject
{
public:
	Stage(const char* filePath) : StaticObject(filePath) {};
	virtual ~Stage() {}

	// 更新処理
	virtual void Update() = 0;

	// 描画処理
	virtual void Render() = 0;

	// レイキャスト
	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;
};