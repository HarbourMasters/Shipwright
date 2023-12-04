#include "ActorDB.h"

#include <assert.h>

ActorDB* ActorDB::Instance;


#define DEFINE_ACTOR(name, _1, _2) extern "C" ActorInit name##_InitVars;
#define DEFINE_ACTOR_INTERNAL(name, _1, _2) extern "C" ActorInit name##_InitVars;
#define DEFINE_ACTOR_UNSET(_0)

#include "tables/actor_table.h"

#undef DEFINE_ACTOR
#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET

struct AddPair {
    std::string name;
    ActorInit& init;
};

#define DEFINE_ACTOR_INTERNAL(name, _1, allocType) { #name, name##_InitVars },
#define DEFINE_ACTOR(name, _1, allocType) { #name, name##_InitVars },
#define DEFINE_ACTOR_UNSET(_0)

static const std::vector<AddPair> initialActorTable = {
#include "tables/actor_table.h"
};

#undef DEFINE_ACTOR_INTERNAL
#undef DEFINE_ACTOR_UNSET
#undef DEFINE_ACTOR

// https://wiki.cloudmodding.com/oot/Actor_List_(Variables)
static std::unordered_map<u16, const char*> actorDescriptions = {
    { ACTOR_PLAYER, "Link" },
    { ACTOR_EN_TEST, "Stalfos" },
    { ACTOR_EN_GIRLA, "Shop Items" },
    { ACTOR_EN_PART, "Body Parts" },
    { ACTOR_EN_LIGHT, "Decorative Flames" },
    { ACTOR_EN_DOOR, "Door" },
    { ACTOR_EN_BOX, "Treasure Chest" },
    { ACTOR_BG_DY_YOSEIZO, "Great Fairy" },
    { ACTOR_BG_HIDAN_FIREWALL, "Proximity activated Fire Wall" },
    { ACTOR_EN_POH, "Poe" },
    { ACTOR_EN_OKUTA, "Octorok" },
    { ACTOR_BG_YDAN_SP, "Webs" },
    { ACTOR_EN_BOM, "Bomb" },
    { ACTOR_EN_WALLMAS, "Wallmaster" },
    { ACTOR_EN_DODONGO, "Dodongo" },
    { ACTOR_EN_FIREFLY, "Keese" },
    { ACTOR_EN_HORSE, "Rideable Horse" },
    { ACTOR_EN_ITEM00, "Collectibles" },
    { ACTOR_EN_ARROW, "Arrow" },
    { ACTOR_EN_ELF, "Fairy" },
    { ACTOR_EN_NIW, "Cucco" },
    { ACTOR_EN_TITE, "Tektite" },
    { ACTOR_EN_REEBA, "Leever" },
    { ACTOR_EN_PEEHAT, "Peahat and Larva" },
    { ACTOR_EN_BUTTE, "Butterfly" },
    { ACTOR_EN_INSECT, "Bugs" },
    { ACTOR_EN_FISH, "Fish" },
    { ACTOR_EN_HOLL, "Room changing plane" },
    { ACTOR_EN_SCENE_CHANGE, "ovl_En_Scene_Change (broken)" },
    { ACTOR_EN_ZF, "Lizalfos and Dinolfos" },
    { ACTOR_EN_HATA, "Wooden Flagpole with Red Cloth" },
    { ACTOR_BOSS_DODONGO, "King Dodongo" },
    { ACTOR_BOSS_GOMA, "Gohma" },
    { ACTOR_EN_ZL1, "Child Princess Zelda (at window)" },
    { ACTOR_EN_VIEWER, "Cutscene Actors" },
    { ACTOR_EN_GOMA, "Gohma Larva" },
    { ACTOR_BG_PUSHBOX, "Cube" },
    { ACTOR_EN_BUBBLE, "Shabom" },
    { ACTOR_DOOR_SHUTTER, "Shutter Door" },
    { ACTOR_EN_DODOJR, "Baby Dodongo" },
    { ACTOR_EN_BDFIRE, "Empty" },
    { ACTOR_EN_BOOM, "Boomerang" },
    { ACTOR_EN_TORCH2, "Dark Link" },
    { ACTOR_EN_BILI, "Biri" },
    { ACTOR_EN_TP, "Electric Tailpasaran" },
    { ACTOR_EN_ST, "Skulltula" },
    { ACTOR_EN_BW, "Torch Slug" },
    { ACTOR_EN_A_OBJ, "Gameplay_keep items" },
    { ACTOR_EN_EIYER, "Stinger (Land)" },
    { ACTOR_EN_RIVER_SOUND, "Ambient Sound Effects" },
    { ACTOR_EN_HORSE_NORMAL, "Horse" },
    { ACTOR_EN_OSSAN, "Shopkeeper" },
    { ACTOR_BG_TREEMOUTH, "Great Deku Tree's Jaw" },
    { ACTOR_BG_DODOAGO, "Dodongo's Cavern Mega Dodongo" },
    { ACTOR_BG_HIDAN_DALM, "Megaton Hammer Statue" },
    { ACTOR_BG_HIDAN_HROCK, "Huge stone spike platform (Fire Temple)" },
    { ACTOR_EN_HORSE_GANON, "Ganondorf's Horse" },
    { ACTOR_BG_HIDAN_ROCK, "Stone Blocks (Fire Temple)" },
    { ACTOR_BG_HIDAN_RSEKIZOU, "Spinning Stone Flamethrower (Fire Temple)" },
    { ACTOR_BG_HIDAN_SEKIZOU, "Stationary Flamethrower Statue (Fire Temple)" },
    { ACTOR_BG_HIDAN_SIMA, "Stone Platform (Fire Temple)" },
    { ACTOR_BG_HIDAN_SYOKU, "Stone Elevator (Fire Temple)" },
    { ACTOR_EN_XC, "Sheik" },
    { ACTOR_BG_HIDAN_CURTAIN, "Flame Circle" },
    { ACTOR_BG_SPOT00_HANEBASI, "Drawbridge Objects" },
    { ACTOR_EN_MB, "Moblins" },
    { ACTOR_EN_BOMBF, "Bombflower" },
    { ACTOR_EN_ZL2, "Adult Zelda (Cutscenes)" },
    { ACTOR_BG_HIDAN_FSLIFT, "Hookshot Elevator Platform (Fire Temple)" },
    { ACTOR_EN_OE2, "Unused NPC" },
    { ACTOR_BG_YDAN_HASI, "Deku Tree Puzzle elements" },
    { ACTOR_BG_YDAN_MARUTA, "Rotating Spiked Log (Deku Tree)" },
    { ACTOR_BOSS_GANONDROF, "Phantom Ganon" },
    { ACTOR_EN_AM, "Armos Statue" },
    { ACTOR_EN_DEKUBABA, "Deku Baba" },
    { ACTOR_EN_M_FIRE1, "Thrown Deku Nut" },
    { ACTOR_EN_M_THUNDER, "Spin Attack" },
    { ACTOR_BG_DDAN_JD, "Rising Stone Platform (Dodongo's Cavern)" },
    { ACTOR_BG_BREAKWALL, "Bombable Wall" },
    { ACTOR_EN_JJ, "Lord Jabu-Jabu" },
    { ACTOR_EN_HORSE_ZELDA, "Zelda's Horse" },
    { ACTOR_BG_DDAN_KD, "Stone Stairs (Dodongo's Cavern)" },
    { ACTOR_DOOR_WARP1, "Warp Portal" },
    { ACTOR_OBJ_SYOKUDAI, "Torch" },
    { ACTOR_ITEM_B_HEART, "Heart container" },
    { ACTOR_EN_DEKUNUTS, "Mad Scrub" },
    { ACTOR_BG_MENKURI_KAITEN, "Rotating Stone Ring Platform" },
    { ACTOR_BG_MENKURI_EYE, "Eye Statue" },
    { ACTOR_EN_VALI, "Bari" },
    { ACTOR_BG_MIZU_MOVEBG, "Water Temple Background Actors" },
    { ACTOR_BG_MIZU_WATER, "Water Plane (Water Temple)" },
    { ACTOR_ARMS_HOOK, "Hookshot" },
    { ACTOR_EN_FHG, "Phantom Ganon's Horse" },
    { ACTOR_BG_MORI_HINERI, "Twisted Hallway Rooms" },
    { ACTOR_EN_BB, "Bubble" },
    { ACTOR_BG_TOKI_HIKARI, "Windows (Temple of Time)" },
    { ACTOR_EN_YUKABYUN, "Flying Floor Tile" },
    { ACTOR_BG_TOKI_SWD, "Master Sword" },
    { ACTOR_EN_FHG_FIRE, "Empty" },
    { ACTOR_BG_MJIN, "Warp Song Pad" },
    { ACTOR_BG_HIDAN_KOUSI, "Sliding Metal Gate" },
    { ACTOR_DOOR_TOKI, "Door of Time Collision" },
    { ACTOR_BG_HIDAN_HAMSTEP, "Stone Steps and Platforms (Fire Temple)" },
    { ACTOR_EN_BIRD, "Brown Bird" },
    { ACTOR_EN_WOOD02, "Trees and Bushes" },
    { ACTOR_EN_LIGHTBOX, "Large noisy stone" },
    { ACTOR_EN_PU_BOX, "Stone cube" },
    { ACTOR_EN_TRAP, "Metal Spike Trap" },
    { ACTOR_EN_AROW_TRAP, "Arrow Trap" },
    { ACTOR_EN_VASE, "Orange Pot" },
    { ACTOR_EN_TA, "Talon" },
    { ACTOR_EN_TK, "Dampe (Alive)" },
    { ACTOR_BG_MORI_BIGST, "Large Round Platform (Forest Temple)" },
    { ACTOR_BG_MORI_ELEVATOR, "Elevator (Forest Temple)" },
    { ACTOR_BG_MORI_KAITENKABE, "Rotatable Walls (Forest Temple)" },
    { ACTOR_BG_MORI_RAKKATENJO, "Falling Ceiling (Forest Temple)" },
    { ACTOR_EN_VM, "Beamos" },
    { ACTOR_DEMO_EFFECT, "Spiritual Stones, Medallions, Triforce" },
    { ACTOR_DEMO_KANKYO, "Temple of Time Objects" },
    { ACTOR_BG_HIDAN_FWBIG, "Large Fire Wall (Fire Temple)" },
    { ACTOR_EN_FLOORMAS, "Floormaster" },
    { ACTOR_EN_HEISHI1, "Castle Courtyard Guards" },
    { ACTOR_EN_RD, "Redead and Gibdo" },
    { ACTOR_EN_PO_SISTERS, "Poe Sisters" },
    { ACTOR_BG_HEAVY_BLOCK, "Golden Gauntlets Pillar" },
    { ACTOR_BG_PO_EVENT, "Poe Sisters' Paintings and Puzzle Block" },
    { ACTOR_OBJ_MURE, "Fish, Bugs, Butterflies Group Spawner" },
    { ACTOR_EN_SW, "Skullwalltula and Gold Skulltulas" },
    { ACTOR_BOSS_FD, "Volvagia (Flying)" },
    { ACTOR_OBJECT_KANKYO, "Environmental Effects" },
    { ACTOR_EN_DU, "Darunia" },
    { ACTOR_EN_FD, "Flare Dancer" },
    { ACTOR_EN_HORSE_LINK_CHILD, "Young Epona" },
    { ACTOR_DOOR_ANA, "Grotto Entrance" },
    { ACTOR_BG_SPOT02_OBJECTS, "Graveyard Actors" },
    { ACTOR_BG_HAKA, "Gravestone" },
    { ACTOR_MAGIC_WIND, "Farore's Wind" },
    { ACTOR_MAGIC_FIRE, "Din's Fire" },
    { ACTOR_EN_RU1, "Ruto (Child)" },
    { ACTOR_BOSS_FD2, "Volvagia (Hole Form)" },
    { ACTOR_EN_FD_FIRE, "Flare Dancer Fire Attack" },
    { ACTOR_EN_DH, "Dead Hand" },
    { ACTOR_EN_DHA, "Dead Hand's Hand" },
    { ACTOR_EN_RL, "Rauru" },
    { ACTOR_EN_ENCOUNT1, "Enemy Spawner" },
    { ACTOR_DEMO_DU, "Darunia (Cutscenes)" },
    { ACTOR_DEMO_IM, "Impa (Cutscenes)" },
    { ACTOR_DEMO_TRE_LGT, "Treasure Chest Light" },
    { ACTOR_EN_FW, "Flare Dancer Core" },
    { ACTOR_BG_VB_SIMA, "Volvagia Platform" },
    { ACTOR_EN_VB_BALL, "Volvagia Rocks and Bones" },
    { ACTOR_BG_HAKA_MEGANE, "Shadow Temple Fake Walls" },
    { ACTOR_BG_HAKA_MEGANEBG, "Platforms (Shadow Temple)" },
    { ACTOR_BG_HAKA_SHIP, "River Boat (Shadow Temple)" },
    { ACTOR_BG_HAKA_SGAMI, "Spinning Scythe Trap" },
    { ACTOR_EN_HEISHI2, "Hyrulian Guards" },
    { ACTOR_EN_ENCOUNT2, "Falling Rock Spawner" },
    { ACTOR_EN_FIRE_ROCK, "Falling Burning Rocks" },
    { ACTOR_EN_BROB, "Flobbery Muscle Block (Jabu-Jabu's Belly)" },
    { ACTOR_MIR_RAY, "Reflectable Light Beam" },
    { ACTOR_BG_SPOT09_OBJ, "Gerudo Valley Objects" },
    { ACTOR_BG_SPOT18_OBJ, "Statue (Darunia's Room)" },
    { ACTOR_BOSS_VA, "Barinade" },
    { ACTOR_BG_HAKA_TUBO, "Giant Skull Jar (Shadow Temple)" },
    { ACTOR_BG_HAKA_TRAP, "Shadow Temple Traps" },
    { ACTOR_BG_HAKA_HUTA, "Coffin Lid" },
    { ACTOR_BG_HAKA_ZOU, "Bird Statue and Wall (Shadow Temple)" },
    { ACTOR_BG_SPOT17_FUNEN, "Smoke Cone (Death Mountain Crater)" },
    { ACTOR_EN_SYATEKI_ITM, "Shooting Gallery Game" },
    { ACTOR_EN_SYATEKI_MAN, "Shooting Gallery Man" },
    { ACTOR_EN_TANA, "Shop Shelves" },
    { ACTOR_EN_NB, "Nabooru" },
    { ACTOR_BOSS_MO, "Morpha" },
    { ACTOR_EN_SB, "Shell Blade" },
    { ACTOR_EN_BIGOKUTA, "Big Octo" },
    { ACTOR_EN_KAREBABA, "Withered Deku Baba" },
    { ACTOR_BG_BDAN_OBJECTS, "Inside Jabu-Jabu's Belly Objects" },
    { ACTOR_DEMO_SA, "Saria (Cutscenes)" },
    { ACTOR_DEMO_GO, "Gorons (Cutscene)" },
    { ACTOR_EN_IN, "Ingo" },
    { ACTOR_EN_TR, "Koume and Kotake" },
    { ACTOR_BG_SPOT16_BOMBSTONE, "Dodongo's Cavern Entrance Blocking Rock" },
    { ACTOR_BG_HIDAN_KOWARERUKABE, "Bombable Walls (Fire Temple)" },
    { ACTOR_BG_BOMBWALL, "2D Bombable Wall" },
    { ACTOR_BG_SPOT08_ICEBLOCK, "Ice Platform (Zora's Fountain)" },
    { ACTOR_EN_RU2, "Ruto (Adult)" },
    { ACTOR_OBJ_DEKUJR, "Deku Tree Sprout" },
    { ACTOR_BG_MIZU_UZU, "Water Vortex Effect (Water Temple)" },
    { ACTOR_BG_SPOT06_OBJECTS, "Lake Hylia Objects" },
    { ACTOR_BG_ICE_OBJECTS, "Pushable Ice Block (Ice Cavern)" },
    { ACTOR_BG_HAKA_WATER, "Bottom of the Well Water Level Changer" },
    { ACTOR_EN_MA2, "Malon (Adult, Ingo's Ranch)" },
    { ACTOR_EN_BOM_CHU, "Bombchu" },
    { ACTOR_EN_HORSE_GAME_CHECK, "Horseback Minigames" },
    { ACTOR_BOSS_TW, "Twinrova" },
    { ACTOR_EN_RR, "Like-Like" },
    { ACTOR_EN_BA, "Tentacle (Inside Jabu-Jabu's Belly)" },
    { ACTOR_EN_BX, "Electrified Tentacle (Inside Jabu-Jabu's Belly)" },
    { ACTOR_EN_ANUBICE, "Anubis (Body)" },
    { ACTOR_EN_ANUBICE_FIRE, "Anubis Fire Attack" },
    { ACTOR_BG_MORI_HASHIGO, "Ladder (Forest Temple)" },
    { ACTOR_BG_MORI_HASHIRA4, "Forest Temple Objects" },
    { ACTOR_BG_MORI_IDOMIZU, "Well Water (Forest Temple)" },
    { ACTOR_BG_SPOT16_DOUGHNUT, "Cloud Ring (Death Mountain)" },
    { ACTOR_BG_BDAN_SWITCH, "Switches (Inside Lord Jabu-Jabu)" },
    { ACTOR_EN_MA1, "Malon (Child)" },
    { ACTOR_BOSS_GANON, "Ganondorf" },
    { ACTOR_BOSS_SST, "Bongo Bongo" },
    { ACTOR_EN_NY, "Spike (Enemy)" },
    { ACTOR_EN_FR, "Frog Song Spot and Frogs" },
    { ACTOR_ITEM_SHIELD, "Deku Shield" },
    { ACTOR_BG_ICE_SHELTER, "Red Ice" },
    { ACTOR_EN_ICE_HONO, "Blue Fire Flames" },
    { ACTOR_ITEM_OCARINA, "Ocarina of Time" },
    { ACTOR_MAGIC_DARK, "Nayru's Love" },
    { ACTOR_DEMO_6K, "Sages, Balls of Light (Cutscene)" },
    { ACTOR_EN_ANUBICE_TAG, "Anubis Spawn" },
    { ACTOR_BG_HAKA_GATE, "Truth Spinner Puzzle (Shadow Temple)" },
    { ACTOR_BG_SPOT15_SAKU, "Hyrule Castle Gate" },
    { ACTOR_BG_JYA_GOROIWA, "Rolling Boulder (Spirit Temple)" },
    { ACTOR_BG_JYA_ZURERUKABE, "Sliding, Climbable Brick Wall (Spirit Temple)" },
    { ACTOR_BG_JYA_COBRA, "Rotatable Cobra Mirror (Spirit Temple)" },
    { ACTOR_BG_JYA_KANAAMI, "Climbable Metal Grating Bridge (Spirit Temple)" },
    { ACTOR_FISHING, "Fishing Pond Man and Fish" },
    { ACTOR_OBJ_OSHIHIKI, "Pushable Block" },
    { ACTOR_BG_GATE_SHUTTER, "Gate to Death Mountain Trail" },
    { ACTOR_EFF_DUST, "Dust Effects" },
    { ACTOR_BG_SPOT01_FUSYA, "Windmill Sails (Kakariko Village)" },
    { ACTOR_BG_SPOT01_IDOHASHIRA, "Well Crossbeam (Kakariko Village)" },
    { ACTOR_BG_SPOT01_IDOMIZU, "Well Water (Kakariko Village)" },
    { ACTOR_BG_PO_SYOKUDAI, "Golden Torch (Poe Sisters)" },
    { ACTOR_BG_GANON_OTYUKA, "Falling Platform (Ganondorf Fight)" },
    { ACTOR_BG_SPOT15_RRBOX, "Milk Crate" },
    { ACTOR_BG_UMAJUMP, "Obstacle Fence (Lon Lon Ranch)" },
    { ACTOR_ARROW_FIRE, "Fire Arrow" },
    { ACTOR_ARROW_ICE, "Ice Arrow" },
    { ACTOR_ARROW_LIGHT, "Light Arrow" },
    { ACTOR_ITEM_ETCETERA, "Collectible Items" },
    { ACTOR_OBJ_KIBAKO, "Small Liftable Crate" },
    { ACTOR_OBJ_TSUBO, "Breakable Pot" },
    { ACTOR_EN_WONDER_ITEM, "Invisible Collectible" },
    { ACTOR_EN_IK, "Iron Knuckle" },
    { ACTOR_DEMO_IK, "Iron Knuckle armor pieces (Spirit Temple Nabooru fight)" },
    { ACTOR_EN_SKJ, "Skullkid" },
    { ACTOR_EN_SKJNEEDLE, "Skullkid Needle Attack" },
    { ACTOR_EN_G_SWITCH, "Silver Rupee" },
    { ACTOR_DEMO_EXT, "Magic Vortex" },
    { ACTOR_DEMO_SHD, "Bongo Bongo's Shadow" },
    { ACTOR_EN_DNS, "Business Scrub" },
    { ACTOR_ELF_MSG, "Navi Message" },
    { ACTOR_EN_HONOTRAP, "Stone Eye (Fire Trap) and Flame" },
    { ACTOR_EN_TUBO_TRAP, "Flying Pot" },
    { ACTOR_OBJ_ICE_POLY, "Ice spawned by red ice? frozen actors" },
    { ACTOR_BG_SPOT03_TAKI, "Zora's River Waterfall" },
    { ACTOR_BG_SPOT07_TAKI, "Zora's Domain Waterfall and Ice" },
    { ACTOR_EN_FZ, "Frezzard" },
    { ACTOR_EN_PO_RELAY, "Dampe's Ghost" },
    { ACTOR_BG_RELAY_OBJECTS, "Windmill Objects" },
    { ACTOR_EN_DIVING_GAME, "Zora Diving Game" },
    { ACTOR_EN_KUSA, "Bush/Grass" },
    { ACTOR_OBJ_BEAN, "Bean Plant Spot" },
    { ACTOR_OBJ_BOMBIWA, "Brown Bombable Boulder" },
    { ACTOR_OBJ_SWITCH, "Switches" },
    { ACTOR_OBJ_ELEVATOR, "Huge Stone Elevator" },
    { ACTOR_OBJ_LIFT, "Square Collapsing Platform" },
    { ACTOR_OBJ_HSBLOCK, "Stone Hookshot Target" },
    { ACTOR_EN_OKARINA_TAG, "Ocarina Music Staff Spot" },
    { ACTOR_EN_YABUSAME_MARK, "Horseback Archery Target" },
    { ACTOR_EN_GOROIWA, "Rolling Boulder" },
    { ACTOR_EN_EX_RUPPY, "Sparkling Rupee" },
    { ACTOR_EN_TORYO, "Boss Carpenter" },
    { ACTOR_EN_DAIKU, "Carpenters" },
    { ACTOR_EN_NWC, "Cucco Chick" },
    { ACTOR_EN_BLKOBJ, "Dark Link's Illusion Room" },
    { ACTOR_ITEM_INBOX, "Zelda's Magic to Open Gates" },
    { ACTOR_EN_GE1, "White Clothed Gerudo" },
    { ACTOR_OBJ_BLOCKSTOP, "Pushblock Stop" },
    { ACTOR_EN_SDA, "Dynamic Shadow" },
    { ACTOR_EN_CLEAR_TAG, "Arwing" },
    { ACTOR_EN_NIW_LADY, "Cucco Lady" },
    { ACTOR_EN_GM, "Medigoron" },
    { ACTOR_EN_MS, "Bean Salesman" },
    { ACTOR_EN_HS, "Carpenter's Son" },
    { ACTOR_BG_INGATE, "Ingo's Gates (Lon Lon Ranch)" },
    { ACTOR_EN_KANBAN, "Square Signpost" },
    { ACTOR_EN_HEISHI3, "Hyrule Castle Guard" },
    { ACTOR_EN_SYATEKI_NIW, "Cucco (Minigames)" },
    { ACTOR_EN_ATTACK_NIW, "Cucco (Attacking)" },
    { ACTOR_BG_SPOT01_IDOSOKO, "Stone Blocking Entrance to Bottom of the Well" },
    { ACTOR_EN_SA, "Saria" },
    { ACTOR_EN_WONDER_TALK, "Checkable Spot (Green Navi)" },
    { ACTOR_BG_GJYO_BRIDGE, "Rainbow Bridge to Ganon's Castle" },
    { ACTOR_EN_DS, "Potion Shop Granny" },
    { ACTOR_EN_MK, "Lakeside Professor" },
    { ACTOR_EN_BOM_BOWL_MAN, "Bombchu Bowling Alley Lady" },
    { ACTOR_EN_BOM_BOWL_PIT, "Bombchu Bowling Alley Final Target" },
    { ACTOR_EN_OWL, "Kaepora Gaebora" },
    { ACTOR_EN_ISHI, "Liftable Rock" },
    { ACTOR_OBJ_HANA, "Uninteractable Flowers, Rocks, Grass" },
    { ACTOR_OBJ_LIGHTSWITCH, "Sun Emblem Switch (Spirit Temple)" },
    { ACTOR_OBJ_MURE2, "Rock/Bush groups" },
    { ACTOR_EN_GO, "Gorons 1" },
    { ACTOR_EN_FU, "Windmill Man" },
    { ACTOR_EN_CHANGER, "Treasure Box Shop Minigame" },
    { ACTOR_BG_JYA_MEGAMI, "Statue Face (Spirit Temple)" },
    { ACTOR_BG_JYA_LIFT, "Chain Platform (Spirit Temple)" },
    { ACTOR_BG_JYA_BIGMIRROR, "Large Circular Mirror (Spirt Temple)" },
    { ACTOR_BG_JYA_BOMBCHUIWA, "Light Blocking Rock (Spirit Temple)" },
    { ACTOR_BG_JYA_AMISHUTTER, "Sliding Circular Metal Grate (Spirit Temple)" },
    { ACTOR_BG_JYA_BOMBIWA, "Bombable Rock Wall (Spirit Temple)" },
    { ACTOR_BG_SPOT18_BASKET, "Big Goron Pot" },
    { ACTOR_EN_GANON_ORGAN, "Ganon's Organ and surroundings" },
    { ACTOR_EN_SIOFUKI, "Water Spout" },
    { ACTOR_EN_STREAM, "Water Vortex" },
    { ACTOR_EN_MM, "Running Man (Child Era)" },
    { ACTOR_EN_KO, "Kokiri Children" },
    { ACTOR_EN_KZ, "King Zora" },
    { ACTOR_EN_WEATHER_TAG, "Proximity Weather Effects" },
    { ACTOR_BG_SST_FLOOR, "Bongo Bongo's Drum" },
    { ACTOR_EN_ANI, "Kakariko Village Rooftop Man" },
    { ACTOR_EN_EX_ITEM, "Minigame Displayed Items" },
    { ACTOR_BG_JYA_IRONOBJ, "Iron Knuckles Room Stuff (Spirit Temple)" },
    { ACTOR_EN_JS, "Magic Carpet Man" },
    { ACTOR_EN_JSJUTAN, "Magic Carpet Man Carpet" },
    { ACTOR_EN_CS, "Graveyard Boy" },
    { ACTOR_EN_MD, "Mido" },
    { ACTOR_EN_HY, "Market NPCs" },
    { ACTOR_EN_GANON_MANT, "Ganondorf's Cape" },
    { ACTOR_EN_OKARINA_EFFECT, "Song of Storms Storm Manager" },
    { ACTOR_EN_MAG, "Title Screen Manager" },
    { ACTOR_DOOR_GERUDO, "Cell Door (Gerudo Fortress)" },
    { ACTOR_ELF_MSG2, "Navi Information Spot (Targetable, Green)" },
    { ACTOR_DEMO_GT, "Ganon's Tower Collapsing (Cutscene Objects)" },
    { ACTOR_EN_PO_FIELD, "Big/Small Poe Spawn Point" },
    { ACTOR_EFC_ERUPC, "Lava Particle Fountain (Death Mountain panorama)" },
    { ACTOR_BG_ZG, "Metal Bars (Ganon's Castle)" },
    { ACTOR_EN_HEISHI4, "Hyrule Guard" },
    { ACTOR_EN_ZL3, "Adult Zelda" },
    { ACTOR_BOSS_GANON2, "Ganon" },
    { ACTOR_EN_KAKASI, "Pierre the Scarecrow" },
    { ACTOR_EN_TAKARA_MAN, "Treasure Box Shop Man" },
    { ACTOR_OBJ_MAKEOSHIHIKI, "Push Block Puzzles" },
    { ACTOR_OCEFF_SPOT, "Sun's Song Effect" },
    { ACTOR_END_TITLE, "The End message" },
    { ACTOR_EN_TORCH, "Grotto Treasure Chest" },
    { ACTOR_DEMO_EC, "Credits Revelers in Lon Lon" },
    { ACTOR_SHOT_SUN, "Lake Hylia Sun Hitbox, Big Fairy Spawner" },
    { ACTOR_EN_DY_EXTRA, "Spiral Beams (Great Fairy Fountains)" },
    { ACTOR_EN_WONDER_TALK2, "Dialog Spot" },
    { ACTOR_EN_GE2, "Patrolling Gerudo" },
    { ACTOR_OBJ_ROOMTIMER, "Room Timer" },
    { ACTOR_EN_SSH, "Cursed Skulltula People" },
    { ACTOR_EN_STH, "Uncursed Skulltula People" },
    { ACTOR_OCEFF_WIPE, "Zelda's Lullaby and Song of Time Ocarina Effect" },
    { ACTOR_OCEFF_STORM, "Song of Storm Ocarina Effect" },
    { ACTOR_EN_WEIYER, "Stinger (Water)" },
    { ACTOR_BG_SPOT05_SOKO, "Sacred Forest Meadow Objects" },
    { ACTOR_BG_JYA_1FLIFT, "Stone Elevator (Spirit Temple)" },
    { ACTOR_BG_JYA_HAHENIRON, "Chunks of Iron Knucle Chair and Pillar" },
    { ACTOR_BG_SPOT12_GATE, "Gerudo Fortress Wooden Gate" },
    { ACTOR_BG_SPOT12_SAKU, "Gerudo Fortress Training Area Gate" },
    { ACTOR_EN_HINTNUTS, "Hint Deku Scrubs (Deku Tree)" },
    { ACTOR_EN_NUTSBALL, "Deku Scrub Nut Attack" },
    { ACTOR_BG_SPOT00_BREAK, "Broken Drawbridge, Fences" },
    { ACTOR_EN_SHOPNUTS, "Grounded Sales Scrub" },
    { ACTOR_EN_IT, "Dampe's Minigame Collectibles" },
    { ACTOR_EN_GELDB, "Gerudo Fighter" },
    { ACTOR_OCEFF_WIPE2, "Epona's Song Ocarina Effect" },
    { ACTOR_OCEFF_WIPE3, "Saria's Song Ocarina Effect" },
    { ACTOR_EN_NIW_GIRL, "Girl Chasing Cucco" },
    { ACTOR_EN_DOG, "Dog" },
    { ACTOR_EN_SI, "Gold Skulltula Token" },
    { ACTOR_BG_SPOT01_OBJECTS2, "Kakariko Village Objects" },
    { ACTOR_OBJ_COMB, "Beehive" },
    { ACTOR_BG_SPOT11_BAKUDANKABE, "Destructible Wall (Desert Colossus)" },
    { ACTOR_OBJ_KIBAKO2, "Large Crate" },
    { ACTOR_EN_DNT_DEMO, "Deku Mask Panel Trigger" },
    { ACTOR_EN_DNT_JIJI, "Deku Mask Panel Head Judge" },
    { ACTOR_EN_DNT_NOMAL, "Deku Game?" },
    { ACTOR_EN_GUEST, "Happy Mask Shop Customer" },
    { ACTOR_BG_BOM_GUARD, "Bombchu Bowling Alley Aiming Area" },
    { ACTOR_EN_HS2, "Carpenter's Son (Child Era)" },
    { ACTOR_DEMO_KEKKAI, "Ganon's Tower Magic Barriers" },
    { ACTOR_BG_SPOT08_BAKUDANKABE, "Destructible Wall (Zora's Fountain)" },
    { ACTOR_BG_SPOT17_BAKUDANKABE, "Destructible Wall (Death Mountain Crater)" },
    { ACTOR_OBJ_MURE3, "Rupee Patterns" },
    { ACTOR_EN_TG, "Entwined Lovers (Honey & Darling)" },
    { ACTOR_EN_MU, "Haggling Townspeople" },
    { ACTOR_EN_GO2, "Gorons 2" },
    { ACTOR_EN_WF, "Wolfos" },
    { ACTOR_EN_SKB, "Stalchild" },
    { ACTOR_DEMO_GJ, "Ganon Battle Rubble" },
    { ACTOR_DEMO_GEFF, "Ganon's Tower Rubble Fragment" },
    { ACTOR_BG_GND_FIREMEIRO, "Sinking Lava Platform (Ganon's Castle)" },
    { ACTOR_BG_GND_DARKMEIRO, "Clear block" },
    { ACTOR_BG_GND_SOULMEIRO, "Web-Blocked Ceiling Hole (Inside Ganon's Castle)" },
    { ACTOR_BG_GND_NISEKABE, "Ganon's Castle Fake Walls" },
    { ACTOR_BG_GND_ICEBLOCK, "Pushable Square Ice Block (Inside Ganon's Castle)" },
    { ACTOR_EN_GB, "Poe Collector and Surroundings" },
    { ACTOR_EN_GS, "Gossip Stone" },
    { ACTOR_BG_MIZU_BWALL, "Bombable Stone Wall" },
    { ACTOR_BG_MIZU_SHUTTER, "Metal Gate (Water Temple)" },
    { ACTOR_EN_DAIKU_KAKARIKO, "Carpenters (Kakariko)" },
    { ACTOR_BG_BOWL_WALL, "Bombchu Bowling Alley Wall" },
    { ACTOR_EN_WALL_TUBO, "Bombchu Bowling Alley Bullseyes" },
    { ACTOR_EN_PO_DESERT, "Poe Guide (Haunted Wasteland)" },
    { ACTOR_EN_CROW, "Guay" },
    { ACTOR_DOOR_KILLER, "Fake Door" },
    { ACTOR_BG_SPOT11_OASIS, "Oasis (Desert Colossus)" },
    { ACTOR_BG_SPOT18_FUTA, "Goron Jar Lid" },
    { ACTOR_BG_SPOT18_SHUTTER, "Sliding Doors (Goron City)" },
    { ACTOR_EN_MA3, "Malon (Adult, Lon Lon Ranch)" },
    { ACTOR_EN_COW, "Cow" },
    { ACTOR_BG_ICE_TURARA, "Icicles" },
    { ACTOR_BG_ICE_SHUTTER, "Vertical Ice Bars (Ice Cavern)" },
    { ACTOR_EN_KAKASI2, "Pierre the Scarecrow Spawn" },
    { ACTOR_EN_KAKASI3, "Bonooru the Scarecrow" },
    { ACTOR_OCEFF_WIPE4, "Scarecrow's Song Ocarina Effect" },
    { ACTOR_EN_EG, "Void-out Trigger (Tower Collapse)" },
    { ACTOR_BG_MENKURI_NISEKABE, "False Stone Walls (Gerudo Training Grounds)" },
    { ACTOR_EN_ZO, "Zora" },
    { ACTOR_OBJ_MAKEKINSUTA, "Skulltula Sprouting from Bean Spot" },
    { ACTOR_EN_GE3, "Gerudo Fortress Leader" },
    { ACTOR_OBJ_TIMEBLOCK, "Time Block" },
    { ACTOR_OBJ_HAMISHI, "Bronze Boulder" },
    { ACTOR_EN_ZL4, "Zelda (Child)" },
    { ACTOR_EN_MM2, "Running Man (Adult Era)" },
    { ACTOR_BG_JYA_BLOCK, "Silver Block (Child Era)" },
    { ACTOR_OBJ_WARP2BLOCK, "Navi Infospot (Green, Time Block)" }
};

