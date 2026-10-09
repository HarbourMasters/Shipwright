#include "global.h"
#include "PR/gs2dex.h"
#include "debug.h"

#include <libultraship/bridge/gfxbridge.h>

void Room_Noop(PlayState* play, Room* room, Input* input, s32 arg3) {
}

void Room_DrawNone(PlayState* play, Room* room, u32 flags) {
}

static Vec3f sZeroVec = { 0.0f, 0.0f, 0.0f };

void Room_DrawNormal(PlayState* play, Room* room, u32 flags) {
    RoomShapeNormal* roomShape;
    RoomShapeDListsEntry* entry;
    s32 i;

    OPEN_DISPS(play->state.gfxCtx);

    if (flags & ROOM_DRAW_OPA) {
        func_800BCBF4(&sZeroVec, play);
        gSPSegment(POLY_OPA_DISP++, 0x03, room->segment);
        func_8012C268(&play->state);
        gSPMatrix(POLY_OPA_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    }

    if (flags & ROOM_DRAW_XLU) {
        func_800BCC68(&sZeroVec, play);
        gSPSegment(POLY_XLU_DISP++, 0x03, room->segment);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gSPMatrix(POLY_XLU_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    }

    roomShape = &room->roomShape->normal;
    entry = Lib_SegmentedToVirtual(roomShape->entries);
    for (i = 0; i < roomShape->numEntries; i++) {
        if ((flags & ROOM_DRAW_OPA) && (entry->opa != NULL)) {
            gSPDisplayList(POLY_OPA_DISP++, entry->opa);
        }

        if ((flags & ROOM_DRAW_XLU) && (entry->xlu != NULL)) {
            gSPDisplayList(POLY_XLU_DISP++, entry->xlu);
        }

        entry++;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

typedef enum {
    /* 0 */ ROOM_CULL_DEBUG_MODE_OFF,
    /* 1 */ ROOM_CULL_DEBUG_MODE_UP_TO_TARGET,
    /* 2 */ ROOM_CULL_DEBUG_MODE_ONLY_TARGET
} RoomCullableDebugMode;

typedef struct RoomShapeCullableEntryLinked {
    /* 0x0 */ RoomShapeCullableEntry* entry;
    /* 0x4 */ f32 boundsNearZ;
    /* 0x8 */ struct RoomShapeCullableEntryLinked* prev;
    /* 0xC */ struct RoomShapeCullableEntryLinked* next;
} RoomShapeCullableEntryLinked; // size = 0x10

#define ROOM_SHAPE_CULLABLE_MAX_ENTRIES 128

void Room_DrawCullable(PlayState* play, Room* room, u32 flags) {
    RoomShapeCullable* roomShape;
    RoomShapeCullableEntry* roomShapeCullableEntry;
    RoomShapeCullableEntryLinked linkedEntriesBuffer[ROOM_SHAPE_CULLABLE_MAX_ENTRIES];
    RoomShapeCullableEntryLinked* head = NULL;
    RoomShapeCullableEntryLinked* tail = NULL;
    RoomShapeCullableEntryLinked* iter;
    Gfx* displayList;
    RoomShapeCullableEntryLinked* insert;
    f32 entryBoundsNearZ;
    s32 i;
    Vec3f pos;
    Vec3f projectedPos;
    RoomShapeCullableEntry* roomShapeCullableEntries;

    OPEN_DISPS(play->state.gfxCtx);

    if (flags & ROOM_DRAW_OPA) {
        func_800BCBF4(&sZeroVec, play);

        gSPSegment(POLY_OPA_DISP++, 0x03, room->segment);
        if (play->roomCtx.unk74 != NULL) {
            gSPSegment(POLY_OPA_DISP++, 0x06, play->roomCtx.unk74);
        }
        func_8012C268(&play->state);
        gSPMatrix(POLY_OPA_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    }

    if (flags & ROOM_DRAW_XLU) {
        func_800BCC68(&sZeroVec, play);
        gSPSegment(POLY_XLU_DISP++, 0x03, room->segment);
        if (play->roomCtx.unk74 != NULL) {
            gSPSegment(POLY_XLU_DISP++, 0x06, play->roomCtx.unk74);
        }
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gSPMatrix(POLY_XLU_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    }

    if (room->enablePosLights || (MREG(93) != 0)) {
        gSPSetGeometryMode(POLY_OPA_DISP++, G_LIGHTING_POSITIONAL);
        gSPSetGeometryMode(POLY_XLU_DISP++, G_LIGHTING_POSITIONAL);
    }

    roomShape = &room->roomShape->cullable;
    roomShapeCullableEntry = Lib_SegmentedToVirtual(roomShape->entries);
    insert = linkedEntriesBuffer;

    roomShapeCullableEntries = roomShapeCullableEntry;

    if (play->roomCtx.unk78 < 0) {
        for (i = 0; i < roomShape->numEntries; i++, roomShapeCullableEntry++) {
            if (R_ROOM_CULL_DEBUG_MODE != 0) {
                if (((R_ROOM_CULL_DEBUG_MODE == ROOM_CULL_DEBUG_MODE_UP_TO_TARGET) &&
                     (i <= R_ROOM_CULL_DEBUG_TARGET)) ||
                    ((R_ROOM_CULL_DEBUG_MODE == ROOM_CULL_DEBUG_MODE_ONLY_TARGET) && (i == R_ROOM_CULL_DEBUG_TARGET))) {
                    if (flags & ROOM_DRAW_OPA) {
                        displayList = roomShapeCullableEntry->opa;
                        if (displayList != NULL) {
                            gSPDisplayList(POLY_OPA_DISP++, displayList);
                        }
                    }

                    if (flags & ROOM_DRAW_XLU) {
                        displayList = roomShapeCullableEntry->xlu;
                        if (displayList != NULL) {
                            gSPDisplayList(POLY_XLU_DISP++, displayList);
                        }
                    }
                }
            } else {
                if (flags & ROOM_DRAW_OPA) {
                    displayList = roomShapeCullableEntry->opa;
                    if (displayList != NULL) {
                        gSPDisplayList(POLY_OPA_DISP++, displayList);
                    }
                }

                if (flags & ROOM_DRAW_XLU) {
                    displayList = roomShapeCullableEntry->xlu;
                    if (displayList != NULL) {
                        gSPDisplayList(POLY_XLU_DISP++, displayList);
                    }
                }
            }
        }
    } else {
        f32 var_fa1 = 1.0f / play->projectionMtxFDiagonal.z;
        f32 var_fv1;
        s32 var_a1;

        // Pick and sort entries by depth
        for (i = 0; i < roomShape->numEntries; i++, roomShapeCullableEntry++) {

            // Project the entry position, to get the depth it is at.
            pos.x = roomShapeCullableEntry->boundsSphereCenter.x;
            pos.y = roomShapeCullableEntry->boundsSphereCenter.y;
            pos.z = roomShapeCullableEntry->boundsSphereCenter.z;
            SkinMatrix_Vec3fMtxFMultXYZ(&play->viewProjectionMtxF, &pos, &projectedPos);

            projectedPos.z *= var_fa1;

            var_fv1 = ABS_ALT(roomShapeCullableEntry->boundsSphereRadius);

            // If the entry bounding sphere isn't fully before the rendered depth range
            if (-var_fv1 < projectedPos.z) {

                // Compute the depth of the nearest point in the entry's bounding sphere
                entryBoundsNearZ = projectedPos.z - var_fv1;

                // If the entry bounding sphere isn't fully beyond the rendered depth range
                if (entryBoundsNearZ < play->lightCtx.zFar) {

                    // This entry will be rendered
                    insert->entry = roomShapeCullableEntry;

                    if (roomShapeCullableEntry->boundsSphereRadius < 0) {
                        insert->boundsNearZ = FLT_MAX;
                    } else {
                        insert->boundsNearZ = entryBoundsNearZ;
                    }

                    // Insert into the linked list, ordered by ascending depth of the nearest point in the bounding
                    // sphere
                    iter = head;
                    if (iter == NULL) {
                        head = tail = insert;
                        insert->prev = insert->next = NULL;
                    } else {
                        do {
                            if (insert->boundsNearZ < iter->boundsNearZ) {
                                break;
                            }
                            iter = iter->next;
                        } while (iter != NULL);

                        if (iter == NULL) {
                            insert->prev = tail;
                            insert->next = NULL;
                            tail->next = insert;
                            tail = insert;
                        } else {
                            insert->prev = iter->prev;
                            if (insert->prev == NULL) {
                                head = insert;
                            } else {
                                insert->prev->next = insert;
                            }
                            iter->prev = insert;
                            insert->next = iter;
                        }
                    }

                    insert++;
                }
            }
        }

        //! FAKE: Similar trick used in OoT
        R_ROOM_CULL_NUM_ENTRIES = roomShape->numEntries & 0xFFFF & 0xFFFF & 0xFFFF;

        // Draw entries, from nearest to furthest
        i = 1;

        if (flags & ROOM_DRAW_OPA) {
            for (; head != NULL; head = head->next, i++) {
                s32 pad;

                roomShapeCullableEntry = head->entry;

                if (R_ROOM_CULL_DEBUG_MODE != ROOM_CULL_DEBUG_MODE_OFF) {
                    // Debug mode drawing

                    if (((R_ROOM_CULL_DEBUG_MODE == ROOM_CULL_DEBUG_MODE_UP_TO_TARGET) &&
                         (i <= R_ROOM_CULL_DEBUG_TARGET)) ||
                        ((R_ROOM_CULL_DEBUG_MODE == ROOM_CULL_DEBUG_MODE_ONLY_TARGET) &&
                         (i == R_ROOM_CULL_DEBUG_TARGET))) {

                        displayList = roomShapeCullableEntry->opa;
                        if (displayList != NULL) {
                            gSPDisplayList(POLY_OPA_DISP++, displayList);
                        }
                    }
                } else {
                    displayList = roomShapeCullableEntry->opa;
                    if (displayList != NULL) {
                        gSPDisplayList(POLY_OPA_DISP++, displayList);
                    }
                }
            }
        }

        if (flags & ROOM_DRAW_XLU) {
            for (; tail != NULL; tail = tail->prev) {
                f32 temp_fv0;
                f32 temp_fv1;

                roomShapeCullableEntry = tail->entry;
                displayList = roomShapeCullableEntry->xlu;

                if (displayList != NULL) {
                    if (roomShapeCullableEntry->boundsSphereRadius & 1) {

                        temp_fv0 = tail->boundsNearZ - (f32)(iREG(93) + 0xBB8);
                        temp_fv1 = iREG(94) + 0x7D0;

                        if (temp_fv0 < temp_fv1) {
                            if (temp_fv0 < 0.0f) {
                                var_a1 = 255;
                            } else {
                                var_a1 = 255 - (s32)((temp_fv0 / temp_fv1) * 255.0f);
                            }
                            gDPSetEnvColor(POLY_XLU_DISP++, 255, 255, 255, var_a1);
                            gSPDisplayList(POLY_XLU_DISP++, displayList);
                        }
                    } else {
                        gSPDisplayList(POLY_XLU_DISP++, displayList);
                    }
                }
            }
        }

        R_ROOM_CULL_USED_ENTRIES = i - 1;
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

#define ROOM_IMAGE_NODRAW_BACKGROUND (1 << 0)
#define ROOM_IMAGE_NODRAW_OPA (1 << 1)
#define ROOM_IMAGE_NODRAW_XLU (1 << 2)

void Room_DrawImageSingle(PlayState* play, Room* room, u32 flags) {
    Camera* activeCam;
    Gfx* gfx;
    RoomShapeImageSingle* roomShape;
    RoomShapeDListsEntry* entry;
    u32 isFixedCamera;
    u32 drawBackground;
    u32 drawOpa;
    u32 drawXlu;

    OPEN_DISPS(play->state.gfxCtx);

    activeCam = GET_ACTIVE_CAM(play);
    isFixedCamera = false; // Condition is inferred from OoT
    roomShape = &room->roomShape->image.single;
    entry = Lib_SegmentedToVirtual(roomShape->base.entry);
    drawBackground = (flags & ROOM_DRAW_OPA) && isFixedCamera && (roomShape->source != NULL) &&
                     !(R_ROOM_IMAGE_NODRAW_FLAGS & ROOM_IMAGE_NODRAW_BACKGROUND);
    drawOpa = (flags & ROOM_DRAW_OPA) && (entry->opa != NULL) && !(R_ROOM_IMAGE_NODRAW_FLAGS & ROOM_IMAGE_NODRAW_OPA);
    drawXlu = (flags & ROOM_DRAW_XLU) && (entry->xlu != NULL) && !(R_ROOM_IMAGE_NODRAW_FLAGS & ROOM_IMAGE_NODRAW_XLU);

    if (drawOpa || drawBackground) {
        gSPSegment(POLY_OPA_DISP++, 0x03, room->segment);

        if (drawOpa) {
            Gfx_SetupDL25_Opa(play->state.gfxCtx);
            gSPMatrix(POLY_OPA_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, entry->opa);
        }

        if (drawBackground) {
            gSPLoadUcodeL(POLY_OPA_DISP++, ucode_s2dex);

            gfx = POLY_OPA_DISP;

            {
                Vec3f quakeOffset;

                quakeOffset = Camera_GetQuakeOffset(activeCam);

                Prerender_DrawBackground2D(
                    &gfx, roomShape->source, roomShape->tlut, roomShape->width, roomShape->height, roomShape->fmt,
                    roomShape->siz, roomShape->tlutMode, roomShape->tlutCount,
                    (quakeOffset.x + quakeOffset.z) * 1.2f + quakeOffset.y * 0.6f,
                    quakeOffset.y * 2.4f + (quakeOffset.x + quakeOffset.z) * 0.3f, 1.0f, 1.0f, 0);
            }

            POLY_OPA_DISP = gfx;

            gSPLoadUcode(POLY_OPA_DISP++, SysUcode_GetUCode());
        }
    }

    if (drawXlu) {
        gSPSegment(POLY_XLU_DISP++, 0x03, room->segment);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gSPMatrix(POLY_XLU_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, entry->xlu);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

RoomShapeImageMultiBgEntry* Room_GetImageMultiBgEntry(RoomShapeImageMulti* roomShapeImageMulti, PlayState* play) {
    Camera* activeCam = GET_ACTIVE_CAM(play);
    s32 bgCamIndex = activeCam->bgCamIndex;
    s16 overrideBgCamIndex;
    Player* player;
    RoomShapeImageMultiBgEntry* bgEntry;
    s32 i;

    overrideBgCamIndex = ((BgCamFuncData*)BgCheck_GetBgCamFuncDataImpl(&play->colCtx, bgCamIndex, BGCHECK_SCENE))
                             ->roomImageOverrideBgCamIndex;
    if (overrideBgCamIndex >= 0) {
        bgCamIndex = overrideBgCamIndex;
    }

    player = GET_PLAYER(play);
    player->actor.params = (player->actor.params & 0xFF00) | bgCamIndex;

    bgEntry = Lib_SegmentedToVirtual(roomShapeImageMulti->backgrounds);
    for (i = 0; i < roomShapeImageMulti->numBackgrounds; i++) {
        if (bgEntry->bgCamIndex == bgCamIndex) {
            return bgEntry;
        }
        bgEntry++;
    }

    _dbg_hungup("../z_room.c", 849);

    return NULL;
}

void Room_DrawImageMulti(PlayState* play, Room* room, u32 flags) {
    Camera* activeCam;
    Gfx* gfx;
    RoomShapeImageMulti* roomShape;
    RoomShapeImageMultiBgEntry* bgEntry;
    RoomShapeDListsEntry* dListsEntry;
    u32 isFixedCamera;
    u32 drawBackground;
    u32 drawOpa;
    u32 drawXlu;

    OPEN_DISPS(play->state.gfxCtx);

    activeCam = GET_ACTIVE_CAM(play);
    isFixedCamera = false; // Condition is inferred from OoT
    roomShape = &room->roomShape->image.multi;
    dListsEntry = Lib_SegmentedToVirtual(roomShape->base.entry);

    bgEntry = Room_GetImageMultiBgEntry(roomShape, play);

    drawBackground = (flags & ROOM_DRAW_OPA) && isFixedCamera && (bgEntry->source != NULL) &&
                     !(R_ROOM_IMAGE_NODRAW_FLAGS & ROOM_IMAGE_NODRAW_BACKGROUND);
    drawOpa =
        (flags & ROOM_DRAW_OPA) && (dListsEntry->opa != NULL) && !(R_ROOM_IMAGE_NODRAW_FLAGS & ROOM_IMAGE_NODRAW_OPA);
    drawXlu =
        (flags & ROOM_DRAW_XLU) && (dListsEntry->xlu != NULL) && !(R_ROOM_IMAGE_NODRAW_FLAGS & ROOM_IMAGE_NODRAW_XLU);

    if (drawOpa || drawBackground) {
        gSPSegment(POLY_OPA_DISP++, 0x03, room->segment);

        if (drawOpa) {
            Gfx_SetupDL25_Opa(play->state.gfxCtx);
            gSPMatrix(POLY_OPA_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
            gSPDisplayList(POLY_OPA_DISP++, dListsEntry->opa);
        }

        if (drawBackground) {
            gSPLoadUcodeL(POLY_OPA_DISP++, ucode_s2dex);

            gfx = POLY_OPA_DISP;

            {
                Vec3f quakeOffset;

                quakeOffset = Camera_GetQuakeOffset(activeCam);

                Prerender_DrawBackground2D(&gfx, bgEntry->source, bgEntry->tlut, bgEntry->width, bgEntry->height,
                                           bgEntry->fmt, bgEntry->siz, bgEntry->tlutMode, bgEntry->tlutCount,
                                           (quakeOffset.x + quakeOffset.z) * 1.2f + quakeOffset.y * 0.6f,
                                           quakeOffset.y * 2.4f + (quakeOffset.x + quakeOffset.z) * 0.3f, 1.0f, 1.0f,
                                           0);
            }

            POLY_OPA_DISP = gfx;

            gSPLoadUcode(POLY_OPA_DISP++, SysUcode_GetUCode());
        }
    }

    if (drawXlu) {
        gSPSegment(POLY_XLU_DISP++, 0x03, room->segment);
        Gfx_SetupDL25_Xlu(play->state.gfxCtx);
        gSPMatrix(POLY_XLU_DISP++, &gIdentityMtx, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
        gSPDisplayList(POLY_XLU_DISP++, dListsEntry->xlu);
    }

    CLOSE_DISPS(play->state.gfxCtx);
}

void Room_DrawImage(PlayState* play, Room* room, u32 flags) {
    RoomShapeImageBase* roomShape = &room->roomShape->image.base;

    if (roomShape->amountType == ROOM_SHAPE_IMAGE_AMOUNT_SINGLE) {
        Room_DrawImageSingle(play, room, flags);
    } else if (roomShape->amountType == ROOM_SHAPE_IMAGE_AMOUNT_MULTI) {
        Room_DrawImageMulti(play, room, flags);
    } else {
        _dbg_hungup("../z_room.c", 965);
    }
}

void Room_Init(PlayState* play, RoomContext* roomCtx) {
    s32 i;

    roomCtx->curRoom.num = -1;
    roomCtx->curRoom.segment = NULL;
    roomCtx->unk78 = 1;
    roomCtx->unk79 = 0;
    for (i = 0; i < ARRAY_COUNT(roomCtx->unk7A); i++) {
        roomCtx->unk7A[i] = 0;
    }
}

/**
 * Allocates memory for rooms and fetches the first room that the player will spawn into.
 *
 * @return u32 size of the buffer reserved for room data
 */
size_t Room_SetupFirstRoom(PlayState* play, RoomContext* roomCtx) {
    size_t roomBufferSize = 0;
    size_t roomSize;
    s32 i;
    s32 j;
    s32 frontRoom;
    s32 backRoom;
    size_t frontRoomSize;
    size_t backRoomSize;
    size_t cumulRoomSize;
    s32 pad[2];

    // Set roomBufferSize to the largest room
    {
        RomFile* roomList = play->roomList.romFiles;

        for (i = 0; i < play->roomList.count; i++) {
            roomSize = roomList[i].vromEnd - roomList[i].vromStart;
            roomBufferSize = MAX(roomSize, roomBufferSize);
        }
    }

    // If there any rooms are connected, find their combined size and update roomBufferSize if larger
    if ((u32)play->transitionActors.count != 0) {
        RomFile* roomList = play->roomList.romFiles;
        TransitionActorEntry* transitionActor = &play->transitionActors.list[0];

        for (j = 0; j < play->transitionActors.count; j++) {
            frontRoom = transitionActor->sides[0].room;
            backRoom = transitionActor->sides[1].room;
            frontRoomSize = (frontRoom < 0) ? 0 : roomList[frontRoom].vromEnd - roomList[frontRoom].vromStart;
            backRoomSize = (backRoom < 0) ? 0 : roomList[backRoom].vromEnd - roomList[backRoom].vromStart;
            cumulRoomSize = (frontRoom != backRoom) ? frontRoomSize + backRoomSize : frontRoomSize;

            roomBufferSize = MAX(cumulRoomSize, roomBufferSize);
            transitionActor++;
        }
    }

    roomCtx->bufPtrs[0] = THA_AllocTailAlign16(&play->state.tha, roomBufferSize);
    if (roomCtx->bufPtrs[0] == NULL) {
        _dbg_hungup("../z_room.c", 1078);
    }
    roomCtx->bufPtrs[1] = (void*)((uintptr_t)roomCtx->bufPtrs[0] + roomBufferSize);
    roomCtx->activeBufPage = 0;
    roomCtx->status = 0;

    if ((gSaveContext.respawnFlag != 0) && (gSaveContext.respawnFlag != -2) && (gSaveContext.respawnFlag != -7)) {
        s32 respawnMode;

        if ((gSaveContext.respawnFlag == -8) || (gSaveContext.respawnFlag == -5) || (gSaveContext.respawnFlag == -4) ||
            ((gSaveContext.respawnFlag < 0) && (gSaveContext.respawnFlag != -1) && (gSaveContext.respawnFlag != -6))) {
            respawnMode = RESPAWN_MODE_DOWN;
        } else if (gSaveContext.respawnFlag < 0) {
            respawnMode = RESPAWN_MODE_TOP;
        } else {
            respawnMode = gSaveContext.respawnFlag - 1;
        }
        frontRoom = gSaveContext.respawn[respawnMode].roomIndex;
    } else {
        frontRoom = play->setupEntranceList[play->curSpawn].room;
    }

    // Load into a room for the first time.
    // Since curRoom was initialized to `room` = -1 and `segment` = NULL in Play_InitScene, the previous room
    // will also be initialized to the nulled state when this function completes.
    Room_RequestNewRoom(play, roomCtx, frontRoom);

    return roomBufferSize;
}

s32 OTRfunc_8009728C(PlayState* play, RoomContext* roomCtx, s32 roomNum);

/**
 * Tries to create an asynchronous request to transfer room data into memory.
 * If successful, the requested room will be loaded into memory and becomes the new current room; the room that was
 * current before becomes the previous room.
 *
 * Room_RequestNewRoom will be blocked from loading new rooms until Room_ProcessRoomRequest completes room
 * initialization.
 *
 * Calling Room_RequestNewRoom outside of Room_SetupFirstRoom will allow for two rooms being initialized simultaneously.
 * This allows an actor like ACTOR_EN_HOLL to seamlessly swap the two rooms as the player moves between them. Calling
 * Room_FinishRoomChange afterward will finalize the room swap.
 *
 * @param roomNum is the id of the room to load. roomNum must NOT be the same id as curRoom.num, since this will create
 * duplicate actor instances that cannot be cleaned up by calling Room_FinishRoomChange
 * @returns bool false if the request could not be created.
 */
s32 Room_RequestNewRoom(PlayState* play, RoomContext* roomCtx, s32 index) {
    return OTRfunc_8009728C(play, roomCtx, index);
#if 0
    if (roomCtx->status == 0) {
        size_t size;

        roomCtx->prevRoom = roomCtx->curRoom;
        roomCtx->curRoom.num = index;
        roomCtx->curRoom.segment = NULL;
        roomCtx->status = 1;

        size = play->roomList.romFiles[index].vromEnd - play->roomList.romFiles[index].vromStart;
        roomCtx->roomRequestAddr = (void*)(ALIGN16((uintptr_t)roomCtx->bufPtrs[roomCtx->activeBufPage] -
                                                   (size + 8) * roomCtx->activeBufPage - 7));

        osCreateMesgQueue(&roomCtx->loadQueue, roomCtx->loadMsg, ARRAY_COUNT(roomCtx->loadMsg));
        DmaMgr_RequestAsync(&roomCtx->dmaRequest, roomCtx->roomRequestAddr, play->roomList.romFiles[index].vromStart,
                            size, 0, &roomCtx->loadQueue, NULL);
        roomCtx->activeBufPage ^= 1;

        return true;
    }

    return 0;
#endif
}

void OTRPlay_InitScene(PlayState* play, s32 spawn);
s32 OTRfunc_800973FC(PlayState* play, RoomContext* roomCtx);

/**
 * Completes room initialization for the room requested by a call to Room_RequestNewRoom.
 * This function does not block the thread if the room data is still being transferred.
 *
 * @returns bool false if a dma transfer is in progress.
 */
s32 Room_ProcessRoomRequest(PlayState* play, RoomContext* roomCtx) {
    return OTRfunc_800973FC(play, roomCtx);
#if 0
    if (roomCtx->status == 1) {
        if (osRecvMesg(&roomCtx->loadQueue, NULL, OS_MESG_NOBLOCK) == 0) {
            roomCtx->status = 0;
            roomCtx->curRoom.segment = roomCtx->roomRequestAddr;
            gSegments[0x03] = OS_K0_TO_PHYSICAL(roomCtx->curRoom.segment);

            Scene_ExecuteCommands(play, roomCtx->curRoom.segment);
            func_80123140(play, GET_PLAYER(play));
            Actor_SpawnTransitionActors(play, &play->actorCtx);

            if (!(((play->sceneId == SCENE_IKANA) && (roomCtx->curRoom.num == 1)) ||
                  (play->sceneId == SCENE_IKNINSIDE))) {
                play->envCtx.lightSettingOverride = LIGHT_SETTING_OVERRIDE_NONE;
                play->envCtx.lightBlendOverride = LIGHT_BLEND_OVERRIDE_NONE;
            }
            func_800FEAB0();
            if (Environment_GetStormState(play) == STORM_STATE_OFF) {
                Environment_StopStormNatureAmbience(play);
            }
        } else {
            return false;
        }
    }

    return true;
#endif
}

RoomDrawHandler sRoomDrawHandlers[] = {
    Room_DrawNormal,   // ROOM_SHAPE_TYPE_NORMAL
    Room_DrawImage,    // ROOM_SHAPE_TYPE_IMAGE
    Room_DrawCullable, // ROOM_SHAPE_TYPE_CULLABLE
    Room_DrawNone,     // ROOM_SHAPE_TYPE_NONE
};

void Room_Draw(PlayState* play, Room* room, u32 flags) {
    if (room->segment != NULL) {
        gSegments[3] = OS_K0_TO_PHYSICAL(room->segment);
        sRoomDrawHandlers[room->roomShape->base.type](play, room, flags);
    }
    return;
}

/**
 * Finalizes a swap between two rooms.
 *
 * When a new room is created with Room_RequestNewRoom, the previous room and its actors remain in memory. This allows
 * an actor like ACTOR_EN_HOLL to seamlessly swap the two rooms as the player moves between them.
 */
void Room_FinishRoomChange(PlayState* play, RoomContext* roomCtx) {
    // Delete the previous room
    roomCtx->prevRoom.num = -1;
    roomCtx->prevRoom.segment = NULL;

    func_800BA798(play, &play->actorCtx);
    Actor_SpawnTransitionActors(play, &play->actorCtx);
    if (roomCtx->curRoom.num > -1) {
        Map_InitRoomData(play, roomCtx->curRoom.num);
        Map_SetAreaEntrypoint(play);
    }
    Audio_SetEnvReverb(play->roomCtx.curRoom.echo);
}
