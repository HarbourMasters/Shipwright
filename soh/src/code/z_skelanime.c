#include "global.h"
#include "vt.h"

#define ANIM_INTERP 1

s32 LinkAnimation_Loop(GlobalContext* globalCtx, SkelAnime* skelAnime);
s32 LinkAnimation_Once(GlobalContext* globalCtx, SkelAnime* skelAnime);
s32 SkelAnime_LoopFull(SkelAnime* skelAnime);
s32 SkelAnime_Once(SkelAnime* skelAnime);
s32 SkelAnime_LoopPartial(SkelAnime* skelAnime);

void SkelAnime_CopyFrameTable(SkelAnime* skelAnime, Vec3s* dst, Vec3s* src);

static u32 sDisableAnimQueueFlags = 0;
static u32 sAnimQueueFlags;

/**
 * Draw a limb of type `LodLimb`
 * Near or far display list is specified via `lod`
 */
void SkelAnime_DrawLimbLod(GlobalContext* globalCtx, s32 limbIndex, void** skeleton, Vec3s* jointTable,
                           OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, void* arg, s32 lod) {
    LodLimb* limb;
    Gfx* dList;
    Vec3f pos;
    Vec3s rot;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    Matrix_Push();
    limb = (LodLimb*)SEGMENTED_TO_VIRTUAL(skeleton[limbIndex]);
    limbIndex++;
    rot = jointTable[limbIndex];

    pos.x = limb->jointPos.x;
    pos.y = limb->jointPos.y;
    pos.z = limb->jointPos.z;

    dList = limb->dLists[lod];

    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(globalCtx, limbIndex, &dList, &pos, &rot, arg)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        if (dList != NULL) {
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx), G_MTX_LOAD);
            gSPDisplayList(POLY_OPA_DISP++, dList);
        }
    }

    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, limbIndex, &dList, &rot, arg);
    }

    if (limb->child != LIMB_DONE) {
        SkelAnime_DrawLimbLod(globalCtx, limb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg, lod);
    }

    Matrix_Pop();

    if (limb->sibling != LIMB_DONE) {
        SkelAnime_DrawLimbLod(globalCtx, limb->sibling, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg, lod);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

/**
 * Draw all limbs of type `LodLimb` in a given skeleton
 * Near or far display list is specified via `lod`
 */
void SkelAnime_DrawLod(GlobalContext* globalCtx, void** skeleton, Vec3s* jointTable,
                       OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, void* arg, s32 lod) {
    LodLimb* rootLimb;
    s32 pad;
    Gfx* dList;
    Vec3f pos;
    Vec3s rot;

    if (skeleton == NULL) {
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("Si2_Lod_draw():skelがNULLです。\n"); // "skel is NULL."
        osSyncPrintf(VT_RST);
        return;
    }

    OPEN_DISPS(globalCtx->state.gfxCtx);

    Matrix_Push();

    rootLimb = (LodLimb*)SEGMENTED_TO_VIRTUAL(skeleton[0]);
    pos.x = jointTable[0].x;
    pos.y = jointTable[0].y;
    pos.z = jointTable[0].z;

    rot = jointTable[1];
    dList = rootLimb->dLists[lod];

    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(globalCtx, 1, &dList, &pos, &rot, arg)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        if (dList != NULL) {
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx), G_MTX_LOAD);
            gSPDisplayList(POLY_OPA_DISP++, dList);
        }
    }
    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, 1, &dList, &rot, arg);
    }

    if (rootLimb->child != LIMB_DONE) {
        SkelAnime_DrawLimbLod(globalCtx, rootLimb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg,
                              lod);
    }

    Matrix_Pop();

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

/**
 * Draw a limb of type `LodLimb` contained within a flexible skeleton
 * Near or far display list is specified via `lod`
 */
void SkelAnime_DrawFlexLimbLod(GlobalContext* globalCtx, s32 limbIndex, void** skeleton, Vec3s* jointTable,
                               OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, void* arg, s32 lod,
                               Mtx** mtx) {
    LodLimb* limb;
    Gfx* newDList;
    Gfx* limbDList;
    Vec3f pos;
    Vec3s rot;

    Matrix_Push();

    limb = (LodLimb*)SEGMENTED_TO_VIRTUAL(skeleton[limbIndex]);
    limbIndex++;

    rot = jointTable[limbIndex];

    pos.x = limb->jointPos.x;
    pos.y = limb->jointPos.y;
    pos.z = limb->jointPos.z;

    newDList = limbDList = limb->dLists[lod];

    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(globalCtx, limbIndex, &newDList, &pos, &rot, arg)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        if (newDList != NULL) {
            MATRIX_TOMTX(*mtx);
            {
                OPEN_DISPS(globalCtx->state.gfxCtx);
                gSPMatrix(POLY_OPA_DISP++, *mtx, G_MTX_LOAD);
                gSPDisplayList(POLY_OPA_DISP++, newDList);
                CLOSE_DISPS(globalCtx->state.gfxCtx);
            }
            (*mtx)++;
        } else if (limbDList != NULL) {
            MATRIX_TOMTX(*mtx);
            (*mtx)++;
        }
    }
    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, limbIndex, &limbDList, &rot, arg);
    }
    if (limb->child != LIMB_DONE) {
        SkelAnime_DrawFlexLimbLod(globalCtx, limb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg,
                                  lod, mtx);
    }

    Matrix_Pop();

    if (limb->sibling != LIMB_DONE) {
        SkelAnime_DrawFlexLimbLod(globalCtx, limb->sibling, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg,
                                  lod, mtx);
    }
}

/**
 * Draws all limbs of type `LodLimb` in a given flexible skeleton
 * Limbs in a flexible skeleton have meshes that can stretch to line up with other limbs.
 * An array of matrices is dynamically allocated so each limb can access any transform to ensure its meshes line up.
 */
void SkelAnime_DrawFlexLod(GlobalContext* globalCtx, void** skeleton, Vec3s* jointTable, s32 dListCount,
                           OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, void* arg, s32 lod) {
    LodLimb* rootLimb;
    s32 pad;
    Gfx* newDList;
    Gfx* limbDList;
    Vec3f pos;
    Vec3s rot;
    Mtx* mtx = Graph_Alloc(globalCtx->state.gfxCtx, dListCount * sizeof(Mtx));

    if (skeleton == NULL) {
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("Si2_Lod_draw_SV():skelがNULLです。\n"); // "skel is NULL."
        osSyncPrintf(VT_RST);
        return;
    }

    OPEN_DISPS(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0xD, mtx);
    Matrix_Push();

    rootLimb = (LodLimb*)SEGMENTED_TO_VIRTUAL(skeleton[0]);
    pos.x = jointTable[0].x;
    pos.y = jointTable[0].y;
    pos.z = jointTable[0].z;

    rot = jointTable[1];

    newDList = limbDList = rootLimb->dLists[lod];

    if ((overrideLimbDraw == 0) || !overrideLimbDraw(globalCtx, 1, &newDList, &pos, &rot, arg)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        if (newDList != NULL) {
            MATRIX_TOMTX(mtx);
            gDPNoOpString(POLY_OPA_DISP++, "T5ST", 0);
            gSPMatrix(POLY_OPA_DISP++, mtx, G_MTX_LOAD);
            gSPDisplayList(POLY_OPA_DISP++, newDList);
            mtx++;
        } else if (limbDList != NULL) {
            MATRIX_TOMTX(mtx);
            mtx++;
        }
    }

    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, 1, &limbDList, &rot, arg);
    }
    if (rootLimb->child != LIMB_DONE) {
        SkelAnime_DrawFlexLimbLod(globalCtx, rootLimb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg,
                                  lod, &mtx);
    }

    Matrix_Pop();

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

/**
 * Draw a limb of type `StandardLimb` to the polyOpa buffer
 */
void SkelAnime_DrawLimbOpa(GlobalContext* globalCtx, s32 limbIndex, void** skeleton, Vec3s* jointTable,
                           OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, void* arg) {
    StandardLimb* limb;
    Gfx* dList;
    Vec3f pos;
    Vec3s rot;

    OPEN_DISPS(globalCtx->state.gfxCtx);
    Matrix_Push();

    limb = (StandardLimb*)SEGMENTED_TO_VIRTUAL(skeleton[limbIndex]);
    limbIndex++;
    rot = jointTable[limbIndex];
    pos.x = limb->jointPos.x;
    pos.y = limb->jointPos.y;
    pos.z = limb->jointPos.z;
    dList = limb->dList;

    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(globalCtx, limbIndex, &dList, &pos, &rot, arg)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        if (dList != NULL) {
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx), G_MTX_LOAD);
            gSPDisplayList(POLY_OPA_DISP++, dList);
        }
    }

    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, limbIndex, &dList, &rot, arg);
    }

    if (limb->child != LIMB_DONE) {
        SkelAnime_DrawLimbOpa(globalCtx, limb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg);
    }

    Matrix_Pop();

    if (limb->sibling != LIMB_DONE) {
        SkelAnime_DrawLimbOpa(globalCtx, limb->sibling, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

/**
 * Draw all limbs of type `StandardLimb` in a given skeleton to the polyOpa buffer
 */
void SkelAnime_DrawOpa(GlobalContext* globalCtx, void** skeleton, Vec3s* jointTable,
                       OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, void* arg) {
    StandardLimb* rootLimb;
    s32 pad;
    Gfx* dList;
    Vec3f pos;
    Vec3s rot;

    if (skeleton == NULL) {
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("Si2_draw():skelがNULLです。\n"); // "skel is NULL."
        osSyncPrintf(VT_RST);
        return;
    }

    OPEN_DISPS(globalCtx->state.gfxCtx);

    Matrix_Push();
    rootLimb = (StandardLimb*)SEGMENTED_TO_VIRTUAL(skeleton[0]);

    pos.x = jointTable[0].x;
    pos.y = jointTable[0].y;
    pos.z = jointTable[0].z;

    rot = jointTable[1];
    dList = rootLimb->dList;

    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(globalCtx, 1, &dList, &pos, &rot, arg)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        if (dList != NULL) {
            gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(globalCtx->state.gfxCtx), G_MTX_LOAD);
            gSPDisplayList(POLY_OPA_DISP++, dList);
        }
    }

    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, 1, &dList, &rot, arg);
    }

    if (rootLimb->child != LIMB_DONE) {
        SkelAnime_DrawLimbOpa(globalCtx, rootLimb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg);
    }

    Matrix_Pop();

    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

