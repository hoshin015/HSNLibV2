#pragma once

#include "Scene.h"
#include "../../Library/2D/Sprite.h"
#include <memory>

class SceneGameOver : public Scene
{
public:
	SceneGameOver() {}
	~SceneGameOver() {}

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V
	void Update() override;

	// �`��
	void Render() override;

private:
	std::unique_ptr<Sprite> sprite;

};