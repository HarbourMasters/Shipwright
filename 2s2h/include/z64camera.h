#ifndef Z64CAMERA_H
#define Z64CAMERA_H

#include "ultra64.h"
#include "z64math.h"
#include "z64actor.h"
#include "z64save.h"
#include "z64view.h"
#include "unk.h"

struct CollisionPoly;

// these two angle conversion macros are slightly inaccurate
#define CAM_DEG_TO_BINANG(degrees) (s16)((degrees) * ((f32)0xFFFF / 360) + .5f)
#define CAM_BINANG_TO_DEG(binang) ((f32)(binang) * (360.0001525f / 0xFFFF))

#define NUM_CAMS 4

// Camera IDs are indices into `cameraPtrs`
#define CAM_ID_MAIN 0 // The index of the main camera
#define CAM_ID_SUB_FIRST 1 // The index sub cameras start at
#define CAM_ID_NONE -1 // Used to indicate no camera. Can be used to default to the active camera in some scenarios

#define SUB_CAM_ID_DONE 0 // Used in some actors for variables holding sub camera IDs to indicate "subcam is finished"

#define CAM_HUD_VISIBILITY_SHIFT 8
#define CAM_HUD_VISIBILITY_MASK (0x0F00)
#define CAM_HUD_VISIBILITY(hudVisibility) (((hudVisibility) & 0xF) << CAM_HUD_VISIBILITY_SHIFT)

#define CAM_HUD_VISIBILITY_ALL \
    CAM_HUD_VISIBILITY(0) // HUD_VISIBILITY_ALL
#define CAM_HUD_VISIBILITY_NONE \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_NONE)
#define CAM_HUD_VISIBILITY_NONE_ALT \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_NONE_ALT)
#define CAM_HUD_VISIBILITY_HEARTS_WITH_OVERWRITE \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_HEARTS_WITH_OVERWRITE)
#define CAM_HUD_VISIBILITY_A \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_A)
#define CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_WITH_OVERWRITE \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_A_HEARTS_MAGIC_WITH_OVERWRITE)
#define CAM_HUD_VISIBILITY_A_HEARTS_MAGIC_MINIMAP_WITH_OVERWRITE \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_A_HEARTS_MAGIC_MINIMAP_WITH_OVERWRITE)
#define CAM_HUD_VISIBILITY_ALL_NO_MINIMAP_W_DISABLED \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_ALL_NO_MINIMAP_W_DISABLED)
#define CAM_HUD_VISIBILITY_B \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_B)
#define CAM_HUD_VISIBILITY_HEARTS_MAGIC \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_HEARTS_MAGIC)
#define CAM_HUD_VISIBILITY_B_ALT \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_B_ALT)
#define CAM_HUD_VISIBILITY_HEARTS \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_HEARTS)
#define CAM_HUD_VISIBILITY_A_B_MINIMAP \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_A_B_MINIMAP)
#define CAM_HUD_VISIBILITY_HEARTS_MAGIC_WITH_OVERWRITE \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_HEARTS_MAGIC_WITH_OVERWRITE)
#define CAM_HUD_VISIBILITY_HEARTS_MAGIC_C \
    CAM_HUD_VISIBILITY(HUD_VISIBILITY_HEARTS_MAGIC_C)
// HUD_VISIBILITY_ALL_NO_MINIMAP and above can not be set using `CAM_HUD_VISIBILITY`
#define CAM_HUD_VISIBILITY_IGNORE \
    CAM_HUD_VISIBILITY(0xF)

/**
 * letterboxFlag: determines the size of the letter-box window. See CAM_LETTERBOX_* enums.
 *                  Can also add on the flag ( | CAM_LETTERBOX_INSTANT) to make the size change immediately
 * hudVisibility: hides certain hud icons
 *    - A value of 0 in camera is translated to a hudVisibility of 50, 
 *      which is the value to restore all hud icons to the screen (CAM_HUD_VISIBILITY_ALL)
 *    - A value of 0xF in camera results in no change in the hudVisibility (CAM_HUD_VISIBILITY_IGNORE)
 * funcFlags: Custom flags for functions
 */
#define CAM_INTERFACE_FLAGS(letterboxFlag, hudVisibility, funcFlags) \
    (((letterboxFlag) & CAM_LETTERBOX_MASK) | (hudVisibility) | ((funcFlags) & 0xFF))

// Shrinking the window from the top and bottom with black borders (letterboxing)
#define CAM_LETTERBOX_MASK 0xF000

#define CAM_LETTERBOX_SIZE_MASK 0x7000
#define CAM_LETTERBOX_NONE 0x0000
// small/medium/large black borders
#define CAM_LETTERBOX_SMALL 0x1000
#define CAM_LETTERBOX_MEDIUM 0x2000
#define CAM_LETTERBOX_LARGE 0x3000
#define CAM_LETTERBOX_NONE_4 0x4000

#define CAM_LETTERBOX_INSTANT 0x8000 // Bit to determine whether to set the current value directly (on), or to set the size target (off)

#define CAM_LETTERBOX_IGNORE 0xF000 // No change in letterbox size, keep the previous size


// Camera behaviorFlags. Flags specifically for settings, modes, and bgCam
// Used to store current state, only CAM_BEHAVIOR_SETTING_1 and CAM_BEHAVIOR_BG_2 are read from and used in logic
// Setting (0x1, 0x10)
#define CAM_BEHAVIOR_SETTING_USE_PRIORITY (1 << 0) // Use settings priority system
#define CAM_BEHAVIOR_SETTING_2 (1 << 4)
// Mode (0x2, 0x20)
#define CAM_BEHAVIOR_MODE_1 (1 << 1)
#define CAM_BEHAVIOR_MODE_VALID (1 << 5)
// bgCam (0x4, 0x40)
#define CAM_BEHAVIOR_BGCAM_1 (1 << 2)
#define CAM_BEHAVIOR_BGCAM_2 (1 << 6)

// Camera stateFlags. Variety of generic flags
#define CAM_STATE_0 (1 << 0) // Must be set for the camera from changing settings based on the bg surface
#define CAM_STATE_CHECK_WATER (1 << 1)
#define CAM_STATE_2 (1 << 2)
#define CAM_STATE_3 (1 << 3)
#define CAM_STATE_4 (1 << 4)
#define CAM_STATE_DISABLE_MODE_CHANGE (1 << 5)
#define CAM_STATE_6 (1 << 6)
#define CAM_STATE_7 (1 << 7)
#define CAM_STATE_UNDERWATER (1 << 8)
#define CAM_STATE_9 (1 << 9)
#define CAM_STATE_10 (1 << 10) // Surpresses the camera from changing settings based on the bg surface
#define CAM_STATE_11 (1 << 11)
#define CAM_STATE_12 (1 << 12)
#define CAM_STATE_13 (1 << 13)
#define CAM_STATE_INITIALIZED (1 << 14)
#define CAM_STATE_15 ((s16)(1 << 15))

// Camera viewFlags. Set params related to view
#define CAM_VIEW_AT (1 << 0) // camera->at
#define CAM_VIEW_EYE (1 << 1) // camera->eye and camera->eyeNext
#define CAM_VIEW_UP (1 << 2) // camera->up
#define CAM_VIEW_TARGET (1 << 3) // camera->target
#define CAM_VIEW_TARGET_POS (1 << 4) // camera->targetPosRot.pos
#define CAM_VIEW_FOV (1 << 5) // camera->fov
#define CAM_VIEW_ROLL (1 << 6) // camera->roll

/**
 * Camera Status type
 * Determines how much the camera is updated each frame
 * Higher values represents higher levels of activity
 */
// Camera is on
#define CAM_STATUS_CUT        0 // The camera is not updated at all
#define CAM_STATUS_WAIT       1 // There is minimally/partially updated, action function is not run
#define CAM_STATUS_UNK3       3 // The camera is mostly updated including running its action function, but data is not set to view
#define CAM_STATUS_ACTIVE     7 // The camera is fully updated, info is sent to view
// Camera is off
#define CAM_STATUS_INACTIVE   0x100

