#include "SetActorCutsceneList.h"

namespace SOH {
CutsceneEntry* SetActorCutsceneList::GetPointer() {
    return entries.data();
}

size_t SetActorCutsceneList::GetPointerSize() {
    return entries.size() * sizeof(CutsceneEntry);
}

} // namespace SOH