#include "z64eff_tire_mark.h"

#include "global.h"
#include "objects/gameplay_keep/gameplay_keep.h"

void func_800AE930(CollisionContext* colCtx, EffectTireMark* this, Vec3f* pos, f32 arg3, s16 angle,
                   CollisionPoly* colPoly, s32 bgId) {
    Vec3s spB8;
    Vec3s spB0;
    EffectTireMarkElement* spAC;
    EffectTireMarkElement* spA8;
    u32 spA4;
    u32 spA0;
    Vec3s* vtxList = colCtx->colHeader->vtxList;

    if ((bgId != BGCHECK_SCENE) || (this->numElements >= (ARRAY_COUNT(this->elements) - 1)) || (colPoly == NULL)) {
        func_800AEF44(this);
        return;
    }

    spB8.x = (Math_SinS(angle - 0x4000) * arg3) + pos->x;
    spB8.z = (Math_CosS(angle - 0x4000) * arg3) + pos->z;
    spB8.y = func_800BFD84(colPoly, spB8.x, spB8.z) + 2.0f;

    spB0.x = (Math_SinS(angle + 0x4000) * arg3) + pos->x;
    spB0.z = (Math_CosS(angle + 0x4000) * arg3) + pos->z;
    spB0.y = func_800BFD84(colPoly, spB0.x, spB0.z) + 2.0f;

    spAC = &this->elements[this->numElements - 1];

    if ((this->numElements != 0) && (spAC->colPoly != NULL) && (colPoly != spAC->colPoly) &&
        (func_800BFDEC(spAC->colPoly, colPoly, &spA4, &spA0) == 2)) {
        Vec3f sp90;
        Vec3f sp84;
        Vec3f sp78;
        Vec3f sp6C;
        Vec3f sp60;
        Vec3f sp54;
        Vec3f sp48;
        Vec3f sp3C;
        Vec3f sp30;

        sp84.x = vtxList[spA4].x;
        sp84.y = vtxList[spA4].y;
        sp84.z = vtxList[spA4].z;
        sp90.x = vtxList[spA0].x;
        sp90.y = vtxList[spA0].y;
        sp90.z = vtxList[spA0].z;
        sp54.x = spAC->p1.x;
        sp54.y = spAC->p1.y;
        sp54.z = spAC->p1.z;
        sp60.x = spB0.x;
        sp60.y = spB0.y;
        sp60.z = spB0.z;
        sp3C.x = spAC->p2.x;
        sp3C.y = spAC->p2.y;
        sp3C.z = spAC->p2.z;
        sp48.x = spB8.x;
        sp48.y = spB8.y;
        sp48.z = spB8.z;

        if (Math3D_LineSegMakePerpLineSeg(&sp84, &sp90, &sp54, &sp60, &sp6C, &sp30) &&
            Math3D_LineSegMakePerpLineSeg(&sp84, &sp90, &sp3C, &sp48, &sp78, &sp30)) {
            if (!(spAC->flags & EFFECT_TIRE_MARK_ELEMENT_FLAG_2)) {
                spAC->flags |= EFFECT_TIRE_MARK_ELEMENT_FLAG_1;
            }

            spA8 = &this->elements[this->numElements];
            spA8->flags = 0;
            spA8->p1.x = sp6C.x;
            spA8->p1.y = sp6C.y;
            spA8->p1.z = sp6C.z;
            spA8->p2.x = sp78.x;
            spA8->p2.y = sp78.y;
            spA8->p2.z = sp78.z;
            spA8->life = this->elemDuration;

            //! FAKE:
            if (1) {}

            spA8->colPoly = NULL;
            this->numElements++;

            spAC = &this->elements[this->numElements - 1];
            if (!(spAC->flags & EFFECT_TIRE_MARK_ELEMENT_FLAG_2)) {
                spAC->flags |= EFFECT_TIRE_MARK_ELEMENT_FLAG_1;
            }

            spA8 = &this->elements[this->numElements];
            spA8->flags = 0;
            spA8->p1 = spB0;
            spA8->p2 = spB8;
            spA8->life = this->elemDuration;
            spA8->colPoly = colPoly;
            this->numElements++;
            return;
        }

        if (!(spAC->flags & EFFECT_TIRE_MARK_ELEMENT_FLAG_2)) {
            spAC->flags |= EFFECT_TIRE_MARK_ELEMENT_FLAG_1;
        }

        spA8 = &this->elements[this->numElements];
        spA8->flags = 0;
        spA8->p1 = spB0;
        spA8->p2 = spB8;
        spA8->life = this->elemDuration;
        spA8->colPoly = colPoly;

        this->numElements++;
        return;
    }

    if (!(spAC->flags & EFFECT_TIRE_MARK_ELEMENT_FLAG_2)) {
        spAC->flags |= EFFECT_TIRE_MARK_ELEMENT_FLAG_1;
    }

    spA8 = &this->elements[this->numElements];
    spA8->flags = 0;
    spA8->p1 = spB0;
    spA8->p2 = spB8;
    spA8->life = this->elemDuration;
    spA8->colPoly = colPoly;

    this->numElements++;
}