typedef enum {
    /* 0x00 */ CAM_SET_NONE,
    /* 0x01 */ CAM_SET_NORMAL0, // Generic camera 0, used in various places "NORMAL0"
    /* 0x02 */ CAM_SET_NORMAL3, // Generic camera 3, used in various places "NORMAL3"
    /* 0x03 */ CAM_SET_PIVOT_DIVING, // Player diving from the surface of the water to underwater not as zora "CIRCLE5"
    /* 0x04 */ CAM_SET_HORSE, // Reiding a horse "HORSE0"
    /* 0x05 */ CAM_SET_ZORA_DIVING, // Parallel's Pivot Diving, but as Zora. However, Zora does not dive like a human. So this setting appears to not be used "ZORA0"
    /* 0x06 */ CAM_SET_PREREND_FIXED, // Unused remnant of OoT: camera is fixed in position and rotation "PREREND0"
    /* 0x07 */ CAM_SET_PREREND_PIVOT, // Unused remnant of OoT: Camera is fixed in position with fixed pitch, but is free to rotate in the yaw direction 360 degrees "PREREND1"
    /* 0x08 */ CAM_SET_DOORC, // Generic room door transitions, camera moves and follows player as the door is open and closed "DOORC"
    /* 0x09 */ CAM_SET_DEMO0, // Unknown, possibly related to treasure chest game as goron? "DEMO0"
    /* 0x0A */ CAM_SET_FREE0, // Free Camera, manual control is given, no auto-updating eye or at "FREE0"
    /* 0x0B */ CAM_SET_BIRDS_EYE_VIEW_0, // Appears unused. Camera is a top-down view "FUKAN0"
    /* 0x0C */ CAM_SET_NORMAL1, // Generic camera 1, used in various places "NORMAL1"
    /* 0x0D */ CAM_SET_NANAME, // Unknown, slanted or tilted. Behaves identical to Normal0 except with added roll "NANAME"
    /* 0x0E */ CAM_SET_CIRCLE0, // Used in Curiosity Shop, Pirates Fortress, Mayor's Residence "CIRCLE0"
    /* 0x0F */ CAM_SET_FIXED0, // Used in Sakon's Hideout puzzle rooms, milk bar stage "FIXED0"
    /* 0x10 */ CAM_SET_SPIRAL_DOOR, // Exiting a Spiral Staircase "SPIRAL"
    /* 0x11 */ CAM_SET_DUNGEON0, // Generic dungeon camera 0, used in various places "DUNGEON0"
    /* 0x12 */ CAM_SET_ITEM0, // Getting an item and holding it above Player's head (from small chest, freestanding, npc, ...) "ITEM0"
    /* 0x13 */ CAM_SET_ITEM1, // Looking at player while playing the ocarina "ITEM1"
    /* 0x14 */ CAM_SET_ITEM2, // Bottles: drinking, releasing fairy, dropping fish "ITEM2"
    /* 0x15 */ CAM_SET_ITEM3, // Bottles: catching fish or bugs, showing an item "ITEM3"
    /* 0x16 */ CAM_SET_NAVI, // Song of Soaring, variations of playing Song of Time "NAVI"
    /* 0x17 */ CAM_SET_WARP_PAD_MOON, // Warp circles from Goron Trial on the moon "WARP0"
    /* 0x18 */ CAM_SET_DEATH, // Player death animation when health goes to 0 "DEATH"
    /* 0x19 */ CAM_SET_REBIRTH, // Unknown set with camDataId = -9 (it's not being revived by a fairy) "REBIRTH"
    /* 0x1A */ CAM_SET_LONG_CHEST_OPENING, // Long cutscene when opening a big chest with a major item "TREASURE"
    /* 0x1B */ CAM_SET_MASK_TRANSFORMATION, // Putting on a transformation mask "TRANSFORM"
    /* 0x1C */ CAM_SET_ATTENTION, // Unknown, set with camDataId = -15 "ATTENTION"
    /* 0x1D */ CAM_SET_WARP_PAD_ENTRANCE, // Warp pad from start of a dungeon to the boss-room "WARP1"
    /* 0x1E */ CAM_SET_DUNGEON1, // Generic dungeon camera 1, used in various places "DUNGEON1"
    /* 0x1F */ CAM_SET_FIXED1, // Fixes camera in place, used in various places eg. entering Stock Pot Inn, hiting a switch, giving witch a red potion, shop browsing "FIXED1"
    /* 0x20 */ CAM_SET_FIXED2, // Used in Pinnacle Rock after defeating Sea Monsters, and by Tatl in Fortress "FIXED2"
    /* 0x21 */ CAM_SET_MAZE, // Unused. Set to use Camera_Parallel2(), which is only Camera_Noop() "MAZE"
    /* 0x22 */ CAM_SET_REMOTEBOMB, // Unused. Set to use Camera_Parallel2(), which is only Camera_Noop(). But also related to Play_ChangeCameraSetting? "REMOTEBOMB"
    /* 0x23 */ CAM_SET_CIRCLE1, // Unknown "CIRCLE1"
    /* 0x24 */ CAM_SET_CIRCLE2, // Looking at far-away NPCs eg. Garo in Road to Ikana, Hungry Goron, Tingle "CIRCLE2"
    /* 0x25 */ CAM_SET_CIRCLE3, // Used in curiosity shop, goron racetrack, final room in Sakon's hideout, other places "CIRCLE3"
    /* 0x26 */ CAM_SET_CIRCLE4, // Used during the races on the doggy racetrack "CIRCLE4"
    /* 0x27 */ CAM_SET_FIXED3, // Used in Stock Pot Inn Toilet and Tatl cutscene after woodfall "FIXED3"
    /* 0x28 */ CAM_SET_TOWER_ASCENT, // Various climbing structures (Snowhead climb to the temple entrance) "TOWER0"
    /* 0x29 */ CAM_SET_PARALLEL0, // Unknown "PARALLEL0"
    /* 0x2A */ CAM_SET_NORMALD, // Unknown, set with camDataId = -20 "NORMALD"
    /* 0x2B */ CAM_SET_SUBJECTD, // Unknown, set with camDataId = -21 "SUBJECTD"
    /* 0x2C */ CAM_SET_START0, // Entering a room, either Dawn of a New Day reload, or entering a door where the camera is fixed on the other end "START0"
    /* 0x2D */ CAM_SET_START2, // Entering a scene, camera is put at a low angle eg. Grottos, Deku Palace, Stock Pot Inn "START2"
    /* 0x2E */ CAM_SET_STOP0, // Called in z_play "STOP0"
    /* 0x2F */ CAM_SET_BOAT_CRUISE, //  Koume's boat cruise "JCRUISING"
    /* 0x30 */ CAM_SET_VERTICAL_CLIMB, // Large vertical climbs, such as Mountain Village wall or Pirates Fortress ladder. "CLIMBMAZE"
    /* 0x31 */ CAM_SET_SIDED, // Unknown, set with camDataId = -24 "SIDED"
    /* 0x32 */ CAM_SET_DUNGEON2, // Generic dungeon camera 2, used in various places "DUNGEON2"
    /* 0x33 */ CAM_SET_BOSS_ODOLWA, // Odolwa's Lair, also used in GBT entrance: "BOSS_SHIGE"
    /* 0x34 */ CAM_SET_KEEPBACK, // Unknown. Possibly related to climbing something? "KEEPBACK"
    /* 0x35 */ CAM_SET_CIRCLE6, // Used in select regions from Ikana "CIRCLE6"
    /* 0x36 */ CAM_SET_CIRCLE7, // Unknown "CIRCLE7"
    /* 0x37 */ CAM_SET_MINI_BOSS, // Used during the various minibosses of the "CHUBOSS"
    /* 0x38 */ CAM_SET_RFIXED1, // Talking to Koume stuck on the floor in woods of mystery "RFIXED1"
    /* 0x39 */ CAM_SET_TREASURE_CHEST_MINIGAME, // Treasure Chest Shop in East Clock Town, minigame location "TRESURE1"
    /* 0x3A */ CAM_SET_HONEY_AND_DARLING_1, // Honey and Darling Minigames "BOMBBASKET"
    /* 0x3B */ CAM_SET_CIRCLE8, // Used by Stone Tower moving platforms, Falling eggs in Marine Lab, Bugs into soilpatch cutscene "CIRCLE8"
    /* 0x3C */ CAM_SET_BIRDS_EYE_VIEW_1, // Camera is a top-down view. Used in Fisherman's minigame and Deku Palace "FUKAN1"
    /* 0x3D */ CAM_SET_DUNGEON3, // Generic dungeon camera 3, used in various places "DUNGEON3"
    /* 0x3E */ CAM_SET_TELESCOPE, // Observatory telescope and Curiosity Shop Peep-Hole "TELESCOPE"
    /* 0x3F */ CAM_SET_ROOM0, // Certain rooms eg. inside the clock tower "ROOM0"
    /* 0x40 */ CAM_SET_RCIRC0, // Used by a few NPC cutscenes, focus close on the NPC "RCIRC0"
    /* 0x41 */ CAM_SET_CIRCLE9, // Used by Sakon Hideout entrance and Deku Palace Maze "CIRCLE9"
    /* 0x42 */ CAM_SET_ONTHEPOLE, // Somewhere in Snowhead Temple and Woodfall Temple "ONTHEPOLE"
    /* 0x43 */ CAM_SET_INBUSH, // Various bush environments eg. grottos, Swamp Spider House, Termina Field grass bushes, Deku Palace near bean "INBUSH"
    /* 0x44 */ CAM_SET_BOSS_MAJORA, // Majora's Lair: "BOSS_LAST" 
    /* 0x45 */ CAM_SET_BOSS_TWINMOLD, // Twinmold's Lair: "BOSS_INI"
    /* 0x46 */ CAM_SET_BOSS_GOHT, // Goht's Lair: "BOSS_HAK" 
    /* 0x47 */ CAM_SET_BOSS_GYORG, // Gyorg's Lair: "BOSS_KON"
    /* 0x48 */ CAM_SET_CONNECT0, // Smoothly and gradually return camera to Player after a cutscene "CONNECT0"
    /* 0x49 */ CAM_SET_PINNACLE_ROCK, // Pinnacle Rock pit "MORAY"
    /* 0x4A */ CAM_SET_NORMAL2, // Generic camera 2, used in various places "NORMAL2"
    /* 0x4B */ CAM_SET_HONEY_AND_DARLING_2, // "BOMBBOWL"
    /* 0x4C */ CAM_SET_CIRCLEA, // Unknown, Circle 10 "CIRCLEA"
    /* 0x4D */ CAM_SET_WHIRLPOOL, // Great Bay Temple Central Room Whirlpool "WHIRLPOOL"
    /* 0x4E */ CAM_SET_CUCCO_SHACK, // "KOKKOGAME"
    /* 0x4F */ CAM_SET_GIANT, // Giants Mask in Twinmold's Lair "GIANT"
    /* 0x50 */ CAM_SET_SCENE0, // Entering doors to a new scene "SCENE0"
    /* 0x51 */ CAM_SET_ROOM1, // Certain rooms eg. some rooms in Stock Pot Inn "ROOM1"
    /* 0x52 */ CAM_SET_WATER2, // Swimming as Zora in Great Bay Temple "WATER2"
    /* 0x53 */ CAM_SET_WOODFALL_SWAMP, // Woodfall inside the swamp, but not on the platforms, "SOKONASI"
    /* 0x54 */ CAM_SET_FORCEKEEP, // Unknown "FORCEKEEP"
    /* 0x55 */ CAM_SET_PARALLEL1, // Unknown "PARALLEL1"
    /* 0x56 */ CAM_SET_START1, // Used when entering the lens cave "START1"
    /* 0x57 */ CAM_SET_ROOM2, // Certain rooms eg. Deku King's Chamber, Ocean Spider House "ROOM2"
    /* 0x58 */ CAM_SET_NORMAL4, // Generic camera 4, used in Ikana Graveyard "NORMAL4"
    /* 0x59 */ CAM_SET_ELEGY_SHELL, // cutscene after playing elegy of emptyness and spawning a shell "SHELL"
    /* 0x5A */ CAM_SET_DUNGEON4, // Used in Pirates Fortress Interior, hidden room near hookshot "DUNGEON4"
    /* 0x5B */ CAM_SET_MAX
} CameraSettingType;

