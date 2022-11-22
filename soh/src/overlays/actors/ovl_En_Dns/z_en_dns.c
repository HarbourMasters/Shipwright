/*
 * File: z_en_dns.c
 * Overlay: En_Dns
 * Description: Deku Salesman
 */

#include "z_en_dns.h"
#include "objects/object_shopnuts/object_shopnuts.h"
#include "vt.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnDns_Init(Actor* thisx, PlayState* play);
void EnDns_Destroy(Actor* thisx, PlayState* play);
void EnDns_Update(Actor* thisx, PlayState* play);
void EnDns_Draw(Actor* thisx, PlayState* play);

u32 EnDns_RandomizerPurchaseableCheck(EnDns* this);
u32 func_809EF5A4(EnDns* this);
u32 func_809EF658(EnDns* this);
u32 func_809EF70C(EnDns* this);
u32 func_809EF73C(EnDns* this);
u32 func_809EF800(EnDns* this);
u32 func_809EF854(EnDns* this);
u32 func_809EF8F4(EnDns* this);
u32 func_809EF9A4(EnDns* this);

void EnDns_RandomizerPurchase(EnDns* this);
void func_809EF9F8(EnDns* this);
void func_809EFA28(EnDns* this);
void func_809EFA58(EnDns* this);
void func_809EFA9C(EnDns* this);
void func_809EFACC(EnDns* this);
void func_809EFAFC(EnDns* this);
void func_809EFB40(EnDns* this);

void EnDns_SetupWait(EnDns* this, PlayState* play);
void EnDns_Wait(EnDns* this, PlayState* play);
void EnDns_Talk(EnDns* this, PlayState* play);
void func_809EFDD0(EnDns* this, PlayState* play);
void func_809EFEE8(EnDns* this, PlayState* play);
void func_809EFF50(EnDns* this, PlayState* play);
void func_809EFF98(EnDns* this, PlayState* play);
void func_809F008C(EnDns* this, PlayState* play);
void EnDns_SetupBurrow(EnDns* this, PlayState* play);
void EnDns_Burrow(EnDns* this, PlayState* play);

const ActorInit En_Dns_InitVars = {
    ACTOR_EN_DNS,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_SHOPNUTS,
    sizeof(EnDns),
    (ActorFunc)EnDns_Init,
    (ActorFunc)EnDns_Destroy,
    (ActorFunc)EnDns_Update,
    (ActorFunc)EnDns_Draw,
    NULL,
};

static ColliderCylinderInitType1 sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0xFFCFFFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_ON,
    },
    { 18, 32, 0, { 0, 0, 0 } },
};

static u16 D_809F040C[] = {
    0x10A0, 0x10A1, 0x10A2, 0x10CA, 0x10CB, 0x10CC, 0x10CD, 0x10CE, 0x10CF, 0x10DC, 0x10DD,
};

// Debug text: "sells"  { "Deku Nuts",    "Deku Sticks",        "Piece of Heart",  "Deku Seeds",
//                        "Deku Shield",  "Bombs",              "Arrows",          "Red Potion",
//                        "Green Potion", "Deku Stick Upgrade", "Deku Nut Upgrade" }
static char* D_809F0424[] = {
    "デクの実売り            ", "デクの棒売り            ", "ハートの欠片売り        ", "デクの種売り            ",
    "デクの盾売り            ", "バクダン売り            ", "矢売り                  ", "赤のくすり売り          ",
    "緑のくすり売り          ", "デクの棒持てる数を増やす", "デクの実持てる数を増やす",
};

static DnsItemEntry D_809F0450 = { 20, 5, GI_NUTS_5_2, func_809EF5A4, func_809EFA28 };

static DnsItemEntry D_809F0460 = { 15, 1, GI_STICKS_1, func_809EF658, func_809EF9F8 };

static DnsItemEntry D_809F0470 = { 10, 1, GI_HEART_PIECE, func_809EF70C, func_809EFA58 };

static DnsItemEntry D_809F0480 = { 40, 30, GI_SEEDS_30, func_809EF73C, func_809EF9F8 };

static DnsItemEntry D_809F0490 = { 50, 1, GI_SHIELD_DEKU, func_809EF800, func_809EF9F8 };

static DnsItemEntry D_809F04A0 = { 40, 5, GI_BOMBS_5, func_809EF854, func_809EFA9C };

static DnsItemEntry D_809F04B0 = { 70, 20, GI_ARROWS_LARGE, func_809EF8F4, func_809EFACC };

