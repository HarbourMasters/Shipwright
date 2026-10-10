#include "z64skin.h"

#include "global.h"
#include "z64malloc.h"

#include "2s2h/BenPort.h"

void Skin_Setup(Skin* skin) {
    skin->skeletonHeader = NULL;
    skin->limbCount = 0;
    skin->vtxTable = NULL;
}

/**
 * Initialises the Vtx buffers used for limb at index `limbIndex`
 */
void Skin_InitAnimatedLimb(GameState* gameState, Skin* skin, s32 limbIndex) {
    s32 i;
    SkinLimb** skeleton = Lib_SegmentedToVirtual(skin->skeletonHeader->segment);
    SkinAnimatedLimbData* animatedLimbData =
        Lib_SegmentedToVirtual(((SkinLimb*)Lib_SegmentedToVirtual(skeleton[limbIndex]))->segment);
    SkinLimbModif* limbModifications = Lib_SegmentedToVirtual(animatedLimbData->limbModifications);

    for (i = 0; i < ARRAY_COUNT(skin->vtxTable->buf); i++) {
        Vtx* vertices = skin->vtxTable[limbIndex].buf[i];
        SkinLimbModif* modifEntry;

        for (modifEntry = limbModifications; modifEntry < &limbModifications[animatedLimbData->limbModifCount];
             modifEntry++) {
            SkinVertex* skinVertices = Lib_SegmentedToVirtual(modifEntry->skinVertices);
            SkinVertex* skinVtxEntry;

            for (skinVtxEntry = skinVertices; skinVtxEntry < &skinVertices[modifEntry->vtxCount];) {
                Vtx* vtx = &vertices[skinVtxEntry->index];

                skinVtxEntry++;

                vtx->n.flag = 0;
                vtx->n.tc[0] = skinVtxEntry[-1].s;
                vtx->n.tc[1] = skinVtxEntry[-1].t;
                vtx->n.a = skinVtxEntry[-1].alpha;
            }
        }
    }
}

/**
 * Initializes a skin skeleton to looping animation, dynamically allocating the frame tables,
 * and dynamically allocating and initialising the Vtx and SkinLimbVtx buffers for its animated limbs
 */
void Skin_Init(GameState* gameState, Skin* skin, SkeletonHeader* skeletonHeader, AnimationHeader* animationHeader) {
    s32 limbCount;
    s32 i;
    SkinLimb** skeleton;

    if (ResourceMgr_OTRSigCheck(skeletonHeader)) {
        skeletonHeader = ResourceMgr_LoadSkeletonByName(skeletonHeader, NULL);
    }

    SkeletonHeader* virtSkelHeader = Lib_SegmentedToVirtual(skeletonHeader);

    skin->limbCount = virtSkelHeader->limbCount;
    skin->skeletonHeader = virtSkelHeader;
    limbCount = skin->skeletonHeader->limbCount;

    skeleton = Lib_SegmentedToVirtual(skin->skeletonHeader->segment);
    skin->vtxTable = ZeldaArena_Malloc(limbCount * sizeof(SkinLimbVtx));

    for (i = 0; i < limbCount; i++) {
        SkinLimbVtx* vtxEntry = &skin->vtxTable[i];

        if ((((SkinLimb*)Lib_SegmentedToVirtual(skeleton[i]))->segmentType != SKIN_LIMB_TYPE_ANIMATED) ||
            (((SkinLimb*)Lib_SegmentedToVirtual(skeleton[i]))->segment == NULL)) {
            vtxEntry->index = 0;

            vtxEntry->buf[0] = NULL;
            vtxEntry->buf[1] = NULL;
        } else {
            SkinAnimatedLimbData* animatedLimbData =
                Lib_SegmentedToVirtual((((SkinLimb*)Lib_SegmentedToVirtual(skeleton[i]))->segment));

            { s32 tmp; }

            vtxEntry->index = 0;
            vtxEntry->buf[0] = ZeldaArena_Malloc(animatedLimbData->totalVtxCount * sizeof(Vtx));
            vtxEntry->buf[1] = ZeldaArena_Malloc(animatedLimbData->totalVtxCount * sizeof(Vtx));

            Skin_InitAnimatedLimb(gameState, skin, i);
        }
    }

    SkelAnime_InitSkin(gameState, &skin->skelAnime, skeletonHeader, animationHeader);
}

