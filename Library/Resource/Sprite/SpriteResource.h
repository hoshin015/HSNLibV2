#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../MyCereal.h"

class SpriteResource
{
public:
	struct Animation
	{
		std::string name = "default"; 
		int frameNum = 1;			// ���v����
		int framePerRow = 1;		// ���̖���
		int frameWidth = 100;		// �P���̉���
		int frameHeight = 100;		// �P���̏c��
		int xCellOffset = 0;	
		int yCellOffset = 0;
		int xPixelOffset = 0;
		int yPixelOffset = 0;

		float secondsLength = 1.0f;	// �A�j���[�V�������v����

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(name, frameNum, framePerRow, frameWidth, frameHeight, xCellOffset, yCellOffset, xPixelOffset, yPixelOffset, secondsLength);
		}
	};

private:
	std::string filePath;
	DirectX::XMFLOAT2 textureSize;	// �摜�S�̂̃T�C�Y
	float scale = 1.0f;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	std::vector<Animation> animations;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(filePath, textureSize, scale, animations);
	}

public:
	void SetFilePath(std::string f) { filePath = f; }
	std::string& GetFilePath() { return filePath; }

	void SetFloat(float s) { scale = s; }

	// �l�ύX�\�ȃQ�b�^�[(��Ȃ����ǂ܂�)
	ID3D11ShaderResourceView* GetSrv() { return shaderResourceView.Get(); }
	ID3D11ShaderResourceView** GetSrvAddres() { return shaderResourceView.GetAddressOf(); }

	DirectX::XMFLOAT2& GetSize() { return textureSize; }
	float GetScale() { return scale; }
	std::vector<Animation>& GetAnimations() { return animations; }

public:
	// �Ǝ��`���ŏ����o��
	void OutputSpriteData();
	// �Ǝ��`���Ǎ�
	void LoadSpriteData(const char* filePath);
};