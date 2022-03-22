#pragma once

#include <impl/fs-bridge/fs-bridge.h>
extern FSBridge* NativeFS;
extern bool single_thread;

void setCurrentStep(const std::string& step);