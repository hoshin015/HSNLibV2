#pragma once

#include <vector>
#include "../../Library/Timer.h"

class Score
{
    Score(){}
    ~Score(){}

public:
    static Score& Instance()
    {
        static Score instance;
        return instance;
    }

    void Initialize();
    void Update();

    std::vector<float> GetTotalTime() { return totalTime; }
    int GetPlayNum() { return playNum; }
    //void Add() { totalTime.at(playNum) += Timer::Instance().DeltaTime(); }

private:
    std::vector<float> totalTime;

    //¡‚ÌƒvƒŒƒC‰ñ”
    int playNum = -1;
};