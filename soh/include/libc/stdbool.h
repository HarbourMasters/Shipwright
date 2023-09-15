#ifndef STDBOOL_H
#define STDBOOL_H

#define __bool_true_false_are_defined 1

#ifndef __cplusplus

// clang-format off
// no idea why AlignConsecutiveMacros seems to mess this one up
#define bool    u32
#define false   0
#define true    1
// clang-format on

#endif /* __cplusplus */

#endif /* STDBOOL */
