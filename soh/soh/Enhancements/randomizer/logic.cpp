#include "logic.h"
#include "../debugger/performanceTimer.h"

#include <algorithm>
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>

#include "soh/OTRGlobals.h"
#include "dungeon.h"
#include "context.h"
#include "macros.h"
#include "variables.h"
#include <spdlog/spdlog.h>

namespace Rando {

    bool Logic::HasItem(RandomizerGet itemName) {
        switch (itemName) {
            case RG_FAIRY_OCARINA:
                return CheckInventory(ITEM_OCARINA_FAIRY, false);
            case RG_OCARINA_OF_TIME:
                return CheckInventory(ITEM_OCARINA_TIME, true);
            case RG_DINS_FIRE:
                return CheckInventory(ITEM_DINS_FIRE, true);
            case RG_FARORES_WIND:
                return CheckInventory(ITEM_FARORES_WIND, true);
            case RG_NAYRUS_LOVE:
                return CheckInventory(ITEM_NAYRUS_LOVE, true);
            case RG_LENS_OF_TRUTH:
                return CheckInventory(ITEM_LENS, true);
            case RG_FAIRY_BOW:
                return CheckInventory(ITEM_BOW, true);
            case RG_MEGATON_HAMMER:
                return CheckInventory(ITEM_HAMMER, true);
            case RG_HOOKSHOT:
                return CheckInventory(ITEM_HOOKSHOT, false);
            case RG_LONGSHOT:
                return CheckInventory(ITEM_LONGSHOT, true);
            case RG_PROGRESSIVE_STICK_UPGRADE:
            case RG_STICKS:
                return CurrentUpgrade(UPG_STICKS);
            case RG_FIRE_ARROWS:
                return CheckInventory(ITEM_ARROW_FIRE, true);
            case RG_ICE_ARROWS:
                return CheckInventory(ITEM_ARROW_ICE, true);
            case RG_LIGHT_ARROWS:
                return CheckInventory(ITEM_ARROW_LIGHT, true);
            case RG_PROGRESSIVE_BOMBCHUS:
            case RG_BOMBCHU_5:
            case RG_BOMBCHU_10:
            case RG_BOMBCHU_20:
                return (BombchusEnabled() && (GetInLogic(LOGIC_BUY_BOMBCHUS) || CouldPlayBowling || CarpetMerchant)) || CheckInventory(ITEM_BOMBCHU, true);
            case RG_FAIRY_SLINGSHOT:
                return CheckInventory(ITEM_SLINGSHOT, true);
            case RG_BOOMERANG:
                return CheckInventory(ITEM_BOOMERANG, true);
            case RG_PROGRESSIVE_NUT_UPGRADE:
            case RG_NUTS:
                return CurrentUpgrade(UPG_NUTS);
            case RG_SCARECROW:
                return ScarecrowsSong() && CanUse(RG_HOOKSHOT);
            case RG_DISTANT_SCARECROW:
                return ScarecrowsSong() && CanUse(RG_LONGSHOT);
            case RG_KOKIRI_SWORD:
            case RG_DEKU_SHIELD:
            case RG_GORON_TUNIC:
            case RG_ZORA_TUNIC:
            case RG_HYLIAN_SHIELD:
            case RG_MIRROR_SHIELD:
            case RG_MASTER_SWORD:
            case RG_BIGGORON_SWORD:
            case RG_IRON_BOOTS:
            case RG_HOVER_BOOTS:
                return CheckEquipment(RandoGetToEquipFlag.at(itemName));
            case RG_GORONS_BRACELET:
                return CurrentUpgrade(UPG_STRENGTH);
            case RG_SILVER_GAUNTLETS:
                return CurrentUpgrade(UPG_STRENGTH) >= 2;
            case RG_GOLDEN_GAUNTLETS:
                return CurrentUpgrade(UPG_STRENGTH) >= 3;
            case RG_PROGRESSIVE_BOMB_BAG:
            case RG_BOMB_BAG:
                return CurrentUpgrade(UPG_BOMB_BAG);
            case RG_MAGIC_SINGLE:
                return GetSaveContext()->magicLevel >= 1;
                // Songs
            case RG_ZELDAS_LULLABY:
            case RG_EPONAS_SONG:
            case RG_SARIAS_SONG:
            case RG_SUNS_SONG:
            case RG_SONG_OF_TIME:
            case RG_SONG_OF_STORMS:
            case RG_MINUET_OF_FOREST:
            case RG_BOLERO_OF_FIRE:
            case RG_SERENADE_OF_WATER:
            case RG_REQUIEM_OF_SPIRIT:
            case RG_NOCTURNE_OF_SHADOW:
            case RG_PRELUDE_OF_LIGHT:
                // Dungeon Rewards
            case RG_KOKIRI_EMERALD:
            case RG_GORON_RUBY:
            case RG_ZORA_SAPPHIRE:
            case RG_FOREST_MEDALLION:
            case RG_FIRE_MEDALLION:
            case RG_WATER_MEDALLION:
            case RG_SPIRIT_MEDALLION:
            case RG_SHADOW_MEDALLION:
            case RG_LIGHT_MEDALLION:
                // Misc Quest Items
            case RG_STONE_OF_AGONY:
            case RG_GERUDO_MEMBERSHIP_CARD:
                return CheckQuestItem(RandoGetToQuestItem.at(itemName));
            case RG_RUTOS_LETTER:
                return CheckEventChkInf(EVENTCHKINF_OBTAINED_RUTOS_LETTER);
            case RG_DOUBLE_DEFENSE:
                return GetSaveContext()->isDoubleDefenseAcquired;
            case RG_FISHING_POLE:
            case RG_ZELDAS_LETTER:
            case RG_WEIRD_EGG:
            case RG_GREG_RUPEE:
                // Ocarina Buttons
            case RG_OCARINA_A_BUTTON:
            case RG_OCARINA_C_LEFT_BUTTON:
            case RG_OCARINA_C_RIGHT_BUTTON:
            case RG_OCARINA_C_DOWN_BUTTON:
            case RG_OCARINA_C_UP_BUTTON:
                // Boss Souls
            case RG_GOHMA_SOUL:
            case RG_KING_DODONGO_SOUL:
            case RG_BARINADE_SOUL:
            case RG_PHANTOM_GANON_SOUL:
            case RG_VOLVAGIA_SOUL:
            case RG_MORPHA_SOUL:
            case RG_BONGO_BONGO_SOUL:
            case RG_TWINROVA_SOUL:
            case RG_GANON_SOUL:
            case RG_SKELETON_KEY:
                return CheckRandoInf(RandoGetToRandInf.at(itemName));
                // Boss Keys
            case RG_EPONA:
                return FreedEpona;
            case RG_FOREST_TEMPLE_BOSS_KEY:
            case RG_FIRE_TEMPLE_BOSS_KEY:
            case RG_WATER_TEMPLE_BOSS_KEY:
            case RG_SPIRIT_TEMPLE_BOSS_KEY:
            case RG_SHADOW_TEMPLE_BOSS_KEY:
            case RG_GANONS_CASTLE_BOSS_KEY:
                return CheckDungeonItem(DUNGEON_KEY_BOSS, RandoGetToDungeonScene.at(itemName));
                // Maps
            case RG_DEKU_TREE_MAP:
            case RG_DODONGOS_CAVERN_MAP:
            case RG_JABU_JABUS_BELLY_MAP:
            case RG_FOREST_TEMPLE_MAP:
            case RG_FIRE_TEMPLE_MAP:
            case RG_WATER_TEMPLE_MAP:
            case RG_SPIRIT_TEMPLE_MAP:
            case RG_SHADOW_TEMPLE_MAP:
            case RG_BOTTOM_OF_THE_WELL_MAP:
            case RG_ICE_CAVERN_MAP:
                return CheckDungeonItem(DUNGEON_MAP, RandoGetToDungeonScene.at(itemName));
                // Compasses
            case RG_DEKU_TREE_COMPASS:
            case RG_DODONGOS_CAVERN_COMPASS:
            case RG_JABU_JABUS_BELLY_COMPASS:
            case RG_FOREST_TEMPLE_COMPASS:
            case RG_FIRE_TEMPLE_COMPASS:
            case RG_WATER_TEMPLE_COMPASS:
            case RG_SPIRIT_TEMPLE_COMPASS:
            case RG_SHADOW_TEMPLE_COMPASS:
            case RG_BOTTOM_OF_THE_WELL_COMPASS:
            case RG_ICE_CAVERN_COMPASS:
                return CheckDungeonItem(DUNGEON_COMPASS, RandoGetToDungeonScene.at(itemName));
                // Wallets
            case RG_CHILD_WALLET:
                return CheckRandoInf(RAND_INF_HAS_WALLET);
            case RG_ADULT_WALLET:
                return CurrentUpgrade(UPG_WALLET) >= 1;
            case RG_GIANT_WALLET:
                return CurrentUpgrade(UPG_WALLET) >= 2;
            case RG_TYCOON_WALLET:
                return CurrentUpgrade(UPG_WALLET) >= 3;
                // Scales
            case RG_BRONZE_SCALE:
                return CheckRandoInf(RAND_INF_CAN_SWIM);
            case RG_SILVER_SCALE:
                return CurrentUpgrade(UPG_SCALE) >= 1;
            case RG_GOLDEN_SCALE:
                return CurrentUpgrade(UPG_SCALE) >= 2;
            case RG_POCKET_EGG:
            case RG_COJIRO:
            case RG_ODD_MUSHROOM:
            case RG_ODD_POTION:
            case RG_POACHERS_SAW:
            case RG_BROKEN_SWORD:
            case RG_PRESCRIPTION:
            case RG_EYEBALL_FROG:
            case RG_EYEDROPS:
            case RG_CLAIM_CHECK:
                return HasAdultTrade(StaticData::RetrieveItem(itemName).GetGIEntry()->itemId);
            case RG_BOTTLE_WITH_BIG_POE:
            case RG_BOTTLE_WITH_BLUE_FIRE:
            case RG_BOTTLE_WITH_BLUE_POTION:
            case RG_BOTTLE_WITH_BUGS:
            case RG_BOTTLE_WITH_FAIRY:
            case RG_BOTTLE_WITH_FISH:
            case RG_BOTTLE_WITH_GREEN_POTION:
            case RG_BOTTLE_WITH_MILK:
            case RG_BOTTLE_WITH_POE:
            case RG_BOTTLE_WITH_RED_POTION:
            case RG_EMPTY_BOTTLE:
                return HasBottle();
            default:
                break;
        }
        SPDLOG_ERROR("HasItem reached `return false;`. Missing case for RandomizerGet of {}", static_cast<uint32_t>(itemName));
        assert(false);
        return false;
    }