/**
 * Draw a limb of type `StandardLimb` contained within a flexible skeleton to the polyOpa buffer
 */
void SkelAnime_DrawFlexLimbOpa(GlobalContext* globalCtx, s32 limbIndex, void** skeleton, Vec3s* jointTable,
                               OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, void* arg,
                               Mtx** limbMatricies) {
    StandardLimb* limb;
    Gfx* newDList;
    Gfx* limbDList;
    Vec3f pos;
    Vec3s rot;

    OPEN_DISPS(globalCtx->state.gfxCtx);

    Matrix_Push();

    limb = (StandardLimb*)SEGMENTED_TO_VIRTUAL(skeleton[limbIndex]);
    limbIndex++;
    rot = jointTable[limbIndex];

    pos.x = limb->jointPos.x;
    pos.y = limb->jointPos.y;
    pos.z = limb->jointPos.z;

    newDList = limbDList = limb->dList;

    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(globalCtx, limbIndex, &newDList, &pos, &rot, arg)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        if (newDList != NULL) {
            MATRIX_TOMTX(*limbMatricies);
            gSPMatrix(POLY_OPA_DISP++, *limbMatricies, G_MTX_LOAD);
            gSPDisplayList(POLY_OPA_DISP++, newDList);
            (*limbMatricies)++;
        } else if (limbDList != NULL) {
            MATRIX_TOMTX(*limbMatricies);
            (*limbMatricies)++;
        }
    }

    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, limbIndex, &limbDList, &rot, arg);
    }

    if (limb->child != LIMB_DONE) {
        SkelAnime_DrawFlexLimbOpa(globalCtx, limb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg,
                                  limbMatricies);
    }

    Matrix_Pop();

    if (limb->sibling != LIMB_DONE) {
        SkelAnime_DrawFlexLimbOpa(globalCtx, limb->sibling, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg,
                                  limbMatricies);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

/**
 * Draw all limbs of type `StandardLimb` in a given flexible skeleton to the polyOpa buffer
 * Limbs in a flexible skeleton have meshes that can stretch to line up with other limbs.
 * An array of matrices is dynamically allocated so each limb can access any transform to ensure its meshes line up.
 */
void SkelAnime_DrawFlexOpa(GlobalContext* globalCtx, void** skeleton, Vec3s* jointTable, s32 dListCount,
                           OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, void* arg) {
    StandardLimb* rootLimb;
    s32 pad;
    Gfx* newDList;
    Gfx* limbDList;
    Vec3f pos;
    Vec3s rot;
    Mtx* mtx = Graph_Alloc(globalCtx->state.gfxCtx, dListCount * sizeof(Mtx));

    if (skeleton == NULL) {
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("Si2_draw_SV():skelがNULLです。\n"); // "skel is NULL."
        osSyncPrintf(VT_RST);
        return;
    }

    OPEN_DISPS(globalCtx->state.gfxCtx);

    gSPSegment(POLY_OPA_DISP++, 0xD, mtx);

    Matrix_Push();

    rootLimb = SEGMENTED_TO_VIRTUAL(skeleton[0]);

    pos.x = jointTable[0].x;
    pos.y = jointTable[0].y;
    pos.z = jointTable[0].z;

    rot = jointTable[1];

    newDList = limbDList = rootLimb->dList;

    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(globalCtx, 1, &newDList, &pos, &rot, arg)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        if (newDList != NULL) {
            MATRIX_TOMTX(mtx);
            gSPMatrix(POLY_OPA_DISP++, mtx, G_MTX_LOAD);
            gSPDisplayList(POLY_OPA_DISP++, newDList);
            mtx++;
        } else if (limbDList != NULL) {
            MATRIX_TOMTX(mtx);
            mtx++;
        }
    }

    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, 1, &limbDList, &rot, arg);
    }

    if (rootLimb->child != LIMB_DONE) {
        SkelAnime_DrawFlexLimbOpa(globalCtx, rootLimb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg,
                                  &mtx);
    }

    Matrix_Pop();
    CLOSE_DISPS(globalCtx->state.gfxCtx);
}

/**
 * Copies frame data from the frame data table, indexed by the joint index table.
 * Indices below limit are copied from that entry in the static frame data table.
 * Indices above limit are offsets to a frame data array indexed by the frame.
 */
void SkelAnime_GetFrameData(AnimationHeader* animation, s32 frame, s32 limbCount, Vec3s* frameTable) {
    if (ResourceMgr_OTRSigCheck(animation) != 0)
        animation = ResourceMgr_LoadAnimByName(animation);

    AnimationHeader* animHeader = SEGMENTED_TO_VIRTUAL(animation);
    JointIndex* jointIndices = SEGMENTED_TO_VIRTUAL(animHeader->jointIndices);
    s16* frameData = SEGMENTED_TO_VIRTUAL(animHeader->frameData);
    s16* staticData = &frameData[0];
    s16* dynamicData = &frameData[frame];
    u16 staticIndexMax = animHeader->staticIndexMax;
    s32 i;

    for (i = 0; i < limbCount; i++, frameTable++, jointIndices++) {
        if ((frameTable == NULL) || (jointIndices == NULL) || (dynamicData == NULL) || (staticData == NULL)) {
            LOG_ADDRESS("out", frameTable);
            LOG_ADDRESS("ref_tbl", jointIndices);
            LOG_ADDRESS("frame_tbl", dynamicData);
            LOG_ADDRESS("tbl", staticData);
        }

        frameTable->x =
            (jointIndices->x >= staticIndexMax) ? dynamicData[jointIndices->x] : staticData[jointIndices->x];
        frameTable->y =
            (jointIndices->y >= staticIndexMax) ? dynamicData[jointIndices->y] : staticData[jointIndices->y];
        frameTable->z =
            (jointIndices->z >= staticIndexMax) ? dynamicData[jointIndices->z] : staticData[jointIndices->z];
    }
}

s16 Animation_GetLength(void* animation) {
    if (ResourceMgr_OTRSigCheck(animation) != 0)
        animation = ResourceMgr_LoadAnimByName(animation);

    AnimationHeaderCommon* common = SEGMENTED_TO_VIRTUAL(animation);

    return common->frameCount;
}

s16 Animation_GetLastFrame(void* animation) {
    if (ResourceMgr_OTRSigCheck(animation) != 0)
        animation = ResourceMgr_LoadAnimByName(animation);
    AnimationHeaderCommon* common = SEGMENTED_TO_VIRTUAL(animation);
    // Loads an unsigned half for some reason.
    return (u16)common->frameCount - 1;
}

/**
 * Draw a limb of type `StandardLimb` to the specified display buffer
 */
Gfx* SkelAnime_DrawLimb(GlobalContext* globalCtx, s32 limbIndex, void** skeleton, Vec3s* jointTable,
                        OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw, void* arg, Gfx* gfx) {
    StandardLimb* limb;
    Gfx* dList;
    Vec3f pos;
    Vec3s rot;

    Matrix_Push();

    limb = (StandardLimb*)SEGMENTED_TO_VIRTUAL(skeleton[limbIndex]);
    limbIndex++;

    rot = jointTable[limbIndex];

    pos.x = limb->jointPos.x;
    pos.y = limb->jointPos.y;
    pos.z = limb->jointPos.z;

    dList = limb->dList;

    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(globalCtx, limbIndex, &dList, &pos, &rot, arg, &gfx)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        if (dList != NULL) {
            gSPMatrix(gfx++, MATRIX_NEWMTX(globalCtx->state.gfxCtx), G_MTX_LOAD);
            gSPDisplayList(gfx++, dList);
        }
    }

    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, limbIndex, &dList, &rot, arg, &gfx);
    }

    if (limb->child != LIMB_DONE) {
        gfx =
            SkelAnime_DrawLimb(globalCtx, limb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg, gfx);
    }

    Matrix_Pop();

    if (limb->sibling != LIMB_DONE) {
        gfx = SkelAnime_DrawLimb(globalCtx, limb->sibling, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg,
                                 gfx);
    }

    return gfx;
}

/**
 * Draw all limbs of type `StandardLimb` in a given skeleton to the specified display buffer
 */
