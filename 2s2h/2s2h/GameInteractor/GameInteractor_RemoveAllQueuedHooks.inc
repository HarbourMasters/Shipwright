// Body of GameInteractor::RemoveAllQueuedHooks, which is defined in the header on MSVC and in the
// .cpp everywhere else. See the declaration in GameInteractor.h for why.

#define DEFINE_HOOK(name, _) ProcessUnregisteredHooks<name>();

#include "GameInteractor_HookTable.h"

#undef DEFINE_HOOK