    //Can the passed in item be used?
    //RANDOTODO catch magic items explicitly and add an assert on miss.
    bool Logic::CanUse(RandomizerGet itemName) {
        if (!HasItem(itemName))
            return false;

        switch (itemName) {
            // Magic items
            case RG_MAGIC_SINGLE:
                return AmmoCanDrop || (HasBottle() && GetInLogic(LOGIC_BUY_MAGIC_POTION));
            case RG_DINS_FIRE:
            case RG_FARORES_WIND:
            case RG_NAYRUS_LOVE:
            case RG_LENS_OF_TRUTH:
                return CanUse(RG_MAGIC_SINGLE);
            case RG_FIRE_ARROWS:
            case RG_ICE_ARROWS:
            case RG_LIGHT_ARROWS:
                return CanUse(RG_MAGIC_SINGLE) && CanUse(RG_FAIRY_BOW);

            // Adult items
            // TODO: Uncomment those if we ever implement more item usability settings
            case RG_FAIRY_BOW:
                return IsAdult && (AmmoCanDrop || GetInLogic(LOGIC_BUY_ARROW));// || BowAsChild;
            case RG_MEGATON_HAMMER:
                return IsAdult;// || HammerAsChild;
            case RG_IRON_BOOTS:
                return IsAdult;// || IronBootsAsChild;
            case RG_HOVER_BOOTS:
                return IsAdult;// || HoverBootsAsChild;
            case RG_HOOKSHOT:
            case RG_LONGSHOT:
            case RG_SCARECROW:
            case RG_DISTANT_SCARECROW:
                return IsAdult;// || HookshotAsChild;
            case RG_GORON_TUNIC:
                return IsAdult;// || GoronTunicAsChild;
            case RG_ZORA_TUNIC:
                return IsAdult;// || ZoraTunicAsChild;
            case RG_MIRROR_SHIELD:
                return IsAdult;// || MirrorShieldAsChild;
            case RG_MASTER_SWORD:
                return IsAdult;// || MasterSwordAsChild;
            case RG_BIGGORON_SWORD:
                return IsAdult;// || BiggoronSwordAsChild;
            case RG_SILVER_GAUNTLETS:
            case RG_GOLDEN_GAUNTLETS:
            // Adult Trade
            case RG_POCKET_EGG:
            case RG_COJIRO:
            case RG_ODD_MUSHROOM:
            case RG_ODD_POTION:
            case RG_POACHERS_SAW:
            case RG_BROKEN_SWORD:
            case RG_PRESCRIPTION:
            case RG_EYEBALL_FROG:
            case RG_EYEDROPS:
            case RG_CLAIM_CHECK:
                return IsAdult;

            // Child items
            case RG_FAIRY_SLINGSHOT:
                return IsChild && (AmmoCanDrop || GetInLogic(LOGIC_BUY_SEED));// || SlingshotAsAdult;
            case RG_BOOMERANG:
                return IsChild;// || BoomerangAsAdult;
            case RG_KOKIRI_SWORD:
                return IsChild;// || KokiriSwordAsAdult;
            case RG_NUTS:
                return (NutPot  || NutCrate || DekuBabaNuts) && AmmoCanDrop; //RANDOTODO BuyNuts currently mixed in with Nuts, should be seperate as BuyNuts are also a Nuts source
            case RG_STICKS:
                return IsChild /* || StickAsAdult;*/&& (StickPot || DekuBabaSticks);
            case RG_DEKU_SHIELD:
                return IsChild;// || DekuShieldAsAdult;
            case RG_PROGRESSIVE_BOMB_BAG:
            case RG_BOMB_BAG:
                return AmmoCanDrop || GetInLogic(LOGIC_BUY_BOMB);
            case RG_PROGRESSIVE_BOMBCHUS:
            case RG_BOMBCHU_5:
            case RG_BOMBCHU_10:
            case RG_BOMBCHU_20:
                return BombchuRefill() && BombchusEnabled();
            case RG_WEIRD_EGG:
            case RG_RUTOS_LETTER:
                return IsChild;

            // Songs
            case RG_ZELDAS_LULLABY:
            case RG_EPONAS_SONG:
            case RG_PRELUDE_OF_LIGHT:
                return HasItem(RG_FAIRY_OCARINA) && HasItem(RG_OCARINA_C_LEFT_BUTTON) && HasItem(RG_OCARINA_C_RIGHT_BUTTON) && HasItem(RG_OCARINA_C_UP_BUTTON);
            case RG_SARIAS_SONG:
                return HasItem(RG_FAIRY_OCARINA) && HasItem(RG_OCARINA_C_LEFT_BUTTON) && HasItem(RG_OCARINA_C_RIGHT_BUTTON) && HasItem(RG_OCARINA_C_DOWN_BUTTON);
            case RG_SUNS_SONG:
                return HasItem(RG_FAIRY_OCARINA) && HasItem(RG_OCARINA_C_RIGHT_BUTTON) && HasItem(RG_OCARINA_C_UP_BUTTON) && HasItem(RG_OCARINA_C_DOWN_BUTTON);
            case RG_SONG_OF_TIME:
            case RG_BOLERO_OF_FIRE:
            case RG_REQUIEM_OF_SPIRIT:
                return HasItem(RG_FAIRY_OCARINA) && HasItem(RG_OCARINA_A_BUTTON) && HasItem(RG_OCARINA_C_RIGHT_BUTTON) && HasItem(RG_OCARINA_C_DOWN_BUTTON);
            case RG_SONG_OF_STORMS:
                return HasItem(RG_FAIRY_OCARINA) && HasItem(RG_OCARINA_A_BUTTON) && HasItem(RG_OCARINA_C_UP_BUTTON) && HasItem(RG_OCARINA_C_DOWN_BUTTON);
            case RG_MINUET_OF_FOREST:
                return HasItem(RG_FAIRY_OCARINA) && HasItem(RG_OCARINA_A_BUTTON) && HasItem(RG_OCARINA_C_LEFT_BUTTON) && HasItem(RG_OCARINA_C_RIGHT_BUTTON) && HasItem(RG_OCARINA_C_UP_BUTTON);
            case RG_SERENADE_OF_WATER:
            case RG_NOCTURNE_OF_SHADOW:
                return HasItem(RG_FAIRY_OCARINA) && HasItem(RG_OCARINA_A_BUTTON) && HasItem(RG_OCARINA_C_LEFT_BUTTON) && HasItem(RG_OCARINA_C_RIGHT_BUTTON) && HasItem(RG_OCARINA_C_DOWN_BUTTON);
            
            // Misc. Items
            case RG_FISHING_POLE:
                return HasItem(RG_CHILD_WALLET); // as long as you have enough rubies
            case RG_EPONA:
                return IsAdult && CanUse(RG_EPONAS_SONG);

            // Bottle Items
            case RG_BOTTLE_WITH_BUGS:
                return BugShrub || WanderingBugs || BugRock || GetInLogic(LOGIC_BUGS_ACCESS);
            case RG_BOTTLE_WITH_FISH:
                return LoneFish || FishGroup || GetInLogic(LOGIC_FISH_ACCESS); //is there any need to care about lone vs group?
            case RG_BOTTLE_WITH_BLUE_FIRE: //RANDOTODO should probably be better named to 
                return BlueFireAccess || GetInLogic(LOGIC_BLUE_FIRE_ACCESS);
            case RG_BOTTLE_WITH_FAIRY:
                return FairyPot || GossipStoneFairy || BeanPlantFairy || ButterflyFairy || FreeFairies || FairyPond || GetInLogic(LOGIC_FAIRY_ACCESS);

            default:
                SPDLOG_ERROR("CanUse reached `default` for {}. Assuming intention is no extra requirements for use so returning true, but HasItem should be used instead.", static_cast<uint32_t>(itemName));
                assert(false);
                return true;
        }
    }

    bool Logic::HasProjectile(HasProjectileAge age) {
        return HasExplosives() ||
               (age == HasProjectileAge::Child  && (CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG))) ||
               (age == HasProjectileAge::Adult  && (CanUse(RG_HOOKSHOT)  || CanUse(RG_FAIRY_BOW)      )) ||
               (age == HasProjectileAge::Both   && (CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG))  && (CanUse(RG_HOOKSHOT) || CanUse(RG_FAIRY_BOW))) ||
               (age == HasProjectileAge::Either && (CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG)   ||  CanUse(RG_HOOKSHOT) || CanUse(RG_FAIRY_BOW)));
    }

    bool Logic::HasBossSoul(RandomizerGet itemName) {
        if (!ctx->GetOption(RSK_SHUFFLE_BOSS_SOULS)) {
            return true;
        }
        switch(itemName) {
            case RG_GOHMA_SOUL:
            case RG_KING_DODONGO_SOUL:
            case RG_BARINADE_SOUL:
            case RG_PHANTOM_GANON_SOUL:
            case RG_VOLVAGIA_SOUL:
            case RG_MORPHA_SOUL:
            case RG_BONGO_BONGO_SOUL:
            case RG_TWINROVA_SOUL:
                return HasItem(itemName);
            case RG_GANON_SOUL:
                return ctx->GetOption(RSK_SHUFFLE_BOSS_SOULS).Is(RO_BOSS_SOULS_ON_PLUS_GANON) ? HasItem(RG_GANON_SOUL) : true;
            default:
                return false;
        }
    }

    uint8_t GetDifficultyValueFromString(Rando::Option& glitchOption) {
        return 0;
    }

    //todo rewrite glitch section

    bool Logic::CanEquipSwap(RandomizerGet itemName) {
        if (!HasItem(itemName))
            return false;
    
        if (CanDoGlitch(GlitchType::EquipSwapDins) || CanDoGlitch(GlitchType::EquipSwap))
            return true;

        return false;
    }

    bool Logic::CanDoGlitch(GlitchType glitch) {
        // TODO: Uncomment when glitches are implemented
        switch(glitch) {
            case GlitchType::EquipSwapDins:
                return ((IsAdult && HasItem(RG_DINS_FIRE)) || (IsChild && (HasItem(RG_STICKS) || HasItem(RG_DINS_FIRE)))) && false; //GlitchEquipSwapDins;
            case GlitchType::EquipSwap: // todo: add bunny hood to adult item equippable list and child trade item to child item equippable list
                return ((IsAdult && (HasItem(RG_DINS_FIRE) || HasItem(RG_FARORES_WIND) || HasItem(RG_NAYRUS_LOVE))) || (IsChild && (HasItem(RG_STICKS) || 
                    HasItem(RG_FAIRY_SLINGSHOT) || HasItem(RG_BOOMERANG) || HasBottle() || CanUse(RG_NUTS) || HasItem(RG_FAIRY_OCARINA) || HasItem(RG_LENS_OF_TRUTH) || HasExplosives() ||
                    GetAmmo(ITEM_BEAN) > 0 || HasItem(RG_DINS_FIRE) || HasItem(RG_FARORES_WIND) || HasItem(RG_NAYRUS_LOVE)))) && false; //GlitchEquipSwap;
        }

        //Shouldn't be reached
        return false;
    }

//RANDOTODO quantity is a placeholder for proper ammo use calculation logic. in time will want updating to account for ammo capacity
//Can we kill this enemy
    bool Logic::CanKillEnemy(RandomizerEnemy enemy, EnemyDistance distance, bool wallOrFloor, uint8_t quantity) {
        bool killed = false;
        switch(enemy) {
            case RE_GOLD_SKULLTULA:
            case RE_GOHMA_LARVA:
            case RE_MAD_SCRUB:
            case RE_DEKU_BABA:
                return CanAttack();
            case RE_BIG_SKULLTULA:
                switch (distance){
                    case ED_CLOSE:
                        //hammer jumpslash cannot damage these, but hammer swing can
                        killed = killed || CanUse(RG_MEGATON_HAMMER);
                        [[fallthrough]];
                    case ED_HAMMER_JUMPSLASH:
                    case ED_MASTER_SWORD_JUMPSLASH:
                        killed = killed || CanJumpslashExceptHammer();
                        [[fallthrough]];
                    case ED_RANG_OR_HOOKSHOT:
                        //RANDOTODO test dins, bomb and chu range in a practical example
                        killed = killed || CanUse(RG_HOOKSHOT) || CanUse(RG_BOMB_BAG) || (wallOrFloor && CanUse(RG_BOMBCHU_5)) || CanUse(RG_DINS_FIRE);
                        [[fallthrough]];
                    case ED_LONGSHOT:
                        killed = killed || CanUse(RG_LONGSHOT);
                        [[fallthrough]];
                    case ED_FAR:
                        killed = CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW);
                        break;
                }
                return killed;
            case RE_DODONGO:
            case RE_LIZALFOS:
                return CanJumpslash() || HasExplosives() || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW);
            case RE_KEESE:
            case RE_FIRE_KEESE:
                return CanJumpslash() || HasExplosives() || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || CanUse(RG_BOOMERANG);
            case RE_BLUE_BUBBLE:
                //RANDOTODO Trick to use shield hylian shield as child to stun these guys
                //RANDOTODO check hammer damage
                return BlastOrSmash() || CanUse(RG_FAIRY_BOW) || ((CanJumpslashExceptHammer() || CanUse(RG_FAIRY_SLINGSHOT)) && (CanUse(RG_NUTS) || HookshotOrBoomerang() || CanStandingShield()));
            case RE_DEAD_HAND:
                //RANDOTODO change Dead Hand trick to be sticks Dead Hand
                return CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || (CanUse(RG_STICKS) && ctx->GetTrickOption(RT_BOTW_CHILD_DEADHAND));
            case RE_WITHERED_DEKU_BABA:
                return CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_BOOMERANG);
            case RE_LIKE_LIKE:
            case RE_FLOORMASTER:
                return CanDamage();
            case RE_STALFOS:
                //RANDOTODO Add trick to kill stalfos with sticks, and a second one for bombs without stunning. Higher ammo logic for bombs is also plausible
                return CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_FAIRY_BOW) || CanUse(RG_BOMBCHU_5) || 
                       (quantity <= 2 && (CanUse(RG_NUTS) || HookshotOrBoomerang()) && CanUse(RG_BOMB_BAG)) || (quantity <= 1 && CanUse(RG_STICKS));
            //Needs 16 bombs, but is in default logic in N64, probably because getting the hits is quite easy.
            //bow and sling can wake them and damage after they shed their armour, so could reduce ammo requirements for explosives to 10.
            //requires 8 sticks to kill so would be a trick unless we apply higher stick bag logic
            case RE_IRON_KNUCKLE:
                return CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_MEGATON_HAMMER) || HasExplosives();
            //To stun flare dancer with chus, you have to hit the flame under it while it is spinning. It should eventually return to spinning after dashing for a while if you miss the window
            //it is possible to damage the core with explosives, but difficult to get all 4 hits in even with chus, and if it reconstructs the core heals, so it would be a trick.
            //the core takes damage from hookshot even if it doesn't show
            //Dins killing isn't hard, but is obscure and tight on single magic, so is a trick
            case RE_FLARE_DANCER:
                return CanUse(RG_MEGATON_HAMMER) || CanUse(RG_HOOKSHOT) || (HasExplosives() && (CanJumpslashExceptHammer() || CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG)));
            case RE_WOLFOS:
                return CanJumpslash() || CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOMBCHU_5) || (CanUse(RG_BOMB_BAG) && (CanUse(RG_NUTS) || CanUse(RG_HOOKSHOT) || CanUse(RG_BOOMERANG)));
            case RE_REDEAD:
                return CanJumpslash() || CanUse(RG_DINS_FIRE);
            case RE_MEG:
                return CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || HasExplosives();
            default:
                SPDLOG_ERROR("CanKillEnemy reached `default`.");
                assert(false);
                return false;
        }
    }

