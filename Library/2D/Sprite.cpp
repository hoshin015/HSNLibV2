#include <sstream>
#include "Sprite.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../ErrorLogger.h"
#include "../RegisterNum.h"
#include "../Timer.h"


// ��]
inline void  Rotate(float& x, float& y, float cx, float cy, float cos, float sin)
{
	x -= cx;
	y -= cy;

	float tx{ x }, ty{ y };
	x = cos * tx + -sin * ty;
	y = sin * tx + cos * ty;

	x += cx;
	y += cy;
};

Sprite::Sprite(const wchar_t* filename, const char* pixelShaderPath)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11Device* device = gfx->GetDevice();

	//--- < ���_���̃Z�b�g > ---
	Vertex vertices[] =
	{
		{{ -0.5f,  0.5f, 0 }, { 1, 1, 1, 1 }, { 0, 0 }},
		{{  0.5f,  0.5f, 0 }, { 1, 0, 0, 1 }, { 1, 0 }},
		{{ -0.5f, -0.5f, 0 }, { 0, 1, 0, 1 }, { 0, 1 }},
		{{  0.5f, -0.5f, 0 }, { 0, 0, 1, 1 }, { 1, 1 }},
	};

	//--- < ���_�o�b�t�@�I�u�W�F�N�g�̐��� > ---
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	HRESULT hr = device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//--- < ���_�V�F�[�_�[�I�u�W�F�N�g�Ɠ��̓��C�A�E�g�I�u�W�F�N�g�̐��� > ---
	const char* csoName{ "./Data/Shader/SpriteVS.cso" };

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CreateVsFromCso(csoName, vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));

	//--- < �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐��� > ---
	if (pixelShaderPath == nullptr) pixelShaderPath = "./Data/Shader/SpritePS.cso";
	csoName = { pixelShaderPath };
	CreatePsFromCso(csoName, pixelShader.GetAddressOf());

	//--- < �摜�t�@�C���̃��[�h��shaderResourceView�̐����ƃe�N�X�`�����̎擾 > ---
	LoadTextureFromFile(filename, shaderResourceView.GetAddressOf(), &texture2dDesc);
}

Sprite::~Sprite()
{
}

//------------------------------------------------------------
//	�A�j���[�V�����X�V�֐�
//------------------------------------------------------------
// frameSpeed			: �A�j���[�V�����̑���
// totalAnimationFrame	: �A�j���[�V�����t���[����
void Sprite::PlayAnimation(const float frameSpeed, const float totalAnimationFrame)
{
	animationTime += frameSpeed * Timer::Instance().DeltaTime();

	const int frame = static_cast<int>(animationTime); // �����_�؂�̂�
	animationFrame = static_cast<float>(frame);

	if (animationFrame > totalAnimationFrame)
	{
		animationFrame = 0.0f;
		animationTime = 0.0f;
	}

	DirectX::XMFLOAT2 texPos = spriteTransform.GetTexPos();
	const DirectX::XMFLOAT2 texSize = spriteTransform.GetTexSize();

	texPos.x = texSize.x * animationFrame;

	spriteTransform.SetTexPos(texPos);
}

void Sprite::Render()
{
	Render(spriteTransform.GetPos(), spriteTransform.GetSize(), spriteTransform.GetColor(), spriteTransform.GetAngle(), spriteTransform.GetTexPos(), spriteTransform.GetTexSize(), spriteTransform.GetAlginType());
}