typedef enum {
    /* 0x00 */ CAM_MODE_NORMAL, // "NORMAL"
    /* 0x01 */ CAM_MODE_JUMP, // "JUMP"
    /* 0x02 */ CAM_MODE_GORONDASH, // "GORONDASH"
    /* 0x03 */ CAM_MODE_DEKUSHOOT, // "NUTSSHOT"
    /* 0x04 */ CAM_MODE_BOWARROWZ, // "BOWARROWZ"
    /* 0x05 */ CAM_MODE_DEKUFLY, // "NUTSFLY"
    /* 0x06 */ CAM_MODE_FIRSTPERSON, // "SUBJECT"
    /* 0x07 */ CAM_MODE_FOLLOWBOOMERANG, // "BOOKEEPON"
    /* 0x08 */ CAM_MODE_ZORAFIN, // "ZORAFIN"
    /* 0x09 */ CAM_MODE_FOLLOWTARGET, // "KEEPON"
    /* 0x0A */ CAM_MODE_TARGET, // "PARALLEL"
    /* 0x0B */ CAM_MODE_TALK, // "TALK"
    /* 0x0C */ CAM_MODE_SLINGSHOT, // "PACHINCO"
    /* 0x0D */ CAM_MODE_BOWARROW, // "BOWARROW"
    /* 0x0E */ CAM_MODE_BATTLE, // "BATTLE"
    /* 0x0F */ CAM_MODE_DEKUHIDE, // "NUTSHIDE"
    /* 0x10 */ CAM_MODE_STILL, // "STILL"
    /* 0x11 */ CAM_MODE_CHARGE, // "CHARGE"
    /* 0x12 */ CAM_MODE_CLIMB, // "CLIMB"
    /* 0x13 */ CAM_MODE_CLIMBZ, // "CLIMBZ"
    /* 0x14 */ CAM_MODE_HOOKSHOT, // "FOOKSHOT"
    /* 0x15 */ CAM_MODE_FREEFALL, // "FREEFALL"
    /* 0x16 */ CAM_MODE_HANG, // "HANG"
    /* 0x17 */ CAM_MODE_HANGZ, // "HANGZ
    /* 0x18 */ CAM_MODE_PUSHPULL, // "PUSHPULL"
    /* 0x19 */ CAM_MODE_DEKUFLYZ, // "CNUTSFLYZ"
    /* 0x1A */ CAM_MODE_GORONJUMP, // "GORONJUMP"
    /* 0x1B */ CAM_MODE_BOOMERANG, // "BOOMERANG"
    /* 0x1C */ CAM_MODE_CHARGEZ, // "CHARGEZ"
    /* 0x1D */ CAM_MODE_ZORAFINZ, // "ZORAFINZ"
    /* 0x1E */ CAM_MODE_MAX
} CameraModeType;

typedef enum {
    /* 0x00 */ CAM_FUNC_NONE,
    /* 0x01 */ CAM_FUNC_NORMAL0,
    /* 0x02 */ CAM_FUNC_NORMAL1,
    /* 0x03 */ CAM_FUNC_NORMAL2,
    /* 0x04 */ CAM_FUNC_NORMAL3,
    /* 0x05 */ CAM_FUNC_NORMAL4,
    /* 0x06 */ CAM_FUNC_PARALLEL0,
    /* 0x07 */ CAM_FUNC_PARALLEL1,
    /* 0x08 */ CAM_FUNC_PARALLEL2,
    /* 0x09 */ CAM_FUNC_PARALLEL3,
    /* 0x0A */ CAM_FUNC_PARALLEL4,
    /* 0x0B */ CAM_FUNC_KEEPON0,
    /* 0x0C */ CAM_FUNC_KEEPON1,
    /* 0x0D */ CAM_FUNC_KEEPON2,
    /* 0x0E */ CAM_FUNC_KEEPON3,
    /* 0x0F */ CAM_FUNC_KEEPON4,
    /* 0x10 */ CAM_FUNC_SUBJECT0,
    /* 0x11 */ CAM_FUNC_SUBJECT1,
    /* 0x12 */ CAM_FUNC_SUBJECT2,
    /* 0x13 */ CAM_FUNC_SUBJECT3,
    /* 0x14 */ CAM_FUNC_SUBJECT4,
    /* 0x15 */ CAM_FUNC_JUMP0,
    /* 0x16 */ CAM_FUNC_JUMP1,
    /* 0x17 */ CAM_FUNC_JUMP2,
    /* 0x18 */ CAM_FUNC_JUMP3,
    /* 0x19 */ CAM_FUNC_JUMP4,
    /* 0x1A */ CAM_FUNC_BATTLE0,
    /* 0x1B */ CAM_FUNC_BATTLE1,
    /* 0x1C */ CAM_FUNC_BATTLE2,
    /* 0x1D */ CAM_FUNC_BATTLE3,
    /* 0x1E */ CAM_FUNC_BATTLE4,
    /* 0x1F */ CAM_FUNC_FIXED0,
    /* 0x20 */ CAM_FUNC_FIXED1,
    /* 0x21 */ CAM_FUNC_FIXED2,
    /* 0x22 */ CAM_FUNC_FIXED3,
    /* 0x23 */ CAM_FUNC_FIXED4,
    /* 0x24 */ CAM_FUNC_DATA0,
    /* 0x25 */ CAM_FUNC_DATA1,
    /* 0x26 */ CAM_FUNC_DATA2,
    /* 0x27 */ CAM_FUNC_DATA3,
    /* 0x28 */ CAM_FUNC_DATA4,
    /* 0x29 */ CAM_FUNC_UNIQUE0,
    /* 0x2A */ CAM_FUNC_UNIQUE1,
    /* 0x2B */ CAM_FUNC_UNIQUE2,
    /* 0x2C */ CAM_FUNC_UNIQUE3,
    /* 0x2D */ CAM_FUNC_UNIQUE4,
    /* 0x2E */ CAM_FUNC_UNIQUE5,
    /* 0x2F */ CAM_FUNC_UNIQUE6,
    /* 0x30 */ CAM_FUNC_UNIQUE7,
    /* 0x31 */ CAM_FUNC_UNIQUE8,
    /* 0x32 */ CAM_FUNC_UNIQUE9,
    /* 0x33 */ CAM_FUNC_DEMO0,
    /* 0x34 */ CAM_FUNC_DEMO1,
    /* 0x35 */ CAM_FUNC_DEMO2,
    /* 0x36 */ CAM_FUNC_DEMO3,
    /* 0x37 */ CAM_FUNC_DEMO4,
    /* 0x38 */ CAM_FUNC_DEMO5,
    /* 0x39 */ CAM_FUNC_DEMO6,
    /* 0x3A */ CAM_FUNC_DEMO7,
    /* 0x3B */ CAM_FUNC_DEMO8,
    /* 0x3C */ CAM_FUNC_DEMO9,
    /* 0x3D */ CAM_FUNC_SPECIAL0,
    /* 0x3E */ CAM_FUNC_SPECIAL1,
    /* 0x3F */ CAM_FUNC_SPECIAL2,
    /* 0x40 */ CAM_FUNC_SPECIAL3,
    /* 0x41 */ CAM_FUNC_SPECIAL4,
    /* 0x42 */ CAM_FUNC_SPECIAL5,
    /* 0x43 */ CAM_FUNC_SPECIAL6,
    /* 0x44 */ CAM_FUNC_SPECIAL7,
    /* 0x45 */ CAM_FUNC_SPECIAL8,
    /* 0x46 */ CAM_FUNC_SPECIAL9,
    /* 0x47 */ CAM_FUNC_MAX
} CameraFuncType;

