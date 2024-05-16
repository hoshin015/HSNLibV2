#include <tchar.h>
#include <DirectXMath.h>
#include <filesystem>
#include "SceneModelEditor.h"
#include "SceneManager.h"
// --- external ---
#include "../../External/ImGui/imgui.h"
// --- libarary ---
#include "../../Library/Framework.h"
#include "../../Library/Timer.h"
#include "../../Library/RegisterNum.h"
#include "../../Library/Math/OperatorXMFloat3.h"
#include "../../Library/Effekseer/EffectManager.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/3D/Camera.h"
#include "../../Library/3D/LineRenderer.h"
#include "../../Library/3D/LightManager.h"

void SceneModelEditor::Initialize()
{
	Framework* frameWork = &Framework::Instance();

	imGuiFrameBuffer = std::make_unique<FrameBuffer>(frameWork->GetScreenWidthF(), frameWork->GetScreenHeightF(), true);

	// --- カメラ初期設定 ---
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 5, 20),		// カメラ座標
		DirectX::XMFLOAT3(0, 0, 0),			// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);
	Camera::Instance().SetAngle(
		{ DirectX::XMConvertToRadians(-45), DirectX::XMConvertToRadians(45), 0 }
	);

	Camera::Instance().cameraType = Camera::CAMERA::MODEL_EDITOR;

	// ライト初期設定
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(-1, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
	LightManager::Instance().SetAmbientColor({ 0.2f, 0.2f, 0.2f, 1.0f });
}

void SceneModelEditor::Finalize()
{
	LightManager::Instance().Clear();
}

void SceneModelEditor::Update()
{
	// --- inputManager処理 ---
	if(isMouseHoverSceneView)
		InputManager::Instance().Update();

	// --- imguiManager処理 ---
	ImGuiManager::Instance().Update();

	// --- effectManager処理 ---
	EffectManager::Instance().Update();

	// --- カメラ処理 ---
	Camera::Instance().Update();

	if(modelObject) modelObject->Update();

}

void SceneModelEditor::Render()
{
	// 必要なポインタ取得
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// renderTargetの設定
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// rasterizerStateの設定
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilStateの設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendStateの設定
	gfx->SetBlend(BLEND_STATE::ALPHA);


	// カメラの定数バッファの更新
	Camera::Instance().UpdateCameraConstant();

	// ライトの定数バッファの更新
	LightManager::Instance().UpdateConstants();


	// --- imGuiFrameBuffer に書きこむ ---
	imGuiFrameBuffer->Clear(gfx->GetBgColor());
	imGuiFrameBuffer->Activate();
	
	// ベースのライン描画(先に書いておく)
	DrawGrid(10, 1.0f);
	LineRenderer::Instance().Render();

	// rasterizerStateの設定
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilStateの設定
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendStateの設定
	gfx->SetBlend(BLEND_STATE::ALPHA);

	if(modelObject) modelObject->Render();

	imGuiFrameBuffer->DeActivate();

	// --- デバッグ描画 ---
	DrawDebugGUI();

	LightManager::Instance().DrawDebugGui();

	// --- パフォーマンス描画 ---
	ImGuiManager::Instance().DisplayPerformanceStats();

	// --- imguiManager 描画 ---
	ImGuiManager::Instance().Render();
}

