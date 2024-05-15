#pragma once
#include "BaseObject.h"
#include "StaticModel.h"

// �A�j���[�V�������Ȃ����f��(�C���X�^���X�`��)
class StaticObject : public BaseObject
{
public:
	StaticObject() {}
	StaticObject(const char* filePath);
	virtual ~StaticObject() {}

	virtual void Update() = 0;
	virtual void Render() = 0;

protected:
	std::unique_ptr<StaticModel> model;

	int					instancingCount = 0;		// �C���X�^���V���O��
	bool				exist[MAX_INSTANCE] = {};	// �`�悷��index��true�ɂȂ�
	DirectX::XMFLOAT4X4 transforms[MAX_INSTANCE];	// index�̎p���������

	// �p���s��X�V
	void UpdateTransform() override;

public:
	//// ���蓖�ĂĔԍ���Ԃ�
	//int AllocateInstancingIndex();
	//// ���蓖�Ă炽�ԍ����������
	//void FreeInstancingIndex(int instancingIndex);
	//// �s��v�Z
	//void UpdateTransform(int instancingIndex, const DirectX::XMFLOAT4X4& transform);
	//// ���݂̎p���s����擾
	//const DirectX::XMFLOAT4X4& GetTransform(int instancingIndex) const;
};