typedef enum {
    /* 0x00 */ CAM_DATA_Y_OFFSET,
    /* 0x01 */ CAM_DATA_01,
    /* 0x02 */ CAM_DATA_02,
    /* 0x03 */ CAM_DATA_PITCH_TARGET,
    /* 0x04 */ CAM_DATA_04,
    /* 0x05 */ CAM_DATA_05,
    /* 0x06 */ CAM_DATA_YAW_DIFF_RANGE,
    /* 0x07 */ CAM_DATA_FOV,
    /* 0x08 */ CAM_DATA_08,
    /* 0x09 */ CAM_DATA_INTERFACE_FLAGS,
    /* 0x0A */ CAM_DATA_10,
    /* 0x0B */ CAM_DATA_11,
    /* 0x0C */ CAM_DATA_12,
    /* 0x0D */ CAM_DATA_13,
    /* 0x0E */ CAM_DATA_14,
    /* 0x0F */ CAM_DATA_15,
    /* 0x10 */ CAM_DATA_16,
    /* 0x11 */ CAM_DATA_17,
    /* 0x12 */ CAM_DATA_18,
    /* 0x13 */ CAM_DATA_19,
    /* 0x14 */ CAM_DATA_20,
    /* 0x15 */ CAM_DATA_21
} CameraDataType;

/*================================
 *   MISC DATA
 *================================
 */

// Camera Collision Check
typedef struct {
    /* 0x00 */ Vec3f pos;
    /* 0x0C */ Vec3f norm;
    /* 0x18 */ struct CollisionPoly* poly;
    /* 0x1C */ VecGeo geoNorm;
    /* 0x24 */ s32 bgId;
} CameraCollision; // size = 0x28

typedef struct {
    /* 0x00 */ Vec3f collisionClosePoint;
    /* 0x0C */ CameraCollision atEyeColChk;
    /* 0x34 */ CameraCollision eyeAtColChk;
    /* 0x5C */ f32 swingUpdateRate;
    /* 0x60 */ s16 pitch;
    /* 0x62 */ s16 yaw;
    /* 0x64 */ s16 unk_64;
    /* 0x66 */ s16 timer; // startSwingTimer
} SwingAnimation; // size = 0x68


/*================================
 *   Camera Update-Function Data
 *================================
 */

#define CAM_FUNCDATA_INTERFACE_FLAGS(interfaceFlags) \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }


/*================================
 *   Camera_Normal1() Data
 *================================
 */

#define CAM_FUNCDATA_NORM1(yOffset, data01, data02, pitchTarget, eyeStepScale, posStepScale, yawDiffRange, fov, data08, interfaceFlags) \
    { yOffset,      CAM_DATA_Y_OFFSET }, \
    { data01,       CAM_DATA_01 }, \
    { data02,       CAM_DATA_02 }, \
    { pitchTarget,  CAM_DATA_PITCH_TARGET }, \
    { eyeStepScale, CAM_DATA_04 }, \
    { posStepScale, CAM_DATA_05 }, \
    { yawDiffRange, CAM_DATA_YAW_DIFF_RANGE }, \
    { fov,          CAM_DATA_FOV }, \
    { data08,       CAM_DATA_08 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x00 */ f32 unk_00; // yOffset
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14; // attenuationYawDiffRange
    /* 0x18 */ f32 unk_18; // fovTarget
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ s16 unk_20; // pitchTarget
    /* 0x22 */ s16 interfaceFlags;
} Normal1ReadOnlyData; // size = 0x24

typedef struct {
    /* 0x00 */ f32 unk_00; // yPos
    /* 0x04 */ f32 unk_04; // xzSpeed
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A; // angle
    /* 0x0C */ s16 unk_0C; // flags (May be s32)
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ f32 unk_10; // set to float
} Normal1ReadWriteData; // size = 0x14

typedef struct {
    /* 0x00 */ Normal1ReadOnlyData roData;
    /* 0x24 */ Normal1ReadWriteData rwData;
} Normal1; // size = 0x38

#define NORMAL1_FLAG_0 (1 << 0)
#define NORMAL1_FLAG_1 (1 << 1)
#define NORMAL1_FLAG_2 (1 << 2)
#define NORMAL1_FLAG_3 (1 << 3)
#define NORMAL1_FLAG_4 (1 << 4)
#define NORMAL1_FLAG_5 (1 << 5)
#define NORMAL1_FLAG_6 (1 << 6)
#define NORMAL1_FLAG_7 (1 << 7)


/*================================
 *   Camera_Normal2() Data
 *================================
 */

// Unused Camera RemoteBomb Setting

#define CAM_FUNCDATA_NORM2(yOffset, data01, data02, pitchTarget, data04, fov, data08, interfaceFlags) \
    { yOffset,     CAM_DATA_Y_OFFSET }, \
    { data01,      CAM_DATA_01 }, \
    { data02,      CAM_DATA_02 }, \
    { pitchTarget, CAM_DATA_PITCH_TARGET }, \
    { data04,      CAM_DATA_04 }, \
    { fov,         CAM_DATA_FOV }, \
    { data08,      CAM_DATA_08 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

#define NORMAL2_FLAG_1 (1 << 1)


/*================================
 *   Camera_Normal3() Data
 *================================
 */

#define CAM_FUNCDATA_NORM3(yOffset, data01, data02, pitchTarget, eyeStepScale, posStepScale, fov, data08, interfaceFlags) \
    { yOffset,      CAM_DATA_Y_OFFSET }, \
    { data01,       CAM_DATA_01 }, \
    { data02,       CAM_DATA_02 }, \
    { pitchTarget,  CAM_DATA_PITCH_TARGET }, \
    { eyeStepScale, CAM_DATA_04 }, \
    { posStepScale, CAM_DATA_05 }, \
    { fov,          CAM_DATA_FOV }, \
    { data08,       CAM_DATA_08 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x00 */ f32 yOffset;
    /* 0x04 */ f32 distMin;
    /* 0x08 */ f32 distMax;
    /* 0x0C */ f32 yawUpdateRateInv;
    /* 0x10 */ f32 pitchUpdateRateInv;
    /* 0x14 */ f32 fovTarget;
    /* 0x18 */ f32 maxAtLERPScale;
    /* 0x1C */ s16 pitchTarget;
    /* 0x1E */ s16 interfaceFlags;
} Normal3ReadOnlyData; // size = 0x20

typedef struct {
    /* 0x00 */ f32 isZero; // set but unused
    /* 0x04 */ f32 yPosOffset;
    /* 0x08 */ s16 curPitch;
    /* 0x0A */ s16 yawUpdateRate;
    /* 0x0C */ s16 yawTimer;
    /* 0x0E */ s16 distTimer;
    /* 0x10 */ s16 flag;
    /* 0x12 */ s16 is1200; // set but unused
} Normal3ReadWriteData; // size = 0x14

typedef struct {
    /* 0x00 */ Normal3ReadOnlyData roData;
    /* 0x20 */ Normal3ReadWriteData rwData;
} Normal3; // size = 0x34

#define NORMAL3_FLAG_1 (1 << 1)
#define NORMAL3_FLAG_5 (1 << 5)
#define NORMAL3_FLAG_6 (1 << 6)
#define NORMAL3_FLAG_7 (1 << 7)


/*================================
 *   Camera_Normal0() Data
 *================================
 */

#define CAM_FUNCDATA_NORM0(yOffset, data01, data02, data21, data04, yawDiffRange, fov, data08, interfaceFlags) \
    { yOffset,      CAM_DATA_Y_OFFSET }, \
    { data01,       CAM_DATA_01 }, \
    { data02,       CAM_DATA_02 }, \
    { data21,       CAM_DATA_21 }, \
    { data04,       CAM_DATA_04 }, \
    { yawDiffRange, CAM_DATA_YAW_DIFF_RANGE }, \
    { fov,          CAM_DATA_FOV }, \
    { data08,       CAM_DATA_08 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ s16 interfaceFlags;
} Normal0ReadOnlyData; // size = 0x20

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ s16 unk_22;
    /* 0x24 */ f32 unk_24;
    /* 0x28 */ f32 unk_28;
    /* 0x2C */ s16 unk_2C;
} Normal0ReadWriteData; // size = 0x30

typedef struct {
    /* 0x00 */ Normal0ReadOnlyData roData;
    /* 0x20 */ Normal0ReadWriteData rwData;
} Normal0; // size = 0x50

#define NORMAL0_FLAG_0 (1 << 0)
#define NORMAL0_FLAG_2 (1 << 2)
#define NORMAL0_FLAG_4 (1 << 4)
#define NORMAL0_FLAG_7 (1 << 7)


