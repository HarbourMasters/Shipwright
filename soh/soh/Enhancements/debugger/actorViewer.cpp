#include "actorViewer.h"
#include "../../util.h"
#include "../libultraship/SohImGuiImpl.h"
#include "ImGuiHelpers.h"

#include <array>
#include <bit>
#include <map>
#include <string>

extern "C" {
#include <z64.h>
#include "z64math.h"
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern GlobalContext* gGlobalCtx;

#include "textures/icon_item_static/icon_item_static.h"
#include "textures/icon_item_24_static/icon_item_24_static.h"
}

typedef struct {
    u16 id;
    u16 params;
    Vec3f pos;
    Vec3s rot;
} ActorInfo;

typedef enum {
    LIST,
    TARGET,
    HELD,
    INTERACT
} RetrievalMethod;

std::vector<std::string> acMapping = { 
    "Switch",
    "Background (Prop type 1)",
    "Player",
    "Bomb",
    "NPC",
    "Enemy",
    "Prop type 2",
    "Item/Action",
    "Misc.",
    "Boss",
    "Door",
    "Chest"
};

// https://wiki.cloudmodding.com/oot/Actor_List_(Variables)
const std::string GetActorDescription(u16 id) {
    switch (id) {
        case ACTOR_PLAYER:
            return "Link";
        case ACTOR_UNSET_1:
            return "Empty";
        case ACTOR_EN_TEST:
            return "Stalfos";
        case ACTOR_UNSET_3:
            return "Empty";
        case ACTOR_EN_GIRLA:
            return "Shop Items";
        case ACTOR_UNSET_5:
            return "Empty";
        case ACTOR_UNSET_6:
            return "Empty";
        case ACTOR_EN_PART:
            return "Body Parts";
        case ACTOR_EN_LIGHT:
            return "Decorative Flames";
        case ACTOR_EN_DOOR:
            return "Door";
        case ACTOR_EN_BOX:
            return "Treasure Chest";
        case ACTOR_BG_DY_YOSEIZO:
            return "Great Fairy";
        case ACTOR_BG_HIDAN_FIREWALL:
            return "Proximity activated Fire Wall";
        case ACTOR_EN_POH:
            return "Poe";
        case ACTOR_EN_OKUTA:
            return "Octorok";
        case ACTOR_BG_YDAN_SP:
            return "Webs";
        case ACTOR_EN_BOM:
            return "Bomb";
        case ACTOR_EN_WALLMAS:
            return "Wallmaster";
        case ACTOR_EN_DODONGO:
            return "Dodongo";
        case ACTOR_EN_FIREFLY:
            return "Keese";
        case ACTOR_EN_HORSE:
            return "Rideable Horse";
        case ACTOR_EN_ITEM00:
            return "Collectibles";
        case ACTOR_EN_ARROW:
            return "Arrow";
        case ACTOR_UNSET_17:
            return "Empty";
        case ACTOR_EN_ELF:
            return "Fairy";
        case ACTOR_EN_NIW:
            return "Cucco";
        case ACTOR_UNSET_1A:
            return "Empty";
        case ACTOR_EN_TITE:
            return "Tektite";
        case ACTOR_EN_REEBA:
            return "Leever";
        case ACTOR_EN_PEEHAT:
            return "Peahat and Larva";
        case ACTOR_EN_BUTTE:
            return "Butterfly";
        case ACTOR_UNSET_1F:
            return "Empty";
        case ACTOR_EN_INSECT:
            return "Bugs";
        case ACTOR_EN_FISH:
            return "Fish";
        case ACTOR_UNSET_22:
            return "Empty";
        case ACTOR_EN_HOLL:
            return "Room changing plane";
        case ACTOR_EN_SCENE_CHANGE:
            return "ovl_En_Scene_Change (broken)";
        case ACTOR_EN_ZF:
            return "Lizalfos and Dinolfos";
        case ACTOR_EN_HATA:
            return "Wooden Flagpole with Red Cloth";
        case ACTOR_BOSS_DODONGO:
            return "King Dodongo";
        case ACTOR_BOSS_GOMA:
            return "Gohma";
        case ACTOR_EN_ZL1:
            return "Child Princess Zelda (at window)";
        case ACTOR_EN_VIEWER:
            return "Cutscene Actors";
        case ACTOR_EN_GOMA:
            return "Gohma Larva";
        case ACTOR_BG_PUSHBOX:
            return "Cube";
        case ACTOR_EN_BUBBLE:
            return "Shabom";
        case ACTOR_DOOR_SHUTTER:
            return "Shutter Door";
        case ACTOR_EN_DODOJR:
            return "Baby Dodongo";
        case ACTOR_EN_BDFIRE:
            return "Empty";
        case ACTOR_UNSET_31:
            return "Empty";
        case ACTOR_EN_BOOM:
            return "Boomerang";
        case ACTOR_EN_TORCH2:
            return "Dark Link";
        case ACTOR_EN_BILI:
            return "Biri";
        case ACTOR_EN_TP:
            return "Electric Tailpasaran";
        case ACTOR_UNSET_36:
            return "Empty";
        case ACTOR_EN_ST:
            return "Skulltula";
        case ACTOR_EN_BW:
            return "Torch Slug";
        case ACTOR_EN_A_OBJ:
            return "Gameplay_keep items";
        case ACTOR_EN_EIYER:
            return "Stinger (Land)";
        case ACTOR_EN_RIVER_SOUND:
            return "Ambient Sound Effects";
        case ACTOR_EN_HORSE_NORMAL:
            return "Horse";
        case ACTOR_EN_OSSAN:
            return "Shopkeeper";
        case ACTOR_BG_TREEMOUTH:
            return "Great Deku Tree's Jaw";
        case ACTOR_BG_DODOAGO:
            return "Dodongo's Cavern Mega Dodongo";
        case ACTOR_BG_HIDAN_DALM:
            return "Megaton Hammer Statue";
        case ACTOR_BG_HIDAN_HROCK:
            return "Huge stone spike platform (Fire Temple)";
        case ACTOR_EN_HORSE_GANON:
            return "Ganondorf's Horse";
        case ACTOR_BG_HIDAN_ROCK:
            return "Stone Blocks (Fire Temple)";
        case ACTOR_BG_HIDAN_RSEKIZOU:
            return "Spinning Stone Flamethrower (Fire Temple)";
        case ACTOR_BG_HIDAN_SEKIZOU:
            return "Stationary Flamethrower Statue (Fire Temple)";
        case ACTOR_BG_HIDAN_SIMA:
            return "Stone Platform (Fire Temple)";
        case ACTOR_BG_HIDAN_SYOKU:
            return "Stone Elevator (Fire Temple)";
        case ACTOR_EN_XC:
            return "Sheik";
        case ACTOR_BG_HIDAN_CURTAIN:
            return "Flame Circle";
        case ACTOR_BG_SPOT00_HANEBASI:
            return "Drawbridge Objects";
        case ACTOR_EN_MB:
            return "Moblins";
        case ACTOR_EN_BOMBF:
            return "Bombflower";
        case ACTOR_EN_ZL2:
            return "Adult Zelda (Cutscenes)";
        case ACTOR_BG_HIDAN_FSLIFT:
            return "Hookshot Elevator Platform (Fire Temple)";
        case ACTOR_EN_OE2:
            return "Unused NPC";
        case ACTOR_BG_YDAN_HASI:
            return "Deku Tree Puzzle elements";
        case ACTOR_BG_YDAN_MARUTA:
            return "Rotating Spiked Log (Deku Tree)";
        case ACTOR_BOSS_GANONDROF:
            return "Phantom Ganon";
        case ACTOR_UNSET_53:
            return "Empty";
        case ACTOR_EN_AM:
            return "Armos Statue";
        case ACTOR_EN_DEKUBABA:
            return "Deku Baba";
        case ACTOR_EN_M_FIRE1:
            return "Thrown Deku Nut";
        case ACTOR_EN_M_THUNDER:
            return "Spin Attack";
        case ACTOR_BG_DDAN_JD:
            return "Rising Stone Platform (Dodongo's Cavern)";
        case ACTOR_BG_BREAKWALL:
            return "Bombable Wall";
        case ACTOR_EN_JJ:
            return "Lord Jabu-Jabu";
        case ACTOR_EN_HORSE_ZELDA:
            return "Zelda's Horse";
        case ACTOR_BG_DDAN_KD:
            return "Stone Stairs (Dodongo's Cavern)";
        case ACTOR_DOOR_WARP1:
            return "Warp Portal";
        case ACTOR_OBJ_SYOKUDAI:
            return "Torch";
        case ACTOR_ITEM_B_HEART:
            return "Heart container";
        case ACTOR_EN_DEKUNUTS:
            return "Mad Scrub";
        case ACTOR_BG_MENKURI_KAITEN:
            return "Rotating Stone Ring Platform";
        case ACTOR_BG_MENKURI_EYE:
            return "Eye Statue";
        case ACTOR_EN_VALI:
            return "Bari";
        case ACTOR_BG_MIZU_MOVEBG:
            return "Water Temple Background Actors";
        case ACTOR_BG_MIZU_WATER:
            return "Water Plane (Water Temple)";
        case ACTOR_ARMS_HOOK:
            return "Hookshot";
        case ACTOR_EN_FHG:
            return "Phantom Ganon's Horse";
        case ACTOR_BG_MORI_HINERI:
            return "Twisted Hallway Rooms";
        case ACTOR_EN_BB:
            return "Bubble";
        case ACTOR_BG_TOKI_HIKARI:
            return "Windows (Temple of Time)";
        case ACTOR_EN_YUKABYUN:
            return "Flying Floor Tile";
        case ACTOR_BG_TOKI_SWD:
            return "Master Sword";
        case ACTOR_EN_FHG_FIRE:
            return "Empty";
        case ACTOR_BG_MJIN:
            return "Warp Song Pad";
        case ACTOR_BG_HIDAN_KOUSI:
            return "Sliding Metal Gate";
        case ACTOR_DOOR_TOKI:
            return "Door of Time Collision";
        case ACTOR_BG_HIDAN_HAMSTEP:
            return "Stone Steps and Platforms (Fire Temple)";
        case ACTOR_EN_BIRD:
            return "Brown Bird";
        case ACTOR_UNSET_73:
            return "Empty";
        case ACTOR_UNSET_74:
            return "Empty";
        case ACTOR_UNSET_75:
            return "Empty";
        case ACTOR_UNSET_76:
            return "Empty";
        case ACTOR_EN_WOOD02:
            return "Trees and Bushes";
        case ACTOR_UNSET_78:
            return "Empty";
        case ACTOR_UNSET_79:
            return "Empty";
        case ACTOR_UNSET_7A:
            return "Empty";
        case ACTOR_UNSET_7B:
            return "Empty";
        case ACTOR_EN_LIGHTBOX:
            return "Large noisy stone";
        case ACTOR_EN_PU_BOX:
            return "Stone cube";
        case ACTOR_UNSET_7E:
            return "Empty";
        case ACTOR_UNSET_7F:
            return "Empty";
        case ACTOR_EN_TRAP:
            return "Metal Spike Trap";
        case ACTOR_EN_AROW_TRAP:
            return "Arrow Trap";
        case ACTOR_EN_VASE:
            return "Orange Pot";
        case ACTOR_UNSET_83:
            return "Empty";
        case ACTOR_EN_TA:
            return "Talon";
        case ACTOR_EN_TK:
            return "Dampe (Alive)";
        case ACTOR_BG_MORI_BIGST:
            return "Large Round Platform (Forest Temple)";
        case ACTOR_BG_MORI_ELEVATOR:
            return "Elevator (Forest Temple)";
        case ACTOR_BG_MORI_KAITENKABE:
            return "Rotatable Walls (Forest Temple)";
        case ACTOR_BG_MORI_RAKKATENJO:
            return "Falling Ceiling (Forest Temple)";
        case ACTOR_EN_VM:
            return "Beamos";
        case ACTOR_DEMO_EFFECT:
            return "Spiritual Stones, Medallions, Triforce";
        case ACTOR_DEMO_KANKYO:
            return "Temple of Time Objects";
        case ACTOR_BG_HIDAN_FWBIG:
            return "Large Fire Wall (Fire Temple)";
        case ACTOR_EN_FLOORMAS:
            return "Floormaster";
        case ACTOR_EN_HEISHI1:
            return "Castle Courtyard Guards";
        case ACTOR_EN_RD:
            return "Redead and Gibdo";
        case ACTOR_EN_PO_SISTERS:
            return "Poe Sisters";
        case ACTOR_BG_HEAVY_BLOCK:
            return "Golden Gauntlets Pillar";
        case ACTOR_BG_PO_EVENT:
            return "Poe Sisters' Paintings and Puzzle Block";
        case ACTOR_OBJ_MURE:
            return "Fish, Bugs, Butterflies Group Spawner";
        case ACTOR_EN_SW:
            return "Skullwalltula and Gold Skulltulas";
        case ACTOR_BOSS_FD:
            return "Volvagia (Flying)";
        case ACTOR_OBJECT_KANKYO:
            return "Environmental Effects";
        case ACTOR_EN_DU:
            return "Darunia";
        case ACTOR_EN_FD:
            return "Flare Dancer";
        case ACTOR_EN_HORSE_LINK_CHILD:
            return "Young Epona";
        case ACTOR_DOOR_ANA:
            return "Grotto Entrance";
        case ACTOR_BG_SPOT02_OBJECTS:
            return "Graveyard Actors";
        case ACTOR_BG_HAKA:
            return "Gravestone";
        case ACTOR_MAGIC_WIND:
            return "Farore's Wind";
        case ACTOR_MAGIC_FIRE:
            return "Din's Fire";
        case ACTOR_UNSET_A0:
            return "Empty";
        case ACTOR_EN_RU1:
            return "Ruto (Child)";
        case ACTOR_BOSS_FD2:
            return "Volvagia (Hole Form)";
        case ACTOR_EN_FD_FIRE:
            return "Flare Dancer Fire Attack";
        case ACTOR_EN_DH:
            return "Dead Hand";
        case ACTOR_EN_DHA:
            return "Dead Hand's Hand";
        case ACTOR_EN_RL:
            return "Rauru";
        case ACTOR_EN_ENCOUNT1:
            return "Enemy Spawner";
        case ACTOR_DEMO_DU:
            return "Darunia (Cutscenes)";
        case ACTOR_DEMO_IM:
            return "Impa (Cutscenes)";
        case ACTOR_DEMO_TRE_LGT:
            return "Treasure Chest Light";
        case ACTOR_EN_FW:
            return "Flare Dancer Core";
        case ACTOR_BG_VB_SIMA:
            return "Volvagia Platform";
        case ACTOR_EN_VB_BALL:
            return "Volvagia Rocks and Bones";
        case ACTOR_BG_HAKA_MEGANE:
            return "Shadow Temple Fake Walls";
        case ACTOR_BG_HAKA_MEGANEBG:
            return "Platforms (Shadow Temple)";
        case ACTOR_BG_HAKA_SHIP:
            return "River Boat (Shadow Temple)";
        case ACTOR_BG_HAKA_SGAMI:
            return "Spinning Scythe Trap";
        case ACTOR_UNSET_B2:
            return "Empty";
        case ACTOR_EN_HEISHI2:
            return "Hyrulian Guards";
        case ACTOR_EN_ENCOUNT2:
            return "Falling Rock Spawner";
        case ACTOR_EN_FIRE_ROCK:
            return "Falling Burning Rocks";
        case ACTOR_EN_BROB:
            return "Flobbery Muscle Block (Jabu-Jabu's Belly)";
        case ACTOR_MIR_RAY:
            return "Reflectable Light Beam";
        case ACTOR_BG_SPOT09_OBJ:
            return "Gerudo Valley Objects";
        case ACTOR_BG_SPOT18_OBJ:
            return "Statue (Darunia's Room)";
        case ACTOR_BOSS_VA:
            return "Barinade";
        case ACTOR_BG_HAKA_TUBO:
            return "Giant Skull Jar (Shadow Temple)";
        case ACTOR_BG_HAKA_TRAP:
            return "Shadow Temple Traps";
        case ACTOR_BG_HAKA_HUTA:
            return "Coffin Lid";
        case ACTOR_BG_HAKA_ZOU:
            return "Bird Statue and Wall (Shadow Temple)";
        case ACTOR_BG_SPOT17_FUNEN:
            return "Smoke Cone (Death Mountain Crater)";
        case ACTOR_EN_SYATEKI_ITM:
            return "Shooting Gallery Game";
        case ACTOR_EN_SYATEKI_MAN:
            return "Shooting Gallery Man";
        case ACTOR_EN_TANA:
            return "Shop Shelves";
        case ACTOR_EN_NB:
            return "Nabooru";
        case ACTOR_BOSS_MO:
            return "Morpha";
        case ACTOR_EN_SB:
            return "Shell Blade";
        case ACTOR_EN_BIGOKUTA:
            return "Big Octo";
        case ACTOR_EN_KAREBABA:
            return "Withered Deku Baba";
        case ACTOR_BG_BDAN_OBJECTS:
            return "Inside Jabu-Jabu's Belly Objects";
        case ACTOR_DEMO_SA:
            return "Saria (Cutscenes)";
        case ACTOR_DEMO_GO:
            return "Gorons (Cutscene)";
        case ACTOR_EN_IN:
            return "Ingo";
        case ACTOR_EN_TR:
            return "Koume and Kotake";
        case ACTOR_BG_SPOT16_BOMBSTONE:
            return "Dodongo's Cavern Entrance Blocking Rock";
        case ACTOR_UNSET_CE:
            return "Empty";
        case ACTOR_BG_HIDAN_KOWARERUKABE:
            return "Bombable Walls (Fire Temple)";
        case ACTOR_BG_BOMBWALL:
            return "2D Bombable Wall";
        case ACTOR_BG_SPOT08_ICEBLOCK:
            return "Ice Platform (Zora's Fountain)";
        case ACTOR_EN_RU2:
            return "Ruto (Adult)";
        case ACTOR_OBJ_DEKUJR:
            return "Deku Tree Sprout";
        case ACTOR_BG_MIZU_UZU:
            return "Water Vortex Effect (Water Temple)";
        case ACTOR_BG_SPOT06_OBJECTS:
            return "Lake Hylia Objects";
        case ACTOR_BG_ICE_OBJECTS:
            return "Pushable Ice Block (Ice Cavern)";
        case ACTOR_BG_HAKA_WATER:
            return "Bottom of the Well Water Level Changer";
        case ACTOR_UNSET_D8:
            return "Empty";
        case ACTOR_EN_MA2:
            return "Malon (Adult, Ingo's Ranch)";
        case ACTOR_EN_BOM_CHU:
            return "Bombchu";
        case ACTOR_EN_HORSE_GAME_CHECK:
            return "Horseback Minigames";
        case ACTOR_BOSS_TW:
            return "Twinrova";
        case ACTOR_EN_RR:
            return "Like-Like";
        case ACTOR_EN_BA:
            return "Tentacle (Inside Jabu-Jabu's Belly)";
        case ACTOR_EN_BX:
            return "Electrified Tentacle (Inside Jabu-Jabu's Belly)";
        case ACTOR_EN_ANUBICE:
            return "Anubis (Body)";
        case ACTOR_EN_ANUBICE_FIRE:
            return "Anubis Fire Attack";
        case ACTOR_BG_MORI_HASHIGO:
            return "Ladder (Forest Temple)";
        case ACTOR_BG_MORI_HASHIRA4:
            return "Forest Temple Objects";
        case ACTOR_BG_MORI_IDOMIZU:
            return "Well Water (Forest Temple)";
        case ACTOR_BG_SPOT16_DOUGHNUT:
            return "Cloud Ring (Death Mountain)";
        case ACTOR_BG_BDAN_SWITCH:
            return "Switches (Inside Lord Jabu-Jabu)";
        case ACTOR_EN_MA1:
            return "Malon (Child)";
        case ACTOR_BOSS_GANON:
            return "Ganondorf";
        case ACTOR_BOSS_SST:
            return "Bongo Bongo";
        case ACTOR_UNSET_EA:
            return "Empty";
        case ACTOR_UNSET_EB:
            return "Empty";
        case ACTOR_EN_NY:
            return "Spike (Enemy)";
        case ACTOR_EN_FR:
            return "Frog Song Spot and Frogs";
        case ACTOR_ITEM_SHIELD:
            return "Deku Shield";
        case ACTOR_BG_ICE_SHELTER:
            return "Red Ice";
        case ACTOR_EN_ICE_HONO:
            return "Blue Fire Flames";
        case ACTOR_ITEM_OCARINA:
            return "Ocarina of Time";
        case ACTOR_UNSET_F2:
            return "Empty";
        case ACTOR_UNSET_F3:
            return "Empty";
        case ACTOR_MAGIC_DARK:
            return "Nayru's Love";
        case ACTOR_DEMO_6K:
            return "Sages, Balls of Light (Cutscene)";
        case ACTOR_EN_ANUBICE_TAG:
            return "Anubis Spawn";
        case ACTOR_BG_HAKA_GATE:
            return "Truth Spinner Puzzle (Shadow Temple)";
        case ACTOR_BG_SPOT15_SAKU:
            return "Hyrule Castle Gate";
        case ACTOR_BG_JYA_GOROIWA:
            return "Rolling Boulder (Spirit Temple)";
        case ACTOR_BG_JYA_ZURERUKABE:
            return "Sliding, Climbable Brick Wall (Spirit Temple)";
        case ACTOR_UNSET_FB:
            return "Empty";
        case ACTOR_BG_JYA_COBRA:
            return "Rotatable Cobra Mirror (Spirit Temple)";
        case ACTOR_BG_JYA_KANAAMI:
            return "Climbable Metal Grating Bridge (Spirit Temple)";
        case ACTOR_FISHING:
            return "Fishing Pond Man and Fish";
        case ACTOR_OBJ_OSHIHIKI:
            return "Pushable Block";
        case ACTOR_BG_GATE_SHUTTER:
            return "Gate to Death Mountain Trail";
        case ACTOR_EFF_DUST:
            return "Dust Effects";
        case ACTOR_BG_SPOT01_FUSYA:
            return "Windmill Sails (Kakariko Village)";
        case ACTOR_BG_SPOT01_IDOHASHIRA:
            return "Well Crossbeam (Kakariko Village)";
        case ACTOR_BG_SPOT01_IDOMIZU:
            return "Well Water (Kakariko Village)";
        case ACTOR_BG_PO_SYOKUDAI:
            return "Golden Torch (Poe Sisters)";
        case ACTOR_BG_GANON_OTYUKA:
            return "Falling Platform (Ganondorf Fight)";
        case ACTOR_BG_SPOT15_RRBOX:
            return "Milk Crate";
        case ACTOR_BG_UMAJUMP:
            return "Obstacle Fence (Lon Lon Ranch)";
        case ACTOR_UNSET_109:
            return "Empty";
        case ACTOR_ARROW_FIRE:
            return "Fire Arrow";
        case ACTOR_ARROW_ICE:
            return "Ice Arrow";
        case ACTOR_ARROW_LIGHT:
            return "Light Arrow";
        case ACTOR_UNSET_10D:
            return "Empty";
        case ACTOR_UNSET_10E:
            return "Empty";
        case ACTOR_ITEM_ETCETERA:
            return "Collectible Items";
        case ACTOR_OBJ_KIBAKO:
            return "Small Liftable Crate";
        case ACTOR_OBJ_TSUBO:
            return "Breakable Pot";
        case ACTOR_EN_WONDER_ITEM:
            return "Invisible Collectible";
        case ACTOR_EN_IK:
            return "Iron Knuckle";
        case ACTOR_DEMO_IK:
            return "Iron Knuckle armor pieces (Spirit Temple Nabooru fight)";
        case ACTOR_EN_SKJ:
            return "Skullkid";
        case ACTOR_EN_SKJNEEDLE:
            return "Skullkid Needle Attack";
        case ACTOR_EN_G_SWITCH:
            return "Silver Rupee";
        case ACTOR_DEMO_EXT:
            return "Magic Vortex";
        case ACTOR_DEMO_SHD:
            return "Bongo Bongo's Shadow";
        case ACTOR_EN_DNS:
            return "Business Scrub";
        case ACTOR_ELF_MSG:
            return "Navi Message";
        case ACTOR_EN_HONOTRAP:
            return "Stone Eye (Fire Trap) and Flame";
        case ACTOR_EN_TUBO_TRAP:
            return "Flying Pot";
        case ACTOR_OBJ_ICE_POLY:
            return "Ice spawned by red ice? frozen actors";
        case ACTOR_BG_SPOT03_TAKI:
            return "Zora's River Waterfall";
        case ACTOR_BG_SPOT07_TAKI:
            return "Zora's Domain Waterfall and Ice";
        case ACTOR_EN_FZ:
            return "Frezzard";
        case ACTOR_EN_PO_RELAY:
            return "Dampe's Ghost";
        case ACTOR_BG_RELAY_OBJECTS:
            return "Windmill Objects";
        case ACTOR_EN_DIVING_GAME:
            return "Zora Diving Game";
        case ACTOR_EN_KUSA:
            return "Bush/Grass";
        case ACTOR_OBJ_BEAN:
            return "Bean Plant Spot";
        case ACTOR_OBJ_BOMBIWA:
            return "Brown Bombable Boulder";
        case ACTOR_UNSET_128:
            return "Empty";
        case ACTOR_UNSET_129:
            return "Empty";
        case ACTOR_OBJ_SWITCH:
            return "Switches";
        case ACTOR_OBJ_ELEVATOR:
            return "Huge Stone Elevator";
        case ACTOR_OBJ_LIFT:
            return "Square Collapsing Platform";
        case ACTOR_OBJ_HSBLOCK:
            return "Stone Hookshot Target";
        case ACTOR_EN_OKARINA_TAG:
            return "Ocarina Music Staff Spot";
        case ACTOR_EN_YABUSAME_MARK:
            return "Horseback Archery Target";
        case ACTOR_EN_GOROIWA:
            return "Rolling Boulder";
        case ACTOR_EN_EX_RUPPY:
            return "Sparkling Rupee";
        case ACTOR_EN_TORYO:
            return "Boss Carpenter";
        case ACTOR_EN_DAIKU:
            return "Carpenters";
        case ACTOR_UNSET_134:
            return "Empty";
        case ACTOR_EN_NWC:
            return "Cucco Chick";
        case ACTOR_EN_BLKOBJ:
            return "Dark Link's Illusion Room";
        case ACTOR_ITEM_INBOX:
            return "Zelda's Magic to Open Gates";
        case ACTOR_EN_GE1:
            return "White Clothed Gerudo";
        case ACTOR_OBJ_BLOCKSTOP:
            return "Pushblock Stop";
        case ACTOR_EN_SDA:
            return "Dynamic Shadow";
        case ACTOR_EN_CLEAR_TAG:
            return "Arwing";
        case ACTOR_EN_NIW_LADY:
            return "Cucco Lady";
        case ACTOR_EN_GM:
            return "Medigoron";
        case ACTOR_EN_MS:
            return "Bean Salesman";
        case ACTOR_EN_HS:
            return "Carpenter's Son";
        case ACTOR_BG_INGATE:
            return "Ingo's Gates (Lon Lon Ranch)";
        case ACTOR_EN_KANBAN:
            return "Square Signpost";
        case ACTOR_EN_HEISHI3:
            return "Hyrule Castle Guard";
        case ACTOR_EN_SYATEKI_NIW:
            return "Cucco (Minigames)";
        case ACTOR_EN_ATTACK_NIW:
            return "Cucco (Attacking)";
        case ACTOR_BG_SPOT01_IDOSOKO:
            return "Stone Blocking Entrance to Bottom of the Well";
        case ACTOR_EN_SA:
            return "Saria";
        case ACTOR_EN_WONDER_TALK:
            return "Checkable Spot (Green Navi)";
        case ACTOR_BG_GJYO_BRIDGE:
            return "Rainbow Bridge to Ganon's Castle";
        case ACTOR_EN_DS:
            return "Potion Shop Granny";
        case ACTOR_EN_MK:
            return "Lakeside Professor";
        case ACTOR_EN_BOM_BOWL_MAN:
            return "Bombchu Bowling Alley Lady";
        case ACTOR_EN_BOM_BOWL_PIT:
            return "Bombchu Bowling Alley Final Target";
        case ACTOR_EN_OWL:
            return "Kaepora Gaebora";
        case ACTOR_EN_ISHI:
            return "Liftable Rock";
        case ACTOR_OBJ_HANA:
            return "Uninteractable Flowers, Rocks, Grass";
        case ACTOR_OBJ_LIGHTSWITCH:
            return "Sun Emblem Switch (Spirit Temple)";
        case ACTOR_OBJ_MURE2:
            return "Rock/Bush groups";
        case ACTOR_EN_GO:
            return "Gorons 1";
        case ACTOR_EN_FU:
            return "Windmill Man";
        case ACTOR_UNSET_154:
            return "Empty";
        case ACTOR_EN_CHANGER:
            return "Treasure Box Shop Minigame";
        case ACTOR_BG_JYA_MEGAMI:
            return "Statue Face (Spirit Temple)";
        case ACTOR_BG_JYA_LIFT:
            return "Chain Platform (Spirit Temple)";
        case ACTOR_BG_JYA_BIGMIRROR:
            return "Large Circular Mirror (Spirt Temple)";
        case ACTOR_BG_JYA_BOMBCHUIWA:
            return "Light Blocking Rock (Spirit Temple)";
        case ACTOR_BG_JYA_AMISHUTTER:
            return "Sliding Circular Metal Grate (Spirit Temple)";
        case ACTOR_BG_JYA_BOMBIWA:
            return "Bombable Rock Wall (Spirit Temple)";
        case ACTOR_BG_SPOT18_BASKET:
            return "Big Goron Pot";
        case ACTOR_UNSET_15D:
            return "Empty";
        case ACTOR_EN_GANON_ORGAN:
            return "Ganon's Organ and surroundings";
        case ACTOR_EN_SIOFUKI:
            return "Water Spout";
        case ACTOR_EN_STREAM:
            return "Water Vortex";
        case ACTOR_UNSET_161:
            return "Empty";
        case ACTOR_EN_MM:
            return "Running Man (Child Era)";
        case ACTOR_EN_KO:
            return "Kokiri Children";
        case ACTOR_EN_KZ:
            return "King Zora";
        case ACTOR_EN_WEATHER_TAG:
            return "Proximity Weather Effects";
        case ACTOR_BG_SST_FLOOR:
            return "Bongo Bongo's Drum";
        case ACTOR_EN_ANI:
            return "Kakariko Village Rooftop Man";
        case ACTOR_EN_EX_ITEM:
            return "Minigame Displayed Items";
        case ACTOR_BG_JYA_IRONOBJ:
            return "Iron Knuckles Room Stuff (Spirit Temple)";
        case ACTOR_EN_JS:
            return "Magic Carpet Man";
        case ACTOR_EN_JSJUTAN:
            return "Magic Carpet Man Carpet";
        case ACTOR_EN_CS:
            return "Graveyard Boy";
        case ACTOR_EN_MD:
            return "Mido";
        case ACTOR_EN_HY:
            return "Market NPCs";
        case ACTOR_EN_GANON_MANT:
            return "Ganondorf's Cape";
        case ACTOR_EN_OKARINA_EFFECT:
            return "Song of Storms Storm Manager";
        case ACTOR_EN_MAG:
            return "Title Screen Manager";
        case ACTOR_DOOR_GERUDO:
            return "Cell Door (Gerudo Fortress)";
        case ACTOR_ELF_MSG2:
            return "Navi Information Spot (Targetable, Green)";
        case ACTOR_DEMO_GT:
            return "Ganon's Tower Collapsing (Cutscene Objects)";
        case ACTOR_EN_PO_FIELD:
            return "Big/Small Poe Spawn Point";
        case ACTOR_EFC_ERUPC:
            return "Lava Particle Fountain (Death Mountain panorama)";
        case ACTOR_BG_ZG:
            return "Metal Bars (Ganon's Castle)";
        case ACTOR_EN_HEISHI4:
            return "Hyrule Guard";
        case ACTOR_EN_ZL3:
            return "Adult Zelda";
        case ACTOR_BOSS_GANON2:
            return "Ganon";
        case ACTOR_EN_KAKASI:
            return "Pierre the Scarecrow";
        case ACTOR_EN_TAKARA_MAN:
            return "Treasure Box Shop Man";
        case ACTOR_OBJ_MAKEOSHIHIKI:
            return "Push Block Puzzles";
        case ACTOR_OCEFF_SPOT:
            return "Sun's Song Effect";
        case ACTOR_END_TITLE:
            return "The End message";
        case ACTOR_UNSET_180:
            return "Empty";
        case ACTOR_EN_TORCH:
            return "Grotto Treasure Chest";
        case ACTOR_DEMO_EC:
            return "Credits Revelers in Lon Lon";
        case ACTOR_SHOT_SUN:
            return "Lake Hylia Sun Hitbox, Big Fairy Spawner";
        case ACTOR_EN_DY_EXTRA:
            return "Spiral Beams (Great Fairy Fountains)";
        case ACTOR_EN_WONDER_TALK2:
            return "Dialog Spot";
        case ACTOR_EN_GE2:
            return "Patrolling Gerudo";
        case ACTOR_OBJ_ROOMTIMER:
            return "Room Timer";
        case ACTOR_EN_SSH:
            return "Cursed Skulltula People";
        case ACTOR_EN_STH:
            return "Uncursed Skulltula People";
        case ACTOR_OCEFF_WIPE:
            return "Zelda's Lullaby and Song of Time Ocarina Effect";
        case ACTOR_OCEFF_STORM:
            return "Song of Storm Ocarina Effect";
        case ACTOR_EN_WEIYER:
            return "Stinger (Water)";
        case ACTOR_BG_SPOT05_SOKO:
            return "Sacred Forest Meadow Objects";
        case ACTOR_BG_JYA_1FLIFT:
            return "Stone Elevator (Spirit Temple)";
        case ACTOR_BG_JYA_HAHENIRON:
            return "Chunks of Iron Knucle Chair and Pillar";
        case ACTOR_BG_SPOT12_GATE:
            return "Gerudo Fortress Wooden Gate";
        case ACTOR_BG_SPOT12_SAKU:
            return "Gerudo Fortress Training Area Gate";
        case ACTOR_EN_HINTNUTS:
            return "Hint Deku Scrubs (Deku Tree)";
        case ACTOR_EN_NUTSBALL:
            return "Deku Scrub Nut Attack";
        case ACTOR_BG_SPOT00_BREAK:
            return "Broken Drawbridge, Fences";
        case ACTOR_EN_SHOPNUTS:
            return "Grounded Sales Scrub";
        case ACTOR_EN_IT:
            return "Dampe's Minigame Collectibles";
        case ACTOR_EN_GELDB:
            return "Gerudo Fighter";
        case ACTOR_OCEFF_WIPE2:
            return "Epona's Song Ocarina Effect";
        case ACTOR_OCEFF_WIPE3:
            return "Saria's Song Ocarina Effect";
        case ACTOR_EN_NIW_GIRL:
            return "Girl Chasing Cucco";
        case ACTOR_EN_DOG:
            return "Dog";
        case ACTOR_EN_SI:
            return "Gold Skulltula Token";
        case ACTOR_BG_SPOT01_OBJECTS2:
            return "Kakariko Village Objects";
        case ACTOR_OBJ_COMB:
            return "Beehive";
        case ACTOR_BG_SPOT11_BAKUDANKABE:
            return "Destructible Wall (Desert Colossus)";
        case ACTOR_OBJ_KIBAKO2:
            return "Large Crate";
        case ACTOR_EN_DNT_DEMO:
            return "Deku Mask Panel Trigger";
        case ACTOR_EN_DNT_JIJI:
            return "Deku Mask Panel Head Judge";
        case ACTOR_EN_DNT_NOMAL:
            return "Deku Game?";
        case ACTOR_EN_GUEST:
            return "Happy Mask Shop Customer";
        case ACTOR_BG_BOM_GUARD:
            return "Bombchu Bowling Alley Aiming Area";
        case ACTOR_EN_HS2:
            return "Carpenter's Son (Child Era)";
        case ACTOR_DEMO_KEKKAI:
            return "Ganon's Tower Magic Barriers";
        case ACTOR_BG_SPOT08_BAKUDANKABE:
            return "Destructible Wall (Zora's Fountain)";
        case ACTOR_BG_SPOT17_BAKUDANKABE:
            return "Destructible Wall (Death Mountain Crater)";
        case ACTOR_UNSET_1AA:
            return "Empty";
        case ACTOR_OBJ_MURE3:
            return "Rupee Patterns";
        case ACTOR_EN_TG:
            return "Entwined Lovers (Honey & Darling)";
        case ACTOR_EN_MU:
            return "Haggling Townspeople";
        case ACTOR_EN_GO2:
            return "Gorons 2";
        case ACTOR_EN_WF:
            return "Wolfos";
        case ACTOR_EN_SKB:
            return "Stalchild";
        case ACTOR_DEMO_GJ:
            return "Ganon Battle Rubble";
        case ACTOR_DEMO_GEFF:
            return "Ganon's Tower Rubble Fragment";
        case ACTOR_BG_GND_FIREMEIRO:
            return "Sinking Lava Platform (Ganon's Castle)";
        case ACTOR_BG_GND_DARKMEIRO:
            return "Clear block";
        case ACTOR_BG_GND_SOULMEIRO:
            return "Web-Blocked Ceiling Hole (Inside Ganon's Castle)";
        case ACTOR_BG_GND_NISEKABE:
            return "Ganon's Castle Fake Walls";
        case ACTOR_BG_GND_ICEBLOCK:
            return "Pushable Square Ice Block (Inside Ganon's Castle)";
        case ACTOR_EN_GB:
            return "Poe Collector and Surroundings";
        case ACTOR_EN_GS:
            return "Gossip Stone";
        case ACTOR_BG_MIZU_BWALL:
            return "Bombable Stone Wall";
        case ACTOR_BG_MIZU_SHUTTER:
            return "Metal Gate (Water Temple)";
        case ACTOR_EN_DAIKU_KAKARIKO:
            return "Carpenters (Kakariko)";
        case ACTOR_BG_BOWL_WALL:
            return "Bombchu Bowling Alley Wall";
        case ACTOR_EN_WALL_TUBO:
            return "Bombchu Bowling Alley Bullseyes";
        case ACTOR_EN_PO_DESERT:
            return "Poe Guide (Desert Wasteland)";
        case ACTOR_EN_CROW:
            return "Guay";
        case ACTOR_DOOR_KILLER:
            return "Fake Door";
        case ACTOR_BG_SPOT11_OASIS:
            return "Oasis (Desert Colossus)";
        case ACTOR_BG_SPOT18_FUTA:
            return "Goron Jar Lid";
        case ACTOR_BG_SPOT18_SHUTTER:
            return "Sliding Doors (Goron City)";
        case ACTOR_EN_MA3:
            return "Malon (Adult, Lon Lon Ranch)";
        case ACTOR_EN_COW:
            return "Cow";
        case ACTOR_BG_ICE_TURARA:
            return "Icicles";
        case ACTOR_BG_ICE_SHUTTER:
            return "Vertical Ice Bars (Ice Cavern)";
        case ACTOR_EN_KAKASI2:
            return "Pierre the Scarecrow Spawn";
        case ACTOR_EN_KAKASI3:
            return "Bonooru the Scarecrow";
        case ACTOR_OCEFF_WIPE4:
            return "Scarecrow's Song Ocarina Effect";
        case ACTOR_EN_EG:
            return "Void-out Trigger (Tower Collapse)";
        case ACTOR_BG_MENKURI_NISEKABE:
            return "False Stone Walls (Gerudo Training Grounds)";
        case ACTOR_EN_ZO:
            return "Zora";
        case ACTOR_OBJ_MAKEKINSUTA:
            return "Skulltula Sprouting from Bean Spot";
        case ACTOR_EN_GE3:
            return "Gerudo Fortress Leader";
        case ACTOR_OBJ_TIMEBLOCK:
            return "Time Block";
        case ACTOR_OBJ_HAMISHI:
            return "Bronze Boulder";
        case ACTOR_EN_ZL4:
            return "Zelda (Child)";
        case ACTOR_EN_MM2:
            return "Running Man (Adult Era)";
        case ACTOR_BG_JYA_BLOCK:
            return "Silver Block (Child Era)";
        case ACTOR_OBJ_WARP2BLOCK:
            return "Navi Infospot (Green, Time Block)";
        default:
            return "";
    }

    return "";
}

