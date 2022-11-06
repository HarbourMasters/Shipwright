#include "ultra64.h"
#include "global.h"

static PlayState* sPlayState;

// TODO: cleanup these arrays and UB access
char* D_8012CEE0[] = { GFXP_KATAKANA "ｷ-ﾌﾚ-ﾑ" GFXP_HIRAGANA "ｶﾞ" };
char* D_8012CEE4 = GFXP_HIRAGANA "ﾀﾘﾏｾﾝ｡";
char* D_8012CEE8 = GFXP_HIRAGANA "ｻｲｾｲﾃﾞｷﾏｾﾝ";
char* D_8012CEEC = GFXP_HIRAGANA "ｻｲｾｲｼｭｳﾘｮｳ";
char* D_8012CEF0 = GFXP_HIRAGANA "ｻｲｾｲﾁｭｳ!";

char* D_8012CEF4 = "DEMO CAMERA TOOL";
char* D_8012CEF8[] = { GFXP_HIRAGANA "ﾓｳﾊｲﾘﾏｾﾝ", GFXP_HIRAGANA "ﾄｳﾛｸ   ﾃﾝﾒ", GFXP_HIRAGANA "ﾍﾝｺｳ   /  " };
char* D_8012CF04 = ">            >";
char* D_8012CF08 = "<            <";
char* D_8012CF0C = "<          >";
char* D_8012CF10 = GFXP_KATAKANA "*ﾌﾟﾚｲﾔ-*";
char* D_8012CF14 = GFXP_KATAKANA "Eﾓ-ﾄﾞ" GFXP_HIRAGANA " ｿｳﾀｲ";
char* D_8012CF18[] = { GFXP_KATAKANA "Eﾓ-ﾄﾞ" GFXP_HIRAGANA "ｾﾞｯﾀｲ", GFXP_HIRAGANA "ｶﾞﾒﾝ" GFXP_KATAKANA "   ﾃﾞﾓ",
                       GFXP_HIRAGANA "ｶﾞﾒﾝ   ﾌﾂｳ" };
char* D_8012CF24[] = { GFXP_HIRAGANA "Pｼﾞｶﾝ  MAX", GFXP_KATAKANA "ﾘﾝｸ" GFXP_HIRAGANA "    ｷｵｸ",
                       GFXP_KATAKANA "ﾘﾝｸ" GFXP_HIRAGANA "     ﾑｼ" };
char* D_8012CF30 = GFXP_HIRAGANA "*ﾐﾃﾙｲﾁ*";
char* D_8012CF34 = GFXP_KATAKANA "*ｶﾒﾗ" GFXP_HIRAGANA "ｲﾁ*";
char* D_8012CF38 = "DEBUG CAMERA";
char* D_8012CF3C = GFXP_KATAKANA "ｾﾝﾀ-/ﾛｯｸ";
char* D_8012CF40 = GFXP_KATAKANA "ｾﾝﾀ-/ﾌﾘ-";

char* D_8012CF44 = "DEMO CONTROL";
char* D_8012CF48 = GFXP_KATAKANA "ﾒﾓﾘ" GFXP_HIRAGANA "ｶﾞﾀﾘﾏｾﾝ";
char* D_8012CF4C = "p";
char* D_8012CF50[] = { "e", "s", "l", "c" };

char* D_8012CF60[] = { GFXP_KATAKANA "ﾒﾓﾘﾊﾟｯｸ" }; // "Mempak"
char* D_8012CF64 = GFXP_KATAKANA "ｾｰﾌﾞ";          // "Save"
char* D_8012CF68 = GFXP_KATAKANA "ﾛｰﾄﾞ";          // "Load"
char* D_8012CF6C = GFXP_KATAKANA "ｸﾘｱ-";          // "Clear"
char* D_8012CF70 = GFXP_HIRAGANA "ｦﾇｶﾅｲﾃﾞﾈ";

char* D_8012CF74 = "FREE      BYTE";
char* D_8012CF78 = "NEED      BYTE";
char* D_8012CF7C = GFXP_KATAKANA "*ﾒﾓﾘ-ﾊﾟｯｸ*";
char* D_8012CF80 = GFXP_HIRAGANA "ｦﾐﾂｹﾗﾚﾏｾﾝ";
char* D_8012CF84 = GFXP_KATAKANA "ﾌｧｲﾙ " GFXP_HIRAGANA "ｦ";
char* D_8012CF88[] = { GFXP_HIRAGANA "ｼﾃﾓｲｲﾃﾞｽｶ?", GFXP_HIRAGANA "ｹﾞﾝｻﾞｲﾍﾝｼｭｳﾁｭｳﾉ",
                       GFXP_KATAKANA "ﾌｧｲﾙ" GFXP_HIRAGANA "ﾊﾊｷｻﾚﾏｽ" };
char* D_8012CF94 = GFXP_HIRAGANA "ﾊｲ";
char* D_8012CF98 = GFXP_HIRAGANA "ｲｲｴ";
char* D_8012CF9C[] = { GFXP_HIRAGANA "ｼﾃｲﾏｽ", GFXP_HIRAGANA "ｳﾜｶﾞｷ" };
char* D_8012CFA4 = GFXP_HIRAGANA "ｼﾏｼﾀ";
char* D_8012CFA8 = "USE       BYTE";
char* D_8012CFAC = GFXP_HIRAGANA "ﾆｼｯﾊﾟｲ";

char* D_8012CFB0 = GFXP_KATAKANA "Eﾓ-ﾄﾞ" GFXP_HIRAGANA "  ｺﾃｲ";
char D_8012CFB4[] = GFXP_KATAKANA "ﾌﾚ-ﾑ       ";
char D_8012CFC4[] = GFXP_KATAKANA "ｷ-   /   ";
char D_8012CFD0[] = GFXP_HIRAGANA "(ｾﾝﾀ-ﾃﾝ)\0\0\0\0\0";
char D_8012CFE4[] = GFXP_HIRAGANA "(ｹﾞﾝﾃﾝ)\0\0\0\0\0\0";
char D_8012CFF8[] = GFXP_KATAKANA "(ﾌﾟﾚｲﾔ-)\0\0\0\0\0";
char D_8012D00C[] = GFXP_HIRAGANA "(ｲﾁｱﾜｾ)\0\0\0\0\0\0";
char D_8012D020[] = GFXP_HIRAGANA "(ｾｯﾃｲ)\0\0\0\0\0\0\0\0";
char D_8012D034[] = GFXP_HIRAGANA "(ｷｬｯｶﾝ)\0\0\0\0\0\0";
char D_8012D048[] = GFXP_KATAKANA "ﾎﾟｲﾝﾄNo.  \0\0\0";
char D_8012D05C[] = GFXP_HIRAGANA "ｶﾞｶｸ    \0\0\0";
char D_8012D070[] = GFXP_KATAKANA "Nﾌﾚ-ﾑ   \0\0\0\0";
char D_8012D084[] = GFXP_HIRAGANA "Zｶｲﾃﾝ   \0\0\0";
char D_8012D098[] = GFXP_KATAKANA "ﾓ-ﾄﾞ    \0\0\0\0";
char D_8012D0AC[] = "  R" GFXP_HIRAGANA "ﾁｭｳｼﾝ  \0\0\0";

char D_8012D0C0[] = GFXP_HIRAGANA "Pｼﾞｶﾝ       \0\0";
char D_8012D0D4[] = GFXP_HIRAGANA "ｷｮﾘ       \0";
char D_8012D0E4[] = GFXP_HIRAGANA "Xｶｲﾃﾝ       \0\0";
char D_8012D0F8[] = GFXP_HIRAGANA "Yｶｲﾃﾝ       \0\0";

#define ACTION_E 0
#define ACTION_SAVE 1
#define ACTION_LOAD 2
#define ACTION_CLEAR 3

#define MENU_INFO 0
#define MENU_CALLBACK 1
#define MENU_SUCCESS 2
#define MENU_ERROR 9

#define DEMO_CTRL_MENU(actionIdx, menuIdx) (actionIdx * 100 + menuIdx)

s32 DbCamera_SaveCallback(char* c);
s32 DbCamera_LoadCallback(char* c);
s32 DbCamera_ClearCallback(char* c);
s32 DbCamera_UpdateDemoControl(DbCamera* dbCamera, Camera* cam);

static DbCamera* sDbCamPtr;
static s16 D_8016110C;
static DbCameraAnim sDbCamAnim;

Vec3f* DbCamera_AddVecSph(Vec3f* out, Vec3f* in, VecSph* sph) {
    Vec3f ret;
    Vec3f vec;

    OLib_VecSphGeoToVec3f(&vec, sph);

    ret.x = in->x + vec.x;
    ret.y = in->y + vec.y;
    ret.z = in->z + vec.z;
    *out = ret;
    return out;
}

Vec3f* DbCamera_CalcUpFromPitchYawRoll(Vec3f* dest, s16 pitch, s16 yaw, s16 roll) {
    f32 sinPitch;
    f32 cosPitch;
    f32 sinYaw;
    f32 cosYaw;
    f32 sinNegRoll;
    f32 cosNegRoll;
    Vec3f spA4;
    f32 sp54;
    f32 sp4C;
    f32 cosPitchCosYawSinRoll;
    f32 negSinPitch;
    f32 temp_f10_2;
    f32 cosPitchcosYaw;
    f32 temp_f14;
    f32 negSinPitchSinYaw;
    f32 negSinPitchCosYaw;
    f32 cosPitchSinYaw;
    f32 temp_f4_2;
    f32 temp_f6;
    f32 temp_f8;
    f32 temp_f8_2;
    f32 temp_f8_3;

    sinPitch = Math_SinS(pitch);
    cosPitch = Math_CosS(pitch);
    sinYaw = Math_SinS(yaw);
    cosYaw = Math_CosS(yaw);
    sinNegRoll = Math_SinS(-roll);
    cosNegRoll = Math_CosS(-roll);
    negSinPitch = -sinPitch;
    negSinPitchSinYaw = negSinPitch * sinYaw;
    negSinPitchCosYaw = negSinPitch * cosYaw;
    temp_f14 = 1.0f - cosNegRoll;
    cosPitchSinYaw = cosPitch * sinYaw;
    sp54 = SQ(cosPitchSinYaw);
    sp4C = (cosPitchSinYaw * sinPitch) * ((void)0, temp_f14);
    cosPitchcosYaw = cosPitch * cosYaw;
    temp_f4_2 = ((1.0f - sp54) * cosNegRoll) + sp54;
    cosPitchCosYawSinRoll = cosPitchcosYaw * sinNegRoll;
    temp_f6 = (cosPitchcosYaw * cosPitchSinYaw) * ((void)0, temp_f14);
    temp_f10_2 = sinPitch * sinNegRoll;
    spA4.x = ((negSinPitchSinYaw * temp_f4_2) + (cosPitch * (sp4C - cosPitchCosYawSinRoll))) +
             (negSinPitchCosYaw * (temp_f6 + temp_f10_2));
    sp54 = SQ(sinPitch);
    temp_f4_2 = (sinPitch * cosPitchcosYaw) * ((void)0, temp_f14);
    temp_f8_3 = cosPitchSinYaw * sinNegRoll;
    temp_f8 = sp4C + cosPitchCosYawSinRoll;
    spA4.y = ((negSinPitchSinYaw * temp_f8) + (cosPitch * (((1.0f - sp54) * cosNegRoll) + sp54))) +
             (negSinPitchCosYaw * (temp_f4_2 - temp_f8_3));
    temp_f8_2 = temp_f6 - temp_f10_2;
    spA4.z = ((negSinPitchSinYaw * temp_f8_2) + (cosPitch * (temp_f4_2 + temp_f8_3))) +
             (negSinPitchCosYaw * (((1.0f - SQ(cosPitchcosYaw)) * cosNegRoll) + SQ(cosPitchcosYaw)));
    *dest = spA4;
    return dest;
}

char* DbCamera_SetTextValue(s16 value, char* str, u8 endIdx) {
    char* strIter;
    char sign;

    // OTRTODO - This crashes...
    return;

    strIter = str + (s32)endIdx - 1;
    str[endIdx] = '\0';

    if (value >= 0) {
        sign = ' ';
    } else {
        sign = '-';
        value = -value;
    }

    do {

        // clang-format off
        *strIter-- = (value % 10) + '0'; value /= 10;
        // clang-format on

    } while (value != 0);

    if (sign == '-') {
        *strIter-- = sign;
    }

    while (strIter >= str) {
        *str++ = ' ';
    }

    return strIter;
}

void DbCamera_Vec3SToF(Vec3s* in, Vec3f* out) {
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
}

void DbCamera_Vec3FToS(Vec3f* in, Vec3s* out) {
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
}

void DbCamera_CopyVec3f(Vec3f* in, Vec3f* out) {
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
}

void DbCamera_Vec3SToF2(Vec3s* in, Vec3f* out) {
    out->x = in->x;
    out->y = in->y;
    out->z = in->z;
}

void func_800B3F94(PosRot* posRot, Vec3f* vec, Vec3s* out) {
    VecSph sph;
    Vec3f tempVec;
    OLib_Vec3fDiffToVecSphGeo(&sph, &posRot->pos, vec);
    sph.yaw -= posRot->rot.y;
    OLib_VecSphGeoToVec3f(&tempVec, &sph);
    DbCamera_Vec3FToS(&tempVec, out);
}

void func_800B3FF4(PosRot* posRot, Vec3f* vec, Vec3f* out) {
    VecSph sph;
    Vec3f tempVec;
    DbCamera_CopyVec3f(vec, &tempVec);
    OLib_Vec3fToVecSphGeo(&sph, &tempVec);
    sph.yaw += posRot->rot.y;
    DbCamera_AddVecSph(out, &posRot->pos, &sph);
}

