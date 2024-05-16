#pragma once
#include <memory>
#include "Scene.h"
#include "../../Library/2D/Sprite.h"
#include "../../Library/Graphics/FrameBuffer.h"
#include "../../Library/Model/ModelResource.h"

#include "../Game/Object/ModelEditorObject.h"


// ���f���G�f�B�^�[�V�[��
class SceneModelEditor : public Scene
{
public:
	SceneModelEditor() {}
	~SceneModelEditor() override {};

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
	// ���f���G�f�B�^�[�p���j���[�o�[�`��
	void DrawModelEditorMenuBar();
	// �O���b�h�`��
	void DrawGrid(int subdivisions, float scale);

	// �{�[�����`��(�����_���Ƃ������ăR�[�h���ɂ�������)
	void DrawBoneDebug(const std::vector<ModelResource::Bone>& bones, int boneIndex);

private:
	// ���̏C���s��
	const DirectX::XMFLOAT4X4 coordinateSystemTransform[4] =
	{
		{ -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	// 0:RHS Y-UP
		{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1	},	// 1:LHS Y-UP
		{ -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 2:RHS Z-UP
		{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },		// 3:LHS Z-UP
	};

private:
	bool isMouseHoverSceneView = false;				// �}�E�X���V�[���r���[��ɂ��邩
	std::unique_ptr<FrameBuffer> imGuiFrameBuffer;	// ImGui�`��p�t���[���o�b�t�@

	std::unique_ptr<Sprite> spr1;
	std::unique_ptr<ModelEditorObject> modelObject;
};