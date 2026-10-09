#ifndef Z64PLAYER_H
#define Z64PLAYER_H

#ifdef __cplusplus
extern "C" {
#define this thisx
#endif

#include "alignment.h"
#include "PR/os.h"
#include "z64actor.h"
#include "z64interface.h"
#include "z64item.h"
#include "z64light.h"
#include "face_change.h"

struct Player;
struct PlayState;

#define PLAYER_GET_BG_CAM_INDEX(thisx) ((thisx)->params & 0xFF)
#define PLAYER_GET_START_MODE(thisx) (((thisx)->params & 0xF00) >> 8)

typedef enum PlayerStartMode {
    /*  0x0 */ PLAYER_START_MODE_NOTHING, // Update is empty and draw function is NULL, nothing occurs. Useful in cutscenes, for example.
    /*  0x1 */ PLAYER_START_MODE_TIME_TRAVEL, // OoT Leftover. Arriving from time travel. Automatically adjusts by age.
    /*  0x2 */ PLAYER_START_MODE_BLUE_WARP, // Arriving from a blue warp.
    /*  0x3 */ PLAYER_START_MODE_DOOR, // Unused. Use a door immediately if one is nearby. If no door is in usable range, a softlock occurs.
    /*  0x4 */ PLAYER_START_MODE_GROTTO, // Arriving from a grotto, launched upward from the ground.
    /*  0x5 */ PLAYER_START_MODE_WARP_SONG, // OoT Leftover. Arriving from a warp song.
    /*  0x6 */ PLAYER_START_MODE_OWL, // Arriving from an Owl Save or Song of Soaring (both overworld and dungeon warps).
    /*  0x7 */ PLAYER_START_MODE_KNOCKED_OVER, // Knocked over on the ground and flashing red.
    /*  0x8 */ PLAYER_START_MODE_8,
    /*  0x9 */ PLAYER_START_MODE_9,
    /*  0xA */ PLAYER_START_MODE_A,
    /*  0xB */ PLAYER_START_MODE_B,
    /*  0xC */ PLAYER_START_MODE_TELESCOPE,
    /*  0xD */ PLAYER_START_MODE_D,
    /*  0xE */ PLAYER_START_MODE_E,
    /*  0xF */ PLAYER_START_MODE_F,
    /* 0x10 */ PLAYER_START_MODE_MAX // Note: By default, this param has 4 bits allocated. The max value is 16.
} PlayerStartMode;

#define PLAYER_PARAMS(startBgCamIndex, startMode) ((startBgCamIndex & 0xFF) | ((startMode & 0xF) << 8))

typedef enum PlayerShield {
    /* 0 */ PLAYER_SHIELD_NONE,
    /* 1 */ PLAYER_SHIELD_HEROS_SHIELD,
    /* 2 */ PLAYER_SHIELD_MIRROR_SHIELD,
    /* 3 */ PLAYER_SHIELD_MAX
} PlayerShield;

typedef enum PlayerBoots {
    /* 0 */ PLAYER_BOOTS_FIERCE_DEITY,
    /* 1 */ PLAYER_BOOTS_HYLIAN,
    /* 2 */ PLAYER_BOOTS_GIANT,
    /* 3 */ PLAYER_BOOTS_DEKU,
    /* 4 */ PLAYER_BOOTS_ZORA_LAND,
    /* 5 */ PLAYER_BOOTS_ZORA_UNDERWATER,
    /* 6 */ PLAYER_BOOTS_GORON,
    /* 7 */ PLAYER_BOOTS_7,
    /* 8 */ PLAYER_BOOTS_MAX
} PlayerBoots;

typedef enum PlayerStrength {
    /* 0 */ PLAYER_STRENGTH_DEKU,
    /* 1 */ PLAYER_STRENGTH_HUMAN,
    /* 2 */ PLAYER_STRENGTH_ZORA,
    /* 3 */ PLAYER_STRENGTH_GORON,
    /* 4 */ PLAYER_STRENGTH_MAX
} PlayerStrength;

typedef enum PlayerTransformation {
    /* 0 */ PLAYER_FORM_FIERCE_DEITY,
    /* 1 */ PLAYER_FORM_GORON,
    /* 2 */ PLAYER_FORM_ZORA,
    /* 3 */ PLAYER_FORM_DEKU,
    /* 4 */ PLAYER_FORM_HUMAN,
    /* 5 */ PLAYER_FORM_MAX
} PlayerTransformation;

typedef enum {
    /* 0 */ PLAYER_ENV_HAZARD_NONE,
    /* 1 */ PLAYER_ENV_HAZARD_HOTROOM,
    /* 2 */ PLAYER_ENV_HAZARD_UNDERWATER_FLOOR,
    /* 3 */ PLAYER_ENV_HAZARD_SWIMMING,
    /* 4 */ PLAYER_ENV_HAZARD_UNDERWATER_FREE
} PlayerEnvHazard;

typedef enum PlayerIdleType {
    /* -0x1 */ PLAYER_IDLE_CRIT_HEALTH = -1,
    /*  0x0 */ PLAYER_IDLE_DEFAULT,
    /*  0x1 */ PLAYER_IDLE_FIDGET
} PlayerIdleType;

/*
 * Current known usages for PLAYER_IA_MINUS1:
 *  1. With TalkExchange requests, used to continue a current conversation after a textbox is closed
 *  2. In `func_80123810` as a return value representing the offer is declined or invalid
 *  3. Used as an item action to return the previously held item after player is done shielding
 */

typedef enum PlayerItemAction {
    /*   -1 */ PLAYER_IA_MINUS1 = -1, // TODO: determine usages with more player docs, possibly split into separate values (see known usages above)
    /* 0x00 */ PLAYER_IA_NONE,
    /* 0x01 */ PLAYER_IA_LAST_USED,
    /* 0x02 */ PLAYER_IA_FISHING_ROD,
    /* 0x03 */ PLAYER_IA_SWORD_MIN,
    /* 0x03 */ PLAYER_IA_SWORD_KOKIRI = PLAYER_IA_SWORD_MIN,
    /* 0x04 */ PLAYER_IA_SWORD_RAZOR,
    /* 0x05 */ PLAYER_IA_SWORD_GILDED,
    /* 0x06 */ PLAYER_IA_SWORD_TWO_HANDED,
    /* 0x07 */ PLAYER_IA_DEKU_STICK,
    /* 0x08 */ PLAYER_IA_ZORA_BOOMERANG,
    /* 0x09 */ PLAYER_IA_BOW,
    /* 0x0A */ PLAYER_IA_BOW_FIRE,
    /* 0x0B */ PLAYER_IA_BOW_ICE,
    /* 0x0C */ PLAYER_IA_BOW_LIGHT,
    /* 0x0D */ PLAYER_IA_HOOKSHOT,
    /* 0x0E */ PLAYER_IA_EXPLOSIVE_MIN,
    /* 0x0E */ PLAYER_IA_BOMB = PLAYER_IA_EXPLOSIVE_MIN,
    /* 0x0F */ PLAYER_IA_POWDER_KEG,
    /* 0x10 */ PLAYER_IA_BOMBCHU,
    /* 0x11 */ PLAYER_IA_11,
    /* 0x12 */ PLAYER_IA_DEKU_NUT,
    /* 0x13 */ PLAYER_IA_PICTOGRAPH_BOX,
    /* 0x14 */ PLAYER_IA_OCARINA,
    /* 0x15 */ PLAYER_IA_BOTTLE_MIN,
    /* 0x15 */ PLAYER_IA_BOTTLE_EMPTY = PLAYER_IA_BOTTLE_MIN,
    /* 0x16 */ PLAYER_IA_BOTTLE_FISH,
    /* 0x17 */ PLAYER_IA_BOTTLE_SPRING_WATER,
    /* 0x18 */ PLAYER_IA_BOTTLE_HOT_SPRING_WATER,
    /* 0x19 */ PLAYER_IA_BOTTLE_ZORA_EGG,
    /* 0x1A */ PLAYER_IA_BOTTLE_DEKU_PRINCESS,
    /* 0x1B */ PLAYER_IA_BOTTLE_GOLD_DUST,
    /* 0x1C */ PLAYER_IA_BOTTLE_1C,
    /* 0x1D */ PLAYER_IA_BOTTLE_SEAHORSE,
    /* 0x1E */ PLAYER_IA_BOTTLE_MUSHROOM,
    /* 0x1F */ PLAYER_IA_BOTTLE_HYLIAN_LOACH,
    /* 0x20 */ PLAYER_IA_BOTTLE_BUG,
    /* 0x21 */ PLAYER_IA_BOTTLE_POE,
    /* 0x22 */ PLAYER_IA_BOTTLE_BIG_POE,
    /* 0x23 */ PLAYER_IA_BOTTLE_POTION_RED,
    /* 0x24 */ PLAYER_IA_BOTTLE_POTION_BLUE,
    /* 0x25 */ PLAYER_IA_BOTTLE_POTION_GREEN,
    /* 0x26 */ PLAYER_IA_BOTTLE_MILK,
    /* 0x27 */ PLAYER_IA_BOTTLE_MILK_HALF,
    /* 0x28 */ PLAYER_IA_BOTTLE_CHATEAU,
    /* 0x29 */ PLAYER_IA_BOTTLE_FAIRY,
    /* 0x2A */ PLAYER_IA_MOONS_TEAR,
    /* 0x2B */ PLAYER_IA_DEED_LAND,
    /* 0x2C */ PLAYER_IA_ROOM_KEY,
    /* 0x2D */ PLAYER_IA_LETTER_TO_KAFEI,
    /* 0x2E */ PLAYER_IA_MAGIC_BEANS,
    /* 0x2F */ PLAYER_IA_DEED_SWAMP,
    /* 0x30 */ PLAYER_IA_DEED_MOUNTAIN,
    /* 0x31 */ PLAYER_IA_DEED_OCEAN,
    /* 0x32 */ PLAYER_IA_32,
    /* 0x33 */ PLAYER_IA_LETTER_MAMA,
    /* 0x34 */ PLAYER_IA_34,
    /* 0x35 */ PLAYER_IA_35,
    /* 0x36 */ PLAYER_IA_PENDANT_OF_MEMORIES,
    /* 0x37 */ PLAYER_IA_37,
    /* 0x38 */ PLAYER_IA_38,
    /* 0x39 */ PLAYER_IA_39,
    /* 0x3A */ PLAYER_IA_MASK_MIN,
    /* 0x3A */ PLAYER_IA_MASK_TRUTH = PLAYER_IA_MASK_MIN,
    /* 0x3B */ PLAYER_IA_MASK_KAFEIS_MASK,
    /* 0x3C */ PLAYER_IA_MASK_ALL_NIGHT,
    /* 0x3D */ PLAYER_IA_MASK_BUNNY,
    /* 0x3E */ PLAYER_IA_MASK_KEATON,
    /* 0x3F */ PLAYER_IA_MASK_GARO,
    /* 0x40 */ PLAYER_IA_MASK_ROMANI,
    /* 0x41 */ PLAYER_IA_MASK_CIRCUS_LEADER,
    /* 0x42 */ PLAYER_IA_MASK_POSTMAN,
    /* 0x43 */ PLAYER_IA_MASK_COUPLE,
    /* 0x44 */ PLAYER_IA_MASK_GREAT_FAIRY,
    /* 0x45 */ PLAYER_IA_MASK_GIBDO,
    /* 0x46 */ PLAYER_IA_MASK_DON_GERO,
    /* 0x47 */ PLAYER_IA_MASK_KAMARO,
    /* 0x48 */ PLAYER_IA_MASK_CAPTAIN,
    /* 0x49 */ PLAYER_IA_MASK_STONE,
    /* 0x4A */ PLAYER_IA_MASK_BREMEN,
    /* 0x4B */ PLAYER_IA_MASK_BLAST,
    /* 0x4C */ PLAYER_IA_MASK_SCENTS,
    /* 0x4D */ PLAYER_IA_MASK_GIANT,
    /* 0x4E */ PLAYER_IA_MASK_TRANSFORMATION_MIN,
    /* 0x4E */ PLAYER_IA_MASK_FIERCE_DEITY = PLAYER_IA_MASK_TRANSFORMATION_MIN,
    /* 0x4F */ PLAYER_IA_MASK_GORON,
    /* 0x50 */ PLAYER_IA_MASK_ZORA,
    /* 0x51 */ PLAYER_IA_MASK_DEKU,
    /* 0x51 */ PLAYER_IA_MASK_MAX = PLAYER_IA_MASK_DEKU,
    /* 0x52 */ PLAYER_IA_LENS_OF_TRUTH,
    /* 0x53 */ PLAYER_IA_MAX
} PlayerItemAction;

// Relies on B swords related item actions to be contiguous
#define GET_B_SWORD_FROM_IA(itemAction) ((itemAction) - PLAYER_IA_SWORD_MIN + 1)

typedef enum PlayerBButtonSword {
    /* 0 */ PLAYER_B_SWORD_NONE,
    /* 1 */ PLAYER_B_SWORD_KOKIRI = GET_B_SWORD_FROM_IA(PLAYER_IA_SWORD_KOKIRI),
    /* 2 */ PLAYER_B_SWORD_RAZOR = GET_B_SWORD_FROM_IA(PLAYER_IA_SWORD_RAZOR),
    /* 3 */ PLAYER_B_SWORD_GILDED = GET_B_SWORD_FROM_IA(PLAYER_IA_SWORD_GILDED),
    /* 4 */ PLAYER_B_SWORD_MAX
} PlayerBButtonSword;

// Relies on melee weapon related item actions to be contiguous
#define GET_MELEE_WEAPON_FROM_IA(itemAction) ((itemAction) - PLAYER_IA_SWORD_MIN + 1)

typedef enum PlayerMeleeWeapon {
    /* 0 */ PLAYER_MELEEWEAPON_NONE,
    /* 1 */ PLAYER_MELEEWEAPON_SWORD_KOKIRI = GET_MELEE_WEAPON_FROM_IA(PLAYER_IA_SWORD_KOKIRI),
    /* 2 */ PLAYER_MELEEWEAPON_SWORD_RAZOR = GET_MELEE_WEAPON_FROM_IA(PLAYER_IA_SWORD_RAZOR),
    /* 3 */ PLAYER_MELEEWEAPON_SWORD_GILDED = GET_MELEE_WEAPON_FROM_IA(PLAYER_IA_SWORD_GILDED),
    /* 4 */ PLAYER_MELEEWEAPON_SWORD_TWO_HANDED = GET_MELEE_WEAPON_FROM_IA(PLAYER_IA_SWORD_TWO_HANDED),
    /* 5 */ PLAYER_MELEEWEAPON_DEKU_STICK = GET_MELEE_WEAPON_FROM_IA(PLAYER_IA_DEKU_STICK),
    /* 6 */ PLAYER_MELEEWEAPON_ZORA_BOOMERANG = GET_MELEE_WEAPON_FROM_IA(PLAYER_IA_ZORA_BOOMERANG),
    /* 7 */ PLAYER_MELEEWEAPON_MAX
} PlayerMeleeWeapon;

// Relies on mask item actions to be contiguous
#define GET_MASK_FROM_IA(itemAction) ((itemAction) - (PLAYER_IA_MASK_MIN - 1))
#define GET_IA_FROM_MASK(mask) ((mask) + (PLAYER_IA_MASK_MIN - 1))

typedef enum PlayerMask {
    /* 0x00 */ PLAYER_MASK_NONE,
    /* 0x01 */ PLAYER_MASK_TRUTH = GET_MASK_FROM_IA(PLAYER_IA_MASK_TRUTH),
    /* 0x02 */ PLAYER_MASK_KAFEIS_MASK = GET_MASK_FROM_IA(PLAYER_IA_MASK_KAFEIS_MASK),
    /* 0x03 */ PLAYER_MASK_ALL_NIGHT = GET_MASK_FROM_IA(PLAYER_IA_MASK_ALL_NIGHT),
    /* 0x04 */ PLAYER_MASK_BUNNY = GET_MASK_FROM_IA(PLAYER_IA_MASK_BUNNY),
    /* 0x05 */ PLAYER_MASK_KEATON = GET_MASK_FROM_IA(PLAYER_IA_MASK_KEATON),
    /* 0x06 */ PLAYER_MASK_GARO = GET_MASK_FROM_IA(PLAYER_IA_MASK_GARO),
    /* 0x07 */ PLAYER_MASK_ROMANI = GET_MASK_FROM_IA(PLAYER_IA_MASK_ROMANI),
    /* 0x08 */ PLAYER_MASK_CIRCUS_LEADER = GET_MASK_FROM_IA(PLAYER_IA_MASK_CIRCUS_LEADER),
    /* 0x09 */ PLAYER_MASK_POSTMAN = GET_MASK_FROM_IA(PLAYER_IA_MASK_POSTMAN),
    /* 0x0A */ PLAYER_MASK_COUPLE = GET_MASK_FROM_IA(PLAYER_IA_MASK_COUPLE),
    /* 0x0B */ PLAYER_MASK_GREAT_FAIRY = GET_MASK_FROM_IA(PLAYER_IA_MASK_GREAT_FAIRY),
    /* 0x0C */ PLAYER_MASK_GIBDO = GET_MASK_FROM_IA(PLAYER_IA_MASK_GIBDO),
    /* 0x0D */ PLAYER_MASK_DON_GERO = GET_MASK_FROM_IA(PLAYER_IA_MASK_DON_GERO),
    /* 0x0E */ PLAYER_MASK_KAMARO = GET_MASK_FROM_IA(PLAYER_IA_MASK_KAMARO),
    /* 0x0F */ PLAYER_MASK_CAPTAIN = GET_MASK_FROM_IA(PLAYER_IA_MASK_CAPTAIN),
    /* 0x10 */ PLAYER_MASK_STONE = GET_MASK_FROM_IA(PLAYER_IA_MASK_STONE),
    /* 0x11 */ PLAYER_MASK_BREMEN = GET_MASK_FROM_IA(PLAYER_IA_MASK_BREMEN),
    /* 0x12 */ PLAYER_MASK_BLAST = GET_MASK_FROM_IA(PLAYER_IA_MASK_BLAST),
    /* 0x13 */ PLAYER_MASK_SCENTS = GET_MASK_FROM_IA(PLAYER_IA_MASK_SCENTS),
    /* 0x14 */ PLAYER_MASK_GIANT = GET_MASK_FROM_IA(PLAYER_IA_MASK_GIANT),
    /* 0x15 */ PLAYER_MASK_FIERCE_DEITY = GET_MASK_FROM_IA(PLAYER_IA_MASK_FIERCE_DEITY),
    /* 0x16 */ PLAYER_MASK_GORON = GET_MASK_FROM_IA(PLAYER_IA_MASK_GORON),
    /* 0x17 */ PLAYER_MASK_ZORA = GET_MASK_FROM_IA(PLAYER_IA_MASK_ZORA),
    /* 0x18 */ PLAYER_MASK_DEKU = GET_MASK_FROM_IA(PLAYER_IA_MASK_DEKU),
    /* 0x19 */ PLAYER_MASK_MAX
} PlayerMask;

// Relies on bottle-related item actions to be contiguous
#define GET_BOTTLE_FROM_IA(itemAction) ((itemAction) - PLAYER_IA_BOTTLE_MIN)

typedef enum PlayerBottle {
    /* -1 */ PLAYER_BOTTLE_NONE = -1,
    /*  0 */ PLAYER_BOTTLE_EMPTY = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_EMPTY),
    /*  1 */ PLAYER_BOTTLE_FISH = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_FISH),
    /*  2 */ PLAYER_BOTTLE_SPRING_WATER = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_SPRING_WATER),
    /*  3 */ PLAYER_BOTTLE_HOT_SPRING_WATER = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_HOT_SPRING_WATER),
    /*  4 */ PLAYER_BOTTLE_ZORA_EGG = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_ZORA_EGG),
    /*  5 */ PLAYER_BOTTLE_DEKU_PRINCESS = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_DEKU_PRINCESS),
    /*  6 */ PLAYER_BOTTLE_GOLD_DUST = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_GOLD_DUST),
    /*  7 */ PLAYER_BOTTLE_1C = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_1C),
    /*  8 */ PLAYER_BOTTLE_SEAHORSE = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_SEAHORSE),
    /*  9 */ PLAYER_BOTTLE_MUSHROOM = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_MUSHROOM),
    /* 10 */ PLAYER_BOTTLE_HYLIAN_LOACH = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_HYLIAN_LOACH),
    /* 11 */ PLAYER_BOTTLE_BUG = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_BUG),
    /* 12 */ PLAYER_BOTTLE_POE = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_POE),
    /* 13 */ PLAYER_BOTTLE_BIG_POE = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_BIG_POE),
    /* 14 */ PLAYER_BOTTLE_POTION_RED = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_POTION_RED),
    /* 15 */ PLAYER_BOTTLE_POTION_BLUE = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_POTION_BLUE),
    /* 16 */ PLAYER_BOTTLE_POTION_GREEN = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_POTION_GREEN),
    /* 17 */ PLAYER_BOTTLE_MILK = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_MILK),
    /* 18 */ PLAYER_BOTTLE_MILK_HALF = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_MILK_HALF),
    /* 19 */ PLAYER_BOTTLE_CHATEAU = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_CHATEAU),
    /* 20 */ PLAYER_BOTTLE_FAIRY = GET_BOTTLE_FROM_IA(PLAYER_IA_BOTTLE_FAIRY),
    /* 21 */ PLAYER_BOTTLE_MAX
} PlayerBottle;

