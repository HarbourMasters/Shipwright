#ifndef Z64PLAY_H
#define Z64PLAY_H

#include "ultra64.h"

#include "z64actor.h"
#include "z64animation.h"
#include "z64bgcheck.h"
#include "z64camera.h"
#include "z64collision_check.h"
#include "z64cutscene.h"
#include "z64eff_footmark.h"
#include "z64environment.h"
#include "z64frameadvance.h"
#include "z64game_over.h"
#include "z64game.h"
#include "z64interface.h"
#include "z64item.h"
#include "z64light.h"
#include "z64math.h"
#include "z64message.h"
#include "z64pause_menu.h"
#include "z64player.h"
#include "z64prerender.h"
#include "z64save.h"
#include "z64scene.h"
#include "z64skybox.h"
#include "z64sound_source.h"
#include "z64transition.h"
#include "z64view.h"

// 2S2H [Custom Audio]. Was originally u8 seqId. Made 16 bit to allow for more than 255 sequences.
typedef struct SceneSequences {
    /* 0x0 */ u16 seqId;
    /* 0x1 */ u8 ambienceId;
} SceneSequences; // size = 0x2

typedef struct PlayState {
    /* 0x00000 */ GameState state;
    /* 0x000A4 */ s16 sceneId;
    /* 0x000A6 */ u8 sceneConfig;
    /* 0x000A7 */ char unk_A7[0x9];
    /* 0x000B0 */ void* sceneSegment;
    /* 0x000B8 */ View view;
    /* 0x00220 */ Camera mainCamera;
    /* 0x00398 */ Camera subCameras[NUM_CAMS - CAM_ID_SUB_FIRST];
    /* 0x00800 */ Camera* cameraPtrs[NUM_CAMS];
    /* 0x00810 */ s16 activeCamId;
    /* 0x00812 */ s16 nextCamera;
    /* 0x00814 */ SceneSequences sceneSequences;
    /* 0x00818 */ LightContext lightCtx;
    /* 0x00828 */ FrameAdvanceContext frameAdvCtx;
    /* 0x00830 */ CollisionContext colCtx;
    /* 0x01CA0 */ ActorContext actorCtx;
    /* 0x01F24 */ CutsceneContext csCtx;
    /* 0x01F78 */ SoundSource soundSources[SOUND_SOURCE_COUNT];
    /* 0x02138 */ EffFootmark footprintInfo[100];
    /* 0x046B8 */ SramContext sramCtx;
    /* 0x046E0 */ SkyboxContext skyboxCtx;
    /* 0x04908 */ MessageContext msgCtx;
    /* 0x169E8 */ InterfaceContext interfaceCtx;
    /* 0x16D30 */ PauseContext pauseCtx;
    /* 0x17000 */ GameOverContext gameOverCtx;
    /* 0x17004 */ EnvironmentContext envCtx;
    /* 0x17104 */ AnimTaskQueue animTaskQueue;
    /* 0x17D88 */ ObjectContext objectCtx;
    /* 0x186E0 */ RoomContext roomCtx;
    /* 0x18760 */ TransitionActorList transitionActors;
    /* 0x18768 */ void (*playerInit)(Player* player, struct PlayState* play, FlexSkeletonHeader* skelHeader);
    /* 0x1876C */ void (*playerUpdate)(Player* player, struct PlayState* play, Input* input);
    /* 0x18770 */ void (*unk_18770)(struct PlayState* play, Player* player);
    /* 0x18774 */ s32 (*startPlayerFishing)(struct PlayState* play);
    /* 0x18778 */ s32 (*grabPlayer)(struct PlayState* play, Player* player);
    /* 0x1877C */ s32 (*tryPlayerCsAction)(struct PlayState* play, Player* player, PlayerCsAction csAction);
    /* 0x18780 */ void (*func_18780)(Player* player, struct PlayState* play);
    /* 0x18784 */ s32 (*damagePlayer)(struct PlayState* play, s32 damage);
    /* 0x18788 */ void (*talkWithPlayer)(struct PlayState* play, Actor* actor);
    /* 0x1878C */ void (*unk_1878C)(struct PlayState* play);
    /* 0x18790 */ void (*unk_18790)(struct PlayState* play, s16 arg1);
    /* 0x18794 */ PlayerItemAction (*unk_18794)(struct PlayState* play, Player* player, ItemId itemId);
    /* 0x18798 */ s32 (*setPlayerTalkAnim)(struct PlayState* play, PlayerAnimationHeader* talkAnim, AnimationMode animMode);
    /* 0x1879C */ s16 playerCsIds[PLAYER_CS_ID_MAX];
    /* 0x187B0 */ MtxF viewProjectionMtxF;
    /* 0x187F0 */ Vec3f projectionMtxFDiagonal;
    /* 0x187FC */ MtxF billboardMtxF;
    /* 0x1883C */ Mtx* billboardMtx;
    /* 0x18840 */ u32 gameplayFrames;
    /* 0x18844 */ u8 soaringCsOrSoTCsPlaying; // Either the Song of Soaring Cutscene or the Song of Time Cutscene is playing.
    /* 0x18845 */ u8 haltAllActors;
    /* 0x18846 */ s16 numSetupActors;
    /* 0x18848 */ RoomList roomList;
    /* 0x18850 */ ActorEntry* linkActorEntry;
    /* 0x18854 */ ActorEntry* setupActorList;
    /* 0x18858 */ ActorCsCamInfo* actorCsCamList;
    /* 0x1885C */ EntranceEntry* setupEntranceList;
    /* 0x18860 */ u16* setupExitList;
    /* 0x18864 */ Path* setupPathList;
    /* 0x18868 */ void* naviQuestHints; // leftover from OoT, system which processes this is removed
    /* 0x1886C */ AnimatedMaterial* sceneMaterialAnims;
    /* 0x18870 */ void* specialEffects;
    /* 0x18874 */ u8 skyboxId; // @see SkyboxId enum
    /* 0x18875 */ s8 transitionTrigger; // "fade_direction"
    /* 0x18876 */ s16 worldCoverAlpha;
    /* 0x18878 */ s16 bgCoverAlpha;
    /* 0x1887A */ u16 nextEntrance;
    /* 0x1887C */ s8 bButtonAmmoPlusOne;
    /* 0x1887D */ s8 unk_1887D;
    /* 0x1887E */ s8 unk_1887E;
    /* 0x1887F */ u8 transitionType; // fadeTransition
    /* 0x18880 */ u8 unk_18880;
    /* 0x18884 */ CollisionCheckContext colChkCtx;
    /* 0x18B20 */ u16 cutsceneFlags[20];
    /* 0x18B48 */ u8 curSpawn;
    /* 0x18B49 */ u8 unk_18B49;
    /* 0x18B4A */ u8 transitionMode;
    /* 0x18B4C */ PreRender pauseBgPreRender;
    /* 0x18B9C */ char unk_18B9C[0x54];
    /* 0x18BF0 */ TransitionContext transitionCtx;
    /* 0x18E48 */ TransitionFade unk_18E48;
    /* 0x18E54 */ SceneTableEntry* loadedScene;
    /* 0x18E58 */ void* unk_18E58;
    /* 0x18E5C */ TexturePtr pictoPhotoI8;
    /* 0x18E60 */ void* unk_18E60;
    /* 0x18E64 */ void* unk_18E64;
    /* 0x18E68 */ void* unk_18E68; // framebuffer related to Lens of Truth
    /* 0x18E6C */ char unk_18E6C[0x3EC];
} PlayState; // size = 0x19258

