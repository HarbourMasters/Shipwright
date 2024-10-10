#pragma once

#include "performanceTimer.h"

void StartPerformanceTimer(TimerID timer){
    timeStarted[timer] = std::chrono::high_resolution_clock::now();
}

void StopPerformanceTimer(TimerID timer){
    totalTimes[timer] += (std::chrono::high_resolution_clock::now() - timeStarted[timer]);
}

std::chrono::duration<double, std::milli> GetPerformanceTimer(TimerID timer){
    return totalTimes[timer];
}

void ResetPerformanceTimers(){
    totalTimes = {};
}