#include "ModdedItems.h"
#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
#include "textures/parameter_static/parameter_static.h"
#include "textures/item_name_static/item_name_static.h"
#include "functions.h"
#include "z64save.h"
#include "variables.h"
#include "objects/gameplay_keep/gameplay_keep.h"

extern "C" {
	void func_80838940(Player* player, LinkAnimationHeader* anim, f32 arg2, PlayState* play, u16 sfxId);
}

void ModdedItems_ExecuteModdedItemAction(PlayState* play, Player* player, s32 modId, s32 itemId) {
    //for testing purposes
    if (modId == 2 && itemId == 0) {
        player->linearVelocity = 5.0f;
        player->actor.velocity.y = 8.0f;
        player->actor.world.rot.y = player->currentYaw = player->actor.shape.rot.y;

        func_80838940(player, (LinkAnimationHeader*)gPlayerAnim_link_fighter_backturn_jump, !(2 & 1) ? 5.8f : 3.5f, play, /* NA_SE_VO_LI_SWORD_N*/ 0);

        Vec3f effectsPos = player->actor.home.pos;
        effectsPos.y += 3;
        f32 effectsScale = 1;
        if (!gSaveContext.linkAge) {
            effectsScale = 1.5f;
        }
        EffectSsGRipple_Spawn(play, &effectsPos, 200 * effectsScale, 300 * effectsScale, 1);
        EffectSsGSplash_Spawn(play, &effectsPos, NULL, NULL, 0, 150 * effectsScale);

        player->stateFlags2 &= ~(PLAYER_STATE2_HOPPING);

        func_8002F7DC(&player->actor, NA_SE_PL_SKIP);
    }
}

void* ModdedItems_GetModdedItemIcon(s32 modId, s32 itemId) {
    //for testing purposes
	if (modId == 2 && itemId == 0) {
		return (void*)gItemIconGiantsWalletTex;
	}

	//in case the item is not found
	return (void*)gItemIconDekuStickTex;
}

const char* ModdedItems_GetModdedItemNameTexture(s32 modId, s32 itemId, s32 language) {
    //for testing purposes
	if (modId == 2 && itemId == 0) {
		return gBombItemNameENGTex;
	}

	//in case the item is not found
	return gDekuStickItemNameENGTex;
}