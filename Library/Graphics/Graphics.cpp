#include <dxgi.h>
#include <memory>
#include "Graphics.h"
#include "../ErrorLogger.h"
#include "../AdapterReader.h"
#include "Shader.h"
#include "../../External/ImGui/imgui.h"


// ������
void Graphics::Initialize(HWND hwnd, int windowWidth, int windowHeight)
{
	///////// AMD �� NVIDIA �̃O���t�B�b�N�{�[�h������ꍇ�͂�����g�p���� /////////

	// adapter�̎擾�ipc���̂��ׂẴO���t�B�b�N�{�[�h�̎擾�j
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
	if (adapters.size() < 1)	// adapter���擾�ł��Ȃ������ꍇ�̏���
	{
		ErrorLogger::Log("No DXGI Adapters found.");
	}

	AdapterData adapter = AdapterReader::GetAdapters()[0];	// �擪�̃O���t�B�b�N�{�[�h(AMD NVIDIA ���Ȃ��ꍇ�͂�����g�p����)

	for (AdapterData& adp : adapters)
	{
		if (adp.description.VendorId == 0x1002/*AMD*/ || adp.description.VendorId == 0x10DE/*NVIDIA*/)
		{
			adapter = adp;
			break;
		}
	}

	// -------------------- swapchain, device, deviceContext �̍쐬 ---------------------
	
	// swapchain�̏��
	DXGI_SWAP_CHAIN_DESC scd = { 0 };	// �쐬�Ɏg�p����swapchain�̐���
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferDesc.Width = windowWidth;		// �𑜓x�̕�
	scd.BufferDesc.Height = windowHeight;		// �𑜓x�̍���
	scd.BufferDesc.RefreshRate.Numerator = 60;	// ���t���b�V�����[�g�̕��q(�Ⴄ����)
	scd.BufferDesc.RefreshRate.Denominator = 1;	// ���t���b�V�����[�g�̕���(�Ⴄ����)
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hwnd;	// �o�̓E�B���h�E�̃n���h��
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	// Present���Ăяo������Ƀo�b�N�o�b�t�@�[��j������ݒ�
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	// �E�B���h�E���[�h����S��ʃ��[�h�ɐ؂�ւ������ɕ\�����[�h(�܂��͉𑜓x)��ύX����

	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		adapter.pAdapter,		// IDXGI Adapter
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		NULL,
		NULL,
		0,
		D3D11_SDK_VERSION,					// SDK�̃o�[�W�����B���̒l�Œ�
		&scd,								// swapchain�̏��
		this->swapchain.GetAddressOf(),		// IDXGISwapChain�̃A�h���X
		this->device.GetAddressOf(),		// ID3D11Device�̃A�h���X
		NULL,
		this->deviceContext.GetAddressOf()	// ID3D11DeviceContext�̃A�h���X
	);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// ---------------------------- renderTargetView �̍쐬 -----------------------------

	// renderTarget�̃o�b�t�@�[���擾
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	hr = this->device->CreateRenderTargetView(backBuffer.Get(), NULL, this->renderTargetView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// ---------------------------- DepthStencilView �̍쐬 -----------------------------

	// DepthStencilBuffer�̍쐬(�o�b�t�@�Ƃ��Ďg�p���邪�e�N�X�`���̐ݒ���s��)
	ID3D11Texture2D* depthStencilBuffer = {};
	D3D11_TEXTURE2D_DESC texture2dDesc = {};
	texture2dDesc.Width = windowWidth;
	texture2dDesc.Height = windowHeight;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;
	hr = this->device->CreateTexture2D(&texture2dDesc, NULL, &depthStencilBuffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// ---------------------------- depthStencilView �̍쐬 -----------------------------
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = texture2dDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	hr = this->device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, depthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	depthStencilBuffer->Release();

	// ---------------------- depthStencilState �I�u�W�F�N�g�̐��� ----------------------
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	// �[�x�e�X�g:ON	�[�x���C�g:ON
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<size_t>(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// �[�x�e�X�g:ON	�[�x���C�g:OFF
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<size_t>(DEPTHSTENCIL_STATE::ZT_ON_ZW_OFF)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// �[�x�e�X�g:OFF	�[�x���C�g:ON
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<size_t>(DEPTHSTENCIL_STATE::ZT_OFF_ZW_ON)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// �[�x�e�X�g:OFF	�[�x���C�g:OFF
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = device->CreateDepthStencilState(&depthStencilDesc, depthStencilStates[static_cast<size_t>(DEPTHSTENCIL_STATE::ZT_OFF_ZW_OFF)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	// -------------------------------- viewport �̍쐬 ---------------------------------

	// renderTargetView�̂ǂ͈̔͂��g�p���邩�����߂�
	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(windowWidth), static_cast<float>(windowHeight));

	// viewport�̐ݒ�(Rasterizer�̐ݒ�)
	this->deviceContext->RSSetViewports(1, &viewport);


	// ------------------------------- rasterizer �̍쐬 --------------------------------
	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	// ���ʂ̒�`�F�����v����	 �`�悷��O�p�`�F���ʌ���		�h��Ԃ����[�h�F�h��Ԃ�
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<size_t>(RASTERIZER_STATE::CLOCK_FALSE_SOLID)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// ���ʂ̒�`�F�����v����	 �`�悷��O�p�`�F���ʌ���		�h��Ԃ����[�h�F���C���[�t���[��
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<size_t>(RASTERIZER_STATE::CLOCK_FALSE_WIREFRAME)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// ���ʂ̒�`�F�����v����	 �`�悷��O�p�`�F�S��		�h��Ԃ����[�h�F�h��Ԃ�
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<size_t>(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// ���ʂ̒�`�F�����v����	 �`�悷��O�p�`�F�S��		�h��Ԃ����[�h�F���C���[�t���[��
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<size_t>(RASTERIZER_STATE::CLOCK_FALSE_WIREFRAME_CULL_NONE)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// ���ʂ̒�`�F���v����	 �`�悷��O�p�`�F���ʌ���		�h��Ԃ����[�h�F�h��Ԃ�
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<size_t>(RASTERIZER_STATE::CLOCK_TRUE_SOLID)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// ���ʂ̒�`�F���v����	 �`�悷��O�p�`�F���ʌ���		�h��Ԃ����[�h�F���C���[�t���[��
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<size_t>(RASTERIZER_STATE::CLOCK_TRUE_WIREFRAME)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// ���ʂ̒�`�F���v����	 �`�悷��O�p�`�F�S��		�h��Ԃ����[�h�F�h��Ԃ�
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<size_t>(RASTERIZER_STATE::CLOCK_TRUE_CULL_NONE)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// ���ʂ̒�`�F���v����	 �`�悷��O�p�`�F�S��		�h��Ԃ����[�h�F���C���[�t���[��
	rasterizerDesc.FrontCounterClockwise = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizerDesc, rasterizerStates[static_cast<size_t>(RASTERIZER_STATE::CLOCK_TRUE_WIREFRAME_CULL_NONE)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// ------------------------------- Blend State �̍쐬 -------------------------------
	D3D11_BLEND_DESC blendDesc = { 0 };
	// none
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = this->device->CreateBlendState(&blendDesc, blendStates[static_cast<size_t>(BLEND_STATE::NONE)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// alpha
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = this->device->CreateBlendState(&blendDesc, blendStates[static_cast<size_t>(BLEND_STATE::ALPHA)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// add
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, blendStates[static_cast<size_t>(BLEND_STATE::ADD)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// multiply
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_SRC_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, blendStates[static_cast<size_t>(BLEND_STATE::MULTIPLY)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// ------------------------------- SamplerState�̍쐬 -------------------------------
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// �|�C���g�T���v�����O
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	hr = device->CreateSamplerState(&samplerDesc, samplerStates[static_cast<size_t>(SAMPLER_STATE::POINT)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// ���`���
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	hr = device->CreateSamplerState(&samplerDesc, samplerStates[static_cast<size_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// �ٕ������
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	hr = device->CreateSamplerState(&samplerDesc, samplerStates[static_cast<size_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// �e�L�X�g�p
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.MaxAnisotropy = 1;
	hr = device->CreateSamplerState(&samplerDesc, samplerStates[static_cast<size_t>(SAMPLER_STATE::TEXT_LINEAR)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// LINEAR_BORDER_BLACK
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	hr = device->CreateSamplerState(&samplerDesc, samplerStates[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_BLACK)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// LINEAR_BORDER_WHITE
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = FLT_MAX;
	samplerDesc.BorderColor[1] = FLT_MAX;
	samplerDesc.BorderColor[2] = FLT_MAX;
	samplerDesc.BorderColor[3] = FLT_MAX;
	hr = device->CreateSamplerState(&samplerDesc, samplerStates[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_WHITE)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	// SHADOWMAP
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1;
	samplerDesc.BorderColor[1] = 1;
	samplerDesc.BorderColor[2] = 1;
	samplerDesc.BorderColor[3] = 1;
	hr = device->CreateSamplerState(&samplerDesc, samplerStates[static_cast<size_t>(SAMPLER_STATE::SHADOWMAP)].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// ----------------------------- ConstantBuffer �̍쐬 ------------------------------
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(SceneConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	bufferDesc.ByteWidth = sizeof(ColorFilter);
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffers[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	bufferDesc.ByteWidth = sizeof(GaussianConstant);
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffers[3].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	bufferDesc.ByteWidth = sizeof(LuminanceExtractionConstant);
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffers[4].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	
	bufferDesc.ByteWidth = sizeof(ShadowMapData);
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffers[5].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// ----- BloomBuffer �̍쐬
	bloomBuffer = std::make_unique<BloomBuffer>(windowWidth, windowHeight);

	// ----- ShadowBuffer �̍쐬 -----
	shadowBuffer = std::make_unique<ShadowBuffer>();

	// ----- FrameBuffer �̍쐬 -----
	frameBuffers[0] = std::make_unique<FrameBuffer>(windowWidth, windowHeight);		// �ʏ�`��
	frameBuffers[1] = std::make_unique<FrameBuffer>(windowWidth, windowHeight);		// ���P�x���o
	frameBuffers[2] = std::make_unique<FrameBuffer>(windowWidth/2, windowHeight);	// ���u���[�p
	frameBuffers[3] = std::make_unique<FrameBuffer>(windowWidth/2, windowHeight/2);	// �c�u���[�p
	frameBuffers[4] = std::make_unique<FrameBuffer>(windowWidth, windowHeight);		// �J���[�t�B���^�[
	frameBuffers[5] = std::make_unique<FrameBuffer>(windowWidth, windowHeight);		// �ŏI�o��
	
	// ----- FullScreenQuad �̍쐬 -----
	bitBlockTransfer = std::make_unique<FullScreenQuad>();

	// ----- pixelShader �̐��� ---
	CreatePsFromCso("Data/Shader/LuminanceExtraction_PS.cso", pixelShaders[static_cast<size_t>(PS_TYPE::LuminanceExtraction_PS)].GetAddressOf());
	CreatePsFromCso("Data/Shader/Blur_PS.cso", pixelShaders[static_cast<size_t>(PS_TYPE::Blur_PS)].GetAddressOf());
	CreatePsFromCso("Data/Shader/ColorFilter_PS.cso", pixelShaders[static_cast<size_t>(PS_TYPE::ColorFilter_PS)].GetAddressOf());
	CreatePsFromCso("Data/Shader/GaussianBlur_PS.cso", pixelShaders[static_cast<size_t>(PS_TYPE::GaussianBlur_PS)].GetAddressOf());
	CreatePsFromCso("Data/Shader/BloomFinalPass_PS.cso", pixelShaders[static_cast<size_t>(PS_TYPE::BloomFinalPass_PS)].GetAddressOf());
	CreatePsFromCso("Data/Shader/SkinnedMesh_PS.cso", pixelShaders[static_cast<size_t>(PS_TYPE::SkinnedMesh_PS)].GetAddressOf());

	// ----- vertexShader �̐��� ---
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"BONES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
	};


	CreateVsFromCso("Data/Shader/GaussianBlurX_VS.cso", vertexShaders[static_cast<size_t>(VS_TYPE::GaussianBlurX_VS)].ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	CreateVsFromCso("Data/Shader/GaussianBlurY_VS.cso", vertexShaders[static_cast<size_t>(VS_TYPE::GaussianBlurY_VS)].ReleaseAndGetAddressOf(), nullptr, nullptr, 0);
	CreateVsFromCso("Data/Shader/ShadowMapCaster_VS.cso", vertexShaders[static_cast<size_t>(VS_TYPE::ShadowMapCaster_VS)].ReleaseAndGetAddressOf(), inputLayouts[0].ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	CreateVsFromCso("Data/Shader/SkinnedMesh_VS.cso", vertexShaders[static_cast<size_t>(VS_TYPE::SkinnedMesh_VS)].ReleaseAndGetAddressOf(), inputLayouts[1].ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));

	// -----	
	colorFilterConstant.hueShift = 0;
	colorFilterConstant.saturation = 1;
	colorFilterConstant.brightness = 1;

	CalcWeightsTableFromGaussian(2.0f);


	// samplerState�̐ݒ�
	deviceContext->PSSetSamplers(0, 1, samplerStates[static_cast<size_t>(SAMPLER_STATE::POINT)].GetAddressOf());
	deviceContext->PSSetSamplers(1, 1, samplerStates[static_cast<size_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
	deviceContext->PSSetSamplers(2, 1, samplerStates[static_cast<size_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
	deviceContext->PSSetSamplers(3, 1, samplerStates[static_cast<size_t>(SAMPLER_STATE::TEXT_LINEAR)].GetAddressOf());
	deviceContext->PSSetSamplers(4, 1, samplerStates[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_BLACK)].GetAddressOf());
	deviceContext->PSSetSamplers(5, 1, samplerStates[static_cast<size_t>(SAMPLER_STATE::LINEAR_BORDER_WHITE)].GetAddressOf());
}

// �o�b�t�@�؂�ւ�
void Graphics::Present()
{
	swapchain->Present(sync ? 1 : 0, NULL);
}



//--------------------------------------------------------------
//  �estate�ݒ�
//--------------------------------------------------------------

// depthStencil�̐ݒ�
void Graphics::SetDepthStencil(DEPTHSTENCIL_STATE state)
{
	deviceContext->OMSetDepthStencilState(depthStencilStates[static_cast<size_t>(state)].Get(), 1);
}
// rasterizer�̐ݒ�
void Graphics::SetRasterizer(RASTERIZER_STATE state)
{
	deviceContext->RSSetState(rasterizerStates[static_cast<size_t>(state)].Get());
}
// blend�̐ݒ�
void Graphics::SetBlend(BLEND_STATE state)
{
	deviceContext->OMSetBlendState(blendStates[static_cast<size_t>(state)].Get(), NULL, 0xFFFFFFFF);
}

/// <summary>
/// �K�E�V�A���֐��𗘗p���ďd�݃e�[�u�����v�Z����
/// </summary>
/// <param name="blurPower">���U��B���̐��l���傫���Ȃ�ƕ��U��������Ȃ�</param>
void  Graphics::CalcWeightsTableFromGaussian(float blurPower)
{
	float total = 0;
	for (int i = 0; i < NUM_WEIGHTS; i++) {
		gaussianConstant.weights[i] = expf(-0.5f * (float)(i * i) / blurPower);
		total += 2.0f * gaussianConstant.weights[i];

	}
	// �K�i��
	for (int i = 0; i < NUM_WEIGHTS; i++) {
		gaussianConstant.weights[i] /= total;
	}
}