void func_800B404C(PosRot* posRot, Vec3s* vec, Vec3f* out) {
    Vec3f tempVec;
    DbCamera_Vec3SToF(vec, &tempVec);
    func_800B3FF4(posRot, &tempVec, out);
}

s32 func_800B4088(DbCamera* dbCamera, Camera* cam) {
    CutsceneCameraPoint* position;
    CutsceneCameraPoint* lookAt;
    s32 i;

    position = &dbCamera->sub.position[dbCamera->sub.unkIdx];
    lookAt = &dbCamera->sub.lookAt[dbCamera->sub.unkIdx];

    position->continueFlag = -1;
    lookAt->continueFlag = position->continueFlag;
    position->nextPointFrame = 0;
    lookAt->nextPointFrame = 30;
    lookAt->cameraRoll = position->cameraRoll = dbCamera->roll * (360.0f / 256.0f);
    lookAt->viewAngle = position->viewAngle = dbCamera->fov;

    if (dbCamera->sub.mode != 1) {
        DbCamera_Vec3FToS(&dbCamera->eye, &position->pos);
        DbCamera_Vec3FToS(&dbCamera->at, &lookAt->pos);
    } else {
        func_800B3F94(&cam->playerPosRot, &dbCamera->at, &lookAt->pos);
        func_800B3F94(&cam->playerPosRot, &dbCamera->eye, &position->pos);
    }

    for (i = 0; i < (dbCamera->sub.nPoints - 2); i++) {
        dbCamera->sub.position[i].continueFlag = dbCamera->sub.lookAt[i].continueFlag = 0;
    }

    dbCamera->sub.position[i].continueFlag = dbCamera->sub.lookAt[i].continueFlag = -1;

    return dbCamera->sub.unkIdx;
}

s16 func_800B41DC(DbCamera* dbCamera, s16 idx, Camera* cameraPtr) {
    CutsceneCameraPoint* position = &dbCamera->sub.position[idx];
    CutsceneCameraPoint* lookAt = &dbCamera->sub.lookAt[idx];

    if (dbCamera->sub.mode != 1) {
        DbCamera_Vec3SToF2(&position->pos, &dbCamera->eye);
        DbCamera_Vec3SToF2(&lookAt->pos, &dbCamera->at);
    } else {
        func_800B404C(&cameraPtr->playerPosRot, &lookAt->pos, &dbCamera->at);
        func_800B404C(&cameraPtr->playerPosRot, &position->pos, &dbCamera->eye);
    }

    dbCamera->roll = lookAt->cameraRoll;
    dbCamera->rollDegrees = dbCamera->roll * (360.0f / 256.0f);
    dbCamera->fov = lookAt->viewAngle;
    return idx;
}

s32 func_800B42C0(DbCamera* dbCamera, Camera* cameraPtr) {

    CutsceneCameraPoint* position = &dbCamera->sub.position[dbCamera->sub.unkIdx];
    CutsceneCameraPoint* lookAt = &dbCamera->sub.lookAt[dbCamera->sub.unkIdx];

    position->continueFlag = lookAt->continueFlag = 0;

    if (dbCamera->sub.mode != 1) {
        DbCamera_Vec3FToS(&dbCamera->eye, &position->pos);
        DbCamera_Vec3FToS(&dbCamera->at, &lookAt->pos);
    } else {
        func_800B3F94(&cameraPtr->playerPosRot, &dbCamera->at, &lookAt->pos);
        func_800B3F94(&cameraPtr->playerPosRot, &dbCamera->eye, &position->pos);
    }

    return dbCamera->sub.unkIdx;
}

s32 func_800B4370(DbCamera* dbCamera, s16 idx, Camera* cam) {
    CutsceneCameraPoint* lookAt = &dbCamera->sub.lookAt[idx];
    CutsceneCameraPoint* position = &dbCamera->sub.position[idx];
    VecSph sph;
    Vec3f at;

    if (dbCamera->sub.mode != 1) {
        if (dbCamera->sub.unk_0C) {
            DbCamera_Vec3SToF2(&position->pos, &dbCamera->at);
        } else {
            DbCamera_Vec3SToF2(&lookAt->pos, &dbCamera->at);
        }
    } else {
        if (dbCamera->sub.unk_0C) {
            func_800B404C(&cam->playerPosRot, &position->pos, &at);
        } else {
            func_800B404C(&cam->playerPosRot, &lookAt->pos, &at);
        }
        dbCamera->at = at;
    }
    sph.pitch = 0x2000;
    sph.yaw -= 0x7FFF;
    sph.r = 250.0f;
    DbCamera_AddVecSph(&dbCamera->eye, &dbCamera->at, &sph);
    dbCamera->roll = lookAt->cameraRoll;
    dbCamera->rollDegrees = dbCamera->roll * (360.0f / 256.0f);
    dbCamera->fov = lookAt->viewAngle;
    return idx;
}

void func_800B44E0(DbCamera* dbCamera, Camera* cam) {
    s32 i;

    if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CRIGHT)) {
        sDbCamAnim.keyframe = 0;
        sDbCamAnim.unk_0A = 1;
        sDbCamAnim.curFrame = 0.0f;
        sDbCamAnim.unk_04 = 0;

        for (i = 0; i < (dbCamera->sub.nPoints - 2); i++) {
            dbCamera->sub.position[i].continueFlag = dbCamera->sub.lookAt[i].continueFlag = 0;
        }

        dbCamera->sub.position[i].continueFlag = dbCamera->sub.lookAt[i].continueFlag = -1;
    }

    if (dbCamera->sub.nPoints < 6) {
        if (sDbCamAnim.unk_0A != 0) {
            Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            sDbCamAnim.unk_0A = 0;
        }
        func_8006376C(0x11, 0x17, 3, D_8012CEE0[0]);
        func_8006376C(0x12, 0x18, 3, D_8012CEE4);
        func_8006376C(0x10, 0x1A, 1, D_8012CEE8);
        return;
    }

    if (!func_800BB2B4(&sDbCamAnim.positionPos, &sDbCamAnim.roll, &sDbCamAnim.fov, dbCamera->sub.position,
                       &sDbCamAnim.keyframe, &sDbCamAnim.curFrame) &&
        !func_800BB2B4(&sDbCamAnim.lookAtPos, &sDbCamAnim.roll, &sDbCamAnim.fov, dbCamera->sub.lookAt,
                       &sDbCamAnim.keyframe, &sDbCamAnim.curFrame) &&
        sDbCamAnim.unk_0A == 1) {
        Audio_PlaySoundGeneral(NA_SE_SY_HP_RECOVER, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        sDbCamAnim.unk_04++;

        if (dbCamera->sub.nFrames > 0 && dbCamera->sub.nFrames < sDbCamAnim.unk_04) {
            sDbCamAnim.unk_0A = 0;
            func_8006376C(0xF, 0x1A, 1, D_8012CEEC);
        }

        if (dbCamera->sub.mode != 1) {
            DbCamera_CopyVec3f(&sDbCamAnim.positionPos, &dbCamera->eye);
            DbCamera_CopyVec3f(&sDbCamAnim.lookAtPos, &dbCamera->at);
        } else {
            func_800B3FF4(&cam->playerPosRot, &sDbCamAnim.lookAtPos, &dbCamera->at);
            func_800B3FF4(&cam->playerPosRot, &sDbCamAnim.positionPos, &dbCamera->eye);
        }

        dbCamera->fov = sDbCamAnim.fov;
        dbCamera->roll = sDbCamAnim.roll;
        dbCamera->rollDegrees = sDbCamAnim.roll * (360.0f / 256.0f);

        DbCamera_SetTextValue(sDbCamAnim.unk_04, &D_8012CFB4[8], 4);
        func_8006376C(0x10, 0x17, 3, D_8012CFB4);
        D_8012CFC4[5] = ((sDbCamAnim.keyframe + 1) / 10) + '0';
        D_8012CFC4[6] = ((sDbCamAnim.keyframe + 1) % 10) + '0';
        D_8012CFC4[8] = ((dbCamera->sub.nPoints - 5) / 10) + '0';
        D_8012CFC4[9] = ((dbCamera->sub.nPoints - 5) % 10) + '0';
        func_8006376C(0x10, 0x18, 3, D_8012CFC4);
        func_8006376C(0x10, 0x1A, 1, D_8012CEF0);
        return;
    }

    sDbCamAnim.unk_0A = 0;
    func_8006376C(0xF, 0x1A, 1, D_8012CEEC);
}

void DbCamera_PrintPoints(const char* name, s16 count, CutsceneCameraPoint* points) {
    s32 i;

    osSyncPrintf("@@@static SplinedatZ  %s[] = {\n", name);
    for (i = 0; i < count; i++) {
        osSyncPrintf("@@@    /* key frame %2d */ {\n", i);
        osSyncPrintf("@@@    /*     code     */ %d,\n", points[i].continueFlag);
        osSyncPrintf("@@@    /*     z        */ %d,\n", points[i].cameraRoll);
        osSyncPrintf("@@@    /*     T        */ %d,\n", points[i].nextPointFrame);
        osSyncPrintf("@@@    /*     zoom     */ %f,\n", points[i].viewAngle);
        osSyncPrintf("@@@    /*     pos      */ { %d, %d, %d }\n", points[i].pos.x, points[i].pos.y, points[i].pos.z);
        osSyncPrintf("@@@    },\n");
    }
    osSyncPrintf("@@@};\n@@@\n");
}

void DbCamera_PrintF32Bytes(f32 value) {
    f32 b = value;
    char* a = (char*)&b;

    osSyncPrintf("\n@@@%d,%d,%d,%d,", a[0], a[1], a[2], a[3]);
}

void DbCamera_PrintU16Bytes(u16 value) {
    u16 pad;
    u16 b = value;
    char* a = (char*)&b;

    osSyncPrintf("\n@@@%d,%d,", a[0], a[1]);
}

void DbCamera_PrintS16Bytes(s16 value) {
    u16 pad;
    s16 b = value;
    char* a = (char*)&b;

    osSyncPrintf("\n@@@%d,%d,", a[0], a[1]);
}

void DbCamera_PrintCutBytes(DbCameraCut* cut) {
    CutsceneCameraPoint* point;
    CutsceneCameraPoint* points;
    s32 i;

    points = cut->lookAt;
    osSyncPrintf("\n@@@ 0,0,0,2,\t/* Look Camera\t*/");
    osSyncPrintf("\n@@@ 0,1,\t/* dousa\t*/");

    osSyncPrintf("\n@@@ 0,0,\t/* Start Flame\t*/");
    DbCamera_PrintU16Bytes(cut->nFrames);
    osSyncPrintf("\t/*  End   Flame\t*/");

    osSyncPrintf("\n@@@0,0,\t/*  Dammy\t*/\n@@@ ");
    for (i = 0; i < cut->nPoints; i++) {
        point = points + i;
        osSyncPrintf("\n@@@    %d, /*     code     */", point->continueFlag);
        osSyncPrintf("\n@@@    %d,  /*     z        */", point->cameraRoll);
        DbCamera_PrintU16Bytes(point->nextPointFrame);
        osSyncPrintf("\t/*  sokudo\t*/");
        DbCamera_PrintF32Bytes(point->viewAngle);
        osSyncPrintf("\t/*  zoom\t*/");
        DbCamera_PrintS16Bytes(point->pos.x);
        osSyncPrintf("\t/*  x pos\t*/");
        DbCamera_PrintS16Bytes(point->pos.y);
        osSyncPrintf("\t/*  y pos\t*/");
        DbCamera_PrintS16Bytes(point->pos.z);
        osSyncPrintf("\t/*  z pos\t*/\n");
        osSyncPrintf("\n@@@0,0,\t/*  Dammy\t*/\n@@@ ");
    }

    points = cut->position;
    osSyncPrintf("\n@@@ 0,0,0,1,\t/* Position Camera */");
    osSyncPrintf("\n@@@ 0,1,\t/* dousa\t*/");

    osSyncPrintf("\n@@@ 0,0,\t/* Start Flame\t*/");
    DbCamera_PrintU16Bytes(cut->nFrames);
    osSyncPrintf("\t/*  End   Flame\t*/");

    osSyncPrintf("\n@@@0,0,\t/*  Dammy\t*/\n@@@ ");
    for (i = 0; i < cut->nPoints; i++) {
        point = points + i;
        osSyncPrintf("\n@@@    %d, /*     code     */", point->continueFlag);
        osSyncPrintf("\n@@@    %d, /*     z        */", point->cameraRoll);
        DbCamera_PrintU16Bytes(point->nextPointFrame);
        osSyncPrintf("\t/*  sokudo\t*/");
        DbCamera_PrintF32Bytes(point->viewAngle);
        osSyncPrintf("\t/*  zoom\t*/");
        DbCamera_PrintS16Bytes(point->pos.x);
        osSyncPrintf("\t/*  x pos\t*/");
        DbCamera_PrintS16Bytes(point->pos.y);
        osSyncPrintf("\t/*  y pos\t*/");
        DbCamera_PrintS16Bytes(point->pos.z);
        osSyncPrintf("\t/*  z pos\t*/");
        osSyncPrintf("\n@@@0,0,\t/*  Dammy\t*/\n@@@ ");
    }
}

