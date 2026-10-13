#include "prevent_bss_reordering.h"
#include "global.h"
#include "z64skin.h"

// 60 is an arbitrary number which specifies the max amount of limbs per skeleton this system supports
MtxF gSkinLimbMatrices[60];

static s32 sBssPad;

void Skin_UpdateVertices(MtxF* mtx, SkinVertex* skinVertices, SkinLimbModif* modifEntry, Vtx* vtxBuf, Vec3f* pos) {
    SkinVertex* vertexEntry;
    Vtx* vtx;
    Vec3f wTemp;
    Vec3f normal;
    Vec3f sp44;

    wTemp.x = mtx->xw;
    wTemp.y = mtx->yw;
    wTemp.z = mtx->zw;

    mtx->xw = 0.0f;
    mtx->yw = 0.0f;
    mtx->zw = 0.0f;

    for (vertexEntry = skinVertices; vertexEntry < &skinVertices[modifEntry->vtxCount]; vertexEntry++) {
        vtx = &vtxBuf[vertexEntry->index];

        vtx->n.ob[0] = pos->x;
        vtx->n.ob[1] = pos->y;
        vtx->n.ob[2] = pos->z;

        sp44.x = vertexEntry->normX;
        sp44.y = vertexEntry->normY;
        sp44.z = vertexEntry->normZ;

        SkinMatrix_Vec3fMtxFMultXYZ(mtx, &sp44, &normal);

        vtx->n.n[0] = normal.x;
        vtx->n.n[1] = normal.y;
        vtx->n.n[2] = normal.z;
    }

    mtx->xw = wTemp.x;
    mtx->yw = wTemp.y;
    mtx->zw = wTemp.z;
}

void Skin_ApplyLimbModifications(GraphicsContext* gfxCtx, Skin* skin, s32 limbIndex, s32 arg3) {
    s32 modifCount;
    SkinLimb** skeleton;
    s32 pad;
    SkinAnimatedLimbData* data;
    s32 pad2[5];
    SkinLimbModif* modif;
    SkinLimbVtx* vtxEntry;
    s32 pad3;
    s32 transformCount;
    f32 scale;
    SkinVertex* skinVertices;
    Vec3f sp88;
    Vtx* vtxBuf;
    SkinTransformation* limbTransformations;
    Vec3f spDC;
    Vec3f spD0;
    SkinTransformation* transformationEntry;
    SkinLimb* limb;
    SkinLimbModif* modifications;

    OPEN_DISPS(gfxCtx);

    skeleton = (SkinLimb**)Lib_SegmentedToVirtual(skin->skeletonHeader->segment);
    limb = (SkinLimb*)Lib_SegmentedToVirtual(skeleton[limbIndex]);
    data = Lib_SegmentedToVirtual(limb->segment);
    modifications = (SkinLimbModif*)Lib_SegmentedToVirtual(data->limbModifications);

    vtxEntry = &skin->vtxTable[limbIndex];

    vtxBuf = vtxEntry->buf[vtxEntry->index];
    modifCount = data->limbModifCount;

    for (modif = modifications; modif < &modifications[modifCount]; modif++) {
        Vec3f spAC;
        Vec3f spA0;

        skinVertices = (SkinVertex*)Lib_SegmentedToVirtual(modif->skinVertices);
        limbTransformations = (SkinTransformation*)Lib_SegmentedToVirtual(modif->limbTransformations);
        transformCount = modif->transformCount;

        if (transformCount == 1) {
            spAC.x = limbTransformations[0].x;
            spAC.y = limbTransformations[0].y;
            spAC.z = limbTransformations[0].z;

            SkinMatrix_Vec3fMtxFMultXYZ(&gSkinLimbMatrices[limbTransformations[0].limbIndex], &spAC, &spDC);
        } else if (arg3) {
            transformationEntry = &limbTransformations[modif->unk_04];

            spA0.x = transformationEntry->x;
            spA0.y = transformationEntry->y;
            spA0.z = transformationEntry->z;
            SkinMatrix_Vec3fMtxFMultXYZ(&gSkinLimbMatrices[transformationEntry->limbIndex], &spA0, &spDC);
        } else {
            spDC.x = 0.0f;
            spDC.y = 0.0f;
            spDC.z = 0.0f;

            for (transformationEntry = limbTransformations; transformationEntry < &limbTransformations[transformCount];
                 transformationEntry++) {
                scale = transformationEntry->scale * 0.01f;

                sp88.x = transformationEntry->x;
                sp88.y = transformationEntry->y;
                sp88.z = transformationEntry->z;

                SkinMatrix_Vec3fMtxFMultXYZ(&gSkinLimbMatrices[transformationEntry->limbIndex], &sp88, &spD0);

                spDC.x += spD0.x * scale;
                spDC.y += spD0.y * scale;
                spDC.z += spD0.z * scale;
            }
        }

        Skin_UpdateVertices(&gSkinLimbMatrices[limbTransformations[modif->unk_04].limbIndex], skinVertices, modif,
                            vtxBuf, &spDC);
    }

    gSPSegment(POLY_OPA_DISP++, 0x08, vtxEntry->buf[vtxEntry->index]);

    vtxEntry->index = (vtxEntry->index == 0);

    CLOSE_DISPS(gfxCtx);
}