// Relies on explosive-related item actions to be contiguous
#define GET_EXPLOSIVE_FROM_IA(itemAction) ((itemAction) - PLAYER_IA_EXPLOSIVE_MIN)

typedef enum PlayerExplosive {
    /* -1 */ PLAYER_EXPLOSIVE_NONE = -1,
    /*  0 */ PLAYER_EXPLOSIVE_BOMB = GET_EXPLOSIVE_FROM_IA(PLAYER_IA_BOMB),
    /*  1 */ PLAYER_EXPLOSIVE_POWDER_KEG = GET_EXPLOSIVE_FROM_IA(PLAYER_IA_POWDER_KEG),
    /*  2 */ PLAYER_EXPLOSIVE_BOMBCHU = GET_EXPLOSIVE_FROM_IA(PLAYER_IA_BOMBCHU),
    /*  3 */ PLAYER_EXPLOSIVE_MAX
} PlayerExplosive;

// Relies on sword item actions to be contiguous
#define GET_SWORD_FROM_IA(itemAction) ((itemAction) - PLAYER_IA_SWORD_MIN)

typedef enum PlayerSword {
    /* -1 */ PLAYER_SWORD_NONE = -1,
    /*  0 */ PLAYER_SWORD_KOKIRI = GET_SWORD_FROM_IA(PLAYER_IA_SWORD_KOKIRI),
    /*  1 */ PLAYER_SWORD_RAZOR = GET_SWORD_FROM_IA(PLAYER_IA_SWORD_RAZOR),
    /*  2 */ PLAYER_SWORD_GILDED = GET_SWORD_FROM_IA(PLAYER_IA_SWORD_GILDED),
    /*  3 */ PLAYER_SWORD_TWO_HANDED = GET_SWORD_FROM_IA(PLAYER_IA_SWORD_TWO_HANDED),
    /*  4 */ PLAYER_SWORD_MAX
} PlayerSword;

