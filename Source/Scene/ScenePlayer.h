#pragma once

#include "Scene.h"
#include <memory>
#include "../../Source/Game/Object/Player/Player.h"

class ScenePlayer : public Scene
{
public:
    ScenePlayer() {}
    ~ScenePlayer()override {}

    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Render() override;

private:
    void DrawDebugGUI();

    std::unique_ptr<Player> player;

    DirectX::XMFLOAT3 cameraOffset = { 0,160,180 };
    float cameraAngle = 45.0f;
};