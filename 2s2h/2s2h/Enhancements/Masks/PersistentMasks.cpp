#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/Enhancements/FrameInterpolation/FrameInterpolation.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "src/overlays/kaleido_scope/ovl_kaleido_scope/z_kaleido_scope.h"
#include "assets/interface/parameter_static/parameter_static.h"

void func_8082E1F0(Player* player, u16 sfxId);
void Player_DrawBunnyHood(PlayState* play);
extern const char* D_801C0B20[28];
}

#define CVAR_NAME "gEnhancements.Masks.PersistentBunnyHood.Enabled"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

static void UpdatePersistentMasksState() {
    static Vtx* persistentMasksVtx;
    static HOOK_ID beforePageDrawHook = 0;
    static HOOK_ID onPlayerPostLimbDrawHook = 0;
    GameInteractor::Instance->UnregisterGameHook<GameInteractor::BeforeKaleidoDrawPage>(beforePageDrawHook);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnPlayerPostLimbDraw>(onPlayerPostLimbDrawHook);

    if (!CVAR) {
        gSaveContext.save.shipSaveInfo.persistentBunnyHood = false;
        return;
    }

    // If the mask is equipped, unequip it
    if (gSaveContext.save.equippedMask == PLAYER_MASK_BUNNY) {
        gSaveContext.save.equippedMask = PLAYER_MASK_NONE;
        gSaveContext.save.shipSaveInfo.persistentBunnyHood = true;

        if (gPlayState != NULL) {
            Player* player = GET_PLAYER(gPlayState);
            player->prevMask = player->currentMask;
            player->currentMask = PLAYER_MASK_NONE;
        }
    }

    // If they don't have the mask, clear the state
    if (INV_CONTENT(ITEM_MASK_BUNNY) != ITEM_MASK_BUNNY) {
        gSaveContext.save.shipSaveInfo.persistentBunnyHood = false;
    }

    // This hook draws the mask on the players head when it's active and they aren't in first person
    onPlayerPostLimbDrawHook = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnPlayerPostLimbDraw>(
        PLAYER_LIMB_HEAD, [](Player* player, s32 limbIndex) {
            if (!gSaveContext.save.shipSaveInfo.persistentBunnyHood) {
                return;
            }

            // This emulates the vanilla check for if the masks should be drawn, specifically around
            // z_player.c 12923 (Player_Draw)
            if (player->stateFlags1 & PLAYER_STATE1_100000) {
                Vec3f temp;
                SkinMatrix_Vec3fMtxFMultXYZ(&gPlayState->viewProjectionMtxF, &player->actor.focus.pos, &temp);
                if (temp.z < -4.0f) {
                    return;
                }
            }

            OPEN_DISPS(gPlayState->state.gfxCtx);

            // Set back geometry modes left over from player head DL, incase another mask changed the values
            gSPLoadGeometryMode(POLY_OPA_DISP++,
                                G_ZBUFFER | G_SHADE | G_CULL_BACK | G_FOG | G_LIGHTING | G_SHADING_SMOOTH);

            Matrix_Push();
            Player_DrawBunnyHood(gPlayState);
            gSPDisplayList(POLY_OPA_DISP++,
                           (Gfx*)D_801C0B20[PLAYER_MASK_BUNNY - 1]); // D_801C0B20 is an array of mask DLs
            Matrix_Pop();

            CLOSE_DISPS(gPlayState->state.gfxCtx);
        });

    // This hook sets up the quad and draws the "active" blue border around the mask in the pause menu
    beforePageDrawHook = GameInteractor::Instance->RegisterGameHookForID<GameInteractor::BeforeKaleidoDrawPage>(
        PAUSE_MASK, [](PauseContext* _, u16 __) {
            GraphicsContext* gfxCtx = gPlayState->state.gfxCtx;
            PauseContext* pauseCtx = &gPlayState->pauseCtx;
            s16 i = 0;
            s16 j = 0;
            s16 k;

            OPEN_DISPS(gfxCtx);

            persistentMasksVtx = (Vtx*)GRAPH_ALLOC(gfxCtx, (4 * 4) * sizeof(Vtx));

            if ((pauseCtx->state == PAUSE_STATE_MAIN)) {
                s16 slot = SLOT_MASK_BUNNY - ITEM_NUM_SLOTS;
                s16 slotX = slot % MASK_GRID_COLS;
                s16 slotY = slot / MASK_GRID_COLS;
                s16 initialX = 0 - (MASK_GRID_COLS * MASK_GRID_CELL_WIDTH) / 2;
                s16 initialY = (MASK_GRID_ROWS * MASK_GRID_CELL_HEIGHT) / 2 - 6;
                s16 vtxX = (initialX + (slotX * MASK_GRID_CELL_WIDTH)) + MASK_GRID_QUAD_MARGIN;
                s16 vtxY = (initialY - (slotY * MASK_GRID_CELL_HEIGHT)) + pauseCtx->offsetY - MASK_GRID_QUAD_MARGIN;
                persistentMasksVtx[i + 0].v.ob[0] = persistentMasksVtx[i + 2].v.ob[0] =
                    vtxX + MASK_GRID_SELECTED_QUAD_MARGIN;
                persistentMasksVtx[i + 1].v.ob[0] = persistentMasksVtx[i + 3].v.ob[0] =
                    persistentMasksVtx[i + 0].v.ob[0] + MASK_GRID_SELECTED_QUAD_WIDTH;
                persistentMasksVtx[i + 0].v.ob[1] = persistentMasksVtx[i + 1].v.ob[1] =
                    vtxY - MASK_GRID_SELECTED_QUAD_MARGIN;

                persistentMasksVtx[i + 2].v.ob[1] = persistentMasksVtx[i + 3].v.ob[1] =
                    persistentMasksVtx[i + 0].v.ob[1] - MASK_GRID_SELECTED_QUAD_HEIGHT;

                persistentMasksVtx[i + 0].v.ob[2] = persistentMasksVtx[i + 1].v.ob[2] =
                    persistentMasksVtx[i + 2].v.ob[2] = persistentMasksVtx[i + 3].v.ob[2] = 0;

                persistentMasksVtx[i + 0].v.flag = persistentMasksVtx[i + 1].v.flag = persistentMasksVtx[i + 2].v.flag =
                    persistentMasksVtx[i + 3].v.flag = 0;

                persistentMasksVtx[i + 0].v.tc[0] = persistentMasksVtx[i + 0].v.tc[1] =
                    persistentMasksVtx[i + 1].v.tc[1] = persistentMasksVtx[i + 2].v.tc[0] = 0;

                persistentMasksVtx[i + 1].v.tc[0] = persistentMasksVtx[i + 2].v.tc[1] =
                    persistentMasksVtx[i + 3].v.tc[0] = persistentMasksVtx[i + 3].v.tc[1] =
                        MASK_GRID_SELECTED_QUAD_TEX_SIZE * (1 << 5);

                persistentMasksVtx[i + 0].v.cn[0] = persistentMasksVtx[i + 1].v.cn[0] =
                    persistentMasksVtx[i + 2].v.cn[0] = persistentMasksVtx[i + 3].v.cn[0] =
                        persistentMasksVtx[i + 0].v.cn[1] = persistentMasksVtx[i + 1].v.cn[1] =
                            persistentMasksVtx[i + 2].v.cn[1] = persistentMasksVtx[i + 3].v.cn[1] =
                                persistentMasksVtx[i + 0].v.cn[2] = persistentMasksVtx[i + 1].v.cn[2] =
                                    persistentMasksVtx[i + 2].v.cn[2] = persistentMasksVtx[i + 3].v.cn[2] = 255;

                persistentMasksVtx[i + 0].v.cn[3] = persistentMasksVtx[i + 1].v.cn[3] =
                    persistentMasksVtx[i + 2].v.cn[3] = persistentMasksVtx[i + 3].v.cn[3] = pauseCtx->alpha;

                if (gSaveContext.save.shipSaveInfo.persistentBunnyHood) {
                    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 100, 200, 255, 255);
                    gSPVertex(POLY_OPA_DISP++, (uintptr_t)persistentMasksVtx, 4, 0);
                    POLY_OPA_DISP = Gfx_DrawTexQuadIA8(POLY_OPA_DISP, (TexturePtr)gEquippedItemOutlineTex, 32, 32, 0);
                }
            }

            CLOSE_DISPS(gfxCtx);
        });
}