static DnsItemEntry D_809F04C0 = { 40, 1, GI_POTION_RED, func_809EF9A4, func_809EF9F8 };

static DnsItemEntry D_809F04D0 = { 40, 1, GI_POTION_GREEN, func_809EF9A4, func_809EF9F8 };

static DnsItemEntry D_809F04E0 = { 40, 1, GI_STICK_UPGRADE_20, func_809EF70C, func_809EFAFC };

static DnsItemEntry D_809F04F0 = { 40, 1, GI_NUT_UPGRADE_30, func_809EF70C, func_809EFB40 };

static DnsItemEntry* sItemEntries[] = {
    &D_809F0450, &D_809F0460, &D_809F0470, &D_809F0480, &D_809F0490, &D_809F04A0,
    &D_809F04B0, &D_809F04C0, &D_809F04D0, &D_809F04E0, &D_809F04F0,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_S8(naviEnemyId, 0x4E, ICHAIN_CONTINUE),
    ICHAIN_U8(targetMode, 2, ICHAIN_CONTINUE),
    ICHAIN_F32(targetArrowOffset, 30, ICHAIN_STOP),
};

typedef enum {
    /* 0 */ ENDNS_ANIM_0,
    /* 1 */ ENDNS_ANIM_1,
    /* 2 */ ENDNS_ANIM_2
} EnDnsAnimation;

static AnimationMinimalInfo sAnimationInfo[] = {
    { &gBusinessScrubNervousIdleAnim, ANIMMODE_LOOP, 0.0f },
    { &gBusinessScrubAnim_4404, ANIMMODE_ONCE, 0.0f },
    { &gBusinessScrubNervousTransitionAnim, ANIMMODE_ONCE, 0.0f },
};

void EnDns_Init(Actor* thisx, PlayState* play) {
    EnDns* this = (EnDns*)thisx;

    if (this->actor.params < 0) {
        // "Function Error (Deku Salesman)"
        osSyncPrintf(VT_FGCOL(RED) "引数エラー（売りナッツ）[ arg_data = %d ]" VT_RST "\n", this->actor.params);
        Actor_Kill(&this->actor);
        return;
    }
    // Sell Seeds instead of Arrows if Link is child
    if ((this->actor.params == 0x0006) && (LINK_AGE_IN_YEARS == YEARS_CHILD)) {
        this->actor.params = 0x0003;
    }
    // "Deku Salesman"
    osSyncPrintf(VT_FGCOL(GREEN) "◆◆◆ 売りナッツ『%s』 ◆◆◆" VT_RST "\n", D_809F0424[this->actor.params],
                 this->actor.params);
    Actor_ProcessInitChain(&this->actor, sInitChain);
    SkelAnime_InitFlex(play, &this->skelAnime, &gBusinessScrubSkel, &gBusinessScrubNervousTransitionAnim,
                       this->jointTable, this->morphTable, 18);
    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinderType1(play, &this->collider, &this->actor, &sCylinderInit);
    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawCircle, 35.0f);
    Actor_SetScale(&this->actor, 0.01f);
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;
    this->maintainCollider = 1;
    this->standOnGround = 1;
    this->dropCollectible = 0;
    this->actor.speedXZ = 0.0f;
    this->actor.velocity.y = 0.0f;
    this->actor.gravity = -1.0f;
    this->actor.textId = D_809F040C[this->actor.params];
    this->dnsItemEntry = sItemEntries[this->actor.params];
    if (gSaveContext.n64ddFlag) {
        // Ugly, but the best way we can identify which grotto we are in, same method 3DRando uses, but we'll need to account for entrance rando
        s16 respawnData = gSaveContext.respawn[RESPAWN_MODE_RETURN].data & ((1 << 8) - 1);
        this->scrubIdentity = Randomizer_IdentifyScrub(play->sceneNum, this->actor.params, respawnData);

        if ((Randomizer_GetSettingValue(RSK_SHUFFLE_SCRUBS) == 1 || Randomizer_GetSettingValue(RSK_SHUFFLE_SCRUBS) == 3) && this->scrubIdentity.itemPrice != -1) {
            this->dnsItemEntry->itemPrice = this->scrubIdentity.itemPrice;
        }

        if (this->scrubIdentity.isShuffled) {
            this->dnsItemEntry->getItemId = this->scrubIdentity.getItemId;
            this->dnsItemEntry->purchaseableCheck = EnDns_RandomizerPurchaseableCheck;
            this->dnsItemEntry->setRupeesAndFlags = EnDns_RandomizerPurchase;
            this->dnsItemEntry->itemAmount = 1;
            this->actor.textId = 0x9000 + (this->scrubIdentity.randomizerInf - RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT);
        }
    }
    this->actionFunc = EnDns_SetupWait;
}

