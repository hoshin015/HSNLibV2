#include "Shadow.h"
#include "Graphics.h"
#include "Shader.h"
#include "../ErrorLogger.h"
#include "../ImGui/ImGuiManager.h"
#include "../3D/LightManager.h"

Shadow::Shadow(uint32_t width, uint32_t height)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	HRESULT hr = S_OK;

	for (int i = 0; i < SHADOWMAP_COUNT; i++)
	{
		D3D11_TEXTURE2D_DESC texture2dDesc{};
		texture2dDesc.Width = width;
		texture2dDesc.Height = height;
		texture2dDesc.MipLevels = 1;
		texture2dDesc.ArraySize = 1;
		texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture2dDesc.SampleDesc.Count = 1;
		texture2dDesc.SampleDesc.Quality = 0;
		texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
		texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texture2dDesc.CPUAccessFlags = 0;
		texture2dDesc.MiscFlags = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
		hr = device->CreateTexture2D(&texture2dDesc, 0, depthStencilBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = 0;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
			hr = device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilViews[i].GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	

		//-------- ShaderResourceView --------
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
		shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(depthStencilBuffer.Get(), &shaderResourceViewDesc, shaderResourceViews[i].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	}


	// ------- viewport -------
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// ------ vertexShader -------
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"BONES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
	};
	CreateVsFromCso("Data/Shader/ShadowMapCaster_VS.cso", shadowVertexShader.ReleaseAndGetAddressOf(), shadowInputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));

	// ------- 定数バッファ ------
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(ShadowConstants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	hr = device->CreateBuffer(&bufferDesc, nullptr, shadowConstant.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	bufferDesc.ByteWidth = sizeof(ShadowCasterConstants);
	hr = device->CreateBuffer(&bufferDesc, nullptr, shadowCasterConstant.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void Shadow::Clear(float r, float g, float b, float a, float depth)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	float color[4] = { r,g,b,a };
	for (auto& depthStencilView : depthStencilViews)
	{
		dc->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, 0);
	}
}

void Shadow::Activate(int index)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	viewportCount = D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	dc->RSGetViewports(&viewportCount, cachedViewports);
	dc->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
	dc->RSSetViewports(1, &viewport);
	dc->OMSetRenderTargets(0, nullptr, depthStencilViews[index].Get());

	gfx->SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);	// デプスシャドウなので深度テストと深度書きこみをONにしておく
}

void Shadow::DeActivate()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	dc->RSSetViewports(viewportCount, cachedViewports);
	dc->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}

// シェーダー設定
void Shadow::SetShadowShader()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	dc->IASetInputLayout(shadowInputLayout.Get());
	dc->VSSetShader(shadowVertexShader.Get(), nullptr, 0);
	dc->PSSetShader(nullptr, nullptr, 0);
}

// 定数バッファ更新
void Shadow::UpdateConstants()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	// カメラパラメータ設定
	{
		// 平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を生成
		DirectX::XMFLOAT3 dir = LightManager::Instance().GetLight(0)->GetDirection();
		DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&dir);
		LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);
		DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(
			LightPosition,
			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		);

		// シャドウマップに描画したい範囲の射影行列を生成
		//DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(shadowDrawRect, shadowDrawRect, 0.1f, 1000.0f);
		//DirectX::XMMATRIX viewProjection = V * P;
		//DirectX::XMStoreFloat4x4(&shadowConstants.lightViewProjections[0], viewProjection);	// ビュー　プロジェクション　変換行列をまとめる
	}
	shadowConstants.shadowColor = shadowColor;

	dc->UpdateSubresource(shadowConstant.Get(), 0, 0, &shadowConstants, 0, 0);
	dc->VSSetConstantBuffers(_shadowConstant, 1, shadowConstant.GetAddressOf());
	dc->PSSetConstantBuffers(_shadowConstant, 1, shadowConstant.GetAddressOf());
}

// テクスチャの設定
void Shadow::SetShadowTexture()
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	ID3D11ShaderResourceView* srvs[SHADOWMAP_COUNT];
	for (int i = 0; i < SHADOWMAP_COUNT; ++i)
	{
		(&shadowConstants.shadowBias.x)[i] = shadowBias[i];
		srvs[i] = shaderResourceViews[i].Get();
	}
	dc->PSSetShaderResources(_shadowTexture, SHADOWMAP_COUNT, srvs);
}

void Shadow::DrawDebugGui()
{
	ImGui::Begin("Shadow");
	{
		ImGui::ColorEdit3("Color", &shadowConstants.shadowColor.x);
		ImGui::SliderFloat4("Bias", &shadowConstants.shadowBias.x, 0.0f, 1.0f);
		for (auto& shaderResourceView : shaderResourceViews)
		{
			ImGui::Image(shaderResourceView.Get(), { 200, 200 });
		}
		
	}
	ImGui::End();
}
