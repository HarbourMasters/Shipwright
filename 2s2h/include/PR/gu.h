#ifndef PR_GU_H
#define PR_GU_H

#include <libultraship/libultra/gu.h>

#if 0

#include "ultratypes.h"
#include "gbi.h"


#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif
#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#define M_DTOR		(3.14159265358979323846/180.0)

#define	FTOFIX32(x)	(long)((x) * (float)0x00010000)
#define	FIX32TOF(x)	((float)(x) * (1.0f / (float)0x00010000))
#define	FTOFRAC8(x)	((int) MIN(((x) * (128.0f)), 127.0f) & 0xff)


void guMtxIdent(Mtx* mtx);
void guMtxIdentF(float mf[4][4]);

void guOrtho(Mtx* m, f32 l, f32 r, f32 b, f32 t, f32 n, f32 f, f32 scale);
void guOrthoF(float m[4][4], f32 l, f32 r, f32 b, f32 t, f32 n, f32 f, f32 scale);

void guPerspective(Mtx* m, u16* perspNorm, f32 fovy, f32 aspect, f32 near, f32 far, f32 scale);
void guPerspectiveF(float mf[4][4], u16* perspNorm, f32 fovy, f32 aspect, f32 near, f32 far, f32 scale);

void guLookAt(Mtx* m, f32 xEye, f32 yEye, f32 zEye, f32 xAt, f32 yAt, f32 zAt, f32 xUp, f32 yUp, f32 zUp);
void guLookAtF(float mf[4][4], f32 xEye, f32 yEye, f32 zEye, f32 xAt, f32 yAt, f32 zAt, f32 xUp, f32 yUp, f32 zUp);

void guLookAtHilite(Mtx* m, LookAt* l, Hilite* h, f32 xEye, f32 yEye, f32 zEye, f32 xAt, f32 yAt, f32 zAt, f32 xUp, f32 yUp, f32 zUp, f32 xl1, f32 yl1, f32 zl1, f32 xl2, f32 yl2, f32 zl2, s32 hiliteWidth, s32 hiliteHeight);
void guLookAtHiliteF(float mf[4][4], LookAt* l, Hilite* h, f32 xEye, f32 yEye, f32 zEye, f32 xAt, f32 yAt, f32 zAt, f32 xUp, f32 yUp, f32 zUp, f32 xl1, f32 yl1, f32 zl1, f32 xl2, f32 yl2, f32 zl2, s32 hiliteWidth, s32 hiliteHeight);

void guRotate(Mtx* m, f32 a, f32 x, f32 y, f32 z);
void guRotateF(float m[4][4], f32 a, f32 x, f32 y, f32 z);

void guScale(Mtx* mtx, f32 x, f32 y, f32 z);
void guTranslate(Mtx* mtx, f32 x, f32 y, f32 z);

void guPosition(Mtx* m, f32 rot, f32 pitch, f32 yaw, f32 scale, f32 x, f32 y, f32 z);
void guPositionF(float mf[4][4], f32 rot, f32 pitch, f32 yaw, f32 scale, f32 x, f32 y, f32 z);

void guMtxF2L(float mf[4][4], Mtx* m);
void guMtxL2F(float m1[4][4], Mtx* m2);

void guNormalize(float* x, float* y, float* z);

#endif // 0

// f32 sinf(f32 __x);
// f32 cosf(f32 __x);

s16 sins(u16 x);
s16 coss(u16 x);

// f32 sqrtf(f32 f);
#ifdef __sgi
#pragma intrinsic(sqrtf);
#endif


#endif