Gfx* SkelAnime_Draw(GlobalContext* globalCtx, void** skeleton, Vec3s* jointTable, OverrideLimbDraw overrideLimbDraw,
                    PostLimbDraw postLimbDraw, void* arg, Gfx* gfx) {
    StandardLimb* rootLimb;
    s32 pad;
    Gfx* dList;
    Vec3f pos;
    Vec3s rot;

    if (skeleton == NULL) {
        osSyncPrintf(VT_FGCOL(RED));
        // "skel is NULL. Returns NULL."
        osSyncPrintf("Si2_draw2():skelがNULLです。NULLを返します。\n");
        osSyncPrintf(VT_RST);
        return NULL;
    }

    Matrix_Push();

    rootLimb = (StandardLimb*)SEGMENTED_TO_VIRTUAL(skeleton[0]);

    pos.x = jointTable[0].x;
    pos.y = jointTable[0].y;
    pos.z = jointTable[0].z;

    rot = jointTable[1];

    dList = rootLimb->dList;

    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(globalCtx, 1, &dList, &pos, &rot, arg, &gfx)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        if (dList != NULL) {
            gSPMatrix(gfx++, MATRIX_NEWMTX(globalCtx->state.gfxCtx), G_MTX_LOAD);
            gSPDisplayList(gfx++, dList);
        }
    }

    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, 1, &dList, &rot, arg, &gfx);
    }

    if (rootLimb->child != LIMB_DONE) {
        gfx = SkelAnime_DrawLimb(globalCtx, rootLimb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg,
                                 gfx);
    }

    Matrix_Pop();

    return gfx;
}

/**
 * Draw a limb of type `StandardLimb` contained within a flexible skeleton to the specified display buffer
 */
Gfx* SkelAnime_DrawFlexLimb(GlobalContext* globalCtx, s32 limbIndex, void** skeleton, Vec3s* jointTable,
                            OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw, void* arg, Mtx** mtx,
                            Gfx* gfx) {
    StandardLimb* limb;
    Gfx* newDList;
    Gfx* limbDList;
    Vec3f pos;
    Vec3s rot;

    Matrix_Push();

    limb = (StandardLimb*)SEGMENTED_TO_VIRTUAL(skeleton[limbIndex]);
    limbIndex++;
    rot = jointTable[limbIndex];

    pos.x = limb->jointPos.x;
    pos.y = limb->jointPos.y;
    pos.z = limb->jointPos.z;

    newDList = limbDList = limb->dList;
    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(globalCtx, limbIndex, &newDList, &pos, &rot, arg, &gfx)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        if (newDList != NULL) {
            MATRIX_TOMTX(*mtx);
            gSPMatrix(gfx++, *mtx, G_MTX_LOAD);
            gSPDisplayList(gfx++, newDList);
            (*mtx)++;
        } else if (limbDList != NULL) {
            MATRIX_TOMTX(*mtx);
            (*mtx)++;
        }
    }
    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, limbIndex, &limbDList, &rot, arg, &gfx);
    }
    if (limb->child != LIMB_DONE) {
        gfx = SkelAnime_DrawFlexLimb(globalCtx, limb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw, arg,
                                     mtx, gfx);
    }

    Matrix_Pop();

    if (limb->sibling != LIMB_DONE) {
        gfx = SkelAnime_DrawFlexLimb(globalCtx, limb->sibling, skeleton, jointTable, overrideLimbDraw, postLimbDraw,
                                     arg, mtx, gfx);
    }

    return gfx;
}

/**
 * Draw all limbs of type `StandardLimb` in a given flexible skeleton to the specified display buffer
 * Limbs in a flexible skeleton have meshes that can stretch to line up with other limbs.
 * An array of matrices is dynamically allocated so each limb can access any transform to ensure its meshes line up.
 */
Gfx* SkelAnime_DrawFlex(GlobalContext* globalCtx, void** skeleton, Vec3s* jointTable, s32 dListCount,
                        OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw, void* arg, Gfx* gfx) {
    StandardLimb* rootLimb;
    s32 pad;
    Gfx* newDList;
    Gfx* limbDList;
    Vec3f pos;
    Vec3s rot;
    Mtx* mtx = Graph_Alloc(globalCtx->state.gfxCtx, dListCount * sizeof(*mtx));

    if (skeleton == NULL) {
        osSyncPrintf(VT_FGCOL(RED));
        // "skel is NULL. Returns NULL."
        osSyncPrintf("Si2_draw2_SV():skelがNULLです。NULLを返します。\n");
        osSyncPrintf(VT_RST);
        return NULL;
    }

    gSPSegment(gfx++, 0xD, mtx);
    Matrix_Push();
    rootLimb = (StandardLimb*)SEGMENTED_TO_VIRTUAL(skeleton[0]);

    pos.x = jointTable[0].x;
    pos.y = jointTable[0].y;
    pos.z = jointTable[0].z;

    rot = jointTable[1];

    newDList = limbDList = rootLimb->dList;

    if ((overrideLimbDraw == NULL) || !overrideLimbDraw(globalCtx, 1, &newDList, &pos, &rot, arg, &gfx)) {
        Matrix_TranslateRotateZYX(&pos, &rot);
        if (newDList != NULL) {
            MATRIX_TOMTX(mtx);
            gSPMatrix(gfx++, mtx, G_MTX_LOAD);
            gSPDisplayList(gfx++, newDList);
            mtx++;
        } else if (limbDList != NULL) {
            MATRIX_TOMTX(mtx);
            mtx++;
        }
    }
    if (postLimbDraw != NULL) {
        postLimbDraw(globalCtx, 1, &limbDList, &rot, arg, &gfx);
    }
    if (rootLimb->child != LIMB_DONE) {
        gfx = SkelAnime_DrawFlexLimb(globalCtx, rootLimb->child, skeleton, jointTable, overrideLimbDraw, postLimbDraw,
                                     arg, &mtx, gfx);
    }

    Matrix_Pop();

    return gfx;
}

/**
 * Unpacks frame data for the animation at the given frame into frameTable
 * Used by the legacy animation format
 */
s32 SkelAnime_GetFrameDataLegacy(LegacyAnimationHeader* animation, s32 frame, Vec3s* frameTable) {
    LegacyAnimationHeader* animHeader = SEGMENTED_TO_VIRTUAL(animation);
    s32 limbCount = animHeader->limbCount;
    JointKey* key = SEGMENTED_TO_VIRTUAL(animHeader->jointKey);
    s16* frameData = SEGMENTED_TO_VIRTUAL(animHeader->frameData);
    s16* staticData = &frameData[0];
    s16* dynamicData = &frameData[frame];
    s32 i;

    frameTable->x = frame < key->xMax ? dynamicData[key->x] : staticData[key->x];
    frameTable->y = frame < key->yMax ? dynamicData[key->y] : staticData[key->y];
    frameTable->z = frame < key->zMax ? dynamicData[key->z] : staticData[key->z];

    frameTable++;
    key++;

    for (i = 1; i <= limbCount; i++, key++, frameTable++) {
        frameTable->x = frame < key->xMax ? dynamicData[key->x] : staticData[key->x];
        frameTable->y = frame < key->yMax ? dynamicData[key->y] : staticData[key->y];
        frameTable->z = frame < key->zMax ? dynamicData[key->z] : staticData[key->z];
    }

    return limbCount;
}

/**
 * Used by legacy animation format
 */
s16 Animation_GetLimbCountLegacy(LegacyAnimationHeader* animation) {
    LegacyAnimationHeader* animHeader = SEGMENTED_TO_VIRTUAL(animation);

    return animHeader->limbCount;
}

/**
 * Used by legacy animation format
 */
s16 Animation_GetLengthLegacy(LegacyAnimationHeader* animation) {
    LegacyAnimationHeader* animHeader = SEGMENTED_TO_VIRTUAL(animation);

    return animHeader->frameCount;
}

/**
 * Used by legacy animation format
 */
s16 Animation_GetLastFrameLegacy(LegacyAnimationHeader* animation) {
    LegacyAnimationHeader* animHeader = SEGMENTED_TO_VIRTUAL(animation);

    return animHeader->frameCount - 1;
}

/**
 * Linearly interpolates the start and target frame tables with the given weight, putting the result in dst
 */
void SkelAnime_InterpFrameTable(s32 limbCount, Vec3s* dst, Vec3s* start, Vec3s* target, f32 weight) {
    s32 i;
    s16 diff;
    s16 base;

    if (weight < 1.0f) {
        for (i = 0; i < limbCount; i++, dst++, start++, target++) {
            base = start->x;
            diff = target->x - base;
            dst->x = (s16)(diff * weight) + base;
            base = start->y;
            diff = target->y - base;
            dst->y = (s16)(diff * weight) + base;
            base = start->z;
            diff = target->z - base;
            dst->z = (s16)(diff * weight) + base;
        }
    } else {
        for (i = 0; i < limbCount; i++, dst++, target++) {
            dst->x = target->x;
            dst->y = target->y;
            dst->z = target->z;
        }
    }
}

/**
 * zeroes out the current request count
 */
void AnimationContext_Reset(AnimationContext* animationCtx) {
    animationCtx->animationCount = 0;
}

/**
 * Shifts the queue flag to the next queue
 */
void AnimationContext_SetNextQueue(GlobalContext* globalCtx) {
    sAnimQueueFlags <<= 1;
}

/**
 * Disables the current animation queue. Only load and move actor requests will be processed for that queue.
 */
void AnimationContext_DisableQueue(GlobalContext* globalCtx) {
    sDisableAnimQueueFlags |= sAnimQueueFlags;
}

AnimationEntry* AnimationContext_AddEntry(AnimationContext* animationCtx, AnimationType type) {
    AnimationEntry* entry;
    s16 index = animationCtx->animationCount;

    if (index >= ANIMATION_ENTRY_MAX) {
        return NULL;
    }

    animationCtx->animationCount = index + 1;
    entry = &animationCtx->entries[index];
    entry->type = type;
    return entry;
}

