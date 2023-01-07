#include "vt.h"
#include "z_link_puppet.h"
#include <objects/gameplay_keep/gameplay_keep.h>
#include <string.h>

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3 | ACTOR_FLAG_4)

void LinkPuppet_Init(Actor* thisx, PlayState* play);
void LinkPuppet_Destroy(Actor* thisx, PlayState* play);
void LinkPuppet_Update(Actor* thisx, PlayState* play);
void LinkPuppet_Draw(Actor* thisx, PlayState* play);

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_HIT0,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00100000, 0x00, 0x00 },
        { 0xEE01FFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 12, 50, 0, { 0, 0, 0 } },
};

const ActorInit Link_Puppet_InitVars = {
    ACTOR_LINK_PUPPET,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_LINK_BOY,
    sizeof(LinkPuppet),
    (ActorFunc)LinkPuppet_Init,
    (ActorFunc)LinkPuppet_Destroy,
    (ActorFunc)LinkPuppet_Update,
    (ActorFunc)LinkPuppet_Draw,
    NULL,
};

static Vec3s D_80854730 = { -57, 3377, 0 };

extern func_80833338(Player* this);

typedef enum {
    PUPPET_DMGEFF_NONE,
    PUPPET_DMGEFF_NORMAL,
    PUPPET_DMGEFF_ICE,
    PUPPET_DMGEFF_FIRE,
    PUPPET_DMGEFF_THUNDER,
    PUPPET_DMGEFF_KNOCKBACK,
    PUPPET_DMGEFF_STUN,
} PuppetDamageEffect;

static DamageTable sDamageTable[] = {
    /* Deku nut      */ DMG_ENTRY(0, PUPPET_DMGEFF_STUN),
    /* Deku stick    */ DMG_ENTRY(2, PUPPET_DMGEFF_NORMAL),
    /* Slingshot     */ DMG_ENTRY(1, PUPPET_DMGEFF_NORMAL),
    /* Explosive     */ DMG_ENTRY(2, PUPPET_DMGEFF_NORMAL),
    /* Boomerang     */ DMG_ENTRY(0, PUPPET_DMGEFF_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, PUPPET_DMGEFF_NORMAL),
    /* Hammer swing  */ DMG_ENTRY(2, PUPPET_DMGEFF_KNOCKBACK),
    /* Hookshot      */ DMG_ENTRY(0, PUPPET_DMGEFF_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, PUPPET_DMGEFF_NORMAL),
    /* Master sword  */ DMG_ENTRY(2, PUPPET_DMGEFF_NORMAL),
    /* Giant's Knife */ DMG_ENTRY(4, PUPPET_DMGEFF_NORMAL),
    /* Fire arrow    */ DMG_ENTRY(2, PUPPET_DMGEFF_FIRE),
    /* Ice arrow     */ DMG_ENTRY(2, PUPPET_DMGEFF_ICE),
    /* Light arrow   */ DMG_ENTRY(2, PUPPET_DMGEFF_THUNDER),
    /* Unk arrow 1   */ DMG_ENTRY(2, PUPPET_DMGEFF_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, PUPPET_DMGEFF_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, PUPPET_DMGEFF_NONE),
    /* Fire magic    */ DMG_ENTRY(2, PUPPET_DMGEFF_FIRE),
    /* Ice magic     */ DMG_ENTRY(0, PUPPET_DMGEFF_ICE),
    /* Light magic   */ DMG_ENTRY(3, PUPPET_DMGEFF_THUNDER),
    /* Shield        */ DMG_ENTRY(0, PUPPET_DMGEFF_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, PUPPET_DMGEFF_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, PUPPET_DMGEFF_NORMAL),
    /* Giant spin    */ DMG_ENTRY(4, PUPPET_DMGEFF_NORMAL),
    /* Master spin   */ DMG_ENTRY(2, PUPPET_DMGEFF_NORMAL),
    /* Kokiri jump   */ DMG_ENTRY(2, PUPPET_DMGEFF_NORMAL),
    /* Giant jump    */ DMG_ENTRY(8, PUPPET_DMGEFF_NORMAL),
    /* Master jump   */ DMG_ENTRY(4, PUPPET_DMGEFF_NORMAL),
    /* Unknown 1     */ DMG_ENTRY(0, PUPPET_DMGEFF_NONE),
    /* Unblockable   */ DMG_ENTRY(0, PUPPET_DMGEFF_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, PUPPET_DMGEFF_KNOCKBACK),
    /* Unknown 2     */ DMG_ENTRY(0, PUPPET_DMGEFF_NONE),
};

