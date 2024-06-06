#pragma once

#include "Scene.h"
#include "SceneStage.h"
#include "../../Source/Game/Object/Player/PlayerManager.h"

class SceneGame3 : public Scene
{
public:
	SceneGame3() {}
	~SceneGame3() {}

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Render() override;


private:
	// デバッグ描画
	void DrawDebugGUI();

	void StageCollision();

	void StageVsRope();

private:
	//カメラを設定するための数値
	DirectX::XMFLOAT3 cameraOffset = { 0,250,600 };
	float cameraAngle = 15.0f;

	std::unordered_map<int, std::unique_ptr<Object3D>>  objects;
	std::vector<Object3D::Transform> transforms;

	std::vector<Object3D::Collision> collisions;

	std::unique_ptr<FrameBuffer> frameBuffer;
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	std::unique_ptr<Bloom> bloom;
	std::unique_ptr<Shadow> shadow;


	//コントローラーの振動のタイマー
	float controllerTimer = 0.0f;
};