ActorDB::ActorDB() {
    db.reserve(ACTOR_NUMBER_MAX); // reserve size for all initial entries so we don't do it for each
    for (const AddPair& pair : initialActorTable) {
        Entry& entry = AddEntry(pair.name, actorDescriptions[pair.init.id], pair.init);
    }
}

// Adds an actor at the given index. The name must be unique.
ActorDB::Entry& ActorDB::AddEntry(const std::string& name, const std::string& desc, size_t index) {
    assert(!nameTable.contains(name)); // TODO this should maybe throw instead. We'll need to think about error handling for mods that try to declare the same actor.

    if (db.size() < (index + 1)) {
        db.resize(index + 1);
    }
    Entry& newEntry = db.at(index);
    newEntry.entry.id = index;

    assert(!newEntry.entry.valid);

    nextFreeId = std::max(nextFreeId, index + 1);

    nameTable[name] = newEntry.entry.id;
    newEntry.SetName(name);
    newEntry.SetDesc(desc);

    newEntry.entry.valid = true;

    return newEntry;
}

// Adds an actor with the legacy ActorInit struct. The id is provided in it.
ActorDB::Entry& ActorDB::AddEntry(const std::string& name, const std::string& desc, const ActorInit& init) {
    Entry& entry = AddEntry(name, desc, init.id);

    entry.entry.category = init.category;
    entry.entry.flags = init.flags;
    entry.entry.objectId = init.objectId;
    entry.entry.instanceSize = init.instanceSize;
    entry.entry.init = init.init;
    entry.entry.destroy = init.destroy;
    entry.entry.update = init.update;
    entry.entry.draw = init.draw;
    entry.entry.reset = init.reset;

    return entry;
}

