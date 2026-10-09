#include "global.h"

static ColliderJntSphElementInit sWrathJntSphElementsInit1[11] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 20 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 30 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 25 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 150 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 150 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 150 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 150 },
    },
};

static ColliderJntSphInit sWrathJntSphInit1 = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sWrathJntSphElementsInit1),
    sWrathJntSphElementsInit1,
};

static ColliderCylinderInit sWrathCylInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0xF7CFFFFF, 0x00, 0x10 },
        { 0xF7CEFFFE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 80, 200, 0, { 0, 0, 0 } },
};

static ColliderJntSphElementInit sWrathJntSphElementsInit2[1] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x04, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_HARD,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 36 }, 200 },
    },
};

static ColliderJntSphInit sWrathJntSphInit2 = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sWrathJntSphElementsInit2),
    sWrathJntSphElementsInit2,
};

static ColliderQuadInit sMaskQuadInit1 = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0xF7CFFFFF, 0x04, 0x00 },
        { 0xF7CEFFFE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderQuadInit sMaskQuadInit2 = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_QUAD,
    },
    {
        ELEM_MATERIAL_UNK2,
        { 0xF7CFFFFF, 0x04, 0x10 },
        { 0xF7CEFFFE, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } } },
};

static ColliderJntSphElementInit sIncarnationJntSphElementsInit[11] = {
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 0, { { 0, 0, 0 }, 25 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 40 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 0 }, 0 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 100 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 150 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 150 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 150 },
    },
    {
        {
            ELEM_MATERIAL_UNK3,
            { 0xF7CFFFFF, 0x00, 0x10 },
            { 0xF7CEFFFE, 0x00, 0x00 },
            ATELEM_ON | ATELEM_SFX_NORMAL,
            ACELEM_ON,
            OCELEM_ON,
        },
        { 1, { { 0, 0, 0 }, 15 }, 150 },
    },
};

static ColliderJntSphInit sIncarnationJntSphInit1 = {
    {
        COL_MATERIAL_HIT3,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_PLAYER,
        OC2_TYPE_1,
        COLSHAPE_JNTSPH,
    },
    ARRAY_COUNT(sIncarnationJntSphElementsInit),
    sIncarnationJntSphElementsInit,
};

static ColliderCylinderInit sShotCylInit = {
    {
        COL_MATERIAL_NONE,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x04, 0x10 },
        { 0x00300000, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON,
        OCELEM_ON,
    },
    { 15, 30, -15, { 0, 0, 0 } },
};

static ColliderCylinderInit sRemainsCylInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x04, 0x10 },
        { 0xF7CFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 50, 100, -50, { 0, 0, 0 } },
};

static ColliderCylinderInit sTopCylInit = {
    {
        COL_MATERIAL_METAL,
        AT_ON | AT_TYPE_ENEMY,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_1,
        COLSHAPE_CYLINDER,
    },
    {
        ELEM_MATERIAL_UNK0,
        { 0xF7CFFFFF, 0x04, 0x10 },
        { 0xF7FFFFFF, 0x00, 0x00 },
        ATELEM_ON | ATELEM_SFX_NORMAL,
        ACELEM_ON | ACELEM_HOOKABLE,
        OCELEM_ON,
    },
    { 40, 20, 15, { 0, 0, 0 } },
};
