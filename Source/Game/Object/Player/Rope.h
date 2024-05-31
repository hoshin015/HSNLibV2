#pragma once

#include "../../Source/Game/Object/Base/AnimatedObject.h"

class Rope : public AnimatedObject
{
public:
    Rope(const char* filePath);
    ~Rope() override {}

    void Update() override;
    void Render(bool isShadow = false) override;
    void DrawDebugImGui();

    float GetRopeLength() { return ropeLength; }
    void SetRopeLength(float length) { ropeLength = length; }
    float GetMaxRopeLength() { return maxRopeLength; }
    bool IsOverRopeLength() { return overRopeLength; }
    void SetOverRopeLengthFalse() { overRopeLength = false; }
private:
    float ropeLength = 0.0f;
    bool overRopeLength = false;
    float maxRopeLength = 75.0f;
    //float ropeLength = 0.0f;
    DirectX::XMFLOAT4 ropeColor = { 1,0,0,1 };
};