void DbCamera_Init(DbCamera* dbCamera, Camera* cameraPtr) {
    dbCamera->sub.unk_104A.z = 0;
    dbCamera->unk_44 = 0;
    dbCamera->unk_00 = 0;
    dbCamera->unk_34 = 0;
    dbCamera->unk_3C = false;
    dbCamera->unk_38 = -1;
    dbCamera->unk_40 = -1;
    dbCamera->roll = 0;
    dbCamera->sub.unk_104A.y = dbCamera->sub.unk_104A.z;
    dbCamera->sub.unk_104A.x = dbCamera->sub.unk_104A.z;
    dbCamera->fov = 0.0f;
    dbCamera->rollDegrees = 0.0f;
    sPlayState = cameraPtr->play;
    dbCamera->sub.mode = 0;
    dbCamera->sub.nFrames = -1;
    dbCamera->sub.nPoints = 1;
    dbCamera->sub.unkIdx = 0;
    dbCamera->sub.unk_08 = 0;
    dbCamera->sub.unk_0A = 0;
    dbCamera->unk_78 = 0;
    dbCamera->unk_7A = 0;
    dbCamera->sub.demoCtrlMenu = DEMO_CTRL_MENU(ACTION_E, MENU_INFO);
    dbCamera->sub.demoCtrlActionIdx = ACTION_E;
    dbCamera->sub.demoCtrlToggleSwitch = 0;
    dbCamera->unk_6C.x = 0;
    dbCamera->unk_6C.y = 0;
    dbCamera->unk_6C.z = 0;
}

void DbgCamera_Enable(DbCamera* dbCamera, Camera* cam) {
    dbCamera->at = cam->at;
    dbCamera->eye = cam->eye;
    dbCamera->unk_1C = cam->up;
    dbCamera->fov = cam->fov;
    dbCamera->roll = 0;
    dbCamera->sub.nPoints = 1;
    dbCamera->sub.unkIdx = 0;
    dbCamera->sub.unk_08 = 0;
    dbCamera->sub.unk_0A = 1;
    dbCamera->sub.unk_0C = true;
    dbCamera->unk_78 = 0;
    dbCamera->unk_7A = 0;
    dbCamera->rollDegrees = 0.0f;
    func_800B4088(dbCamera, cam);
}

