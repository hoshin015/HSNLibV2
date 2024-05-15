#include "TestStatic.h"

void TestStatic::Update()
{
	// p¨s—ñXV
	UpdateTransform();

	for (int i = 0; i < instanceNum; i++)
	{
		DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&transform);
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(-(i%10)*3, 0, -(i/10)*3);
		DirectX::XMStoreFloat4x4(&m[i], W * T);
	}
}

void TestStatic::Render()
{
	model->Render(instanceNum, m);
}
