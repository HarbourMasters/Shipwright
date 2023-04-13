#include "context.h"

namespace Rando {
std::weak_ptr<Context> Context::mContext;

std::shared_ptr<Context> Context::CreateInstance() {
    if (mContext.expired()) {
        auto instance = std::make_shared<Context>();
        mContext = instance;
        return instance;
    }
    return GetInstance();
}

std::shared_ptr<Context> Context::GetInstance() {
    return mContext.lock();
}

void Context::AddHint(RandomizerHintKey hintId, Text text) {
    hintTable[hintId] = RandoHint(text);
}
}