/**
 * Requests loading frame data from the Link animation into frameTable
 */
void AnimationContext_SetLoadFrame(GlobalContext* globalCtx, LinkAnimationHeader* animation, s32 frame, s32 limbCount,
                                   Vec3s* frameTable) {
    if (CVar_GetS32("gN64WeirdFrames", 0) && frame < 0) {
        Vec3s* src = (Vec3s*)getN64WeirdFrame((sizeof(Vec3s) * limbCount + 2) * frame);
        memcpy(frameTable, src, sizeof(Vec3s) * limbCount + 2);
        return;
    }

    AnimationEntry* entry = AnimationContext_AddEntry(&globalCtx->animationCtx, ANIMENTRY_LOADFRAME);

    if (entry != NULL)
    {
        if (ResourceMgr_OTRSigCheck(animation) != 0)
            animation = ResourceMgr_LoadAnimByName(animation);

        LinkAnimationHeader* linkAnimHeader = SEGMENTED_TO_VIRTUAL(animation);
        Vec3s* ram = frameTable;

        osCreateMesgQueue(&entry->data.load.msgQueue, &entry->data.load.msg, 1);

        char animPath[2048];

        sprintf(animPath, "misc/link_animetion/gPlayerAnimData_%06X", (((uintptr_t)linkAnimHeader->segment - 0x07000000)));

        //printf("Streaming %s, seg = %08X\n", animPath, linkAnimHeader->segment);

        s16* animData = ResourceMgr_LoadPlayerAnimByName(animPath);

        memcpy(ram, (uintptr_t)animData + (((sizeof(Vec3s) * limbCount + 2) * frame)), sizeof(Vec3s) * limbCount + 2);

        /*u32* ramPtr = (u32*)ram;

        for (int i = 0; i < 1024; i++)
        {
            ramPtr[i] = i * 7;
        }*/


        //DmaMgr_SendRequest2(&entry->data.load.req, ram,
                            //LINK_ANIMATION_OFFSET(linkAnimHeader->segment, ((sizeof(Vec3s) * limbCount + 2) * frame)),
                            //sizeof(Vec3s) * limbCount + 2, 0, &entry->data.load.msgQueue, NULL, __FILE__,
                            //__LINE__);
    }
}

/**
 * Requests copying all vectors from src frame table into dst frame table
 */
void AnimationContext_SetCopyAll(GlobalContext* globalCtx, s32 vecCount, Vec3s* dst, Vec3s* src) {
    AnimationEntry* entry = AnimationContext_AddEntry(&globalCtx->animationCtx, ANIMENTRY_COPYALL);

    if (entry != NULL) {
        entry->data.copy.queueFlag = sAnimQueueFlags;
        entry->data.copy.vecCount = vecCount;
        entry->data.copy.dst = dst;
        entry->data.copy.src = src;
    }
}

/**
 * Requests interpolating between base and mod frame tables with the given weight, placing the result in base
 */
void AnimationContext_SetInterp(GlobalContext* globalCtx, s32 vecCount, Vec3s* base, Vec3s* mod, f32 weight) {
    AnimationEntry* entry = AnimationContext_AddEntry(&globalCtx->animationCtx, ANIMENTRY_INTERP);

    if (entry != NULL) {
        entry->data.interp.queueFlag = sAnimQueueFlags;
        entry->data.interp.vecCount = vecCount;
        entry->data.interp.base = base;
        entry->data.interp.mod = mod;
        entry->data.interp.weight = weight;
    }
}

/**
 * Requests copying vectors from src frame table to dst frame table whose copy flag is true
 */
void AnimationContext_SetCopyTrue(GlobalContext* globalCtx, s32 vecCount, Vec3s* dst, Vec3s* src, u8* copyFlag) {
    AnimationEntry* entry = AnimationContext_AddEntry(&globalCtx->animationCtx, ANIMENTRY_COPYTRUE);

    if (entry != NULL) {
        entry->data.copy1.queueFlag = sAnimQueueFlags;
        entry->data.copy1.vecCount = vecCount;
        entry->data.copy1.dst = dst;
        entry->data.copy1.src = src;
        entry->data.copy1.copyFlag = copyFlag;
    }
}

/**
 * Requests copying vectors from src frame table to dst frame table whose copy flag is false
 */
void AnimationContext_SetCopyFalse(GlobalContext* globalCtx, s32 vecCount, Vec3s* dst, Vec3s* src, u8* copyFlag) {
    AnimationEntry* entry = AnimationContext_AddEntry(&globalCtx->animationCtx, ANIMENTRY_COPYFALSE);

    if (entry != NULL) {
        entry->data.copy0.queueFlag = sAnimQueueFlags;
        entry->data.copy0.vecCount = vecCount;
        entry->data.copy0.dst = dst;
        entry->data.copy0.src = src;
        entry->data.copy0.copyFlag = copyFlag;
    }
}

/**
 * Requests moving an actor according to the translation of its root limb
 */
void AnimationContext_SetMoveActor(GlobalContext* globalCtx, Actor* actor, SkelAnime* skelAnime, f32 arg3) {
    AnimationEntry* entry = AnimationContext_AddEntry(&globalCtx->animationCtx, ANIMENTRY_MOVEACTOR);

    if (entry != NULL) {
        entry->data.move.actor = actor;
        entry->data.move.skelAnime = skelAnime;
        entry->data.move.unk_08 = arg3;
    }
}

/**
 * Receives the request for Link's animation frame data
 */
void AnimationContext_LoadFrame(GlobalContext* globalCtx, AnimationEntryData* data) {
    AnimEntryLoadFrame* entry = &data->load;

    osRecvMesg(&entry->msgQueue, NULL, OS_MESG_BLOCK);
}

/**
 * If the entry's queue is enabled, copies all vectors from src frame table to dst frame table
 */
void AnimationContext_CopyAll(GlobalContext* globalCtx, AnimationEntryData* data) {
    AnimEntryCopyAll* entry = &data->copy;

    if (!(entry->queueFlag & sDisableAnimQueueFlags)) {
        Vec3s* dst = entry->dst;
        Vec3s* src = entry->src;
        s32 i;

        for (i = 0; i < entry->vecCount; i++) {
            *dst++ = *src++;
        }
    }
}

/**
 * If the entry's queue is enabled, interpolates between the base and mod frame tables, placing the result in base
 */
void AnimationContext_Interp(GlobalContext* globalCtx, AnimationEntryData* data) {
    AnimEntryInterp* entry = &data->interp;

    if (!(entry->queueFlag & sDisableAnimQueueFlags)) {
        SkelAnime_InterpFrameTable(entry->vecCount, entry->base, entry->base, entry->mod, entry->weight);
    }
}

/**
 * If the entry's queue is enabled, copies all vectors from src frame table to dst frame table whose copy flag is true
 */
void AnimationContext_CopyTrue(GlobalContext* globalCtx, AnimationEntryData* data) {
    AnimEntryCopyTrue* entry = &data->copy1;

    if (!(entry->queueFlag & sDisableAnimQueueFlags)) {
        Vec3s* dst = entry->dst;
        Vec3s* src = entry->src;
        u8* copyFlag = entry->copyFlag;
        s32 i;

        for (i = 0; i < entry->vecCount; i++, dst++, src++) {
            if (*copyFlag++) {
                *dst = *src;
            }
        }
    }
}

/**
 * If the entry's queue is enabled, copies all vectors from src frame table to dst frame table whose copy flag is false
 */
void AnimationContext_CopyFalse(GlobalContext* globalCtx, AnimationEntryData* data) {
    AnimEntryCopyFalse* entry = &data->copy0;

    if (!(entry->queueFlag & sDisableAnimQueueFlags)) {
        Vec3s* dst = entry->dst;
        Vec3s* src = entry->src;
        u8* copyFlag = entry->copyFlag;
        s32 i;

        for (i = 0; i < entry->vecCount; i++, dst++, src++) {
            if (!(*copyFlag++)) {
                *dst = *src;
            }
        }
    }
}

/**
 * Moves an actor according to the translation of its root limb
 */
void AnimationContext_MoveActor(GlobalContext* globalCtx, AnimationEntryData* data) {
    AnimEntryMoveActor* entry = &data->move;
    Actor* actor = entry->actor;
    Vec3f diff;

    SkelAnime_UpdateTranslation(entry->skelAnime, &diff, actor->shape.rot.y);
    actor->world.pos.x += diff.x * actor->scale.x;
    actor->world.pos.y += diff.y * actor->scale.y * entry->unk_08;
    actor->world.pos.z += diff.z * actor->scale.z;
}

/**
 * Performs all requests in the animation queue, then resets the queue flags.
 */
void AnimationContext_Update(GlobalContext* globalCtx, AnimationContext* animationCtx) {
    static AnimationEntryCallback animFuncs[] = {
        AnimationContext_LoadFrame, AnimationContext_CopyAll,   AnimationContext_Interp,
        AnimationContext_CopyTrue,  AnimationContext_CopyFalse, AnimationContext_MoveActor,
    };
    AnimationEntry* entry;

    for (entry = animationCtx->entries; animationCtx->animationCount != 0; entry++, animationCtx->animationCount--) {
        animFuncs[entry->type](globalCtx, &entry->data);
    }

    sAnimQueueFlags = 1;
    sDisableAnimQueueFlags = 0;
}

