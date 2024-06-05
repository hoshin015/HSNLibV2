#pragma once

#include "Scene.h"
#include "../../Library/2D/Sprite.h"
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
    std::unique_ptr<Sprite> sprite;

};