// デバッグ描画
void SceneModelEditor::DrawDebugGUI()
{
	Graphics* gfx = &Graphics::Instance();
	
	// モデルエディター用メニューバー描画
	DrawModelEditorMenuBar();

	ImGui::Begin("Inspector");
	if (modelObject)
	{
		if (ImGui::CollapsingHeader("Object", ImGuiTreeNodeFlags_None))
		{
			DirectX::XMFLOAT3 pos = modelObject->GetPos();
			DirectX::XMFLOAT3 angle = modelObject->GetAngle();
			float scale = modelObject->GetScaleX();
			ImGui::DragFloat3("POSITION", &pos.x, 0.1f);
			ImGui::DragFloat3("ANGLE", &angle.x);
			ImGui::DragFloat("SCALE", &scale, 0.01f);
			modelObject->SetPos(pos);
			modelObject->SetAngle(angle);
			modelObject->SetScale({ scale, scale, scale });
		}

		if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_None))
		{

			ImGui::InputText("Path", &modelObject->GetModel()->GetModelResource()->GetFilePath()[0], modelObject->GetModel()->GetModelResource()->GetFilePath().size() + 1);

			ImGui::Separator();

			//===== coordinateSystemTransform =====
			const char* coordinateSystemTransformName[] = { "RHS Y - UP", "LHS Y - UP", "RHS Z - UP", "LHS Z - UP" };
			if (ImGui::BeginCombo("Axis", coordinateSystemTransformName[modelObject->GetModel()->GetModelResource()->GetCoordinateSystemTransformType()]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(coordinateSystemTransformName); i++)
				{
					const bool isSelected = (modelObject->GetModel()->GetModelResource()->GetCoordinateSystemTransformType() == i);
					if (ImGui::Selectable(coordinateSystemTransformName[i], isSelected))
					{
						modelObject->GetModel()->GetModelResource()->SetCoordinateSystemTransformType(static_cast<CoordinateSystemTransform>(i));
						modelObject->GetModel()->GetModelResource()->SetCoordinateSystemTransform(coordinateSystemTransform[i]);
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			// scale
			{
				float s = modelObject->GetModel()->GetModelResource()->GetScale();
				ImGui::DragFloat("Scale", &s, 0.001f);
				modelObject->GetModel()->GetModelResource()->SetScale(s);
			}
			
#pragma region Mesh
			if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_None))
			{
				for (auto& mesh : modelObject->GetModel()->GetModelResource()->GetMeshes())
				{
					ImGui::Indent(); // 子ヘッダーを少し右にずらす
					if (ImGui::TreeNode(mesh.meshName.c_str()))
					{
						ImGui::InputText("id", &std::to_string(mesh.uniqueId)[0], std::to_string(mesh.uniqueId).size() + 1);
						ImGui::TreePop();
					}
					ImGui::Unindent(); // インデントを元に戻す
				}
			}
#pragma endregion

#pragma region Material
			if (!modelObject->GetModel()->GetModelResource()->GetMaterials().empty())
			{
				if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_None))
				{
					for (auto& [name, material] : modelObject->GetModel()->GetModelResource()->GetMaterials())
					{
						ImGui::Indent(); // 子ヘッダーを少し右にずらす

						if (ImGui::TreeNode(material.name.c_str()))
						{
							const char* textureLabelNames[_textureNum] = { "Diffuse", "Normal", "Specular", "Emissive" };
							for (int textureIndex = 0; textureIndex < _textureNum; textureIndex++)
							{

								ImGui::InputText(textureLabelNames[textureIndex], &material.textureFilenames[textureIndex][0], material.textureFilenames[_deffuseTexture].size() + 1);
								ImGui::Image(material.shaderResourceViews[textureIndex].Get(), { 64,64 });
								ImGui::SameLine();
								std::string buttonLabel = textureLabelNames[textureIndex] + std::to_string(textureIndex);
								if (ImGui::Button(buttonLabel.c_str()))
								{
									OPENFILENAME ofn;       // ファイル選択用の構造体
									TCHAR szFile[260] = { 0 };  // ファイルパスを格納するバッファ

									// 構造体の初期化
									ZeroMemory(&ofn, sizeof(ofn));
									ofn.lpstrFilter = _TEXT("pngファイル(*.png)\0*.png\0") _TEXT("全てのファイル(*.*)\0*.*\0");
									ofn.lStructSize = sizeof(ofn);
									ofn.lpstrFile = szFile;
									ofn.lpstrFile[0] = '\0';
									ofn.nMaxFile = sizeof(szFile);
									ofn.nFilterIndex = 1;
									ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

									if (GetOpenFileName(&ofn))
									{
										// --- filesystem によるpath変換 ---

										std::filesystem::path selectedPath(szFile);
										// 現在のディレクトリのパスを取得
										std::filesystem::path currentPath = std::filesystem::current_path();


										// 選択されたファイルへの相対パスを取得
										std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);


										// 開いてるモデルのパスを取得
										std::filesystem::path modelPath(modelObject->GetModel()->GetModelResource()->GetFilePath());
										std::filesystem::path modelParebtPath = modelPath.parent_path();

										relativePath = relativePath.lexically_relative(modelParebtPath);
										std::string filePath = relativePath.string();

										// テクスチャ差し替え
										material.textureFilenames[textureIndex] = filePath;

										for (auto& [name, material] : modelObject->GetModel()->GetModelResource()->GetMaterials())		// 構造化束縛
										{
											for (int textureIndex = 0; textureIndex < 4; textureIndex++)
											{
												if (material.textureFilenames[textureIndex].size() > 0)
												{
													std::filesystem::path path(modelObject->GetModel()->GetModelResource()->GetFilePath());
													path.replace_filename(material.textureFilenames[textureIndex]);
													D3D11_TEXTURE2D_DESC texture2dDesc{};
													LoadTextureFromFile(path.c_str(), material.shaderResourceViews[textureIndex].GetAddressOf(), &texture2dDesc);
												}
												else
												{
													LoadFbx::Instance().MakeDummyTexture(material.shaderResourceViews[textureIndex].GetAddressOf(), textureIndex == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);
												}
											}
										}
									}
								}
							}

							ImGui::TreePop();
						}

						ImGui::Unindent(); // インデントを元に戻す
					}
				}
			}