void func_800AEF44(EffectTireMark* this) {
    EffectTireMarkElement* elem = &this->elements[this->numElements - 1];

    elem->flags |= EFFECT_TIRE_MARK_ELEMENT_FLAG_2;
}

void EffectTireMark_InitElement(EffectTireMarkElement* elem) {
    elem->flags = 0;
    elem->life = 0;
    elem->colPoly = NULL;
    elem->p1.x = elem->p1.y = elem->p1.z = elem->p2.x = elem->p2.y = elem->p2.z = 0;
}

void EffectTireMark_Init(void* thisx, void* initParamsx) {
    EffectTireMark* this = (EffectTireMark*)thisx;
    EffectTireMarkInit* initParams = (EffectTireMarkInit*)initParamsx;
    s32 i;

    for (i = 0; i < ARRAY_COUNT(this->elements); i++) {
        EffectTireMark_InitElement(&this->elements[i]);
    }

    this->unk600 = initParams->unk0;
    this->numElements = 0;

    if (initParams->elemDuration > 62) {
        this->elemDuration = 62;
    } else {
        this->elemDuration = initParams->elemDuration;
    }

    this->color = initParams->color;
}

void EffectTireMark_Destroy(void* thisx) {
}

s32 EffectTireMark_Update(void* thisx) {
    EffectTireMark* this = (EffectTireMark*)thisx;
    s32 i;
    s32 j;

    if (this->numElements == 0) {
        return 0;
    }

    for (i = 0; i < this->numElements; i++) {
        this->elements[i].life--;
    }

    if (this->elements[0].life <= 0) {
        for (j = 0; j < ARRAY_COUNT(this->elements) - 1; j++) {
            this->elements[j] = this->elements[j + 1];
        }

        EffectTireMark_InitElement(&this->elements[j]);

        this->numElements--;
        if (this->numElements < 0) {
            this->numElements = 0;
        }
        return 0;
    }

    return 0;
}

void EffectTireMark_SetVertices(Vtx* vtx, EffectTireMarkElement* elem, s32 i, s32 alpha) {
    s32 i1 = i * 2;
    s32 i2 = i * 2 + 1;

    vtx[i1].v.ob[0] = elem->p1.x;
    vtx[i1].v.ob[1] = elem->p1.y;
    vtx[i1].v.ob[2] = elem->p1.z;
    vtx[i1].v.tc[0] = 2048;
    vtx[i1].v.tc[1] = 0;
    vtx[i1].v.cn[0] = 255;
    vtx[i1].v.cn[1] = 255;
    vtx[i1].v.cn[2] = 255;
    vtx[i1].v.cn[3] = alpha;

    vtx[i2].v.ob[0] = elem->p2.x;
    vtx[i2].v.ob[1] = elem->p2.y;
    vtx[i2].v.ob[2] = elem->p2.z;
    vtx[i2].v.tc[0] = 2048;
    vtx[i2].v.tc[1] = 1024;
    vtx[i2].v.cn[0] = 255;
    vtx[i2].v.cn[1] = 255;
    vtx[i2].v.cn[2] = 255;
    vtx[i2].v.cn[3] = alpha;
}