typedef enum PlayerMeleeWeaponAnimation {
    /*  0 */ PLAYER_MWA_FORWARD_SLASH_1H,      // Vertical one-handed slash
    /*  1 */ PLAYER_MWA_FORWARD_SLASH_2H,      // Vertical two-handed slash
    /*  2 */ PLAYER_MWA_FORWARD_COMBO_1H,      // Third vertical one-handed slash
    /*  3 */ PLAYER_MWA_FORWARD_COMBO_2H,      // Third vertical two-handed slash
    /*  4 */ PLAYER_MWA_RIGHT_SLASH_1H,        // Horizontal one-handed slash
    /*  5 */ PLAYER_MWA_RIGHT_SLASH_2H,        // Horizontal two-handed slash
    /*  6 */ PLAYER_MWA_RIGHT_COMBO_1H,        // Third horizontal one-handed slash
    /*  7 */ PLAYER_MWA_RIGHT_COMBO_2H,        // Third horizontal two-handed slash
    /*  8 */ PLAYER_MWA_LEFT_SLASH_1H,         // Targeted one-handed rightwalk slash
    /*  9 */ PLAYER_MWA_LEFT_SLASH_2H,         // Targeted two-handed rightwalk slash
    /* 10 */ PLAYER_MWA_LEFT_COMBO_1H,         // Third targeted one-handed rightwalk slash
    /* 11 */ PLAYER_MWA_LEFT_COMBO_2H,         // Third targeted two-handed rightwalk slash
    /* 12 */ PLAYER_MWA_STAB_1H,               // Crouch stab/targeted one-handed stab
    /* 13 */ PLAYER_MWA_STAB_2H,               // Targeted two-handed stab
    /* 14 */ PLAYER_MWA_STAB_COMBO_1H,         // Third targeted one-handed stab
    /* 15 */ PLAYER_MWA_STAB_COMBO_2H,         // Third targeted two-handed stab
    // These animations result in double damage
    /* 16 */ PLAYER_MWA_FLIPSLASH_START,       // unused
    /* 17 */ PLAYER_MWA_JUMPSLASH_START,       // Start of jumpslash
    /* 18 */ PLAYER_MWA_ZORA_JUMPKICK_START,   // Start of Zora jump attack
    /* 19 */ PLAYER_MWA_FLIPSLASH_FINISH,      // unused
    /* 20 */ PLAYER_MWA_JUMPSLASH_FINISH,      // End of jumpslash
    /* 21 */ PLAYER_MWA_ZORA_JUMPKICK_FINISH,  // End of Zora jump attack
    /* 22 */ PLAYER_MWA_BACKSLASH_RIGHT,       // unused
    /* 23 */ PLAYER_MWA_BACKSLASH_LEFT,        // unused
    /* 24 */ PLAYER_MWA_GORON_PUNCH_LEFT,      // Goron punch
    /* 25 */ PLAYER_MWA_GORON_PUNCH_RIGHT,     // Second Goron punch
    /* 26 */ PLAYER_MWA_GORON_PUNCH_BUTT,      // Goron butt punch
    /* 27 */ PLAYER_MWA_ZORA_PUNCH_LEFT,       // Zora punch
    /* 28 */ PLAYER_MWA_ZORA_PUNCH_COMBO,      // Second Zora punch
    /* 29 */ PLAYER_MWA_ZORA_PUNCH_KICK,       // Zora kick
    /* 30 */ PLAYER_MWA_SPIN_ATTACK_1H,        // Half-charged one-handed spin
    /* 31 */ PLAYER_MWA_SPIN_ATTACK_2H,        // Half-charged two-handed spin
    /* 32 */ PLAYER_MWA_BIG_SPIN_1H,           // Fully-charged one-handed spin
    /* 33 */ PLAYER_MWA_BIG_SPIN_2H,           // Fully-charged two-handed spin
    /* 34 */ PLAYER_MWA_MAX
} PlayerMeleeWeaponAnimation;

typedef enum PlayerMeleeWeaponState {
    /* -1 */ PLAYER_MELEE_WEAPON_STATE_MINUS_1 = -1,
    /*  0 */ PLAYER_MELEE_WEAPON_STATE_0,
    /*  1 */ PLAYER_MELEE_WEAPON_STATE_1
} PlayerMeleeWeaponState;

typedef enum PlayerDoorType {
    /* -1 */ PLAYER_DOORTYPE_TALKING = -1, // Displays a message instead of opening
    /*  0 */ PLAYER_DOORTYPE_NONE,
    /*  1 */ PLAYER_DOORTYPE_HANDLE,
    /*  2 */ PLAYER_DOORTYPE_SLIDING,
    /*  3 */ PLAYER_DOORTYPE_FAKE, // Unused? OoT leftover (?)
    /*  4 */ PLAYER_DOORTYPE_STAIRCASE,
    /*  5 */ PLAYER_DOORTYPE_PROXIMITY
} PlayerDoorType;

// Some player animations are played at this reduced speed, for reasons yet unclear.
// Perhaps to compress animation data?
// This is called "adjusted" for now.
// z_en_horse also has many instances of this adjusted speed
#define PLAYER_ANIM_ADJUSTED_SPEED (2.0f / 3.0f)
#define PLAYER_ANIM_NORMAL_SPEED   (3.0f / 3.0f)

typedef enum PlayerAnimType {
    /* 0 */ PLAYER_ANIMTYPE_DEFAULT, // DEFAULT
    /* 1 */ PLAYER_ANIMTYPE_1,
    /* 2 */ PLAYER_ANIMTYPE_2, // Shield but no sword?
    /* 3 */ PLAYER_ANIMTYPE_3, // Two hand weapon
    /* 4 */ PLAYER_ANIMTYPE_4,
    /* 5 */ PLAYER_ANIMTYPE_5,
    /* 6 */ PLAYER_ANIMTYPE_MAX
} PlayerAnimType;

typedef enum PlayerAnimGroup {
    /*  0 */ PLAYER_ANIMGROUP_wait, // STANDING_IDLE
    /*  1 */ PLAYER_ANIMGROUP_walk, // WALKING
    /*  2 */ PLAYER_ANIMGROUP_run, // RUNNING
    /*  3 */ PLAYER_ANIMGROUP_damage_run,
    /*  4 */ PLAYER_ANIMGROUP_waitL,
    /*  5 */ PLAYER_ANIMGROUP_waitR, // Z-Targeting ?
    /*  6 */ PLAYER_ANIMGROUP_wait2waitR, // Start or finishing some animation
    /*  7 */ PLAYER_ANIMGROUP_normal2fighter, // DRAW_MELEE_WEAPON
    /*  8 */ PLAYER_ANIMGROUP_doorA_free, // OPEN_DOOR_LEFT_FIERCE_DEITY
    /*  9 */ PLAYER_ANIMGROUP_doorA, // OPEN_DOOR_LEFT_HUMAN
    /* 10 */ PLAYER_ANIMGROUP_doorB_free, // OPEN_DOOR_RIGHT_FIERCE_DEITY
    /* 11 */ PLAYER_ANIMGROUP_doorB, // OPEN_DOOR_RIGHT_HUMAN
    /* 12 */ PLAYER_ANIMGROUP_carryB, // GRABBING
    /* 13 */ PLAYER_ANIMGROUP_landing, // FALLING/LANDING?
    /* 14 */ PLAYER_ANIMGROUP_short_landing, // landing from short distances?
    /* 15 */ PLAYER_ANIMGROUP_landing_roll, // ROLLING
    /* 16 */ PLAYER_ANIMGROUP_hip_down, // BONK/BONKING
    /* 17 */ PLAYER_ANIMGROUP_walk_endL,
    /* 18 */ PLAYER_ANIMGROUP_walk_endR,
    /* 19 */ PLAYER_ANIMGROUP_defense, // START_SHIELDING
    /* 20 */ PLAYER_ANIMGROUP_defense_wait, // SHIELDING
    /* 21 */ PLAYER_ANIMGROUP_defense_end, // STOP_SHIELDING/END_SHIELDING
    /* 22 */ PLAYER_ANIMGROUP_side_walk, // SLOW_SIDE_WALK?
    /* 23 */ PLAYER_ANIMGROUP_side_walkL, // SIDE_WALK
    /* 24 */ PLAYER_ANIMGROUP_side_walkR, // SIDE_WALK_RIGHT
    /* 25 */ PLAYER_ANIMGROUP_45_turn,
    /* 26 */ PLAYER_ANIMGROUP_waitL2wait,
    /* 27 */ PLAYER_ANIMGROUP_waitR2wait,
    /* 28 */ PLAYER_ANIMGROUP_throw, // THROWING
    /* 29 */ PLAYER_ANIMGROUP_put, // PUT_DOWN_OBJECT?
    /* 30 */ PLAYER_ANIMGROUP_back_walk, // back walking slow?
    /* 31 */ PLAYER_ANIMGROUP_check,
    /* 32 */ PLAYER_ANIMGROUP_check_wait,
    /* 33 */ PLAYER_ANIMGROUP_check_end,
    /* 34 */ PLAYER_ANIMGROUP_pull_start, // PULLING_START
    /* 35 */ PLAYER_ANIMGROUP_pulling, // PULLING
    /* 36 */ PLAYER_ANIMGROUP_pull_end, // PULLING_END
    /* 37 */ PLAYER_ANIMGROUP_fall_up, // climbing from a ledge?
    /* 38 */ PLAYER_ANIMGROUP_jump_climb_hold, // ledge
    /* 39 */ PLAYER_ANIMGROUP_jump_climb_wait, // ledge
    /* 40 */ PLAYER_ANIMGROUP_jump_climb_up, // climbing from a ledge
    /* 41 */ PLAYER_ANIMGROUP_down_slope_slip_end,
    /* 42 */ PLAYER_ANIMGROUP_up_slope_slip_end,
    /* 43 */ PLAYER_ANIMGROUP_nwait,
    /* 44 */ PLAYER_ANIMGROUP_MAX
} PlayerAnimGroup;

typedef enum PlayerModelType {
    // left hand
    /*   0 */ PLAYER_MODELTYPE_LH_OPEN,
    /*   1 */ PLAYER_MODELTYPE_LH_CLOSED,
    /*   2 */ PLAYER_MODELTYPE_LH_ONE_HAND_SWORD,
    /*   3 */ PLAYER_MODELTYPE_LH_TWO_HAND_SWORD,
    /*   4 */ PLAYER_MODELTYPE_LH_4,
    /*   5 */ PLAYER_MODELTYPE_LH_BOTTLE,
    // right hand
    /*   6 */ PLAYER_MODELTYPE_RH_OPEN,
    /*   7 */ PLAYER_MODELTYPE_RH_CLOSED,
    /*   8 */ PLAYER_MODELTYPE_RH_SHIELD,
    /*   9 */ PLAYER_MODELTYPE_RH_BOW,
    /*  10 */ PLAYER_MODELTYPE_RH_INSTRUMENT,
    /*  11 */ PLAYER_MODELTYPE_RH_HOOKSHOT,
    // sheath
    /*  12 */ PLAYER_MODELTYPE_SHEATH_12,
    /*  13 */ PLAYER_MODELTYPE_SHEATH_13,
    /*  14 */ PLAYER_MODELTYPE_SHEATH_14,
    /*  15 */ PLAYER_MODELTYPE_SHEATH_15,
    // waist
    /*  16 */ PLAYER_MODELTYPE_WAIST,
    /*  17 */ PLAYER_MODELTYPE_17, // NULL?
    /*  18 */ PLAYER_MODELTYPE_MAX,
    /* 255 */ PLAYER_MODELTYPE_RH_FF = 0xFF // disable shield collider, cutscene-specific
} PlayerModelType;

typedef struct PlayerModelIndices {
    /* 0x0 */ u8 modelAnimType; // PlayerAnimType enum
    /* 0x1 */ u8 leftHandType; // PlayerModelType enum
    /* 0x2 */ u8 rightHandType; // PlayerModelType enum
    /* 0x3 */ u8 sheathType; // PlayerModelType enum
    /* 0x4 */ u8 waistType; // PlayerModelType enum
} PlayerModelIndices; // size = 0x5

typedef enum PlayerModelGroup {
    /*  0 */ PLAYER_MODELGROUP_0,
    /*  1 */ PLAYER_MODELGROUP_1,
    /*  2 */ PLAYER_MODELGROUP_ONE_HAND_SWORD,
    /*  3 */ PLAYER_MODELGROUP_DEFAULT, // non-specific models, for items that don't have particular link models
    /*  4 */ PLAYER_MODELGROUP_4,
    /*  5 */ PLAYER_MODELGROUP_TWO_HAND_SWORD,
    /*  6 */ PLAYER_MODELGROUP_BOW,
    /*  7 */ PLAYER_MODELGROUP_EXPLOSIVES,
    /*  8 */ PLAYER_MODELGROUP_8,
    /*  9 */ PLAYER_MODELGROUP_HOOKSHOT,
    /* 10 */ PLAYER_MODELGROUP_DEKU_STICK,
    /* 11 */ PLAYER_MODELGROUP_INSTRUMENT,
    /* 12 */ PLAYER_MODELGROUP_BOTTLE,
    /* 13 */ PLAYER_MODELGROUP_13,
    /* 14 */ PLAYER_MODELGROUP_ZORA_BOOMERANG,
    /* 15 */ PLAYER_MODELGROUP_MAX
} PlayerModelGroup;

