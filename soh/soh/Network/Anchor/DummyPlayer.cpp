#include "Anchor.h"
#include "soh/Network/Anchor/AnchorModRegistry.h"
#include "soh/Enhancements/nametag.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/frame_interpolation.h"
#include <unordered_map>
#include <ship/Context.h>
#include <ship/resource/Resource.h>
#include <ship/resource/ResourceManager.h>

extern "C" {
#include "macros.h"
#include "variables.h"
#include "functions.h"
extern PlayState* gPlayState;

void Player_UseItem(PlayState* play, Player* player, s32 item);
void Player_Draw(Actor* actor, PlayState* play);

#if defined(MODDING) || defined(_MSC_VER) || defined(__GNUC__)
extern void* sEyeTextures[2][8];
extern void* sMouthTextures[2][4];
#endif

extern u8 sEyeMouthIndexes[][2];
}

namespace {
struct AnchorTextureOverrides {
    bool hasEye;
    bool hasMouth;
    int eyeIndex;
    int mouthIndex;
    void* originalEye;
    void* originalMouth;
};

const char* kAdultEyeTextureNames[] = { "gLinkAdultEyesOpenTex", "gLinkAdultEyesHalfTex", "gLinkAdultEyesClosedfTex",
                                        "gLinkAdultEyesRollLeftTex", "gLinkAdultEyesRollRightTex",
                                        "gLinkAdultEyesShockTex", "gLinkAdultEyesUnk1Tex", "gLinkAdultEyesUnk2Tex" };
const char* kChildEyeTextureNames[] = { "gLinkChildEyesOpenTex", "gLinkChildEyesHalfTex", "gLinkChildEyesClosedfTex",
                                        "gLinkChildEyesRollLeftTex", "gLinkChildEyesRollRightTex",
                                        "gLinkChildEyesShockTex", "gLinkChildEyesUnk1Tex", "gLinkChildEyesUnk2Tex" };
const char* kAdultMouthTextureNames[] = { "gLinkAdultMouth1Tex", "gLinkAdultMouth2Tex", "gLinkAdultMouth3Tex",
                                          "gLinkAdultMouth4Tex" };
const char* kChildMouthTextureNames[] = { "gLinkChildMouth1Tex", "gLinkChildMouth2Tex", "gLinkChildMouth3Tex",
                                          "gLinkChildMouth4Tex" };

std::unordered_map<std::string, std::shared_ptr<Ship::IResource>> sCustomTextureCache;

int ClampIndex(int value, int maxValue) {
    if (value < 0) {
        return 0;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

void* LoadCustomTexture(const std::string& modelId, int32_t linkAge, const char* baseName) {
    if (modelId.empty() || baseName == nullptr) {
        return nullptr;
    }

    auto archive = AnchorModRegistry::FindArchiveById(modelId);
    if (archive == nullptr) {
        return nullptr;
    }

    std::string folder = linkAge == LINK_AGE_ADULT ? (modelId + "_adult") : (modelId + "_child");
    std::string path = "objects/object_anchor_models/" + folder + "/" + baseName;
    std::string cacheKey = archive->GetPath() + "|" + path;

    auto cached = sCustomTextureCache.find(cacheKey);
    if (cached != sCustomTextureCache.end()) {
        return cached->second ? cached->second->GetRawPointer() : nullptr;
    }

    auto context = Ship::Context::GetInstance();
    if (context == nullptr || context->GetResourceManager() == nullptr) {
        return nullptr;
    }

    auto resource = context->GetResourceManager()->LoadResource(Ship::ResourceIdentifier(path.c_str(), 0, archive), true);
    if (resource == nullptr) {
        sCustomTextureCache.emplace(cacheKey, nullptr);
        return nullptr;
    }

    sCustomTextureCache.emplace(cacheKey, resource);
    return resource->GetRawPointer();
}

AnchorTextureOverrides ApplyAnchorFlipbookTextures(Player* player, const std::string& modelId, int32_t linkAge) {
    AnchorTextureOverrides overrides = {};
    overrides.eyeIndex = -1;
    overrides.mouthIndex = -1;

#if defined(MODDING) || defined(_MSC_VER) || defined(__GNUC__)
    if (player == nullptr) {
        return overrides;
    }

    int eyeIndex = (player->skelAnime.jointTable[22].x & 0xF) - 1;
    int mouthIndex = (player->skelAnime.jointTable[22].x >> 4) - 1;
    if (eyeIndex < 0) {
        eyeIndex = sEyeMouthIndexes[player->actor.shape.face][0];
    }
    if (mouthIndex < 0) {
        mouthIndex = sEyeMouthIndexes[player->actor.shape.face][1];
    }

    eyeIndex = ClampIndex(eyeIndex, 7);
    mouthIndex = ClampIndex(mouthIndex, 3);

    const char* eyeName = linkAge == LINK_AGE_ADULT ? kAdultEyeTextureNames[eyeIndex]
                                                    : kChildEyeTextureNames[eyeIndex];
    const char* mouthName = linkAge == LINK_AGE_ADULT ? kAdultMouthTextureNames[mouthIndex]
                                                      : kChildMouthTextureNames[mouthIndex];
    void* eyeTexture = LoadCustomTexture(modelId, linkAge, eyeName);
    void* mouthTexture = LoadCustomTexture(modelId, linkAge, mouthName);

    if (eyeTexture != nullptr) {
        overrides.eyeIndex = eyeIndex;
        overrides.originalEye = sEyeTextures[linkAge][eyeIndex];
        sEyeTextures[linkAge][eyeIndex] = eyeTexture;
        overrides.hasEye = true;
    }

    if (mouthTexture != nullptr) {
        overrides.mouthIndex = mouthIndex;
        overrides.originalMouth = sMouthTextures[linkAge][mouthIndex];
        sMouthTextures[linkAge][mouthIndex] = mouthTexture;
        overrides.hasMouth = true;
    }
#endif

    return overrides;
}

void RestoreAnchorFlipbookTextures(const AnchorTextureOverrides& overrides, int32_t linkAge) {
#if defined(MODDING) || defined(_MSC_VER) || defined(__GNUC__)
    if (overrides.hasEye && overrides.eyeIndex >= 0) {
        sEyeTextures[linkAge][overrides.eyeIndex] = overrides.originalEye;
    }
    if (overrides.hasMouth && overrides.mouthIndex >= 0) {
        sMouthTextures[linkAge][overrides.mouthIndex] = overrides.originalMouth;
    }
#endif
}
} // namespace

static DamageTable DummyPlayerDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_STUN),
    /* Deku stick    */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Slingshot     */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Explosive     */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Boomerang     */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_STUN),
    /* Normal arrow  */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Hammer swing  */ DMG_ENTRY(2, PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE),
    /* Hookshot      */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_STUN),
    /* Kokiri sword  */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Master sword  */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Giant's Knife */ DMG_ENTRY(4, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Fire arrow    */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_FIRE),
    /* Ice arrow     */ DMG_ENTRY(4, PLAYER_HIT_RESPONSE_ICE_TRAP),
    /* Light arrow   */ DMG_ENTRY(2, PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK),
    /* Unk arrow 1   */ DMG_ENTRY(2, PLAYER_HIT_RESPONSE_NONE),
    /* Unk arrow 2   */ DMG_ENTRY(2, PLAYER_HIT_RESPONSE_NONE),
    /* Unk arrow 3   */ DMG_ENTRY(2, PLAYER_HIT_RESPONSE_NONE),
    /* Fire magic    */ DMG_ENTRY(0, DUMMY_PLAYER_HIT_RESPONSE_FIRE),
    /* Ice magic     */ DMG_ENTRY(3, PLAYER_HIT_RESPONSE_ICE_TRAP),
    /* Light magic   */ DMG_ENTRY(0, PLAYER_HIT_RESPONSE_ELECTRIC_SHOCK),
    /* Shield        */ DMG_ENTRY(0, PLAYER_HIT_RESPONSE_NONE),
    /* Mirror Ray    */ DMG_ENTRY(0, PLAYER_HIT_RESPONSE_NONE),
    /* Kokiri spin   */ DMG_ENTRY(1, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Giant spin    */ DMG_ENTRY(4, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Master spin   */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Kokiri jump   */ DMG_ENTRY(2, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Giant jump    */ DMG_ENTRY(8, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Master jump   */ DMG_ENTRY(4, DUMMY_PLAYER_HIT_RESPONSE_NORMAL),
    /* Unknown 1     */ DMG_ENTRY(0, PLAYER_HIT_RESPONSE_NONE),
    /* Unblockable   */ DMG_ENTRY(0, PLAYER_HIT_RESPONSE_NONE),
    /* Hammer jump   */ DMG_ENTRY(4, PLAYER_HIT_RESPONSE_KNOCKBACK_LARGE),
    /* Unknown 2     */ DMG_ENTRY(0, PLAYER_HIT_RESPONSE_NONE),
};

void DummyPlayer_Init(Actor* actor, PlayState* play) {
    Player* player = (Player*)actor;

    uint32_t clientId = Anchor::Instance->GetDummyPlayerClientId(actor);

    if (!Anchor::Instance->clients.contains(clientId)) {
        Actor_Kill(actor);
        return;
    }

    AnchorClient& client = Anchor::Instance->clients[clientId];

    // Hack to account for usage of gSaveContext in Player_Init
    s32 originalAge = gSaveContext.linkAge;
    gSaveContext.linkAge = client.linkAge;

    // #region modeled after EnTorch2_Init and Player_Init
    actor->room = -1;
    player->itemAction = player->heldItemAction = -1;
    player->heldItemId = ITEM_NONE;
    Player_UseItem(play, player, ITEM_NONE);
    Player_SetModelGroup(player, Player_ActionToModelGroup(player, player->heldItemAction));
    play->playerInit(player, play, gPlayerSkelHeaders[client.linkAge]);

    play->func_11D54(player, play);
    // #endregion

    player->cylinder.base.acFlags = AC_ON | AC_TYPE_PLAYER;
    player->cylinder.base.ocFlags2 = OC2_TYPE_1;
    player->cylinder.info.bumperFlags = BUMP_ON | BUMP_HOOKABLE | BUMP_NO_HITMARK;
    player->actor.flags |= ACTOR_FLAG_HOOKSHOT_PULLS_PLAYER;
    player->cylinder.dim.radius = 30;
    player->actor.colChkInfo.damageTable = &DummyPlayerDamageTable;

    gSaveContext.linkAge = originalAge;

    bool isGlobalRoom = (std::string("soh-global") == CVarGetString(CVAR_REMOTE_ANCHOR("RoomId"), ""));

    if (!isGlobalRoom) {
        NameTag_RegisterForActorWithOptions(actor, client.name.c_str(), {});
    }

    AnchorModRegistry::ApplyModelToPlayer(client.modelId, client.linkAge, player);
    client.appliedModelId = client.modelId;
}

void Math_Vec3s_Copy(Vec3s* dest, Vec3s* src) {
    dest->x = src->x;
    dest->y = src->y;
    dest->z = src->z;
}

// Update the actor with new data from the client
void DummyPlayer_Update(Actor* actor, PlayState* play) {
    Player* player = (Player*)actor;

    uint32_t clientId = Anchor::Instance->GetDummyPlayerClientId(actor);

    if (!Anchor::Instance->clients.contains(clientId)) {
        Actor_Kill(actor);
        return;
    }

    AnchorClient& client = Anchor::Instance->clients[clientId];

    if (client.sceneNum != gPlayState->sceneNum || !client.online || !client.isSaveLoaded) {
        actor->world.pos.x = -9999.0f;
        actor->world.pos.y = -9999.0f;
        actor->world.pos.z = -9999.0f;
        actor->shape.shadowAlpha = 0;
        return;
    }

    actor->shape.shadowAlpha = 255;
    Math_Vec3s_Copy(&player->upperLimbRot, &client.upperLimbRot);
    Math_Vec3s_Copy(&actor->shape.rot, &client.posRot.rot);
    Math_Vec3f_Copy(&actor->world.pos, &client.posRot.pos);
    player->skelAnime.jointTable = client.jointTable;
    player->skelAnime.movementFlags = client.movementFlags;
    Math_Vec3s_Copy(&player->skelAnime.prevTransl, &client.prevTransl);
    player->currentBoots = client.currentBoots;
    player->currentShield = client.currentShield;
    player->currentTunic = client.currentTunic;
    player->stateFlags1 = client.stateFlags1;
    player->stateFlags2 = client.stateFlags2;
    player->itemAction = client.itemAction;
    player->heldItemAction = client.heldItemAction;
    player->invincibilityTimer = client.invincibilityTimer;
    player->unk_862 = client.unk_862;
    player->unk_85C = client.unk_85C;
    player->av1.actionVar1 = client.actionVar1;

    // Apply animation movement (Copied from Player_ApplyAnimMovementScaledByAge)
    Vec3f diff;
    SkelAnime_UpdateTranslation(&player->skelAnime, &diff, player->actor.shape.rot.y);

    if (player->skelAnime.movementFlags & 1) {
        if (!LINK_IS_ADULT) {
            diff.x *= 0.64f;
            diff.z *= 0.64f;
        }

        player->actor.world.pos.x += diff.x * player->actor.scale.x;
        player->actor.world.pos.z += diff.z * player->actor.scale.z;
    }

    if (player->skelAnime.movementFlags & 2) {
        if (!(player->skelAnime.movementFlags & 4)) {
            diff.y *= player->ageProperties->unk_08;
        }

        player->actor.world.pos.y += diff.y * player->actor.scale.y;
    }

    if (player->modelGroup != client.modelGroup) {
        // Hack to account for usage of gSaveContext
        s32 originalAge = gSaveContext.linkAge;
        gSaveContext.linkAge = client.linkAge;
        u8 originalButtonItem0 = gSaveContext.equips.buttonItems[0];
        gSaveContext.equips.buttonItems[0] = client.buttonItem0;
        Player_SetModelGroup(player, client.modelGroup);
        gSaveContext.linkAge = originalAge;
        gSaveContext.equips.buttonItems[0] = originalButtonItem0;
    }

    if (client.modelId != client.appliedModelId) {
        AnchorModRegistry::ApplyModelToPlayer(client.modelId, client.linkAge, player);
        client.appliedModelId = client.modelId;
    }

    if (Anchor::Instance->roomState.pvpMode == 0 ||
        (Anchor::Instance->roomState.pvpMode == 1 &&
         client.teamId == CVarGetString(CVAR_REMOTE_ANCHOR("TeamId"), "default"))) {
        actor->flags |= ACTOR_FLAG_LOCK_ON_DISABLED;
        return;
    }

    actor->flags &= ~ACTOR_FLAG_LOCK_ON_DISABLED;

    if (player->cylinder.base.acFlags & AC_HIT && player->invincibilityTimer == 0) {
        Anchor::Instance->SendPacket_DamagePlayer(client.clientId, player->actor.colChkInfo.damageEffect,
                                                  player->actor.colChkInfo.damage);
        if (player->actor.colChkInfo.damageEffect == DUMMY_PLAYER_HIT_RESPONSE_STUN) {
            Actor_SetColorFilter(&player->actor, 0, 0xFF, 0, 24);
        } else {
            player->invincibilityTimer = 20;
        }
    }

    Collider_UpdateCylinder(&player->actor, &player->cylinder);

    if (!(player->stateFlags2 & PLAYER_STATE2_FROZEN)) {
        if (!(player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_HANGING_OFF_LEDGE |
                                     PLAYER_STATE1_CLIMBING_LEDGE | PLAYER_STATE1_ON_HORSE))) {
            CollisionCheck_SetOC(play, &play->colChkCtx, &player->cylinder.base);
        }

        if (!(player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_DAMAGED)) &&
            (player->invincibilityTimer <= 0)) {
            CollisionCheck_SetAC(play, &play->colChkCtx, &player->cylinder.base);

            if (player->invincibilityTimer < 0) {
                CollisionCheck_SetAT(play, &play->colChkCtx, &player->cylinder.base);
            }
        }
    }

    if (player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_ITEM_CS | PLAYER_STATE1_IN_CUTSCENE)) {
        player->actor.colChkInfo.mass = MASS_IMMOVABLE;
    } else {
        player->actor.colChkInfo.mass = 50;
    }

    Collider_ResetCylinderAC(play, &player->cylinder.base);
}

