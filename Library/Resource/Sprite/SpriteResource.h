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
		int frameNum = 1;			// 合計枚数
		int framePerRow = 1;		// 横の枚数
		int frameWidth = 100;		// １枚の横幅
		int frameHeight = 100;		// １枚の縦幅
		int xCellOffset = 0;	
		int yCellOffset = 0;
		int xPixelOffset = 0;
		int yPixelOffset = 0;

		float secondsLength = 1.0f;	// アニメーション再生時間

		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(name, frameNum, framePerRow, frameWidth, frameHeight, xCellOffset, yCellOffset, xPixelOffset, yPixelOffset, secondsLength);
		}
	};

private:
	std::string filePath;			// 画像の取得元パス
	DirectX::XMFLOAT2 textureSize;	// 画像全体のサイズ
	float scale = 1.0f;				// 描画時のスケール
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	std::vector<Animation> animations;

	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(filePath, textureSize, scale, animations);
	}

public:
	// ファイルパス
	void SetFilePath(std::string f) { filePath = f; }
	std::string& GetFilePath() { return filePath; }

	// スケール
	void SetFloat(float s) { scale = s; }
	DirectX::XMFLOAT2& GetSize() { return textureSize; }

	// srv
	ID3D11ShaderResourceView* GetSrv() { return shaderResourceView.Get(); }
	ID3D11ShaderResourceView** GetSrvAddres() { return shaderResourceView.GetAddressOf(); }

	// サイズ
	float GetScale() { return scale; }

	// animation
	std::vector<Animation>& GetAnimations() { return animations; }

public:
	// 独自形式で書き出し
	void OutputSpriteData();
	// 独自形式読込
	void LoadSpriteData(const char* filePath);
};