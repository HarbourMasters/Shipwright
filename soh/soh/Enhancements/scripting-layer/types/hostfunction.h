#pragma once

#include "soh/Enhancements/scripting-layer/hosts/hostapi.h"

class HostFunction {
    HostAPI* mHost;
    uintptr_t mContext;
    uintptr_t mFunction;
public:
    explicit HostFunction(HostAPI* host, uintptr_t context, uintptr_t function) : mHost(host), mContext(context), mFunction(function) {}
    void execute();
    void execute(std::any result...);
};