/*================================
 *   Camera_Parallel1() Data
 *================================
 */

#define CAM_FUNCDATA_PARA1(yOffset, data01, data02, pitchTarget, data10, data04, data05, fov, data08, interfaceFlags, data11, data12) \
    { yOffset,      CAM_DATA_Y_OFFSET }, \
    { data01,       CAM_DATA_01 }, \
    { data02,       CAM_DATA_02 }, \
    { pitchTarget,  CAM_DATA_PITCH_TARGET }, \
    { data10,       CAM_DATA_10 }, \
    { data04,       CAM_DATA_04 }, \
    { data05,       CAM_DATA_05 }, \
    { fov,          CAM_DATA_FOV }, \
    { data08,       CAM_DATA_08 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }, \
    { data11,       CAM_DATA_11 }, \
    { data12,       CAM_DATA_12 }

// Same as above but with extra unused entry
#define CAM_FUNCDATA_PARA1_ALT(yOffset, data01, data02, pitchTarget, data10, data04, data05, fov, data08, interfaceFlags, data11, data12, data18) \
    { yOffset,      CAM_DATA_Y_OFFSET }, \
    { data01,       CAM_DATA_01 }, \
    { data02,       CAM_DATA_02 }, \
    { pitchTarget,  CAM_DATA_PITCH_TARGET }, \
    { data10,       CAM_DATA_10 }, \
    { data04,       CAM_DATA_04 }, \
    { data05,       CAM_DATA_05 }, \
    { fov,          CAM_DATA_FOV }, \
    { data08,       CAM_DATA_08 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }, \
    { data11,       CAM_DATA_11 }, \
    { data12,       CAM_DATA_12 }, \
    { data18,       CAM_DATA_18 }

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ s16 unk_22;
    /* 0x24 */ s16 unk_24;
    /* 0x26 */ s16 interfaceFlags;
} Parallel1ReadOnlyData; // size = 0x28

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 timer1;
    /* 0x10 */ Vec3f unk_10;
    /* 0x1C */ s16 unk_1C;
    /* 0x1E */ s16 unk_1E;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ s16 timer2;
    /* 0x24 */ s16 timer3;
    /* 0x26 */ s16 unk_26;
} Parallel1ReadWriteData; // size = 0x28

typedef struct {
    /* 0x00 */ Parallel1ReadOnlyData roData;
    /* 0x28 */ Parallel1ReadWriteData rwData;
} Parallel1; // size = 0x50

#define PARALLEL1_FLAG_0 (1 << 0)
#define PARALLEL1_FLAG_1 (1 << 1)
#define PARALLEL1_FLAG_2 (1 << 2)
#define PARALLEL1_FLAG_3 (1 << 3)
#define PARALLEL1_FLAG_4 (1 << 4)
#define PARALLEL1_FLAG_5 (1 << 5)
#define PARALLEL1_FLAG_6 (1 << 6)
#define PARALLEL1_FLAG_7 (1 << 7)


/*================================
 *   Camera_Parallel2() Data
 *================================
 */

// Unused Camera Maze Setting

#define CAM_FUNCDATA_PARA2(yOffset, data02, data01, pitchTarget, data04, fov, data08, interfaceFlags) \
    { yOffset,     CAM_DATA_Y_OFFSET }, \
    { data02,      CAM_DATA_02 }, \
    { data01,      CAM_DATA_01 }, \
    { pitchTarget, CAM_DATA_PITCH_TARGET }, \
    { data04,      CAM_DATA_04 }, \
    { fov,         CAM_DATA_FOV }, \
    { data08,      CAM_DATA_08 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

#define PARALLEL2_FLAG_1 (1 << 1)


/*================================
 *   Camera_Jump2() Data
 *================================
 */

#define CAM_FUNCDATA_JUMP2(yOffset, data01, data02, data20, data04, data05, fov, data08, interfaceFlags) \
    { yOffset,     CAM_DATA_Y_OFFSET }, \
    { data01,      CAM_DATA_01 }, \
    { data02,      CAM_DATA_02 }, \
    { data20,      CAM_DATA_20 }, \
    { data04,      CAM_DATA_04 }, \
    { data05,      CAM_DATA_05 }, \
    { fov,         CAM_DATA_FOV }, \
    { data08,      CAM_DATA_08 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ s16 interfaceFlags;
} Jump2ReadOnlyData; // size = 0x24

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s16 timer;
    /* 0x0E */ s16 unk_0E;
    /* 0x10 */ s32 unk_10; // unused?
    /* 0x1C */ s16 unk_1C;
} Jump2ReadWriteData; // size = 0x20

typedef struct {
    /* 0x00 */ Jump2ReadOnlyData roData;
    /* 0x24 */ Jump2ReadWriteData rwData;
} Jump2; // size = 0x44

#define JUMP2_FLAG_1 (1 << 1)
#define JUMP2_FLAG_2 (1 << 2)


/*================================
 *   Camera_Jump3() Data
 *================================
 */

#define CAM_FUNCDATA_JUMP3(yOffset, data01, data02, pitchTarget, data04, data05, yawDiffRange, fov, data08, interfaceFlags) \
    { yOffset,      CAM_DATA_Y_OFFSET }, \
    { data01,       CAM_DATA_01 }, \
    { data02,       CAM_DATA_02 }, \
    { pitchTarget,  CAM_DATA_PITCH_TARGET }, \
    { data04,       CAM_DATA_04 }, \
    { data05,       CAM_DATA_05 }, \
    { yawDiffRange, CAM_DATA_YAW_DIFF_RANGE }, \
    { fov,          CAM_DATA_FOV }, \
    { data08,       CAM_DATA_08 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x00 */ f32 unk_00; // yOffset
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14; // attenuationYawDiffRange
    /* 0x18 */ f32 unk_18; // fovTarget
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ s16 interfaceFlags;
} Jump3ReadOnlyData; // size = 0x24

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ s16 unk_04;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 timer1;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s32 timer2;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x1C */ s16 unk_1C;
} Jump3ReadWriteData; // size = 0x20

typedef struct {
    /* 0x00 */ Jump3ReadOnlyData roData; // yOffset
    /* 0x24 */ Jump3ReadWriteData rwData;
} Jump3; // size = 0x44

#define JUMP3_FLAG_0 (1 << 0)
#define JUMP3_FLAG_2 (1 << 2)
#define JUMP3_FLAG_3 (1 << 3)
#define JUMP3_FLAG_4 (1 << 4)
#define JUMP3_FLAG_5 (1 << 5)
#define JUMP3_FLAG_6 (1 << 6)
#define JUMP3_FLAG_7 (1 << 7)


/*================================
 *   Camera_Battle1() Data
 *================================
 */

#define CAM_FUNCDATA_BATT1(yOffset, data01, data02, data13, data14, data15, data16, data17, fov, data08, interfaceFlags, data11, data18) \
    { yOffset, CAM_DATA_Y_OFFSET }, \
    { data01,  CAM_DATA_01 }, \
    { data02,  CAM_DATA_02 }, \
    { data13,  CAM_DATA_13 }, \
    { data14,  CAM_DATA_14 }, \
    { data15,  CAM_DATA_15 }, \
    { data16,  CAM_DATA_16 }, \
    { data17,  CAM_DATA_17 }, \
    { fov,     CAM_DATA_FOV }, \
    { data08,  CAM_DATA_08 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }, \
    { data11,  CAM_DATA_11 }, \
    { data18,  CAM_DATA_18 }

typedef struct {
    /* 0x00 */ f32 yOffset;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 swingPitchInitial;
    /* 0x18 */ f32 swingPitchFinal;
    /* 0x1C */ f32 swingPitchAdj;
    /* 0x20 */ f32 fov;
    /* 0x24 */ f32 unk_24;
    /* 0x28 */ f32 unk_28;
    /* 0x2C */ f32 unk_2C;
    /* 0x30 */ s16 interfaceFlags;
} Battle1ReadOnlyData; // size = 0x34

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ Actor* unk_08; // target
    /* 0x0C */ s32 unk_0C; // unused?
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ s16 unk_14;
    /* 0x16 */ s16 unk_16;
    /* 0x18 */ s16 chargeTimer;
    /* 0x1A */ s16 unk_1A;
} Battle1ReadWriteData; // size = 0x1C

typedef struct {
    /* 0x00 */ Battle1ReadOnlyData roData;
    /* 0x24 */ Battle1ReadWriteData rwData;
} Battle1; // size = 0x50

#define BATTLE1_FLAG_0 (1 << 0)
#define BATTLE1_FLAG_1 (1 << 1)
#define BATTLE1_FLAG_4 (1 << 4)
#define BATTLE1_FLAG_7 (1 << 7)


/*================================
 *   Camera_KeepOn1() Data
 *================================
 */

#define CAM_FUNCDATA_KEEP1(yOffset, data01, data02, data13, data14, data15, data16, data17, fov, data08, interfaceFlags, data11) \
    { yOffset, CAM_DATA_Y_OFFSET }, \
    { data01,  CAM_DATA_01 }, \
    { data02,  CAM_DATA_02 }, \
    { data13,  CAM_DATA_13 }, \
    { data14,  CAM_DATA_14 }, \
    { data15,  CAM_DATA_15 }, \
    { data16,  CAM_DATA_16 }, \
    { data17,  CAM_DATA_17 }, \
    { fov,     CAM_DATA_FOV }, \
    { data08,  CAM_DATA_08 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }, \
    { data11,  CAM_DATA_11 }

