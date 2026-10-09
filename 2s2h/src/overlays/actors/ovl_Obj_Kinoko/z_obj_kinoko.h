#ifndef Z_OBJ_KINOKO_H
#define Z_OBJ_KINOKO_H

#include "global.h"

struct ObjKinoko;

typedef struct ObjKinoko {
    /* 0x000 */ Actor actor;
} ObjKinoko; // size = 0x144

#define OBJ_KINOKO_GET_FLAG(thisx) ((thisx)->params & 0x7F) 

#endif // Z_OBJ_KINOKO_H
