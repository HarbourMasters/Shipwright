// Force-included into every game source file on Nintendo Switch builds.
//
// newlib's <sys/types.h> (with BSD extensions visible, as they are under libnx) does
//   #define quad    quad_t
//   #define physadr physadr_t
// which silently renames identifiers such as ColliderQuadDim::quad depending on include order.
// Pull the header in first and drop those aliases; its include guard keeps them from coming back.
#pragma once
#include <sys/types.h>
#undef quad
#undef physadr
