#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <mutex>
#include "FrameBuffer.h"
#include "BloomBuffer.h"
#include "ShadowBuffer.h"
#include "FullScreenQuad.h"

enum class SAMPLER_STATE
{
	POINT,
	LINEAR,
	ANISOTROPIC,
	TEXT_LINEAR,
	LINEAR_BORDER_BLACK,
	LINEAR_BORDER_WHITE,
	SHADOWMAP,

	NUM,
};

enum class RASTERIZER_STATE
{
	CLOCK_FALSE_SOLID,
	CLOCK_FALSE_WIREFRAME,
	CLOCK_FALSE_CULL_NONE,
	CLOCK_FALSE_WIREFRAME_CULL_NONE,
	CLOCK_TRUE_SOLID,
	CLOCK_TRUE_WIREFRAME,
	CLOCK_TRUE_CULL_NONE,
	CLOCK_TRUE_WIREFRAME_CULL_NONE,

	NUM,
};

enum class BLEND_STATE
{
	NONE,
	ALPHA,
	ADD,
	MULTIPLY,

	NUM,
};

enum class DEPTHSTENCIL_STATE
{
	ZT_ON_ZW_ON,
	ZT_ON_ZW_OFF,
	ZT_OFF_ZW_ON,
	ZT_OFF_ZW_OFF,

	NUM,
};

enum class OBJECT_TYPE
{
	OFF_LUMINANCE,
	ON_LUMINANCE,
};

enum class PS_TYPE
{
	LuminanceExtraction_PS,
	Blur_PS,
	ColorFilter_PS,
	GaussianBlur_PS,
	BloomFinalPass_PS,

	SkinnedMesh_PS,

	NUM
};

enum class VS_TYPE
{
	GaussianBlurX_VS,
	GaussianBlurY_VS,
	ShadowMapCaster_VS,

	SkinnedMesh_VS,

	NUM
};



//--------------------------------------------------------------
//  Graphics
//--------------------------------------------------------------
class Graphics
{
private:
	Graphics() {}
	~Graphics() {}

public:
	static Graphics& Instance()
	{
		static Graphics instance;
		return instance;
	}

	// �~���[�e�b�N�X�擾
	std::mutex& GetMutex() { return mutex; }

	// ������
	void Initialize(HWND hwnd, int windowWidth, int windowHeight);

	// �o�b�t�@�؂�ւ�
	void Present();

	// depthStencil�̐ݒ�
	void SetDepthStencil(DEPTHSTENCIL_STATE state);
	// rasterizer�̐ݒ�
	void SetRasterizer(RASTERIZER_STATE state);
	// blend�̐ݒ�
	void SetBlend(BLEND_STATE state);

public:
	// �K�E�X�t�B���^�[�v�Z���
	//struct GaussianFilterData
	//{
	//	int kernelSize = 8;				// �J�[�l���T�C�Y
	//	float deviation = 10.0f;		// �W���΍�
	//	DirectX::XMFLOAT2 textureSize;	// �򂷃e�N�X�`���̃T�C�Y
	//};
	//static const int MaxKernelSize = 16;

	// �J���[�t�B���^�[���
	struct ColorFilter
	{
		float hueShift;
		float saturation;
		float brightness;
		float pad;
	};
	ColorFilter colorFilterConstant;

	static const int NUM_WEIGHTS = 8;
	struct GaussianConstant
	{
		float weights[NUM_WEIGHTS];
	};
	GaussianConstant gaussianConstant;

	struct LuminanceExtractionConstant
	{
		float threshould;
		float intensity = 2.0f;
		DirectX::XMFLOAT2 lumiPad;
	};
	LuminanceExtractionConstant luminanceExtractionConstant;

	struct ShadowMapData
	{
		DirectX::XMFLOAT4X4 lightViewProjection;				// ���C�g�r���[�v���W�F�N�V�����s��
		DirectX::XMFLOAT3 shadowColor = { 0.2f,0.2f,0.2f };		// �e�̐F
		float shadowBias = 0.0001f;								// �[�x��r�p�̃I�t�Z�b�g�l
	};
	ID3D11ShaderResourceView* shadowMap;						// �V���h�E�}�b�v�e�N�X�`��
	float shadowDrawRect = 200.0f;
	ShadowMapData shadowMapData;

	// �萔�o�b�t�@�p�\����
	struct SceneConstants
	{
		DirectX::XMFLOAT4X4 viewProjection;
		//DirectionalLightData directionalLightData;
		//PointLightData pointLightData[PointLightMax];
		//SpotLightData spotLightData[SpotLightMax];
		//int pointLightCount = 0;
		//int spotLightCount = 0;
		//DirectX::XMFLOAT2 pad;
		//DirectX::XMFLOAT4 ambientLightColor;
		DirectX::XMFLOAT4 cameraPosition;
		//GaussianFilterData gaussianFilterData;
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[static_cast<size_t>(DEPTHSTENCIL_STATE::NUM)];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[static_cast<size_t>(RASTERIZER_STATE::NUM)];
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[static_cast<size_t>(BLEND_STATE::NUM)];
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStates[static_cast<size_t>(SAMPLER_STATE::NUM)];

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffers[8];
	std::unique_ptr<FrameBuffer> frameBuffers[8];
	std::unique_ptr<BloomBuffer> bloomBuffer;
	std::unique_ptr<ShadowBuffer> shadowBuffer;
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaders[8];
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShaders[8];
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayouts[8];
public:
	ID3D11Device* GetDevice() { return device.Get(); }
	ID3D11Device** GetDeviceAddress() { return device.GetAddressOf(); }
	ID3D11DeviceContext* GetDeviceContext() { return deviceContext.Get(); }
	ID3D11RenderTargetView* GetRTV() { return renderTargetView.Get(); }
	ID3D11RenderTargetView** GetRTVAddress() { return renderTargetView.GetAddressOf(); }
	ID3D11DepthStencilView* GetDSV() { return depthStencilView.Get(); }

	// �K�E�V�A���֐��𗘗p���ďd�݃e�[�u�����v�Z����
	void CalcWeightsTableFromGaussian(float blurPower);

	float* GetBgColor() { return bgcolor; }
private:
	std::mutex mutex;

	bool sync = false;								// fps �Œ�t���O
	float bgcolor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };	// �w�i�F

};