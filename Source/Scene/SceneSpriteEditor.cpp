#include <tchar.h>
#include <DirectXMath.h>
#include <filesystem>
#include "SceneSpriteEditor.h"
#include "SceneManager.h"
// --- external ---
#include "../../External/ImGui/imgui.h"
// --- libarary ---
#include "../../Library/Framework.h"
#include "../../Library/Timer.h"
#include "../../Library/RegisterNum.h"
#include "../../Library/Math/OperatorXMFloat3.h"
#include "../../Library/ImGui/ImGuiManager.h"
#include "../../Library/Input/InputManager.h"
#include "../../Library/Graphics/Graphics.h"
#include "../../Library/3D/LineRenderer.h"

void SceneSpriteEditor::Initialize()
{
	Framework* frameWork = &Framework::Instance();

	spr1 = std::make_unique<Sprite>(L"Data/Texture/Nessie.png");

	imGuiFrameBuffer = std::make_unique<FrameBuffer>(frameWork->GetScreenWidthF(), frameWork->GetScreenHeightF(), true);
}

void SceneSpriteEditor::Finalize()
{
}

void SceneSpriteEditor::Update()
{
	// --- inputManager���� ---
	if (isMouseHoverSceneView)
		InputManager::Instance().Update();

	// --- imguiManager���� ---
	ImGuiManager::Instance().Update();
}

void SceneSpriteEditor::Render()
{
	// �K�v�ȃ|�C���^�擾
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// renderTarget�̐ݒ�
	dc->OMSetRenderTargets(1, gfx->GetRTVAddress(), gfx->GetDSV());

	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);


	// --- imGuiFrameBuffer �ɏ������� ---
	imGuiFrameBuffer->Clear(gfx->GetBgColor());
	imGuiFrameBuffer->Activate();

	// �x�[�X�̃��C���`��(��ɏ����Ă���)
	//DrawGrid(10, 1.0f);
	LineRenderer::Instance().Render();

	// rasterizerState�̐ݒ�
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);
	// depthStencilState�̐ݒ�
	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	// blendState�̐ݒ�
	gfx->SetBlend(BLEND_STATE::ALPHA);

	spr1->Render();

	imGuiFrameBuffer->DeActivate();

	// --- �f�o�b�O�`�� ---
	DrawDebugGUI();

	// --- �p�t�H�[�}���X�`�� ---
	ImGuiManager::Instance().DisplayPerformanceStats();

	// --- imguiManager �`�� ---
	ImGuiManager::Instance().Render();
}

// �f�o�b�O�`��
void SceneSpriteEditor::DrawDebugGUI()
{
	Graphics* gfx = &Graphics::Instance();

	// ���f���G�f�B�^�[�p���j���[�o�[�`��
	DrawSpriteEditorMenuBar();

	ImGui::Begin("Inspector");
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

		// ImGui�̕`��\�̈�̎擾
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
	
	// --- Console�`�� ---
	ImGuiManager::Instance().Console();
}

// ���f���G�f�B�^�[�p���j���[�o�[�`��
void SceneSpriteEditor::DrawSpriteEditorMenuBar()
{
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Menu")) {
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("Title"))
				{
					SceneManager::Instance().ChangeScene(new SceneTitle);
				}
				if (ImGui::MenuItem("SpriteEditor"))
				{
					SceneManager::Instance().ChangeScene(new SceneSpriteEditor);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Load")) {
			if (ImGui::MenuItem(".imageFormat"))
			{
				Timer::Instance().Stop();

				OPENFILENAME ofn;       // �t�@�C���I��p�̍\����
				TCHAR szFile[260] = { 0 };  // �t�@�C���p�X���i�[����o�b�t�@

				// �\���̂̏�����
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter = _TEXT("�S�Ẵt�@�C��(*.*)\0*.*\0");
				ofn.lStructSize = sizeof(ofn);
				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileName(&ofn))
				{
					// --- filesystem �ɂ��path�ϊ� ---

					std::filesystem::path selectedPath(szFile);
					// ���݂̃f�B���N�g���̃p�X���擾
					std::filesystem::path currentPath = std::filesystem::current_path();
					// �I�����ꂽ�t�@�C���ւ̑��΃p�X���擾
					std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
					std::string fbxPath = relativePath.string();

					// ���f���Ǎ�
					//modelObject = std::make_unique<ModelEditorObject>(fbxPath.c_str());
				}

				Timer::Instance().Start();
			}
			if (ImGui::MenuItem(".sprite"))
			{
				Timer::Instance().Stop();

				OPENFILENAME ofn;       // �t�@�C���I��p�̍\����
				TCHAR szFile[260] = { 0 };  // �t�@�C���p�X���i�[����o�b�t�@

				// �\���̂̏�����
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter = _TEXT("sprite�t�@�C��(*.sprite)\0*.sprite\0") _TEXT("�S�Ẵt�@�C��(*.*)\0*.*\0");
				ofn.lStructSize = sizeof(ofn);
				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				if (GetOpenFileName(&ofn))
				{
					// --- filesystem �ɂ��path�ϊ� ---

					std::filesystem::path selectedPath(szFile);
					// ���݂̃f�B���N�g���̃p�X���擾
					std::filesystem::path currentPath = std::filesystem::current_path();
					// �I�����ꂽ�t�@�C���ւ̑��΃p�X���擾
					std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
					std::string fbxPath = relativePath.string();

					// ���f���Ǎ�
					//modelObject = std::make_unique<ModelEditorObject>(fbxPath.c_str());
				}

				Timer::Instance().Start();
			}
			
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Save"))
		{
			if (ImGui::MenuItem(".model"))
			{
				// ������ .sprite �����o��
				//modelObject->GetModel()->GetModelResource()->OutputModelData();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