/**
 * Initializes a skeleton to be used with Link animations to a looping animation, dynamically allocating the frame
 * tables if not given.
 */
void SkelAnime_InitLink(GlobalContext* globalCtx, SkelAnime* skelAnime, FlexSkeletonHeader* skeletonHeaderSeg,
                        LinkAnimationHeader* animation, s32 flags, Vec3s* jointTable, Vec3s* morphTable,
                        s32 limbBufCount) {
    if (ResourceMgr_OTRSigCheck(skeletonHeaderSeg) != 0)
        skeletonHeaderSeg = ResourceMgr_LoadSkeletonByName(skeletonHeaderSeg);

    FlexSkeletonHeader* skeletonHeader = SEGMENTED_TO_VIRTUAL(skeletonHeaderSeg);
    s32 headerJointCount = skeletonHeader->sh.limbCount;
    s32 limbCount;
    size_t allocSize;

    skelAnime->initFlags = flags;
    limbCount = (flags & 2) ? headerJointCount : 1;

    if (flags & 1) {
        limbCount += headerJointCount;
    }
    if (flags & 4) {
        limbCount += headerJointCount;
    }

    skelAnime->limbCount = limbCount;
    skelAnime->dListCount = skeletonHeader->dListCount;

    allocSize = limbCount * sizeof(Vec3s);
    skelAnime->skeleton = SEGMENTED_TO_VIRTUAL(skeletonHeader->sh.segment);

    if (flags & 8) {
        allocSize += 2;
    }

    if (jointTable == NULL) {
        skelAnime->jointTable = ZELDA_ARENA_MALLOC_DEBUG(allocSize);
        skelAnime->morphTable = ZELDA_ARENA_MALLOC_DEBUG(allocSize);
    } else {
        ASSERT(limbBufCount == limbCount);

        skelAnime->jointTable = (Vec3s*)ALIGN16((uintptr_t)jointTable);
        skelAnime->morphTable = (Vec3s*)ALIGN16((uintptr_t)morphTable);
    }

    if ((skelAnime->jointTable == NULL) || (skelAnime->morphTable == NULL)) {
        osSyncPrintf(VT_FGCOL(RED));
        // "Memory allocation error"
        osSyncPrintf("Skeleton_Info_Rom_SV_ct メモリアロケーションエラー\n");
        osSyncPrintf(VT_RST);
    }

    LinkAnimation_Change(globalCtx, skelAnime, animation, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f);
}

/**
 * Sets the update function of a SkelAnime that uses Link animations based on its mode
 */
void LinkAnimation_SetUpdateFunction(SkelAnime* skelAnime) {
    if (skelAnime->mode <= ANIMMODE_LOOP_INTERP) {
        skelAnime->update = LinkAnimation_Loop;
    } else {
        skelAnime->update = LinkAnimation_Once;
    }
    skelAnime->morphWeight = 0.0f;
}

/**
 * Advances the current Link animation and updates all frame tables. If the animation plays once, returns true when it
 * finishes.
 */
s32 LinkAnimation_Update(GlobalContext* globalCtx, SkelAnime* skelAnime) {
    return skelAnime->update(globalCtx, skelAnime);
}

/**
 * Requests an interpolation between the pose in jointTable to the one in morphTable, advancing the morph but not the
 * animation frame
 */
s32 LinkAnimation_Morph(GlobalContext* globalCtx, SkelAnime* skelAnime) {
    f32 prevMorphWeight = skelAnime->morphWeight;
    f32 updateRate = R_UPDATE_RATE * 0.5f;

    skelAnime->morphWeight -= skelAnime->morphRate * updateRate;

    if (skelAnime->morphWeight <= 0.0f) {
        LinkAnimation_SetUpdateFunction(skelAnime);
    }

    AnimationContext_SetInterp(globalCtx, skelAnime->limbCount, skelAnime->jointTable, skelAnime->morphTable,
                               1.0f - (skelAnime->morphWeight / prevMorphWeight));
    return 0;
}

/**
 * Requests a load of the next frame of a Link animation, advances the morph, and requests an interpolation between
 * jointTable and morphTable
 */
void LinkAnimation_AnimateFrame(GlobalContext* globalCtx, SkelAnime* skelAnime) {
    AnimationContext_SetLoadFrame(globalCtx, skelAnime->animation, skelAnime->curFrame, skelAnime->limbCount,
                                  skelAnime->jointTable);
    if (skelAnime->morphWeight != 0) {
        f32 updateRate = R_UPDATE_RATE * 0.5f;

        skelAnime->morphWeight -= skelAnime->morphRate * updateRate;
        if (skelAnime->morphWeight <= 0.0f) {
            skelAnime->morphWeight = 0.0f;
        } else {
            AnimationContext_SetInterp(globalCtx, skelAnime->limbCount, skelAnime->jointTable, skelAnime->morphTable,
                                       skelAnime->morphWeight);
        }
    }
}

/**
 * Advances a Link animation that loops over its full length
 */
s32 LinkAnimation_Loop(GlobalContext* globalCtx, SkelAnime* skelAnime) {
    f32 updateRate = R_UPDATE_RATE * 0.5f;

    skelAnime->curFrame += skelAnime->playSpeed * updateRate;
    if (skelAnime->curFrame < 0.0f) {
        skelAnime->curFrame += skelAnime->animLength;
    } else if (skelAnime->animLength <= skelAnime->curFrame) {
        skelAnime->curFrame -= skelAnime->animLength;
    }
    LinkAnimation_AnimateFrame(globalCtx, skelAnime);
    return 0;
}

/**
 * Advances a Link animation that stops at endFrame and returns true when it is reached.
 */
s32 LinkAnimation_Once(GlobalContext* globalCtx, SkelAnime* skelAnime) {
    f32 updateRate = R_UPDATE_RATE * 0.5f;

    if (skelAnime->curFrame == skelAnime->endFrame) {
        LinkAnimation_AnimateFrame(globalCtx, skelAnime);
        return 1;
    }
    skelAnime->curFrame += skelAnime->playSpeed * updateRate;
    if ((skelAnime->curFrame - skelAnime->endFrame) * skelAnime->playSpeed > 0.0f) {
        skelAnime->curFrame = skelAnime->endFrame;
    } else if (skelAnime->curFrame < 0.0f) {
        skelAnime->curFrame += skelAnime->animLength;
    } else if (skelAnime->animLength <= skelAnime->curFrame) {
        skelAnime->curFrame -= skelAnime->animLength;
    }
    LinkAnimation_AnimateFrame(globalCtx, skelAnime);
    return 0;
}

/**
 * Sets a new morph and resets the morph weight for the current animation.
 */
void Animation_SetMorph(GlobalContext* globalCtx, SkelAnime* skelAnime, f32 morphFrames) {
    skelAnime->morphWeight = 1.0f;
    skelAnime->morphRate = 1.0f / morphFrames;
}

/**
 * General way to set a new Link animation, allowing choice of playback speed, start frame, end frame, play mode, and
 * number of transition frames. Positive morph frames morph from the current pose to the start pose of the new
 * animation, then start the new animation. Negative morph frames start the new animation immediately, modified by the
 * pose immediately before the animation change.
 */
void LinkAnimation_Change(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation, f32 playSpeed,
                          f32 startFrame, f32 endFrame, u8 mode, f32 morphFrames) {
    LinkAnimationHeader* ogAnim = animation;

    if (ResourceMgr_OTRSigCheck(animation) != 0)
        animation = ResourceMgr_LoadAnimByName(animation);

    AnimationHeader* currentAnimation = (AnimationHeader*)skelAnime->animation;
    if (ResourceMgr_OTRSigCheck(currentAnimation) != 0)
        currentAnimation = ResourceMgr_LoadAnimByName(currentAnimation);

    skelAnime->mode = mode;
    if ((morphFrames != 0.0f) && ((animation != currentAnimation) || (startFrame != skelAnime->curFrame))) {
        if (morphFrames < 0) {
            LinkAnimation_SetUpdateFunction(skelAnime);
            SkelAnime_CopyFrameTable(skelAnime, skelAnime->morphTable, skelAnime->jointTable);
            morphFrames = -morphFrames;
        } else {
            skelAnime->update = LinkAnimation_Morph;
            AnimationContext_SetLoadFrame(globalCtx, animation, (s32)startFrame, skelAnime->limbCount,
                                          skelAnime->morphTable);
        }
        skelAnime->morphWeight = 1.0f;
        skelAnime->morphRate = 1.0f / morphFrames;
    } else {
        LinkAnimation_SetUpdateFunction(skelAnime);
        AnimationContext_SetLoadFrame(globalCtx, animation, (s32)startFrame, skelAnime->limbCount,
                                      skelAnime->jointTable);
        skelAnime->morphWeight = 0.0f;
    }

    skelAnime->animation = ogAnim;
    skelAnime->curFrame = 0.0f;
    skelAnime->startFrame = startFrame;
    skelAnime->curFrame = startFrame;
    skelAnime->endFrame = endFrame;
    skelAnime->animLength = Animation_GetLength(animation);
    skelAnime->playSpeed = playSpeed;
}

/**
 * Immediately changes to a Link animation that plays once at the default speed.
 */
void LinkAnimation_PlayOnce(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation) {
    LinkAnimation_Change(globalCtx, skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE,
                         0.0f);
}

/**
 * Immediately changes to a Link animation that plays once at the specified speed.
 */
