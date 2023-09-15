#include "global.h"
#include "objects/gameplay_keep/gameplay_keep.h"

#include "soh/frame_interpolation.h"

// original name: "spark"
void EffectSpark_Init(void* thisx, void* initParamsx) {
    EffectSpark* this = (EffectSpark*)thisx;
    EffectSparkInit* initParams = (EffectSparkInit*)initParamsx;
    EffectSparkElement* elem;
    f32 velocityNorm;
    s32 i;

    if ((this != NULL) && (initParams != NULL)) {
        if ((initParams->uDiv == 0) || (initParams->vDiv == 0)) {
            osSyncPrintf("spark():u_div,v_div 0では困る。\n"); // "u_div,v_div 0 is not good."
            return;
        }

        this->position = initParams->position;
        this->speed = initParams->speed;
        this->gravity = initParams->gravity;
        this->uDiv = initParams->uDiv;
        this->vDiv = initParams->vDiv;
        this->colorStart[0].r = initParams->colorStart[0].r;
        this->colorStart[0].g = initParams->colorStart[0].g;
        this->colorStart[0].b = initParams->colorStart[0].b;
        this->colorStart[0].a = initParams->colorStart[0].a;
        this->colorStart[1].r = initParams->colorStart[1].r;
        this->colorStart[1].g = initParams->colorStart[1].g;
        this->colorStart[1].b = initParams->colorStart[1].b;
        this->colorStart[1].a = initParams->colorStart[1].a;
        this->colorStart[2].r = initParams->colorStart[2].r;
        this->colorStart[2].g = initParams->colorStart[2].g;
        this->colorStart[2].b = initParams->colorStart[2].b;
        this->colorStart[2].a = initParams->colorStart[2].a;
        this->colorStart[3].r = initParams->colorStart[3].r;
        this->colorStart[3].g = initParams->colorStart[3].g;
        this->colorStart[3].b = initParams->colorStart[3].b;
        this->colorStart[3].a = initParams->colorStart[3].a;
        this->colorEnd[0].r = initParams->colorEnd[0].r;
        this->colorEnd[0].g = initParams->colorEnd[0].g;
        this->colorEnd[0].b = initParams->colorEnd[0].b;
        this->colorEnd[0].a = initParams->colorEnd[0].a;
        this->colorEnd[1].r = initParams->colorEnd[1].r;
        this->colorEnd[1].g = initParams->colorEnd[1].g;
        this->colorEnd[1].b = initParams->colorEnd[1].b;
        this->colorEnd[1].a = initParams->colorEnd[1].a;
        this->colorEnd[2].r = initParams->colorEnd[2].r;
        this->colorEnd[2].g = initParams->colorEnd[2].g;
        this->colorEnd[2].b = initParams->colorEnd[2].b;
        this->colorEnd[2].a = initParams->colorEnd[2].a;
        this->colorEnd[3].r = initParams->colorEnd[3].r;
        this->colorEnd[3].g = initParams->colorEnd[3].g;
        this->colorEnd[3].b = initParams->colorEnd[3].b;
        this->colorEnd[3].a = initParams->colorEnd[3].a;
        this->duration = initParams->duration;

        this->numElements = (this->uDiv * this->vDiv) + 2;
        if (this->numElements > ARRAY_COUNT(this->elements)) {
            osSyncPrintf("table_sizeオーバー\n"); // "over table_size"
            return;
        }

        for (i = 0; i < this->numElements; i++) {
            elem = &this->elements[i];

            elem->position.x = this->position.x;
            elem->position.y = this->position.y;
            elem->position.z = this->position.z;
            elem->velocity.x = Rand_ZeroOne() - 0.5f;
            elem->velocity.y = Rand_ZeroOne() - 0.5f;
            elem->velocity.z = Rand_ZeroOne() - 0.5f;

            velocityNorm = sqrtf(SQ(elem->velocity.x) + SQ(elem->velocity.y) + SQ(elem->velocity.z));

            if (!(fabsf(velocityNorm) < 0.008f)) {
                elem->velocity.x *= this->speed * (1.0f / velocityNorm);
                elem->velocity.y *= this->speed * (1.0f / velocityNorm);
                elem->velocity.z *= this->speed * (1.0f / velocityNorm);
            } else {
                elem->velocity.x = elem->velocity.z = 0.0f;
                elem->velocity.y = this->speed;
            }

            elem->unkVelocity.x = 30000.0f - Rand_ZeroOne() * 15000.0f;
            elem->unkVelocity.y = 30000.0f - Rand_ZeroOne() * 15000.0f;
            elem->unkVelocity.z = 30000.0f - Rand_ZeroOne() * 15000.0f;
            elem->unkPosition.x = Rand_ZeroOne() * 65534.0f;
            elem->unkPosition.y = Rand_ZeroOne() * 65534.0f;
            elem->unkPosition.z = Rand_ZeroOne() * 65534.0f;
            elem->epoch++;
        }

        this->timer = 0;
    }
}