//It is rare for Pass Enemy to need distance, this only happens when the enemy blocks a platform and you can't reach it before it blocks you
//an example is the Big Skulltula in water room of MQ deku, which is out of sword swing height but blocks off the whole SoT block
//Can we get past this enemy in a tight space?
    bool Logic::CanPassEnemy(RandomizerEnemy enemy, EnemyDistance distance, bool wallOrFloor) {
        if (CanKillEnemy(enemy, distance, wallOrFloor)){
            return true;
        }
        switch(enemy) {
            case RE_GOLD_SKULLTULA: 
            case RE_GOHMA_LARVA:
            case RE_LIZALFOS:
            case RE_DODONGO: //RANDOTODO do dodongos block the way in tight corridors?
            case RE_MAD_SCRUB:
            case RE_KEESE:
            case RE_FIRE_KEESE:
            case RE_BLUE_BUBBLE:
            case RE_DEAD_HAND:
            case RE_DEKU_BABA:
            case RE_WITHERED_DEKU_BABA:
            case RE_STALFOS:
            case RE_FLARE_DANCER:
            case RE_WOLFOS:
            case RE_FLOORMASTER:
            case RE_MEG:
                return true;
            case RE_BIG_SKULLTULA:
                //hammer jumpslash can pass, but only on flat land where you can kill with hammer swing
                return CanUse(RG_NUTS) || CanUse(RG_BOOMERANG);
            case RE_LIKE_LIKE:
                return CanUse(RG_HOOKSHOT) || CanUse(RG_BOOMERANG);
            case RE_REDEAD:
                // we need a way to check if suns won't force a reload
                return CanUse(RG_HOOKSHOT) || CanUse(RG_SUNS_SONG);
            case RE_IRON_KNUCKLE:
                return false;
            default:
                SPDLOG_ERROR("CanPassEnemy reached `default`.");
                assert(false);
                return false;
        }
    }

//Can we avoid this enemy while climbing up a wall, or doing a difficult platforming challenge?
    bool Logic::CanAvoidEnemy(RandomizerEnemy enemy) {
        if (CanKillEnemy(enemy)){
            return true;
        }
        switch(enemy) {
            case RE_GOLD_SKULLTULA: 
            case RE_GOHMA_LARVA:
            case RE_LIZALFOS:
            case RE_DODONGO: //RANDOTODO do dodongos block the way in tight corridors?
            case RE_BIG_SKULLTULA:
            case RE_DEAD_HAND:
            case RE_DEKU_BABA:
            case RE_WITHERED_DEKU_BABA:
            case RE_LIKE_LIKE:
            case RE_STALFOS:
            case RE_IRON_KNUCKLE:
            case RE_FLARE_DANCER:
            case RE_WOLFOS:
            case RE_FLOORMASTER:
            case RE_REDEAD:
            case RE_MEG:
                return true;
            case RE_MAD_SCRUB:
            case RE_KEESE:
            case RE_FIRE_KEESE:
                return CanUse(RG_NUTS);
            case RE_BLUE_BUBBLE:
                //RANDOTODO Trick to use shield hylian shield as child to stun these guys
                return CanUse(RG_NUTS) || HookshotOrBoomerang() || CanStandingShield();
            default:
                SPDLOG_ERROR("CanPassEnemy reached `default`.");
                assert(false);
                return false;
        }
    }

    bool Logic::CanGetEnemyDrop(RandomizerEnemy enemy, EnemyDistance distance, bool aboveLink) {
        if (!CanKillEnemy(enemy, distance)){
            return false;
        }
        if (distance <= ED_MASTER_SWORD_JUMPSLASH){
            return true;
        }
        switch(enemy) {
            case RE_GOLD_SKULLTULA:
                //RANDOTODO double check all jumpslash kills that might be out of jump/backflip range
                return distance <= ED_HAMMER_JUMPSLASH || (distance <= ED_RANG_OR_HOOKSHOT && (CanUse(RG_HOOKSHOT) || CanUse(RG_BOOMERANG))) || (distance == ED_LONGSHOT && CanUse(RG_LONGSHOT));
            case RE_KEESE:
            case RE_FIRE_KEESE:
                return true;
            default:
                return aboveLink || (distance <= ED_RANG_OR_HOOKSHOT && CanUse(RG_BOOMERANG));
        }
    }

    bool Logic::CanBreakMudWalls() {
        //RANDOTODO blue fire tricks
        return BlastOrSmash();
    }

    bool Logic::CanGetDekuBabaSticks() {
        return (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD) || CanUse(RG_BOOMERANG));
    }

    bool Logic::CanGetDekuBabaNuts() {
        return CanJumpslash() || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_FAIRY_BOW) || HasExplosives() || CanUse(RG_DINS_FIRE);
    }

    bool Logic::CanHitEyeTargets() {
        return CanUse(RG_FAIRY_BOW) || CanUse(RG_FAIRY_SLINGSHOT);
    }

    bool Logic::CanDetonateBombFlowers() {
        return CanUse(RG_FAIRY_BOW) || HasExplosives() || CanUse(RG_DINS_FIRE);
    }

    bool Logic::CanDetonateUprightBombFlower() {
        return CanDetonateBombFlowers() || HasItem(RG_GORONS_BRACELET);
    }

    Logic::Logic() {
        
    }

    uint8_t Logic::BottleCount() {
        uint8_t count = 0;
        if (!CanEmptyBigPoes){
            return 0;
        }
        for (int i = SLOT_BOTTLE_1; i <= SLOT_BOTTLE_4; i++) {
            uint8_t item = GetSaveContext()->inventory.items[i];
            if (item != ITEM_NONE && (item != ITEM_LETTER_RUTO || (item == ITEM_LETTER_RUTO && DeliverLetter))) {
                count++;
            }
        }
        return count;
    }

    uint8_t Logic::OcarinaButtons(){
        return HasItem(RG_OCARINA_A_BUTTON) + HasItem(RG_OCARINA_C_LEFT_BUTTON) + HasItem(RG_OCARINA_C_RIGHT_BUTTON) + HasItem(RG_OCARINA_C_UP_BUTTON) + HasItem(RG_OCARINA_C_DOWN_BUTTON);
    }

    bool Logic::HasBottle(){
        return BottleCount() >= 1;
    }

    bool Logic::CanJumpslashExceptHammer() {
        // Not including hammer as hammer jump attacks can be weird;
        return CanUse(RG_STICKS) || CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD);
    }

    bool Logic::CanJumpslash() {
        return CanJumpslashExceptHammer() || CanUse(RG_MEGATON_HAMMER);
    }

    bool Logic::CanHitSwitch() {
        return CanUse(RG_FAIRY_SLINGSHOT) || CanJumpslash() || HasExplosives() || CanUse(RG_FAIRY_BOW) || CanUse(RG_BOOMERANG) || CanUse(RG_HOOKSHOT);
    }

    bool Logic::CanDamage() {
        return CanUse(RG_FAIRY_SLINGSHOT) || CanJumpslash() || HasExplosives() || CanUse(RG_DINS_FIRE) || CanUse(RG_FAIRY_BOW);
    }

    bool Logic::CanAttack() {
        return CanDamage() || CanUse(RG_BOOMERANG) || CanUse(RG_HOOKSHOT);
    }

    bool Logic::BombchusEnabled(){
        return ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) ? CheckInventory(ITEM_BOMBCHU, true) : HasItem(RG_BOMB_BAG);
    }

    // TODO: Implement Ammo Drop Setting in place of bombchu drops
    bool Logic::BombchuRefill(){
        return GetInLogic(LOGIC_BUY_BOMBCHUS) || CouldPlayBowling || CarpetMerchant || (ctx->GetOption(RSK_ENABLE_BOMBCHU_DROPS).Is(RO_AMMO_DROPS_ON/*_PLUS_BOMBCHU*/));
    }

    bool Logic::HookshotOrBoomerang(){
        return CanUse(RG_HOOKSHOT) || CanUse(RG_BOOMERANG);
    }

    bool Logic::ScarecrowsSong(){
        return (ctx->GetOption(RSK_SKIP_SCARECROWS_SONG) && HasItem(RG_FAIRY_OCARINA) && OcarinaButtons() >= 2)
                || (ChildScarecrow && AdultScarecrow);
    }

    bool Logic::BlueFire(){
        return CanUse(RG_BOTTLE_WITH_BLUE_FIRE) || (ctx->GetOption(RSK_BLUE_FIRE_ARROWS) && CanUse(RG_ICE_ARROWS));
    }

    bool Logic::HasExplosives(){
        return CanUse(RG_BOMB_BAG) || CanUse(RG_BOMBCHU_5);
    }

    bool Logic::BlastOrSmash(){
        return HasExplosives() || CanUse(RG_MEGATON_HAMMER);
    }

    bool Logic::CanSpawnSoilSkull(){
        return IsChild && CanUse(RG_BOTTLE_WITH_BUGS);
    }

    bool Logic::CanReflectNuts(){
        return CanUse(RG_DEKU_SHIELD) || (IsAdult && HasItem(RG_HYLIAN_SHIELD));
    }

    bool Logic::CanCutShrubs(){
        return CanUse(RG_KOKIRI_SWORD) || CanUse(RG_BOOMERANG) || HasExplosives() || CanUse(RG_MASTER_SWORD) || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_BIGGORON_SWORD);
    }

    bool Logic::CanStunDeku(){
        return CanAttack() || CanUse(RG_NUTS) || CanReflectNuts();
    }

    bool Logic::CanLeaveForest(){
        return ctx->GetOption(RSK_FOREST).IsNot(RO_FOREST_CLOSED) || IsAdult || DekuTreeClear || ctx->GetOption(RSK_SHUFFLE_INTERIOR_ENTRANCES) || ctx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES);
    }

    bool Logic::CallGossipFairyExceptSuns(){
        return CanUse(RG_ZELDAS_LULLABY) || CanUse(RG_EPONAS_SONG) || CanUse(RG_SONG_OF_TIME);
    }

    bool Logic::CallGossipFairy(){
        return CallGossipFairyExceptSuns() || CanUse(RG_SUNS_SONG);
    }

    //the number returned by this is in half heart hits taken.
    //RANDOTODO work in OoT side health instead for greater applicability (16 per heart)
    uint8_t Logic::EffectiveHealth(){
        /* Multiplier will be:
        0 for half daamge
        1 for normal damage
        2 for double damage
        3 for quad damage
        4 for 8* damage
        5 for 16* damage
        10 for OHKO.
        This is the number of shifts to apply, not a real multiplier
        */
        uint8_t Multiplier = (ctx->GetOption(RSK_DAMAGE_MULTIPLIER).Value<uint8_t>() < 6) ? ctx->GetOption(RSK_DAMAGE_MULTIPLIER).Value<uint8_t>() : 10;
        //(Hearts() << (2 + HasItem(RG_DOUBLE_DEFENSE))) is quarter hearts after DD
        //>> Multiplier halves on normal and does nothing on half, meaning we're working with half hearts on normal damage 
        return ((Hearts() << (2 + HasItem(RG_DOUBLE_DEFENSE))) >> Multiplier) + 
                //As 1 is a quarter heart, (1 << Multiplier) is effectivly half-hearts of unmodified damage
                //Adds an extra hit if the damage is not exact lethal
                ((Hearts() << (2 + HasItem(RG_DOUBLE_DEFENSE))) % (1 << Multiplier) > 0);
    }

    uint8_t Logic::Hearts(){
        return GetSaveContext()->healthCapacity / 16;
    }

    uint8_t Logic::DungeonCount(){
        return DekuTreeClear + DodongosCavernClear + JabuJabusBellyClear + ForestTempleClear + FireTempleClear + WaterTempleClear + SpiritTempleClear + ShadowTempleClear;
    }

    uint8_t Logic::StoneCount(){
        return HasItem(RG_KOKIRI_EMERALD) + HasItem(RG_GORON_RUBY) + HasItem(RG_ZORA_SAPPHIRE);
    }

    uint8_t Logic::MedallionCount(){
        return HasItem(RG_FOREST_MEDALLION) + HasItem(RG_FIRE_MEDALLION) + HasItem(RG_WATER_MEDALLION) + HasItem(RG_SPIRIT_MEDALLION) + HasItem(RG_SHADOW_MEDALLION) + HasItem(RG_LIGHT_MEDALLION);
    }

    uint8_t Logic::FireTimer(){
        return CanUse(RG_GORON_TUNIC) ? 255 : (ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS)) ? (Hearts() * 8) : 0;
    }
    
    uint8_t Logic::WaterTimer(){
        return CanUse(RG_ZORA_TUNIC) ? 255 : (ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS)) ? (Hearts() * 8) : 0;
    }

    bool Logic::TakeDamage(){
        return CanUse(RG_BOTTLE_WITH_FAIRY) || EffectiveHealth() != 1 || CanUse(RG_NAYRUS_LOVE);
    }

    bool Logic::CanOpenBombGrotto(){
        return BlastOrSmash() && (HasItem(RG_STONE_OF_AGONY) || ctx->GetTrickOption(RT_GROTTOS_WITHOUT_AGONY));
    }

    bool Logic::CanOpenStormsGrotto(){
        return CanUse(RG_SONG_OF_STORMS) && (HasItem(RG_STONE_OF_AGONY) || ctx->GetTrickOption(RT_GROTTOS_WITHOUT_AGONY));
    }

    bool Logic::CanGetNightTimeGS(){
        return CanUse(RG_SUNS_SONG) || !ctx->GetOption(RSK_SKULLS_SUNS_SONG);
    }

    bool Logic::CanBreakUpperBeehives(){
        return HookshotOrBoomerang() || (ctx->GetTrickOption(RT_BOMBCHU_BEEHIVES) && CanUse(RG_BOMBCHU_5));
    }

    bool Logic::CanBreakLowerBeehives(){
        return CanBreakUpperBeehives() || CanUse(RG_BOMB_BAG);
    }

    bool Logic::HasFireSource(){
        return CanUse(RG_DINS_FIRE) || CanUse(RG_FIRE_ARROWS);
    }

    bool Logic::HasFireSourceWithTorch(){
        return HasFireSource() || CanUse(RG_STICKS);
    }