/**
 * Draw a limb of type SKIN_LIMB_TYPE_ANIMATED, of the skeleton `skin` at index `limbIndex`
 * The vertices of this limb are modified dynamically
 */
void Skin_DrawAnimatedLimb(GraphicsContext* gfxCtx, Skin* skin, s32 limbIndex, s32 arg3, s32 drawFlags) {
    SkinLimb** skeleton;
    s32 pad[3];
    SkinAnimatedLimbData* data;

    OPEN_DISPS(gfxCtx);

    skeleton = Lib_SegmentedToVirtual(skin->skeletonHeader->segment);
    data = Lib_SegmentedToVirtual(((SkinLimb*)Lib_SegmentedToVirtual(skeleton[limbIndex]))->segment);

    if (!(drawFlags & SKIN_DRAW_FLAG_CUSTOM_TRANSFORMS)) {
        Skin_ApplyLimbModifications(gfxCtx, skin, limbIndex, arg3);
    }

    gSPDisplayList(POLY_OPA_DISP++, data->dlist);

    CLOSE_DISPS(gfxCtx);
}

/**
 * Draw a limb of type SKIN_LIMB_TYPE_NORMAL, of the skeleton `skin` at index `limbIndex`
 */
void Skin_DrawLimb(GraphicsContext* gfxCtx, Skin* skin, s32 limbIndex, Gfx* dListOverride, s32 drawFlags) {
    Gfx* gfx = dListOverride;
    SkinLimb** skeleton;

    OPEN_DISPS(gfxCtx);

    skeleton = Lib_SegmentedToVirtual(skin->skeletonHeader->segment);

    if (dListOverride == NULL) {
        gfx = ((SkinLimb*)Lib_SegmentedToVirtual(skeleton[limbIndex]))->segment;
    }

    if (gfx != NULL) {
        Mtx* mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &gSkinLimbMatrices[limbIndex]);

        if (mtx != NULL) {
            gSPMatrix(POLY_OPA_DISP++, mtx, G_MTX_PUSH | G_MTX_MUL | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, gfx);
            gSPPopMatrix(POLY_OPA_DISP++, G_MTX_MODELVIEW);
            gDPPipeSync(POLY_OPA_DISP++);
        }
    }

    CLOSE_DISPS(gfxCtx);
}