void DbCamera_Update(DbCamera* dbCamera, Camera* cam) {
    static s32 D_8012D10C = 100;
    static s32 D_8012D110 = 0;
    static s32 D_80161140; // bool
    static s32 D_80161144; // bool
    Vec3f* sp124;
    f32 temp_f0_5;
    s16 yaw;
    f32 new_var2;
    f32 temp_f2;
    s16 pitch;
    char sp111;
    char sp110;
    f32 temp_f2_2;
    VecSph sp104;
    VecSph spFC;
    VecSph spF4;
    PosRot* temp_s6;
    Vec3f* eye;
    Vec3f* at;
    Vec3f* phi_s0;
    Vec3f spD8;
    s32 pad;
    Vec3f* sp90;
    Vec3f* sp80;
    Vec3f* sp7C;
    s32 i;
    Vec3f spB8;
    Vec3f spAC;
    s16 spAA;
    VecSph spA0;

    sp90 = &dbCamera->unk_54;
    temp_s6 = &cam->playerPosRot;
    at = &cam->at;
    eye = &cam->eye;
    *sp90 = temp_s6->pos;
    dbCamera->unk_60 = cam->at;
    sp80 = &dbCamera->eye;
    sp7C = &dbCamera->at;

    if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_Z)) {
        dbCamera->unk_00++;
        dbCamera->unk_00 %= 3;
        dbCamera->unk_38 = 1;
        dbCamera->unk_44 = 0;
        dbCamera->unk_40 = -1;
        dbCamera->sub.demoCtrlActionIdx = 0;
        sDbCamAnim.unk_0A = 0;
        Audio_PlaySoundGeneral(NA_SE_SY_LOCK_ON, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    } else if (dbCamera->unk_38 == -1) {
        dbCamera->unk_38 = 1;
    } else {
        dbCamera->unk_38 = 0;
    }

    switch (dbCamera->unk_00) {
        case 0:
            switch (dbCamera->unk_78) {
                case 0:
                    sp124 = &dbCamera->unk_60;
                    D_80161144 = false;
                    D_80161140 = false;
                    break;
                case 1:
                    sp124 = &dbCamera->unk_6C;
                    D_80161144 = false;
                    D_80161140 = false;
                    break;
                case 2:
                    sp124 = &dbCamera->unk_54;
                    D_80161144 = false;
                    D_80161140 = true;
                    break;
            }
            break;
        case 1:
            switch (dbCamera->sub.unk_08) {
                case 0:
                    D_80161144 = dbCamera->sub.unk_0C;
                    if (D_80161144) {
                        sp124 = sp80;
                    } else {
                        sp124 = sp7C;
                    }
                    D_80161140 = false;
                    break;
                case 1:
                    D_80161144 = dbCamera->sub.unk_0C;
                    if (D_80161144) {
                        sp124 = sp80;
                    } else {
                        sp124 = sp7C;
                    }
                    D_80161140 = false;
                    break;
                case 2:
                    D_80161144 = false;
                    D_80161140 = true;
                    sp124 = sp7C;
                    break;
            }
            break;
        case 2:
            DbCamera_UpdateDemoControl(dbCamera, cam);
            return;
        default:
            break;
    }
    phi_s0 = sp124;

    if (!D_80161144) {
        OLib_Vec3fDiffToVecSphGeo(&sp104, sp7C, sp80);
    } else {
        OLib_Vec3fDiffToVecSphGeo(&sp104, sp80, sp7C);
    }

    if (dbCamera->unk_44 > 100) {
        dbCamera->unk_44 = 100;
    }

    new_var2 = ((dbCamera->unk_44 * 0.15f) + 0.2f);
    temp_f2 = new_var2 * (sp104.r / 100.0f);
    if ((dbCamera->unk_38 != 0) || dbCamera->unk_3C) {
        if (D_80161144) {
            *sp80 = *phi_s0;
        } else {
            *sp7C = *phi_s0;
        }

        dbCamera->unk_3C = D_80161140;
        if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_B | BTN_L)) {
            sp104.r += temp_f2;

            if (sp104.r > 30000.0f) {
                sp104.r = 30000.0f;
            }

            if (dbCamera->unk_40 == 7) {
                dbCamera->unk_44++;
            } else {
                dbCamera->unk_44 = 0;
            }

            dbCamera->unk_40 = 7;
        } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_B)) {
            spFC = sp104;
            spFC.r = temp_f2;
            if (!D_80161144) {
                spFC.yaw = sp104.yaw;
                DbCamera_AddVecSph(sp7C, sp7C, &spFC);
            } else {
                spFC.pitch = -spFC.pitch;
                spFC.yaw = BINANG_ROT180(sp104.yaw);
                DbCamera_AddVecSph(sp80, sp80, &spFC);
            }
            if (dbCamera->unk_40 == 0xB) {
                dbCamera->unk_44++;
            } else {
                dbCamera->unk_44 = 0;
            }
            dbCamera->unk_40 = 0xB;
        } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_A | BTN_L)) {
            sp104.r -= temp_f2;
            if (sp104.r < 10.0f) {
                sp104.r = 10.0f;
            }
            if (dbCamera->unk_40 == 8) {
                dbCamera->unk_44++;
            } else {
                dbCamera->unk_44 = 0;
            }
            dbCamera->unk_40 = 8;
        } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_A)) {
            spFC = sp104;
            spFC.r = -temp_f2;
            if (!D_80161144) {
                spFC.yaw = sp104.yaw;
                DbCamera_AddVecSph(sp7C, sp7C, &spFC);
            } else {
                spFC.pitch = -spFC.pitch;
                spFC.yaw = BINANG_ROT180(sp104.yaw);
                DbCamera_AddVecSph(sp80, sp80, &spFC);
            }
            if (dbCamera->unk_40 == 0xC) {
                dbCamera->unk_44++;
            } else {
                dbCamera->unk_44 = 0;
            }
            dbCamera->unk_40 = 0xC;
        } else {
            dbCamera->unk_44 = 0;
            dbCamera->unk_40 = -1;
        }

    } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_DDOWN | BTN_L)) {
        spFC = sp104;
        spFC.r = temp_f2;
        spFC.pitch = 0;
        if (!D_80161144) {
            spFC.yaw = sp104.yaw;
            DbCamera_AddVecSph(sp7C, sp7C, &spFC);
        } else {
            spFC.yaw = BINANG_ROT180(sp104.yaw);
            DbCamera_AddVecSph(sp80, sp80, &spFC);
        }

        if (dbCamera->unk_40 == 1) {
            dbCamera->unk_44++;
        } else {
            dbCamera->unk_44 = 0;
        }
        dbCamera->unk_40 = 1;
    } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_DUP | BTN_L)) {
        spFC = sp104;
        spFC.r = -temp_f2;
        spFC.pitch = 0;
        if (!D_80161144) {
            spFC.yaw = sp104.yaw;
            DbCamera_AddVecSph(sp7C, sp7C, &spFC);
        } else {
            spFC.yaw = BINANG_ROT180(sp104.yaw);
            DbCamera_AddVecSph(sp80, sp80, &spFC);
        }
        if (dbCamera->unk_40 == 2) {
            dbCamera->unk_44++;
        } else {
            dbCamera->unk_44 = 0;
        }
        dbCamera->unk_40 = 2;
    } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_DUP)) {
        spFC = sp104;
        spFC.r = temp_f2;
        spFC.pitch = 0x3FFF;
        spFC.yaw = sp104.yaw;
        if (!D_80161144) {
            DbCamera_AddVecSph(sp7C, sp7C, &spFC);
        } else {
            DbCamera_AddVecSph(sp80, sp80, &spFC);
        }
        if (dbCamera->unk_40 == 3) {
            dbCamera->unk_44++;
        } else {
            dbCamera->unk_44 = 0;
        }
        dbCamera->unk_40 = 3;
    } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_DDOWN)) {
        spFC = sp104;
        spFC.r = temp_f2;
        spFC.pitch = -0x3FFF;
        spFC.yaw = sp104.yaw;
        if (!D_80161144) {
            DbCamera_AddVecSph(sp7C, sp7C, &spFC);
        } else {
            DbCamera_AddVecSph(sp80, sp80, &spFC);
        }
        if (dbCamera->unk_40 == 4) {
            dbCamera->unk_44++;
        } else {
            dbCamera->unk_44 = 0;
        }
        dbCamera->unk_40 = 4;
    } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, (BTN_DRIGHT | BTN_L)) ||
               CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_DRIGHT)) {
        spFC = sp104;
        spFC.r = temp_f2;
        spFC.pitch = 0;
        if (!D_80161144) {
            spFC.yaw = sp104.yaw + 0x3FFF;
            DbCamera_AddVecSph(sp7C, sp7C, &spFC);
        } else {
            spFC.yaw = sp104.yaw - 0x3FFF;
            DbCamera_AddVecSph(sp80, sp80, &spFC);
        }
        if (dbCamera->unk_40 == 5) {
            dbCamera->unk_44++;
        } else {
            dbCamera->unk_44 = 0;
        }
        dbCamera->unk_40 = 5;
    } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, (BTN_DLEFT | BTN_L)) ||
               CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_DLEFT)) {
        spFC = sp104;
        spFC.r = temp_f2;
        spFC.pitch = 0;
        if (!D_80161144) {
            spFC.yaw = sp104.yaw - 0x3FFF;
            DbCamera_AddVecSph(sp7C, sp7C, &spFC);
        } else {
            spFC.yaw = sp104.yaw + 0x3FFF;
            DbCamera_AddVecSph(sp80, sp80, &spFC);
        }
        if (dbCamera->unk_40 == 6) {
            dbCamera->unk_44++;
        } else {
            dbCamera->unk_44 = 0;
        }
        dbCamera->unk_40 = 6;
    } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_B | BTN_L)) {
        sp104.r = sp104.r + temp_f2;
        if (sp104.r > 30000.0f) {
            sp104.r = 30000.0f;
        }
        if (dbCamera->unk_40 == 7) {
            dbCamera->unk_44++;
        } else {
            dbCamera->unk_44 = 0;
        }
        dbCamera->unk_40 = 7;
    } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_B)) {
        spFC = sp104;
        spFC.r = temp_f2;
        if (!D_80161144) {
            spFC.yaw = sp104.yaw;
            DbCamera_AddVecSph(sp7C, sp7C, &spFC);
        } else {
            spFC.pitch = -spFC.pitch;
            spFC.yaw = BINANG_ROT180(sp104.yaw);
            DbCamera_AddVecSph(sp80, sp80, &spFC);
        }
        if (dbCamera->unk_40 == 0xB) {
            dbCamera->unk_44++;
        } else {
            dbCamera->unk_44 = 0;
        }
        dbCamera->unk_40 = 0xB;
    } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_A | BTN_L)) {

        sp104.r -= temp_f2;
        if (sp104.r < 10.0f) {
            sp104.r = 10.0f;
        }
        if (dbCamera->unk_40 == 8) {
            dbCamera->unk_44++;
        } else {
            dbCamera->unk_44 = 0;
        }
        dbCamera->unk_40 = 8;
    } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_A)) {
        spFC = sp104;
        spFC.r = -temp_f2;
        if (!D_80161144) {
            spFC.yaw = sp104.yaw;
            DbCamera_AddVecSph(sp7C, sp7C, &spFC);
        } else {
            spFC.pitch = -spFC.pitch;
            spFC.yaw = BINANG_ROT180(sp104.yaw);
            DbCamera_AddVecSph(sp80, sp80, &spFC);
        }
        if (dbCamera->unk_40 == 0xC) {
            dbCamera->unk_44++;
        } else {
            dbCamera->unk_44 = 0;
        }
        dbCamera->unk_40 = 0xC;
    } else {
        dbCamera->unk_44 = 0;
        dbCamera->unk_40 = -1;
    }

    if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_R)) {
        if (dbCamera->unk_00 == 0) {
            dbCamera->sub.unk_104A = cam->inputDir;
            *sp7C = cam->at;
            *sp80 = cam->eye;
            dbCamera->unk_1C.x = 0.0f;
            dbCamera->unk_1C.z = 0.0f;
            dbCamera->unk_1C.y = 1.0f;
        } else if (dbCamera->sub.unk_08 == 2) {
            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            dbCamera->sub.unk_08 = 0;
            func_800B41DC(dbCamera, dbCamera->sub.unkIdx, cam);
        } else {

            if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_R) &&
                CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L)) {
                Audio_PlaySoundGeneral(NA_SE_SY_CANCEL, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                dbCamera->sub.nPoints = dbCamera->sub.unkIdx + 1;
                func_800B4088(dbCamera, cam);
            } else if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_R)) {
                if (dbCamera->sub.unkIdx == 0x80) {
                    Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                } else {
                    Audio_PlaySoundGeneral(NA_SE_IT_SWORD_PUTAWAY, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    func_800B42C0(dbCamera, cam);
                    if (dbCamera->sub.unkIdx == (dbCamera->sub.nPoints - 1)) {
                        dbCamera->sub.unkIdx++;
                        dbCamera->sub.nPoints++;
                        func_800B4088(dbCamera, cam);
                    }
                }
            }
        }
    } else {
        temp_f0_5 = sPlayState->state.input[2].rel.stick_y;
        temp_f2_2 = sPlayState->state.input[2].rel.stick_x;
        pitch = DEGF_TO_BINANG((SQ(temp_f0_5) / 600.0f) * 0.8f);
        yaw = DEGF_TO_BINANG((SQ(temp_f2_2) / 600.0f) * 0.8f);
        if (!D_80161144) {
            sp104.pitch += (s16)((temp_f0_5 >= 0.0f) ? pitch : -pitch);
            sp104.yaw += (s16)((temp_f2_2 >= 0.0f) ? yaw : -yaw);
            DbCamera_AddVecSph(sp80, sp7C, &sp104);
            dbCamera->sub.unk_104A.x = -sp104.pitch;
            dbCamera->sub.unk_104A.y = BINANG_ROT180(sp104.yaw);
        } else {
            sp104.pitch += (s16)((temp_f0_5 >= 0.0f) ? -pitch : pitch);
            sp104.yaw += (s16)((temp_f2_2 >= 0.0f) ? -yaw : yaw);
            DbCamera_AddVecSph(sp7C, sp80, &sp104);
            dbCamera->sub.unk_104A.x = sp104.pitch;
            dbCamera->sub.unk_104A.y = sp104.yaw;
        }

        OLib_Vec3fDiffToVecSphGeo(&spF4, sp80, sp7C);
        DbCamera_CalcUpFromPitchYawRoll(&dbCamera->unk_1C, spF4.pitch, spF4.yaw, DEGF_TO_BINANG(dbCamera->rollDegrees));
        if (dbCamera->unk_00 == 1) {
            if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_CRIGHT)) {
                cam->inputDir = dbCamera->sub.unk_104A;
                new_var2 = OLib_Vec3fDist(&cam->at, &cam->eye);
                cam->at = *sp7C;
                spFC = sp104;
                spFC.r = new_var2;
                DbCamera_AddVecSph(&cam->eye, &cam->at, &spFC);
            }
        }
    }

    if (dbCamera->unk_00 == 1) {
        OREG(0) = 8;
        func_8006376C(0xC, 5, 0, D_8012CEF4);
        if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_CRIGHT) &&
            !CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L)) {
            func_800B44E0(dbCamera, cam);
        } else {
            if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CRIGHT) &&
                CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L)) {
                Audio_PlaySoundGeneral(NA_SE_SY_GET_RUPY, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                osSyncPrintf("@@@\n@@@\n@@@/* *** spline point data ** start here *** */\n@@@\n");
                DbCamera_PrintPoints("Lookat", dbCamera->sub.nPoints, dbCamera->sub.lookAt);
                DbCamera_PrintPoints("Position", dbCamera->sub.nPoints, dbCamera->sub.position);
                osSyncPrintf("@@@static short  nPoints = %d;\n@@@\n", dbCamera->sub.nPoints);
                osSyncPrintf("@@@static short  nFrames = %d;\n@@@\n", dbCamera->sub.nFrames);
                osSyncPrintf("@@@static short  Mode = %d;\n@@@\n", dbCamera->sub.mode);
                osSyncPrintf("@@@\n@@@\n@@@/* *** spline point data ** finish! *** */\n@@@\n");
            } else if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CLEFT)) {
                Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                dbCamera->sub.unk_08 = (dbCamera->sub.unk_08 + 1) % 3;
            }

            if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CUP) &&
                CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L)) {
                Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                if (dbCamera->sub.unkIdx > 0) {
                    dbCamera->sub.unkIdx--;
                } else {
                    dbCamera->sub.unkIdx = dbCamera->sub.nPoints - 1;
                }
            } else {
                if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CUP)) {
                    Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    if (dbCamera->sub.unkIdx > 0) {
                        dbCamera->sub.unkIdx--;
                    } else {
                        dbCamera->sub.unkIdx = dbCamera->sub.nPoints - 1;
                    }
                    if ((dbCamera->sub.unk_08 == 2) && (dbCamera->sub.unkIdx != (dbCamera->sub.nPoints - 1))) {
                        func_800B4370(dbCamera, dbCamera->sub.unkIdx, cam);
                        dbCamera->roll = 0;
                        dbCamera->fov = 60.0f;
                        dbCamera->rollDegrees = 0;
                    } else {
                        func_800B41DC(dbCamera, dbCamera->sub.unkIdx, cam);
                        dbCamera->fov = dbCamera->sub.lookAt[dbCamera->sub.unkIdx].viewAngle;
                        dbCamera->roll = dbCamera->sub.lookAt[dbCamera->sub.unkIdx].cameraRoll;
                        dbCamera->rollDegrees = dbCamera->roll * 1.40625f;
                    }
                }
            }
            if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L) &&
                CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CDOWN)) {
                Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                if (dbCamera->sub.unkIdx < (dbCamera->sub.nPoints - 1)) {
                    dbCamera->sub.unkIdx++;
                } else {
                    dbCamera->sub.unkIdx = 0;
                }
            } else {
                if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CDOWN)) {
                    Audio_PlaySoundGeneral(NA_SE_IT_SWORD_IMPACT, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    if (dbCamera->sub.unkIdx < (dbCamera->sub.nPoints - 1)) {
                        dbCamera->sub.unkIdx++;
                    } else {
                        dbCamera->sub.unkIdx = 0;
                    }

                    if ((dbCamera->sub.unk_08 == 2) && (dbCamera->sub.unkIdx != (dbCamera->sub.nPoints - 1))) {
                        func_800B4370(dbCamera, dbCamera->sub.unkIdx, cam);
                        dbCamera->roll = 0;
                        dbCamera->fov = 60.0f;
                        dbCamera->rollDegrees = 0;
                    } else {
                        func_800B41DC(dbCamera, dbCamera->sub.unkIdx, cam);
                        dbCamera->fov = dbCamera->sub.lookAt[dbCamera->sub.unkIdx].viewAngle;
                        dbCamera->roll = dbCamera->sub.lookAt[dbCamera->sub.unkIdx].cameraRoll;
                        dbCamera->rollDegrees = dbCamera->roll * 1.40625f;
                    }
                }
            }

            func_8006376C(0xA, 6, (dbCamera->sub.unk_08 == 0) ? 7 : 4, D_8012D00C);
            func_8006376C(0x11, 6, (dbCamera->sub.unk_08 == 1) ? 7 : 4, D_8012D020);
            func_8006376C(0x17, 6, (dbCamera->sub.unk_08 == 2) ? 7 : 4, D_8012D034);
            if (dbCamera->sub.unkIdx == 0x80) {
                func_8006376C(0x10, 0x1A, 1, D_8012CEF8[0]);
            } else if (dbCamera->sub.unkIdx == (dbCamera->sub.nPoints - 1)) {
                D_8012CEE0[7][10] = (dbCamera->sub.nPoints / 10) + '0';
                D_8012CEE0[7][11] = (dbCamera->sub.nPoints % 10) + '0';
                func_8006376C(0xF, 0x1A, 1, D_8012CEE0[7]);
            } else {
                D_8012CEE0[8][10] = ((dbCamera->sub.unkIdx + 1) / 10) + '0';
                D_8012CEE0[8][11] = ((dbCamera->sub.unkIdx + 1) % 10) + '0';
                D_8012CEE0[8][13] = ((dbCamera->sub.nPoints - 1) / 10) + '0';
                D_8012CEE0[8][14] = ((dbCamera->sub.nPoints - 1) % 10) + '0';
                func_8006376C(0xF, 0x1A, 1, D_8012CEE0[8]);
            }

            switch (dbCamera->sub.unk_08) {
                case 2:
                    dbCamera->unk_3C = false;
                    break;
                case 0:
                    dbCamera->unk_3C = false;
                    if (dbCamera->sub.mode != 1) {
                        func_8006376C(0xD, 0x18, 3, !D_80161144 ? D_8012CF04 : D_8012CF08);
                        DbCamera_SetTextValue(sp104.pitch * 0.00549325f, &D_8012D0E4[11], 4);
                        func_8006376C(0xF, 0x17, 3, D_8012D0E4);
                        DbCamera_SetTextValue(sp104.yaw * 0.00549325f, &D_8012D0F8[11], 4);
                        func_8006376C(0xF, 0x18, 3, D_8012D0F8);
                        DbCamera_SetTextValue(sp104.r, &D_8012D0D4[8], 6);
                        func_8006376C(0xF, 0x19, 3, D_8012D0D4);
                    } else {
                        func_8006376C(0xE, 0x18, 3, D_8012CF0C);
                        func_8006376C(0x10, 0x16, 3, D_8012CF10);
                        sp110 = 'X';
                        DbCamera_SetTextValue(temp_s6->pos.x, &sp111, 7);
                        func_8006376C(0x10, 0x17, 3, &sp110);
                        sp110 = 'Y';
                        DbCamera_SetTextValue(temp_s6->pos.y, &sp111, 7);
                        func_8006376C(0x10, 0x18, 3, &sp110);
                        sp110 = 'Z';
                        DbCamera_SetTextValue(temp_s6->pos.z, &sp111, 7);
                        func_8006376C(0x10, 0x19, 3, &sp110);
                    }
                    break;
                case 1:
                    dbCamera->unk_3C = true;
                    if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DUP)) {
                        Audio_PlaySoundGeneral(NA_SE_SY_ATTENTION_ON, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                        if (dbCamera->sub.unk_0A == 0) {
                            dbCamera->sub.unk_0A = 5;
                        } else {
                            dbCamera->sub.unk_0A--;
                        }
                    }
                    if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DDOWN)) {
                        Audio_PlaySoundGeneral(NA_SE_SY_ATTENTION_ON, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                        if (dbCamera->sub.unk_0A == 5) {
                            dbCamera->sub.unk_0A = 0;
                        } else {
                            dbCamera->sub.unk_0A++;
                        }
                    }
                    if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DLEFT)) {
                        Audio_PlaySoundGeneral(NA_SE_SY_ATTENTION_ON, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);
                        switch (dbCamera->sub.unk_0A) {
                            case 1:
                                if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L)) {
                                    dbCamera->sub.lookAt[dbCamera->sub.unkIdx].nextPointFrame -= 5;
                                } else {
                                    dbCamera->sub.lookAt[dbCamera->sub.unkIdx].nextPointFrame--;
                                }
                                break;
                            case 3:
                                dbCamera->sub.mode--;
                                if (dbCamera->sub.mode == -1) {
                                    dbCamera->sub.mode = 2;
                                }
                                if (dbCamera->sub.mode == 1) {
                                    dbCamera->unk_78 = 2;
                                    for (i = 0; i < dbCamera->sub.nPoints; i++) {
                                        DbCamera_Vec3SToF2(&dbCamera->sub.lookAt[i].pos, &spD8);
                                        func_800B3F94(temp_s6, &spD8, &dbCamera->sub.lookAt[i].pos);
                                        DbCamera_Vec3SToF2(&dbCamera->sub.position[i].pos, &spD8);
                                        func_800B3F94(temp_s6, &spD8, &dbCamera->sub.position[i].pos);
                                    }
                                } else {
                                    for (i = 0; i < dbCamera->sub.nPoints; i++) {
                                        func_800B404C(temp_s6, &dbCamera->sub.lookAt[i].pos, &spD8);
                                        DbCamera_Vec3FToS(&spD8, &dbCamera->sub.lookAt[i].pos);
                                        func_800B404C(temp_s6, &dbCamera->sub.position[i].pos, &spD8);
                                        DbCamera_Vec3FToS(&spD8, &dbCamera->sub.position[i].pos);
                                    }
                                }
                                break;
                            case 4:
                                dbCamera->sub.unk_0C = false;
                                break;
                            case 2:
                                if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L)) {
                                    dbCamera->sub.lookAt[dbCamera->sub.unkIdx].cameraRoll -= 5;
                                    dbCamera->roll = dbCamera->sub.lookAt[dbCamera->sub.unkIdx].cameraRoll;
                                } else {
                                    dbCamera->sub.lookAt[dbCamera->sub.unkIdx].cameraRoll--;
                                    dbCamera->roll = dbCamera->sub.lookAt[dbCamera->sub.unkIdx].cameraRoll;
                                }
                                dbCamera->rollDegrees = dbCamera->roll * 1.40625f;
                                break;
                        }
                    }

                    if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_DLEFT)) {
                        if ((D_8012D10C++ % 5) == 0) {
                            Audio_PlaySoundGeneral(NA_SE_SY_ATTENTION_ON, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                        }

                        switch (dbCamera->sub.unk_0A) {
                            case 0:
                                if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L)) {
                                    dbCamera->sub.lookAt[dbCamera->sub.unkIdx].viewAngle -= 1.0f;
                                    dbCamera->fov = dbCamera->sub.lookAt[dbCamera->sub.unkIdx].viewAngle;
                                } else {
                                    dbCamera->sub.lookAt[dbCamera->sub.unkIdx].viewAngle -= 0.2f;
                                    dbCamera->fov = dbCamera->sub.lookAt[dbCamera->sub.unkIdx].viewAngle;
                                }
                                break;
                            case 5:
                                if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L)) {
                                    dbCamera->sub.nFrames -= 10;
                                } else {
                                    dbCamera->sub.nFrames--;
                                }

                                if (dbCamera->sub.nFrames < -1) {
                                    if (dbCamera->sub.nPoints < 5) {
                                        dbCamera->sub.nFrames = -1;
                                    } else {
                                        dbCamera->sub.nFrames = 0;
                                        for (i = 0; i < dbCamera->sub.nPoints; i++) {
                                            dbCamera->sub.nFrames += dbCamera->sub.lookAt[i].nextPointFrame;
                                        }
                                        i = dbCamera->sub.nFrames / dbCamera->sub.nPoints;
                                        dbCamera->sub.nFrames -= (s16)(i * 5);
                                    }
                                }
                                break;
                        }
                    }

                    if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DRIGHT)) {
                        Audio_PlaySoundGeneral(NA_SE_SY_ATTENTION_ON, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                               &D_801333E8);

                        switch (dbCamera->sub.unk_0A) {
                            case 1:
                                if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L)) {
                                    dbCamera->sub.lookAt[dbCamera->sub.unkIdx].nextPointFrame += 5;
                                } else {
                                    dbCamera->sub.lookAt[dbCamera->sub.unkIdx].nextPointFrame++;
                                }
                                break;
                            case 3:
                                dbCamera->sub.mode++;
                                if (dbCamera->sub.mode == 3) {
                                    dbCamera->sub.mode = 0;
                                }
                                if (dbCamera->sub.mode == 1) {
                                    dbCamera->unk_78 = 2;
                                    for (i = 0; i < dbCamera->sub.nPoints; i++) {
                                        DbCamera_Vec3SToF2(&dbCamera->sub.lookAt[i].pos, &spD8);
                                        func_800B3F94(temp_s6, &spD8, &dbCamera->sub.lookAt[i].pos);
                                        DbCamera_Vec3SToF2(&dbCamera->sub.position[i].pos, &spD8);
                                        func_800B3F94(temp_s6, &spD8, &dbCamera->sub.position[i].pos);
                                    }
                                } else {
                                    for (i = 0; i < dbCamera->sub.nPoints; i++) {
                                        func_800B404C(temp_s6, &dbCamera->sub.lookAt[i].pos, &spD8);
                                        DbCamera_Vec3FToS(&spD8, &dbCamera->sub.lookAt[i].pos);
                                        func_800B404C(temp_s6, &dbCamera->sub.position[i].pos, &spD8);
                                        DbCamera_Vec3FToS(&spD8, &dbCamera->sub.position[i].pos);
                                    }
                                }
                            case 4:
                                dbCamera->sub.unk_0C = true;
                                break;
                            case 2:
                                if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L)) {
                                    dbCamera->sub.lookAt[dbCamera->sub.unkIdx].cameraRoll += 5;
                                    dbCamera->roll = dbCamera->sub.lookAt[dbCamera->sub.unkIdx].cameraRoll;
                                } else {
                                    dbCamera->sub.lookAt[dbCamera->sub.unkIdx].cameraRoll++;
                                    dbCamera->roll = dbCamera->sub.lookAt[dbCamera->sub.unkIdx].cameraRoll;
                                }
                                dbCamera->rollDegrees = dbCamera->roll * 1.40625f;
                                break;
                        }
                    }
                    if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_DRIGHT)) {
                        if ((D_8012D10C++ % 5) == 0) {
                            Audio_PlaySoundGeneral(NA_SE_SY_ATTENTION_ON, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                        }

                        switch (dbCamera->sub.unk_0A) {
                            case 0:
                                if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L)) {
                                    dbCamera->sub.lookAt[dbCamera->sub.unkIdx].viewAngle += 1.0f;
                                    dbCamera->fov = dbCamera->sub.lookAt[dbCamera->sub.unkIdx].viewAngle;
                                } else {
                                    dbCamera->sub.lookAt[dbCamera->sub.unkIdx].viewAngle += 0.2f;
                                    dbCamera->fov = dbCamera->sub.lookAt[dbCamera->sub.unkIdx].viewAngle;
                                }
                                break;
                            case 5:
                                if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L)) {
                                    dbCamera->sub.nFrames += 10;
                                } else {
                                    dbCamera->sub.nFrames++;
                                }
                                if (&dbCamera->at) {}
                                break;
                        }
                    }

                    DbCamera_SetTextValue(dbCamera->sub.lookAt[dbCamera->sub.unkIdx].viewAngle, &D_8012D05C[10], 3);
                    func_8006376C(0x10, 0x14, (dbCamera->sub.unk_0A == 0) ? 7 : 4, D_8012D05C);
                    DbCamera_SetTextValue(dbCamera->sub.lookAt[dbCamera->sub.unkIdx].nextPointFrame, &D_8012D070[9], 3);
                    func_8006376C(0x10, 0x15, (dbCamera->sub.unk_0A == 1) ? 7 : 4, D_8012D070);
                    DbCamera_SetTextValue(dbCamera->sub.lookAt[dbCamera->sub.unkIdx].cameraRoll, &D_8012D084[10], 3);
                    func_8006376C(0x10, 0x16, (dbCamera->sub.unk_0A == 2) ? 7 : 4, D_8012D084);
                    func_8006376C(0xF, 0x17, (dbCamera->sub.unk_0A == 3) ? 7 : 4,
                                  (dbCamera->sub.mode == 1)   ? D_8012CF14
                                  : (dbCamera->sub.mode == 0) ? *D_8012CF18
                                                              : D_8012CFB0);
                    if (dbCamera->sub.unk_0C) {
                        D_8012D05C[80] = '>';
                    } else {
                        D_8012D05C[80] = '<';
                    }

                    D_8012D05C[81] = ' ';
                    D_8012D05C[94] = ' ';

                    if (dbCamera->sub.unk_0C) {
                        D_8012D05C[95] = '>';
                    } else {
                        D_8012D05C[95] = '<';
                    }
                    D_8012D05C[96] = '\0';
                    func_8006376C(0xF, 0x18, (dbCamera->sub.unk_0A == 4) ? 7 : 4, D_8012D0AC);
                    DbCamera_SetTextValue(dbCamera->sub.nFrames, &D_8012D0C0[10], 5);
                    func_8006376C(0xF, 0x19, (dbCamera->sub.unk_0A == 5) ? 7 : 4,
                                  (dbCamera->sub.nFrames == -1) ? *D_8012CF24 : D_8012D0C0);
                    break;
            }

            if (dbCamera->sub.mode != 1) {
                func_8006376C(3, 0x16,
                              ((dbCamera->sub.unk_08 == 1) && (dbCamera->sub.unk_0A == 4) && !D_80161144) ? 7
                              : !D_80161144                                                               ? 4
                                                                                                          : 3,
                              D_8012CF30);
                sp110 = 'X';
                DbCamera_SetTextValue(dbCamera->at.x, &sp111, 6);
                func_8006376C(3, 0x17, 2, &sp110);
                sp110 = 'Y';
                DbCamera_SetTextValue(dbCamera->at.y, &sp111, 6);
                func_8006376C(3, 0x18, 2, &sp110);
                sp110 = 'Z';
                DbCamera_SetTextValue(dbCamera->at.z, &sp111, 6);
                func_8006376C(3, 0x19, 2, &sp110);
                func_8006376C(0x1E, 0x16,
                              ((dbCamera->sub.unk_08 == 1) && (dbCamera->sub.unk_0A == 4) && D_80161144) ? 7
                              : D_80161144                                                               ? 4
                                                                                                         : 3,
                              D_8012CF34);
                sp110 = 'X';
                DbCamera_SetTextValue(dbCamera->eye.x, &sp111, 6);
                func_8006376C(0x1E, 0x17, 2, &sp110);
                sp110 = 'Y';
                DbCamera_SetTextValue(dbCamera->eye.y, &sp111, 6);
                func_8006376C(0x1E, 0x18, 2, &sp110);
                sp110 = 'Z';
                DbCamera_SetTextValue(dbCamera->eye.z, &sp111, 6);
                func_8006376C(0x1E, 0x19, 2, &sp110);
            } else {
                if (D_8012CEE0[0]) {}
                OLib_Vec3fDiffToVecSphGeo(&spFC, sp90, sp7C);
                spFC.yaw -= cam->playerPosRot.rot.y;
                func_8006376C(3, 0x16,
                              ((dbCamera->sub.unk_08 == 1) && (dbCamera->sub.unk_0A == 4) && !D_80161144) ? 7
                              : !D_80161144                                                               ? 4
                                                                                                          : 3,
                              D_8012CF30);
                DbCamera_SetTextValue(spFC.pitch * 0.00549325f, &D_8012D0E4[10], 4);
                func_8006376C(3, 0x17, 3, D_8012D0E4);
                DbCamera_SetTextValue(spFC.yaw * 0.00549325f, &D_8012D0F8[10], 4);
                func_8006376C(3, 0x18, 3, D_8012D0F8);
                DbCamera_SetTextValue(spFC.r, &D_8012D0D4[7], 6);
                func_8006376C(3, 0x19, 3, D_8012D0D4);
                OLib_Vec3fDiffToVecSphGeo(&spFC, sp90, sp80);
                spFC.yaw -= cam->playerPosRot.rot.y;
                func_8006376C(0x1E, 0x16,
                              ((dbCamera->sub.unk_08 == 1) && (dbCamera->sub.unk_0A == 4) && D_80161144) ? 7
                              : D_80161144                                                               ? 4
                                                                                                         : 3,
                              D_8012CF34);
                DbCamera_SetTextValue(spFC.pitch * 0.00549325f, &D_8012D0E4[10], 4);
                func_8006376C(0x1C, 0x17, 3, D_8012D0E4);
                DbCamera_SetTextValue(spFC.yaw * 0.00549325f, &D_8012D0F8[10], 4);
                func_8006376C(0x1C, 0x18, 3, D_8012D0F8);
                DbCamera_SetTextValue(spFC.r, &D_8012D0D4[7], 6);
                func_8006376C(0x1C, 0x19, 3, D_8012D0D4);
            }

            DebugDisplay_AddObject(dbCamera->at.x, dbCamera->at.y + 1.0f, dbCamera->at.z, 0, 0, 0, 0.02f, 2.0f, 0.02f,
                                   0xFF, 0xFF, 0x7F, 0x40, 0, cam->play->view.gfxCtx);
            DebugDisplay_AddObject(dbCamera->at.x, dbCamera->at.y + 1.0f, dbCamera->at.z, 0, 0, 0, 2.0f, 0.02f, 0.02f,
                                   0x7F, 0xFF, 0xFF, 0x40, 0, cam->play->view.gfxCtx);
            DebugDisplay_AddObject(dbCamera->at.x, dbCamera->at.y + 1.0f, dbCamera->at.z, 0, 0, 0, 0.02f, 0.02f, 2.0f,
                                   0xFF, 0x7F, 0xFF, 0x40, 0, cam->play->view.gfxCtx);
            if (dbCamera->sub.unk_08 == 2) {
                for (i = 0; i < (dbCamera->sub.nPoints - 1); i++) {
                    if (dbCamera->sub.mode != 1) {
                        DbCamera_Vec3SToF2(&(dbCamera->sub.position + i)->pos, &spAC);
                        DbCamera_Vec3SToF2(&(dbCamera->sub.lookAt + i)->pos, &spB8);
                    } else {
                        func_800B404C(temp_s6, &(dbCamera->sub.lookAt + i)->pos, &spB8);
                        func_800B404C(temp_s6, &(dbCamera->sub.position + i)->pos, &spAC);
                    }
                    OLib_Vec3fDiffToVecSphGeo(&spFC, &spAC, &spB8);
                    spAA = dbCamera->sub.lookAt[i].cameraRoll * 0xB6;
                    if (i == dbCamera->sub.unkIdx) {
                        DebugDisplay_AddObject(spAC.x, spAC.y, spAC.z, spFC.pitch * -1, spFC.yaw, spAA, .5f, .5f, .5f,
                                               0x7F, 0xFF, 0x7F, 0x80, 5, cam->play->view.gfxCtx);
                        DebugDisplay_AddObject(spB8.x, spB8.y, spB8.z, spFC.pitch * -1, spFC.yaw, spAA, 1.5f, 2.0f,
                                               1.0f, 0x7F, 0xFF, 0x7F, 0x80, 4, cam->play->view.gfxCtx);
                    } else {
                        DebugDisplay_AddObject(spAC.x, spAC.y, spAC.z, spFC.pitch * -1, spFC.yaw, spAA, .5f, .5f, .5f,
                                               0xFF, 0x7F, 0x7F, 0x80, 5, cam->play->view.gfxCtx);
                        DebugDisplay_AddObject(spB8.x, spB8.y, spB8.z, spFC.pitch * -1, spFC.yaw, spAA, 1.5f, 2.0f,
                                               1.0f, 0xFF, 0x7F, 0x7F, 0x80, 4, cam->play->view.gfxCtx);
                    }
                }
            }
        }
    } else {
        OREG(0) = 8;
        dbCamera->roll = 0;
        dbCamera->fov = 60.0f;
        dbCamera->rollDegrees = dbCamera->roll * 1.40625f;
        if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CLEFT)) {
            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            dbCamera->unk_78 = (dbCamera->unk_78 + 1) % 3;
            dbCamera->unk_38 = -1;
        }

        func_8006376C(0xE, 5, 0, D_8012CF38);
        func_8006376C(9, 6, (dbCamera->unk_78 == 0) ? 7 : 4, D_8012CFD0);
        func_8006376C(0x11, 6, (dbCamera->unk_78 == 1) ? 7 : 4, D_8012CFE4);
        func_8006376C(0x18, 6, (dbCamera->unk_78 == 2) ? 7 : 4, D_8012CFF8);
        func_8006376C(3, 0x16, D_80161144 ? 3 : 4, D_8012CF30);
        sp110 = 'X';
        DbCamera_SetTextValue(dbCamera->at.x, &sp111, 6);
        func_8006376C(3, 0x17, 2, &sp110);
        sp110 = 'Y';
        DbCamera_SetTextValue(dbCamera->at.y, &sp111, 6);
        func_8006376C(3, 0x18, 2, &sp110);
        sp110 = 'Z';
        DbCamera_SetTextValue(dbCamera->at.z, &sp111, 6);
        func_8006376C(3, 0x19, 2, &sp110);
        func_8006376C(0x1E, 0x16, D_80161144 ? 4 : 3, D_8012CF34);
        sp110 = 'X';
        DbCamera_SetTextValue(dbCamera->eye.x, &sp111, 6);
        func_8006376C(0x1E, 0x17, 2, &sp110);
        sp110 = 'Y';
        DbCamera_SetTextValue(dbCamera->eye.y, &sp111, 6);
        func_8006376C(0x1E, 0x18, 2, &sp110);
        sp110 = 'Z';
        DbCamera_SetTextValue(dbCamera->eye.z, &sp111, 6);
        func_8006376C(0x1E, 0x19, 2, &sp110);
        func_8006376C(0xD, 0x18, 3, !D_80161144 ? D_8012CF04 : D_8012CF08);
        DbCamera_SetTextValue(sp104.pitch * 0.00549325f, &D_8012D0E4[11], 4);
        func_8006376C(0xF, 0x17, 3, D_8012D0E4);
        DbCamera_SetTextValue(sp104.yaw * 0.00549325f, &D_8012D0F8[11], 4);
        func_8006376C(0xF, 0x18, 3, D_8012D0F8);
        DbCamera_SetTextValue(sp104.r, &D_8012D0D4[8], 6);
        func_8006376C(0xF, 0x19, 3, D_8012D0D4);
        if (dbCamera->unk_3C) {
            func_8006376C(0x10, 0x1A, 1, D_8012CF3C);
        } else {
            func_8006376C(0x10, 0x1A, 1, D_8012CF40);
        }

        D_8012D110++;
        D_8012D110 %= 50;

        OLib_Vec3fDiffToVecSphGeo(&spA0, &cam->eye, &cam->at);
        DebugDisplay_AddObject(dbCamera->at.x, dbCamera->at.y + 1.0f, dbCamera->at.z, 0, 0, 0, 0.02f, 2.0f, 0.02f, 0xFF,
                               0xFF, 0x7F, 0x2D, 0, cam->play->view.gfxCtx);
        DebugDisplay_AddObject(dbCamera->at.x, dbCamera->at.y + 1.0f, dbCamera->at.z, 0, 0, 0, 2.0f, 0.02f, 0.02f, 0x7F,
                               0xFF, 0xFF, 0x2D, 0, cam->play->view.gfxCtx);
        DebugDisplay_AddObject(dbCamera->at.x, dbCamera->at.y + 1.0f, dbCamera->at.z, 0, 0, 0, 0.02f, 0.02f, 2.0f, 0xFF,
                               0x7F, 0xFF, 0x2D, 0, cam->play->view.gfxCtx);
        DebugDisplay_AddObject(cam->eye.x, cam->eye.y, cam->eye.z, spA0.pitch * -1, spA0.yaw, 0, .5f, .5f, .5f, 0xFF,
                               0x7F, 0x7F, 0x80, 5, cam->play->view.gfxCtx);
        DebugDisplay_AddObject(cam->at.x, cam->at.y, cam->at.z, spA0.pitch * -1, spA0.yaw, 0, 1.5f, 2.0f, 1.0f, 0xFF,
                               0x7F, 0x7F, 0x80, 4, cam->play->view.gfxCtx);
        OLib_Vec3fDiffToVecSphGeo(&spA0, &cam->eyeNext, &cam->at);
        DebugDisplay_AddObject(cam->eyeNext.x, cam->eyeNext.y, cam->eyeNext.z, spA0.pitch * -1, spA0.yaw, 0, .5f, .5f,
                               .5f, 0xFF, 0xC0, 0x7F, 0x50, 5, cam->play->view.gfxCtx);
    }
}

