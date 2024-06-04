#pragma once

#include "Scene.h"
#include "SceneStage.h"
#include "../../Source/Game/Object/Player/PlayerManager.h"

#include <string>

class SceneTutorial : public Scene
{
private:
	enum EXPLAIN
	{
		FIRST = 0,
		MOVE_Z = 3,
		INPUT_MOVE = 10,
		LENGTH_STICK = 19,
		MAXLENGTH = 26,
	};

public:
	SceneTutorial() {}
	~SceneTutorial() {}

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

	//�`���[�g���A�����̃A�b�v�f�[�g
	void UpdateTutorial();

	//�`���[�g���A�����̃e�L�X�g�̕`��
	void DrawTutorialText();

private:
	//�J������ݒ肷�邽�߂̐��l
	DirectX::XMFLOAT3 cameraOffset = { 0,350,400 };
	float cameraAngle = 45.0f;

	std::unordered_map<int, std::unique_ptr<Object3D>>  objects;
	std::vector<Object3D::Transform> transforms;

	std::vector<Object3D::Collision> collisions;


	//�R���g���[���[�̐U���̃^�C�}�[
	float controllerTimer = 0.0f;

	//�`���[�g���A���̏󋵂��Ǘ�����X�e�[�g
	int tutorialState = 0;
	//�`���[�g���A�����̃e�L�X�g�\�����Ǘ�����X�e�[�g
	int textState = 0;
	//�e�L�X�g��\�����钆���l
	DirectX::XMFLOAT2 textPosition = { 640, 600 };
	//�e�L�X�g�̃T�C�Y
	float textSize = 40;
	//�`���[�g���A������Enter�L�[�������ƃe�L�X�g�����֑��邩�ǂ���
	bool updateText = true;
	
	//�v���C���[�̑��삪���ꂽ���Ԃ��Ǘ�����
	float moveTimer = 0;

private:
	//�\������e�L�X�g�̓��e
	const std::wstring textMessage[30] = {
		L"neck run �̐��E�ւ悤�����I",
		L"���̃Q�[���̗V�ѕ����������",
		L"�܂��́A�v���C���[�ɂ��Đ��������",

		L"�n�߂ɁA�v���C���[�͏�ɉ��֍s���������",
		L"���̃v���C���[���S�[���ɂ��ǂ蒅�����邱�Ƃ��ł�����Q�[���N���A�I",

		L"���ꂶ�Ⴀ�A������@�ɂ��ċ������",
		L"�v���C���[����l�����ˁH",
		L"���̓�l�͂ǂ�������������Ƃ��ł����",
		L"������WASD�L�[�A�Ԃ������\���L�[�œ��������",
		L"�����������Ă݂悤�I",
		L"",
		L"�����I",

		L"���ꂶ�Ⴀ�A���x�̓M�~�b�N�ɂ��Đ�������ˁH",
		L"��l�̊Ԃɖ_������̂������邩�ȁH",
		L"���̖_�́A��l������邱�Ƃ̂ł��Ȃ��悤�ɂ��Ă����",
		L"��l�̋������J���قǁA���̖_���L�тĂ�����",
		L"�����āA�_���L�т�ΐL�т�قǁA��l�����ɍs�����x���オ���",
		L"���삪����Ȃ锼�ʁA�^�C���̒Z�k���_�����",
		L"�m�F���Ă݂悤�I",
		L"",
		L"�����������ȁH",

		L"���������A���̖_�̒����ɂ͌��E������񂾂�",
		L"��l�̋������J��������ƁA���E���}�����Ⴄ��",
		L"���������Ȃ�ƁA��l�͋����I�ɐ^�񒆂Ɉ����񂹂����",
		L"�ً}����ɂ��g���邯�ǁA�_���Z���Ȃ邩��A�^�C���͈����Ȃ邩����",
		L"���ꂶ�Ⴀ�A����Ă݂悤�I",
		L"",
		L"���������I",

	};


};