// Adds an actor with the new ActorDBInit struct. The id assigned to the actor is dynamic. Use the return Entry or RetrieveId to get it.
ActorDB::Entry& ActorDB::AddEntry(const ActorDBInit& init) {
    Entry& entry = AddEntry(init.name, init.desc, nextFreeId);

    entry.entry.category = init.category;
    entry.entry.flags = init.flags;
    entry.entry.objectId = init.objectId;
    entry.entry.instanceSize = init.instanceSize;
    entry.entry.init = init.init;
    entry.entry.destroy = init.destroy;
    entry.entry.update = init.update;
    entry.entry.draw = init.draw;
    entry.entry.reset = init.reset;

    return entry;
}

// Get the ActorDB::Entry for the given actor id.
ActorDB::Entry& ActorDB::RetrieveEntry(const int id) {
    static Entry invalid;
    if ((id < 0) || (id >= db.size())) {
        return invalid;
    }
    return db[id];
}

// Get the id for a given actor by name.
int ActorDB::RetrieveId(const std::string& name) {
    auto entry = nameTable.find(name);
    if (entry == nameTable.end()) {
        return -1;
    }

    return entry->second;
}

ActorDB::Entry::Entry() {
    entry.name = nullptr;
    entry.desc = nullptr;
    entry.valid = false;
    entry.id = 0;
    entry.category = 0;
    entry.flags = 0;
    entry.objectId = 0;
    entry.instanceSize = 0;
    entry.init = nullptr;
    entry.destroy = nullptr;
    entry.update = nullptr;
    entry.draw = nullptr;
    entry.reset = nullptr;
    entry.numLoaded = 0;
}