typedef struct PlayerFaceIndices {
    /* 0x0 */ u8 eyeIndex;
    /* 0x1 */ u8 mouthIndex;
} PlayerFaceIndices; // size = 0x2

typedef enum PlayerEyes {
    /* 0 */ PLAYER_EYES_OPEN,
    /* 1 */ PLAYER_EYES_HALF,
    /* 2 */ PLAYER_EYES_CLOSED,
    /* 3 */ PLAYER_EYES_RIGHT,
    /* 4 */ PLAYER_EYES_LEFT,
    /* 5 */ PLAYER_EYES_UP,
    /* 6 */ PLAYER_EYES_DOWN,
    /* 7 */ PLAYER_EYES_WINCING, // For Goron, this is a surprised eye
    /* 8 */ PLAYER_EYES_MAX
} PlayerEyes;

typedef enum PlayerMouth {
    /* 0 */ PLAYER_MOUTH_CLOSED,
    /* 1 */ PLAYER_MOUTH_HALF,
    /* 2 */ PLAYER_MOUTH_OPEN,
    /* 3 */ PLAYER_MOUTH_SMILE,
    /* 4 */ PLAYER_MOUTH_MAX
} PlayerMouth;

typedef enum PlayerFace {
    /*  0 */ PLAYER_FACE_NEUTRAL,
    /*  1 */ PLAYER_FACE_NEUTRAL_BLINKING_HALF,
    /*  2 */ PLAYER_FACE_NEUTRAL_BLINKING_CLOSED,
    /*  3 */ PLAYER_FACE_NEUTRAL_2,
    /*  4 */ PLAYER_FACE_NEUTRAL_BLINKING_HALF_2,
    /*  5 */ PLAYER_FACE_NEUTRAL_BLINKING_CLOSED_2,
    /*  6 */ PLAYER_FACE_LOOK_LEFT,
    /*  7 */ PLAYER_FACE_SURPRISED,
    /*  8 */ PLAYER_FACE_HURT,
    /*  9 */ PLAYER_FACE_GASP,
    /* 10 */ PLAYER_FACE_LOOK_RIGHT,
    /* 11 */ PLAYER_FACE_LOOK_LEFT_2,
    /* 12 */ PLAYER_FACE_EYES_CLOSED_MOUTH_OPEN,
    /* 13 */ PLAYER_FACE_OPENING,
    /* 14 */ PLAYER_FACE_EYES_AND_MOUTH_OPEN,
    /* 15 */ PLAYER_FACE_SMILE,
    /* 16 */ PLAYER_FACE_MAX
} PlayerFace;

typedef enum PlayerLimb {
    /* 0x00 */ PLAYER_LIMB_NONE,
    /* 0x01 */ PLAYER_LIMB_ROOT,
    /* 0x02 */ PLAYER_LIMB_WAIST,
    /* 0x03 */ PLAYER_LIMB_LOWER_ROOT,
    /* 0x04 */ PLAYER_LIMB_RIGHT_THIGH,
    /* 0x05 */ PLAYER_LIMB_RIGHT_SHIN,
    /* 0x06 */ PLAYER_LIMB_RIGHT_FOOT,
    /* 0x07 */ PLAYER_LIMB_LEFT_THIGH,
    /* 0x08 */ PLAYER_LIMB_LEFT_SHIN,
    /* 0x09 */ PLAYER_LIMB_LEFT_FOOT,
    /* 0x0A */ PLAYER_LIMB_UPPER_ROOT,
    /* 0x0B */ PLAYER_LIMB_HEAD,
    /* 0x0C */ PLAYER_LIMB_HAT,
    /* 0x0D */ PLAYER_LIMB_COLLAR,
    /* 0x0E */ PLAYER_LIMB_LEFT_SHOULDER,
    /* 0x0F */ PLAYER_LIMB_LEFT_FOREARM,
    /* 0x10 */ PLAYER_LIMB_LEFT_HAND,
    /* 0x11 */ PLAYER_LIMB_RIGHT_SHOULDER,
    /* 0x12 */ PLAYER_LIMB_RIGHT_FOREARM,
    /* 0x13 */ PLAYER_LIMB_RIGHT_HAND,
    /* 0x14 */ PLAYER_LIMB_SHEATH,
    /* 0x15 */ PLAYER_LIMB_TORSO,
    /* 0x16 */ PLAYER_LIMB_MAX
} PlayerLimb;

typedef enum PlayerBodyPart {
    /* 0x00 */ PLAYER_BODYPART_WAIST,          // PLAYER_LIMB_WAIST
    /* 0x01 */ PLAYER_BODYPART_RIGHT_THIGH,    // PLAYER_LIMB_RIGHT_THIGH
    /* 0x02 */ PLAYER_BODYPART_RIGHT_SHIN,     // PLAYER_LIMB_RIGHT_SHIN
    /* 0x03 */ PLAYER_BODYPART_RIGHT_FOOT,     // PLAYER_LIMB_RIGHT_FOOT
    /* 0x04 */ PLAYER_BODYPART_LEFT_THIGH,     // PLAYER_LIMB_LEFT_THIGH
    /* 0x05 */ PLAYER_BODYPART_LEFT_SHIN,      // PLAYER_LIMB_LEFT_SHIN
    /* 0x06 */ PLAYER_BODYPART_LEFT_FOOT,      // PLAYER_LIMB_LEFT_FOOT
    /* 0x07 */ PLAYER_BODYPART_HEAD,           // PLAYER_LIMB_HEAD
    /* 0x08 */ PLAYER_BODYPART_HAT,            // PLAYER_LIMB_HAT
    /* 0x09 */ PLAYER_BODYPART_COLLAR,         // PLAYER_LIMB_COLLAR
    /* 0x0A */ PLAYER_BODYPART_LEFT_SHOULDER,  // PLAYER_LIMB_LEFT_SHOULDER
    /* 0x0B */ PLAYER_BODYPART_LEFT_FOREARM,   // PLAYER_LIMB_LEFT_FOREARM
    /* 0x0C */ PLAYER_BODYPART_LEFT_HAND,      // PLAYER_LIMB_LEFT_HAND
    /* 0x0D */ PLAYER_BODYPART_RIGHT_SHOULDER, // PLAYER_LIMB_R_SHOULDER
    /* 0x0E */ PLAYER_BODYPART_RIGHT_FOREARM,  // PLAYER_LIMB_R_FOREARM
    /* 0x0F */ PLAYER_BODYPART_RIGHT_HAND,     // PLAYER_LIMB_R_HAND
    /* 0x10 */ PLAYER_BODYPART_SHEATH,         // PLAYER_LIMB_SHEATH
    /* 0x11 */ PLAYER_BODYPART_TORSO,          // PLAYER_LIMB_TORSO
    /* 0x12 */ PLAYER_BODYPART_MAX
} PlayerBodyPart;

typedef struct PlayerAnimationFrame {
    /* 0x000 */ Vec3s frameTable[PLAYER_LIMB_MAX];
    /* 0x108 */ s16 appearanceInfo; // bitpack containing the face and hands info
} PlayerAnimationFrame; // size = 0x10A

#define PLAYER_LIMB_BUF_SIZE (ALIGN16(sizeof(PlayerAnimationFrame)) + 0xF)

#define GET_APPEARANCE_FROM_JOINT_TABLE(jointTable)           (((PlayerAnimationFrame *)(jointTable))->appearanceInfo)
#define GET_EYE_INDEX_FROM_JOINT_TABLE(jointTable)         ((GET_APPEARANCE_FROM_JOINT_TABLE(jointTable) & 0xF) - 1)
#define GET_MOUTH_INDEX_FROM_JOINT_TABLE(jointTable)       (((GET_APPEARANCE_FROM_JOINT_TABLE(jointTable) >> 4) & 0xF) - 1)

// Note the returned value from this macro needs to be shifted
#define GET_LEFT_HAND_INDEX_FROM_JOINT_TABLE(jointTable)   (GET_APPEARANCE_FROM_JOINT_TABLE(jointTable) & 0xF000)
#define GET_RIGHT_HAND_INDEX_FROM_JOINT_TABLE(jointTable)  (GET_APPEARANCE_FROM_JOINT_TABLE(jointTable) & 0x0F00)

typedef enum PlayerLedgeClimbType {
    /* 0 */ PLAYER_LEDGE_CLIMB_NONE,
    /* 1 */ PLAYER_LEDGE_CLIMB_1,
    /* 2 */ PLAYER_LEDGE_CLIMB_2,
    /* 3 */ PLAYER_LEDGE_CLIMB_3,
    /* 4 */ PLAYER_LEDGE_CLIMB_4
} PlayerLedgeClimbType;

#define LEDGE_DIST_MAX 399.96002f

typedef enum PlayerStickDirection {
    /* -1 */ PLAYER_STICK_DIR_NONE = -1,
    /*  0 */ PLAYER_STICK_DIR_FORWARD,
    /*  1 */ PLAYER_STICK_DIR_LEFT,
    /*  2 */ PLAYER_STICK_DIR_BACKWARD,
    /*  3 */ PLAYER_STICK_DIR_RIGHT
} PlayerStickDirection;

// TODO: less dumb name
#define SFX_VOICE_BANK_SIZE 0x20

typedef struct PlayerAgeProperties {
    /* 0x00 */ f32 ceilingCheckHeight;
    /* 0x04 */ f32 shadowScale;
    /* 0x08 */ f32 unk_08;
    /* 0x0C */ f32 unk_0C;
    /* 0x10 */ f32 unk_10;
    /* 0x14 */ f32 unk_14; // compared to yDistToLedge
    /* 0x18 */ f32 unk_18; // compared to yDistToLedge
    /* 0x1C */ f32 unk_1C; // compared to yDistToLedge
    /* 0x20 */ f32 unk_20; // unused?
    /* 0x24 */ f32 unk_24; // water stuff // depthInWater
    /* 0x28 */ f32 unk_28; // water stuff // depthInWater
    /* 0x2C */ f32 unk_2C; // water stuff // depthInWater
    /* 0x30 */ f32 unk_30; // water stuff // depthInWater
    /* 0x34 */ f32 unk_34; // height?
    /* 0x38 */ f32 wallCheckRadius;
    /* 0x3C */ f32 unk_3C;
    /* 0x40 */ f32 unk_40;
    /* 0x44 */ Vec3s unk_44;
    /* 0x4A */ Vec3s unk_4A[4];
    /* 0x62 */ Vec3s unk_62[4];
    /* 0x7A */ Vec3s unk_7A[4];
    /* 0x92 */ u16 voiceSfxIdOffset;
    /* 0x94 */ u16 surfaceSfxIdOffset;
    /* 0x98 */ f32 unk_98;
    /* 0x9C */ f32 unk_9C;
    /* 0xA0 */ PlayerAnimationHeader* openChestAnim;
    /* 0xA4 */ PlayerAnimationHeader* timeTravelStartAnim; // OoT leftovers to interact with the Master Sword
    /* 0xA8 */ PlayerAnimationHeader* timeTravelEndAnim; // OoT leftovers to interact with the Master Sword
    /* 0xAC */ PlayerAnimationHeader* unk_AC;
    /* 0xB0 */ PlayerAnimationHeader* unk_B0;
    /* 0xB4 */ PlayerAnimationHeader* unk_B4[4];
    /* 0xC4 */ PlayerAnimationHeader* unk_C4[2];
    /* 0xCC */ PlayerAnimationHeader* unk_CC[2];
    /* 0xD4 */ PlayerAnimationHeader* unk_D4[2];
} PlayerAgeProperties; // size = 0xDC

typedef struct {
    /* 0x00 */ s32 active;
    /* 0x04 */ Vec3f tip;
    /* 0x10 */ Vec3f base;
} WeaponInfo; // size = 0x1C


typedef struct {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ u8 alpha;
    /* 0x04 */ MtxF mf;
} struct_80122D44_arg1_unk_04; // size = 0x44

