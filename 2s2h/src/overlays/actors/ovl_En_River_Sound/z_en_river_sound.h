#ifndef Z_EN_RIVER_SOUND_H
#define Z_EN_RIVER_SOUND_H

#include "global.h"

struct EnRiverSound;

#define RS_GET_TYPE(thisx) ((thisx)->params & 0xFF);
#define RS_GET_PATH_INDEX(thisx) (((thisx)->params >> 8) & 0xFF);

#define RS_PATH_INDEX_NONE 0xFF

// Any param not as one of these values will result in UB
typedef enum {
    /* 0x00 */ RS_RIVER_STREAM,   
    /* 0x01 */ RS_WAVE,
    /* 0x02 */ RS_WATER_WALL_BIG, 
    /* 0x03 */ RS_WATER_WALL,
    /* 0x04 */ RS_MAGMA_LEVEL,    
    /* 0x05 */ RS_MAGMA_LEVEL_M,
    /* 0x06 */ RS_MAGMA_LEVEL_L,  
    /* 0x07 */ RS_TORCH,
    /* 0x08 */ RS_FOUNTAIN,       
    /* 0x09 */ RS_DRAIN,
    /* 0x0A */ RS_CROWD,          
    /* 0x0B */ RS_WATER_CONVECTION,
    /* 0x0C */ RS_GORON_CHEER,    
    /* 0x0D */ RS_WATER_WALL_BIG_SILENT,
    /* 0x0E */ RS_WATER_BUBBLE,   
    /* 0x0F */ RS_COW_CRY_LV,
    /* 0x10 */ RS_MAKE_TURRET,    
    /* 0x11 */ RS_BOILED_WATER_S,
    /* 0x12 */ RS_BOILED_WATER_L, 
    /* 0x13 */ RS_WAVE_0,
    /* 0x14 */ RS_WAVE_1,         
    /* 0x15 */ RS_WAVE_2,
    /* 0xFD */ RS_RIVER_DEFAULT_LOW_FREQ = 0xFD,
    /* 0xFE */ RS_RIVER_DEFAULT_MEDIUM_FREQ,
    /* 0xFF */ RS_RIVER_DEFAULT_HIGH_FREQ
} RiverSoundType;

typedef struct EnRiverSound {
    /* 0x000 */ Actor actor;
    /* 0x144 */ u8 playSound; // Unused remnant of OoT
    /* 0x145 */ u8 soundFreqIndex;
    /* 0x146 */ u8 numPoints;
    /* 0x148 */ Vec3s* pathPoints;
} EnRiverSound; // size = 0x14C

#endif // Z_EN_RIVER_SOUND_H