static s16 sCurFileIdx;
static s16 sLastFileIdx; // holds the file index of the slot to move
// is the size correct? todo: add ALIGN32 for sizeof in Mempak functions, replace 0xF with sizeof()
static DbCameraCut sDbCameraCuts[16];
static char D_80161250[0x80];
static char sLetters[26];
static char D_801612EA;
static s32 sAllocSize;

s32 DbCamera_GetFirstAvailableLetter(void) {
    s32 i;
    for (i = 0; i < ARRAY_COUNT(sLetters); i++) {
        switch (sLetters[i]) {
            case 'O':
                break;
            default:
                return 'A' + i;
        }
    }

    return '?';
}

char DbCamera_InitCut(s32 idx, DbCameraSub* sub) {
    s32 i;

    sDbCameraCuts[idx].unk_01 = 0x61;
    sDbCameraCuts[idx].letter = DbCamera_GetFirstAvailableLetter();
    D_80161250[0x3F + sDbCameraCuts[idx].letter] = 'O';

    i = sub->nPoints * sizeof(CutsceneCameraPoint);
    sDbCameraCuts[idx].lookAt = DEBUG_ARENA_MALLOC_DEBUG(i);
    if (sDbCameraCuts[idx].lookAt == NULL) {
        // "Debug camera memory allocation failure"
        osSyncPrintf("%s: %d: デバッグカメラ メモリ確保失敗！！\n", __FILE__, __LINE__);
        return '?';
    }

    sDbCameraCuts[idx].position = DEBUG_ARENA_MALLOC_DEBUG(i);
    if (sDbCameraCuts[idx].position == NULL) {
        // "Debug camera memory allocation failure"
        osSyncPrintf("%s: %d: デバッグカメラ メモリ確保失敗！！\n", __FILE__, __LINE__);
        DEBUG_ARENA_FREE_DEBUG(sDbCameraCuts[idx].lookAt);
        sDbCameraCuts[idx].lookAt = NULL;
        return '?';
    }

    sDbCameraCuts[idx].mode = sub->mode;
    sDbCameraCuts[idx].nFrames = sub->nFrames;
    sDbCameraCuts[idx].nPoints = sub->nPoints;

    for (i = 0; i < sub->nPoints; i++) {
        sDbCameraCuts[idx].lookAt[i] = sub->lookAt[i];
        sDbCameraCuts[idx].position[i] = sub->position[i];
    }

    return sDbCameraCuts[idx].letter;
}

