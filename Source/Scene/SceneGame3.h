#pragma once

#include "Scene.h"
#include "SceneStage.h"
#include "../../Source/Game/Object/Player/PlayerManager.h"

class SceneGame3 : public Scene
{
public:
	SceneGame3() {}
	~SceneGame3() {}

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
	DirectX::XMFLOAT3 cameraOffset = { 0,250,600 };
	float cameraAngle = 15.0f;

	std::unordered_map<int, std::unique_ptr<Object3D>>  objects;
	std::vector<Object3D::Transform> transforms;

	std::vector<Object3D::Collision> collisions;

	std::unique_ptr<FrameBuffer> frameBuffer;
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	std::unique_ptr<Bloom> bloom;
	std::unique_ptr<Shadow> shadow;


	//�R���g���[���[�̐U���̃^�C�}�[
	float controllerTimer = 0.0f;
};