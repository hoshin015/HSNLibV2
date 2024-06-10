#include "Score.h"
#include "../../Library/Timer.h"

void Score::Initialize()
{
    //ƒvƒŒƒC‰ñ”‚ğ‘‚â‚·
    totalTime.emplace_back();
    playNum++;
}

void Score::Update()
{
    totalTime.at(playNum) += Timer::Instance().DeltaTime();
}