void EnDns_Destroy(Actor* thisx, PlayState* play) {
    EnDns* this = (EnDns*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void EnDns_ChangeAnim(EnDns* this, u8 index) {
    s16 frameCount;

    frameCount = Animation_GetLastFrame(sAnimationInfo[index].animation);
    this->unk_2BA = index; // Not used anywhere else?
    Animation_Change(&this->skelAnime, sAnimationInfo[index].animation, 1.0f, 0.0f, (f32)frameCount,
                     sAnimationInfo[index].mode, sAnimationInfo[index].morphFrames);
}

/* Item give checking functions */

u32 EnDns_RandomizerPurchaseableCheck(EnDns* this) {
    if (gSaveContext.rupees < this->dnsItemEntry->itemPrice || Flags_GetRandomizerInf(this->scrubIdentity.randomizerInf)) {
        return 0;
    }
    return 4;
}

u32 func_809EF5A4(EnDns* this) {
    if ((CUR_CAPACITY(UPG_NUTS) != 0) && (AMMO(ITEM_NUT) >= CUR_CAPACITY(UPG_NUTS))) {
        return 1;
    }
    if (gSaveContext.rupees < this->dnsItemEntry->itemPrice) {
        return 0;
    }
    if (Item_CheckObtainability(ITEM_NUT) == ITEM_NONE) {
        return 2;
    }
    return 4;
}

u32 func_809EF658(EnDns* this) {
    if ((CUR_CAPACITY(UPG_STICKS) != 0) && (AMMO(ITEM_STICK) >= CUR_CAPACITY(UPG_STICKS))) {
        return 1;
    }
    if (gSaveContext.rupees < this->dnsItemEntry->itemPrice) {
        return 0;
    }
    if (Item_CheckObtainability(ITEM_STICK) == ITEM_NONE) {
        return 2;
    }
    return 4;
}

u32 func_809EF70C(EnDns* this) {
    if (gSaveContext.rupees < this->dnsItemEntry->itemPrice) {
        return 0;
    }
    return 4;
}

u32 func_809EF73C(EnDns* this) {
    if (INV_CONTENT(ITEM_SLINGSHOT) == ITEM_NONE) {
        return 3;
    }
    if (AMMO(ITEM_SLINGSHOT) >= CUR_CAPACITY(UPG_BULLET_BAG)) {
        return 1;
    }
    if (gSaveContext.rupees < this->dnsItemEntry->itemPrice) {
        return 0;
    }
    if (Item_CheckObtainability(ITEM_SEEDS) == ITEM_NONE) {
        return 2;
    }
    return 4;
}

u32 func_809EF800(EnDns* this) {
    if (gBitFlags[4] & gSaveContext.inventory.equipment) {
        return 1;
    }
    if (gSaveContext.rupees < this->dnsItemEntry->itemPrice) {
        return 0;
    }
    return 4;
}

u32 func_809EF854(EnDns* this) {
    if (!CHECK_QUEST_ITEM(QUEST_GORON_RUBY)) {
        return 3;
    }
    if (AMMO(ITEM_BOMB) >= CUR_CAPACITY(UPG_BOMB_BAG)) {
        return 1;
    }
    if (gSaveContext.rupees < this->dnsItemEntry->itemPrice) {
        return 0;
    }
    return 4;
}

u32 func_809EF8F4(EnDns* this) {
    if (Item_CheckObtainability(ITEM_BOW) == ITEM_NONE) {
        return 3;
    }
    if (AMMO(ITEM_BOW) >= CUR_CAPACITY(UPG_QUIVER)) {
        return 1;
    }
    if (gSaveContext.rupees < this->dnsItemEntry->itemPrice) {
        return 0;
    }
    return 4;
}

u32 func_809EF9A4(EnDns* this) {
    if (!Inventory_HasEmptyBottle()) {
        return 1;
    }
    if (gSaveContext.rupees < this->dnsItemEntry->itemPrice) {
        return 0;
    }
    return 4;
}

/* Paying and flagging functions */
void EnDns_RandomizerPurchase(EnDns* this) {
    Rupees_ChangeBy(-this->dnsItemEntry->itemPrice);
    Flags_SetRandomizerInf(this->scrubIdentity.randomizerInf);
}

void func_809EF9F8(EnDns* this) {
    Rupees_ChangeBy(-this->dnsItemEntry->itemPrice);
}

void func_809EFA28(EnDns* this) {
    Rupees_ChangeBy(-this->dnsItemEntry->itemPrice);
}

void func_809EFA58(EnDns* this) {
    gSaveContext.itemGetInf[0] |= 0x800;
    Rupees_ChangeBy(-this->dnsItemEntry->itemPrice);
}

void func_809EFA9C(EnDns* this) {
    Rupees_ChangeBy(-this->dnsItemEntry->itemPrice);
}

void func_809EFACC(EnDns* this) {
    Rupees_ChangeBy(-this->dnsItemEntry->itemPrice);
}

void func_809EFAFC(EnDns* this) {
    gSaveContext.infTable[25] |= 0x4;
    Rupees_ChangeBy(-this->dnsItemEntry->itemPrice);
}

void func_809EFB40(EnDns* this) {
    gSaveContext.infTable[25] |= 0x8;
    Rupees_ChangeBy(-this->dnsItemEntry->itemPrice);
}

void EnDns_SetupWait(EnDns* this, PlayState* play) {
    if (this->skelAnime.curFrame == this->skelAnime.endFrame) {
        this->actionFunc = EnDns_Wait;
        EnDns_ChangeAnim(this, ENDNS_ANIM_0);
    }
}

void EnDns_Wait(EnDns* this, PlayState* play) {
    Math_SmoothStepToS(&this->actor.shape.rot.y, this->actor.yawTowardsPlayer, 3, 2000, 0);
    this->actor.world.rot.y = this->actor.shape.rot.y;
    if (Actor_ProcessTalkRequest(&this->actor, play)) {
        this->actionFunc = EnDns_Talk;
    } else {
        if ((this->collider.base.ocFlags1 & OC1_HIT) || this->actor.isTargeted) {
            this->actor.flags |= ACTOR_FLAG_16;
        } else {
            this->actor.flags &= ~ACTOR_FLAG_16;
        }
        if (this->actor.xzDistToPlayer < 130.0f) {
            func_8002F2F4(&this->actor, play);
        }
    }
}

void EnDns_Talk(EnDns* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_CHOICE) && Message_ShouldAdvance(play)) {
        switch (play->msgCtx.choiceIndex) {
            case 0: // OK
                switch (this->dnsItemEntry->purchaseableCheck(this)) {
                    case 0:
                        Message_ContinueTextbox(play, 0x10A5);
                        this->actionFunc = func_809F008C;
                        break;
                    case 1:
                        Message_ContinueTextbox(play, 0x10A6);
                        this->actionFunc = func_809F008C;
                        break;
                    case 3:
                        Message_ContinueTextbox(play, 0x10DE);
                        this->actionFunc = func_809F008C;
                        break;
                    case 2:
                    case 4:
                        Message_ContinueTextbox(play, 0x10A7);
                        this->actionFunc = func_809EFEE8;
                        break;
                }
                break;
            case 1: // No way
                Message_ContinueTextbox(play, 0x10A4);
                this->actionFunc = func_809F008C;
        }
    }
}