template <typename T> void DrawGroupWithBorder(T&& drawFunc) {
    // First group encapsulates the inner portion and border
    ImGui::BeginGroup();

    ImVec2 padding = ImGui::GetStyle().FramePadding;
    ImVec2 p0 = ImGui::GetCursorScreenPos();
    ImGui::SetCursorScreenPos(ImVec2(p0.x + padding.x, p0.y + padding.y));

    // Second group encapsulates just the inner portion
    ImGui::BeginGroup();

    drawFunc();

    ImGui::Dummy(padding);
    ImGui::EndGroup();

    ImVec2 p1 = ImGui::GetItemRectMax();
    p1.x += padding.x;
    ImVec4 borderCol = ImGui::GetStyle().Colors[ImGuiCol_Border];
    ImGui::GetWindowDrawList()->AddRect(
        p0, p1, IM_COL32(borderCol.x * 255, borderCol.y * 255, borderCol.z * 255, borderCol.w * 255));

    ImGui::EndGroup();
}

std::vector<Actor*> PopulateActorDropdown(int i, std::vector<Actor*> data) {
    if (gGlobalCtx == nullptr) {
        return data;
    }
    if (data.size() != 0) {
        data.clear();
    }
    ActorListEntry currList = gGlobalCtx->actorCtx.actorLists[i];
    Actor* currAct = currList.head;
    if (currAct != nullptr) {
        while (currAct != nullptr) {
            data.push_back(currAct);
            currAct = currAct->next;
        }
    }
    return data;
}

