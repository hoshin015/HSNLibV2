#pragma once

#include "Scene.h"
#include "SceneStage.h"
#include "../../Source/Game/Object/Player/PlayerManager.h"

constexpr float CAMERA_LAPTIME = 5.0f;
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

	void LerpCameraTarget(DirectX::XMFLOAT3 target, float factor);

	void CameraUpdate();

	void GoalCheack();

private:
	//�J������ݒ肷�邽�߂̐��l
	const DirectX::XMFLOAT3 cameraOffset = { 0,350,400 };
	float cameraAngle = 20.0f;

	float lerpAngle = 0.0f;

	std::unordered_map<int, std::unique_ptr<Object3D>>  objects;
	std::vector<Object3D::Transform> transforms;

	std::vector<Object3D::Collision> collisions;

	std::unique_ptr<FrameBuffer> frameBuffer;
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	std::unique_ptr<Bloom> bloom;
	std::unique_ptr<Shadow> shadow;




	//�R���g���[���[�̐U���̃^�C�}�[
	float controllerTimer = 0.0f;

	//�J�������o�p�ϐ�
	int cameraState = 0;
	DirectX::XMFLOAT3 cameraTarget = { 0,0,0 };
public:
};