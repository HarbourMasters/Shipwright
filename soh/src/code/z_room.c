#include "global.h"
#include "vt.h"

void func_80095AB4(PlayState* play, Room* room, u32 flags);
void func_80095D04(PlayState* play, Room* room, u32 flags);
void func_80096F6C(PlayState* play, Room* room, u32 flags);

Vec3f D_801270A0 = { 0.0f, 0.0f, 0.0f };

// unused
Gfx D_801270B0[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_ZBUFFER | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetOtherMode(G_AD_DISABLE | G_CD_MAGICSQ | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE |
                         G_TD_CLAMP | G_TP_PERSP | G_CYC_FILL | G_PM_NPRIMITIVE,
                     G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2),
    gsSPLoadGeometryMode(G_ZBUFFER | G_SHADE | G_CULL_BACK | G_LIGHTING | G_SHADING_SMOOTH),
    gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT),
    gsSPClipRatio(FRUSTRATIO_1),
    gsSPEndDisplayList(),
};

void (*sRoomDrawHandlers[])(PlayState* play, Room* room, u32 flags) = {
    func_80095AB4,
    func_80096F6C,
    func_80095D04,
};

void func_80095AA0(PlayState* play, Room* room, Input* arg2, UNK_TYPE arg3) {
}

// Room Draw Polygon Type 0
void func_80095AB4(PlayState* play, Room* room, u32 flags) {
    s32 i;
    PolygonType0* polygon0;
    PolygonDlist* polygonDlist;

    OPEN_DISPS(play->state.gfxCtx);

    if (flags & 1) {
        func_800342EC(&D_801270A0, play);
        gSPSegment(POLY_OPA_DISP++, 0x03, room->segment);
        func_80093C80(play);
        gSPMatrix(POLY_OPA_DISP++, &gMtxClear, G_MTX_MODELVIEW | G_MTX_LOAD);
    }

    if (flags & 2) {
        func_8003435C(&D_801270A0, play);
        gSPSegment(POLY_XLU_DISP++, 0x03, room->segment);
        func_80093D84(play->state.gfxCtx);
        gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_MODELVIEW | G_MTX_LOAD);
    }

    polygon0 = &room->meshHeader->polygon0;
    polygonDlist = SEGMENTED_TO_VIRTUAL(polygon0->start);
    for (i = 0; i < polygon0->num; i++) {
        if ((flags & 1) && (polygonDlist->opa != NULL)) {
            gSPDisplayList(POLY_OPA_DISP++, polygonDlist->opa);
        }

        if ((flags & 2) && (polygonDlist->xlu != NULL)) {
            gSPDisplayList(POLY_XLU_DISP++, polygonDlist->xlu);
        }

        polygonDlist++;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

#define SHAPE_SORT_MAX 64

typedef struct struct_80095D04 {
    /* 0x00 */ PolygonDlist2* unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ struct struct_80095D04* unk_08;
    /* 0x0C */ struct struct_80095D04* unk_0C;
} struct_80095D04; // size = 0x10

// Room Draw Polygon Type 2
void func_80095D04(PlayState* play, Room* room, u32 flags) {
    PolygonType2* polygon2;
    PolygonDlist2* polygonDlist;
    struct_80095D04 spB8[SHAPE_SORT_MAX];
    struct_80095D04* spB4 = NULL;
    struct_80095D04* spB0 = NULL;
    struct_80095D04* phi_v0;
    s32 pad;
    struct_80095D04* spA4;
    s32 phi_v1;
    s32 sp9C;
    Vec3f sp90;
    Vec3f sp84;
    f32 sp80;
    s32 pad2;
    PolygonDlist2* sp78;
    PolygonDlist2* temp;
    f32 temp_f2;

    OPEN_DISPS(play->state.gfxCtx);
    if (flags & 1) {
        func_800342EC(&D_801270A0, play);
        //gSPSegment(POLY_OPA_DISP++, 0x03, room->segment);
        func_80093C80(play);
        gSPMatrix(POLY_OPA_DISP++, &gMtxClear, G_MTX_MODELVIEW | G_MTX_LOAD);
    }
    if (flags & 2) {
        func_8003435C(&D_801270A0, play);
        //gSPSegment(POLY_XLU_DISP++, 0x03, room->segment);
        func_80093D84(play->state.gfxCtx);
        gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_MODELVIEW | G_MTX_LOAD);
    }

    polygon2 = &room->meshHeader->polygon2;
    polygonDlist = SEGMENTED_TO_VIRTUAL(polygon2->start);
    spA4 = spB8;

    ASSERT(polygon2->num <= SHAPE_SORT_MAX);
    sp78 = polygonDlist;

    for (sp9C = 0; sp9C < polygon2->num; sp9C++, polygonDlist++) {
        sp90.x = polygonDlist->pos.x;
        sp90.y = polygonDlist->pos.y;
        sp90.z = polygonDlist->pos.z;
        SkinMatrix_Vec3fMtxFMultXYZW(&play->viewProjectionMtxF, &sp90, &sp84, &sp80);
        if (-(f32)polygonDlist->unk_06 < sp84.z)
        {
            temp_f2 = sp84.z - polygonDlist->unk_06;
            if (temp_f2 < play->lightCtx.fogFar) {
                phi_v0 = spB4;
                spA4->unk_00 = polygonDlist;
                spA4->unk_04 = temp_f2;
                if (phi_v0 == NULL) {
                    spB4 = spB0 = spA4;
                    spA4->unk_08 = spA4->unk_0C = NULL;
                } else {
                    do {
                        if (spA4->unk_04 < phi_v0->unk_04) {
                            break;
                        }
                        phi_v0 = phi_v0->unk_0C;
                    } while (phi_v0 != NULL);

                    if (phi_v0 == NULL) {
                        spA4->unk_08 = spB0;
                        spA4->unk_0C = NULL;
                        spB0->unk_0C = spA4;
                        spB0 = spA4;
                    } else {
                        spA4->unk_08 = phi_v0->unk_08;
                        if (spA4->unk_08 == NULL) {
                            spB4 = spA4;
                        } else {
                            spA4->unk_08->unk_0C = spA4;
                        }
                        phi_v0->unk_08 = spA4;
                        spA4->unk_0C = phi_v0;
                    }
                }
                spA4++;
            }
        }
    }

    iREG(87) = polygon2->num & 0xFFFF & 0xFFFF & 0xFFFF; // if this is real then I might not be

    for (sp9C = 1; spB4 != NULL; spB4 = spB4->unk_0C, sp9C++) {
        Gfx* temp2;

        polygonDlist = spB4->unk_00;
        if (iREG(86) != 0)
        {
            temp = sp78;
            for (phi_v1 = 0; phi_v1 < polygon2->num; phi_v1++, temp++) {
                if (polygonDlist == temp) {
                    break; // This loop does nothing?
                }
            }

            if (((iREG(86) == 1) && (iREG(89) >= sp9C)) || ((iREG(86) == 2) && (iREG(89) == sp9C)))
            {
                if (flags & 1) {
                    temp2 = polygonDlist->opa;
                    if (temp2 != NULL) {
                        gSPDisplayList(POLY_OPA_DISP++, temp2);
                    }
                }

                if (flags & 2) {
                    temp2 = polygonDlist->xlu;
                    if (temp2 != NULL) {
                        gSPDisplayList(POLY_XLU_DISP++, temp2);
                    }
                }
            }
        } else {
            if (flags & 1) {
                temp2 = polygonDlist->opa;
                if (temp2 != NULL) {
                    gSPDisplayList(POLY_OPA_DISP++, temp2);
                }
            }

            if (flags & 2) {
                temp2 = polygonDlist->xlu;
                if (temp2 != NULL) {
                    gSPDisplayList(POLY_XLU_DISP++, temp2);
                }
            }
        }
    }

    iREG(88) = sp9C - 1;

    CLOSE_DISPS(play->state.gfxCtx);
}