typedef struct {
    /* 0x00 */ u8 unk_00;
    /* 0x01 */ s8 unk_01;
    /* 0x02 */ char unk_02[2]; // probably alignment padding
    /* 0x04 */ struct_80122D44_arg1_unk_04 unk_04[4];
} struct_80122D44_arg1; // size >= 0x114

typedef struct PlayerOverrideInputEntry {
    /* 0x0 */ s8 numPoints;
    /* 0x1 */ s8 curPoint;
    /* 0x4 */ Vec3s* targetPosList;
} PlayerOverrideInputEntry; // size = 0x8

typedef enum PlayerCsAction {
    /*   -1 */ PLAYER_CSACTION_NEG1 = -1, // Specific to Kafei, any negative number works
    /* 0x00 */ PLAYER_CSACTION_NONE,
    /* 0x01 */ PLAYER_CSACTION_1,
    /* 0x02 */ PLAYER_CSACTION_2,
    /* 0x03 */ PLAYER_CSACTION_3,
    /* 0x04 */ PLAYER_CSACTION_4,
    /* 0x05 */ PLAYER_CSACTION_5,
    /* 0x06 */ PLAYER_CSACTION_END,
    /* 0x07 */ PLAYER_CSACTION_WAIT,
    /* 0x08 */ PLAYER_CSACTION_8,
    /* 0x09 */ PLAYER_CSACTION_9,
    /* 0x0A */ PLAYER_CSACTION_10,
    /* 0x0B */ PLAYER_CSACTION_11,
    /* 0x0C */ PLAYER_CSACTION_12,
    /* 0x0D */ PLAYER_CSACTION_13,
    /* 0x0E */ PLAYER_CSACTION_14,
    /* 0x0F */ PLAYER_CSACTION_15,
    /* 0x10 */ PLAYER_CSACTION_16,
    /* 0x11 */ PLAYER_CSACTION_17,
    /* 0x12 */ PLAYER_CSACTION_18, // Strangled by Wallmaster
    /* 0x13 */ PLAYER_CSACTION_19,
    /* 0x14 */ PLAYER_CSACTION_20,
    /* 0x15 */ PLAYER_CSACTION_21,
    /* 0x16 */ PLAYER_CSACTION_22,
    /* 0x17 */ PLAYER_CSACTION_23,
    /* 0x18 */ PLAYER_CSACTION_24,
    /* 0x19 */ PLAYER_CSACTION_25,
    /* 0x1A */ PLAYER_CSACTION_26, // Halt!
    /* 0x1B */ PLAYER_CSACTION_27,
    /* 0x1C */ PLAYER_CSACTION_28,
    /* 0x1D */ PLAYER_CSACTION_29,
    /* 0x1E */ PLAYER_CSACTION_30,
    /* 0x1F */ PLAYER_CSACTION_31,
    /* 0x20 */ PLAYER_CSACTION_32,
    /* 0x21 */ PLAYER_CSACTION_33,
    /* 0x22 */ PLAYER_CSACTION_34,
    /* 0x23 */ PLAYER_CSACTION_35,
    /* 0x24 */ PLAYER_CSACTION_36,
    /* 0x25 */ PLAYER_CSACTION_37,
    /* 0x26 */ PLAYER_CSACTION_38,
    /* 0x27 */ PLAYER_CSACTION_39,
    /* 0x28 */ PLAYER_CSACTION_40,
    /* 0x29 */ PLAYER_CSACTION_41,
    /* 0x2A */ PLAYER_CSACTION_42,
    /* 0x2B */ PLAYER_CSACTION_43,
    /* 0x2C */ PLAYER_CSACTION_44,
    /* 0x2D */ PLAYER_CSACTION_45,
    /* 0x2E */ PLAYER_CSACTION_46,
    /* 0x2F */ PLAYER_CSACTION_47,
    /* 0x30 */ PLAYER_CSACTION_48,
    /* 0x31 */ PLAYER_CSACTION_49,
    /* 0x32 */ PLAYER_CSACTION_50,
    /* 0x33 */ PLAYER_CSACTION_51,
    /* 0x34 */ PLAYER_CSACTION_52,
    /* 0x35 */ PLAYER_CSACTION_53,
    /* 0x36 */ PLAYER_CSACTION_54,
    /* 0x37 */ PLAYER_CSACTION_55,
    /* 0x38 */ PLAYER_CSACTION_56,
    /* 0x39 */ PLAYER_CSACTION_57,
    /* 0x3A */ PLAYER_CSACTION_58,
    /* 0x3B */ PLAYER_CSACTION_59,
    /* 0x3C */ PLAYER_CSACTION_60,
    /* 0x3D */ PLAYER_CSACTION_61,
    /* 0x3E */ PLAYER_CSACTION_62,
    /* 0x3F */ PLAYER_CSACTION_63,
    /* 0x40 */ PLAYER_CSACTION_64,
    /* 0x41 */ PLAYER_CSACTION_65,
    /* 0x42 */ PLAYER_CSACTION_66, // Look side-to-side with chin down
    /* 0x43 */ PLAYER_CSACTION_67,
    /* 0x44 */ PLAYER_CSACTION_68,
    /* 0x45 */ PLAYER_CSACTION_69,
    /* 0x46 */ PLAYER_CSACTION_70,
    /* 0x47 */ PLAYER_CSACTION_71,
    /* 0x48 */ PLAYER_CSACTION_72,
    /* 0x49 */ PLAYER_CSACTION_73,
    /* 0x4A */ PLAYER_CSACTION_74, // Give a big nod of approval
    /* 0x4B */ PLAYER_CSACTION_75,
    /* 0x4C */ PLAYER_CSACTION_76,
    /* 0x4D */ PLAYER_CSACTION_77,
    /* 0x4E */ PLAYER_CSACTION_78,
    /* 0x4F */ PLAYER_CSACTION_79,
    /* 0x50 */ PLAYER_CSACTION_80,
    /* 0x51 */ PLAYER_CSACTION_81, // Look side-to-side with chin up
    /* 0x52 */ PLAYER_CSACTION_82, // Close eyes and sway body in circles
    /* 0x53 */ PLAYER_CSACTION_83,
    /* 0x54 */ PLAYER_CSACTION_84, // Sucked by the moon
    /* 0x55 */ PLAYER_CSACTION_85,
    /* 0x56 */ PLAYER_CSACTION_86,
    /* 0x57 */ PLAYER_CSACTION_87,
    /* 0x58 */ PLAYER_CSACTION_88,
    /* 0x59 */ PLAYER_CSACTION_89,
    /* 0x5A */ PLAYER_CSACTION_90,
    /* 0x5B */ PLAYER_CSACTION_91,
    /* 0x5C */ PLAYER_CSACTION_92,
    /* 0x5D */ PLAYER_CSACTION_93,
    /* 0x5E */ PLAYER_CSACTION_94,
    /* 0x5F */ PLAYER_CSACTION_95,
    /* 0x60 */ PLAYER_CSACTION_96,
    /* 0x61 */ PLAYER_CSACTION_97,
    /* 0x62 */ PLAYER_CSACTION_98,
    /* 0x63 */ PLAYER_CSACTION_99,
    /* 0x64 */ PLAYER_CSACTION_100,
    /* 0x65 */ PLAYER_CSACTION_101,
    /* 0x66 */ PLAYER_CSACTION_102,
    /* 0x67 */ PLAYER_CSACTION_103,
    /* 0x68 */ PLAYER_CSACTION_104,
    /* 0x69 */ PLAYER_CSACTION_105,
    /* 0x6A */ PLAYER_CSACTION_106,
    /* 0x6B */ PLAYER_CSACTION_107,
    /* 0x6C */ PLAYER_CSACTION_108,
    /* 0x6D */ PLAYER_CSACTION_109,
    /* 0x6E */ PLAYER_CSACTION_110,
    /* 0x6F */ PLAYER_CSACTION_111,
    /* 0x70 */ PLAYER_CSACTION_112,
    /* 0x71 */ PLAYER_CSACTION_113,
    /* 0x72 */ PLAYER_CSACTION_114,
    /* 0x73 */ PLAYER_CSACTION_115,
    /* 0x74 */ PLAYER_CSACTION_116,
    /* 0x75 */ PLAYER_CSACTION_117,
    /* 0x76 */ PLAYER_CSACTION_118,
    /* 0x77 */ PLAYER_CSACTION_119,
    /* 0x78 */ PLAYER_CSACTION_120,
    /* 0x79 */ PLAYER_CSACTION_121,
    /* 0x7A */ PLAYER_CSACTION_122,
    /* 0x7B */ PLAYER_CSACTION_123,
    /* 0x7C */ PLAYER_CSACTION_124,
    /* 0x7D */ PLAYER_CSACTION_125,
    /* 0x7E */ PLAYER_CSACTION_126,
    /* 0x7F */ PLAYER_CSACTION_127,
    /* 0x80 */ PLAYER_CSACTION_128,
    /* 0x81 */ PLAYER_CSACTION_129,
    /* 0x82 */ PLAYER_CSACTION_130,
    /* 0x83 */ PLAYER_CSACTION_131,
    /* 0x84 */ PLAYER_CSACTION_132,
    /* 0x85 */ PLAYER_CSACTION_133,
    /* 0x86 */ PLAYER_CSACTION_134,
    /* 0x87 */ PLAYER_CSACTION_135,
    /* 0x88 */ PLAYER_CSACTION_136,
    /* 0x89 */ PLAYER_CSACTION_137,
    /* 0x8A */ PLAYER_CSACTION_138,
    /* 0x8B */ PLAYER_CSACTION_139,
    /* 0x8C */ PLAYER_CSACTION_MAX
} PlayerCsAction;

