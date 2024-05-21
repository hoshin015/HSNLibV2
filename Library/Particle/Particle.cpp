#include "Particle.h"
#include "../ErrorLogger.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../Timer.h"

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
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = static_cast<UINT>(particleCount);
	uavDesc.Buffer.Flags = 0;
	hr = device->CreateUnorderedAccessView(particleBuffer.Get(), &uavDesc, particleBufferUav.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// 定数バッファ作成
	bufferDesc.ByteWidth = sizeof(ParticleConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	hr = device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	CreateVsFromCso("Data/Shader/ParticlesVS.cso", vertexShader.ReleaseAndGetAddressOf(), NULL, NULL, 0);
	CreatePsFromCso("Data/Shader/ParticlesPS.cso", pixelShader.ReleaseAndGetAddressOf());
	CreateGsFromCso("Data/Shader/ParticlesGS.cso", geometryShader.ReleaseAndGetAddressOf());
	CreateCsFromCso("Data/Shader/ParticlesInitCS.cso", initCs.ReleaseAndGetAddressOf());
	CreateCsFromCso("Data/Shader/ParticlesUpdateCS.cso", updateCs.ReleaseAndGetAddressOf());

	sprParticle = std::make_unique<Sprite>("Data/Texture/particle1.png");
}

void Particle::Initialize()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	dc->CSSetShader(initCs.Get(), NULL, 0);
	dc->CSSetUnorderedAccessViews(0, 1, particleBufferUav.GetAddressOf(), nullptr);
	dc->Dispatch(particleCount / THREAD_NUM_X, 1, 1);

	//dc->PSSetShaderResources(9, 1, sprParticle->GetSpriteResource()->GetSrvAddres());

	ID3D11UnorderedAccessView* nullUav = {};
	dc->CSSetUnorderedAccessViews(0, 1, &nullUav, nullptr);
}

void Particle::Update()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	particleConstants.deltaTime = Timer::Instance().DeltaTime();
	dc->UpdateSubresource(constantBuffer.Get(), 0, 0, &particleConstants, 0, 0);
	dc->CSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());
	dc->GSSetConstantBuffers(9, 1, constantBuffer.GetAddressOf());
	dc->CSSetShader(updateCs.Get(), NULL, 0);

	dc->CSSetUnorderedAccessViews(0, 1, particleBufferUav.GetAddressOf(), nullptr);
	dc->Dispatch(particleCount / THREAD_NUM_X, 1, 1);

	ID3D11UnorderedAccessView* nullUav = {};
	dc->CSSetUnorderedAccessViews(0, 1, &nullUav, nullptr);
}

void Particle::Render()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	dc->VSSetShader(vertexShader.Get(), NULL, 0);
	dc->PSSetShader(pixelShader.Get(), NULL, 0);
	dc->GSSetShader(geometryShader.Get(), NULL, 0);

	dc->GSSetShaderResources(9, 1, particleBufferSrv.GetAddressOf());

	dc->IASetInputLayout(NULL);
	dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	dc->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	dc->Draw(static_cast<UINT>(particleCount), 0);

	ID3D11ShaderResourceView* nullSrv = {};
	dc->GSSetShaderResources(9, 1, &nullSrv);
	dc->VSSetShader(NULL, NULL, 0);
	dc->PSSetShader(NULL, NULL, 0);
	dc->GSSetShader(NULL, NULL, 0);
}