#define JPEG_MARKER 0xFFD8FFE0

s32 func_80096238(void* data) {
    OSTime time;

    if (BE32SWAP(*(u32*)data) == JPEG_MARKER)
    {
        char* decodedJpeg = ResourceMgr_LoadJPEG(data, 320 * 240 * 2);
        //char* decodedJpeg = ResourceMgr_LoadJPEG(data, 480 * 240 * 2);

        osSyncPrintf("JPEGデータを展開します\n");        // "Expanding jpeg data"
        osSyncPrintf("JPEGデータアドレス %08x\n", data); // "Jpeg data address %08x"
        // "Work buffer address (Z buffer) %08x"
        osSyncPrintf("ワークバッファアドレス（Ｚバッファ）%08x\n", gZBuffer);

        time = osGetTime();

        //if (!Jpeg_Decode(data, gZBuffer, gGfxSPTaskOutputBuffer, sizeof(gGfxSPTaskOutputBuffer)))
        if (1)
        {
            memcpy(data, decodedJpeg, 320 * 240 * 2);
            //memcpy(data, decodedJpeg, 480 * 240 * 2);
            time = osGetTime() - time;

            // "Success... I think. time = %6.3f ms"
            osSyncPrintf("成功…だと思う。 time = %6.3f ms \n", OS_CYCLES_TO_USEC(time) / 1000.0f);
            // "Writing back to original address from work buffer."
            osSyncPrintf("ワークバッファから元のアドレスに書き戻します。\n");
            // "If the original buffer size isn't at least 150kb, it will be out of control."
            osSyncPrintf("元のバッファのサイズが150キロバイト無いと暴走するでしょう。\n");

            //bcopy(gZBuffer, data, sizeof(gZBuffer));
        } else {
            osSyncPrintf("失敗！なんで〜\n"); // "Failure! Why is it 〜"
        }
    }

    return 0;
}