void Skin_DrawImpl(Actor* actor, PlayState* play, Skin* skin, SkinPostDraw postDraw,
                   SkinOverrideLimbDraw overrideLimbDraw, s32 setTranslation, s32 arg6, s32 drawFlags) {
    s32 i;
    SkinLimb** skeleton;
    GraphicsContext* gfxCtx = play->state.gfxCtx;

    OPEN_DISPS(gfxCtx);

    if (!(drawFlags & SKIN_DRAW_FLAG_CUSTOM_TRANSFORMS)) {
        Skin_ApplyAnimTransformations(skin, gSkinLimbMatrices, actor, setTranslation);
    }

    skeleton = Lib_SegmentedToVirtual(skin->skeletonHeader->segment);

    if (!(drawFlags & SKIN_DRAW_FLAG_CUSTOM_MATRIX)) {
        Mtx* mtx;

        gSPMatrix(POLY_OPA_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &skin->mtx);
        if (mtx == NULL) {
            goto close_disps;
        }
        gSPMatrix(POLY_OPA_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    }

    for (i = 0; i < skin->skeletonHeader->limbCount; i++) {
        s32 shouldDraw = true;
        s32 segmentType;

        if (overrideLimbDraw != NULL) {
            shouldDraw = overrideLimbDraw(actor, play, i, skin);
        }

        segmentType = ((SkinLimb*)Lib_SegmentedToVirtual(skeleton[i]))->segmentType;

        if (segmentType == SKIN_LIMB_TYPE_ANIMATED && shouldDraw) {
            Skin_DrawAnimatedLimb(gfxCtx, skin, i, arg6, drawFlags);
        } else if (segmentType == SKIN_LIMB_TYPE_NORMAL && shouldDraw) {
            Skin_DrawLimb(gfxCtx, skin, i, NULL, drawFlags);
        }
    }

    if (postDraw != NULL) {
        postDraw(actor, play, skin);
    }

close_disps:;
    CLOSE_DISPS(gfxCtx);
}

// allows specifying postDraw and setTranslation
void func_80138228(Actor* actor, PlayState* play, Skin* skin, SkinPostDraw postDraw, s32 setTranslation) {
    Skin_DrawImpl(actor, play, skin, postDraw, NULL, setTranslation, false, 0);
}

// allows specifying OverrideLimbDraw, postDraw and setTranslation
void func_80138258(Actor* actor, PlayState* play, Skin* skin, SkinPostDraw postDraw,
                   SkinOverrideLimbDraw overrideLimbDraw, s32 setTranslation) {
    Skin_DrawImpl(actor, play, skin, postDraw, overrideLimbDraw, setTranslation, false, 0);
}

// allows specifying OverrideLimbDraw, postDraw, setTranslation, and arg6
void func_8013828C(Actor* actor, PlayState* play, Skin* skin, SkinPostDraw postDraw,
                   SkinOverrideLimbDraw overrideLimbDraw, s32 setTranslation, s32 arg6) {
    Skin_DrawImpl(actor, play, skin, postDraw, overrideLimbDraw, setTranslation, arg6, 0);
}

// allows specifying all variables
void func_801382C4(Actor* actor, PlayState* play, Skin* skin, SkinPostDraw postDraw,
                   SkinOverrideLimbDraw overrideLimbDraw, s32 setTranslation, s32 arg6, s32 drawFlags) {
    Skin_DrawImpl(actor, play, skin, postDraw, overrideLimbDraw, setTranslation, arg6, drawFlags);
}

void Skin_GetLimbPos(Skin* skin, s32 limbIndex, Vec3f* offset, Vec3f* dst) {
    MtxF mtxf;

    SkinMatrix_MtxFMtxFMult(&skin->mtx, &gSkinLimbMatrices[limbIndex], &mtxf);
    SkinMatrix_Vec3fMtxFMultXYZ(&mtxf, offset, dst);
}

void Skin_GetVertexPos(Skin* skin, s32 limbIndex, s32 vtxIndex, Vec3f* dst) {
    Vec3f pos;
    Vtx* vtx = &skin->vtxTable[limbIndex].buf[skin->vtxTable[0].index][vtxIndex];

    pos.x = vtx->n.ob[0];
    pos.y = vtx->n.ob[1];
    pos.z = vtx->n.ob[2];
    SkinMatrix_Vec3fMtxFMultXYZ(&skin->mtx, &pos, dst);
}