typedef enum PlayerCueId {
    /* 0x00 */ PLAYER_CUEID_NONE,
    /* 0x01 */ PLAYER_CUEID_1,
    /* 0x02 */ PLAYER_CUEID_2,
    /* 0x03 */ PLAYER_CUEID_3,
    /* 0x04 */ PLAYER_CUEID_4,
    /* 0x05 */ PLAYER_CUEID_5,
    /* 0x06 */ PLAYER_CUEID_6,
    /* 0x07 */ PLAYER_CUEID_7,
    /* 0x08 */ PLAYER_CUEID_8,
    /* 0x09 */ PLAYER_CUEID_9,
    /* 0x0A */ PLAYER_CUEID_10,
    /* 0x0B */ PLAYER_CUEID_11,
    /* 0x0C */ PLAYER_CUEID_12,
    /* 0x0D */ PLAYER_CUEID_13,
    /* 0x0E */ PLAYER_CUEID_14,
    /* 0x0F */ PLAYER_CUEID_15,
    /* 0x10 */ PLAYER_CUEID_16,
    /* 0x11 */ PLAYER_CUEID_17,
    /* 0x12 */ PLAYER_CUEID_18,
    /* 0x13 */ PLAYER_CUEID_19,
    /* 0x14 */ PLAYER_CUEID_20,
    /* 0x15 */ PLAYER_CUEID_21,
    /* 0x16 */ PLAYER_CUEID_22,
    /* 0x17 */ PLAYER_CUEID_23,
    /* 0x18 */ PLAYER_CUEID_24,
    /* 0x19 */ PLAYER_CUEID_25,
    /* 0x1A */ PLAYER_CUEID_26,
    /* 0x1B */ PLAYER_CUEID_27,
    /* 0x1C */ PLAYER_CUEID_28,
    /* 0x1D */ PLAYER_CUEID_29,
    /* 0x1E */ PLAYER_CUEID_30,
    /* 0x1F */ PLAYER_CUEID_31,
    /* 0x20 */ PLAYER_CUEID_32,
    /* 0x21 */ PLAYER_CUEID_33,
    /* 0x22 */ PLAYER_CUEID_34,
    /* 0x23 */ PLAYER_CUEID_35,
    /* 0x24 */ PLAYER_CUEID_36,
    /* 0x25 */ PLAYER_CUEID_37,
    /* 0x26 */ PLAYER_CUEID_38,
    /* 0x27 */ PLAYER_CUEID_39,
    /* 0x28 */ PLAYER_CUEID_40,
    /* 0x29 */ PLAYER_CUEID_41,
    /* 0x2A */ PLAYER_CUEID_42,
    /* 0x2B */ PLAYER_CUEID_43,
    /* 0x2C */ PLAYER_CUEID_44,
    /* 0x2D */ PLAYER_CUEID_45,
    /* 0x2E */ PLAYER_CUEID_46,
    /* 0x2F */ PLAYER_CUEID_47,
    /* 0x30 */ PLAYER_CUEID_48,
    /* 0x31 */ PLAYER_CUEID_49,
    /* 0x32 */ PLAYER_CUEID_50,
    /* 0x33 */ PLAYER_CUEID_51,
    /* 0x34 */ PLAYER_CUEID_52,
    /* 0x35 */ PLAYER_CUEID_53,
    /* 0x36 */ PLAYER_CUEID_54,
    /* 0x37 */ PLAYER_CUEID_55,
    /* 0x38 */ PLAYER_CUEID_56,
    /* 0x39 */ PLAYER_CUEID_57,
    /* 0x3A */ PLAYER_CUEID_58,
    /* 0x3B */ PLAYER_CUEID_59,
    /* 0x3C */ PLAYER_CUEID_60,
    /* 0x3D */ PLAYER_CUEID_61,
    /* 0x3E */ PLAYER_CUEID_62,
    /* 0x3F */ PLAYER_CUEID_63,
    /* 0x40 */ PLAYER_CUEID_64,
    /* 0x41 */ PLAYER_CUEID_65,
    /* 0x42 */ PLAYER_CUEID_66,
    /* 0x43 */ PLAYER_CUEID_67,
    /* 0x44 */ PLAYER_CUEID_68,
    /* 0x45 */ PLAYER_CUEID_69,
    /* 0x46 */ PLAYER_CUEID_70,
    /* 0x47 */ PLAYER_CUEID_71,
    /* 0x48 */ PLAYER_CUEID_72,
    /* 0x49 */ PLAYER_CUEID_73,
    /* 0x4A */ PLAYER_CUEID_74,
    /* 0x4B */ PLAYER_CUEID_75,
    /* 0x4C */ PLAYER_CUEID_76,
    /* 0x4D */ PLAYER_CUEID_77,
    /* 0x4E */ PLAYER_CUEID_78,
    /* 0x4F */ PLAYER_CUEID_79,
    /* 0x50 */ PLAYER_CUEID_80,
    /* 0x51 */ PLAYER_CUEID_81,
    /* 0x52 */ PLAYER_CUEID_82,
    /* 0x53 */ PLAYER_CUEID_83,
    /* 0x54 */ PLAYER_CUEID_84,
    /* 0x55 */ PLAYER_CUEID_85,
    /* 0x56 */ PLAYER_CUEID_86,
    /* 0x57 */ PLAYER_CUEID_87,
    /* 0x58 */ PLAYER_CUEID_88,
    /* 0x59 */ PLAYER_CUEID_89,
    /* 0x5A */ PLAYER_CUEID_90,
    /* 0x5B */ PLAYER_CUEID_91,
    /* 0x5C */ PLAYER_CUEID_MAX
} PlayerCueId;


// 
#define PLAYER_STATE1_1          (1 << 0)
// 
#define PLAYER_STATE1_2          (1 << 1)
// Climbing ledge
#define PLAYER_STATE1_4          (1 << 2)
// 
#define PLAYER_STATE1_8          (1 << 3)
// Zora electric shield
#define PLAYER_STATE1_10         (1 << 4)
// 
#define PLAYER_STATE1_20         (1 << 5)
// Currently talking to an actor. This includes item exchanges.
#define PLAYER_STATE1_TALKING (1 << 6)
// Player has died. Note that this gets set when the death cutscene has started, after landing from the air.
// This also gets set when either deku/zora forms touches lava floor, or goron form enters water and the scene resets.
#define PLAYER_STATE1_DEAD         (1 << 7)
// 
#define PLAYER_STATE1_100        (1 << 8)
// 
#define PLAYER_STATE1_200        (1 << 9)
// 
#define PLAYER_STATE1_400        (1 << 10)
// Currently carrying an actor
#define PLAYER_STATE1_CARRYING_ACTOR (1 << 11)
// Currently charging a spin attack (by holding down the B button)
#define PLAYER_STATE1_CHARGING_SPIN_ATTACK (1 << 12)
// 
#define PLAYER_STATE1_2000       (1 << 13)
// 
#define PLAYER_STATE1_4000       (1 << 14)
// Either lock-on or parallel is active. This flag is never checked for and is practically unused.
#define PLAYER_STATE1_Z_TARGETING       (1 << 15)
// Currently focusing on a friendly actor. Includes friendly lock-on, talking, and more. Usually does not include hostile actor lock-on, see `PLAYER_STATE3_HOSTILE_LOCK_ON`.
#define PLAYER_STATE1_FRIENDLY_ACTOR_FOCUS      (1 << 16)
// "Parallel" mode, Z-Target without an actor lock-on
#define PLAYER_STATE1_PARALLEL   (1 << 17)
// 
#define PLAYER_STATE1_40000      (1 << 18)
// 
#define PLAYER_STATE1_80000      (1 << 19)
// 
#define PLAYER_STATE1_100000     (1 << 20)
// 
#define PLAYER_STATE1_200000     (1 << 21)
// 
#define PLAYER_STATE1_400000     (1 << 22)
// 
#define PLAYER_STATE1_800000     (1 << 23)
// Currently using the zora boomerang. This includes all phases (aiming, throwing, and catching).
#define PLAYER_STATE1_USING_ZORA_BOOMERANG (1 << 24)
// Zora boomerang has been thrown and is flying in the air
#define PLAYER_STATE1_ZORA_BOOMERANG_THROWN (1 << 25)
// 
#define PLAYER_STATE1_4000000    (1 << 26)
// Swimming?
#define PLAYER_STATE1_8000000    (1 << 27)
// 
#define PLAYER_STATE1_10000000   (1 << 28)
// Time is stopped but Link & NPC animations continue
#define PLAYER_STATE1_20000000   (1 << 29)
// Lock-on was released automatically, for example by leaving the lock-on leash range
#define PLAYER_STATE1_LOCK_ON_FORCED_TO_RELEASE   (1 << 30)
// Related to exit a grotto
#define PLAYER_STATE1_80000000   (1 << 31)


// 
#define PLAYER_STATE2_1          (1 << 0)
// Can accept a talk offer. "Speak" or "Check" is shown on the A button.
#define PLAYER_STATE2_CAN_ACCEPT_TALK_OFFER          (1 << 1)
// 
#define PLAYER_STATE2_4          (1 << 2)
// 
#define PLAYER_STATE2_8          (1 << 3)
// 
#define PLAYER_STATE2_10         (1 << 4)
// 
#define PLAYER_STATE2_20         (1 << 5)
// 
#define PLAYER_STATE2_40         (1 << 6)
// 
#define PLAYER_STATE2_80         (1 << 7)
// 
#define PLAYER_STATE2_100        (1 << 8)
// 
#define PLAYER_STATE2_FORCE_SAND_FLOOR_SOUND (1 << 9)
// 
#define PLAYER_STATE2_400        (1 << 10)
// Diving
#define PLAYER_STATE2_800        (1 << 11)
// 
#define PLAYER_STATE2_1000       (1 << 12)
// Actor lock-on is active, specifically with Switch Targeting. Hold Targeting checks the state of the Z button instead of this flag.
#define PLAYER_STATE2_LOCK_ON_WITH_SWITCH       (1 << 13)
// 
#define PLAYER_STATE2_4000       (1 << 14)
// 
#define PLAYER_STATE2_8000       (1 << 15)
// 
#define PLAYER_STATE2_10000      (1 << 16)
// A non-magic spin attack
#define PLAYER_STATE2_20000      (1 << 17)
// 
#define PLAYER_STATE2_40000      (1 << 18)
// 
#define PLAYER_STATE2_80000      (1 << 19)
// 
#define PLAYER_STATE2_100000     (1 << 20)
// 
#define PLAYER_STATE2_200000     (1 << 21)
// 
#define PLAYER_STATE2_400000     (1 << 22)
// 
#define PLAYER_STATE2_800000     (1 << 23)
// 
#define PLAYER_STATE2_1000000    (1 << 24)
// 
#define PLAYER_STATE2_2000000    (1 << 25)
// 
#define PLAYER_STATE2_4000000    (1 << 26)
// Playing the ocarina
#define PLAYER_STATE2_USING_OCARINA  (1 << 27)
// Playing a fidget idle animation (under typical circumstances, see `Player_ChooseNextIdleAnim` for more info)
#define PLAYER_STATE2_IDLE_FIDGET   (1 << 28)
// Disable drawing player
#define PLAYER_STATE2_20000000   (1 << 29)
// Lunge: small forward boost at the end of certain attack animations
#define PLAYER_STATE2_40000000   (1 << 30)
// Void-out
#define PLAYER_STATE2_80000000   (1 << 31)


// Ignores collision with floor?
#define PLAYER_STATE3_1          (1 << 0)
// 
#define PLAYER_STATE3_2          (1 << 1)
// 
#define PLAYER_STATE3_4          (1 << 2)
// 
#define PLAYER_STATE3_8          (1 << 3)
// 
#define PLAYER_STATE3_10         (1 << 4)
// 
#define PLAYER_STATE3_20         (1 << 5)
// 
#define PLAYER_STATE3_40         (1 << 6)
// Flying in the air with the hookshot as it pulls Player toward its destination
#define PLAYER_STATE3_FLYING_WITH_HOOKSHOT (1 << 7)
// Deku flower dive
#define PLAYER_STATE3_100        (1 << 8)
// 
#define PLAYER_STATE3_200        (1 << 9)
// 
#define PLAYER_STATE3_400        (1 << 10)
// 
#define PLAYER_STATE3_800        (1 << 11)
// goron curled
#define PLAYER_STATE3_1000       (1 << 12)
// 
#define PLAYER_STATE3_2000       (1 << 13)
// 
#define PLAYER_STATE3_4000       (1 << 14)
// Swimming Zora
#define PLAYER_STATE3_8000       (1 << 15)
// 
#define PLAYER_STATE3_10000      (1 << 16)
// 
#define PLAYER_STATE3_20000      (1 << 17)
// Related to form Deku
#define PLAYER_STATE3_40000      (1 << 18)
// 
#define PLAYER_STATE3_80000      (1 << 19)
// 
#define PLAYER_STATE3_100000     (1 << 20)
// Deku hopping?
#define PLAYER_STATE3_200000     (1 << 21)
// 
#define PLAYER_STATE3_400000     (1 << 22)
// A Zora boomerang has been caught this frame
#define PLAYER_STATE3_ZORA_BOOMERANG_CAUGHT (1 << 23)
// 
#define PLAYER_STATE3_1000000    (1 << 24)
// 
#define PLAYER_STATE3_2000000    (1 << 25)
// 
#define PLAYER_STATE3_4000000    (1 << 26)
// 
#define PLAYER_STATE3_8000000    (1 << 27)
// 
#define PLAYER_STATE3_10000000   (1 << 28)
// breman mask march?
#define PLAYER_STATE3_20000000   (1 << 29)
// Item change process has begun
#define PLAYER_STATE3_START_CHANGING_HELD_ITEM   (1 << 30)
// Currently locked onto a hostile actor. Triggers a "battle" variant of many actions.
#define PLAYER_STATE3_HOSTILE_LOCK_ON   (1 << 31)

typedef enum PlayerUnkAA5 {
    /* 0 */ PLAYER_UNKAA5_0,
    /* 1 */ PLAYER_UNKAA5_1,
    /* 2 */ PLAYER_UNKAA5_2,
    /* 3 */ PLAYER_UNKAA5_3,
    /* 4 */ PLAYER_UNKAA5_4,
    /* 5 */ PLAYER_UNKAA5_5
} PlayerUnkAA5;

typedef void (*PlayerActionFunc)(struct Player* this, struct PlayState* play);
typedef s32 (*PlayerUpperActionFunc)(struct Player* this, struct PlayState* play);
typedef void (*AfterPutAwayFunc)(struct PlayState* play, struct Player* this);

