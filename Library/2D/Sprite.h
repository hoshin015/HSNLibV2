#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../Graphics/Texture.h"

class Sprite
{
public:
	Sprite(const wchar_t* filename, const char* pixelShaderPath = nullptr);
	~Sprite();

public:
	struct SpriteTransform
	{
	public:
		// 画像の中心点
		enum class ALGIN
		{
			TOP_LEFT,		
			TOP_MIDDLE,		
			TOP_RIGHT,		
			MIDDLE_LEFT,	
			MIDDLE_MIDDLE,	
			MIDDLE_RIGHT,	
			BOTTOM_LEFT,	
			BOTTOM_MIDDLE,	
			BOTTOM_RIGHT,
			MAX,
		};

	private:	// カプセル化
		DirectX::XMFLOAT2	position = {};
		DirectX::XMFLOAT2	size = { 100, 100 };
		DirectX::XMFLOAT4	color = { 1,1,1,1 };
		float				angle = 0;
		DirectX::XMFLOAT2	texPos = {};
		DirectX::XMFLOAT2	texSize = { 100,100 };
		ALGIN				alginType = ALGIN::TOP_LEFT;

	public:
		// セッター
		void SetPos(const DirectX::XMFLOAT2 p) { position = p; }
		void SetPosX(const float x) { position.x = x; }
		void SetPosY(const float y) { position.y = y; }
		void SetSize(DirectX::XMFLOAT2 s) { size = s; }
		void SetSizeX(const float x) { size.x = x; }
		void SetSizeY(const float y) { size.y = y; }
		void SetColor(const DirectX::XMFLOAT4 c) { color = c; }
		void SetColorR(const float r) { color.x = r; }
		void SetColorG(const float g) { color.y = g; }
		void SetColorB(const float b) { color.z = b; }
		void SetColorA(const float a) { color.w = a; }
		void SetAngle(const float a) { angle = a; }
		void SetTexPos(const DirectX::XMFLOAT2 tP) { texPos = tP; }
		void SetTexPosX(const float x) { texPos.x = x; }
		void SetTexPosY(const float y) { texPos.y = y; }
		void SetTexSize(const DirectX::XMFLOAT2 tS) { texSize = tS; }
		void SetTexSizeX(const float x) { texSize.x = x; }
		void SetTexSizeY(const float y) { texSize.y = y; }
		void SetAlgin(const ALGIN a) { alginType = a; }

		// ゲッター
		DirectX::XMFLOAT2	GetPos() { return position; }
		float				GetPosX() { return position.x; }
		float				GetPosY() { return position.y; }
		DirectX::XMFLOAT2	GetSize() { return size; }
		float				GetSizeX() { return size.x; }
		float				GetSizeY() { return size.y; }
		DirectX::XMFLOAT4	GetColor() { return color; }
		float				GetColorR() { return color.x; }
		float				GetColorG() { return color.y; }
		float				GetColorB() { return color.z; }
		float				GetColorA() { return color.w; }
		float				GetAngle() { return angle; }
		DirectX::XMFLOAT2	GetTexPos() { return texPos; }
		float				GetTexPosX() { return texPos.x; }
		float				GetTexPosY() { return texPos.y; }
		DirectX::XMFLOAT2	GetTexSize() { return texSize; }
		float				GetTexSizeX() { return texSize.x; }
		float				GetTexSizeY() { return texSize.y; }
		ALGIN				GetAlginType() { return alginType; }
		int					GetAlginTypeInt() { return static_cast<int>(alginType); }
	};

private:
	SpriteTransform spriteTransform;

	// Animation
	float animationTime = 0.0f;
	float animationFrame = 0.0f;

public:
	SpriteTransform* GetSpriteTransform() { return &spriteTransform; }


public:
	// アニメーション更新
	void PlayAnimation(const float frameSpeed, const float totalAnimationFrame);

	// 描画
	void Render();
	void Render(DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size, DirectX::XMFLOAT4 color, float angle, DirectX::XMFLOAT2 texPos, DirectX::XMFLOAT2 texSize, SpriteTransform::ALGIN algin);


private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_TEXTURE2D_DESC texture2dDesc;


	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};
};