#pragma endregion

#pragma region Skeleton
			bool isShowSkeletonData = false;
			for (auto& mesh : modelObject->GetModel()->GetModelResource()->GetMeshes())
			{
				if (!mesh.skeleton.bones.empty()) isShowSkeletonData = true;
			}
			if (isShowSkeletonData)
			{
				if (ImGui::CollapsingHeader("Skeleton", ImGuiTreeNodeFlags_None))
				{
					for (auto& mesh : modelObject->GetModel()->GetModelResource()->GetMeshes())
					{
						ImGui::Indent(); // 子ヘッダーを少し右にずらす
						std::string meshName = mesh.meshName + " (mesh)";
						if (ImGui::TreeNode(meshName.c_str()))
						{
							// 再帰的にBoneを描画するラムダ式
							DrawBoneDebug(mesh.skeleton.bones, 0);
							ImGui::TreePop();
						}
						ImGui::Unindent(); // インデントを元に戻す
					}
				}
			}
#pragma endregion

#pragma region Animation
			if (!modelObject->GetModel()->GetModelResource()->GetAnimationClips().empty())
			{
				if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_None))
				{
					int index = 0;
						for (auto& animationClips : modelObject->GetModel()->GetModelResource()->GetAnimationClips())
						{
							ImGui::PushID(index);
							ImGui::Indent();		// 子ヘッダーを少し右にずらす

							ImGuiManager::Instance().InputText("name", animationClips.name);
							if (ImGui::Button("Play"))
							{
								modelObject->PlayAnimation(index, true);
							}
							ImGui::SameLine();
							if (ImGui::Button("Delete"))
							{
								// 親ウィンドウのハンドル（NULLの場合、メッセージボックスはオーナーレスとなる）
								HWND hwnd = NULL;
								// メッセージボックスのスタイル
								UINT boxStyle = MB_YESNO | MB_ICONQUESTION;

								// メッセージボックスを表示し、ユーザーの応答を取得
								int result = MessageBox(hwnd, L"本当に削除しますか？", L"警告", boxStyle);

								// ユーザーの応答に基づいて処理を行う
								if (result == IDYES)
								{
									// 削除
									std::filesystem::path path(modelObject->GetModel()->GetModelResource()->GetFilePath());
									std::string parentPath = path.parent_path().string();
									std::string deleteFilename = parentPath + "/Anims/" + modelObject->GetModel()->GetModelResource()->GetAnimationClips().at(index).name + ".anim";
									std::filesystem::remove(deleteFilename);

									modelObject->GetModel()->GetModelResource()->GetAnimationClips().erase(modelObject->GetModel()->GetModelResource()->GetAnimationClips().begin() + index);

									// 削除後
									modelObject->SetCurrentAnimationIndex(0);
									modelObject->SetCurrentKeyFrame(0);
									modelObject->SetCurrentAnimationSeconds(0.0f);
								}
							}
							ImGui::Text(("secondsLength : " + std::to_string(animationClips.secondsLength)).c_str());

							ImGui::Separator();

							ImGui::Unindent();		// インデントを元に戻す
							ImGui::PopID();

							index++;
						}
				}
			}
