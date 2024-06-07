#include "Score.h"
#include "../../Library/Timer.h"

void Score::Initialize()
{
    //�v���C�񐔂𑝂₷
    totalTime.emplace_back();
    playNum++;
}

void Score::Update()
{
    totalTime.at(playNum) += Timer::Instance().DeltaTime();
}
