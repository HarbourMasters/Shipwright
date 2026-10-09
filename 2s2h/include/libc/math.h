#ifndef LIBC_MATH_H
#define LIBC_MATH_H

#include <math.h>


#define M_PIf 3.14159265358979323846f
#define M_SQRT2f 1.41421356237309504880f
#define M_SQRT1_2f 0.70710678118654752440f /* 1/sqrt(2) */

#ifndef FLT_MAX
#define FLT_MAX 340282346638528859811704183484516925440.0f
#endif

#ifndef SHT_MAX
#define SHT_MAX 32767.0f
#endif

#ifndef SHT_MINV
#define SHT_MINV (1.0f / SHT_MAX)
#endif

//double sqrt(double d);
//#pragma intrinsic(sqrt)

#endif