void LinkAnimation_PlayOnceSetSpeed(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation,
                                    f32 playSpeed) {
    LinkAnimation_Change(globalCtx, skelAnime, animation, playSpeed, 0.0f, Animation_GetLastFrame(animation),
                         ANIMMODE_ONCE, 0.0f);
}

/**
 * Immediately changes to a Link animation that loops at the default speed.
 */
void LinkAnimation_PlayLoop(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation) {
    LinkAnimation_Change(globalCtx, skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP,
                         0.0f);
}

/**
 * Immediately changes to a Link animation that loops at the specified speed.
 */
void LinkAnimation_PlayLoopSetSpeed(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation,
                                    f32 playSpeed) {
    LinkAnimation_Change(globalCtx, skelAnime, animation, playSpeed, 0.0f, Animation_GetLastFrame(animation),
                         ANIMMODE_LOOP, 0.0f);
}

/**
 * Requests copying jointTable to morphTable
 */
void LinkAnimation_CopyJointToMorph(GlobalContext* globalCtx, SkelAnime* skelAnime) {
    AnimationContext_SetCopyAll(globalCtx, skelAnime->limbCount, skelAnime->morphTable, skelAnime->jointTable);
}

/**
 * Requests copying morphTable to jointTable
 * unused
 */
void LinkAnimation_CopyMorphToJoint(GlobalContext* globalCtx, SkelAnime* skelAnime) {
    AnimationContext_SetCopyAll(globalCtx, skelAnime->limbCount, skelAnime->jointTable, skelAnime->morphTable);
}

/**
 * Requests loading frame data from the Link animation into morphTable
 */
void LinkAnimation_LoadToMorph(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation,
                               f32 frame) {
    AnimationContext_SetLoadFrame(globalCtx, animation, (s32)frame, skelAnime->limbCount, skelAnime->morphTable);
}

/**
 * Requests loading frame data from the Link animation into jointTable
 */
void LinkAnimation_LoadToJoint(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation,
                               f32 frame) {
    AnimationContext_SetLoadFrame(globalCtx, animation, (s32)frame, skelAnime->limbCount, skelAnime->jointTable);
}

/**
 * Requests interpolating between jointTable and morphTable, placing the result in jointTable
 */
void LinkAnimation_InterpJointMorph(GlobalContext* globalCtx, SkelAnime* skelAnime, f32 weight) {
    AnimationContext_SetInterp(globalCtx, skelAnime->limbCount, skelAnime->jointTable, skelAnime->morphTable, weight);
}

/**
 * Requests loading frame data from the Link animations and blending them, placing the result in jointTable
 */
void LinkAnimation_BlendToJoint(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation1,
                                f32 frame1, LinkAnimationHeader* animation2, f32 frame2, f32 blendWeight,
                                Vec3s* blendTable) {
    Vec3s* alignedBlendTable;

    AnimationContext_SetLoadFrame(globalCtx, animation1, (s32)frame1, skelAnime->limbCount, skelAnime->jointTable);

    alignedBlendTable = (Vec3s*)ALIGN16((uintptr_t)blendTable);

    AnimationContext_SetLoadFrame(globalCtx, animation2, (s32)frame2, skelAnime->limbCount, alignedBlendTable);
    AnimationContext_SetInterp(globalCtx, skelAnime->limbCount, skelAnime->jointTable, alignedBlendTable, blendWeight);
}

/**
 * Requests loading frame data from the Link animations and blending them, placing the result in morphTable
 */
void LinkAnimation_BlendToMorph(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation1,
                                f32 frame1, LinkAnimationHeader* animation2, f32 frame2, f32 blendWeight,
                                Vec3s* blendTable) {
    Vec3s* alignedBlendTable;

    AnimationContext_SetLoadFrame(globalCtx, animation1, (s32)frame1, skelAnime->limbCount, skelAnime->morphTable);

    alignedBlendTable = (Vec3s*)ALIGN16((uintptr_t)blendTable);

    AnimationContext_SetLoadFrame(globalCtx, animation2, (s32)frame2, skelAnime->limbCount, alignedBlendTable);
    AnimationContext_SetInterp(globalCtx, skelAnime->limbCount, skelAnime->morphTable, alignedBlendTable, blendWeight);
}

/**
 * Changes a looping animation to one that stops at the end. Unused
 */
void LinkAnimation_EndLoop(SkelAnime* skelAnime) {
    skelAnime->mode = ANIMMODE_ONCE;
    LinkAnimation_SetUpdateFunction(skelAnime);
}

/**
 * Checks if the current frame is after frame and the previous frame was before it.
 */
s32 Animation_OnFrameImpl(SkelAnime* skelAnime, f32 frame, f32 updateRate) {
    f32 updateSpeed = skelAnime->playSpeed * updateRate;
    f32 prevFrame = skelAnime->curFrame - updateSpeed;
    f32 curFrameDiff;
    f32 prevFrameDiff;

    if (prevFrame < 0.0f) {
        prevFrame += skelAnime->animLength;
    } else if (prevFrame >= skelAnime->animLength) {
        prevFrame -= skelAnime->animLength;
    }

    if ((frame == 0.0f) && (updateSpeed > 0.0f)) {
        frame = skelAnime->animLength;
    }

    curFrameDiff = prevFrame + updateSpeed - frame;
    prevFrameDiff = curFrameDiff - updateSpeed;
    if ((curFrameDiff * updateSpeed >= 0.0f) && (prevFrameDiff * updateSpeed < 0.0f)) {
        return true;
    }
    return false;
}

/**
 * Checks if the current Link animation has reached the specified frame
 */
s32 LinkAnimation_OnFrame(SkelAnime* skelAnime, f32 frame) {
    f32 updateRate = R_UPDATE_RATE * 0.5f;

    return Animation_OnFrameImpl(skelAnime, frame, updateRate);
}

/**
 * Initializes a normal skeleton to a looping animation, dynamically allocating the frame tables if not provided.
 */
s32 SkelAnime_Init(GlobalContext* globalCtx, SkelAnime* skelAnime, SkeletonHeader* skeletonHeaderSeg,
                   AnimationHeader* animation, Vec3s* jointTable, Vec3s* morphTable, s32 limbCount) {
    if (ResourceMgr_OTRSigCheck(skeletonHeaderSeg))
        skeletonHeaderSeg = ResourceMgr_LoadSkeletonByName(skeletonHeaderSeg);

    SkeletonHeader* skeletonHeader = SEGMENTED_TO_VIRTUAL(skeletonHeaderSeg);

    skelAnime->limbCount = skeletonHeader->limbCount + 1;
    skelAnime->skeleton = SEGMENTED_TO_VIRTUAL(skeletonHeader->segment);
    if (jointTable == NULL) {
        skelAnime->jointTable =
            ZELDA_ARENA_MALLOC_DEBUG(skelAnime->limbCount * sizeof(*skelAnime->jointTable));
        skelAnime->morphTable =
            ZELDA_ARENA_MALLOC_DEBUG(skelAnime->limbCount * sizeof(*skelAnime->morphTable));
    } else {
        ASSERT(limbCount == skelAnime->limbCount);
        skelAnime->jointTable = jointTable;
        skelAnime->morphTable = morphTable;
    }
    if ((skelAnime->jointTable == NULL) || (skelAnime->morphTable == NULL)) {
        osSyncPrintf(VT_FGCOL(RED));
        osSyncPrintf("Skeleton_Info2_ct メモリアロケーションエラー\n"); // "Memory allocation error"
        osSyncPrintf(VT_RST);
    }

    if (animation != NULL) {
        Animation_PlayLoop(skelAnime, animation);
    }
}

/**
 * Initializes a flex skeleton to a looping animation, dynamically allocating the frame tables if not given.
 */
s32 SkelAnime_InitFlex(GlobalContext* globalCtx, SkelAnime* skelAnime, FlexSkeletonHeader* skeletonHeaderSeg,
                       AnimationHeader* animation, Vec3s* jointTable, Vec3s* morphTable, s32 limbCount) {
    if (ResourceMgr_OTRSigCheck(skeletonHeaderSeg) != 0)
        skeletonHeaderSeg = ResourceMgr_LoadSkeletonByName(skeletonHeaderSeg);

    FlexSkeletonHeader* skeletonHeader = SEGMENTED_TO_VIRTUAL(skeletonHeaderSeg);

    skelAnime->limbCount = skeletonHeader->sh.limbCount + 1;
    skelAnime->dListCount = skeletonHeader->dListCount;
    skelAnime->skeleton = SEGMENTED_TO_VIRTUAL(skeletonHeader->sh.segment);

    if (jointTable == NULL) {
        skelAnime->jointTable =
            ZELDA_ARENA_MALLOC_DEBUG(skelAnime->limbCount * sizeof(*skelAnime->jointTable));

        skelAnime->morphTable =
            ZELDA_ARENA_MALLOC_DEBUG(skelAnime->limbCount * sizeof(*skelAnime->morphTable));
    } else {
        ASSERT(limbCount == skelAnime->limbCount);
        skelAnime->jointTable = jointTable;
        skelAnime->morphTable = morphTable;
    }
    if ((skelAnime->jointTable == NULL) || (skelAnime->morphTable == NULL)) {
        osSyncPrintf(VT_FGCOL(RED));
        // "Memory allocation error"
        osSyncPrintf("Skeleton_Info_Rom_SV_ct メモリアロケーションエラー\n");
        osSyncPrintf(VT_RST);
    }

    if (animation != NULL) {
        Animation_PlayLoop(skelAnime, animation);
    }
}

