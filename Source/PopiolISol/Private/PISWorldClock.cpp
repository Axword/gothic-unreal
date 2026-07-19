#include "PISWorldClock.h"
APISWorldClock::APISWorldClock(){PrimaryActorTick.bCanEverTick=true;} void APISWorldClock::Tick(float D){Hour+=D*GameMinutesPerRealSecond/60.f;while(Hour>=24){Hour-=24;++Day;}} void APISWorldClock::SleepTo(float T){Hour=FMath::Clamp(T,0.f,23.99f);}