//Is this best off signaling what you have already traded, or what step you are currently on?
    bool Logic::TradeQuestStep(RandomizerGet rg){
        if (ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE)){
            return false; //This does not apply when we are shuffling trade items
        }
        bool hasState = false;
        //Falling through each case to test each possibility
        switch (rg){
            case RG_POCKET_EGG:
                hasState = hasState || HasItem(RG_POCKET_EGG);
                [[fallthrough]];
            case RG_COJIRO:
                hasState = hasState || HasItem(RG_COJIRO);
                [[fallthrough]];
            case RG_ODD_MUSHROOM:
                hasState = hasState || HasItem(RG_ODD_MUSHROOM);
                [[fallthrough]];
            case RG_ODD_POTION:
                hasState = hasState || HasItem(RG_ODD_POTION);
                [[fallthrough]];
            case RG_POACHERS_SAW:
                hasState = hasState || HasItem(RG_POACHERS_SAW);
                [[fallthrough]];
            case RG_BROKEN_SWORD:
                hasState = hasState || HasItem(RG_BROKEN_SWORD);
                [[fallthrough]];
            case RG_PRESCRIPTION:
                hasState = hasState || HasItem(RG_PRESCRIPTION);
                [[fallthrough]];
            case RG_EYEDROPS:
                hasState = hasState || HasItem(RG_EYEDROPS);
                [[fallthrough]];
            case RG_CLAIM_CHECK:
                hasState = hasState || HasItem(RG_CLAIM_CHECK);
                break;
            default:
                SPDLOG_ERROR("TradeQuestStep reached `return false;`. Missing case for RandomizerGet of {}", static_cast<uint32_t>(rg));
                assert(false);
                return false;
        }
        return hasState;
    }

    bool Logic::CanFinishGerudoFortress(){
        return (ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_NORMAL) && SmallKeys(RR_GERUDO_FORTRESS, 4) && (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) && (HasItem(RG_GERUDO_MEMBERSHIP_CARD) || CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_GF_KITCHEN))) ||
               (ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_FAST)   && SmallKeys(RR_GERUDO_FORTRESS, 1) && (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD))) ||
               ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_OPEN);
    }

    bool Logic::CanStandingShield(){
        return CanUse(RG_MIRROR_SHIELD) || (IsAdult && HasItem(RG_HYLIAN_SHIELD)) || CanUse(RG_DEKU_SHIELD);
    }

    bool Logic::CanShield(){
        return CanUse(RG_MIRROR_SHIELD) || HasItem(RG_HYLIAN_SHIELD) || CanUse(RG_DEKU_SHIELD);
    }

    bool Logic::CanUseProjectile(){
        return  HasExplosives() || CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG);
    }

    bool Logic::CanBuildRainbowBridge(){
        return ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_ALWAYS_OPEN)      ||
               (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_VANILLA)         && HasItem(RG_SHADOW_MEDALLION) && HasItem(RG_SPIRIT_MEDALLION) && CanUse(RG_LIGHT_ARROWS)) ||
               (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_STONES)          && StoneCount() + (HasItem(RG_GREG_RUPEE) && ctx->GetOption(RSK_BRIDGE_OPTIONS).Is(RO_BRIDGE_GREG)) >= ctx->GetOption(RSK_RAINBOW_BRIDGE_STONE_COUNT).Value<uint8_t>()) ||
               (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_MEDALLIONS)      && MedallionCount() + (HasItem(RG_GREG_RUPEE) && ctx->GetOption(RSK_BRIDGE_OPTIONS).Is(RO_BRIDGE_GREG)) >= ctx->GetOption(RSK_RAINBOW_BRIDGE_MEDALLION_COUNT).Value<uint8_t>()) ||
               (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEON_REWARDS) && StoneCount() + MedallionCount() + (HasItem(RG_GREG_RUPEE) && ctx->GetOption(RSK_BRIDGE_OPTIONS).Is(RO_BRIDGE_GREG)) >= ctx->GetOption(RSK_RAINBOW_BRIDGE_REWARD_COUNT).Value<uint8_t>()) ||
               (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEONS)        && DungeonCount() + (HasItem(RG_GREG_RUPEE) && ctx->GetOption(RSK_BRIDGE_OPTIONS).Is(RO_BRIDGE_GREG)) >= ctx->GetOption(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT).Value<uint8_t>()) ||
               (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_TOKENS)          && GetGSCount() >= ctx->GetOption(RSK_RAINBOW_BRIDGE_TOKEN_COUNT).Value<uint8_t>()) ||
               (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_GREG)            && HasItem(RG_GREG_RUPEE));
    }

    bool Logic::CanTriggerLACS(){
        return (ctx->GetSettings()->LACSCondition() == RO_LACS_VANILLA    && HasItem(RG_SHADOW_MEDALLION) && HasItem(RG_SPIRIT_MEDALLION)) ||
               (ctx->GetSettings()->LACSCondition() == RO_LACS_STONES     && StoneCount() + (HasItem(RG_GREG_RUPEE) && ctx->GetOption(RSK_LACS_OPTIONS).Is(RO_LACS_GREG_REWARD)) >= ctx->GetOption(RSK_LACS_STONE_COUNT).Value<uint8_t>()) ||
               (ctx->GetSettings()->LACSCondition() == RO_LACS_MEDALLIONS && MedallionCount() + (HasItem(RG_GREG_RUPEE) && ctx->GetOption(RSK_LACS_OPTIONS).Is(RO_LACS_GREG_REWARD)) >= ctx->GetOption(RSK_LACS_MEDALLION_COUNT).Value<uint8_t>()) ||
               (ctx->GetSettings()->LACSCondition() == RO_LACS_REWARDS    && StoneCount() + MedallionCount() + (HasItem(RG_GREG_RUPEE) && ctx->GetOption(RSK_LACS_OPTIONS).Is(RO_LACS_GREG_REWARD)) >= ctx->GetOption(RSK_LACS_REWARD_COUNT).Value<uint8_t>()) ||
               (ctx->GetSettings()->LACSCondition() == RO_LACS_DUNGEONS   && DungeonCount() + (HasItem(RG_GREG_RUPEE) && ctx->GetOption(RSK_LACS_OPTIONS).Is(RO_LACS_GREG_REWARD)) >= ctx->GetOption(RSK_LACS_DUNGEON_COUNT).Value<uint8_t>()) ||
               (ctx->GetSettings()->LACSCondition() == RO_LACS_TOKENS     && GetGSCount() >= ctx->GetOption(RSK_LACS_TOKEN_COUNT).Value<uint8_t>());
    }

    bool Logic::SmallKeys(RandomizerRegion dungeon, uint8_t requiredAmount) {
        return SmallKeys(dungeon, requiredAmount, requiredAmount);
    }

    bool Logic::SmallKeys(RandomizerRegion dungeon, uint8_t requiredAmountGlitchless, uint8_t requiredAmountGlitched) {
        if (HasItem(RG_SKELETON_KEY)) {
            return true;
        }
        switch (dungeon) {
            case RR_FOREST_TEMPLE:
                /*if (IsGlitched && (GetDifficultyValueFromString(GlitchHookshotJump_Boots) >= static_cast<uint8_t>(GlitchDifficulty::INTERMEDIATE) || GetDifficultyValueFromString(GlitchHoverBoost) >= static_cast<uint8_t>(GlitchDifficulty::NOVICE) ||
                                    (GetDifficultyValueFromString(GlitchHover) >= static_cast<uint8_t>(GlitchDifficulty::NOVICE) && GetDifficultyValueFromString(GlitchISG) >= static_cast<uint8_t>(GlitchDifficulty::INTERMEDIATE)))) {
                    return ForestTempleKeys >= requiredAmountGlitched;
                }*/
                return GetSmallKeyCount(SCENE_FOREST_TEMPLE) >= requiredAmountGlitchless;

            case RR_FIRE_TEMPLE:
                /*if (IsGlitched && (GetDifficultyValueFromString(GlitchLedgeClip) >= static_cast<uint8_t>(GlitchDifficulty::INTERMEDIATE) || GetDifficultyValueFromString(GlitchHover) >= static_cast<uint8_t>(GlitchDifficulty::INTERMEDIATE))) {
                    return FireTempleKeys >= requiredAmountGlitched;
                }*/
                return GetSmallKeyCount(SCENE_FIRE_TEMPLE) >= requiredAmountGlitchless;

            case RR_WATER_TEMPLE:
                /*if (IsGlitched && (false)) {
                    return WaterTempleKeys >= requiredAmountGlitched;
                }*/
                return GetSmallKeyCount(SCENE_WATER_TEMPLE) >= requiredAmountGlitchless;

            case RR_SPIRIT_TEMPLE:
                /*if (IsGlitched && (false)) {
                    return SpiritTempleKeys >= requiredAmountGlitched;
                }*/
                return GetSmallKeyCount(SCENE_SPIRIT_TEMPLE) >= requiredAmountGlitchless;

            case RR_SHADOW_TEMPLE:
                /*if (IsGlitched && (GetDifficultyValueFromString(GlitchHookshotClip) >= static_cast<uint8_t>(GlitchDifficulty::NOVICE))) {
                    return ShadowTempleKeys >= requiredAmountGlitched;
                }*/
                return GetSmallKeyCount(SCENE_SHADOW_TEMPLE) >= requiredAmountGlitchless;

            case RR_BOTTOM_OF_THE_WELL:
                /*if (IsGlitched && (false)) {
                    return BottomOfTheWellKeys >= requiredAmountGlitched;
                }*/
                return GetSmallKeyCount(SCENE_BOTTOM_OF_THE_WELL) >= requiredAmountGlitchless;

            case RR_GERUDO_TRAINING_GROUNDS:
                /*if (IsGlitched && (false)) {
                    return GerudoTrainingGroundsKeys >= requiredAmountGlitched;
                }*/
                return GetSmallKeyCount(SCENE_GERUDO_TRAINING_GROUND) >= requiredAmountGlitchless;

            case RR_GANONS_CASTLE:
                /*if (IsGlitched && (false)) {
                    return GanonsCastleKeys >= requiredAmountGlitched;
                }*/
                return GetSmallKeyCount(SCENE_INSIDE_GANONS_CASTLE) >= requiredAmountGlitchless;

            case RR_MARKET_TREASURE_CHEST_GAME:
                /*if (IsGlitched && (false)) {
                    return TreasureGameKeys >= requiredAmountGlitched;
                }*/
                return GetSmallKeyCount(SCENE_TREASURE_BOX_SHOP) >= requiredAmountGlitchless;

            case RR_GERUDO_FORTRESS:
                return GetSmallKeyCount(SCENE_THIEVES_HIDEOUT) >= requiredAmountGlitchless;

            default:
                return false;
        }
    }

    std::map<RandomizerGet, uint32_t> Logic::RandoGetToEquipFlag = {
        { RG_KOKIRI_SWORD,           EQUIP_FLAG_SWORD_KOKIRI },
        { RG_MASTER_SWORD,           EQUIP_FLAG_SWORD_MASTER },
        { RG_BIGGORON_SWORD,         EQUIP_FLAG_SWORD_BGS },
        { RG_DEKU_SHIELD,            EQUIP_FLAG_SHIELD_DEKU },
        { RG_HYLIAN_SHIELD,          EQUIP_FLAG_SHIELD_HYLIAN },
        { RG_MIRROR_SHIELD,          EQUIP_FLAG_SHIELD_MIRROR },
        { RG_GORON_TUNIC,            EQUIP_FLAG_TUNIC_GORON },
        { RG_ZORA_TUNIC,             EQUIP_FLAG_TUNIC_ZORA },
        { RG_BUY_DEKU_SHIELD,        EQUIP_FLAG_SHIELD_DEKU },
        { RG_BUY_HYLIAN_SHIELD,      EQUIP_FLAG_SHIELD_HYLIAN },
        { RG_BUY_GORON_TUNIC,        EQUIP_FLAG_TUNIC_GORON },
        { RG_BUY_ZORA_TUNIC,         EQUIP_FLAG_TUNIC_ZORA },
        { RG_IRON_BOOTS,             EQUIP_FLAG_BOOTS_IRON },
        { RG_HOVER_BOOTS,            EQUIP_FLAG_BOOTS_HOVER }
    };

    std::map<RandomizerGet, uint32_t> Logic::RandoGetToRandInf = {
        { RG_ZELDAS_LETTER,          RAND_INF_ZELDAS_LETTER },
        { RG_WEIRD_EGG,              RAND_INF_WEIRD_EGG },
        { RG_GOHMA_SOUL,             RAND_INF_GOHMA_SOUL },
        { RG_KING_DODONGO_SOUL,      RAND_INF_KING_DODONGO_SOUL },
        { RG_BARINADE_SOUL,          RAND_INF_BARINADE_SOUL },
        { RG_PHANTOM_GANON_SOUL,     RAND_INF_PHANTOM_GANON_SOUL },
        { RG_VOLVAGIA_SOUL,          RAND_INF_VOLVAGIA_SOUL },
        { RG_MORPHA_SOUL,            RAND_INF_MORPHA_SOUL },
        { RG_BONGO_BONGO_SOUL,       RAND_INF_BONGO_BONGO_SOUL },
        { RG_TWINROVA_SOUL,          RAND_INF_TWINROVA_SOUL },
        { RG_GANON_SOUL,             RAND_INF_GANON_SOUL },
        { RG_OCARINA_A_BUTTON,       RAND_INF_HAS_OCARINA_A },
        { RG_OCARINA_C_UP_BUTTON,    RAND_INF_HAS_OCARINA_C_UP },
        { RG_OCARINA_C_DOWN_BUTTON,  RAND_INF_HAS_OCARINA_C_DOWN },
        { RG_OCARINA_C_LEFT_BUTTON,  RAND_INF_HAS_OCARINA_C_LEFT },
        { RG_OCARINA_C_RIGHT_BUTTON, RAND_INF_HAS_OCARINA_C_RIGHT },
        { RG_SKELETON_KEY,           RAND_INF_HAS_SKELETON_KEY },
        { RG_GREG_RUPEE,             RAND_INF_GREG_FOUND },
        { RG_FISHING_POLE,           RAND_INF_FISHING_POLE_FOUND }
    };

    std::map<uint32_t, uint32_t> Logic::RandoGetToDungeonScene = {
        { RG_FOREST_TEMPLE_SMALL_KEY,           SCENE_FOREST_TEMPLE },
        { RG_FIRE_TEMPLE_SMALL_KEY,             SCENE_FIRE_TEMPLE },
        { RG_WATER_TEMPLE_SMALL_KEY,            SCENE_WATER_TEMPLE },
        { RG_SPIRIT_TEMPLE_SMALL_KEY,           SCENE_SPIRIT_TEMPLE },
        { RG_SHADOW_TEMPLE_SMALL_KEY,           SCENE_SHADOW_TEMPLE },
        { RG_BOTTOM_OF_THE_WELL_SMALL_KEY,      SCENE_BOTTOM_OF_THE_WELL },
        { RG_GERUDO_TRAINING_GROUNDS_SMALL_KEY, SCENE_GERUDO_TRAINING_GROUND },
        { RG_GERUDO_FORTRESS_SMALL_KEY,         SCENE_THIEVES_HIDEOUT },
        { RG_GANONS_CASTLE_SMALL_KEY,           SCENE_INSIDE_GANONS_CASTLE },
        { RG_FOREST_TEMPLE_KEY_RING,            SCENE_FOREST_TEMPLE },
        { RG_FIRE_TEMPLE_KEY_RING,              SCENE_FIRE_TEMPLE },
        { RG_WATER_TEMPLE_KEY_RING,             SCENE_WATER_TEMPLE },
        { RG_SPIRIT_TEMPLE_KEY_RING,            SCENE_SPIRIT_TEMPLE },
        { RG_SHADOW_TEMPLE_KEY_RING,            SCENE_SHADOW_TEMPLE },
        { RG_BOTTOM_OF_THE_WELL_KEY_RING,       SCENE_BOTTOM_OF_THE_WELL },
        { RG_GERUDO_TRAINING_GROUNDS_KEY_RING,  SCENE_GERUDO_TRAINING_GROUND },
        { RG_GERUDO_FORTRESS_KEY_RING,          SCENE_THIEVES_HIDEOUT },
        { RG_GANONS_CASTLE_KEY_RING,            SCENE_INSIDE_GANONS_CASTLE },
        { RG_FOREST_TEMPLE_BOSS_KEY,            SCENE_FOREST_TEMPLE },
        { RG_FIRE_TEMPLE_BOSS_KEY,              SCENE_FIRE_TEMPLE },
        { RG_WATER_TEMPLE_BOSS_KEY,             SCENE_WATER_TEMPLE },
        { RG_SPIRIT_TEMPLE_BOSS_KEY,            SCENE_SPIRIT_TEMPLE },
        { RG_SHADOW_TEMPLE_BOSS_KEY,            SCENE_SHADOW_TEMPLE },
        { RG_GANONS_CASTLE_BOSS_KEY,            SCENE_INSIDE_GANONS_CASTLE },
        { RG_DEKU_TREE_MAP,                     SCENE_DEKU_TREE },
        { RG_DODONGOS_CAVERN_MAP,               SCENE_DODONGOS_CAVERN },
        { RG_JABU_JABUS_BELLY_MAP,              SCENE_JABU_JABU },
        { RG_FOREST_TEMPLE_MAP,                 SCENE_FOREST_TEMPLE },
        { RG_FIRE_TEMPLE_MAP,                   SCENE_FIRE_TEMPLE },
        { RG_WATER_TEMPLE_MAP,                  SCENE_WATER_TEMPLE },
        { RG_SPIRIT_TEMPLE_MAP,                 SCENE_SPIRIT_TEMPLE },
        { RG_SHADOW_TEMPLE_MAP,                 SCENE_SHADOW_TEMPLE },
        { RG_BOTTOM_OF_THE_WELL_MAP,            SCENE_BOTTOM_OF_THE_WELL },
        { RG_ICE_CAVERN_MAP,                    SCENE_ICE_CAVERN },
        { RG_DEKU_TREE_COMPASS,                 SCENE_DEKU_TREE },
        { RG_DODONGOS_CAVERN_COMPASS,           SCENE_DODONGOS_CAVERN },
        { RG_JABU_JABUS_BELLY_COMPASS,          SCENE_JABU_JABU },
        { RG_FOREST_TEMPLE_COMPASS,             SCENE_FOREST_TEMPLE },
        { RG_FIRE_TEMPLE_COMPASS,               SCENE_FIRE_TEMPLE },
        { RG_WATER_TEMPLE_COMPASS,              SCENE_WATER_TEMPLE },
        { RG_SPIRIT_TEMPLE_COMPASS,             SCENE_SPIRIT_TEMPLE },
        { RG_SHADOW_TEMPLE_COMPASS,             SCENE_SHADOW_TEMPLE },
        { RG_BOTTOM_OF_THE_WELL_COMPASS,        SCENE_BOTTOM_OF_THE_WELL },
        { RG_ICE_CAVERN_COMPASS,                SCENE_ICE_CAVERN },
        { RG_TREASURE_GAME_SMALL_KEY,           SCENE_TREASURE_BOX_SHOP }
    };

    std::map<uint32_t, uint32_t> Logic::RandoGetToQuestItem = {
        { RG_FOREST_MEDALLION,       QUEST_MEDALLION_FOREST },
        { RG_FIRE_MEDALLION,         QUEST_MEDALLION_FIRE },
        { RG_WATER_MEDALLION,        QUEST_MEDALLION_WATER },
        { RG_SPIRIT_MEDALLION,       QUEST_MEDALLION_SPIRIT },
        { RG_SHADOW_MEDALLION,       QUEST_MEDALLION_SHADOW },
        { RG_LIGHT_MEDALLION,        QUEST_MEDALLION_LIGHT },
        { RG_MINUET_OF_FOREST,       QUEST_SONG_MINUET },
        { RG_BOLERO_OF_FIRE,         QUEST_SONG_BOLERO },
        { RG_SERENADE_OF_WATER,      QUEST_SONG_SERENADE },
        { RG_REQUIEM_OF_SPIRIT,      QUEST_SONG_REQUIEM },
        { RG_NOCTURNE_OF_SHADOW,     QUEST_SONG_NOCTURNE },
        { RG_PRELUDE_OF_LIGHT,       QUEST_SONG_PRELUDE },
        { RG_ZELDAS_LULLABY,         QUEST_SONG_LULLABY },
        { RG_EPONAS_SONG,            QUEST_SONG_EPONA },
        { RG_SARIAS_SONG,            QUEST_SONG_SARIA },
        { RG_SUNS_SONG,              QUEST_SONG_SUN },
        { RG_SONG_OF_TIME,           QUEST_SONG_TIME },
        { RG_SONG_OF_STORMS,         QUEST_SONG_STORMS },
        { RG_KOKIRI_EMERALD,         QUEST_KOKIRI_EMERALD },
        { RG_GORON_RUBY,             QUEST_GORON_RUBY },
        { RG_ZORA_SAPPHIRE,          QUEST_ZORA_SAPPHIRE },
        { RG_STONE_OF_AGONY,         QUEST_STONE_OF_AGONY },
        { RG_GERUDO_MEMBERSHIP_CARD, QUEST_GERUDO_CARD },
    };

    uint32_t HookshotLookup[3] = { ITEM_NONE, ITEM_HOOKSHOT, ITEM_LONGSHOT };
    uint32_t OcarinaLookup[3] = { ITEM_NONE, ITEM_OCARINA_FAIRY, ITEM_OCARINA_TIME };

    void Logic::ApplyItemEffect(Item& item, bool state) {
        auto randoGet = item.GetRandomizerGet();
        if (item.GetGIEntry()->objectId == OBJECT_GI_STICK) {
            SetInventory(ITEM_STICK, (!state ? ITEM_NONE : ITEM_STICK));
        }
        if (item.GetGIEntry()->objectId == OBJECT_GI_NUTS) {
            SetInventory(ITEM_NUT, (!state ? ITEM_NONE : ITEM_NUT));
        }
        switch (item.GetItemType()) {
        case ITEMTYPE_ITEM:
        {
            switch (randoGet) {
            case RG_STONE_OF_AGONY:
            case RG_GERUDO_MEMBERSHIP_CARD:
                SetQuestItem(RandoGetToQuestItem.at(randoGet), state);
                break;
            case RG_WEIRD_EGG:
                SetRandoInf(RAND_INF_WEIRD_EGG, state);
                break;
            case RG_ZELDAS_LETTER:
                SetRandoInf(RAND_INF_ZELDAS_LETTER, state);
                break;
            case RG_DOUBLE_DEFENSE:
                mSaveContext->isDoubleDefenseAcquired = state;
                break;
            case RG_POCKET_EGG:
            case RG_COJIRO:
            case RG_ODD_MUSHROOM:
            case RG_ODD_POTION:
            case RG_POACHERS_SAW:
            case RG_BROKEN_SWORD:
            case RG_PRESCRIPTION:
            case RG_EYEBALL_FROG:
            case RG_EYEDROPS:
            case RG_CLAIM_CHECK:
                SetAdultTrade(item.GetGIEntry()->itemId, state);
                break;
            case RG_PROGRESSIVE_HOOKSHOT:
            {
                uint8_t i;
                for (i = 0; i < 3; i++) {
                    if (CurrentInventory(ITEM_HOOKSHOT) == HookshotLookup[i]) {
                        break;
                    }
                }
                auto newItem = i + (!state ? -1 : 1);
                if (newItem < 0) {
                    newItem = 0;
                }
                else if (newItem > 2) {
                    newItem = 2;
                }
                SetInventory(ITEM_HOOKSHOT, HookshotLookup[newItem]);
            }   break;
            case RG_PROGRESSIVE_STRENGTH:
            {
                auto currentLevel = CurrentUpgrade(UPG_STRENGTH);
                auto newLevel = currentLevel + (!state ? -1 : 1);
                SetUpgrade(UPG_STRENGTH, newLevel);
            }   break;
            case RG_PROGRESSIVE_BOMB_BAG:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_BOMB_BAG_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_BOMB_BAG, true);
                    break;
                }
                auto currentLevel = CurrentUpgrade(UPG_BOMB_BAG);
                auto newLevel = currentLevel + (!state ? -1 : 1);
                if (currentLevel == 0 && state || currentLevel == 1 && !state) {
                    SetInventory(ITEM_BOMB, (!state ? ITEM_NONE : ITEM_BOMB));
                }
                SetUpgrade(UPG_BOMB_BAG, newLevel);
            }   break;
            case RG_PROGRESSIVE_BOW:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_QUIVER_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_QUIVER, true);
                    break;
                }
                auto currentLevel = CurrentUpgrade(UPG_QUIVER);
                auto newLevel = currentLevel + (!state ? -1 : 1);
                if (currentLevel == 0 && state || currentLevel == 1 && !state) {
                    SetInventory(ITEM_BOW, (!state ? ITEM_NONE : ITEM_BOW));
                }
                SetUpgrade(UPG_QUIVER, newLevel);
            }   break;
            case RG_PROGRESSIVE_SLINGSHOT:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_BULLET_BAG_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_BULLET_BAG, true);
                    break;
                }
                auto currentLevel = CurrentUpgrade(UPG_BULLET_BAG);
                auto newLevel = currentLevel + (!state ? -1 : 1);
                if (currentLevel == 0 && state || currentLevel == 1 && !state) {
                    SetInventory(ITEM_SLINGSHOT, (!state ? ITEM_NONE : ITEM_SLINGSHOT));
                }
                SetUpgrade(UPG_BULLET_BAG, newLevel);
            }   break;
            case RG_PROGRESSIVE_WALLET:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_WALLET_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_MONEY, true);
                    break;
                }
                auto currentLevel = CurrentUpgrade(UPG_WALLET);
                if (!CheckRandoInf(RAND_INF_HAS_WALLET) && state) {
                    SetRandoInf(RAND_INF_HAS_WALLET, true);
                }
                else if (currentLevel == 0 && !state) {
                    SetRandoInf(RAND_INF_HAS_WALLET, false);
                }
                else {
                    auto newLevel = currentLevel + (!state ? -1 : 1);
                    SetUpgrade(UPG_WALLET, newLevel);
                }
            }   break;
            case RG_PROGRESSIVE_SCALE:
            {
                auto currentLevel = CurrentUpgrade(UPG_SCALE);
                if (!CheckRandoInf(RAND_INF_CAN_SWIM) && state) {
                    SetRandoInf(RAND_INF_CAN_SWIM, true);
                }
                else if (currentLevel == 0 && !state) {
                    SetRandoInf(RAND_INF_CAN_SWIM, false);
                }
                else {
                    auto newLevel = currentLevel + (!state ? -1 : 1);
                    SetUpgrade(UPG_SCALE, newLevel);
                }
            }   break;
            case RG_PROGRESSIVE_NUT_UPGRADE:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_NUT_UPGRADE_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_NUT_UPGRADE, true);
                    break;
                }
                auto currentLevel = CurrentUpgrade(UPG_NUTS);
                auto newLevel = currentLevel + (!state ? -1 : 1);
                if (currentLevel == 0 && state || currentLevel == 1 && !state) {
                    SetInventory(ITEM_NUT, (!state ? ITEM_NONE : ITEM_NUT));
                }
                SetUpgrade(UPG_NUTS, newLevel);
            }   break;
            case RG_PROGRESSIVE_STICK_UPGRADE:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_STICK_UPGRADE_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_STICK_UPGRADE, true);
                    break;
                }
                auto currentLevel = CurrentUpgrade(UPG_STICKS);
                auto newLevel = currentLevel + (!state ? -1 : 1);
                if (currentLevel == 0 && state || currentLevel == 1 && !state) {
                    SetInventory(ITEM_STICK, (!state ? ITEM_NONE : ITEM_STICK));
                }
                SetUpgrade(UPG_STICKS, newLevel);
            }   break;
            case RG_PROGRESSIVE_BOMBCHUS:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_BOMBCHU_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_BOMBCHUS, true);
                    break;
                }
                SetInventory(ITEM_BOMBCHU, (!state ? ITEM_NONE : ITEM_BOMBCHU));
            } break;
            case RG_PROGRESSIVE_MAGIC_METER:
            {
                auto realGI = item.GetGIEntry();
                if (realGI->itemId == RG_MAGIC_INF && realGI->modIndex == MOD_RANDOMIZER) {
                    SetRandoInf(RAND_INF_HAS_INFINITE_MAGIC_METER, true);
                    break;
                }
                mSaveContext->magicLevel += (!state ? -1 : 1);
            } break;
            case RG_PROGRESSIVE_OCARINA:
            {
                uint8_t i;
                for (i = 0; i < 3; i++) {
                    if (CurrentInventory(ITEM_OCARINA_FAIRY) == OcarinaLookup[i]) {
                        break;
                    }
                }
                i += (!state ? -1 : 1);
                if (i < 0) {
                    i = 0;
                }
                else if (i > 2) {
                    i = 2;
                }
                SetInventory(ITEM_OCARINA_FAIRY, OcarinaLookup[i]);
            }   break;
            case RG_HEART_CONTAINER:
                mSaveContext->healthCapacity += (!state ? -16 : 16);
                break;
            case RG_PIECE_OF_HEART:
                mSaveContext->healthCapacity += (!state ? -4 : 4);
                break;
            case RG_BOOMERANG:
            case RG_LENS_OF_TRUTH:
            case RG_MEGATON_HAMMER:
            case RG_DINS_FIRE:
            case RG_FARORES_WIND:
            case RG_NAYRUS_LOVE:
            case RG_FIRE_ARROWS:
            case RG_ICE_ARROWS:
            case RG_LIGHT_ARROWS:
                SetInventory(item.GetGIEntry()->itemId, (!state ? ITEM_NONE : item.GetGIEntry()->itemId));
                break;
            case RG_MAGIC_BEAN:
            case RG_MAGIC_BEAN_PACK:
            {
                auto change = (item.GetRandomizerGet() == RG_MAGIC_BEAN ? 1 : 10);
                auto current = GetAmmo(ITEM_BEAN);
                SetAmmo(ITEM_BEAN, current + (!state ? -change : change));
            }   break;
            case RG_EMPTY_BOTTLE:
            case RG_BOTTLE_WITH_MILK:
            case RG_BOTTLE_WITH_RED_POTION:
            case RG_BOTTLE_WITH_GREEN_POTION:
            case RG_BOTTLE_WITH_BLUE_POTION:
            case RG_BOTTLE_WITH_FAIRY:
            case RG_BOTTLE_WITH_FISH:
            case RG_BOTTLE_WITH_BLUE_FIRE:
            case RG_BOTTLE_WITH_BUGS:
            case RG_BOTTLE_WITH_POE:
            case RG_BOTTLE_WITH_BIG_POE:
            {
                uint8_t slot = SLOT_BOTTLE_1;
                while (slot != SLOT_BOTTLE_4) {
                    if (mSaveContext->inventory.items[slot] == ITEM_NONE) {
                        break;
                    }
                    slot++;
                }
                mSaveContext->inventory.items[slot] = item.GetGIEntry()->itemId;
            }   break;
            case RG_RUTOS_LETTER:
                SetEventChkInf(EVENTCHKINF_OBTAINED_RUTOS_LETTER, state);
                break;
            case RG_GOHMA_SOUL:
            case RG_KING_DODONGO_SOUL:
            case RG_BARINADE_SOUL:
            case RG_PHANTOM_GANON_SOUL:
            case RG_VOLVAGIA_SOUL:
            case RG_MORPHA_SOUL:
            case RG_BONGO_BONGO_SOUL:
            case RG_TWINROVA_SOUL:
            case RG_GANON_SOUL:
            case RG_OCARINA_A_BUTTON:
            case RG_OCARINA_C_UP_BUTTON:
            case RG_OCARINA_C_DOWN_BUTTON:
            case RG_OCARINA_C_LEFT_BUTTON:
            case RG_OCARINA_C_RIGHT_BUTTON:
            case RG_GREG_RUPEE:
            case RG_FISHING_POLE:
                SetRandoInf(RandoGetToRandInf.at(randoGet), state);
                break;
            case RG_TRIFORCE_PIECE:
                mSaveContext->triforcePiecesCollected += (!state ? -1 : 1);
                break;
            case RG_BOMBCHU_5:
            case RG_BOMBCHU_10:
            case RG_BOMBCHU_20:
                SetInventory(ITEM_BOMBCHU, (!state ? ITEM_NONE : ITEM_BOMBCHU));
                break;
            }
        }
        break;
        case ITEMTYPE_EQUIP:
        {
            RandomizerGet itemRG = item.GetRandomizerGet();
            if (itemRG == RG_GIANTS_KNIFE) {
                return;
            }
            uint32_t equipId = RandoGetToEquipFlag.find(itemRG)->second;
            if (!state) {
                mSaveContext->inventory.equipment &= ~equipId;
                if (equipId == EQUIP_FLAG_SWORD_BGS) {
                    mSaveContext->bgsFlag = false;
                }
            }
            else {
                mSaveContext->inventory.equipment |= equipId;
                if (equipId == EQUIP_FLAG_SWORD_BGS) {
                    mSaveContext->bgsFlag = true;
                }
            }
        }
        break;
        case ITEMTYPE_DUNGEONREWARD:
        case ITEMTYPE_SONG:
            SetQuestItem(RandoGetToQuestItem.find(item.GetRandomizerGet())->second, state);
            break;
        case ITEMTYPE_MAP:
            SetDungeonItem(DUNGEON_MAP, RandoGetToDungeonScene.find(item.GetRandomizerGet())->second, state);
            break;
        case ITEMTYPE_COMPASS:
            SetDungeonItem(DUNGEON_COMPASS, RandoGetToDungeonScene.find(item.GetRandomizerGet())->second, state);
            break;
        case ITEMTYPE_BOSSKEY:
            SetDungeonItem(DUNGEON_KEY_BOSS, RandoGetToDungeonScene.find(item.GetRandomizerGet())->second, state);
            break;
        case ITEMTYPE_FORTRESS_SMALLKEY:
        case ITEMTYPE_SMALLKEY:
        {
            auto randoGet = item.GetRandomizerGet();
            auto keyring = randoGet >= RG_FOREST_TEMPLE_KEY_RING && randoGet <= RG_GANONS_CASTLE_KEY_RING;
            auto dungeonIndex = RandoGetToDungeonScene.find(randoGet)->second;
            auto count = GetSmallKeyCount(dungeonIndex);
            if (!state) {
                if (keyring) {
                    count = 0;
                }
                else {
                    count -= 1;
                }
            }
            else {
                if (keyring) {
                    count = 10;
                }
                else {
                    count += 1;
                }
            }
            SetSmallKeyCount(dungeonIndex, count);
        } break;
        case ITEMTYPE_TOKEN:
            mSaveContext->inventory.gsTokens += (!state ? -1 : 1);
            break;
        case ITEMTYPE_EVENT:
            break;
        case ITEMTYPE_DROP:
        case ITEMTYPE_REFILL:
        case ITEMTYPE_SHOP:
        {
            RandomizerGet itemRG = item.GetRandomizerGet();
            if (itemRG == RG_BUY_HYLIAN_SHIELD || itemRG == RG_BUY_DEKU_SHIELD || itemRG == RG_BUY_GORON_TUNIC || itemRG == RG_BUY_ZORA_TUNIC) {
                uint32_t equipId = RandoGetToEquipFlag.find(itemRG)->second;
                if (!state) {
                    mSaveContext->inventory.equipment &= ~equipId;
                }
                else {
                    mSaveContext->inventory.equipment |= equipId;
                }
            }
            switch (itemRG) {
            case RG_DEKU_NUTS_5:
            case RG_DEKU_NUTS_10:
            case RG_BUY_DEKU_NUTS_5:
            case RG_BUY_DEKU_NUTS_10:
                SetInventory(ITEM_NUT, (!state ? ITEM_NONE : ITEM_NUT));
                break;
            case RG_DEKU_STICK_1:
            case RG_BUY_DEKU_STICK_1:
            case RG_STICKS:
                SetInventory(ITEM_STICK, (!state ? ITEM_NONE : ITEM_STICK));
                break;
            case RG_BOMBCHU_5:
            case RG_BOMBCHU_10:
            case RG_BOMBCHU_20:
                SetInventory(ITEM_BOMBCHU, (!state ? ITEM_NONE : ITEM_BOMBCHU));
                break;
            }
        } break;
        }
    }

    SaveContext* Logic::GetSaveContext() {
        if (mSaveContext == nullptr) {
            NewSaveContext();
        }
        return mSaveContext;
    }

    void Logic::SetSaveContext(SaveContext* context) {
        mSaveContext = context;
    }

    void Logic::InitSaveContext() {
        mSaveContext->totalDays = 0;
        mSaveContext->bgsDayCount = 0;

        mSaveContext->deaths = 0;
        for (int i = 0; i < ARRAY_COUNT(mSaveContext->playerName); i++) {
            mSaveContext->playerName[i] = 0x3E;
        }
        mSaveContext->n64ddFlag = 0;
        mSaveContext->healthCapacity = 0x30;
        mSaveContext->health = 0x30;
        mSaveContext->magicLevel = 0;
        mSaveContext->magic = 0x30;
        mSaveContext->rupees = 0;
        mSaveContext->swordHealth = 0;
        mSaveContext->naviTimer = 0;
        mSaveContext->isMagicAcquired = 0;
        mSaveContext->isDoubleMagicAcquired = 0;
        mSaveContext->isDoubleDefenseAcquired = 0;
        mSaveContext->bgsFlag = 0;
        mSaveContext->ocarinaGameRoundNum = 0;
        for (int button = 0; button < ARRAY_COUNT(mSaveContext->childEquips.buttonItems); button++) {
            mSaveContext->childEquips.buttonItems[button] = ITEM_NONE;
        }
        for (int button = 0; button < ARRAY_COUNT(mSaveContext->childEquips.cButtonSlots); button++) {
            mSaveContext->childEquips.cButtonSlots[button] = SLOT_NONE;
        }
        mSaveContext->childEquips.equipment = 0;
        for (int button = 0; button < ARRAY_COUNT(mSaveContext->adultEquips.buttonItems); button++) {
            mSaveContext->adultEquips.buttonItems[button] = ITEM_NONE;
        }
        for (int button = 0; button < ARRAY_COUNT(mSaveContext->adultEquips.cButtonSlots); button++) {
            mSaveContext->adultEquips.cButtonSlots[button] = SLOT_NONE;
        }
        mSaveContext->adultEquips.equipment = 0;
        mSaveContext->unk_54 = 0;
        mSaveContext->savedSceneNum = SCENE_LINKS_HOUSE;

        // Equipment
        for (int button = 0; button < ARRAY_COUNT(mSaveContext->equips.buttonItems); button++) {
            mSaveContext->equips.buttonItems[button] = ITEM_NONE;
        }
        for (int button = 0; button < ARRAY_COUNT(mSaveContext->equips.cButtonSlots); button++) {
            mSaveContext->equips.cButtonSlots[button] = SLOT_NONE;
        }
        mSaveContext->equips.equipment = 0;

        // Inventory
        for (int item = 0; item < ARRAY_COUNT(mSaveContext->inventory.items); item++) {
            mSaveContext->inventory.items[item] = ITEM_NONE;
        }
        for (int ammo = 0; ammo < ARRAY_COUNT(mSaveContext->inventory.ammo); ammo++) {
            mSaveContext->inventory.ammo[ammo] = 0;
        }
        mSaveContext->inventory.equipment = 0;
        mSaveContext->inventory.upgrades = 0;
        mSaveContext->inventory.questItems = 0;
        for (int dungeon = 0; dungeon < ARRAY_COUNT(mSaveContext->inventory.dungeonItems); dungeon++) {
            mSaveContext->inventory.dungeonItems[dungeon] = 0;
        }
        for (int dungeon = 0; dungeon < ARRAY_COUNT(mSaveContext->inventory.dungeonKeys); dungeon++) {
            mSaveContext->inventory.dungeonKeys[dungeon] = 0x0;
        }
        mSaveContext->inventory.defenseHearts = 0;
        mSaveContext->inventory.gsTokens = 0;
        for (int scene = 0; scene < ARRAY_COUNT(mSaveContext->sceneFlags); scene++) {
            mSaveContext->sceneFlags[scene].chest = 0;
            mSaveContext->sceneFlags[scene].swch = 0;
            mSaveContext->sceneFlags[scene].clear = 0;
            mSaveContext->sceneFlags[scene].collect = 0;
            mSaveContext->sceneFlags[scene].unk = 0;
            mSaveContext->sceneFlags[scene].rooms = 0;
            mSaveContext->sceneFlags[scene].floors = 0;
        }
        mSaveContext->fw.pos.x = 0;
        mSaveContext->fw.pos.y = 0;
        mSaveContext->fw.pos.z = 0;
        mSaveContext->fw.yaw = 0;
        mSaveContext->fw.playerParams = 0;
        mSaveContext->fw.entranceIndex = 0;
        mSaveContext->fw.roomIndex = 0;
        mSaveContext->fw.set = 0;
        mSaveContext->fw.tempSwchFlags = 0;
        mSaveContext->fw.tempCollectFlags = 0;
        for (int flag = 0; flag < ARRAY_COUNT(mSaveContext->gsFlags); flag++) {
            mSaveContext->gsFlags[flag] = 0;
        }
        for (int highscore = 0; highscore < ARRAY_COUNT(mSaveContext->highScores); highscore++) {
            mSaveContext->highScores[highscore] = 0;
        }
        for (int flag = 0; flag < ARRAY_COUNT(mSaveContext->eventChkInf); flag++) {
            mSaveContext->eventChkInf[flag] = 0;
        }
        for (int flag = 0; flag < ARRAY_COUNT(mSaveContext->itemGetInf); flag++) {
            mSaveContext->itemGetInf[flag] = 0;
        }
        for (int flag = 0; flag < ARRAY_COUNT(mSaveContext->infTable); flag++) {
            mSaveContext->infTable[flag] = 0;
        }
        mSaveContext->worldMapAreaData = 0;
        mSaveContext->scarecrowLongSongSet = 0;
        for (int i = 0; i < ARRAY_COUNT(mSaveContext->scarecrowLongSong); i++) {
            mSaveContext->scarecrowLongSong[i].noteIdx = 0;
            mSaveContext->scarecrowLongSong[i].unk_01 = 0;
            mSaveContext->scarecrowLongSong[i].unk_02 = 0;
            mSaveContext->scarecrowLongSong[i].volume = 0;
            mSaveContext->scarecrowLongSong[i].vibrato = 0;
            mSaveContext->scarecrowLongSong[i].tone = 0;
            mSaveContext->scarecrowLongSong[i].semitone = 0;
        }
        mSaveContext->scarecrowSpawnSongSet = 0;
        for (int i = 0; i < ARRAY_COUNT(mSaveContext->scarecrowSpawnSong); i++) {
            mSaveContext->scarecrowSpawnSong[i].noteIdx = 0;
            mSaveContext->scarecrowSpawnSong[i].unk_01 = 0;
            mSaveContext->scarecrowSpawnSong[i].unk_02 = 0;
            mSaveContext->scarecrowSpawnSong[i].volume = 0;
            mSaveContext->scarecrowSpawnSong[i].vibrato = 0;
            mSaveContext->scarecrowSpawnSong[i].tone = 0;
            mSaveContext->scarecrowSpawnSong[i].semitone = 0;
        }

        mSaveContext->horseData.scene = SCENE_HYRULE_FIELD;
        mSaveContext->horseData.pos.x = -1840;
        mSaveContext->horseData.pos.y = 72;
        mSaveContext->horseData.pos.z = 5497;
        mSaveContext->horseData.angle = -0x6AD9;
        mSaveContext->magicLevel = 0;
        mSaveContext->infTable[29] = 1;
        mSaveContext->sceneFlags[5].swch = 0x40000000;

        // SoH specific
        mSaveContext->backupFW = mSaveContext->fw;
        mSaveContext->pendingSale = ITEM_NONE;
        mSaveContext->pendingSaleMod = MOD_NONE;
        mSaveContext->isBossRushPaused = 0;
        mSaveContext->pendingIceTrapCount = 0;

        // Init with normal quest unless only an MQ rom is provided
        mSaveContext->questId = OTRGlobals::Instance->HasOriginal() ? QUEST_NORMAL : QUEST_MASTER;

        //RANDOTODO (ADD ITEMLOCATIONS TO GSAVECONTEXT)
    }

    void Logic::NewSaveContext() {
        if (mSaveContext != nullptr && mSaveContext != &gSaveContext) {
            free(mSaveContext);
        }
        mSaveContext = new SaveContext();
        InitSaveContext();
    }

    uint8_t Logic::InventorySlot(uint32_t item) {
        return gItemSlots[item];
    }

    uint32_t Logic::CurrentUpgrade(uint32_t upgrade) {
        return (mSaveContext->inventory.upgrades & gUpgradeMasks[upgrade]) >> gUpgradeShifts[upgrade];
    }

    uint32_t Logic::CurrentInventory(uint32_t item) {
        return mSaveContext->inventory.items[InventorySlot(item)];
    }

    void Logic::SetUpgrade(uint32_t upgrade, uint8_t level) {
        mSaveContext->inventory.upgrades &= gUpgradeNegMasks[upgrade];
        mSaveContext->inventory.upgrades |= level << gUpgradeShifts[upgrade];
    }

    bool Logic::CheckInventory(uint32_t item, bool exact) {
        auto current = mSaveContext->inventory.items[InventorySlot(item)];
        return exact ? (current == item) : (current != ITEM_NONE);
    }

    void Logic::SetInventory(uint32_t itemSlot, uint32_t item) {
        mSaveContext->inventory.items[InventorySlot(itemSlot)] = item;
    }

    bool Logic::CheckEquipment(uint32_t equipFlag) {
        return (equipFlag & mSaveContext->inventory.equipment);
    }

    bool Logic::CheckQuestItem(uint32_t item) {
        return ((1 << item) & mSaveContext->inventory.questItems);
    }

    bool Logic::HasAdultTrade(uint32_t itemID) {
        int tradeIndex = itemID - ITEM_POCKET_EGG;
        return mSaveContext->adultTradeItems & (1 << tradeIndex);
    }

    void Logic::SetAdultTrade(uint32_t itemID, bool state) {
        int tradeIndex = itemID - ITEM_POCKET_EGG;
        if (!state) {
            mSaveContext->adultTradeItems &= ~(1 << tradeIndex);
        }
        else {
            mSaveContext->adultTradeItems |= (1 << tradeIndex);
        }
    }

    void Logic::SetQuestItem(uint32_t item, bool state) {
        if (!state) {
            mSaveContext->inventory.questItems &= ~(1 << item);
        }
        else {
            mSaveContext->inventory.questItems |= (1 << item);
        }
    }

    uint8_t Logic::GetSmallKeyCount(uint32_t dungeonIndex) {
        return mSaveContext->inventory.dungeonKeys[dungeonIndex];
    }

    void Logic::SetSmallKeyCount(uint32_t dungeonIndex, uint8_t count) {
        mSaveContext->inventory.dungeonKeys[dungeonIndex] = count;
    }

    bool Logic::CheckDungeonItem(uint32_t item, uint32_t dungeonIndex) {
        return mSaveContext->inventory.dungeonItems[dungeonIndex] & gBitFlags[item];
    }

    void Logic::SetDungeonItem(uint32_t item, uint32_t dungeonIndex, bool state) {
        if (!state) {
            mSaveContext->inventory.dungeonItems[dungeonIndex] &= ~gBitFlags[item];
        }
        else {
            mSaveContext->inventory.dungeonItems[dungeonIndex] |= gBitFlags[item];
        }
    }

    bool Logic::CheckRandoInf(uint32_t flag) {
        return mSaveContext->randomizerInf[flag >> 4] & (1 << (flag & 0xF));
    }

    void Logic::SetRandoInf(uint32_t flag, bool state) {
        if (!state) {
            mSaveContext->randomizerInf[flag >> 4] &= ~(1 << (flag & 0xF));
        }
        else {
            mSaveContext->randomizerInf[flag >> 4] |= (1 << (flag & 0xF));
        }
    }

    bool Logic::CheckEventChkInf(int32_t flag) {
        return mSaveContext->eventChkInf[flag >> 4] & (1 << (flag & 0xF));
    }

    void Logic::SetEventChkInf(int32_t flag, bool state) {
        if (!state) {
            mSaveContext->eventChkInf[flag >> 4] &= ~(1 << (flag & 0xF));
        }
        else {
            mSaveContext->eventChkInf[flag >> 4] |= (1 << (flag & 0xF));
        }
    }

    uint8_t Logic::GetGSCount() {
        return mSaveContext->inventory.gsTokens;
    }

    uint8_t Logic::GetAmmo(uint32_t item) {
        return mSaveContext->inventory.ammo[gItemSlots[item]];
    }

    void Logic::SetAmmo(uint32_t item, uint8_t count) {
        mSaveContext->inventory.ammo[gItemSlots[item]] = count;
    }

    void Logic::SetContext(std::shared_ptr<Context> _ctx) {
        ctx = _ctx;
    }

    bool Logic::GetInLogic(LogicVal logicVal) {
        return inLogic[logicVal];
    }

    void Logic::SetInLogic(LogicVal logicVal, bool value) {
        inLogic[logicVal] = value;
    }

    void Logic::Reset() {
        NewSaveContext();
        StartPerformanceTimer(PT_LOGIC_RESET);
        memset(inLogic, false, sizeof(inLogic));
        //Settings-dependent variables
        IsKeysanity = ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_ANYWHERE) || 
                    ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_ANYWHERE) || 
                    ctx->GetOption(RSK_KEYSANITY).Is(RO_DUNGEON_ITEM_LOC_ANYWHERE);
        AmmoCanDrop = /*AmmoDrops.IsNot(AMMODROPS_NONE) TODO: AmmoDrop setting*/ true;

        //Child item logic
        SkullMask     = false;
        MaskOfTruth   = false;

        //Adult logic
        FreedEpona         = false;
        //BigPoe        = false;

        //Trade Quest Events
        WakeUpAdultTalon   = false;

        //Dungeon Clears
        DekuTreeClear       = false;
        DodongosCavernClear = false;
        JabuJabusBellyClear = false;
        ForestTempleClear   = false;
        FireTempleClear     = false;
        WaterTempleClear    = false;
        SpiritTempleClear   = false;
        ShadowTempleClear   = false;

        //Trial Clears
        ForestTrialClear = false;
        FireTrialClear   = false;
        WaterTrialClear  = false;
        SpiritTrialClear = false;
        ShadowTrialClear = false;
        LightTrialClear  = false;

        //Ocarina C Buttons
        bool ocBtnShuffle = ctx->GetOption(RSK_SHUFFLE_OCARINA_BUTTONS).Is(true);
        SetRandoInf(RAND_INF_HAS_OCARINA_A, !ocBtnShuffle);
        SetRandoInf(RAND_INF_HAS_OCARINA_C_UP, !ocBtnShuffle);
        SetRandoInf(RAND_INF_HAS_OCARINA_C_DOWN, !ocBtnShuffle);
        SetRandoInf(RAND_INF_HAS_OCARINA_C_LEFT, !ocBtnShuffle);
        SetRandoInf(RAND_INF_HAS_OCARINA_C_RIGHT, !ocBtnShuffle);

        //Progressive Items
        SetUpgrade(UPG_STICKS, ctx->GetOption(RSK_SHUFFLE_DEKU_STICK_BAG).Is(true) ? 0 : 1);
        SetUpgrade(UPG_NUTS, ctx->GetOption(RSK_SHUFFLE_DEKU_NUT_BAG).Is(true) ? 0 : 1);

        //If we're not shuffling swim, we start with it
        if (ctx->GetOption(RSK_SHUFFLE_SWIM).Is(false)) {
            SetRandoInf(RAND_INF_CAN_SWIM, true);
        }

        //If we're not shuffling child's wallet, we start with it
        if (ctx->GetOption(RSK_SHUFFLE_CHILD_WALLET).Is(false)) {
            SetRandoInf(RAND_INF_HAS_WALLET, true);
        }

        //If we're not shuffling fishing pole, we start with it
        if (ctx->GetOption(RSK_SHUFFLE_FISHING_POLE).Is(false)) {
            SetRandoInf(RAND_INF_FISHING_POLE_FOUND, true);
        }

        //If not keysanity, start with 1 logical key to account for automatically unlocking the basement door in vanilla FiT
        if (!IsKeysanity && ctx->GetDungeon(Rando::FIRE_TEMPLE)->IsVanilla()) {
            SetSmallKeyCount(SCENE_FIRE_TEMPLE, 1);
        }

        //Bottle Count
        Bottles    = 0;
        NumBottles = 0;
        CanEmptyBigPoes = true;

        //Drops and Bottle Contents Access
        NutPot           = false;
        NutCrate         = false;
        DekuBabaNuts     = false;
        StickPot         = false;
        DekuBabaSticks   = false;
        BugShrub         = false;
        WanderingBugs    = false;
        BugRock          = false;
        BlueFireAccess   = false;
        FishGroup        = false;
        LoneFish         = false;
        GossipStoneFairy = false;
        BeanPlantFairy   = false;
        ButterflyFairy   = false;
        FairyPot         = false;
        FreeFairies      = false;
        FairyPond        = false;

        PieceOfHeart     = 0;
        HeartContainer   = 0;

        /* --- HELPERS, EVENTS, AND LOCATION ACCESS --- */
        /* These are used to simplify reading the logic, but need to be updated
        /  every time a base value is updated.                       */

        ChildScarecrow   = false;
        AdultScarecrow   = false;

        CouldPlayBowling = false;
        IsChild          = false;
        IsAdult          = false;
        //CanPlantBean        = false;
        BigPoeKill            = false;

        BaseHearts      = ctx->GetOption(RSK_STARTING_HEARTS).Value<uint8_t>() + 1;
        

        //Bridge Requirements
        BuiltRainbowBridge    = false;

        //Other
        AtDay         = false;
        AtNight       = false;
        GetSaveContext()->linkAge = !ctx->GetSettings()->ResolvedStartingAge();

        //Events
        ShowedMidoSwordAndShield  = false;
        CarpenterRescue           = false;
        GF_GateOpen               = false;
        GtG_GateOpen              = false;
        DampesWindmillAccess      = false;
        DrainWell                 = false;
        GoronCityChildFire        = false;
        GCWoodsWarpOpen           = false;
        GCDaruniasDoorOpenChild   = false;
        StopGCRollingGoronAsAdult = false;
        WaterTempleLow            = false;
        WaterTempleMiddle         = false;
        WaterTempleHigh           = false;
        KakarikoVillageGateOpen   = false;
        KingZoraThawed            = false;
        ForestTempleJoelle        = false;
        ForestTempleBeth          = false;
        ForestTempleAmy           = false;
        ForestTempleMeg           = false;
        FireLoopSwitch            = false;
        LinksCow                  = false;
        DeliverLetter             = false;
        ClearMQDCUpperLobbyRocks  = false;
        LoweredWaterInsideBotw    = false;
        OpenedWestRoomMQBotw      = false;
        OpenedMiddleHoleMQBotw    = false;
        BrokeDeku1FWeb            = false;
        ClearedMQDekuSERoom       = false;
        MQDekuWaterRoomTorches    = false;
        PushedDekuBasementBlock   = false;
        OpenedLowestGoronCage     = false;
        OpenedUpperFireShortcut   = false;
        HitFireTemplePlatform     = false;
        OpenedFireMQFireMazeDoor  = false;
        MQForestBlockRoomTargets  = false;
        ForestCanTwistHallway     = false;
        ForestClearBelowBowChest  = false;
        ForestOpenBossCorridor    = false;

        StopPerformanceTimer(PT_LOGIC_RESET);
    }
}