/**
 * Initializes a skeleton with SkinLimbs to a looping animation, dynamically allocating the frame tables.
 */
s32 SkelAnime_InitSkin(GlobalContext* globalCtx, SkelAnime* skelAnime, SkeletonHeader* skeletonHeaderSeg,
                       AnimationHeader* animation) {
    if (ResourceMgr_OTRSigCheck(skeletonHeaderSeg) != 0)
        animation = ResourceMgr_LoadAnimByName(skeletonHeaderSeg);

    SkeletonHeader* skeletonHeader = SEGMENTED_TO_VIRTUAL(skeletonHeaderSeg);

    skelAnime->limbCount = skeletonHeader->limbCount + 1;
    skelAnime->skeleton = SEGMENTED_TO_VIRTUAL(skeletonHeader->segment);
    skelAnime->jointTable =
        ZELDA_ARENA_MALLOC_DEBUG(skelAnime->limbCount * sizeof(*skelAnime->jointTable));
    skelAnime->morphTable =
        ZELDA_ARENA_MALLOC_DEBUG(skelAnime->limbCount * sizeof(*skelAnime->morphTable));
    if ((skelAnime->jointTable == NULL) || (skelAnime->morphTable == NULL)) {
        osSyncPrintf(VT_FGCOL(RED));
        // "Memory allocation error"
        osSyncPrintf("Skeleton_Info2_skin2_ct メモリアロケーションエラー\n");
        osSyncPrintf(VT_RST);
    }

    if (animation != NULL) {
        Animation_PlayLoop(skelAnime, animation);
    }
}

/**
 * Sets the SkelAnime's update function based on its current mode.
 */
void SkelAnime_SetUpdate(SkelAnime* skelAnime) {
    if (skelAnime->mode <= ANIMMODE_LOOP_INTERP) {
        skelAnime->update = SkelAnime_LoopFull;
    } else if (skelAnime->mode <= ANIMMODE_ONCE_INTERP) {
        skelAnime->update = SkelAnime_Once;
    } else {
        skelAnime->update = SkelAnime_LoopPartial;
    }
}

/**
 * Advances the current animation and updates all frame tables. If the animation plays once, returns true when it
 * finishes.
 */
s32 SkelAnime_Update(SkelAnime* skelAnime) {
    return skelAnime->update(skelAnime);
}

/**
 * Morphs from the pose in jointTable to the one in morphTable, advancing the morph but not the animation frame
 */
s32 SkelAnime_Morph(SkelAnime* skelAnime) {
    f32 prevMorphWeight = skelAnime->morphWeight;
    f32 updateRate = R_UPDATE_RATE * (1.0f / 3.0f);

    skelAnime->morphWeight -= skelAnime->morphRate * updateRate;
    if (skelAnime->morphWeight <= 0.0f) {
        SkelAnime_SetUpdate(skelAnime);
        skelAnime->morphWeight = 0.0f;
    }
    SkelAnime_InterpFrameTable(skelAnime->limbCount, skelAnime->jointTable, skelAnime->jointTable,
                               skelAnime->morphTable, 1.0f - (skelAnime->morphWeight / prevMorphWeight));
    return 0;
}

/**
 * Performs a tapered morph from the pose in jointTable to the one in morphTable, advancing the morph but not the
 * animation frame
 */
s32 SkelAnime_MorphTaper(SkelAnime* skelAnime) {
    s16 prevPhase = skelAnime->morphWeight * 0x4000;
    s16 curPhase;
    f32 prevWeight;
    f32 curWeight;
    f32 updateRate = R_UPDATE_RATE * (1.0f / 3.0f);

    skelAnime->morphWeight -= skelAnime->morphRate * updateRate;
    if (skelAnime->morphWeight <= 0.0f) {
        SkelAnime_SetUpdate(skelAnime);
        skelAnime->morphWeight = 0.0f;
    }
    curPhase = skelAnime->morphWeight * 0x4000;
    if (skelAnime->taper <= ANIMTAPER_DECEL) {
        prevWeight = 1.0f - Math_CosS(prevPhase);
        curWeight = 1.0f - Math_CosS(curPhase);
    } else {
        prevWeight = Math_SinS(prevPhase);
        curWeight = Math_SinS(curPhase);
    }
    if (curWeight != 0.0f) {
        curWeight /= prevWeight;
    } else {
        curWeight = 0.0f;
    }
    SkelAnime_InterpFrameTable(skelAnime->limbCount, skelAnime->jointTable, skelAnime->jointTable,
                               skelAnime->morphTable, 1.0f - curWeight);
    return 0;
}

/**
 * Gets frame data for the current frame as modified by morphTable and advances the morph
 */
void SkelAnime_AnimateFrame(SkelAnime* skelAnime) {
    Vec3s nextjointTable[100];

    SkelAnime_GetFrameData(skelAnime->animation, skelAnime->curFrame, skelAnime->limbCount, skelAnime->jointTable);
    if (skelAnime->mode & ANIM_INTERP) {
        s32 frame = skelAnime->curFrame;
        f32 partialFrame = skelAnime->curFrame - frame;

        if (++frame >= (s32)skelAnime->animLength) {
            frame = 0;
        }
        SkelAnime_GetFrameData(skelAnime->animation, frame, skelAnime->limbCount, nextjointTable);
        SkelAnime_InterpFrameTable(skelAnime->limbCount, skelAnime->jointTable, skelAnime->jointTable, nextjointTable,
                                   partialFrame);
    }
    if (skelAnime->morphWeight != 0) {
        f32 updateRate = R_UPDATE_RATE * (1.0f / 3.0f);

        skelAnime->morphWeight -= skelAnime->morphRate * updateRate;
        if (skelAnime->morphWeight <= 0.0f) {
            skelAnime->morphWeight = 0.0f;
        } else {
            SkelAnime_InterpFrameTable(skelAnime->limbCount, skelAnime->jointTable, skelAnime->jointTable,
                                       skelAnime->morphTable, skelAnime->morphWeight);
        }
    }
}

/**
 * Advances an animation that loops over its full length and updates the frame tables
 */
s32 SkelAnime_LoopFull(SkelAnime* skelAnime) {
    f32 updateRate = R_UPDATE_RATE * (1.0f / 3.0f);

    skelAnime->curFrame += skelAnime->playSpeed * updateRate;
    if (skelAnime->curFrame < 0.0f) {
        skelAnime->curFrame += skelAnime->animLength;
    } else if (skelAnime->animLength <= skelAnime->curFrame) {
        skelAnime->curFrame -= skelAnime->animLength;
    }
    SkelAnime_AnimateFrame(skelAnime);
    return 0;
}

/**
 * Advances an animation that loops over part of its length and updates the frame tables
 */
s32 SkelAnime_LoopPartial(SkelAnime* skelAnime) {
    f32 updateRate = R_UPDATE_RATE * (1.0f / 3.0f);

    skelAnime->curFrame += skelAnime->playSpeed * updateRate;
    if (skelAnime->curFrame < skelAnime->startFrame) {
        skelAnime->curFrame = (skelAnime->curFrame - skelAnime->startFrame) + skelAnime->endFrame;
    } else if (skelAnime->endFrame <= skelAnime->curFrame) {
        skelAnime->curFrame = (skelAnime->curFrame - skelAnime->endFrame) + skelAnime->startFrame;
    }

    SkelAnime_AnimateFrame(skelAnime);
    return 0;
}

/**
 * Advances an animation that stops at endFrame and returns true when it is reached.
 */
s32 SkelAnime_Once(SkelAnime* skelAnime) {
    f32 updateRate = R_UPDATE_RATE * (1.0f / 3.0f);

    if (skelAnime->curFrame == skelAnime->endFrame) {
        SkelAnime_GetFrameData(skelAnime->animation, (s32)skelAnime->curFrame, skelAnime->limbCount,
                               skelAnime->jointTable);
        SkelAnime_AnimateFrame(skelAnime);
        return 1;
    }

    skelAnime->curFrame += skelAnime->playSpeed * updateRate;

    if ((skelAnime->curFrame - skelAnime->endFrame) * skelAnime->playSpeed > 0.0f) {
        skelAnime->curFrame = skelAnime->endFrame;
    } else if (skelAnime->curFrame < 0.0f) {
        skelAnime->curFrame += skelAnime->animLength;
    } else if (skelAnime->animLength <= skelAnime->curFrame) {
        skelAnime->curFrame -= skelAnime->animLength;
    }
    SkelAnime_AnimateFrame(skelAnime);
    return 0;
}

/**
 * Fully general way to set a new animation, allowing choice of playback speed, start frame, end frame, play mode,
 * number of transition frames, and tapering of the transition. Positive morph frames morph from the current pose to the
 * start pose of the new animation, then start the new animation. Negative morph frames start the new animation
 * immediately, modified by the pose immediately before the animation change.
 */
