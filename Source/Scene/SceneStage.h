#pragma once
#include <memory>
// --- Library ---
#include "../../Library/2D/Sprite.h"
#include "../../Library/Graphics/FrameBuffer.h"
#include "../../Library/Graphics/FullScreenQuad.h"
#include "../../Library/Graphics/Bloom.h"
#include "../../Library/Graphics/Shadow.h"
#include "../../Library/2D/Sprite.h"
#include "../../Library/Particle/Emitter.h"
#include "../../Library/3D/LineRenderer.h"
#include "../../Library/3D/DebugPrimitive.h"
// --- external ---
#include "../../External/ImGui/imgui.h"
// --- Scene ---
#include "Scene.h"
// --- Game ---
#include "../Game/Object/TestStatic.h"
#include "../Game/Object/TestAnimated.h"

enum eObjectType {
	Kesigomu = 0,
	Pentate = 1,
	Enpitu = 2,
	Tokei = 3,
	Max
};

class Object3D : public StaticObject {
public:
	struct Transform {
		DirectX::XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
		float radius = 17.0f;
	};

	struct Collision {
		DirectX::XMFLOAT3 pos{ 0.0f, 0.0f, 0.0f };
		float radius = 17.0f;
	};

	Collision initCollision;
	std::vector<Collision> collisions;

	Object3D(const char* filePath, eObjectType type) : StaticObject(filePath), type(type) 
	{
		//if (type == eObjectType::Enpitu) {
		//	initCollision.radius = 17.0f;
		//}
	}

	~Object3D() override {}

	void Update() override 
	{
		// 姿勢行列更新
		UpdateTransform();

		int bufferPos = 5;

		DirectX::XMMATRIX T;

		collisions.clear();
		for (int i = 0; i < active; i++) {
			if (type == eObjectType::Enpitu) {
				const float add = 33.0f;

				auto& t = transforms[i];
				DirectX::XMFLOAT3 pos = t.pos;
				collisions.emplace_back(Collision{ pos, t.radius });
				pos.y += add;
				collisions.emplace_back(Collision{ pos, t.radius });
				pos.y += add;
				collisions.emplace_back(Collision{ pos, t.radius });
				pos.y += add;
				collisions.emplace_back(Collision{ pos, t.radius });

			}
			else
			{
				auto& t = transforms[i];
				DirectX::XMFLOAT3 pos = t.pos;
				collisions.emplace_back(Collision{ pos, t.radius });
			}
		}

		for (int i = 0; i < instanceNum; i++)
		{
			DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
			
			int s = transforms.size();
			if (i >= 0 && i < transforms.size() && transforms.size() != 0) {
				Transform& t = transforms[i];
				T = DirectX::XMMatrixTranslation(t.pos.x, t.pos.y, t.pos.z);
			}
			else {
				T = DirectX::XMMatrixTranslation(0.0f, -2.0f, 0.0f);
			}
			
			DirectX::XMStoreFloat4x4(&m[i], W * T);
		}
	}
	void Render(bool isShadow = false) override
	{

		model->Render(active, m, isShadow);


		LineRenderer& lineRenderer = LineRenderer::Instance();
		// Create vertical lines
		//float scaling = static_cast<float>(subdivisions) * scale;
		float scaling = 1.0f;
		DirectX::XMMATRIX M = DirectX::XMMatrixScaling(scaling, scaling, scaling);
		DirectX::XMVECTOR V, P;
		DirectX::XMFLOAT3 position;

		//座標軸
		/*
		for (auto& t : transforms) {
			// X軸
			{
				const DirectX::XMFLOAT4 red = DirectX::XMFLOAT4(1, 0, 0, 1);
				V = DirectX::XMVectorSet(t.pos.x, t.pos.y, t.pos.z, 0);
				P = DirectX::XMVector3TransformCoord(V, M);
				DirectX::XMStoreFloat3(&position, P);
				lineRenderer.AddVertex(position, red);

				V = DirectX::XMVectorSet(t.pos.x + 3.0f, t.pos.y, t.pos.z, 0);
				P = DirectX::XMVector3TransformCoord(V, M);
				DirectX::XMStoreFloat3(&position, P);
				lineRenderer.AddVertex(position, red);
			}

			// Y軸
			{
				const DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0, 1, 0, 1);
				V = DirectX::XMVectorSet(t.pos.x, t.pos.y, t.pos.z, 0);
				P = DirectX::XMVector3TransformCoord(V, M);
				DirectX::XMStoreFloat3(&position, P);
				lineRenderer.AddVertex(position, green);

				V = DirectX::XMVectorSet(t.pos.x, t.pos.y + 3.0f, t.pos.z, 0);
				P = DirectX::XMVector3TransformCoord(V, M);
				DirectX::XMStoreFloat3(&position, P);
				lineRenderer.AddVertex(position, green);
			}

			// Z軸
			{
				const DirectX::XMFLOAT4 blue = DirectX::XMFLOAT4(0, 0, 1, 1);
				V = DirectX::XMVectorSet(t.pos.x, t.pos.y, t.pos.z, 0);
				P = DirectX::XMVector3TransformCoord(V, M);
				DirectX::XMStoreFloat3(&position, P);
				lineRenderer.AddVertex(position, blue);

				V = DirectX::XMVectorSet(t.pos.x, t.pos.y, t.pos.z + 3.0f, 0);
				P = DirectX::XMVector3TransformCoord(V, M);
				DirectX::XMStoreFloat3(&position, P);
				lineRenderer.AddVertex(position, blue);
			}
		}
		*/

		//コリジョン
		if (!isShadow) {
			for (auto& t : collisions) {
				DebugPrimitive::Instance().AddSphere(t.pos, t.radius, { 1.0f, 1.0f, 1.0f, 1.0f });
			}
		}

	}