typedef struct {
    /* 0x00 */ f32 yOffset;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ f32 unk_20;
    /* 0x24 */ f32 unk_24;
    /* 0x28 */ f32 unk_28;
    /* 0x2C */ s16 interfaceFlags;
} KeepOn1ReadOnlyData; // size = 0x30

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ Actor* unk_0C;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ s16 unk_14;
    /* 0x16 */ s16 unk_16;
    /* 0x18 */ s16 unk_18;
} KeepOn1ReadWriteData; // size = 0x1C

typedef struct {
    /* 0x00 */ KeepOn1ReadOnlyData roData;
    /* 0x30 */ KeepOn1ReadWriteData rwData;
} KeepOn1; // size = 0x4C

#define KEEPON1_FLAG_0 (1 << 0)
#define KEEPON1_FLAG_1 (1 << 1)
#define KEEPON1_FLAG_4 (1 << 4)
#define KEEPON1_FLAG_7 (1 << 7)


/*================================
 *   Camera_KeepOn3() Data
 *================================
 */

#define CAM_FUNCDATA_KEEP3(yOffset, data01, data02, data13, data14, data15, data16, data17, fov, data08, timer, interfaceFlags, data18) \
    { yOffset, CAM_DATA_Y_OFFSET }, \
    { data01,  CAM_DATA_01 }, \
    { data02,  CAM_DATA_02 }, \
    { data13,  CAM_DATA_13 }, \
    { data14,  CAM_DATA_14 }, \
    { data15,  CAM_DATA_15 }, \
    { data16,  CAM_DATA_16 }, \
    { data17,  CAM_DATA_17 }, \
    { fov,     CAM_DATA_FOV }, \
    { data08,  CAM_DATA_08 }, \
    { timer,  CAM_DATA_04 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }, \
    { data18,  CAM_DATA_18 }

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ f32 unk_20;
    /* 0x24 */ f32 unk_24;
    /* 0x28 */ f32 unk_28;
    /* 0x2C */ s16 timer; // initial timer, stored at twice the value used in code
    /* 0x2E */ s16 interfaceFlags;
} KeepOn3ReadOnlyData; // size = 0x30

typedef struct {
    /* 0x00 */ f32 unk_00; // Vec3f?
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ Actor* unk_0C;
    /* 0x10 */ Vec3f unk_10;
    /* 0x1C */ s16 timer;
} KeepOn3ReadWriteData; // size = 0x20

typedef struct {
    /* 0x00 */ KeepOn3ReadOnlyData roData;
    /* 0x30 */ KeepOn3ReadWriteData rwData;
} KeepOn3; // size = 0x50

#define KEEPON3_FLAG_5 (1 << 5)
#define KEEPON3_FLAG_6 (1 << 6)
#define KEEPON3_FLAG_7 (1 << 7)


/*================================
 *   Camera_KeepOn4() Data
 *================================
 */

#define CAM_FUNCDATA_KEEP4(yOffset, data01, pitchTarget, data10, data18, fov, interfaceFlags, data04, timer) \
    { yOffset,     CAM_DATA_Y_OFFSET }, \
    { data01,      CAM_DATA_01 }, \
    { pitchTarget, CAM_DATA_PITCH_TARGET }, \
    { data10,      CAM_DATA_10 }, \
    { data18,      CAM_DATA_18 }, \
    { fov,         CAM_DATA_FOV }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }, \
    { data04,      CAM_DATA_04 }, \
    { timer,      CAM_DATA_12 }

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ s16 interfaceFlags;
    /* 0x1E */ s16 timer;
} KeepOn4ReadOnlyData; // size = 0x20

typedef struct {
    /* 0x00 */ f32 unk_00; // Vec3f?
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ Actor* unk_0C;
    /* 0x10 */ s16 unk_10;
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ s16 timer;
    /* 0x16 */ s16 unk_16;
    /* 0x18 */ s16 unk_18;
} KeepOn4ReadWriteData; // size = 0x1C

typedef struct {
    /* 0x00 */ KeepOn4ReadOnlyData roData;
    /* 0x20 */ KeepOn4ReadWriteData rwData;
} KeepOn4; // size = 0x3C

#define KEEPON4_FLAG_1 (1 << 1)
#define KEEPON4_FLAG_2 (1 << 2)
#define KEEPON4_FLAG_3 (1 << 3)
#define KEEPON4_FLAG_6 (1 << 6)


/*================================
 *   Camera_Fixed1() Data
 *================================
 */

#define CAM_FUNCDATA_FIXD1(yOffset, data04, fov, interfaceFlags) \
    { yOffset,     CAM_DATA_Y_OFFSET }, \
    { data04,      CAM_DATA_04 }, \
    { fov,         CAM_DATA_FOV }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x0 */ f32 unk_00;
    /* 0x4 */ f32 unk_04;
    /* 0x8 */ f32 fov;
    /* 0xC */ s16 interfaceFlags;
} Fixed1ReadOnlyData; // size = 0x10

typedef struct {
    /* 0x00 */ PosRot eyePosRotTarget;
    /* 0x14 */ s16 fov;
    /* 0x18 */ Actor* focalActor;
} Fixed1ReadWriteData; // size = 0x1C

typedef struct {
    /* 0x00 */ Fixed1ReadOnlyData roData;
    /* 0x10 */ Fixed1ReadWriteData rwData;
} Fixed1; // size = 0x2C

#define FIXED1_FLAG_4 (1 << 4)


/*================================
 *   Camera_Fixed2() Data
 *================================
 */

#define CAM_FUNCDATA_FIXD2(yOffset, data01, data02, data04, data05, fov, interfaceFlags) \
    { yOffset,     CAM_DATA_Y_OFFSET }, \
    { data01,      CAM_DATA_01 }, \
    { data02,      CAM_DATA_02 }, \
    { data04,      CAM_DATA_04 }, \
    { data05,      CAM_DATA_05 }, \
    { fov,         CAM_DATA_FOV }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ s16 interfaceFlags;
} Fixed2ReadOnlyData; // size = 0x1C

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ s16 unk_1C;
} Fixed2ReadWriteData; // size = 0x20

typedef struct {
    /* 0x00 */ Fixed2ReadOnlyData roData;
    /* 0x1C */ Fixed2ReadWriteData rwData;
} Fixed2; // size = 0x3C

#define FIXED2_FLAG_0 (1 << 0)
#define FIXED2_FLAG_1 (1 << 1)
#define FIXED2_FLAG_2 (1 << 2)
#define FIXED2_FLAG_3 (1 << 3)
#define FIXED2_FLAG_4 (1 << 4)
#define FIXED2_FLAG_5 (1 << 5)
#define FIXED2_FLAG_6 (1 << 6)
#define FIXED2_FLAG_7 (1 << 7)


/*================================
 *   Camera_Fixed3() Data
 *================================
 */

#define CAM_FUNCDATA_FIXD3(interfaceFlags) \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }


/*================================
 *   Camera_Subject1() Data
 *================================
 */

#define CAM_FUNCDATA_SUBJ1(yOffset, data01, data02, data04, data19, data17, data18, fov, interfaceFlags) \
    { yOffset, CAM_DATA_Y_OFFSET }, \
    { data01,  CAM_DATA_01 }, \
    { data02,  CAM_DATA_02 }, \
    { data04,  CAM_DATA_04 }, \
    { data19,  CAM_DATA_19 }, \
    { data17,  CAM_DATA_17 }, \
    { data18,  CAM_DATA_18 }, \
    { fov,     CAM_DATA_FOV }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ f32 unk_1C;
    /* 0x20 */ s16 interfaceFlags;
} Subject1ReadOnlyData; // size = 0x24

typedef struct {
    /* 0x0 */ f32 unk_00;
    /* 0x4 */ s16 unk_04; // yaw
    /* 0x6 */ s16 unk_06; // pitch
    /* 0x8 */ s16 timer;
} Subject1ReadWriteData; // size = 0xC

typedef struct {
    /* 0x00 */ Subject1ReadOnlyData roData;
    /* 0x24 */ Subject1ReadWriteData rwData;
} Subject1; // size = 0x30

#define SUBJECT1_FLAG_4 (1 << 4)


/*================================
 *   Camera_Unique2() Data
 *================================
 */

#define CAM_FUNCDATA_UNIQ2(yOffset, data01, fov, interfaceFlags) \
    { yOffset,     CAM_DATA_Y_OFFSET }, \
    { data01,      CAM_DATA_01 }, \
    { fov,         CAM_DATA_FOV }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ f32 unk_04;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ s16 interfaceFlags;
} Unique2ReadOnlyData; // size = 0x14

typedef struct {
    /* 0x0 */ f32 unk_00;
    /* 0x4 */ s16 unk_04;
} Unique2ReadWriteData; // size = 0x8

typedef struct {
    /* 0x00 */ Unique2ReadOnlyData roData;
    /* 0x14 */ Unique2ReadWriteData rwData;
} Unique2; // size = 0x1C