void Animation_ChangeImpl(SkelAnime* skelAnime, AnimationHeader* animation, f32 playSpeed, f32 startFrame, f32 endFrame,
                          u8 mode, f32 morphFrames, s8 taper) {
    LinkAnimationHeader* ogAnim = animation;

    if (ResourceMgr_OTRSigCheck(animation) != 0)
        animation = ResourceMgr_LoadAnimByName(animation);

    AnimationHeader* currentAnimation = (AnimationHeader*)skelAnime->animation;
    if (ResourceMgr_OTRSigCheck(currentAnimation) != 0)
        currentAnimation = ResourceMgr_LoadAnimByName(currentAnimation);

    skelAnime->mode = mode;
    if ((morphFrames != 0.0f) && ((animation != currentAnimation) || (startFrame != skelAnime->curFrame))) {
        if (morphFrames < 0) {
            SkelAnime_SetUpdate(skelAnime);
            SkelAnime_CopyFrameTable(skelAnime, skelAnime->morphTable, skelAnime->jointTable);
            morphFrames = -morphFrames;
        } else {
            if (taper != ANIMTAPER_NONE) {
                skelAnime->update = SkelAnime_MorphTaper;
                skelAnime->taper = taper;
            } else {
                skelAnime->update = SkelAnime_Morph;
            }
            SkelAnime_GetFrameData(animation, startFrame, skelAnime->limbCount, skelAnime->morphTable);
        }
        skelAnime->morphWeight = 1.0f;
        skelAnime->morphRate = 1.0f / morphFrames;
    } else {
        SkelAnime_SetUpdate(skelAnime);
        SkelAnime_GetFrameData(animation, startFrame, skelAnime->limbCount, skelAnime->jointTable);
        skelAnime->morphWeight = 0.0f;
    }

    skelAnime->animation = ogAnim;
    skelAnime->startFrame = startFrame;
    skelAnime->endFrame = endFrame;
    skelAnime->animLength = Animation_GetLength(animation);
    if (skelAnime->mode >= ANIMMODE_LOOP_PARTIAL) {
        skelAnime->curFrame = 0.0f;
    } else {
        skelAnime->curFrame = startFrame;
        if (skelAnime->mode <= ANIMMODE_LOOP_INTERP) {
            skelAnime->endFrame = skelAnime->animLength - 1.0f;
        }
    }
    skelAnime->playSpeed = playSpeed;
}

/**
 * General way to set a new animation, allowing choice of playback speed, start frame, end frame, play mode, and number
 * of transition frames. Positive morph frames morph from the current pose to the start pose of the new animation, then
 * start the new animation. Negative morph frames start the new animation immediately, modified by the pose immediately
 * before the animation change.
 */
void Animation_Change(SkelAnime* skelAnime, AnimationHeader* animation, f32 playSpeed, f32 startFrame, f32 endFrame,
                      u8 mode, f32 morphFrames) {
    AnimationHeader* ogAnim = animation;

    if (ResourceMgr_OTRSigCheck(animation) != 0)
        animation = ResourceMgr_LoadAnimByName(animation);

    Animation_ChangeImpl(skelAnime, animation, playSpeed, startFrame, endFrame, mode, morphFrames, ANIMTAPER_NONE);
    skelAnime->animation = ogAnim;
}

/**
 * Immediately changes to an animation that plays once at the default speed.
 */
void Animation_PlayOnce(SkelAnime* skelAnime, AnimationHeader* animation) {
    Animation_Change(skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE, 0.0f);
}

/**
 * Smoothly transitions to an animation that plays once at the default speed.
 * Positive morph frames morph from the current pose to the start pose of the new animation, then start the new
 * animation. Negative morph frames start the new animation immediately, modified by the pose immediately before the
 * animation change.
 */
void Animation_MorphToPlayOnce(SkelAnime* skelAnime, AnimationHeader* animation, f32 morphFrames) {
    Animation_Change(skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE, morphFrames);
}

/**
 * Immediately changes to an animation that plays once at the specified speed.
 */
void Animation_PlayOnceSetSpeed(SkelAnime* skelAnime, AnimationHeader* animation, f32 playSpeed) {
    Animation_Change(skelAnime, animation, playSpeed, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_ONCE, 0.0f);
}

/**
 * Immediately changes to an animation that loops at the default.
 */
void Animation_PlayLoop(SkelAnime* skelAnime, AnimationHeader* animation) {
    Animation_Change(skelAnime, animation, 1.0f, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP, 0.0f);
}

/**
 * Smoothly transitions to a looping animation, specifying the number of frames for the transition.
 * Positive morph frames morph from the current pose to the start pose of the new animation, then start the new
 * animation. Negative morph frames start the new animation immediately, modified by the pose immediately before the
 * animation change.
 */
void Animation_MorphToLoop(SkelAnime* skelAnime, AnimationHeader* animation, f32 morphFrames) {
    Animation_Change(skelAnime, animation, 1.0f, 0.0f, 0.0f, ANIMMODE_LOOP, morphFrames);
}

/**
 * Immediately changes to an animation that loops at the specified speed.
 */
void Animation_PlayLoopSetSpeed(SkelAnime* skelAnime, AnimationHeader* animation, f32 playSpeed) {
    Animation_Change(skelAnime, animation, playSpeed, 0.0f, Animation_GetLastFrame(animation), ANIMMODE_LOOP, 0.0f);
}

/**
 * Changes a looping animation to one that stops at the end. Unused
 */
void Animation_EndLoop(SkelAnime* skelAnime) {
    skelAnime->mode = ANIMMODE_ONCE;
    skelAnime->endFrame = skelAnime->animLength;
    SkelAnime_SetUpdate(skelAnime);
}

/**
 * Reverses the current animation.
 */
void Animation_Reverse(SkelAnime* skelAnime) {
    f32 startFrame = skelAnime->startFrame;

    skelAnime->startFrame = skelAnime->endFrame;
    skelAnime->playSpeed = -skelAnime->playSpeed;
    skelAnime->endFrame = startFrame;
}

/**
 * Copies the src frame table to the dst frame table if copyFlag for that limb is true.
 * Used only by En_Test
 */
void SkelAnime_CopyFrameTableTrue(SkelAnime* skelAnime, Vec3s* dst, Vec3s* src, u8* copyFlag) {
    s32 i;

    for (i = 0; i < skelAnime->limbCount; i++, dst++, src++) {
        if (*copyFlag++) {
            *dst = *src;
        }
    }
}

/**
 * Copies the src frame table to the dst frame table if copyFlag for that limb is false.
 * Unused.
 */
void SkelAnime_CopyFrameTableFalse(SkelAnime* skelAnime, Vec3s* dst, Vec3s* src, u8* copyFlag) {
    s32 i;

    for (i = 0; i < skelAnime->limbCount; i++, dst++, src++) {
        if (!*copyFlag++) {
            *dst = *src;
        }
    }
}

/**
 * Updates translation of the root limb and calculates `pos`, the difference between
 * the old and new positions of the root limb as rotated by `angle`. Used to allow
 * animations to change an actor's position.
 */
void SkelAnime_UpdateTranslation(SkelAnime* skelAnime, Vec3f* diff, s16 angle) {
    f32 x;
    f32 z;
    f32 sin;
    f32 cos;

    if (skelAnime->moveFlags & ANIM_FLAG_NOMOVE) {
        diff->x = diff->z = 0.0f;
    } else {
        x = skelAnime->jointTable[0].x;
        z = skelAnime->jointTable[0].z;
        sin = Math_SinS(angle);
        cos = Math_CosS(angle);
        diff->x = x * cos + z * sin;
        diff->z = z * cos - x * sin;
        x = skelAnime->prevTransl.x;
        z = skelAnime->prevTransl.z;
        sin = Math_SinS(skelAnime->prevRot);
        cos = Math_CosS(skelAnime->prevRot);
        diff->x -= x * cos + z * sin;
        diff->z -= z * cos - x * sin;
    }

    skelAnime->prevRot = angle;
    skelAnime->prevTransl.x = skelAnime->jointTable[0].x;
    skelAnime->jointTable[0].x = skelAnime->baseTransl.x;
    skelAnime->prevTransl.z = skelAnime->jointTable[0].z;
    skelAnime->jointTable[0].z = skelAnime->baseTransl.z;
    if (skelAnime->moveFlags & ANIM_FLAG_UPDATEY) {
        if (skelAnime->moveFlags & ANIM_FLAG_NOMOVE) {
            diff->y = 0.0f;
        } else {
            diff->y = skelAnime->jointTable[0].y - skelAnime->prevTransl.y;
        }
        skelAnime->prevTransl.y = skelAnime->jointTable[0].y;
        skelAnime->jointTable[0].y = skelAnime->baseTransl.y;
    } else {
        diff->y = 0.0f;
        skelAnime->prevTransl.y = skelAnime->jointTable[0].y;
    }
    skelAnime->moveFlags &= ~ANIM_FLAG_NOMOVE;
}

/**
 * Checks if the current animation is at the specified frame
 */
s32 Animation_OnFrame(SkelAnime* skelAnime, f32 frame) {
    return Animation_OnFrameImpl(skelAnime, frame, 1.0f);
}

/**
 * Frees the frame tables for a skelAnime with dynamically allocated tables.
 */
void SkelAnime_Free(SkelAnime* skelAnime, GlobalContext* globalCtx) {
    if (skelAnime->jointTable != NULL) {
        ZELDA_ARENA_FREE_DEBUG(skelAnime->jointTable);
    } else {
        osSyncPrintf("now_joint あきまへん！！\n"); // "now_joint is freed! !"
    }

    if (skelAnime->morphTable != NULL) {
        ZELDA_ARENA_FREE_DEBUG(skelAnime->morphTable);
    } else {
        osSyncPrintf("morf_joint あきまへん！！\n"); // "morf_joint is freed !!"
    }
}

/**
 * Copies the src frame table to the dst frame table.
 */
void SkelAnime_CopyFrameTable(SkelAnime* skelAnime, Vec3s* dst, Vec3s* src) {
    s32 i;

    for (i = 0; i < skelAnime->limbCount; i++) {
        *dst++ = *src++;
    }
}