void func_809EFDD0(EnDns* this, PlayState* play) {
    if (!gSaveContext.n64ddFlag || !this->scrubIdentity.isShuffled) {
        if (this->actor.params == 0x9) {
            if (CUR_UPG_VALUE(UPG_STICKS) < 2) {
                func_8002F434(&this->actor, play, GI_STICK_UPGRADE_20, 130.0f, 100.0f);
            } else {
                func_8002F434(&this->actor, play, GI_STICK_UPGRADE_30, 130.0f, 100.0f);
            }
        } else if (this->actor.params == 0xA) {
            if (CUR_UPG_VALUE(UPG_NUTS) < 2) {
                func_8002F434(&this->actor, play, GI_NUT_UPGRADE_30, 130.0f, 100.0f);
            } else {
                func_8002F434(&this->actor, play, GI_NUT_UPGRADE_40, 130.0f, 100.0f);
            }
        } else {
            func_8002F434(&this->actor, play, this->dnsItemEntry->getItemId, 130.0f, 100.0f);
        }
    } else {
        GetItemEntry itemEntry = Randomizer_GetItemFromKnownCheck(this->scrubIdentity.randomizerCheck, this->scrubIdentity.getItemId);
        GiveItemEntryFromActor(&this->actor, play, itemEntry, 130.0f, 100.0f);
    }
}

void func_809EFEE8(EnDns* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_EVENT) && Message_ShouldAdvance(play)) {
        Message_CloseTextbox(play);
        func_809EFDD0(this, play);
        this->actionFunc = func_809EFF50;
    }
}