void func_8009638C(Gfx** displayList, void* source, void* tlut, u16 width, u16 height, u8 fmt, u8 siz, u16 mode0,
                   u16 tlutCount, f32 frameX, f32 frameY) {
    Gfx* displayListHead;
    uObjBg* bg;
    s32 temp;

    displayListHead = *displayList;
    func_80096238(SEGMENTED_TO_VIRTUAL(source));

    bg = (uObjBg*)(displayListHead + 1);
    gSPBranchList(displayListHead, (u8*)bg + sizeof(uObjBg));
    bg->b.imageX = 0;
    bg->b.imageW = width * 4;
    bg->b.frameX = frameX * 4;
    bg->b.imageY = 0;
    bg->b.imageH = height * 4;
    bg->b.frameY = frameY * 4;
    bg->b.imagePtr = source;
    bg->b.imageLoad = G_BGLT_LOADTILE;
    bg->b.imageFmt = fmt;
    bg->b.imageSiz = siz;
    bg->b.imagePal = 0;
    bg->b.imageFlip = 0;

    displayListHead = (void*)(bg + 1);
    if (fmt == G_IM_FMT_CI) {
        gDPLoadTLUT(displayListHead++, tlutCount, 256, tlut);
    } else {
        gDPPipeSync(displayListHead++);
    }

    if ((fmt == G_IM_FMT_RGBA) && (SREG(26) == 0)) {
        bg->b.frameW = width * 4;
        bg->b.frameH = height * 4;
        guS2DInitBg(bg);

        gDPSetOtherMode(displayListHead++, mode0 | G_TL_TILE | G_TD_CLAMP | G_TP_NONE | G_CYC_FILL | G_PM_NPRIMITIVE,
                        G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2);

        gDPSetFillColor(displayListHead++, GPACK_RGBA5551(0, 0, 0, 1) << 16 | GPACK_RGBA5551(0, 0, 0, 1));
        gDPFillWideRectangle(displayListHead++, OTRGetRectDimensionFromLeftEdge(0), 0,
                         OTRGetRectDimensionFromRightEdge(SCREEN_WIDTH), SCREEN_HEIGHT);

        gDPSetOtherMode(displayListHead++, mode0 | G_TL_TILE | G_TD_CLAMP | G_TP_NONE | G_CYC_COPY | G_PM_NPRIMITIVE,
                        G_AC_THRESHOLD | G_ZS_PIXEL | G_RM_NOOP | G_RM_NOOP2);

        gDPLoadMultiTile(displayListHead++, bg->b.imagePtr, 0,
            G_TX_RENDERTILE, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, 0, 0, 0, 0 + 31,
            0 + 31, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD,
            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);

        gSPBgRectCopy(displayListHead++, bg);

    } else {
        bg->s.frameW = width * 4;
        bg->s.frameH = height * 4;
        bg->s.scaleW = 1024;
        bg->s.scaleH = 1024;
        bg->s.imageYorig = bg->b.imageY;
        gDPSetOtherMode(displayListHead++,
                        mode0 | G_AD_DISABLE | G_CD_DISABLE | G_CK_NONE | G_TC_FILT | G_TF_POINT | G_TT_NONE |
                            G_TL_TILE | G_TD_CLAMP | G_TP_NONE | G_CYC_1CYCLE | G_PM_NPRIMITIVE,
                        G_AC_THRESHOLD | G_ZS_PIXEL | AA_EN | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | ALPHA_CVG_SEL |
                            GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_BL, G_BL_1MA) |
                            GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_BL, G_BL_1MA));
        gDPSetCombineLERP(displayListHead++, 0, 0, 0, TEXEL0, 0, 0, 0, 1, 0, 0, 0, TEXEL0, 0, 0, 0, 1);
        gSPObjRenderMode(displayListHead++, G_OBJRM_ANTIALIAS | G_OBJRM_BILERP);
        gSPBgRect1Cyc(displayListHead++, bg);
    }

    gDPPipeSync(displayListHead++);
    *displayList = displayListHead;
}