void EffectTireMark_Draw(void* thisx, GraphicsContext* gfxCtx) {
    EffectTireMark* this = (EffectTireMark*)thisx;
    EffectTireMarkElement* elem;
    Vtx* vtx;
    s32 pad;
    f32 sp34;
    s32 i;

    sp34 = 1.0f / this->elemDuration;

    OPEN_DISPS(gfxCtx);

    if (this->numElements >= 2) {
        vtx = GRAPH_ALLOC(gfxCtx, this->numElements * (2 * sizeof(Vtx)));

        if (vtx != NULL) {
            gSPMatrix(POLY_OPA_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

            POLY_OPA_DISP = Gfx_SetupDL(POLY_OPA_DISP++, SETUPDL_44);
            gDPSetRenderMode(POLY_OPA_DISP++, G_RM_PASS, G_RM_ZB_CLD_SURF2);

            gDPLoadTextureBlock(POLY_OPA_DISP++, gameplay_keep_Tex_014570, G_IM_FMT_I, G_IM_SIZ_8b, 64, 32, 0,
                                G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_WRAP, 6, 5, G_TX_NOLOD, G_TX_NOLOD);

            gDPSetCombineLERP(POLY_OPA_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED, 0, 0, 0,
                              COMBINED);
            gSPSetGeometryMode(POLY_OPA_DISP++, G_SHADE | G_SHADING_SMOOTH);
            gSPClearGeometryMode(POLY_OPA_DISP++, G_CULL_BACK | G_LIGHTING);

            EffectTireMark_SetVertices(vtx, &this->elements[0], 0, 255);

            vtx[0].v.tc[0] = 0;
            vtx[0].v.tc[1] = 0;
            vtx[1].v.tc[0] = 0;
            vtx[1].v.tc[1] = 1024;

            for (i = 1; i < this->numElements; i++) {
                elem = &this->elements[i];

                EffectTireMark_SetVertices(vtx, elem, i, 255);

                if ((elem - 1)->flags & EFFECT_TIRE_MARK_ELEMENT_FLAG_1) {
                    if (!(elem->flags & EFFECT_TIRE_MARK_ELEMENT_FLAG_1)) {
                        s32 requiredScopeTemp;

                        vtx[i * 2 + 0].v.tc[0] = 0;
                        vtx[i * 2 + 0].v.tc[1] = 0;
                        vtx[i * 2 + 1].v.tc[0] = 0;
                        vtx[i * 2 + 1].v.tc[1] = 1024;
                    } else if ((i >= 2) && !((elem - 2)->flags & EFFECT_TIRE_MARK_ELEMENT_FLAG_1)) {
                        vtx[i * 2 - 2].v.tc[0] = 0;
                        vtx[i * 2 - 2].v.tc[1] = 0;
                        vtx[i * 2 - 1].v.tc[0] = 0;
                        vtx[i * 2 - 1].v.tc[1] = 1024;
                    }

                    if (i == (this->numElements - 1)) {
                        vtx[i * 2 + 0].v.tc[0] = 0;
                        vtx[i * 2 + 0].v.tc[1] = 0;
                        vtx[i * 2 + 1].v.tc[0] = 0;
                        vtx[i * 2 + 1].v.tc[1] = 1024;
                    }

                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, this->color.r, this->color.g, this->color.b,
                                    (s32)((f32)this->color.a * (f32)elem->life * sp34));
                    gSPVertex(POLY_OPA_DISP++, &vtx[i * 2 - 2], 4, 0);
                    gSP2Triangles(POLY_OPA_DISP++, 0, 1, 3, 0, 0, 3, 2, 0);
                }
            }
        }
    }

    CLOSE_DISPS(gfxCtx);
}