/**
 * Frees the dynamically allocated Vtx and SkinLimbVtx buffers and tables
 */
void Skin_Free(GameState* gameState, Skin* skin) {
    if (skin->vtxTable != NULL) {
        s32 i;

        for (i = 0; i < skin->limbCount; i++) {
            if (skin->vtxTable[i].buf[0] != NULL) {
                ZeldaArena_Free(skin->vtxTable[i].buf[0]);
                skin->vtxTable[i].buf[0] = NULL;
            }
            if (skin->vtxTable[i].buf[1] != NULL) {
                ZeldaArena_Free(skin->vtxTable[i].buf[1]);
                skin->vtxTable[i].buf[1] = NULL;
            }
        }

        if (skin->vtxTable != NULL) {
            ZeldaArena_Free(skin->vtxTable);
        }

        SkelAnime_Free(&skin->skelAnime, (PlayState*)gameState);
    }
}

s32 func_801387D4(Skin* skin, SkinLimb** skeleton, MtxF* limbMatrices, u8 parentIndex, u8 limbIndex) {
    s32 pad;
    SkinLimb* limb = Lib_SegmentedToVirtual(skeleton[limbIndex]);
    MtxF* mtx;
    s32 ret;
    MtxF sp28;

    if (parentIndex == LIMB_DONE) {
        SkinMatrix_GetClear(&mtx);
    } else {
        mtx = &limbMatrices[(s32)parentIndex];
    }

    SkinMatrix_MtxFMtxFMult(mtx, &limbMatrices[limbIndex], &sp28);
    SkinMatrix_MtxFCopy(&sp28, &limbMatrices[limbIndex]);

    if (limb->child != LIMB_DONE) {
        ret = func_801387D4(skin, skeleton, limbMatrices, limbIndex, limb->child);
        if (ret) {
            return ret;
        }
    }

    if (limb->sibling != LIMB_DONE) {
        ret = func_801387D4(skin, skeleton, limbMatrices, parentIndex, limb->sibling);
        if (ret) {
            return ret;
        }
    }

    return false;
}

/**
 * Recursively applies matrix tranformations to each limb
 */
s32 Skin_ApplyAnimTransformations(Skin* skin, MtxF* limbMatrices, Actor* actor, s32 setTranslation) {
    s32 i;
    s32 pad;
    f32 yRot;
    f32 xRot;
    f32 zRot;
    s32 ret;
    f32 yTransl;
    f32 xTransl;
    f32 zTransl;
    SkinLimb** skeleton = Lib_SegmentedToVirtual(skin->skeletonHeader->segment);
    Vec3s* jointRot = skin->skelAnime.jointTable;

    jointRot++;
    xRot = jointRot->x;
    yRot = jointRot->y;
    zRot = jointRot->z;

    if (setTranslation) {
        jointRot--;
        xTransl = jointRot->x;
        yTransl = jointRot->y;
        zTransl = jointRot->z;
        jointRot++;

        SkinMatrix_SetRotateRPYTranslate(limbMatrices, xRot, yRot, zRot, xTransl, yTransl, zTransl);
    } else {
        SkinMatrix_SetRotateRPYTranslate(limbMatrices, xRot, yRot, zRot, 0.0f, 0.0f, 0.0f);
    }

    jointRot++;

    for (i = 1; i < skin->skeletonHeader->limbCount; i++) {
        SkinLimb* limb = Lib_SegmentedToVirtual(skeleton[i]);

        xTransl = limb->jointPos.x;
        yTransl = limb->jointPos.y;
        zTransl = limb->jointPos.z;
        xRot = jointRot->x;
        yRot = jointRot->y;
        zRot = jointRot->z;
        jointRot++;
        SkinMatrix_SetRotateRPYTranslate(&limbMatrices[i], xRot, yRot, zRot, xTransl, yTransl, zTransl);
    }

    SkinMatrix_SetScaleRotateYRPTranslate(
        &skin->mtx, actor->scale.x, actor->scale.y, actor->scale.z, actor->shape.rot.x, actor->shape.rot.y,
        actor->shape.rot.z, actor->world.pos.x, actor->world.pos.y + (actor->shape.yOffset * actor->scale.y),
        actor->world.pos.z);

    ret = func_801387D4(skin, Lib_SegmentedToVirtual(skin->skeletonHeader->segment), limbMatrices, LIMB_DONE, 0);
    if (!ret) {
        return ret;
    }

    return false;
}
