#ifndef Z_EN_NIW_H
#define Z_EN_NIW_H

#include "ultra64.h"
#include "global.h"

struct EnNiw;

typedef void (*EnNiwActionFunc)(struct EnNiw*, PlayState*);

typedef struct {
    /* 0x0000 */ u8 type;
    /* 0x0004 */ Vec3f pos;
    /* 0x0010 */ Vec3f vel;
    /* 0x001C */ Vec3f accel;
    /* 0x0028 */ s16 life;
    /* 0x002A */ s16 unk_2A; // Variance timer?
    /* 0x002C */ f32 scale;
    /* 0x0030 */ f32 unk_30;
    /* 0x0034 */ u8 timer;
    u32 epoch;
} EnNiwFeather; // size = 0x0038

typedef struct EnNiw {
    /* 0x0000 */ Actor actor;
    /* 0x014C */ SkelAnime skelAnime;
    /* 0x0190 */ Vec3s jointTable[16];
    /* 0x01F0 */ Vec3s morphTable[16];
    /* 0x0250 */ EnNiwActionFunc actionFunc;
    /* 0x0254 */ s16 timer1;
    /* 0x0256 */ s16 timer2;
    /* 0x0258 */ s16 timer3;
    /* 0x025A */ s16 timer4;
    /* 0x025C */ s16 timer5;
    /* 0x025E */ s16 timer6;
    /* 0x0260 */ s16 sfxTimer1;
    /* 0x0262 */ s16 sfxTimer2;
    /* 0x0264 */ s16 sfxTimer3;
    /* 0x0266 */ s16 timer7;
    /* 0x0268 */ s16 timer8;
    /* 0x026A */ s16 timer9;
    /* 0x026C */ f32 unk_26C[10];
    /* 0x0294 */ s16 unk_294;
    /* 0x0296 */ s16 unk_296;
    /* 0x0298 */ s16 unk_298;
    /* 0x029A */ s16 unk_29A;
    /* 0x029C */ s16 unk_29C;
    /* 0x029E */ s16 unk_29E;
    /* 0x02A0 */ s16 unk_2A0;
    /* 0x02A2 */ s16 unk_2A2;
    /* 0x02A4 */ s16 unk_2A4;
    /* 0x02A6 */ s16 unk_2A6;
    /* 0x02A8 */ s16 unk_2A8;
    /* 0x02AA */ s16 unk_2AA;
    /* 0x02AC */ Vec3f unk_2AC;
    /* 0x02B8 */ Vec3f unk_2B8;
    /* 0x02C4 */ f32 unk_2C4;
    /* 0x02C8 */ f32 unk_2C8;
    /* 0x02CC */ f32 unk_2CC;
    /* 0x02D0 */ f32 unk_2D0;
    /* 0x02D4 */ f32 unk_2D4;
    /* 0x02D8 */ f32 unk_2D8;
    /* 0x02DC */ f32 unk_2DC;
    /* 0x02E0 */ f32 unk_2E0;
    /* 0x02E4 */ s16 unk_2E4;
    /* 0x02E6 */ s16 unk_2E6;
    /* 0x02E8 */ s16 path;
    /* 0x02EA */ s16 waypoint;
    /* 0x02EC */ s16 unk_2EC;
    /* 0x02EE */ s16 unk_2EE;
    /* 0x02F0 */ Vec3f unk_2F0;
    /* 0x02FC */ f32 unk_2FC;
    /* 0x0300 */ f32 unk_300;
    /* 0x0304 */ f32 unk_304;
    /* 0x0308 */ u8 unk_308;
    /* 0x030C */ ColliderCylinder collider;
    /* 0x0358 */ EnNiwFeather feathers[20];
} EnNiw; // size = 0x07B8

#ifdef __cplusplus
#define this thisx
extern "C"
{
#endif
    void func_80AB70A0(EnNiw* this, PlayState* play);
    void func_80AB70A0_nocutscene(EnNiw* this, PlayState* play);
#ifdef __cplusplus
#undef this
};
#undef this
#endif

#endif