void DummyPlayer_Draw(Actor* actor, PlayState* play) {
    Player* player = (Player*)actor;

    uint32_t clientId = Anchor::Instance->GetDummyPlayerClientId(actor);

    if (!Anchor::Instance->clients.contains(clientId)) {
        Actor_Kill(actor);
        return;
    }

    AnchorClient& client = Anchor::Instance->clients[clientId];

    if (client.sceneNum != gPlayState->sceneNum || !client.online || !client.isSaveLoaded) {
        return;
    }

    // Hack to account for usage of gSaveContext in Player_Draw
    s32 originalAge = gSaveContext.linkAge;
    gSaveContext.linkAge = client.linkAge;
    u8 originalButtonItem0 = gSaveContext.equips.buttonItems[0];
    gSaveContext.equips.buttonItems[0] = client.buttonItem0;

    AnchorTextureOverrides textureOverrides = {};
    bool hasCustomModel = AnchorModRegistry::HasCustomModel(client.modelId, client.linkAge, player->skelAnime.limbCount);
    if (hasCustomModel) {
        ResourceMgr_SetAnchorModelOverride(client.modelId, client.linkAge);
        textureOverrides = ApplyAnchorFlipbookTextures(player, client.modelId, client.linkAge);
    }

    Player_Draw((Actor*)player, play);

    if (textureOverrides.hasEye || textureOverrides.hasMouth) {
        RestoreAnchorFlipbookTextures(textureOverrides, client.linkAge);
    }
    if (hasCustomModel) {
        ResourceMgr_ClearAnchorModelOverride();
    }

    gSaveContext.linkAge = originalAge;
    gSaveContext.equips.buttonItems[0] = originalButtonItem0;
}

void DummyPlayer_Destroy(Actor* actor, PlayState* play) {
}
