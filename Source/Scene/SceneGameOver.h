#pragma once

#include "Scene.h"
#include "../../Library/2D/Sprite.h"
#include <memory>
#include "../../Library/2D/Primitive2D.h"

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
	void FirstPerformRender();

public:
	const float SQUARE_SIZE = 64;

private:
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Primitive2D> primitive2d;


	bool isFinishFirstPerform = false;

	float goalPerformX = 0;
	float goalPerformTimer = 0.0f;

};