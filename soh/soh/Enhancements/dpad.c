
#include "dpad.h"

#include "textures/icon_item_static/icon_item_static.h"

// Sprite/texture references.

sprite_t sprDPad = { "__OTR__dpad", 32, 32, G_IM_FMT_IA, G_IM_SIZ_16b };                        // Need dpad.otr file!!!
sprite_t sprFairyOcarina = { gFairyOcarinaIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
sprite_t sprHoverBoots = { gHoverBootsIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
sprite_t sprIronBoots = { gIronBootsIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };
sprite_t sprOcarinaOfTime = { gOcarinaofTimeIconTex, 32, 32, G_IM_FMT_RGBA, G_IM_SIZ_32b };

/**
 * @NOTE Need to use <value> - 1 for CHECK_OWNED_EQUIP vs CUR_EQUIP_VALUE (idk, some bitwise stuff I don't understand)
 * So to check if the player has Iron Boots: CHECK_OWNED_EQUIP(EQUIP_BOOTS, 1)
 * Then, to check if Iron Boots are equipped: CUR_EQUIP_VALUE(EQUIP_BOOTS) == 2
 */

/**
 * Handles the DPad shortcuts, such as toggling iron/hover boots
 * and playing the ocarina.
 */
void handle_dpad() {
    Input* p1Input = &gGlobalCtx->state.input[0];
    Player* player = GET_PLAYER(gGlobalCtx);

    if (!CAN_USE_DPAD || !DISPLAY_DPAD || CVar_GetS32("gDPadShortcuts", 0) == 0)
        return;

    // Don't allow toggling from the pause menu if DPad Support is enabled for pause/file select.
    if (gGlobalCtx->pauseCtx.state != 0 && CVar_GetS32("gDpadPauseName", 0) == 1)
        return;

    // Boots shortcuts.
    if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
        // Toggle Iron Boots.
        if (CHECK_BTN_ALL(p1Input->press.button, BTN_DLEFT) && CHECK_OWNED_EQUIP(EQUIP_BOOTS, 1)) {
            Inventory_ChangeEquipment(EQUIP_BOOTS, CUR_EQUIP_VALUE(EQUIP_BOOTS) != 2 ? 2 : 1);
            Player_SetEquipmentData(&gGlobalCtx, player);
            Audio_PlaySoundGeneral(NA_SE_PL_CHANGE_ARMS, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }

        // Toggle Hover Boots.
        if (CHECK_BTN_ALL(p1Input->press.button, BTN_DRIGHT) && CHECK_OWNED_EQUIP(EQUIP_BOOTS, 2)) {
            Inventory_ChangeEquipment(EQUIP_BOOTS, CUR_EQUIP_VALUE(EQUIP_BOOTS) != 3 ? 3 : 1);
            Player_SetEquipmentData(&gGlobalCtx, player);
            Audio_PlaySoundGeneral(NA_SE_PL_CHANGE_ARMS, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
        }
    }

    // Ocarina shortcut.
    if (CHECK_BTN_ALL(p1Input->press.button, BTN_DDOWN) && CAN_USE_OCARINA) {
        // Bad/undocumented function name :((((
        // @TODO Maybe extract this to a nicely named utility?
        func_80835F44(&gGlobalCtx, player, INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY ? ITEM_OCARINA_FAIRY : ITEM_OCARINA_TIME);
    }
}

/**
 * Draws the DPad UI.
 */
void draw_dpad() {
    if (!DISPLAY_DPAD || CVar_GetS32("gDPadShortcuts", 0) == 0)
        return;

    OPEN_DISPS(gGlobalCtx->state.gfxCtx, "dpad.c", 60);
    gDPPipeSync(OVERLAY_DISP++);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);

    uint16_t alpha = gGlobalCtx->interfaceCtx.minimapAlpha;

    if (alpha == 0xAA) {
        alpha = 0xFF;
    }

    // Draw DPad Icon.
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0xFF, 0xFF, 0xFF, alpha);
    sprite_load(&sprDPad);
    sprite_draw(&sprDPad, 271, 64, 16, 16);

    if (alpha == 0xFF && !CAN_USE_DPAD) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0xFF, 0xFF, 0xFF, 0x46);
    }

    // Draw Iron Boots
    if (CHECK_OWNED_EQUIP(EQUIP_BOOTS, 1) && LINK_AGE_IN_YEARS != YEARS_CHILD) {
        sprite_load(&sprIronBoots);

        if (CUR_EQUIP_VALUE(EQUIP_BOOTS) == 2) {
            sprite_draw(&sprIronBoots, 258, 64, 16, 16);
        } else {
            sprite_draw(&sprIronBoots, 260, 66, 12, 12);
        }
    }

    // Draw Hover Boots
    if (CHECK_OWNED_EQUIP(EQUIP_BOOTS, 2) && LINK_AGE_IN_YEARS != YEARS_CHILD) {
        sprite_load(&sprHoverBoots);

        if (CUR_EQUIP_VALUE(EQUIP_BOOTS) == 3) {
            sprite_draw(&sprHoverBoots, 283, 64, 16, 16);
        } else {
            sprite_draw(&sprHoverBoots, 285, 66, 12, 12);
        }
    }

    // Draw Ocarina
    if (INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY || INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_TIME) {
        if (alpha == 0xFF && !CAN_USE_OCARINA) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0xFF, 0xFF, 0xFF, 0x46);
        }

        sprite_load(INV_CONTENT(ITEM_OCARINA_FAIRY) == ITEM_OCARINA_FAIRY ? &sprFairyOcarina : &sprOcarinaOfTime);
        sprite_draw(&sprFairyOcarina, 273, 77, 12, 12); // Doesn't matter which sprite you use here since they're the same size.
    }

    gDPPipeSync(OVERLAY_DISP++);

    CLOSE_DISPS(gGlobalCtx->state.gfxCtx, "dpad.c", 113);
}
