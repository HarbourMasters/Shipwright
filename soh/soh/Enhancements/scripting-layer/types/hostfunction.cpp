#include "hostfunction.h"

void HostFunction::execute() {
    mHost->Call(mContext, mFunction, {});
}

void HostFunction::execute(std::any result...) {
    mHost->Call(mContext, mFunction, { std::move(result) });
}