void LinkPuppet_Init(Actor* thisx, PlayState* play) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    this->actor.room = -1;
    this->actor.targetMode = 1;

    this->puppetPacket.scene_id = -1;

    ActorShape_Init(&this->actor.shape, 0.0f, ActorShadow_DrawFeet, 90.0f);

    Collider_InitCylinder(play, &this->collider);
    Collider_SetCylinder(play, &this->collider, &this->actor, &sCylinderInit);

    this->actor.colChkInfo.damageTable = sDamageTable;
}

void LinkPuppet_Destroy(Actor* thisx, PlayState* play) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    Collider_DestroyCylinder(play, &this->collider);
}

void LinkPuppet_Update(Actor* thisx, PlayState* play) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    f32 puppetHeight = this->puppetPacket.puppet_age == 0 ? 60.0f : 30.0f;

    Actor_SetFocus(this, puppetHeight);

    Actor_UpdateBgCheckInfo(play, &this->actor, 15.0f, 30.0f, puppetHeight, 0x1D);

    if (this->damageTimer > 0) {
        this->damageTimer--;
    }

    // PvP detection
    if (this->collider.base.acFlags & AC_HIT && this->damageTimer <= 0) {
        this->collider.base.acFlags &= ~AC_HIT;

        this->damagePacket.hit_player_id = this->puppetPacket.player_id;
        this->damagePacket.damageValue = this->actor.colChkInfo.damage;
        this->damagePacket.damageEffect = this->actor.colChkInfo.damageEffect;
        this->damagePacket.damageEffect = this->actor.colChkInfo.damageEffect;
        this->damagePacket.knockbackRotation = -this->actor.yawTowardsPlayer;

        OTRSendDamagePacketToServer(&this->damagePacket);

        this->damageTimer = 12;
    }

    Collider_UpdateCylinder(thisx, &this->collider);
    CollisionCheck_SetOC(play, &play->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(play, &play->colChkCtx, &this->collider.base);

    // Position & rotation syncing
    if (play->sceneNum == this->puppetPacket.scene_id) {
        if (this->initialized == false) {
            SkelAnime_InitLink(play, &this->linkSkeleton, gPlayerSkelHeaders[((void)0, this->puppetPacket.puppet_age)],
                               gPlayerAnim_link_normal_wait, 9, this->linkSkeleton.jointTable,
                               this->linkSkeleton.morphTable, PLAYER_LIMB_MAX);
            this->initialized = true;
        }
        this->actor.world.pos = this->puppetPacket.posRot.pos;
        this->actor.shape.rot = this->puppetPacket.posRot.rot;
    } else {
        this->initialized = false;
        this->actor.world.pos.x = -32000.0f;
        this->actor.world.pos.y = -32000.0f;
        this->actor.world.pos.z = -32000.0f;
    }

    // Animation syncing
    if (this->puppetPacket.jointTable != NULL) {
        this->linkSkeleton.jointTable = this->puppetPacket.jointTable;
    }

    // Voice & SFX syncing
    for (size_t i = 0; i < 4; i++) {
        if (this->puppetPacket.sound_id[i] != 0) {
            Audio_PlaySoundGeneral(this->puppetPacket.sound_id[i], &this->actor.projectedPos, 4, &D_801333E0,
                                   &D_801333E0, &D_801333E8);
            this->puppetPacket.sound_id[i] = 0;
        }
    }

    // Ocarina notes syncing
    if ((this->puppetPacket.ocarina_freqScale != 0 || this->puppetPacket.ocarina_vol != 0 ||
         this->puppetPacket.ocarina_pitch != 0) &&
        this->ocarina_playing == 0) {
        Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | 0xD07, 0);
        Audio_QueueCmdS8(0x6 << 24 | SEQ_PLAYER_SFX << 16 | 0xD05, this->puppetPacket.ocarina_pitch);
        Audio_PlaySoundGeneral(NA_SE_OC_OCARINA, &this->actor.projectedPos, 4, &this->puppetPacket.ocarina_freqScale,
                               &this->puppetPacket.ocarina_vol, &D_801333E8);
        this->ocarina_playing = 1;
    } else if (this->ocarina_playing == 1 && this->puppetPacket.ocarina_freqScale == 0 &&
               this->puppetPacket.ocarina_vol == 0 && this->puppetPacket.ocarina_pitch == 0) {
        Audio_StopSfxById(NA_SE_OC_OCARINA);
        this->ocarina_playing = 0;
    }
}