#pragma endregion

		}


	}
	ImGui::End();




	ImGui::Begin("Scene");
	{
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();

		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;
		vMax.x += ImGui::GetWindowPos().x;
		vMax.y += ImGui::GetWindowPos().y;

		isMouseHoverSceneView = ImGui::IsMouseHoveringRect(vMin, vMax);

		// ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 0, 0, 255));

		// ImGuiの描画可能領域の取得
		ImVec2 WindowSize(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		DirectX::XMFLOAT3 imWindowSize = { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y, 0 };
		DirectX::XMFLOAT3 windowSize = { static_cast<float>(Framework::Instance().GetScreenWidth()), static_cast<float>(Framework::Instance().GetScreenHeight()), 0 };

		DirectX::XMFLOAT3 aspectWindow = imWindowSize / windowSize;
		DirectX::XMFLOAT3 drawWindowSize = { 0,0,0 };
		DirectX::XMFLOAT3 drawPos = { 0,0,0 };

		if (aspectWindow.x > aspectWindow.y)
		{
			drawWindowSize.x = imWindowSize.y / windowSize.y * windowSize.x;
			drawWindowSize.y = imWindowSize.y;
			drawPos.x = (imWindowSize.x - drawWindowSize.x) / 2;
		}
		else
		{
			drawWindowSize.x = imWindowSize.x;
			drawWindowSize.y = imWindowSize.x / windowSize.x * windowSize.y;
			drawPos.y = (imWindowSize.y - drawWindowSize.y) / 2;
		}

		ImGui::SetCursorPos({ ImGui::GetWindowContentRegionMin().x + drawPos.x, ImGui::GetWindowContentRegionMin().y + drawPos.y });
		ImGui::Image(imGuiFrameBuffer->shaderResourceViews[0].Get(), { drawWindowSize.x, drawWindowSize.y }, { 0, 0 }, { 1,1 }, { 1, 1, 1, 1 });
	}
	ImGui::End();



	// --- Console描画 ---
	ImGuiManager::Instance().Console();
}