// Room Draw Polygon Type 1 - Single Format
void func_80096680(PlayState* play, Room* room, u32 flags) {
    Camera* camera;
    Gfx* spA8;
    PolygonType1* polygon1;
    PolygonDlist* polygonDlist;
    u32 sp9C;
    u32 sp98;
    u32 sp94;
    u32 sp90;

    OPEN_DISPS(play->state.gfxCtx);

    camera = GET_ACTIVE_CAM(play);
    sp9C = (camera->setting == CAM_SET_PREREND_FIXED);
    polygon1 = &room->meshHeader->polygon1;
    polygonDlist = SEGMENTED_TO_VIRTUAL(polygon1->dlist);
    sp98 = (flags & 1) && sp9C && polygon1->single.source && !(SREG(25) & 1);
    sp94 = (flags & 1) && polygonDlist->opa && !(SREG(25) & 2);
    sp90 = (flags & 2) && polygonDlist->xlu && !(SREG(25) & 4);

    if (sp94 || sp98) {
        gSPSegment(POLY_OPA_DISP++, 0x03, room->segment);

        if (sp94) {
            func_80093D18(play->state.gfxCtx);
            gSPMatrix(POLY_OPA_DISP++, &gMtxClear, G_MTX_MODELVIEW | G_MTX_LOAD);
            gSPDisplayList(POLY_OPA_DISP++, polygonDlist->opa);
        }

        if (sp98) {
            // gSPLoadUcodeL(POLY_OPA_DISP++, rspS2DEX)?
            //gSPLoadUcodeEx(POLY_OPA_DISP++, OS_K0_TO_PHYSICAL(D_80113070), OS_K0_TO_PHYSICAL(D_801579A0), 0x800);

            {
                Vec3f sp60;
                spA8 = POLY_OPA_DISP;
                Camera_GetSkyboxOffset(&sp60, camera);
                func_8009638C(&spA8, polygon1->single.source, polygon1->single.tlut, polygon1->single.width,
                              polygon1->single.height, polygon1->single.fmt, polygon1->single.siz,
                              polygon1->single.mode0, polygon1->single.tlutCount,
                              (sp60.x + sp60.z) * 1.2f + sp60.y * 0.6f, sp60.y * 2.4f + (sp60.x + sp60.z) * 0.3f);
                POLY_OPA_DISP = spA8;
            }

            // gSPLoadUcode(POLY_OPA_DISP++, SysUcode_GetUCode(), SysUcode_GetUCodeData())?
            gSPLoadUcodeEx(POLY_OPA_DISP++, SysUcode_GetUCode(), SysUcode_GetUCodeData(), 0x800);
        }
    }

    if (sp90) {
        gSPSegment(POLY_XLU_DISP++, 0x03, room->segment);
        func_80093D84(play->state.gfxCtx);
        gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_MODELVIEW | G_MTX_LOAD);
        gSPDisplayList(POLY_XLU_DISP++, polygonDlist->xlu);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

BgImage* func_80096A74(PolygonType1* polygon1, PlayState* play) {
    Camera* camera;
    s32 camId;
    s16 camId2;
    Player* player;
    BgImage* bgImage;
    s32 i;

    camera = GET_ACTIVE_CAM(play);
    camId = camera->camDataIdx;
    if (camId == -1 && CVar_GetS32("gNoRestrictItems", 0)) {
        // This prevents a crash when using items that change the
        // camera (such as din's fire) on scenes with prerendered backgrounds
        return NULL;
    }
    
    // jfifid
    camId2 = func_80041C10(&play->colCtx, camId, BGCHECK_SCENE)[2].y;
    if (camId2 >= 0) {
        camId = camId2;
    }

    player = GET_PLAYER(play);
    player->actor.params = (player->actor.params & 0xFF00) | camId;

    bgImage = SEGMENTED_TO_VIRTUAL(polygon1->multi.list);
    for (i = 0; i < polygon1->multi.count; i++) {
        if (bgImage->id == camId) {
            return bgImage;
        }
        bgImage++;
    }

    // "z_room.c: Data consistent with camera id does not exist camid=%d"
    osSyncPrintf(VT_COL(RED, WHITE) "z_room.c:カメラＩＤに一致するデータが存在しません camid=%d\n" VT_RST, camId);
    LOG_HUNGUP_THREAD();

    return NULL;
}

// Room Draw Polygon Type 1 - Multi Format
void func_80096B6C(PlayState* play, Room* room, u32 flags) {
    Camera* camera;
    Gfx* spA8;
    PolygonType1* polygon1;
    BgImage* bgImage;
    PolygonDlist* polygonDlist;
    u32 sp98;
    u32 sp94;
    u32 sp90;
    u32 sp8C;

    OPEN_DISPS(play->state.gfxCtx);

    camera = GET_ACTIVE_CAM(play);
    sp98 = (camera->setting == CAM_SET_PREREND_FIXED);
    polygon1 = &room->meshHeader->polygon1;
    polygonDlist = SEGMENTED_TO_VIRTUAL(polygon1->dlist);
    bgImage = func_80096A74(polygon1, play);
    sp94 = (flags & 1) && sp98 && bgImage->source && !(SREG(25) & 1);
    sp90 = (flags & 1) && polygonDlist->opa && !(SREG(25) & 2);
    sp8C = (flags & 2) && polygonDlist->xlu && !(SREG(25) & 4);

    if (sp90 || sp94) {
        gSPSegment(POLY_OPA_DISP++, 0x03, room->segment);

        if (sp90) {
            func_80093D18(play->state.gfxCtx);
            gSPMatrix(POLY_OPA_DISP++, &gMtxClear, G_MTX_MODELVIEW | G_MTX_LOAD);
            gSPDisplayList(POLY_OPA_DISP++, polygonDlist->opa);
        }

        if (sp94) {
            // gSPLoadUcodeL(POLY_OPA_DISP++, rspS2DEX)?
            //gSPLoadUcodeEx(POLY_OPA_DISP++, OS_K0_TO_PHYSICAL(D_80113070), OS_K0_TO_PHYSICAL(D_801579A0), 0x800);

            {
                Vec3f sp5C;
                spA8 = POLY_OPA_DISP;
                Camera_GetSkyboxOffset(&sp5C, camera);
                func_8009638C(&spA8, bgImage->source, bgImage->tlut, bgImage->width, bgImage->height, bgImage->fmt,
                              bgImage->siz, bgImage->mode0, bgImage->tlutCount,
                              (sp5C.x + sp5C.z) * 1.2f + sp5C.y * 0.6f, sp5C.y * 2.4f + (sp5C.x + sp5C.z) * 0.3f);
                POLY_OPA_DISP = spA8;
            }

            // gSPLoadUcode(POLY_OPA_DISP++, SysUcode_GetUCode(), SysUcode_GetUCodeData())?
            gSPLoadUcodeEx(POLY_OPA_DISP++, SysUcode_GetUCode(), SysUcode_GetUCodeData(), 0x800);
        }
    }

    if (sp8C) {
        gSPSegment(POLY_XLU_DISP++, 0x03, room->segment);
        func_80093D84(play->state.gfxCtx);
        gSPMatrix(POLY_XLU_DISP++, &gMtxClear, G_MTX_MODELVIEW | G_MTX_LOAD);
        gSPDisplayList(POLY_XLU_DISP++, polygonDlist->xlu);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

// Room Draw Polygon Type 1
void func_80096F6C(PlayState* play, Room* room, u32 flags) {
    PolygonType1* polygon1 = &room->meshHeader->polygon1;

    if (polygon1->format == 1) {
        func_80096680(play, room, flags);
    } else if (polygon1->format == 2) {
        func_80096B6C(play, room, flags);
    } else {
        LOG_HUNGUP_THREAD();
    }
}

void func_80096FD4(PlayState* play, Room* room) {
    room->num = -1;
    room->segment = NULL;
}

u32 func_80096FE8(PlayState* play, RoomContext* roomCtx) {
    u32 maxRoomSize = 0;
    RomFile* roomList = play->roomList;
    u32 roomSize;
    s32 i;
    s32 j;
    s32 frontRoom;
    s32 backRoom;
    u32 frontRoomSize;
    u32 backRoomSize;
    u32 cumulRoomSize;

    for (i = 0; i < play->numRooms; i++) {
        roomSize = roomList[i].vromEnd - roomList[i].vromStart;
        osSyncPrintf("ROOM%d size=%d\n", i, roomSize);
        if (maxRoomSize < roomSize) {
            maxRoomSize = roomSize;
        }
    }

    if (play->transiActorCtx.numActors != 0) {
        RomFile* roomList = play->roomList;
        TransitionActorEntry* transitionActor = &play->transiActorCtx.list[0];

        LOG_NUM("game_play->room_rom_address.num", play->numRooms);

        for (j = 0; j < play->transiActorCtx.numActors; j++) {
            frontRoom = transitionActor->sides[0].room;
            backRoom = transitionActor->sides[1].room;
            frontRoomSize = (frontRoom < 0) ? 0 : roomList[frontRoom].vromEnd - roomList[frontRoom].vromStart;
            backRoomSize = (backRoom < 0) ? 0 : roomList[backRoom].vromEnd - roomList[backRoom].vromStart;
            cumulRoomSize = (frontRoom != backRoom) ? frontRoomSize + backRoomSize : frontRoomSize;

            osSyncPrintf("DOOR%d=<%d> ROOM1=<%d, %d> ROOM2=<%d, %d>\n", j, cumulRoomSize, frontRoom, frontRoomSize,
                         backRoom, backRoomSize);
            if (maxRoomSize < cumulRoomSize) {
                maxRoomSize = cumulRoomSize;
            }
            transitionActor++;
        }
    }

    osSyncPrintf(VT_FGCOL(YELLOW));
    // "Room buffer size=%08x(%5.1fK)"
    osSyncPrintf("部屋バッファサイズ=%08x(%5.1fK)\n", maxRoomSize, maxRoomSize / 1024.0f);
    roomCtx->bufPtrs[0] = GAMESTATE_ALLOC_MC(&play->state, maxRoomSize);
    // "Room buffer initial pointer=%08x"
    osSyncPrintf("部屋バッファ開始ポインタ=%08x\n", roomCtx->bufPtrs[0]);
    roomCtx->bufPtrs[1] = (void*)((intptr_t)roomCtx->bufPtrs[0] + maxRoomSize);
    // "Room buffer end pointer=%08x"
    osSyncPrintf("部屋バッファ終了ポインタ=%08x\n", roomCtx->bufPtrs[1]);
    osSyncPrintf(VT_RST);
    roomCtx->unk_30 = 0;
    roomCtx->status = 0;

    frontRoom = gSaveContext.respawnFlag > 0 ? ((void)0, gSaveContext.respawn[gSaveContext.respawnFlag - 1].roomIndex)
                                             : play->setupEntranceList[play->curSpawn].room;
    func_8009728C(play, roomCtx, frontRoom);

    return maxRoomSize;
}

s32 func_8009728C(PlayState* play, RoomContext* roomCtx, s32 roomNum) {
    size_t size;

    return OTRfunc_8009728C(play, roomCtx, roomNum);

    if (roomCtx->status == 0) {
        roomCtx->prevRoom = roomCtx->curRoom;
        roomCtx->curRoom.num = roomNum;
        roomCtx->curRoom.segment = NULL;
        roomCtx->status = 1;

        ASSERT(roomNum < play->numRooms);

        size = play->roomList[roomNum].vromEnd - play->roomList[roomNum].vromStart;
        roomCtx->unk_34 = (void*)ALIGN16((intptr_t)roomCtx->bufPtrs[roomCtx->unk_30] - ((size + 8) * roomCtx->unk_30 + 7));

        osCreateMesgQueue(&roomCtx->loadQueue, &roomCtx->loadMsg, 1);
        DmaMgr_SendRequest2(&roomCtx->dmaRequest, roomCtx->unk_34, play->roomList[roomNum].vromStart, size, 0,
                            &roomCtx->loadQueue, OS_MESG_PTR(NULL), __FILE__, __LINE__);
        roomCtx->unk_30 ^= 1;

        return 1;
    }

    return 0;
}

s32 func_800973FC(PlayState* play, RoomContext* roomCtx) {
    return OTRfunc_800973FC(play, roomCtx);


    if (roomCtx->status == 1) {
        if (!osRecvMesg(&roomCtx->loadQueue, NULL, OS_MESG_NOBLOCK)) {
            roomCtx->status = 0;
            roomCtx->curRoom.segment = roomCtx->unk_34;
            gSegments[3] = VIRTUAL_TO_PHYSICAL(roomCtx->unk_34);

            Scene_ExecuteCommands(play, roomCtx->curRoom.segment);
            Player_SetBootData(play, GET_PLAYER(play));
            Actor_SpawnTransitionActors(play, &play->actorCtx);

            return 1;
        }

        return 0;
    }

    return 1;
}

void Room_Draw(PlayState* play, Room* room, u32 flags) {
    if (room->segment != NULL)
    {
        gSegments[3] = VIRTUAL_TO_PHYSICAL(room->segment);
        ASSERT(room->meshHeader->base.type < ARRAY_COUNTU(sRoomDrawHandlers));
        sRoomDrawHandlers[room->meshHeader->base.type](play, room, flags);
    }
}

void func_80097534(PlayState* play, RoomContext* roomCtx) {
    roomCtx->prevRoom.num = -1;
    roomCtx->prevRoom.segment = NULL;
    func_80031B14(play, &play->actorCtx);
    Actor_SpawnTransitionActors(play, &play->actorCtx);
    Map_InitRoomData(play, roomCtx->curRoom.num);
    if (!((play->sceneNum >= SCENE_SPOT00) && (play->sceneNum <= SCENE_SPOT20))) {
        Map_SavePlayerInitialInfo(play);
    }
    Audio_SetEnvReverb(play->roomCtx.curRoom.echo);
}