	void SetObjectType(const eObjectType type) {
		this->type = type;
	}

	int GetActive() { return active; }

	void Add(const DirectX::XMFLOAT3& pos) {
		transforms.emplace_back(Object3D::Transform{ pos });
		active++;
	}

	void UpdateImGui(const char* objectName) noexcept {

		std::string objName{ objectName };
		;

		//オブジェクトを追加
		if (ImGui::Button((objName + "Add Object").c_str())) {
			//transforms.emplace_back(Transform{});
			//active++;
			Add({});
		}

		/*全てのオブジェクトを操作*/
#if 1     
		//オブジェクト　インスペクター
		std::string pos_tag{ "Position" + objName };
		std::string rem_tag{ "Remove " + objName };
		for (int i = 0; i < transforms.size(); i++) {

			//位置を調整する
			Transform& t = transforms[i];
			float* d[]{&t.pos.x, &t.pos.y, &t.pos.z};
			//ImGui::SliderFloat3((pos_tag + to_string(i)).c_str(), *d, -10.0f, 10.0f);
			ImGui::InputFloat3((pos_tag + std::to_string(i)).c_str(), *d);
		
			//オブジェクトを取り除く
			if (ImGui::Button((rem_tag + std::to_string(i)).c_str())) {
				transforms.erase(transforms.begin() + i);
				i--;
				active--;
			}
		
		}

#else
		/*指定したオブジェクトのみを操作*/

#endif
	}

	
	const Transform& GetTransform(const int index) {
		return transforms[index];
	}

	std::vector<Transform> transforms;
	

	void Clear() {
		transforms.clear();
		active = 0;
	}

	//float GetRadius() { return radius; }
	
private:
	static const int instanceNum = 100;
	DirectX::XMFLOAT4X4 m[instanceNum] = {};

	int active = 0;

	eObjectType type;

	//float radius = 1.0f;
};

// ステージ　シーン
class SceneStage : public Scene
{
public:
	SceneStage() {}
	~SceneStage() override {};

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

	char filename[128] = "./Data/Stage/Stage.txt";

	//ステージ情報をテキストファイルに出力する
	bool SaveFileStage(const char* filename);

	//指定したファイルからステージ情報を読み取る
	bool LoadFileStage(const char* filename);

	//ステージを初期化する
	void ClearStage();

	void StageCollision();

	void StageVsRope();

private:
	std::unique_ptr<FrameBuffer> frameBuffer;
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	std::unique_ptr<Bloom> bloom;
	std::unique_ptr<Shadow> shadow;

	std::unique_ptr<TestStatic> testStatic;
	std::unique_ptr<TestAnimated> testAnimated;
	//std::unique_ptr<Sprite> sprTest;
	//std::unique_ptr<Sprite> sprTest2;
	//std::unique_ptr<Sprite> sprTest3;
	//std::unique_ptr<Emitter> emitter1;

	

	const char* objectNames[eObjectType::Max]{"Kesigomu", "Pentate", "Enpitu", "Tokei" };

	std::unordered_map<int, std::unique_ptr<Object3D>>  objects;
	std::vector<Object3D::Transform> transforms;

	std::vector<Object3D::Collision> collisions;
};