void EffectSpark_Destroy(void* thisx) {
}

// original name: "EffectSparkInfo_proc"
s32 EffectSpark_Update(void* thisx) {
    EffectSpark* this = (EffectSpark*)thisx;
    EffectSparkElement* elem;
    s32 i;

    if (this == NULL) {
        osSyncPrintf("EffectSparkInfo_proc():Spark Pointer is NULL\n");
    }

    for (i = 0; i < this->numElements; i++) {
        elem = &this->elements[i];

        elem->position.x += elem->velocity.x;
        elem->position.y += elem->velocity.y;
        elem->position.z += elem->velocity.z;
        elem->velocity.y += this->gravity;
        elem->unkPosition.x += elem->unkVelocity.x;
        elem->unkPosition.y += elem->unkVelocity.y;
        elem->unkPosition.z += elem->unkVelocity.z;
    }

    this->timer++;

    if (this->duration < this->timer) {
        return 1;
    } else {
        return 0;
    }
}

// original name: "EffectSparkInfo_disp"
void EffectSpark_Draw(void* thisx, GraphicsContext* gfxCtx) {
    Vtx* vertices;
    EffectSpark* this = (EffectSpark*)thisx;
    PlayState* play = Effect_GetPlayState();
    s32 i;
    s32 j;
    u8 sp1D3;
    u8 sp1D2;
    u8 sp1D1;
    u8 sp1D0;
    u8 sp1CF;
    u8 sp1CE;
    u8 sp1CD;
    u8 sp1CC;
    u8 sp1CB;
    u8 sp1CA;
    u8 sp1C9;
    u8 sp1C8;
    u8 sp1C7;
    u8 sp1C6;
    u8 sp1C5;
    u8 sp1C4;
    f32 ratio;

    FrameInterpolation_RecordOpenChild(this, 0);
    OPEN_DISPS(gfxCtx);

    if (this != NULL) {
        gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        POLY_XLU_DISP = Gfx_SetupDL(POLY_XLU_DISP, 0x26);
        gDPSetCycleType(POLY_XLU_DISP++, G_CYC_2CYCLE);
        gDPPipeSync(POLY_XLU_DISP++);

        gSPTexture(POLY_XLU_DISP++, 0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON);
        gDPLoadTextureBlock(POLY_XLU_DISP++, gUnknownCircle6Tex, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD);

        gDPSetCombineMode(POLY_XLU_DISP++, G_CC_SHADEDECALA, G_CC_PASS2);
        gDPSetRenderMode(POLY_XLU_DISP++, G_RM_PASS, G_RM_ZB_CLD_SURF2);
        gSPClearGeometryMode(POLY_XLU_DISP++, G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);
        gSPSetGeometryMode(POLY_XLU_DISP++, G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH);
        gDPPipeSync(POLY_XLU_DISP++);

        vertices = Graph_Alloc(gfxCtx, this->numElements * sizeof(Vtx[4]));
        if (vertices == NULL) {
            // "Memory Allocation Failure graph_malloc"
            osSyncPrintf("EffectSparkInfo_disp():メモリー確保失敗 graph_malloc\n");
            goto end;
        }

        j = 0;

        ratio = (f32)this->timer / (f32)this->duration;
        sp1D3 = this->colorStart[0].r + ((f32)this->colorEnd[0].r - (f32)this->colorStart[0].r) * ratio;
        sp1D2 = this->colorStart[0].g + ((f32)this->colorEnd[0].g - (f32)this->colorStart[0].g) * ratio;
        sp1D1 = this->colorStart[0].b + ((f32)this->colorEnd[0].b - (f32)this->colorStart[0].b) * ratio;
        sp1D0 = this->colorStart[0].a + ((f32)this->colorEnd[0].a - (f32)this->colorStart[0].a) * ratio;
        sp1CF = this->colorStart[1].r + ((f32)this->colorEnd[1].r - (f32)this->colorStart[1].r) * ratio;
        sp1CE = this->colorStart[1].g + ((f32)this->colorEnd[1].g - (f32)this->colorStart[1].g) * ratio;
        sp1CD = this->colorStart[1].b + ((f32)this->colorEnd[1].b - (f32)this->colorStart[1].b) * ratio;
        sp1CC = this->colorStart[1].a + ((f32)this->colorEnd[1].a - (f32)this->colorStart[1].a) * ratio;
        sp1CB = this->colorStart[2].r + ((f32)this->colorEnd[2].r - (f32)this->colorStart[2].r) * ratio;
        sp1CA = this->colorStart[2].g + ((f32)this->colorEnd[2].g - (f32)this->colorStart[2].g) * ratio;
        sp1C9 = this->colorStart[2].b + ((f32)this->colorEnd[2].b - (f32)this->colorStart[2].b) * ratio;
        sp1C8 = this->colorStart[2].a + ((f32)this->colorEnd[2].a - (f32)this->colorStart[2].a) * ratio;
        sp1C7 = this->colorStart[3].r + ((f32)this->colorEnd[3].r - (f32)this->colorStart[3].r) * ratio;
        sp1C6 = this->colorStart[3].g + ((f32)this->colorEnd[3].g - (f32)this->colorStart[3].g) * ratio;
        sp1C5 = this->colorStart[3].b + ((f32)this->colorEnd[3].b - (f32)this->colorStart[3].b) * ratio;
        sp1C4 = this->colorStart[3].a + ((f32)this->colorEnd[3].a - (f32)this->colorStart[3].a) * ratio;

        for (i = 0; i < this->numElements; i++) {
            MtxF sp12C;
            MtxF spEC;
            MtxF spAC;
            MtxF sp6C;
            EffectSparkElement* elem = &this->elements[i];
            Mtx* mtx;
            f32 temp;

            FrameInterpolation_RecordOpenChild(elem, elem->epoch);

            SkinMatrix_SetTranslate(&spEC, elem->position.x, elem->position.y, elem->position.z);
            temp = ((Rand_ZeroOne() * 2.5f) + 1.5f) / 64.0f;
            SkinMatrix_SetScale(&spAC, temp, temp, 1.0f);
            SkinMatrix_MtxFMtxFMult(&spEC, &play->billboardMtxF, &sp6C);
            SkinMatrix_MtxFMtxFMult(&sp6C, &spAC, &sp12C);

            vertices[j].v.ob[0] = -32;
            vertices[j].v.ob[1] = -32;
            vertices[j].v.ob[2] = 0;
            vertices[j].v.cn[0] = sp1D3;
            vertices[j].v.cn[1] = sp1D2;
            vertices[j].v.cn[2] = sp1D1;
            vertices[j].v.cn[3] = sp1D0;
            vertices[j].v.tc[0] = 0;
            vertices[j].v.tc[1] = 1024;
            vertices[j].v.flag = 0;

            vertices[j + 1].v.ob[0] = 32;
            vertices[j + 1].v.ob[1] = 32;
            vertices[j + 1].v.ob[2] = 0;
            vertices[j + 1].v.cn[0] = sp1CF;
            vertices[j + 1].v.cn[1] = sp1CE;
            vertices[j + 1].v.cn[2] = sp1CD;
            vertices[j + 1].v.cn[3] = sp1CC;
            vertices[j + 1].v.tc[0] = 1024;
            vertices[j + 1].v.tc[1] = 0;
            vertices[j + 1].v.flag = 0;

            vertices[j + 2].v.ob[0] = -32;
            vertices[j + 2].v.ob[1] = 32;
            vertices[j + 2].v.ob[2] = 0;
            vertices[j + 2].v.cn[0] = sp1CB;
            vertices[j + 2].v.cn[1] = sp1CA;
            vertices[j + 2].v.cn[2] = sp1C9;
            vertices[j + 2].v.cn[3] = sp1C8;
            vertices[j + 2].v.tc[0] = 0;
            vertices[j + 2].v.tc[1] = 0;
            vertices[j + 2].v.flag = 0;

            vertices[j + 3].v.ob[0] = 32;
            vertices[j + 3].v.ob[1] = -32;
            vertices[j + 3].v.ob[2] = 0;
            vertices[j + 3].v.cn[0] = sp1C7;
            vertices[j + 3].v.cn[1] = sp1C6;
            vertices[j + 3].v.cn[2] = sp1C5;
            vertices[j + 3].v.cn[3] = sp1C4;
            vertices[j + 3].v.tc[0] = 1024;
            vertices[j + 3].v.tc[1] = 1024;
            vertices[j + 3].v.flag = 0;

            j += 4;

            mtx = SkinMatrix_MtxFToNewMtx(gfxCtx, &sp12C);
            if (mtx == NULL) {
                FrameInterpolation_RecordCloseChild();
                goto end;
            }

            gSPMatrix(POLY_XLU_DISP++, mtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPVertex(POLY_XLU_DISP++, &vertices[4 * i], 4, 0);
            gSP2Triangles(POLY_XLU_DISP++, 2, 0, 3, 0, 2, 3, 1, 0);
        }

        gDPPipeSync(POLY_XLU_DISP++);

        FrameInterpolation_RecordCloseChild();
    }

end:
    CLOSE_DISPS(gfxCtx);
    FrameInterpolation_RecordCloseChild();
}
