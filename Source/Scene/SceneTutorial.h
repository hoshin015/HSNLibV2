#pragma once

#include "Scene.h"
#include "SceneStage.h"
#include "../../Source/Game/Object/Player/PlayerManager.h"
#include "../../Library/2D/Primitive2D.h"

#include <string>
#include <map>

class SceneTutorial : public Scene
{
private:
	/*enum EXPLAIN
	{
		FIRST = 0,
		MOVE_Z = 3,
		INPUT_MOVE = 10,
		LENGTH_STICK = 19,
		MAXLENGTH = 26,
		OBJECT_READY = 29,
	};*/

	enum EXPLAIN_STATE
	{
		READYING = 0,
		MOVE_Z,
		INPUT_MOVE,
		LENGTH_STICK,
		MAXLENGTH,
		OBJECT_READY,
		HITOBJECT_PLAYER,
		HITOBJECT_ROPE,
		END,
	};

	struct TextMessage
	{
		std::wstring text;
		bool changeState = false;
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

	bool StageCollision();

	bool StageVsRope();

	//�`���[�g���A�����̃A�b�v�f�[�g
	void UpdateTutorial();

	//�`���[�g���A�����̃e�L�X�g�̕`��
	void DrawTutorialText();

private:
	//�J������ݒ肷�邽�߂̐��l
	DirectX::XMFLOAT3 cameraOffset = { 0,350,400 };
	float cameraAngle = 30.0f;

	std::unordered_map<int, std::unique_ptr<Object3D>>  objects;
	std::vector<Object3D::Transform> transforms;

	std::vector<Object3D::Collision> collisions;

	std::unique_ptr<Primitive2D> primitive;

	//�R���g���[���[�̐U���̃^�C�}�[
	float controllerTimer = 0.0f;

	//�`���[�g���A���̏󋵂��Ǘ�����X�e�[�g
	int tutorialState = 0;
	int nextState = 0;
	//�`���[�g���A�����̃e�L�X�g�\�����Ǘ�����X�e�[�g
	int textState = 0;
	
	//�e�L�X�g�̃T�C�Y
	float textSize = 40;
	//�`���[�g���A������Enter�L�[�������ƃe�L�X�g�����֑��邩�ǂ���
	bool updateText = true;
	bool updateState = true;
	
	//�v���C���[�̑��삪���ꂽ���Ԃ��Ǘ�����
	float moveTimer = 0;

	std::unique_ptr<FrameBuffer> frameBuffer;
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	std::unique_ptr<Bloom> bloom;
	std::unique_ptr<Shadow> shadow;

private:
	//���[�h����X�e�[�W�̃e�L�X�g�t�@�C����
	const std::string fileName = "Data/Stage/TutorialStage.txt";

	const TextMessage TEXT_MESSEAGE[50]
	{
		{L"neck run �̐��E�ւ悤�����I"},
		{L"���̃Q�[���̗V�ѕ����������"},
		{L"�܂��́A�v���C���[�ɂ��Đ��������"},
																		
		{L"�n�߂ɁA�v���C���[�͏�ɉ��֍s���������",true} ,
		{L"���̃v���C���[���S�[���ɂ��ǂ蒅�����邱�Ƃ��ł�����Q�[���N���A",},

		{L"���ꂶ�Ⴀ�A������@�ɂ��ċ������"},
		{L"�v���C���[����l�����ˁH"},
		{L"���̓�l�͂ǂ�������������Ƃ��ł����"} ,
		{L"������WASD�L�[�A�Ԃ������\���L�[�œ��������"},
		{L"�R���g���[���[�Ȃ���E�X�e�B�b�N�A�Ԃ����X�e�B�b�N����"},
		{L"�����������Ă݂悤�I",	true},
		{L"" },
		{L"�����I"},

		{L"���ɁA���x�̓M�~�b�N�ɂ��Đ�������ˁH" },
		{L"��l�̊Ԃɖ_������̂������邩�ȁH"},
		{L"���̖_�́A��l������邱�Ƃ̂ł��Ȃ��悤�ɂ��Ă����"} ,
		{L"��l�̋������J���قǁA���̖_���L�тĂ�����"},
		{L"�����āA�_���L�т�ΐL�т�قǁA��l�����ɍs�����x���オ���"},
		{L"���삪����Ȃ锼�ʁA�^�C���̒Z�k���_�����",},
		{L"�m�F���Ă݂悤�I",true},
		{L""} ,
		{L"�����������ȁH"} ,

		{L"���������A���̖_�̒����ɂ͌��E������񂾂�"},
		{L"��l�̋������J��������ƁA���E���}�����Ⴄ��" }	,
		{L"���������Ȃ�ƁA��l�͋����I�ɐ^�񒆂Ɉ����񂹂����"}	,
		{L"�ً}����ɂ��g���邯�ǁA�_���Z���Ȃ邩��A�^�C���͈����Ȃ邩����"},
		{L"�_���Ԃ��Ȃ���������������E���}���鍇�}����"},
		{L"���ꂶ�Ⴀ�A����Ă݂悤�I",true},
		{L""},
		{L"���������I"} ,

		{L"�悵�A���͏�Q���ɂ��ċ�����ˁI"},
		{L"������ƈړ������",true},

		{L"�O�ɂ����Q���������邩�ȁH"},
		{L"��l�͂������]���Ȃ���A�i�܂Ȃ��Ƃ����Ȃ���"},
		{L"������ƁE�E�E",true},
		{L""},

		{L"����ȕ��Ɍ��ɉ������āA�i�ނ̂������x���Ȃ��"},
		{L"�ɗ́A�����Đi�񂾕���������"},

		{L"�Ō�ɁA��Q�����_�ɓ����������̐����������"},
		{L"���ɂ��钷����Q��������̂������邩�ȁH"},
		{L"���̒������ƁA�_�ɓ��������Ⴄ��"},
		{L"�����_�ɓ��������Ⴄ�ƁA�Q�[���I�[�o�[�ɂȂ��"},
		{L"�Ȃɂ������Ă���΂ɖ_�ɓ��Ă���Ȃ��悤�ɂ��Ȃ��Ƒʖڂ���"},
		{L"�����A����͂킴�Ɠ��ĂĂ݂悤��",true},
		{L""},

		{L"�{���Ȃ炱��ŃQ�[���I�[�o�[�ɂȂ��"},
		{L"�C�����Ă�"},
		{L"���āA�`���[�g���A���͂���ŏI��肾��"},
		{L"�{�҂��y����łˁI",true},
	};


};