void func_809EFF50(EnDns* this, PlayState* play) {
    if (Actor_HasParent(&this->actor, play)) {
        this->actor.parent = NULL;
        this->actionFunc = func_809EFF98;
    } else {
        func_809EFDD0(this, play);
    }
}

void func_809EFF98(EnDns* this, PlayState* play) {
    Player* player = GET_PLAYER(play);

    if (player->stateFlags1 & 0x400) {
        if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
            this->dnsItemEntry->setRupeesAndFlags(this);
            this->dropCollectible = 1;
            this->maintainCollider = 0;
            this->actor.flags &= ~ACTOR_FLAG_0;
            EnDns_ChangeAnim(this, ENDNS_ANIM_1);
            this->actionFunc = EnDns_SetupBurrow;
        }
    } else {
        this->dnsItemEntry->setRupeesAndFlags(this);
        this->dropCollectible = 1;
        this->maintainCollider = 0;
        this->actor.flags &= ~ACTOR_FLAG_0;
        EnDns_ChangeAnim(this, ENDNS_ANIM_1);
        this->actionFunc = EnDns_SetupBurrow;
    }
}

void func_809F008C(EnDns* this, PlayState* play) {
    if ((Message_GetState(&play->msgCtx) == TEXT_STATE_DONE) && Message_ShouldAdvance(play)) {
        this->maintainCollider = 0;
        this->actor.flags &= ~ACTOR_FLAG_0;
        EnDns_ChangeAnim(this, ENDNS_ANIM_1);
        this->actionFunc = EnDns_SetupBurrow;
    }
}

void EnDns_SetupBurrow(EnDns* this, PlayState* play) {
    f32 frameCount = Animation_GetLastFrame(&gBusinessScrubAnim_4404);

    if (this->skelAnime.curFrame == frameCount) {
        Audio_PlayActorSound2(&this->actor, NA_SE_EN_AKINDONUTS_HIDE);
        this->actionFunc = EnDns_Burrow;
        this->standOnGround = 0;
        this->yInitPos = this->actor.world.pos.y;
    }
}

void EnDns_Burrow(EnDns* this, PlayState* play) {
    f32 depth;
    Vec3f initPos;
    s32 i;

    depth = this->yInitPos - this->actor.world.pos.y;
    if ((this->dustTimer & 3) == 0) {
        initPos.x = this->actor.world.pos.x;
        initPos.y = this->yInitPos;
        initPos.z = this->actor.world.pos.z;
        func_80028990(play, 20.0f, &initPos);
    }
    this->actor.shape.rot.y += 0x2000;
    // Drops only if you bought its item
    if (depth > 400.0f) {
        if (this->dropCollectible) {
            initPos.x = this->actor.world.pos.x;
            initPos.y = this->yInitPos;
            initPos.z = this->actor.world.pos.z;
            for (i = 0; i < 3; i++) {
                Item_DropCollectible(play, &initPos, ITEM00_HEART);
            }
        }
        Actor_Kill(&this->actor);
    }
}

void EnDns_Update(Actor* thisx, PlayState* play) {
    EnDns* this = (EnDns*)thisx;
    s16 pad;

    this->dustTimer++;
    this->actor.textId = D_809F040C[this->actor.params];
    if (gSaveContext.n64ddFlag && this->scrubIdentity.isShuffled) {
        this->actor.textId = 0x9000 + (this->scrubIdentity.randomizerInf - RAND_INF_SCRUBS_PURCHASED_DODONGOS_CAVERN_DEKU_SCRUB_NEAR_BOMB_BAG_LEFT);
    }
    Actor_SetFocus(&this->actor, 60.0f);
    Actor_SetScale(&this->actor, 0.01f);
    SkelAnime_Update(&this->skelAnime);
    Actor_MoveForward(&this->actor);
    this->actionFunc(this, play);
    if (this->standOnGround) {
        Actor_UpdateBgCheckInfo(play, &this->actor, 20.0f, 20.0f, 20.0f, 4);
    }
    if (this->maintainCollider) {
        Collider_UpdateCylinder(&this->actor, &this->collider);
        CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    }
}

void EnDns_Draw(Actor* thisx, PlayState* play) {
    EnDns* this = (EnDns*)thisx;

    func_80093D18(play->state.gfxCtx);
    SkelAnime_DrawFlexOpa(play, this->skelAnime.skeleton, this->skelAnime.jointTable, this->skelAnime.dListCount,
                          NULL, NULL, &this->actor);
}