#define UNIQUE2_FLAG_0 (1 << 0)
#define UNIQUE2_FLAG_1 (1 << 1)
#define UNIQUE2_FLAG_4 (1 << 4)
#define UNIQUE2_FLAG_5 (1 << 5)


/*================================
 *   Camera_Unique0() Data
 *================================
 */

#define CAM_FUNCDATA_UNIQ0(yOffset, data04, interfaceFlags) \
    { yOffset,     CAM_DATA_Y_OFFSET }, \
    { data04,      CAM_DATA_04 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x0 */ f32 unk_00;
    /* 0x4 */ f32 unk_04;
    /* 0x8 */ s16 interfaceFlags;
} Unique0ReadOnlyData; // size = 0xC

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ Vec3f unk_0C;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ InfiniteLine unk_1C;
    /* 0x34 */ Vec3s unk_34;
    /* 0x3A */ s16 unk_3A;
    /* 0x3C */ s16 timer;
    /* 0x3E */ s16 unk_3E;
} Unique0ReadWriteData; // size = 0x40

typedef struct {
    /* 0x0 */ Unique0ReadOnlyData roData;
    /* 0xC */ Unique0ReadWriteData rwData;
} Unique0; // size = 0x4C

#define UNIQUE0_FLAG_0 (1 << 0)
#define UNIQUE0_FLAG_1 (1 << 1)
#define UNIQUE0_FLAG_4 (1 << 4)


/*================================
 *   Camera_Unique6() Data
 *================================
 */

typedef struct {
    /* 0x0 */ s16 interfaceFlags;
} Unique6ReadOnlyData; // size = 0x4

typedef struct {
    /* 0x0 */ Unique6ReadOnlyData roData;
} Unique6; // size = 0x4

#define UNIQUE6_FLAG_0 (1 << 0)


/*================================
 *   Camera_Unique7() Data
 *================================
 */

#define CAM_FUNCDATA_UNIQ7(fov, interfaceFlags) \
    { fov,      CAM_DATA_FOV }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }


/*================================
 *   Camera_Demo1() Data
 *================================
 */

typedef struct {
    /* 0x0 */ s16 interfaceFlags;
} Demo1ReadOnlyData; // size = 0x4

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ VecGeo unk_0C;
    /* 0x14 */ VecGeo unk_14;
    /* 0x1C */ s16 unk_1C;
} Demo1ReadWriteData; // size = 0x20

typedef struct {
    /* 0x0 */ Demo1ReadOnlyData roData;
    /* 0x4 */ Demo1ReadWriteData rwData;
} Demo1; // size = 0x24


/*================================
 *   Camera_Demo2() Data
 *================================
 */

#define CAM_FUNCDATA_DEMO2(fov, data08, interfaceFlags) \
    { fov,    CAM_DATA_FOV }, \
    { data08, CAM_DATA_08 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x0 */ f32 fov;
    /* 0x4 */ f32 unk_04; // unused
    /* 0x8 */ s16 interfaceFlags;
} Demo2ReadOnlyData; // size = 0xC

typedef struct {
    /* 0x00 */ Vec3f initialAt;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ s16 animFrame;
    /* 0x12 */ s16 yawDir;
} Demo2ReadWriteData; // size = 0x14

typedef struct {
    /* 0x8 */ Demo2ReadOnlyData roData;
    /* 0xC */ Demo2ReadWriteData rwData;
} Demo2; // size = 0x20


/*================================
 *   Camera_Demo3() Data
 *================================
 */

typedef struct {
    /* 0x0 */ s16 interfaceFlags;
} Demo3ReadOnlyData; // size = 0x4

typedef struct {
    /* 0x0 */ f32 unk_00;
    /* 0x4 */ s16 unk_04;
    /* 0x6 */ s16 timer;
} Demo3ReadWriteData; // size = 0x8

typedef struct {
    /* 0x0 */ Demo3ReadOnlyData roData;
    /* 0x4 */ Demo3ReadWriteData rwData;
} Demo3; // size = 0xC


/*================================
 *   Camera_Demo4() Data
 *================================
 */

typedef struct {
    /* 0x0 */ s16 interfaceFlags;
} Demo4ReadOnlyData; // size = 0x4

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ VecGeo unk_18;
    /* 0x20 */ s16 unk_20;
    /* 0x22 */ s16 timer;
} Demo4ReadWriteData; // size = 0x24

typedef struct {
    /* 0x0 */ Demo4ReadOnlyData roData;
    /* 0x4 */ Demo4ReadWriteData rwData;
} Demo4; // size = 0x28


/*================================
 *   Camera_Demo5() Data
 *================================
 */

typedef struct {
    /* 0x0 */ s16 interfaceFlags;
} Demo5ReadOnlyData; // size = 0x4

typedef struct {
    /* 0x00 */ Vec3f unk_00;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14;
    /* 0x18 */ f32 unk_18;
    /* 0x1C */ VecGeo unk_1C;
    /* 0x24 */ s16 unk_24;
    /* 0x26 */ s16 timer;
} Demo5ReadWriteData; // size = 0x28

typedef struct {
    /* 0x0 */ Demo5ReadOnlyData roData;
    /* 0x4 */ Demo5ReadWriteData rwData;
} Demo5; // size = 0x2C


/*================================
 *   Camera_Demo0() Data
 *================================
 */

typedef struct {
    /* 0x0 */ s16 interfaceFlags;
} Demo0ReadOnlyData; // size = 0x4

typedef struct {
    /* 0x00 */ f32 unk_00;
    /* 0x04 */ VecGeo subAtToEye;
    /* 0x0C */ VecGeo atToEye;
    /* 0x14 */ s16 unk_14;
    /* 0x16 */ s16 unk_16;
    /* 0x18 */ s16 unk_18;
    /* 0x1A */ s16 timer;
} Demo0ReadWriteData; // size = 0x1C

typedef struct {
    /* 0x0 */ Demo0ReadOnlyData roData;
    /* 0x4 */ Demo0ReadWriteData rwData;
} Demo0; // size = 0x20


/*================================
 *   Camera_Special5() Data
 *================================
 */

#define CAM_FUNCDATA_SPEC5(yOffset, eyeDist, minDistForRot, fov, atMaxLERPScale, timerInit, pitch, interfaceFlags) \
    { yOffset,        CAM_DATA_Y_OFFSET }, \
    { eyeDist,        CAM_DATA_01 }, \
    { minDistForRot,  CAM_DATA_02 }, \
    { fov,            CAM_DATA_PITCH_TARGET }, \
    { atMaxLERPScale, CAM_DATA_FOV }, \
    { timerInit,      CAM_DATA_08 }, \
    { pitch,          CAM_DATA_12 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x00 */ f32 yOffset;
    /* 0x04 */ f32 eyeDist;
    /* 0x08 */ f32 minDistForRot;
    /* 0x0C */ f32 fovTarget;
    /* 0x10 */ f32 atMaxLERPScale;
    /* 0x14 */ s16 timerInit;
    /* 0x16 */ s16 pitch;
    /* 0x18 */ s16 interfaceFlags;
} Special5ReadOnlyData; // size = 0x1C

typedef struct {
    /* 0x0 */ s16 animTimer;
} Special5ReadWriteData; // size = 0x4

typedef struct {
    /* 0x00 */ Special5ReadOnlyData roData;
    /* 0x1C */ Special5ReadWriteData rwData;
} Special5; // size = 0x20


/*================================
 *   DOOR Data
 *================================
 */

// For functions that deal with doors, an extra struct is added to the paramData

typedef struct {
    /* 0x0 */ Actor* doorActor;
    /* 0x4 */ s16 bgCamIndex;
    /* 0x6 */ union {
        Vec3s eye; // position of the camera while exiting a spiral staircase
        struct {
            s16 timer1; // timer while camera is fixed in front of the door
            s16 timer2; // timer while camera is behind the door looking at player
            s16 timer3; // timer while camera turns around to face forward
        };
    };
} DoorParams; // size = 0xC

#define CAM_GET_DOOR_PARAMS(type) &((type*)camera->paramData)->doorParams


/*================================
 *   Camera_Special8() Data
 *================================
 */

#define CAM_FUNCDATA_SPEC8(yOffset, eyeStepScale, posStepScale, fov, spiralDoorCsLength, interfaceFlags) \
    { yOffset,            CAM_DATA_Y_OFFSET }, \
    { eyeStepScale,       CAM_DATA_04 }, \
    { posStepScale,       CAM_DATA_05 }, \
    { fov,                CAM_DATA_FOV }, \
    { spiralDoorCsLength, CAM_DATA_12 }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x00 */ f32 yOffset;
    /* 0x04 */ f32 eyeStepScale;
    /* 0x08 */ f32 posStepScale;
    /* 0x0C */ f32 fov;
    /* 0x10 */ s16 spiralDoorCsLength;
    /* 0x12 */ s16 interfaceFlags;
} Special8ReadOnlyData; // size = 0x14

typedef struct {
    /* 0x0 */ Vec3f eye;
    /* 0xC */ s16 spiralDoorCsFrame; // 1/5th of the length of the cutscene
    /* 0xE */ s16 fov;
} Special8ReadWriteData; // size = 0x10

