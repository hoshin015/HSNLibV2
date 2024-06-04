#pragma once

#include "Scene.h"
#include "SceneStage.h"
#include "../../Source/Game/Object/Player/PlayerManager.h"

class SceneGame1 : public Scene
{
public:
    SceneGame1() {}
    ~SceneGame1() {}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V
	void Update() override;

	// �`��
	void Render() override;


private:
	// �f�o�b�O�`��
	void DrawDebugGUI();

	void StageCollision();

	void StageVsRope();

private:
	//�J������ݒ肷�邽�߂̐��l
	DirectX::XMFLOAT3 cameraOffset = { 0,150,150 };
	float cameraAngle = 45.0f;

	std::unordered_map<int, std::unique_ptr<Object3D>>  objects;
	std::vector<Object3D::Transform> transforms;

	std::vector<Object3D::Collision> collisions;


	//�R���g���[���[�̐U���̃^�C�}�[
	float controllerTimer = 0.0f;
};