static void RegisterPersistentMasks() {
    UpdatePersistentMasksState();

    // Easiest way to handle things like loading into a different file, debug warping, etc.
    COND_HOOK(OnSceneInit, CVAR, [](s8 sceneId, s8 spawnNum) { UpdatePersistentMasksState(); });

    // Speed the player up when the bunny hood state is active
    COND_VB_SHOULD(VB_CONSIDER_BUNNY_HOOD_EQUIPPED, CVAR, {
        Player* player = va_arg(args, Player*);

        // But don't speed up if the player is non-human and controller input is being overriden for cutscenes/minigames
        // or if player is Kafei
        if (player && gSaveContext.save.shipSaveInfo.persistentBunnyHood && player->actor.id == ACTOR_PLAYER &&
            (GET_PLAYER_FORM == PLAYER_FORM_HUMAN || gPlayState->actorCtx.isOverrideInputOn == 0)) {
            *should = true;
        }
    });

    // Overrides allowing them to equip a mask while transformed
    COND_VB_SHOULD(VB_USE_ITEM_CONSIDER_LINK_HUMAN, CVAR, {
        PlayerItemAction* itemAction = va_arg(args, PlayerItemAction*);
        if (*itemAction == PLAYER_IA_MASK_BUNNY) {
            *should = true;
        }
    });

    // When they do equip the mask, prevent it and instead set our state
    COND_VB_SHOULD(VB_USE_ITEM_EQUIP_MASK, CVAR, {
        PlayerMask* maskId = va_arg(args, PlayerMask*);
        Player* player = GET_PLAYER(gPlayState);

        if (*maskId == PLAYER_MASK_BUNNY) {
            *should = false;

            gSaveContext.save.shipSaveInfo.persistentBunnyHood = !gSaveContext.save.shipSaveInfo.persistentBunnyHood;
            if (gSaveContext.save.shipSaveInfo.persistentBunnyHood) {
                func_8082E1F0(player, NA_SE_PL_CHANGE_ARMS);
            } else {
                func_8082E1F0(player, NA_SE_PL_TAKE_OUT_SHIELD);
            }
        }
    });

    // Prevent the "equipped" white border from being drawn so ours shows instead (ours was drawn before it, so it's
    // underneath)
    COND_VB_SHOULD(VB_DRAW_ITEM_EQUIPPED_OUTLINE, CVAR, {
        ItemId* itemId = va_arg(args, ItemId*);
        va_arg(args, s32); // slot
        va_arg(args, s32); // isDpad
        va_arg(args, s32); // pageIndex
        if (*itemId == ITEM_MASK_BUNNY && gSaveContext.save.shipSaveInfo.persistentBunnyHood) {
            *should = false;
        }
    });

    // Typically when you are in a transformation all masks are dimmed on the C-Buttons
    COND_VB_SHOULD(VB_ITEM_BE_RESTRICTED, CVAR, {
        ItemId* itemId = va_arg(args, ItemId*);
        if (*itemId == ITEM_MASK_BUNNY) {
            *should = false;
        }
    });

    // Override "A" press behavior on kaleido scope to toggle the mask state
    COND_VB_SHOULD(VB_KALEIDO_DISPLAY_ITEM_TEXT, CVAR, {
        ItemId itemId = (ItemId)*va_arg(args, u16*);
        if (itemId == ITEM_MASK_BUNNY) {
            *should = false;
            gSaveContext.save.shipSaveInfo.persistentBunnyHood = !gSaveContext.save.shipSaveInfo.persistentBunnyHood;
            if (gSaveContext.save.shipSaveInfo.persistentBunnyHood) {
                Audio_PlaySfx(NA_SE_SY_CAMERA_ZOOM_DOWN);
            } else {
                Audio_PlaySfx(NA_SE_SY_CAMERA_ZOOM_UP);
            }
        }
    });

    // Prevent persistent Bunny Hood from being used as an Item Action. Normally, the currently equipped mask converts
    // to PLAYER_IA_NONE
    COND_VB_SHOULD(VB_GET_ITEM_ACTION_FROM_MASK, CVAR, {
        Player* player = GET_PLAYER(gPlayState);
        PlayerItemAction playerItemAction = (PlayerItemAction)va_arg(args, int);

        if (player && gSaveContext.save.shipSaveInfo.persistentBunnyHood && playerItemAction == PLAYER_IA_MASK_BUNNY) {
            *should = true;
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterPersistentMasks, { CVAR_NAME });
