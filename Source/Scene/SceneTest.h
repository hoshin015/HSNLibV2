#pragma once
#include <memory>
#include "Scene.h"
#include "../../Library/2D/Sprite.h"
#include "../../Library/Graphics/FrameBuffer.h"
#include "../Game/Object/TestStatic.h"
#include "../../Library/2D/Sprite.h"


// �e�X�g�V�[��
class SceneTest : public Scene
{
public:
	SceneTest() {}
	~SceneTest() override {};

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
private:
	std::unique_ptr<TestStatic> testStatic;
	std::unique_ptr<Sprite> sprTest;
};