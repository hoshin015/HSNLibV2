#pragma once

#include "Player.h"

class PlayerManager
{
private:
    PlayerManager(){}
    ~PlayerManager(){}


public:
    static PlayerManager& Instance() 
    {
        static PlayerManager instance;
        return instance;
    }

    void Register(Player* player);
    void Update();
    void Render();
    void DrawDebugImGui();
    void Clear();

    DirectX::XMFLOAT3 GetPositionCenter();

private:
    //ロープの長さが最大値を超えた時の処理
    void OverMaxRopeLength();

private:
    //定数値
    const int MAXPLAYERNUM = 2;

private:
    std::vector<Player*> players;
    float maxRopeLength = 10.0f;
    float ropeLength = 0.0f;

    bool overRopeLength = false;
    //float totalFactor = 0.0f;
};