Vec3f FEET_POS[] = {
    { 200.0f, 300.0f, 0.0f },
    { 200.0f, 200.0f, 0.0f },
};

extern Gfx** sPlayerDListGroups[];
extern Gfx* D_80125D28[];

s32 Puppet_OverrideLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx) {
    LinkPuppet* this = (LinkPuppet*)thisx;

     if (limbIndex == PLAYER_LIMB_ROOT) {
        if (this->puppetPacket.puppet_age == 1) {
            if (!(this->linkSkeleton.moveFlags & 4) || (this->linkSkeleton.moveFlags & 1)) {
                pos->x *= 0.64f;
                pos->z *= 0.64f;
            }

            if (!(this->linkSkeleton.moveFlags & 4) || (this->linkSkeleton.moveFlags & 2)) {
                pos->y *= 0.64f;
            }
        }
    } else if (limbIndex == PLAYER_LIMB_SHEATH) {

        Gfx** dLists = &sPlayerDListGroups[this->puppetPacket.sheathType][(void)0, this->puppetPacket.puppet_age];
        if ((this->puppetPacket.sheathType == 18) || (this->puppetPacket.sheathType == 19)) {
            dLists += this->puppetPacket.shieldType * 4;
        }
        *dList = ResourceMgr_LoadGfxByName(dLists[0]);

    } else if (limbIndex == PLAYER_LIMB_L_HAND) {

        Gfx** dLists = &sPlayerDListGroups[this->puppetPacket.leftHandType][(void)0, this->puppetPacket.puppet_age];
        if ((this->puppetPacket.leftHandType == 4) && this->puppetPacket.biggoron_broken) {
            dLists += 4;
        }
        *dList = ResourceMgr_LoadGfxByName(dLists[0]);

    } else if (limbIndex == PLAYER_LIMB_R_HAND) {

        Gfx** dLists = &sPlayerDListGroups[this->puppetPacket.rightHandType][(void)0, this->puppetPacket.puppet_age];
        if (this->puppetPacket.rightHandType == 10) {
            dLists += this->puppetPacket.shieldType * 4;
        }
        *dList = ResourceMgr_LoadGfxByName(dLists[0]);
    }

    return false;
}

Vec3f limbPos;
Vec3f zeroVec = { 0.0f, 0.0f, 0.0f };

void Puppet_PostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    if (limbIndex == PLAYER_LIMB_HEAD) {
        Matrix_MultVec3f(&zeroVec, &limbPos);
        this->actor.focus.pos = limbPos;
    }

    Vec3f* vec = &FEET_POS[((void)0, this->puppetPacket.puppet_age)];
    Actor_SetFeetPos(&this->actor, limbIndex, PLAYER_LIMB_L_FOOT, vec, PLAYER_LIMB_R_FOOT, vec);
}

void LinkPuppet_Draw(Actor* thisx, PlayState* play) {
    LinkPuppet* this = (LinkPuppet*)thisx;

    if (this->initialized) {
        func_8008F470(play, this->linkSkeleton.skeleton, this->linkSkeleton.jointTable,
                      this->linkSkeleton.dListCount, 0, this->puppetPacket.tunicType, this->puppetPacket.bootsType,
                      this->puppetPacket.faceType, Puppet_OverrideLimbDraw, Puppet_PostLimbDraw, this);
    }
}