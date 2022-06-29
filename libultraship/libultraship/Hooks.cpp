#include "Hooks.h"
#include <map>
#include <string>
#include <vector>
#include <stdarg.h>
#include <iostream>

void ModInternal_ExecuteAudioInitHooks() {
    ModInternal::ExecuteHooks<ModInternal::AudioInit>();
}
