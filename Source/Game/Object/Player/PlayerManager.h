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

    DirectX::XMFLOAT3 GetPositionAverage();

public:

private:
    const int MAXPLAYERNUM = 2;
    std::vector<Player*> players;
    float maxRopeLength = 7.0f;
    float ropeLength = 0.0f;

};