#define UNKAA6_ROT_FOCUS_X (1 << 0)
#define UNKAA6_ROT_FOCUS_Y (1 << 1)
#define UNKAA6_ROT_FOCUS_Z (1 << 2)
#define UNKAA6_ROT_HEAD_X (1 << 3)
#define UNKAA6_ROT_HEAD_Y (1 << 4)
#define UNKAA6_ROT_HEAD_Z (1 << 5)
#define UNKAA6_ROT_UPPER_X (1 << 6)
#define UNKAA6_ROT_UPPER_Y (1 << 7)
#define UNKAA6_ROT_UPPER_Z (1 << 8)

typedef struct Player {
    /* 0x000 */ Actor actor;
    /* 0x144 */ s8 currentShield;
    /* 0x145 */ s8 currentBoots;
    /* 0x146 */ s8 heldItemButton;
    /* 0x147 */ s8 heldItemAction; // PlayerItemAction enum
    /* 0x148 */ u8 heldItemId; // ItemId enum
    /* 0x149 */ s8 prevBoots;
    /* 0x14A */ s8 itemAction; // PlayerItemAction enum
    /* 0x14B */ u8 transformation; // PlayerTransformation enum
    /* 0x14C */ u8 modelGroup; // PlayerModelGroup enum
    /* 0x14D */ u8 nextModelGroup;
    /* 0x14E */ s8 itemChangeType; // ItemChangeType enum
    /* 0x14F */ u8 modelAnimType; // PlayerAnimType enum
    /* 0x150 */ u8 leftHandType;
    /* 0x151 */ u8 rightHandType;
    /* 0x152 */ u8 sheathType;
    /* 0x153 */ u8 currentMask; // PlayerMask enum
    /* 0x154 */ s8 unk_154; // EquipSlot enum // Last pressed equip slot?
    /* 0x155 */ u8 prevMask;
    /* 0x158 */ Gfx** rightHandDLists;
    /* 0x15C */ Gfx** leftHandDLists;
    /* 0x160 */ Gfx** sheathDLists;
    /* 0x164 */ Gfx** waistDLists;
    /* 0x168 */ UNK_TYPE1 unk_168[0x4C];
    /* 0x1B4 */ s16 unk_1B4;
    /* 0x1B6 */ UNK_TYPE1 unk_1B6[0x2];
    /* 0x1B8 */ u8 giObjectLoading;
    /* 0x1BC */ DmaRequest giObjectDmaRequest;
    /* 0x1DC */ OSMesgQueue giObjectLoadQueue;
    /* 0x1F4 */ OSMesg giObjectLoadMsg;
    /* 0x1F8 */ void* giObjectSegment;
    /* 0x1FC */ u8 maskObjectLoadState;
    /* 0x1FD */ s8 maskId;
    /* 0x200 */ DmaRequest maskDmaRequest;
    /* 0x220 */ OSMesgQueue maskObjectLoadQueue;
    /* 0x238 */ OSMesg maskObjectLoadMsg;
    /* 0x23C */ void* maskObjectSegment;
    /* 0x240 */ SkelAnime skelAnime;
    /* 0x284 */ SkelAnime skelAnimeUpper;
    /* 0x2C8 */ SkelAnime unk_2C8;
    /* 0x30C */ Vec3s jointTable[5];
    /* 0x32A */ Vec3s morphTable[5];
    /* 0x348 */ FaceChange faceChange;
    /* 0x34C */ Actor* heldActor;
    /* 0x350 */ PosRot leftHandWorld;
    /* 0x364 */ Actor* rightHandActor;
    /* 0x368 */ PosRot rightHandWorld;
    /* 0x37C */ s8 doorType; // PlayerDoorType enum
    /* 0x37D */ s8 doorDirection;
    /* 0x37E */ s8 doorTimer;
    /* 0x37F */ s8 doorNext; // used with spiral staircase
    /* 0x380 */ Actor* doorActor;
    /* 0x384 */ s16 getItemId; // GetItemId enum
    /* 0x386 */ u16 getItemDirection;
    /* 0x388 */ Actor* interactRangeActor;
    /* 0x38C */ s8 mountSide;
    /* 0x390 */ Actor* rideActor;
    /* 0x394 */ u8 csAction; // PlayerCsAction enum
    /* 0x395 */ u8 prevCsAction; // PlayerCsAction enum
    /* 0x396 */ u8 cueId; // PlayerCueId enum
    /* 0x397 */ u8 unk_397; // PlayerDoorType enum
    /* 0x398 */ Actor* csActor; // Actor involved in a `csAction`. Typically the actor that invoked the cutscene.
    /* 0x39C */ UNK_TYPE1 unk_39C[0x4];
    /* 0x3A0 */ Vec3f unk_3A0;
    /* 0x3AC */ Vec3f unk_3AC;
    /* 0x3B8 */ u16 unk_3B8;
    /* 0x3BA */ union {
                    s16 haltActorsDuringCsAction; // If true, halt actors belonging to certain categories during a `csAction`
                    s16 doorBgCamIndex; // `BgCamIndex` used during a sliding door and spiral staircase cutscenes
                } cv; // "Cutscene Variable": context dependent variable that has different meanings depending on what function is called
    /* 0x3BC */ s16 subCamId;
    /* 0x3C0 */ Vec3f unk_3C0;
    /* 0x3CC */ s16 unk_3CC;
    /* 0x3CE */ s8 unk_3CE;
    /* 0x3CF */ u8 unk_3CF;
    /* 0x3D0 */ struct_80122D44_arg1 unk_3D0;
    /* 0x4E4 */ UNK_TYPE1 unk_4E4[0x20];
    /* 0x504 */ LightNode* lightNode;
    /* 0x508 */ LightInfo lightInfo;
    /* 0x518 */ ColliderCylinder cylinder;
    /* 0x564 */ ColliderQuad meleeWeaponQuads[2];
    /* 0x664 */ ColliderQuad shieldQuad;
    /* 0x6E4 */ ColliderCylinder shieldCylinder;
    /* 0x730 */ Actor* focusActor; // Actor that Player and the camera are looking at; Used for lock-on, talking, and more
    /* 0x734 */ UNK_TYPE1 unk_734[0x4];
    /* 0x738 */ s32 zTargetActiveTimer; // Non-zero values indicate Z-Targeting should update; Values under 5 indicate lock-on is releasing
    /* 0x73C */ s32 meleeWeaponEffectIndex[3];
    /* 0x748 */ PlayerActionFunc actionFunc;
    /* 0x74C */ u8 jointTableBuffer[PLAYER_LIMB_BUF_SIZE];
    /* 0x7EB */ u8 morphTableBuffer[PLAYER_LIMB_BUF_SIZE];
    /* 0x88A */ u8 blendTableBuffer[PLAYER_LIMB_BUF_SIZE];
    /* 0x929 */ u8 jointTableUpperBuffer[PLAYER_LIMB_BUF_SIZE];
    /* 0x9C8 */ u8 morphTableUpperBuffer[PLAYER_LIMB_BUF_SIZE];
    /* 0xA68 */ PlayerAgeProperties* ageProperties; // repurposed as "transformation properties"?
    /* 0xA6C */ u32 stateFlags1;
    /* 0xA70 */ u32 stateFlags2;
    /* 0xA74 */ u32 stateFlags3;
    /* 0xA78 */ Actor* autoLockOnActor; // Actor that is locked onto automatically without player input; see `Player_SetAutoLockOnActor`
    /* 0xA7C */ Actor* zoraBoomerangActor; // Defaults to the left zora boomerang, but will switch to right if only the left boomerang is caught.
    /* 0xA80 */ Actor* tatlActor;
    /* 0xA84 */ s16 tatlTextId;
    /* 0xA86 */ s8 csId;
    /* 0xA87 */ s8 exchangeItemAction; // PlayerItemAction enum
    /* 0xA88 */ Actor* talkActor;
    /* 0xA8C */ f32 talkActorDistance;
    /* 0xA90 */ Actor* ocarinaInteractionActor;
    /* 0xA94 */ f32 ocarinaInteractionDistance;
    /* 0xA98 */ UNK_TYPE1 unk_A98[0x4];
    /* 0xA9C */ f32 secretRumbleCharge; // builds per frame until discharges with a rumble request
    /* 0xAA0 */ f32 closestSecretDistSq; // Used to augment `secretRumbleCharge`. Cleared every frame
    /* 0xAA4 */ s8 idleType;
    /* 0xAA5 */ u8 unk_AA5; // PlayerUnkAA5 enum
    /* 0xAA6 */ u16 unk_AA6_rotFlags; // See `UNKAA6_ROT_` macros. If its flag isn't set, a rot steps to 0.
    /* 0xAA8 */ s16 upperLimbYawSecondary;
    /* 0xAAA */ s16 unk_AAA;
    /* 0xAAC */ Vec3s headLimbRot;
    /* 0xAB2 */ Vec3s upperLimbRot;
    /* 0xAB8 */ f32 unk_AB8;
    /* 0xABC */ f32 unk_ABC;
    /* 0xAC0 */ f32 unk_AC0;
    /* 0xAC4 */ PlayerUpperActionFunc upperActionFunc; // Upper body/item action functions
    /* 0xAC8 */ f32 skelAnimeUpperBlendWeight;
    /* 0xACC */ s16 unk_ACC;
    /* 0xACE */ s8 unk_ACE;
    /* 0xACF */ u8 putAwayCooldownTimer; // Frames to wait before showing "Put Away" on A
    /* 0xAD0 */ f32 speedXZ; // Controls horizontal speed, used for `actor.speed`. Current or target value depending on context.
    /* 0xAD4 */ s16 yaw; // General yaw value, used both for world and shape rotation. Current or target value depending on context.
    /* 0xAD6 */ s16 parallelYaw; // yaw in "parallel" mode, Z-Target without an actor lock-on
    /* 0xAD8 */ u16 underwaterTimer;
    /* 0xADA */ s8 meleeWeaponAnimation;
    /* 0xADB */ s8 meleeWeaponState;
    /* 0xADC */ s8 unk_ADC;
    /* 0xADD */ s8 unk_ADD; // Some sort of combo counter
    /* 0xADE */ u8 controlStickDataIndex; // cycles between 0 - 3. Used to index `controlStickSpinAngles` and `controlStickDirections`
    /* 0xADF */ s8 controlStickSpinAngles[4]; // Stores a modified version of the control stick angle for the last 4 frames. Used for checking spins.
    /* 0xAE3 */ s8 controlStickDirections[4]; // Stores the control stick direction (relative to shape yaw) for the last 4 frames. See `PlayerStickDirection`.
    /* 0xAE7 */ union {
        s8 actionVar1;
        s8 startedAnim; // Player_Action_TimeTravelEnd: Started playing the animation that was previously frozen
        s8 facingUpSlope; // Player_Action_SlideOnSlope: Facing uphill when sliding on a slope
    } av1; // "Action Variable 1": context dependent variable that has different meanings depending on what action is currently running
    /* 0xAE8 */ union {
        s16 actionVar2;
        s16 fallDamageStunTimer; // Player_Action_Idle: Prevents any movement and shakes model up and down quickly to indicate fall damage stun
        s16 animDelayTimer; // Player_Action_TimeTravelEnd: Delays playing animation until finished counting down
        s16 csDelayTimer; // Player_Action_WaitForCutscene: Number of frames to wait before responding to a cutscene
        s16 playedLandingSfx; // Player_Action_BlueWarpArrive: Played sfx when landing on the ground
    } av2; // "Action Variable 2": context dependent variable that has different meanings depending on what action is currently running
    /* 0xAEC */ f32 unk_AEC;
    /* 0xAF0 */ union {
                    Vec3f unk_AF0[2];
                    f32 arr_AF0[6];
                };
    /* 0xB08 */ f32 unk_B08;
    /* 0xB0C */ f32 unk_B0C;
    /* 0xB10 */ f32 unk_B10[6];
    /* 0xB28 */ s16 unk_B28; // Burning stick timer?
    /* 0xB2A */ s8 getItemDrawIdPlusOne;
    /* 0xB2B */ s8 unk_B2B;
    /* 0xB2C */ f32 windSpeed;
    /* 0xB30 */ s16 windAngleX;
    /* 0xB32 */ s16 windAngleY;
    /* 0xB34 */ f32 unk_B34;
    /* 0xB38 */ f32 unk_B38;
    /* 0xB3C */ f32 unk_B3C;
    /* 0xB40 */ f32 unk_B40;
    /* 0xB44 */ f32 unk_B44;
    /* 0xB48 */ f32 unk_B48;
    /* 0xB4C */ s16 unk_B4C;
    /* 0xB4E */ s16 turnRate; // Amount angle is changed every frame when turning in place
    /* 0xB50 */ f32 unk_B50;
    /* 0xB54 */ f32 yDistToLedge; // y distance to ground above an interact wall. LEDGE_DIST_MAX if no ground if found
    /* 0xB58 */ f32 distToInteractWall; // xyz distance to the interact wall
    /* 0xB5C */ u8 ledgeClimbType; // see PlayerLedgeClimbType enum
    /* 0xB5D */ u8 ledgeClimbDelayTimer;
    /* 0xB5E */ u8 textboxBtnCooldownTimer; // Prevents usage of A/B/C-up when counting down
    /* 0xB5F */ u8 unk_B5F;
    /* 0xB60 */ u16 blastMaskTimer;
    /* 0xB62 */ s16 unk_B62;
    /* 0xB64 */ u8 unk_B64;
    /* 0xB65 */ u8 bodyShockTimer;
    /* 0xB66 */ u8 unk_B66;
    /* 0xB67 */ u8 remainingHopsCounter; // Deku hopping on water
    /* 0xB68 */ s16 fallStartHeight; // last truncated Y position before falling
    /* 0xB6A */ s16 fallDistance; // truncated Y distance the player has fallen so far (positive is down)
    /* 0xB6C */ s16 floorPitch; // angle of the floor slope in the direction of current world yaw (positive for ascending slope)
    /* 0xB6E */ s16 floorPitchAlt; // the calculation for this value is bugged and doesn't represent anything meaningful
    /* 0xB70 */ s16 unk_B70;
    /* 0xB72 */ u16 floorSfxOffset;
    /* 0xB74 */ u8 unk_B74;
    /* 0xB75 */ u8 unk_B75;
    /* 0xB76 */ s16 unk_B76;
    /* 0xB78 */ f32 unk_B78;
    /* 0xB7C */ f32 unk_B7C;
    /* 0xB80 */ f32 pushedSpeed; // Pushing player, examples include water currents, floor conveyors, climbing sloped surfaces
    /* 0xB84 */ s16 pushedYaw; // Yaw of direction in which player is being pushed
    /* 0xB86 */ s16 unk_B86[2]; // unknown length
    /* 0xB8A */ s16 unk_B8A;
    /* 0xB8C */ s16 unk_B8C;
    /* 0xB8E */ s16 unk_B8E;
    /* 0xB90 */ s16 unk_B90;
    /* 0xB92 */ s16 unk_B92;
    /* 0xB94 */ s16 unk_B94;
    /* 0xB96 */ s16 unk_B96;
    /* 0xB98 */ WeaponInfo meleeWeaponInfo[3];
    /* 0xBEC */ Vec3f bodyPartsPos[PLAYER_BODYPART_MAX];
    /* 0xCC4 */ MtxF leftHandMf;
    /* 0xD04 */ MtxF shieldMf;
    /* 0xD44 */ u8 bodyIsBurning;
    /* 0xD45 */ u8 bodyFlameTimers[PLAYER_BODYPART_MAX]; // one flame per body part
    /* 0xD57 */ u8 unk_D57;
    /* 0xD58 */ AfterPutAwayFunc afterPutAwayFunc; // See `Player_SetupWaitForPutAway` and `Player_Action_WaitForPutAway`
    /* 0xD5C */ s8 invincibilityTimer; // prevents damage when nonzero (positive = visible, counts towards zero each frame)
    /* 0xD5D */ u8 floorTypeTimer; // Unused remnant of OoT
    /* 0xD5E */ u8 floorProperty; // FloorProperty enum
    /* 0xD5F */ u8 prevFloorType; // Unused remnant of OoT
    /* 0xD60 */ f32 prevControlStickMagnitude;
    /* 0xD64 */ s16 prevControlStickAngle;
    /* 0xD66 */ u16 prevFloorSfxOffset;
    /* 0xD68 */ s16 unk_D68;
    /* 0xD6A */ s8 unk_D6A;
    /* 0xD6B */ u8 unk_D6B;
    /* 0xD6C */ Vec3f unk_D6C; // previous body part 0 position
} Player; // size = 0xD78
#ifdef __cplusplus
}
#undef this
#endif

