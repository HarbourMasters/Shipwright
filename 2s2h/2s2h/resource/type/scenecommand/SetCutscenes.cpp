#include "SetCutscenes.h"

namespace SOH {
CutsceneScriptEntry* SetCutscenesMM::GetPointer() {
    if (entries.size() == 0) {
        return nullptr;
    }
    return entries.data();
}

size_t SetCutscenesMM::GetPointerSize() {
    if (entries.size() == 0) {
        return 0;
    }
    return entries.size() * sizeof(CutsceneScriptEntry);
}

} // namespace SOH
