#include "GameInteractor_Hooks.h"
#include "EventSystem_List.h"

bool GameInteractor_Should(GIVanillaBehavior flag, u32 result, ...) {
    // Only the external function can use the Variadic Function syntax
    // To pass the va args to the next caller must be done using va_list and reading the args into it
    // Because there can be N subscribers registered to each template call, the subscribers will be responsible for
    // creating a copy of this va_list to avoid incrementing the original pointer between calls
    va_list args;
    va_start(args, result);

    // Because of default argument promotion, even though our incoming "result" is just a bool, it needs to be typed as
    // an int to be permitted to be used in `va_start`, otherwise it is undefined behavior.
    // Here we downcast back to a bool for our actual hook handlers
    bool boolResult = static_cast<bool>(result);

    CALL_EVENT(OnVanillaBehavior, flag, &boolResult, args);

    va_end(args);
    return boolResult;
}