typedef enum PictoPhotoState {
    /* 0 */ PICTO_PHOTO_STATE_OFF,
    /* 1 */ PICTO_PHOTO_STATE_SETUP,
    /* 2 */ PICTO_PHOTO_STATE_PROCESS,
    /* 3 */ PICTO_PHOTO_STATE_READY
} PictoPhotoState;

#define GET_ACTIVE_CAM(play) ((play)->cameraPtrs[(play)->activeCamId])
#define GET_PLAYER(play) ((Player*)(play)->actorCtx.actorLists[ACTORCAT_PLAYER].first)
#define GET_FIRST_ENEMY(play) ((Actor*)(play)->actorCtx.actorLists[ACTORCAT_ENEMY].first)


void Play_SetMotionBlurAlpha(u32 alpha);
void Play_EnableMotionBlur(u32 alpha);
void Play_DisableMotionBlur(void);
void Play_SetMotionBlurPriorityAlpha(u32 alpha);
void Play_EnableMotionBlurPriority(u32 alpha);
void Play_DisableMotionBlurPriority(void);
void Play_TriggerPictoPhoto(void);
Gfx* Play_SetFog(PlayState* thisx, Gfx* gfx);
void Play_Destroy(GameState* thisxx);
void Play_CompressI8ToI5(void* srcI8, void* destI5, size_t size);
void Play_DecompressI5ToI8(void* srcI5, void* destI8, size_t size);
void Play_Update(PlayState* thisx);
void Play_Draw(PlayState* thisx);
void Play_Main(GameState* thisxx);
bool Play_InCsMode(PlayState* thisx);
f32 Play_GetFloorSurfaceImpl(PlayState* thisx, MtxF* mtx, CollisionPoly** poly, s32* bgId, Vec3f* pos);
void Play_GetFloorSurface(PlayState* thisx, MtxF* mtx, Vec3f* pos);
void* Play_LoadFile(PlayState* thisx, RomFile* entry);
void Play_InitEnvironment(PlayState* thisx, s16 skyboxId);
void Play_GetScreenPos(PlayState* thisx, Vec3f* worldPos, Vec3f* screenPos);
s16 Play_CreateSubCamera(PlayState* thisx);
s16 Play_GetActiveCamId(PlayState* thisx);
s32 Play_ChangeCameraStatus(PlayState* thisx, s16 camId, s16 status);
void Play_ClearCamera(PlayState* thisx, s16 camId);
Camera* Play_GetCamera(PlayState* thisx, s16 camId);
s32 Play_SetCameraAtEye(PlayState* thisx, s16 camId, Vec3f* at, Vec3f* eye);
s32 Play_SetCameraAtEyeUp(PlayState* thisx, s16 camId, Vec3f* at, Vec3f* eye, Vec3f* up);
s32 Play_SetCameraFov(PlayState* thisx, s16 camId, f32 fov);
s32 Play_SetCameraRoll(PlayState* thisx, s16 camId, s16 roll);
void Play_CopyCamera(PlayState* thisx, s16 destCamId, s16 srcCamId);
s32 func_80169A50(PlayState* thisx, s16 camId, Player* player, s16 setting);
s32 Play_ChangeCameraSetting(PlayState* thisx, s16 camId, s16 setting);
void func_80169AFC(PlayState* thisx, s16 camId, s16 timer);
u16 Play_GetActorCsCamSetting(PlayState* thisx, s32 csCamDataIndex);
Vec3s* Play_GetActorCsCamFuncData(PlayState* thisx, s32 csCamDataIndex);
s16 Play_GetOriginalSceneId(s16 sceneId);
void Play_SaveCycleSceneFlags(PlayState* thisx);
void Play_SetRespawnData(PlayState* thisx, s32 respawnMode, u16 entrance, s32 roomIndex, s32 playerParams, Vec3f* pos, s16 yaw);
void Play_SetupRespawnPoint(PlayState* thisx, s32 respawnMode, s32 playerParams);
void func_80169EFC(PlayState* thisx);
void func_80169F78(PlayState* thisx);
void func_80169FDC(PlayState* thisx);
s32 Play_CamIsNotFixed(PlayState* thisx);
s32 func_8016A02C(PlayState* thisx, Actor* actor, s16* yaw);
s32 Play_IsUnderwater(PlayState* thisx, Vec3f* pos);
s32 Play_IsDebugCamEnabled(void);
void Play_AssignPlayerCsIdsFromScene(PlayState* thisx, s32 spawnCsId);
void Play_FillScreen(PlayState* thisx, s16 fillScreenOn, u8 red, u8 green, u8 blue, u8 alpha);
void Play_Init(GameState* thisxx);

extern s32 gDbgCamEnabled;

extern s32 gTransitionTileState;
extern Color_RGBA8_u32 gPlayVisMonoColor;

#endif
