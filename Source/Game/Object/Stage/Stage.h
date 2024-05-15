#pragma once

#include "../Base/StaticObject.h"
#include "../../Library/Math/Collision.h"

// �X�e�[�W
class Stage : public StaticObject
{
public:
	Stage(const char* filePath) : StaticObject(filePath) {};
	virtual ~Stage() {}

	// �X�V����
	virtual void Update() = 0;

	// �`�揈��
	virtual void Render() = 0;

	// ���C�L���X�g
	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;
};