void Sprite::Render(DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size, DirectX::XMFLOAT4 color, float angle, DirectX::XMFLOAT2 texPos, DirectX::XMFLOAT2 texSize, SpriteTransform::ALGIN algin)
{
	Graphics* gfx = &Graphics::Instance();
	ID3D11DeviceContext* dc = gfx->GetDeviceContext();

	//--- < �X�N���[��(�r���[�|�[�g)�̃T�C�Y���擾 > ---
	D3D11_VIEWPORT viewport{};
	UINT numViewports{ 1 };
	dc->RSGetViewports(&numViewports, &viewport);

	// ���S�_�̕ύX
	DirectX::XMFLOAT2 posBuffer = {};
	switch (algin)
	{
	case Sprite::SpriteTransform::ALGIN::TOP_LEFT:
		break;
	case Sprite::SpriteTransform::ALGIN::TOP_MIDDLE:
		posBuffer.x = -size.x * 0.5f;
		break;
	case Sprite::SpriteTransform::ALGIN::TOP_RIGHT:
		posBuffer.x = -size.x;
		break;
	case Sprite::SpriteTransform::ALGIN::MIDDLE_LEFT:
		posBuffer.y = -size.y * 0.5f;
		break;
	case Sprite::SpriteTransform::ALGIN::MIDDLE_MIDDLE:
		posBuffer.x = -size.x * 0.5f;
		posBuffer.y = -size.y * 0.5f;
		break;
	case Sprite::SpriteTransform::ALGIN::MIDDLE_RIGHT:
		posBuffer.x = -size.x;
		posBuffer.y = -size.y * 0.5f;
		break;
	case Sprite::SpriteTransform::ALGIN::BOTTOM_LEFT:
		posBuffer.y = -size.y;
		break;
	case Sprite::SpriteTransform::ALGIN::BOTTOM_MIDDLE:
		posBuffer.x = -size.x * 0.5f;
		posBuffer.y = -size.y;
		break;
	case Sprite::SpriteTransform::ALGIN::BOTTOM_RIGHT:
		posBuffer.x = -size.x;
		posBuffer.y = -size.y;
		break;
	case Sprite::SpriteTransform::ALGIN::MAX:
		break;
	default:
		break;
	}
	position.x += posBuffer.x;
	position.y += posBuffer.y;

	//--- < ��`�̊e���_�̈ʒu(�X�N���[�����W�n)���v�Z���� > ---

	// left top
	float x0{ position.x };
	float y0{ position.y };
	// right top
	float x1{ position.x + size.x };
	float y1{ position.y };
	// left bottom
	float x2{ position.x };
	float y2{ position.y + size.y };
	// right bottom
	float x3{ position.x + size.x };
	float y3{ position.y + size.y };

	//--- < ���_��]���� > ---

	// ��]�̒��S����`�̒��S�_�ɂ����ꍇ
	float cx = position.x + size.x * 0.5f;
	float cy = position.y + size.y * 0.5f;

	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };

	Rotate(x0, y0, cx, cy, cos, sin);
	Rotate(x1, y1, cx, cy, cos, sin);
	Rotate(x2, y2, cx, cy, cos, sin);
	Rotate(x3, y3, cx, cy, cos, sin);


	//--- < �X�N���[�����W�n����NDC�֍��W�ϊ� > ---
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;


	//--- < �e�N�Z�����W�n����UV���W�n�ւ̕ϊ� > ---
	float u0{ texPos.x / texture2dDesc.Width };
	float v0{ texPos.y / texture2dDesc.Height };
	float u1{ (texPos.x + texSize.x) / texture2dDesc.Width };
	float v1{ (texPos.y + texSize.y) / texture2dDesc.Height };

	//--- < �v�Z���ʂŒ��_�o�b�t�@�I�u�W�F�N�g���X�V���� > ---
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = dc->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	Vertex* vertices{ reinterpret_cast<Vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0 ,y0, 0 };
		vertices[1].position = { x1 ,y1, 0 };
		vertices[2].position = { x2 ,y2, 0 };
		vertices[3].position = { x3 ,y3, 0 };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { color.x, color.y, color.z, color.w };

		vertices[0].texcoord = { u0,v0 };
		vertices[1].texcoord = { u1,v0 };
		vertices[2].texcoord = { u0,v1 };
		vertices[3].texcoord = { u1,v1 };
	}
	dc->Unmap(vertexBuffer.Get(), 0);

	//--- < ���_�o�b�t�@�[�̃o�C���h > ---
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	dc->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	//--- < �v���~�e�B�u�^�C�v����Ƀf�[�^�����Ɋւ�����̃o�C���h > ---
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//--- < ���̓��C�A�E�g�I�u�W�F�N�g�̃o�C���h > ---
	dc->IASetInputLayout(inputLayout.Get());

	//--- < �V�F�[�_�[�̃o�C���h > ---
	dc->VSSetShader(vertexShader.Get(), nullptr, 0);
	dc->PSSetShader(pixelShader.Get(), nullptr, 0);

	//--- < �V�F�[�_�[���\�[�X�̃o�C���h > ---
	dc->PSSetShaderResources(_spriteTexture, 1, shaderResourceView.GetAddressOf());

	//--- < �v���~�e�B�u�̕`�� > ---
	dc->Draw(4, 0);
}
