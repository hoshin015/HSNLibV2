#pragma once
#include "StaticObject.h"

class TestStatic : public StaticObject
{
public:
	TestStatic(const char* filePath) : StaticObject(filePath) {}
	~TestStatic() override {}

	void Update() override;
	void Render() override;


private:
	static const int instanceNum = 300;
	DirectX::XMFLOAT4X4 m[instanceNum] = {};
};