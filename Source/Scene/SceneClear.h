#pragma once

#include "Scene.h"
#include "../../Library/2D/Sprite.h"
#include "../../Library/2D/Primitive2D.h"
#include <memory>

class SceneClear : public Scene
{
public:
    SceneClear(){}
    ~SceneClear(){}

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
	void ScorePerformRender();

public:
	const float SQUARE_SIZE = 64;

private:
    std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Primitive2D> primitive2d;

	bool isFinishFirstPerform = false;

	float goalPerformX = 0;
	float goalPerformTimer = 0.0f;

	bool onScorePerform = false;
	int scorePerformCurrentState = 0;
	int scorePerformNextState = 0;
	float scorePerformTimer = 0;
	std::wstringstream scorePerformString;
	float scorePerformColorA = 0.0f;
	DirectX::XMFLOAT2 drawPosRandumNum;

};