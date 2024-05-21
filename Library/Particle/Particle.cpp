#include "Particle.h"
#include "../ErrorLogger.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"

Particle::Particle()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	//総パーティクルの数をスレッド数で割り切れる数にする
	particleCount = static_cast<size_t>((MAX_PARTICLE / THREAD_NUM_X)) * THREAD_NUM_X;

	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = static_cast<UINT>(sizeof(ParticleData) * particleCount);
	bufferDesc.StructureByteStride = sizeof(ParticleData);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	hr = device->CreateBuffer(&bufferDesc, NULL, particleBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// shaderResourceView 作成
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementOffset = 0;
	srvDesc.Buffer.NumElements = static_cast<UINT>(particleCount);
	hr = device->CreateShaderResourceView(particleBuffer.Get(), &srvDesc, particleBufferSrv.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// unorderedAccessView 作成
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = static_cast<UINT>(particleCount);
	uavDesc.Buffer.Flags = 0;
	hr = device->CreateUnorderedAccessView(particleBuffer.Get(), &uavDesc, particleBufferUav.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// 定数バッファ作成
	bufferDesc.ByteWidth = sizeof(ParticleConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	CreateVsFromCso("ParticleVs.cso", vertexShader.ReleaseAndGetAddressOf(), NULL, NULL, 0);
	CreatePsFromCso("ParticlePs.cso", pixelShader.ReleaseAndGetAddressOf());
	CreateGsFromCso("ParticleGs.cso", geometryShader.ReleaseAndGetAddressOf());
	CreateCsFromCso("ParticleInitCs.cso", initCs.ReleaseAndGetAddressOf());
	CreateCsFromCso("ParticleCs.cso", updateCs.ReleaseAndGetAddressOf());
}

void Particle::Initialize()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	dc->CSSetUnorderedAccessViews(0, 1, particleBufferUav.GetAddressOf(), nullptr);
	dc->Dispatch(particleCount / THREAD_NUM_X, 1, 1);
}

void Particle::Update()
{
}

void Particle::Render()
{
}