// z_player_call.c functions

void PlayerCall_Init(Actor* thisx, struct PlayState* play);
void PlayerCall_Destroy(Actor* thisx, struct PlayState* play);
void PlayerCall_Update(Actor* thisx, struct PlayState* play);
void PlayerCall_Draw(Actor* thisx, struct PlayState* play);

// z_actor.c functions

f32 Player_GetHeight(Player* player);
f32 Player_GetRunSpeedLimit(Player* player);
bool func_800B7118(Player* player);
bool func_800B7128(Player* player);
bool func_800B715C(struct PlayState* play);
void Player_SetCameraHorseSetting(struct PlayState* play, Player* player);
void Player_MountHorse(struct PlayState* play, Player* player, Actor* horse);
s32 Player_SetCsAction(struct PlayState* play, Actor* csActor, u8 csAction);
s32 Player_SetCsActionWithHaltedActors(struct PlayState* play, Actor* csActor, u8 csAction);

s32 Player_IsFacingActor(Actor* actor, s16 maxAngleDiff, struct PlayState* play);

PlayerItemAction Player_GetExchangeItemAction(struct PlayState* play);

void func_800B8D10(struct PlayState* play, Actor* actor, f32 arg2, s16 arg3, f32 arg4, s32 arg5, u32 arg6);
void func_800B8D50(struct PlayState* play, Actor* actor, f32 arg2, s16 yaw, f32 arg4, u32 arg5);
void func_800B8D98(struct PlayState* play, Actor* actor, f32 arg2, s16 arg3, f32 arg4);
void func_800B8DD4(struct PlayState* play, Actor* actor, f32 arg2, s16 arg3, f32 arg4, u32 arg5);
void func_800B8E1C(struct PlayState* play, Actor* actor, f32 arg2, s16 arg3, f32 arg4);
void Player_PlaySfx(Player* player, u16 sfxId);

// z_player_lib.c functions

s32 func_801226E0(struct PlayState* play, s32 arg1);
s32 Player_InitOverrideInput(struct PlayState* play, PlayerOverrideInputEntry* inputEntry, u32 numPoints, Vec3s* targetPosList);
s32 Player_UpdateOverrideInput(struct PlayState* play, PlayerOverrideInputEntry* inputEntry, f32 distXZRange);
void func_80122868(struct PlayState* play, Player* player);
void func_801229A0(struct PlayState* play, Player* player);
void Player_DoNothing(Actor* thisx, struct PlayState* play);
void func_801229FC(Player* player);
void func_80122BA4(struct PlayState* play, struct_80122D44_arg1* arg1, s32 arg2, s32 alpha);
void func_80122C20(struct PlayState* play, struct_80122D44_arg1* arg1);
void func_80122D44(struct PlayState* play, struct_80122D44_arg1* arg1);
u8 Player_MaskIdToItemId(s32 maskIdMinusOne);
s32 Player_GetCurMaskItemId(struct PlayState* play);
void func_80122F28(Player* player);
bool func_80122F9C(struct PlayState* play);
bool func_80122FCC(struct PlayState* play);
void func_8012300C(struct PlayState* play, s32 arg1);
void func_8012301C(Actor* thisx, struct PlayState* play2);
void func_80123140(struct PlayState* play, Player* player);
bool Player_InBlockingCsMode(struct PlayState* play, Player* player);
bool Player_InCsMode(struct PlayState* play);
bool Player_CheckHostileLockOn(Player* player);
bool Player_FriendlyLockOnOrParallel(Player* player);
bool func_80123448(struct PlayState* play);
bool Player_IsGoronOrDeku(Player* player);
bool func_801234D4(struct PlayState* play);
bool func_80123590(struct PlayState* play, Actor* actor);
// #region 2S2H [Dpad]
ItemId Player_Dpad_GetItemOnButton(struct PlayState* play, Player* player, DpadEquipSlot slot);
// #endregion
ItemId Player_GetItemOnButton(struct PlayState* play, Player* player, EquipSlot slot);
PlayerItemAction func_80123810(struct PlayState* play);
PlayerModelGroup Player_ActionToModelGroup(Player* player, PlayerItemAction itemAction);
void Player_SetModelsForHoldingShield(Player* player);
void Player_SetModels(Player* player, PlayerModelGroup modelGroup);
void Player_SetModelGroup(Player* player, PlayerModelGroup modelGroup);
void func_80123C58(Player* player);
void Player_SetEquipmentData(struct PlayState* play, Player* player);
void Player_UpdateBottleHeld(struct PlayState* play, Player* player, ItemId itemId, PlayerItemAction itemAction);
void Player_ReleaseLockOn(Player* player);
void Player_ClearZTargeting(Player* player);
void Player_SetAutoLockOnActor(struct PlayState* play, Actor* actor);
s32 Player_SetBButtonAmmo(struct PlayState* play, s32 ammo);
bool Player_IsBurningStickInRange(struct PlayState* play, Vec3f* pos, f32 xzRange, f32 yRange);
u8 Player_GetStrength(void);
PlayerMask Player_GetMask(struct PlayState* play);
void Player_RemoveMask(struct PlayState* play);
bool Player_HasMirrorShieldEquipped(struct PlayState* play);
bool Player_IsHoldingMirrorShield(struct PlayState* play);
bool Player_IsHoldingHookshot(Player* player);
bool func_801240DC(Player* player);
PlayerBButtonSword Player_BButtonSwordFromIA(Player* player, PlayerItemAction itemAction);
PlayerBButtonSword Player_GetHeldBButtonSword(Player* player);
PlayerMeleeWeapon Player_MeleeWeaponFromIA(PlayerItemAction itemAction);
PlayerMeleeWeapon Player_GetMeleeWeaponHeld(Player* player);
s32 Player_IsHoldingTwoHandedWeapon(Player* player);
PlayerBottle Player_BottleFromIA(Player* player, PlayerItemAction itemAction);
PlayerBottle Player_GetBottleHeld(Player* Player);
PlayerExplosive Player_ExplosiveFromIA(Player* player, PlayerItemAction itemAction);
PlayerExplosive Player_GetExplosiveHeld(Player* player);
PlayerSword Player_SwordFromIA(Player* player, PlayerItemAction itemAction);
bool func_801242B4(Player* player);
s32 Player_GetEnvironmentalHazard(struct PlayState* play);
void Player_UpdateBunnyEars(Player* player);
void func_80124618(struct_80124618 arg0[], f32 curFrame, Vec3f* arg2);
void Player_DrawImpl(struct PlayState* play, void** skeleton, Vec3s* jointTable, s32 dListCount, s32 lod, PlayerTransformation playerForm, s32 boots, s32 face, OverrideLimbDrawFlex overrideLimbDraw, PostLimbDrawFlex postLimbDraw, Actor* actor);
void func_80125318(Vec3f* arg0, Vec3s* arg1);
void Player_DrawZoraShield(struct PlayState* play, Player* player);
void func_80125500(struct PlayState* play, Player* player, s32 limbIndex, Vec3f* pos, Vec3s* rot);
s32 Player_OverrideLimbDrawGameplayDefault(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* actor);
s32 Player_OverrideLimbDrawGameplayFirstPerson(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* actor);
s32 Player_OverrideLimbDrawGameplayCrawling(struct PlayState* play, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, Actor* thisx);
s32 func_80126440(struct PlayState* play, ColliderQuad* collider, WeaponInfo* weaponInfo, Vec3f* newTip, Vec3f* newBase);
void Player_DrawGetItem(struct PlayState* play, Player* player);
void func_80126B8C(struct PlayState* play, Player* player);
s32 func_80127438(struct PlayState* play, Player* player, s32 currentMask);
s32 func_80128640(struct PlayState* play, Player* player, Gfx* dlist);
void Player_SetFeetPos(struct PlayState* play, Player* player, s32 limbIndex);
void Player_PostLimbDrawGameplay(struct PlayState* play, s32 limbIndex, Gfx** dList1, Gfx** dList2, Vec3s* rot, Actor* actor);

#endif