typedef struct {
    /* 0x00 */ Special8ReadOnlyData roData;
    /* 0x14 */ Special8ReadWriteData rwData;
} Special8; // size = 0x24

#define SPECIAL8_FLAG_0 (1 << 0)
#define SPECIAL8_FLAG_3 (1 << 3)


/*================================
 *   Camera_Special9() Data
 *================================
 */

#define CAM_FUNCDATA_SPEC9(yOffset, fov, interfaceFlags) \
    { yOffset, CAM_DATA_Y_OFFSET }, \
    { fov,     CAM_DATA_FOV }, \
    { interfaceFlags, CAM_DATA_INTERFACE_FLAGS }

typedef struct {
    /* 0x0 */ f32 yOffset;
    /* 0x4 */ f32 fov;
    /* 0x8 */ s16 interfaceFlags;
} Special9ReadOnlyData; // size = 0xC

typedef struct {
    /* 0x0 */ s16 unk_00;
} Special9ReadWriteData; // size = 0x4

typedef struct {
    /* 0x0 */ Special9ReadOnlyData roData;
    /* 0xC */ Special9ReadWriteData rwData;
} Special9; // size = 0x10

#define SPECIAL9_FLAG_0 (1 << 0)
#define SPECIAL9_FLAG_1 (1 << 1)
#define SPECIAL9_FLAG_3 (1 << 3)

typedef union {
    Normal1 norm1;
    Normal3 norm3;
    Normal0 norm0;
    Parallel1 para1;
    Jump2 jump2;
    Jump3 jump3;
    Battle1 batt1;
    KeepOn1 keep1;
    KeepOn3 keep3;
    KeepOn4 keep4;
    Fixed1 fixd1;
    Fixed2 fixd2;
    Subject1 subj1;
    Unique2 uniq2;
    Unique0 uniq0;
    Unique6 uniq6;
    Demo1 demo1;
    Demo2 demo2;
    Demo3 demo3;
    Demo4 demo4;
    Demo5 demo5;
    Demo0 demo0;
    Special5 spec5;
    struct {
        /* 0x0 */ DoorParams doorParams;
        /* 0xC */ union {
            Special8 spec8;
            Special9 spec9;
        };
    };
} CamParamData; // size = 0x50

/*================================
 *   MAIN CAMERA STRUCT
 *================================
 */

typedef struct Camera {
    /* 0x000 */ CamParamData paramData; // update function data that's reset every time a new update function is switched to
    /* 0x050 */ Vec3f at;
    /* 0x05C */ Vec3f eye;
    /* 0x068 */ Vec3f up;
    /* 0x074 */ Vec3f eyeNext;
    /* 0x080 */ Vec3f quakeOffset;
    /* 0x08C */ struct PlayState* play;
    /* 0x090 */ Actor* focalActor; // the actor the camera is focused on. Most often Player, but can be any actor
    /* 0x094 */ PosRot focalActorPosRot;
    /* 0x0A8 */ Actor* target; // the actor that is being z-targeted
    /* 0x0AC */ PosRot targetPosRot;
    /* 0x0C0 */ f32 rUpdateRateInv;
    /* 0x0C4 */ f32 pitchUpdateRateInv;
    /* 0x0C8 */ f32 yawUpdateRateInv;
    /* 0x0CC */ f32 yOffsetUpdateRate;
    /* 0x0D0 */ f32 xzOffsetUpdateRate;
    /* 0x0D4 */ f32 fovUpdateRate;
    /* 0x0D8 */ f32 xzSpeed;
    /* 0x0DC */ f32 dist;
    /* 0x0E0 */ f32 speedRatio;
    /* 0x0E4 */ Vec3f focalActorAtOffset; // Offset between the camera's at Pos and the focalActor's Pos
    /* 0x0F0 */ Vec3f unk_0F0; // Offset between the focalActor's Pos and the camera's focalActor's Pos from the previous frame
    /* 0x0FC */ f32 fov;
    /* 0x100 */ f32 atLerpStepScale;
    /* 0x104 */ f32 focalActorFloorHeight;
    /* 0x108 */ Vec3f floorNorm;
    /* 0x114 */ f32 waterYPos;
    /* 0x118 */ s32 waterPrevBgCamDataId;
    /* 0x11C */ s32 waterPrevCamSetting;
    /* 0x120 */ s16 waterQuakeId;
    /* 0x122 */ s16 unk122;
    /* 0x124 */ void* data0;
    /* 0x128 */ void* data1;
    /* 0x12C */ s16 data2;
    /* 0x12E */ s16 data3;
    /* 0x130 */ s16 uid;
    /* 0x132 */ UNK_TYPE1 unk_132[2];
    /* 0x134 */ Vec3s inputDir;
    /* 0x13A */ Vec3s camDir;
    /* 0x140 */ s16 status;
    /* 0x142 */ s16 setting;
    /* 0x144 */ s16 mode;
    /* 0x146 */ s16 bgId;
    /* 0x148 */ s16 bgCamIndex;
    /* 0x14A */ s16 behaviorFlags;
    /* 0x14C */ s16 stateFlags;
    /* 0x14E */ s16 childCamId;
    /* 0x150 */ s16 doorTimer1; // a door timer used when door cam is indexed from bgCamIndex
    /* 0x152 */ s16 unk152;
    /* 0x154 */ s16 prevSetting;
    /* 0x156 */ s16 nextCamSceneDataId;
    /* 0x158 */ s16 nextBgId;
    /* 0x15A */ s16 roll;
    /* 0x15C */ s16 viewFlags;
    /* 0x15E */ s16 animState; // Determines the current state of the current camera behavior function
    /* 0x160 */ s16 timer; // Unused remnant of OoT: originally destoryed subCamera when timer ran out
    /* 0x162 */ s16 doorTimer2; // a door timer used when door cam is indexed from bgCamIndex
    /* 0x164 */ s16 camId;
    /* 0x166 */ s16 prevBgCamDataId;
    /* 0x168 */ s16 unk168;
    /* 0x16C */ Vec3f meshActorPos;
} Camera; // size = 0x178

Camera* Camera_Create(struct View* view, struct CollisionContext* colCtx, struct PlayState* play);
void Camera_Destroy(Camera* camera);
void Camera_Init(Camera* camera, struct View* view, struct CollisionContext* colCtx, struct PlayState* play);
void func_800DDFE0(Camera* camera);
void Camera_InitFocalActorSettings(Camera* camera, struct Actor* focalActor);
s32 Camera_ChangeStatus(Camera* camera, s16 status);
s32 Camera_UpdateWater(Camera* camera);
void Camera_EarthquakeDay3(Camera* camera);
s32 Camera_UpdateHotRoom(Camera* camera);
s32 Camera_SetSwordDistortion(Camera* camera);
s32 Camera_RequestGiantsMaskSetting(Camera* camera);
Vec3s Camera_Update(Camera* camera);
s32 func_800DF498(Camera* camera);
s32 Camera_ChangeModeFlags(Camera* camera, s16 mode, u8 forceChange);
s32 Camera_ChangeMode(Camera* camera, s16 mode);
s32 Camera_CheckValidMode(Camera* camera, s16 mode);
s16 Camera_ChangeSettingFlags(Camera* camera, s16 setting, s16 flags);
s32 Camera_ChangeSetting(Camera* camera, s16 setting);
s32 Camera_ChangeActorCsCamIndex(Camera* camera, s32 bgCamIndex);
Vec3s Camera_GetInputDir(Camera* camera);
s16 Camera_GetInputDirPitch(Camera* camera);
s16 Camera_GetInputDirYaw(Camera* camera);
Vec3s Camera_GetCamDir(Camera* camera);
s16 Camera_GetCamDirPitch(Camera* camera);
s16 Camera_GetCamDirYaw(Camera* camera);
s32 Camera_AddQuake(Camera* camera, s32 arg1, s16 y, s32 countdown);
s32 Camera_SetViewParam(Camera* camera, s32 viewFlag, void* param);
s32 Camera_UnsetViewFlag(Camera* camera, s16 viewFlag);
s32 Camera_OverwriteStateFlags(Camera* camera, s16 stateFlags);
s16 Camera_SetStateFlag(Camera* camera, s16 flags);
s16 Camera_UnsetStateFlag(Camera* camera, s16 flags);
s32 Camera_ChangeDoorCam(Camera* camera, struct Actor* doorActor, s16 bgCamIndex, f32 arg3, s16 timer1, s16 timer2, s16 timer3);
s32 Camera_Copy(Camera* dstCam, Camera* srcCam);
s32 Camera_IsDbgCamEnabled(void);
Vec3f Camera_GetQuakeOffset(Camera* camera);
void Camera_SetCameraData(Camera* camera, s16 setDataFlags, void* data0, void* data1, s16 data2, s16 data3);
s32 Camera_GetNegOne(void);
s16 Camera_SetFinishedFlag(Camera* camera);
void Camera_SetFocalActor(Camera* camera, struct Actor* actor);
void Camera_SetTargetActor(Camera* camera, struct Actor* actor);
f32 Camera_GetWaterYPos(Camera* camera);
void func_800E0348(Camera* camera);

#endif