ActorDB::Entry::Entry(const Entry& other) {
    entry = other.entry;
    SetName(other.name);
    SetDesc(other.desc);
}

ActorDB::Entry& ActorDB::Entry::operator=(const Entry& other) {
    entry = other.entry;
    SetName(other.name);
    SetDesc(other.desc);
    return *this;
}

void ActorDB::Entry::SetName(const std::string& newName) {
    name = newName;
    entry.name = name.c_str();
}

void ActorDB::Entry::SetDesc(const std::string& newDesc) {
    desc = newDesc;
    entry.desc = desc.c_str();
}

#include "src/overlays/actors/ovl_En_Partner/z_en_partner.h"
static ActorDBInit EnPartnerInit = {
    "En_Partner",
    "Ivan",
    ACTORCAT_ITEMACTION,
    (ACTOR_FLAG_UPDATE_WHILE_CULLED | ACTOR_FLAG_DRAW_WHILE_CULLED | ACTOR_FLAG_DRAGGED_BY_HOOKSHOT | ACTOR_FLAG_CAN_PRESS_SWITCH),
    OBJECT_GAMEPLAY_KEEP,
    sizeof(EnPartner),
    (ActorFunc)EnPartner_Init,
    (ActorFunc)EnPartner_Destroy,
    (ActorFunc)EnPartner_Update,
    (ActorFunc)EnPartner_Draw,
    nullptr,
};
extern "C" s16 gEnPartnerId;

void ActorDB::AddBuiltInCustomActors() {
    gEnPartnerId = ActorDB::Instance->AddEntry(EnPartnerInit).entry.id;
}

extern "C" ActorDBEntry* ActorDB_Retrieve(const int id) {
    return &ActorDB::Instance->RetrieveEntry(id).entry;
}

extern "C" int ActorDB_RetrieveId(const char* name) {
    return ActorDB::Instance->RetrieveId(name);
}