// モデルエディター用メニューバー描画
void SceneModelEditor::DrawModelEditorMenuBar()
{
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Menu")) {
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("Title"))
				{
					SceneManager::Instance().ChangeScene(new SceneTitle);
				}
				if (ImGui::MenuItem("ModelEditor"))
				{
					SceneManager::Instance().ChangeScene(new SceneModelEditor);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Load")) {
			if (ImGui::MenuItem(".fbx"))
			{
				Timer::Instance().Stop();

				OPENFILENAME ofn;       // ファイル選択用の構造体
				TCHAR szFile[260] = { 0 };  // ファイルパスを格納するバッファ

				// 構造体の初期化
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter = _TEXT("fbxファイル(*.fbx)\0*.fbx\0") _TEXT("全てのファイル(*.*)\0*.*\0");
				ofn.lStructSize = sizeof(ofn);
				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileName(&ofn))
				{
					// --- filesystem によるpath変換 ---

					std::filesystem::path selectedPath(szFile);
					// 現在のディレクトリのパスを取得
					std::filesystem::path currentPath = std::filesystem::current_path();
					// 選択されたファイルへの相対パスを取得
					std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
					std::string fbxPath = relativePath.string();

					// モデル読込
					modelObject = std::make_unique<ModelEditorObject>(fbxPath.c_str());
				}

				Timer::Instance().Start();
			}
			if (ImGui::MenuItem(".model"))
			{
				Timer::Instance().Stop();

				OPENFILENAME ofn;       // ファイル選択用の構造体
				TCHAR szFile[260] = { 0 };  // ファイルパスを格納するバッファ

				// 構造体の初期化
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter = _TEXT("modelファイル(*.model)\0*.model\0") _TEXT("全てのファイル(*.*)\0*.*\0");
				ofn.lStructSize = sizeof(ofn);
				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileName(&ofn))
				{
					// --- filesystem によるpath変換 ---

					std::filesystem::path selectedPath(szFile);
					// 現在のディレクトリのパスを取得
					std::filesystem::path currentPath = std::filesystem::current_path();
					// 選択されたファイルへの相対パスを取得
					std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
					std::string modelPath = relativePath.string();

					// モデル読込
					modelObject = std::make_unique<ModelEditorObject>(modelPath.c_str());
				}
				Timer::Instance().Start();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::MenuItem(".fbx"))
			{
				Timer::Instance().Stop();

				OPENFILENAME ofn;       // ファイル選択用の構造体
				TCHAR szFile[260] = { 0 };  // ファイルパスを格納するバッファ

				// 構造体の初期化
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter = _TEXT("fbxファイル(*.fbx)\0*.fbx\0") _TEXT("全てのファイル(*.*)\0*.*\0");
				ofn.lStructSize = sizeof(ofn);
				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileName(&ofn))
				{
					// --- filesystem によるpath変換 ---

					std::filesystem::path selectedPath(szFile);
					// 現在のディレクトリのパスを取得
					std::filesystem::path currentPath = std::filesystem::current_path();
					// 選択されたファイルへの相対パスを取得
					std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
					std::string fbxPath = relativePath.string();

					// アニメーション追加
					LoadFbx::Instance().AppendAnimation(fbxPath.c_str(), modelObject->GetModel()->GetModelResource(), 30);
				}
				Timer::Instance().Start();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem(".model"))
			{
				// ここで .model 書き出し
				modelObject->GetModel()->GetModelResource()->OutputModelData();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

// グリッド描画
void SceneModelEditor::DrawGrid(int subdivisions, float scale)
{
	int numLines = (subdivisions + 1) * 2;
	int vertexCount = numLines * 2;

	float corner = 0.5f;
	float step = 1.0f / static_cast<float>(subdivisions);

	int index = 0;
	float s = -corner;

	const DirectX::XMFLOAT4 white = DirectX::XMFLOAT4(1, 1, 1, 1);

	LineRenderer& lineRenderer = LineRenderer::Instance();
	// Create vertical lines
	float scaling = static_cast<float>(subdivisions) * scale;
	DirectX::XMMATRIX M = DirectX::XMMatrixScaling(scaling, scaling, scaling);
	DirectX::XMVECTOR V, P;
	DirectX::XMFLOAT3 position;
	for (int i = 0; i <= subdivisions; i++)
	{
		V = DirectX::XMVectorSet(s, 0, corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, white);

		V = DirectX::XMVectorSet(s, 0, -corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, white);

		s += step;
	}

	// Create horizontal lines
	s = -corner;
	for (int i = 0; i <= subdivisions; i++)
	{
		V = DirectX::XMVectorSet(corner, 0, s, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, white);

		V = DirectX::XMVectorSet(-corner, 0, s, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, white);

		s += step;
	}

	// X軸
	{
		const DirectX::XMFLOAT4 red = DirectX::XMFLOAT4(1, 0, 0, 1);
		V = DirectX::XMVectorSet(0, 0, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, red);

		V = DirectX::XMVectorSet(corner, 0, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, red);
	}

	// Y軸
	{
		const DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0, 1, 0, 1);
		V = DirectX::XMVectorSet(0, 0, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, green);

		V = DirectX::XMVectorSet(0, corner, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, green);
	}

	// Z軸
	{
		const DirectX::XMFLOAT4 blue = DirectX::XMFLOAT4(0, 0, 1, 1);
		V = DirectX::XMVectorSet(0, 0, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, blue);

		V = DirectX::XMVectorSet(0, 0, corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, blue);
	}
}

void SceneModelEditor::DrawBoneDebug(const std::vector<ModelResource::Bone>& bones, int boneIndex)
{
	// boneIndexが有効でない場合は処理を終了する
	if (boneIndex < 0 || boneIndex >= bones.size())
		return;

	// 現在のボーンを描画
	const ModelResource::Bone& bone = bones[boneIndex];
	if (ImGui::TreeNode(bone.name.c_str()))
	{
		// 子ボーンがある場合、再帰的に描画する
		for (int i = 0; i < bones.size(); ++i)
		{
			if (bones[i].parentIndex == boneIndex)
			{
				DrawBoneDebug(bones, i);
			}
		}
		ImGui::TreePop();
	}
}