void DrawActorViewer(bool& open) {
    if (!open) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Actor Viewer", &open, ImGuiWindowFlags_NoFocusOnAppearing)) {
        ImGui::End();
        return;
    }

    static int category = 0;
    static ImU16 one = 1;
    static int actor;
    static RetrievalMethod rm;
    static std::vector<Actor*> list;
    static Actor display;
    static Actor* fetch = NULL;
    static ActorInfo newActor = {0,0, {0, 0, 0}, {0, 0, 0}};
    static ActorOverlay* dispOverlay;
    static std::string filler = "Please select";

    if (ImGui::BeginCombo("Actor Type", acMapping[category].c_str())) {
        for (int i = 0; i < acMapping.size(); i++) {
            if (ImGui::Selectable(acMapping[i].c_str())) {
                category = i;
                list = PopulateActorDropdown(category, list);
                break;
            }
        }
        ImGui::EndCombo();
    }

    if (ImGui::BeginCombo("Actor", filler.c_str())) {
        for (int i = 0; i < list.size(); i++) { 
            std::string label = std::to_string(i) + ": " + list[i]->overlayEntry->name;
            std::string description = GetActorDescription(list[i]->id);
            if (description != "")
                label += " (" + description + ")";

            if (ImGui::Selectable(label.c_str())) {
                rm = LIST;
                display = *list[i];
                actor = i;
                filler = label;
                break;
            }
        }
        ImGui::EndCombo();
    }

    if (ImGui::TreeNode("Selected Actor")) {
        dispOverlay = display.overlayEntry;
        DrawGroupWithBorder([&]() {          
            ImGui::Text("Name: %s", dispOverlay != nullptr ? dispOverlay->name : "???");
            ImGui::Text("Description: %s", GetActorDescription(display.id).c_str());
            ImGui::Text("Category: %s", dispOverlay != nullptr ? acMapping[display.category].c_str() : "???");
            ImGui::Text("ID: %d", display.id);
            ImGui::Text("Parameters: %d", display.params);
        });
        
        ImGui::PushItemWidth(ImGui::GetFontSize() * 6);

        DrawGroupWithBorder([&]() {
            ImGui::Text("Actor Position");
            ImGui::InputScalar("x pos", ImGuiDataType_Float, &display.world.pos.x);
            ImGui::SameLine();
            ImGui::InputScalar("y pos", ImGuiDataType_Float, &display.world.pos.y);
            ImGui::SameLine();
            ImGui::InputScalar("z pos", ImGuiDataType_Float, &display.world.pos.z);
        });        

        DrawGroupWithBorder([&]() {
            ImGui::Text("Actor Rotation");
            ImGui::InputScalar("x rot", ImGuiDataType_S16, &display.world.rot.x);
            ImGui::SameLine();
            ImGui::InputScalar("y rot", ImGuiDataType_S16, &display.world.rot.y);
            ImGui::SameLine();
            ImGui::InputScalar("z rot", ImGuiDataType_S16, &display.world.rot.z);
        });        

        if (display.category == ACTORCAT_BOSS || display.category == ACTORCAT_ENEMY) {
            ImGui::InputScalar("Enemy Health", ImGuiDataType_U8, &display.colChkInfo.health);
            InsertHelpHoverText("Some actors might not use this!");
        }

        if (ImGui::Button("Kill")) {
            Actor_Delete(&gGlobalCtx->actorCtx, &display, gGlobalCtx);
            PopulateActorDropdown(category, list);
            filler = "Please select";
        }

        if (ImGui::Button("Refresh")) {
            PopulateActorDropdown(category, list);
            switch (rm) { 
                case INTERACT:
                case HELD:
                case TARGET: 
                    display = *fetch;
                    break;
                case LIST:
                    display = *list[actor];
                    break;
                default:
                    break;
            }
            //display = *list[actor];
        }

        if (ImGui::Button("Go to Actor")) {
            Player* player = GET_PLAYER(gGlobalCtx);
            Math_Vec3f_Copy(&player->actor.world.pos, &display.world.pos);
            Math_Vec3f_Copy(&player->actor.home.pos, &player->actor.world.pos);
        }

        if (ImGui::Button("Fetch from Target")) {
            Player* player = GET_PLAYER(gGlobalCtx);
            fetch = player->targetActor;
            if (fetch != NULL) {
                display = *fetch;
                category = fetch->category;
                PopulateActorDropdown(category, list);
                rm = TARGET;
            }
        }
        InsertHelpHoverText("Grabs actor with target arrow above it. You might need C-Up for enemies");
        if (ImGui::Button("Fetch from Held")) {
            Player* player = GET_PLAYER(gGlobalCtx);
            fetch = player->heldActor;
            if (fetch != NULL) {
                display = *fetch;
                category = fetch->category;
                PopulateActorDropdown(category, list);
                rm = HELD;
            }
        }
        InsertHelpHoverText("Grabs actor that Link is holding");
        if (ImGui::Button("Fetch from Interaction")) {
            Player* player = GET_PLAYER(gGlobalCtx);
            fetch = player->interactRangeActor;
            if (fetch != NULL) {
                display = *fetch;
                category = fetch->category;
                PopulateActorDropdown(category, list);
                rm = INTERACT;
            }
        }
        InsertHelpHoverText("Grabs actor from \"interaction range\"");

        ImGui::TreePop();
    }
    
    if (ImGui::TreeNode("New...")) {
        ImGui::PushItemWidth(ImGui::GetFontSize() * 10);

        ImGui::InputScalar("ID", ImGuiDataType_S16, &newActor.id, &one);
        ImGui::InputScalar("params", ImGuiDataType_S16, &newActor.params, &one);

        ImGui::PushItemWidth(ImGui::GetFontSize() * 6);

        DrawGroupWithBorder([&]() {
            ImGui::Text("New Actor Position");
            ImGui::InputScalar("posX", ImGuiDataType_Float, &newActor.pos.x);
            ImGui::SameLine();
            ImGui::InputScalar("posY", ImGuiDataType_Float, &newActor.pos.y);
            ImGui::SameLine();
            ImGui::InputScalar("posZ", ImGuiDataType_Float, &newActor.pos.z);
        });

        DrawGroupWithBorder([&]() {
            ImGui::Text("New Actor Rotation");
            ImGui::InputScalar("rotX", ImGuiDataType_S16, &newActor.rot.x);
            ImGui::SameLine();
            ImGui::InputScalar("rotY", ImGuiDataType_S16, &newActor.rot.y);
            ImGui::SameLine();
            ImGui::InputScalar("rotZ", ImGuiDataType_S16, &newActor.rot.z);
        });

        if (ImGui::Button("Fetch from Link")) {
            Player* player = GET_PLAYER(gGlobalCtx);
            Vec3f newPos = player->actor.world.pos;
            Vec3s newRot = player->actor.world.rot;
            newActor.pos = newPos;
            newActor.rot = newRot;
        }

        if (ImGui::Button("Spawn")) {
            Actor_Spawn(&gGlobalCtx->actorCtx, gGlobalCtx, newActor.id, newActor.pos.x, newActor.pos.y, newActor.pos.z,
                        newActor.rot.x, newActor.rot.y, newActor.rot.z, newActor.params);
        }

        if (ImGui::Button("Spawn as Child")) {
            Actor* parent = gGlobalCtx->actorCtx.actorLists[category].head;
            if (parent != NULL) {
                Actor_SpawnAsChild(&gGlobalCtx->actorCtx, parent, gGlobalCtx, newActor.id, newActor.pos.x,
                                   newActor.pos.y, newActor.pos.z, newActor.rot.x, newActor.rot.y, newActor.rot.z,
                                   newActor.params);
            }
            
        }

        ImGui::TreePop();
    }

    ImGui::End();
}

void InitActorViewer() {
    SohImGui::AddWindow("Developer Tools", "Actor Viewer", DrawActorViewer);
}