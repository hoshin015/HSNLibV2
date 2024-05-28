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

    //プレイヤー間の中心座標を取得
    DirectX::XMFLOAT3 GetPositionCenter();
    //プレイヤー型Vectorのゲッター
    std::vector<Player*> GetPlayer() { return players; }

    //球と光線の交差判定(今は当たっているかの判定だけ)
    //ライブラリの方を変えたくないのでとりあえずここに書く
    static bool IntersectSphereVsLine(DirectX::XMFLOAT3 spherePosition, float radius,DirectX::XMFLOAT3 start,DirectX::XMFLOAT3 end);

private:
    //ロープの長さが最大値を超えた時の処理
    void OverMaxRopeLength();

    //プレイヤーVSプレイヤーの当たり判定
    void CollisionPlayerVsPlayer();

    //紐とモデル(障害物)との当たり判定
    void CollisionRopeVsModel();

    //プレイヤーとモデルとの当たり判定
    void CollisionPlayerVsModel();

private:
    //定数値
    const int MAXPLAYERNUM = 2;

private:
    std::vector<Player*> players;
    float maxRopeLength = 10.0f;
    float ropeLength = 0.0f; 
    DirectX::XMFLOAT4 ropeColor = { 1,0,0,1 };

    bool overRopeLength = false;


};