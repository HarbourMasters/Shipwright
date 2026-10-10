#ifndef Z64_H
#define Z64_H
#ifdef __cplusplus
extern "C" {
#endif
#include "libc/math.h"
#include "libc/stdarg.h"
#include "libc/stdbool.h"
#include "libc/stddef.h"
#include "libc/stdint.h"
#include "libc/stdlib.h"

#include "ultra64.h"

#include "color.h"
#include "controller.h"
#include "gfx.h"
#include "gfx_setupdl.h"
#include "gfxprint.h"
#include "ichain.h"
#include "irqmgr.h"
#include "main.h"
#include "message_data_static.h"
#include "padmgr.h"
#include "padutils.h"
#include "regs.h"
#include "scheduler.h"
#include "sequence.h"
#include "seqcmd.h"
#include "sfx.h"

#include "main.h"

#include "rand.h"
#include "sys_matrix.h"
#include "tha.h"
#include "thga.h"

#include "z64actor.h"
#include "z64animation.h"
#include "z64animation_legacy.h"
#include "z64audio.h"
#include "z64bgcheck.h"
#include "z64bombers_notebook.h"
#include "z64camera.h"
#include "z64collision_check.h"
#include "z64curve.h"
#include "z64cutscene.h"
#include "z64dma.h"
#include "z64eff_footmark.h"
#include "z64effect.h"
#include "z64environment.h"
#include "z64frameadvance.h"
#include "z64game_over.h"
#include "z64game.h"
#include "z64interface.h"
#include "z64item.h"
#include "z64keyframe.h"
#include "z64lib.h"
#include "z64light.h"
#include "z64map.h"
#include "z64math.h"
#include "z64message.h"
#include "z64msgevent.h"
#include "z64object.h"
#include "z64ocarina.h"
#include "z64pause_menu.h"
#include "z64play.h"
#include "z64player.h"
#include "z64prerender.h"
#include "z64save.h"
#include "z64scene.h"
#include "z64schedule.h"
#include "z64skin_matrix.h"
#include "z64skin.h"
#include "z64skybox.h"
#include "z64sound_source.h"
#include "z64subs.h"
#include "z64rumble.h"
#include "z64transition.h"
#include "z64view.h"

#define AUDIO_HEAP_SIZE 0x1380000
#define SYSTEM_HEAP_SIZE (1024 * 1024 * 32)

typedef struct {
    /* 0x00 */ TexturePtr timg;
    /* 0x04 */ TexturePtr tlut;
    /* 0x08 */ u16 width;
    /* 0x0A */ u16 height;
    /* 0x0C */ u8 fmt;
    /* 0x0D */ u8 siz;
    /* 0x0E */ u16 tt;
    /* 0x10 */ u16 unk_10;
    /* 0x14 */ f32 x;
    /* 0x18 */ f32 y;
    /* 0x1C */ f32 xScale;
    /* 0x20 */ f32 yScale;
    /* 0x24 */ u32 flags;
} PreRenderParams; // size = 0x28

struct PlayState;

#ifdef __cplusplus
}
#endif

#endif