void DbCamera_ResetCut(s32 idx, s32 shouldFree) {
    if (sDbCameraCuts[idx].letter != '?') {
        D_80161250[0x3F + sDbCameraCuts[idx].letter] = 'X';
    }

    if (shouldFree) {
        DEBUG_ARENA_FREE_DEBUG(sDbCameraCuts[idx].lookAt);
        DEBUG_ARENA_FREE_DEBUG(sDbCameraCuts[idx].position);
    }

    sDbCameraCuts[idx].letter = '?';
    sDbCameraCuts[idx].lookAt = NULL;
    sDbCameraCuts[idx].position = NULL;
    sDbCameraCuts[idx].mode = 0;
    sDbCameraCuts[idx].nFrames = 0;
    sDbCameraCuts[idx].nPoints = 0;
}

s32 DbCamera_CalcMempakAllocSize(void) {
    s32 i;

    sAllocSize = 0;
    for (i = 0; i < ARRAY_COUNT(sDbCameraCuts) - 1; i++) {
        if (sDbCameraCuts[i].letter != '?') {
            sAllocSize += ALIGN32(sDbCameraCuts[i].nPoints * sizeof(CutsceneCameraPoint)) * 2;
        }
    }
    sAllocSize += 0x100;
    sAllocSize = ALIGN256(sAllocSize);
    return sAllocSize;
}

s32 DbCamera_GetMempakAllocSize(void) {
    return sAllocSize;
}

s32 DbCamera_LoadCallback(char* c) {
    s32 i;
    ptrdiff_t size;
    s32 off;

    for (i = 0; i < ARRAY_COUNT(sDbCameraCuts) - 1; i++) {
        if (sDbCameraCuts[i].letter != '?') {
            DbCamera_ResetCut(i, true);
            sLetters[i] = 'X';
        }
    }

    if (!Mempak_Read(2, *c, sDbCameraCuts, 0, sizeof(sDbCameraCuts))) {
        return false;
    }

    off = sizeof(sDbCameraCuts);
    for (i = 0; i < ARRAY_COUNT(sDbCameraCuts) - 1; i++) {
        if (sDbCameraCuts[i].letter != '?') {
            size = sDbCameraCuts[i].nPoints * sizeof(CutsceneCameraPoint);

            sDbCameraCuts[i].lookAt = DEBUG_ARENA_MALLOC_DEBUG(ALIGN32(size));
            if (sDbCameraCuts[i].lookAt == NULL) {
                // "Debug camera memory allocation failure"
                osSyncPrintf("%s: %d: デバッグカメラ メモリ確保失敗！！\n", __FILE__, __LINE__);
                return false;
            }
            if (!Mempak_Read(2, *c, sDbCameraCuts[i].lookAt, off, ALIGN32(size))) {
                return false;
            }
            off += ALIGN32(size);

            sDbCameraCuts[i].position = DEBUG_ARENA_MALLOC_DEBUG(ALIGN32(size));
            if (sDbCameraCuts[i].position == NULL) {
                // "Debug camera memory allocation failure"
                osSyncPrintf("%s: %d: デバッグカメラ メモリ確保失敗！！\n", __FILE__, __LINE__);
                return false;
            }
            if (!Mempak_Read(2, *c, sDbCameraCuts[i].position, off, ALIGN32(size))) {
                return false;
            }
            off += ALIGN32(size);

            D_80161250[0x3F + sDbCameraCuts[i].letter] = 'O';
        }
    }

    return true;
}

s32 DbCamera_SaveCallback(char* c) {
    s32 pad[2];
    s32 ret;
    size_t freeSize;
    ptrdiff_t off;
    ptrdiff_t size;
    s32 i;

    ret = Mempak_GetFileSize(2, *c);
    freeSize = Mempak_GetFreeBytes(2);

    if ((u32)sAllocSize < (freeSize + ret)) {
        if (!Mempak_Alloc(2, c, sAllocSize)) {
            return false;
        }

        if (!Mempak_Write(2, *c, sDbCameraCuts, 0, sizeof(sDbCameraCuts))) {
            Mempak_DeleteFile(2, *c);
            return false;
        }

        off = sizeof(sDbCameraCuts);
        for (i = 0; i < ARRAY_COUNT(sDbCameraCuts) - 1; i++) {
            if (sDbCameraCuts[i].letter != '?') {
                size = sDbCameraCuts[i].nPoints * sizeof(CutsceneCameraPoint);

                ret = Mempak_Write(2, *c, sDbCameraCuts[i].lookAt, off, ALIGN32(size));
                if (!ret) {
                    break;
                }
                off += ALIGN32(size);

                ret = Mempak_Write(2, *c, sDbCameraCuts[i].position, off, ALIGN32(size));
                if (!ret) {
                    break;
                }

                off += ALIGN32(size);
            }
            ret = true;
        }

        if (ret) {
            return *c;
        } else {
            Mempak_DeleteFile(2, *c);
            return false;
        }
    }

    return false;
}

s32 DbCamera_ClearCallback(char* c) {
    return Mempak_DeleteFile(2, *c);
}

void DbCamera_DrawSlotLetters(char* str, s16 y, s16 x, s32 colorId) {
    s32 i;

    for (i = 0; i < ARRAY_COUNT(sDbCameraCuts) - 1; i++) {
        str[i * 2 + 1] = sDbCameraCuts[i].letter;
        str[i * 2 + 0] = '-';
    }

    str[0x14] = str[i * 2 + 1] = '\0';
    func_8006376C(x, y, colorId, str);
    str[0x14] = str[i * 2 + 0] = '-';
    func_8006376C(x + 0x14, y, colorId, str + 0x14);
}

