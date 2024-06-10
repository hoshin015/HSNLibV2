#include "Score.h"
#include "../../Library/Timer.h"

void Score::Initialize()
{
    //プレイ回数を増やす
    totalTime.emplace_back();
    playNum++;
}

void Score::Update()
{
    totalTime.at(playNum) += Timer::Instance().DeltaTime();
}
