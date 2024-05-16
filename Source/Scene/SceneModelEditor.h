#pragma once
#include <memory>
#include "Scene.h"
#include "../../Library/2D/Sprite.h"
#include "../../Library/Graphics/FrameBuffer.h"
#include "../../Library/Model/ModelResource.h"

#include "../Game/Object/ModelEditorObject.h"


// モデルエディターシーン
class SceneModelEditor : public Scene
{
public:
	SceneModelEditor() {}
	~SceneModelEditor() override {};

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
	// モデルエディター用メニューバー描画
	void DrawModelEditorMenuBar();
	// グリッド描画
	void DrawGrid(int subdivisions, float scale);

	// ボーン情報描画(ラムダ式とかつかってコード内にかきたい)
	void DrawBoneDebug(const std::vector<ModelResource::Bone>& bones, int boneIndex);

private:
	// 軸の修正行列
	const DirectX::XMFLOAT4X4 coordinateSystemTransform[4] =
	{
		{ -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	// 0:RHS Y-UP
		{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1	},	// 1:LHS Y-UP
		{ -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 2:RHS Z-UP
		{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },		// 3:LHS Z-UP
	};

private:
	bool isMouseHoverSceneView = false;				// マウスがシーンビュー上にあるか
	std::unique_ptr<FrameBuffer> imGuiFrameBuffer;	// ImGui描画用フレームバッファ

	std::unique_ptr<Sprite> spr1;
	std::unique_ptr<ModelEditorObject> modelObject;
};