void DbCamera_PrintAllCuts(Camera* cam) {
    s32 i;

    Audio_PlaySoundGeneral(NA_SE_SY_GET_RUPY, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
    osSyncPrintf("@@@\n@@@\n@@@/* ****** spline point data ** start here ***** */\n@@@\n");

    for (i = 0; i < ARRAY_COUNT(sDbCameraCuts) - 1; i++) {
        DbCameraCut* cut = &sDbCameraCuts[i];
        if (cut->nPoints != 0) {
            if (i != 0) {
                osSyncPrintf("@@@\n@@@/* ** %d ** */\n@@@\n", i);
            }

            DbCamera_PrintPoints("Lookat", cut->nPoints, cut->lookAt);
            DbCamera_PrintPoints("Position", cut->nPoints, cut->position);
            osSyncPrintf("@@@static short  nPoints = %d;\n@@@\n", cut->nPoints);
            osSyncPrintf("@@@static short  nFrames = %d;\n@@@\n", cut->nFrames);
            osSyncPrintf("@@@static short  Mode = %d;\n@@@\n", cut->mode);
        }
    }

    osSyncPrintf("@@@\n@@@\n@@@/* ****** spline point data ** finish! ***** */\n@@@\n");
}

char D_8012D114[] = GFXP_KATAKANA "ﾌﾚ-ﾑ         ";
char D_8012D128[] = GFXP_KATAKANA "ﾄ-ﾀﾙ         ";
char D_8012D13C[] = GFXP_KATAKANA "ｷ-     /   ";

s32 func_800B91B0(Camera* cam, DbCamera* dbCamera) {
    s32 pointCount;
    s32 curPoint;

    while (sDbCameraCuts[D_8016110C].letter == '?') {
        D_8016110C++;
        if (D_8016110C >= ARRAY_COUNT(sDbCameraCuts) - 1) {
            sDbCamAnim.curFrame = 0.0f;
            sDbCamAnim.unk_04 = 0;
            sDbCamAnim.keyframe = 0;
            sDbCamAnim.unk_0A = 0;
            D_8016110C = 0;
            return 0;
        }
    }

    if (!func_800BB2B4(&sDbCamAnim.positionPos, &sDbCamAnim.roll, &sDbCamAnim.fov, sDbCameraCuts[D_8016110C].position,
                       &sDbCamAnim.keyframe, &sDbCamAnim.curFrame) &&
        !func_800BB2B4(&sDbCamAnim.lookAtPos, &sDbCamAnim.roll, &sDbCamAnim.fov, sDbCameraCuts[D_8016110C].lookAt,
                       &sDbCamAnim.keyframe, &sDbCamAnim.curFrame)) {

        D_8012D13C[7] = ((sDbCamAnim.keyframe + 1) / 10) + '0';
        D_8012D13C[8] = ((sDbCamAnim.keyframe + 1) % 10) + '0';
        D_8012D13C[10] = ((sDbCameraCuts[D_8016110C].nPoints - 5) / 10) + '0';
        D_8012D13C[11] = ((sDbCameraCuts[D_8016110C].nPoints - 5) % 10) + '0';
        DbCamera_SetTextValue(sDbCamAnim.unk_04, &D_8012D114[10], 4);
        func_8006376C(0xF, 0x16, 3, D_8012D114);
        DbCamera_SetTextValue(sDbCamAnim.unk_0C, &D_8012D128[10], 4);
        func_8006376C(0xF, 0x17, 3, D_8012D128);
        func_8006376C(0xF, 0x18, 3, D_8012D13C);
        func_8006376C(0x10, 0x1A, 1, D_8012CEF0);

        sDbCamAnim.unk_04++;
        sDbCamAnim.unk_0C++;

        if (sDbCameraCuts[D_8016110C].nFrames > 0 && sDbCameraCuts[D_8016110C].nFrames < sDbCamAnim.unk_04) {
            D_8016110C++;
            sDbCamAnim.curFrame = 0.0f;
            sDbCamAnim.unk_04 = 0;
            sDbCamAnim.keyframe = 0;
            return D_8016110C | 0x8000;
        }

        if (sDbCameraCuts[D_8016110C].mode != 1) {
            DbCamera_CopyVec3f(&sDbCamAnim.positionPos, &dbCamera->eye);
            DbCamera_CopyVec3f(&sDbCamAnim.lookAtPos, &dbCamera->at);
        } else {
            func_800B3FF4(&cam->playerPosRot, &sDbCamAnim.lookAtPos, &dbCamera->at);
            func_800B3FF4(&cam->playerPosRot, &sDbCamAnim.positionPos, &dbCamera->eye);
        }

        dbCamera->fov = sDbCamAnim.fov;
        dbCamera->roll = sDbCamAnim.roll;
        dbCamera->rollDegrees = sDbCamAnim.roll * (360.0f / 256.0f);

    } else {
        D_8016110C++;
        sDbCamAnim.keyframe = 0;
        sDbCamAnim.curFrame = 0.0f;
        sDbCamAnim.unk_04 = 0;
        if (D_8016110C == ARRAY_COUNT(sDbCameraCuts) - 1) {
            D_8016110C = 0;
            sDbCamAnim.curFrame = 0.0f;
            sDbCamAnim.keyframe = 0;
            sDbCamAnim.unk_0A = 0;
            return -1;
        }
    }

    return D_8016110C | 0x8000;
}

void DbCamera_Reset(Camera* cam, DbCamera* dbCam) {
    s32 i;

    D_801612EA = '*';
    for (i = 0; i < ARRAY_COUNT(sLetters); i++) {
        sLetters[i] = 'X';
    }

    for (i = 0; i < 0xF; i++) {
        DbCamera_ResetCut(i, false);
    }
    sDbCamPtr = dbCam;
    D_8016110C = 0;
    sCurFileIdx = 0;
    sLastFileIdx = -1;
    sDbCamAnim.unk_0A = 0;
}

s32 DbCamera_UpdateDemoControl(DbCamera* dbCamera, Camera* cam) {
    static s32 sMempakFiles;
    static u32 sDbCameraColors[] = {
        4, 4, 4, 7, 4, 4,
    };
    static s32 sMempakFilesize = 0;
    s32 i;
    s32 idx1;
    s32 idx2;
    s16 idx3;
    char sp74[(ARRAY_COUNT(sDbCameraCuts) - 1 + 4) * 2];
    DbCameraCut sp64;
    VecSph sp5C;
    s32 (*callbacks[])(char*) = { DbCamera_SaveCallback, DbCamera_LoadCallback, DbCamera_ClearCallback };

    func_8006376C(0xE, 5, 0, D_8012CF44); // DEMO CONTROL

    idx1 = sCurFileIdx >> 1;
    idx2 = sLastFileIdx >> 1;

    switch (dbCamera->sub.demoCtrlActionIdx) {
        case ACTION_SAVE:
        case ACTION_LOAD:
        case ACTION_CLEAR:
            switch (dbCamera->sub.demoCtrlMenu) {

                case DEMO_CTRL_MENU(ACTION_SAVE, MENU_INFO):
                case DEMO_CTRL_MENU(ACTION_LOAD, MENU_INFO):
                case DEMO_CTRL_MENU(ACTION_CLEAR, MENU_INFO): {
                    if ((1 << sCurFileIdx) & sMempakFiles) {
                        if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DLEFT) ||
                            CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DRIGHT)) {
                            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                            dbCamera->sub.demoCtrlToggleSwitch ^= 1;
                        }
                        D_8012CEE0[41][9] = sCurFileIdx + 'A';
                        func_8006376C(0xA, 7, 5, D_8012CEE0[41]);
                        func_8006376C(0x10, 7, 5, D_8012CF60[dbCamera->sub.demoCtrlActionIdx]);
                        func_8006376C(0x14, 7, 5, D_8012CF88[0]);

                        func_8006376C(0x11, 8, dbCamera->sub.demoCtrlToggleSwitch ? 4 : 7, D_8012CF94);
                        func_8006376C(0x15, 8, dbCamera->sub.demoCtrlToggleSwitch ? 7 : 4, D_8012CF98);

                        if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_A)) {
                            if (dbCamera->sub.demoCtrlToggleSwitch == 0) {
                                Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                       &D_801333E8);
                                dbCamera->sub.demoCtrlMenu++;
                            } else {
                                Audio_PlaySoundGeneral(NA_SE_SY_CANCEL, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                       &D_801333E8);
                                dbCamera->sub.demoCtrlMenu = 0;
                            }
                        }
                    } else {
                        if (dbCamera->sub.demoCtrlMenu == 100) {
                            dbCamera->sub.demoCtrlMenu++;
                        } else {
                            dbCamera->sub.demoCtrlToggleSwitch ^= 1;
                            D_8012CF84[9] = sCurFileIdx + 'A';
                            func_8006376C(0xD, 7, 5, D_8012CF88[-1]); // todo: find something better
                            func_8006376C(0x12, 7, 5, D_8012CF80);
                            func_8006376C(0xD, 9, dbCamera->sub.demoCtrlToggleSwitch ? 1 : 6, "PRESS B BUTTON");
                        }
                    }
                    if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_B)) {
                        Audio_PlaySoundGeneral(NA_SE_SY_CANCEL, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                        dbCamera->sub.demoCtrlMenu = 0;
                        return 1;
                    }
                    goto block_2;
                }

                case DEMO_CTRL_MENU(ACTION_SAVE, MENU_CALLBACK):
                case DEMO_CTRL_MENU(ACTION_LOAD, MENU_CALLBACK):
                case DEMO_CTRL_MENU(ACTION_CLEAR, MENU_CALLBACK): {
                    D_8012CEE0[41][9] = sCurFileIdx + 'A';
                    func_8006376C(0xC, 7, 5, D_8012CEE0[41]);
                    func_8006376C(0x12, 7, 5, D_8012CF60[dbCamera->sub.demoCtrlActionIdx]);
                    func_8006376C(0x16, 7, 5, D_8012CF9C[0]);

                    if (callbacks[dbCamera->sub.demoCtrlActionIdx - 1](&D_8012CF84[9])) {
                        dbCamera->sub.demoCtrlMenu++;
                        return 1;
                    } else {
                        dbCamera->sub.demoCtrlMenu += 8;
                        return 1;
                    }
                }

                case DEMO_CTRL_MENU(ACTION_SAVE, MENU_SUCCESS):
                case DEMO_CTRL_MENU(ACTION_LOAD, MENU_SUCCESS):
                case DEMO_CTRL_MENU(ACTION_CLEAR, MENU_SUCCESS): {
                    dbCamera->sub.demoCtrlToggleSwitch ^= 1;
                    D_8012CEE0[41][9] = sCurFileIdx + 'A';
                    func_8006376C(0xD, 7, 5, D_8012CEE0[41]);
                    func_8006376C(0x13, 7, 5, D_8012CF60[dbCamera->sub.demoCtrlMenu / 100]);
                    func_8006376C(0x17, 7, 5, D_8012CFA4);
                    func_8006376C(0xD, 9, (dbCamera->sub.demoCtrlToggleSwitch != 0) ? 1 : 6, "PRESS B BUTTON");

                    if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_A) ||
                        CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_B)) {
                        Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                        if (dbCamera->sub.demoCtrlMenu == DEMO_CTRL_MENU(ACTION_LOAD, MENU_SUCCESS)) {
                            dbCamera->sub.demoCtrlActionIdx = ACTION_E;
                        }
                        dbCamera->sub.demoCtrlMenu = DEMO_CTRL_MENU(ACTION_E, MENU_INFO);
                        return 1;
                    }
                    goto block_2;
                }

                case DEMO_CTRL_MENU(ACTION_SAVE, MENU_ERROR):
                case DEMO_CTRL_MENU(ACTION_LOAD, MENU_ERROR):
                case DEMO_CTRL_MENU(ACTION_CLEAR, MENU_ERROR): {
                    dbCamera->sub.demoCtrlToggleSwitch ^= 1;
                    D_8012CEE0[41][9] = sCurFileIdx + 'A';
                    func_8006376C(0xD, 7, 5, D_8012CEE0[(dbCamera->sub.demoCtrlMenu / 100) + 32]);
                    func_8006376C(0x11, 7, 5, D_8012CFAC);
                    func_8006376C(0x17, 7, 5, D_8012CFA4);
                    func_8006376C(0xD, 9, (dbCamera->sub.demoCtrlToggleSwitch != 0) ? 1 : 6, "PRESS B BUTTON");

                    if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_A) ||
                        CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_B)) {
                        Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                        dbCamera->sub.demoCtrlMenu -= 9;
                    }
                block_2:
                    return 1;
                }

                case 1:
                    goto block_1;

                default: {
                    if (Mempak_Init(2)) {
                        sMempakFiles = Mempak_FindFile(2, 'A', 'E');
                        dbCamera->sub.demoCtrlMenu = DEMO_CTRL_MENU(ACTION_E, MENU_CALLBACK);
                        DbCamera_CalcMempakAllocSize();
                        if ((1 << sCurFileIdx) & sMempakFiles) {
                            sMempakFilesize = Mempak_GetFileSize(2, sCurFileIdx + 'A');
                            dbCamera->sub.demoCtrlActionIdx = ACTION_LOAD;
                        } else {
                            sMempakFilesize = 0;
                            dbCamera->sub.demoCtrlActionIdx = ACTION_SAVE;
                        }
                    block_1:
                        idx2 = 1;
                        for (i = 0; i < 5; i++) {
                            sp74[i * 2 + 1] = (sMempakFiles & idx2) ? i + 'A' : '?';
                            sp74[i * 2 + 0] = '-';

                            idx2 <<= 1;
                        }
                        sp74[i * 2 + 0] = '-';
                        sp74[i * 2 + 1] = '\0';

                        if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DRIGHT)) {
                            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                            if (sCurFileIdx >= 4) {
                                sCurFileIdx = 0;
                            } else {
                                sCurFileIdx++;
                            }

                            if ((1 << sCurFileIdx) & sMempakFiles) {
                                sMempakFilesize = Mempak_GetFileSize(2, sCurFileIdx + 'A');
                                dbCamera->sub.demoCtrlActionIdx = ACTION_LOAD;
                            } else {
                                sMempakFilesize = 0;
                                dbCamera->sub.demoCtrlActionIdx = ACTION_SAVE;
                            }
                        }
                        if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DLEFT)) {
                            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                            if (sCurFileIdx <= 0) {
                                sCurFileIdx = 4;
                            } else {
                                sCurFileIdx--;
                            }

                            if ((1 << sCurFileIdx) & sMempakFiles) {
                                sMempakFilesize = Mempak_GetFileSize(2, sCurFileIdx + 'A');
                                dbCamera->sub.demoCtrlActionIdx = ACTION_LOAD;
                            } else {
                                sMempakFilesize = 0;
                                dbCamera->sub.demoCtrlActionIdx = ACTION_SAVE;
                            }
                        }
                        idx3 = dbCamera->sub.demoCtrlActionIdx;
                        func_8006376C(0xE, 7, 5, D_8012CF50[idx3]);
                        func_8006376C(0xF, 7, 4, sp74);

                        func_8006376C((sCurFileIdx * 2) + 0x10, 7, 7, "_"); // cursor
                        DbCamera_SetTextValue(DbCamera_GetMempakAllocSize(), sp74, 6);
                        func_8006376C(0xD, 9, 6, D_8012CF78); // NEED      BYTE
                        func_8006376C(0x11, 9, 4, sp74);
                        DbCamera_SetTextValue(Mempak_GetFreeBytes(2), sp74, 6);
                        func_8006376C(0xD, 0xA, 6, D_8012CF74); // FREE      BYTE
                        func_8006376C(0x11, 0xA, 4, sp74);
                        if (sMempakFilesize != 0) {
                            DbCamera_SetTextValue(sMempakFilesize, sp74, 6);
                            func_8006376C(0xD, 0xB, 7, D_8012CFA8);
                            func_8006376C(0x11, 0xB, 4, sp74);
                        }

                        idx1 = (dbCamera->sub.demoCtrlActionIdx + 2);
                        func_8006376C(0xF, 0x16, 1, D_8012CF7C);
                        func_8006376C(0x12, 0x17, sDbCameraColors[idx1], D_8012CF64);
                        func_8006376C(0x12, 0x18, sDbCameraColors[idx1 - 1], D_8012CF68);
                        func_8006376C(0x12, 0x19, sDbCameraColors[idx1 - 2], D_8012CF6C);
                        func_8006376C(0xE, dbCamera->sub.demoCtrlActionIdx + 0x16, 7, D_8012CF0C); // current selection
                        func_8006376C(0xD, 0x1A, 5, D_8012CF60[0]);
                        func_8006376C(0x14, 0x1A, 5, D_8012CF70);

                        if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DUP)) {
                            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                            dbCamera->sub.demoCtrlActionIdx = (dbCamera->sub.demoCtrlActionIdx - 1) % 4u;
                        }
                        if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DDOWN)) {
                            Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                            dbCamera->sub.demoCtrlActionIdx = (dbCamera->sub.demoCtrlActionIdx + 1) % 4u;
                        }

                        if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_A)) {
                            Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                            dbCamera->sub.demoCtrlToggleSwitch = 0;
                            dbCamera->sub.demoCtrlMenu = DEMO_CTRL_MENU(dbCamera->sub.demoCtrlActionIdx, MENU_INFO);
                        }
                        if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_B)) {
                            Audio_PlaySoundGeneral(NA_SE_SY_CANCEL, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                            dbCamera->sub.demoCtrlActionIdx = ACTION_E;
                            return 1;
                        }
                        goto block_2;
                    } else {
                        func_8006376C(0xC, 0x1A, 4, D_8012CF60[0]);
                        func_8006376C(0x13, 0x1A, 4, D_8012CF80);
                        if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_B) ||
                            CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DUP) ||
                            CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DDOWN)) {

                            Audio_PlaySoundGeneral(NA_SE_SY_CANCEL, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                            dbCamera->sub.demoCtrlActionIdx = ACTION_E;
                        }
                        return 2;
                    }
                    break;
                }
            }
            break;

        default: {
            if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DUP)) {
                Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                dbCamera->sub.demoCtrlMenu = DEMO_CTRL_MENU(ACTION_E, MENU_INFO);
                dbCamera->sub.demoCtrlActionIdx = (dbCamera->sub.demoCtrlActionIdx - 1) % 4u;
                sCurFileIdx = 0;
            }
            if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DDOWN)) {
                Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                dbCamera->sub.demoCtrlMenu = DEMO_CTRL_MENU(ACTION_E, MENU_INFO);
                dbCamera->sub.demoCtrlActionIdx = (dbCamera->sub.demoCtrlActionIdx + 1) % 4u;
                sCurFileIdx = 0;
            }

            DbCamera_DrawSlotLetters(sp74, 7, 5, 4);

            if (sDbCamAnim.unk_0A != 0) {
                func_8006376C(4, 7, 5, D_8012CF4C);
                func_8006376C(D_8016110C * 2 + 6, 7, 7, ">");

                if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CUP)) {
                    if (D_8016110C > 0) {
                        D_8016110C--;
                    }

                    sDbCamAnim.curFrame = 0.0f;
                    sDbCamAnim.keyframe = 0;
                    sDbCamAnim.unk_04 = 0;
                } else if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CDOWN)) {
                    if (D_8016110C < 14) {
                        D_8016110C++;
                    }

                    sDbCamAnim.curFrame = 0.0f;
                    sDbCamAnim.keyframe = 0;
                    sDbCamAnim.unk_04 = 0;
                } else if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CLEFT)) {
                    sDbCamAnim.unk_0A = 0;
                    Interface_ChangeAlpha(2);
                    ShrinkWindow_SetVal(0);
                    D_8016110C = 0;
                    return 2;
                }

                if (func_800B91B0(cam, dbCamera) == 0) {
                    Interface_ChangeAlpha(2);
                    ShrinkWindow_SetVal(0);
                    Audio_PlaySoundGeneral(NA_SE_SY_GET_RUPY, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                }
                OLib_Vec3fDiffToVecSphGeo(&sp5C, &dbCamera->eye, &dbCamera->at);
                DbCamera_CalcUpFromPitchYawRoll(&dbCamera->unk_1C, sp5C.pitch, sp5C.yaw,
                                                DEGF_TO_BINANG(dbCamera->rollDegrees));
                return 2;
            }

            if (CHECK_BTN_ALL(sPlayState->state.input[1].press.button, BTN_CRIGHT)) {
                D_8015FCC8 = 0;
                gSaveContext.cutsceneIndex = 0xFFFD;
                gSaveContext.cutsceneTrigger = 1;
                sDbCamAnim.curFrame = 0.0f;
                sDbCamAnim.keyframe = 0;
                sDbCamAnim.unk_04 = 0;
                sDbCamAnim.unk_0A = 1;
                sDbCamAnim.unk_0C = 0;
                D_8016110C = 0;
                Audio_PlaySoundGeneral(NA_SE_SY_HP_RECOVER, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }

            if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_L)) {
                if (sp74[sCurFileIdx] == '?') {
                    sLastFileIdx = -1;
                    D_801612EA = '*';
                } else {
                    sLastFileIdx = sCurFileIdx;
                    D_801612EA = sDbCameraCuts[idx1].letter;
                }
            } else if (!CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L)) {
                if (sLastFileIdx != -1) {
                    switch (sp74[sCurFileIdx]) {
                        case '?':
                            Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                            sDbCameraCuts[idx1] = sDbCameraCuts[idx2];
                            sp74[sCurFileIdx] = '?'; // useless
                            DbCamera_ResetCut(idx2, false);
                            break;
                        case '-':
                            Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);

                            sp64 = sDbCameraCuts[idx2];
                            if (sLastFileIdx < sCurFileIdx) {
                                // rotate right
                                for (i = idx2; i < idx1 - 1 && i < ARRAY_COUNT(sDbCameraCuts) - 1; i++) {
                                    sDbCameraCuts[i] = sDbCameraCuts[i + 1];
                                }
                                sDbCameraCuts[idx1 - 1] = sp64;
                            } else if (sCurFileIdx < sLastFileIdx) {
                                // rotate left
                                for (i = idx2; idx1 < i && i > 0; i--) {
                                    sDbCameraCuts[i] = sDbCameraCuts[i - 1];
                                }
                                sDbCameraCuts[idx1] = sp64;
                            }

                            for (i = 0; i < ARRAY_COUNT(sDbCameraCuts) - 1; i++) {
                                sp74[i * 2 + 1] = sDbCameraCuts[i].letter;
                            }
                            break;
                        default:
                            Audio_PlaySoundGeneral(NA_SE_SY_ERROR, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                                   &D_801333E8);
                            break;
                    }
                }
                sLastFileIdx = -1;
            }

            if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_A)) {
                if (sp74[sCurFileIdx] == '?') {
                    Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    sp74[sCurFileIdx] = DbCamera_InitCut(idx1, &dbCamera->sub);
                    if (sp74[sCurFileIdx] == '?') {
                        func_8006376C(0xF, 0x18, 7, D_8012CF48);
                    }
                }
            }

            if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_B)) {
                if (sp74[sCurFileIdx] != '?' && sp74[sCurFileIdx] != '-') {
                    Audio_PlaySoundGeneral(NA_SE_SY_CANCEL, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                    sp74[sCurFileIdx] = '?';
                    DbCamera_ResetCut(idx1, true);
                }
            }

            if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_R)) {
                if (sp74[sCurFileIdx] != '?' && sp74[sCurFileIdx] != '-') {
                    Audio_PlaySoundGeneral(NA_SE_SY_DECIDE, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);

                    for (i = 0; i < sDbCameraCuts[idx1].nPoints; i++) {
                        dbCamera->sub.lookAt[i] = sDbCameraCuts[idx1].lookAt[i];
                    }
                    // why use another loop for that...
                    for (i = 0; i < sDbCameraCuts[idx1].nPoints; i++) {
                        dbCamera->sub.position[i] = sDbCameraCuts[idx1].position[i];
                    }

                    dbCamera->sub.mode = sDbCameraCuts[idx1].mode;
                    dbCamera->sub.nFrames = sDbCameraCuts[idx1].nFrames;
                    dbCamera->sub.nPoints = sDbCameraCuts[idx1].nPoints;
                    dbCamera->sub.unkIdx = 0;
                    func_800B41DC(dbCamera, dbCamera->sub.unkIdx, cam);
                    sp74[sCurFileIdx] = '?';
                    DbCamera_ResetCut(idx1, true);
                    dbCamera->unk_00 = 1;
                }
            }

            if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DRIGHT)) {
                Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                if (sCurFileIdx == 0x1E) {
                    sCurFileIdx = 0;
                } else {
                    sCurFileIdx++;
                }
            }
            if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_DLEFT)) {
                Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                sCurFileIdx = (sCurFileIdx == 0) ? 0x1E : sCurFileIdx - 1;
            }

            if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L) &&
                CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CRIGHT)) {
                for (i = 0; i < ARRAY_COUNT(sDbCameraCuts) - 1; i++) {
                    osSyncPrintf("###%2d:(%c) (%d %d) %d %d %d\n", i, sDbCameraCuts[i].letter,
                                 sDbCameraCuts[i].position, sDbCameraCuts[i].lookAt, sDbCameraCuts[i].nFrames,
                                 sDbCameraCuts[i].nPoints, sDbCameraCuts[i].mode);
                }
                DbCamera_PrintAllCuts(cam);
            } else if (CHECK_BTN_ALL(sPlayState->state.input[2].cur.button, BTN_L) &&
                       CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CLEFT)) {
                Audio_PlaySoundGeneral(NA_SE_SY_GET_RUPY, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                for (i = 0; i < ARRAY_COUNT(sDbCameraCuts) - 1; i++) {
                    if (sDbCameraCuts[i].nPoints != 0) {
                        osSyncPrintf("\n@@@ /* CUT [%d]\t*/", i);
                        DbCamera_PrintCutBytes(&sDbCameraCuts[i]);
                    }
                }
            } else if (CHECK_BTN_ALL(sPlayState->state.input[2].press.button, BTN_CRIGHT)) {
                sDbCamAnim.curFrame = 0.0f;
                sDbCamAnim.keyframe = 0;
                sDbCamAnim.unk_04 = 0.0f;
                sDbCamAnim.unk_0A = 1;
                sDbCamAnim.unk_0C = 0;
                Interface_ChangeAlpha(50);
                ShrinkWindow_SetVal(0x20);
                D_8016110C = 0;
                Audio_PlaySoundGeneral(NA_SE_SY_HP_RECOVER, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            }

            func_8006376C(4, 7, 5, D_8012CF50[0]);
            sp74[1] = 0;
            if (sLastFileIdx != -1) {
                sp74[0] = D_801612EA;
                func_8006376C(sLastFileIdx + 5, 7, 2, sp74);
            } else {
                sp74[0] = '_';
            }
            func_8006376C(sCurFileIdx + 5, 7, 7, sp74);

            break;
        }
    }

    return 1;
}

void func_800BB03C(Camera* cam) {
    func_800B91B0(cam, sDbCamPtr);
}

void func_800BB060(void) {
    sDbCamAnim.unk_0A = 0;
}

s32 func_800BB06C(void) {
    return sDbCamPtr->unk_00 == 2 && sDbCamAnim.unk_0A != 0;
}
