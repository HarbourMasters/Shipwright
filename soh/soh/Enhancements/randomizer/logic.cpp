#include "logic.h"

#include <algorithm>
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>

#include "dungeon.h"
#include "context.h"

namespace Rando {
    bool Logic::CanPlay(bool song) {
        return Ocarina && song;
    }

    bool Logic::IsMagicItem(RandomizerGet item) {
        return item == RG_DINS_FIRE    ||
                item == RG_FARORES_WIND ||
                item == RG_NAYRUS_LOVE  ||
                item == RG_LENS_OF_TRUTH;
    }

    bool Logic::IsMagicArrow(RandomizerGet item) {
        return item == RG_FIRE_ARROWS ||
                item == RG_ICE_ARROWS  ||
                item == RG_LIGHT_ARROWS;
    }

    bool Logic::HasItem(RandomizerGet itemName) {
        return (itemName == RG_DINS_FIRE         && DinsFire)        ||
               (itemName == RG_FARORES_WIND      && FaroresWind)     ||
               (itemName == RG_NAYRUS_LOVE       && NayrusLove)      ||
               (itemName == RG_LENS_OF_TRUTH     && LensOfTruth)     ||
               (itemName == RG_FAIRY_BOW         && Bow)             ||
               (itemName == RG_MEGATON_HAMMER    && Hammer)          ||
               (itemName == RG_IRON_BOOTS        && IronBoots)       ||
               (itemName == RG_HOVER_BOOTS       && HoverBoots)      ||
               (itemName == RG_HOOKSHOT          && Hookshot)        ||
               (itemName == RG_LONGSHOT          && Longshot)        ||
               (itemName == RG_SILVER_GAUNTLETS  && SilverGauntlets) ||
               (itemName == RG_GOLDEN_GAUNTLETS  && GoldenGauntlets) ||
               (itemName == RG_GORON_TUNIC       && GoronTunic)      ||
               (itemName == RG_ZORA_TUNIC        && ZoraTunic)       ||
               (itemName == RG_SCARECROW         && Scarecrow)       ||
               (itemName == RG_DISTANT_SCARECROW && DistantScarecrow)||
               (itemName == RG_HYLIAN_SHIELD     && HylianShield)    ||
               (itemName == RG_MIRROR_SHIELD     && MirrorShield)    ||
               (itemName == RG_MASTER_SWORD      && MasterSword)     ||
               (itemName == RG_BIGGORON_SWORD    && BiggoronSword)   ||
               (itemName == RG_FAIRY_SLINGSHOT   && Slingshot)       ||
               (itemName == RG_BOOMERANG         && Boomerang)       ||
               (itemName == RG_KOKIRI_SWORD      && KokiriSword)     ||
               (itemName == RG_STICKS            && Sticks)          ||
               (itemName == RG_DEKU_SHIELD       && DekuShield)      ||
               (itemName == RG_FIRE_ARROWS       && FireArrows)      ||
               (itemName == RG_ICE_ARROWS        && IceArrows)       ||
               (itemName == RG_LIGHT_ARROWS      && LightArrows);
    }

    //Can the passed in item be used?
    bool Logic::CanUse(RandomizerGet itemName) {
        if (!HasItem(itemName))
            return false;

        switch (itemName) {
            // Adult items
            // TODO: Uncomment those if we ever implement more item usability settings
            case RG_FAIRY_BOW:
                return IsAdult;// || BowAsChild;
            case RG_MEGATON_HAMMER:
                return IsAdult;// || HammerAsChild;
            case RG_IRON_BOOTS:
                return IsAdult;// || IronBootsAsChild;
            case RG_HOVER_BOOTS:
                return IsAdult;// || HoverBootsAsChild;
            case RG_HOOKSHOT:
                return IsAdult;// || HookshotAsChild;
            case RG_LONGSHOT:
                return IsAdult;// || HookshotAsChild;
            case RG_SILVER_GAUNTLETS:
                return IsAdult;
            case RG_GOLDEN_GAUNTLETS:
                return IsAdult;
            case RG_GORON_TUNIC:
                return IsAdult;// || GoronTunicAsChild;
            case RG_ZORA_TUNIC:
                return IsAdult;// || ZoraTunicAsChild;
            case RG_SCARECROW:
                return IsAdult;// || HookshotAsChild;
            case RG_DISTANT_SCARECROW:
                return IsAdult;// || HookshotAsChild;
            case RG_HYLIAN_SHIELD:
                return IsAdult;
            case RG_MIRROR_SHIELD:
                return IsAdult;// || MirrorShieldAsChild;
            case RG_MASTER_SWORD:
                return IsAdult;// || MasterSwordAsChild;
            case RG_BIGGORON_SWORD:
                return IsAdult;// || BiggoronSwordAsChild;

            // Child items
            case RG_FAIRY_SLINGSHOT:
                return IsChild;// || SlingshotAsAdult;
            case RG_BOOMERANG:
                return IsChild;// || BoomerangAsAdult;
            case RG_KOKIRI_SWORD:
                return IsChild;// || KokiriSwordAsAdult;
            case RG_STICKS:
                return IsChild;// || StickAsAdult;
            case RG_DEKU_SHIELD:
                return IsChild;// || DekuShieldAsAdult;

            // Magic items
            default:
                return MagicMeter && (IsMagicItem(itemName) || (IsMagicArrow(itemName) && CanUse(RG_FAIRY_BOW)));
        }
    }

    bool Logic::HasProjectile(HasProjectileAge age) {
        return HasExplosives ||
               (age == HasProjectileAge::Child  && (Slingshot || Boomerang)) ||
               (age == HasProjectileAge::Adult  && (Hookshot  || Bow      )) ||
               (age == HasProjectileAge::Both   && (Slingshot || Boomerang)  && (Hookshot || Bow)) ||
               (age == HasProjectileAge::Either && (Slingshot || Boomerang   ||  Hookshot || Bow));
    }

    bool Logic::HasBossSoul(RandomizerGet itemName) {
        auto ctx = Rando::Context::GetInstance();
        if (!ctx->GetOption(RSK_SHUFFLE_BOSS_SOULS)) {
            return true;
        }
        switch(itemName) {
            case RG_GOHMA_SOUL:
                return CanSummonGohma;
            case RG_KING_DODONGO_SOUL:
                return CanSummonKingDodongo;
            case RG_BARINADE_SOUL:
                return CanSummonBarinade;
            case RG_PHANTOM_GANON_SOUL:
                return CanSummonPhantomGanon;
            case RG_VOLVAGIA_SOUL:
                return CanSummonVolvagia;
            case RG_MORPHA_SOUL:
                return CanSummonMorpha;
            case RG_BONGO_BONGO_SOUL:
                return CanSummonBongoBongo;
            case RG_TWINROVA_SOUL:
                return CanSummonTwinrova;
            case RG_GANON_SOUL:
                return ctx->GetOption(RSK_SHUFFLE_BOSS_SOULS).Is(RO_BOSS_SOULS_ON_PLUS_GANON) ? CanSummonGanon : true;
            default:
                break;
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
                    HasItem(RG_FAIRY_SLINGSHOT) || HasItem(RG_BOOMERANG) || HasBottle || Nuts || Ocarina || HasItem(RG_LENS_OF_TRUTH) || HasExplosives ||
                    (MagicBean || MagicBeanPack) || HasItem(RG_DINS_FIRE) || HasItem(RG_FARORES_WIND) || HasItem(RG_NAYRUS_LOVE)))) && false; //GlitchEquipSwap;
        }

        //Shouldn't be reached
        return false;
    }

    Logic::Logic() {}

    // Updates all logic helpers. Should be called whenever a non-helper is changed
    void Logic::UpdateHelpers() {
        auto ctx = Rando::Context::GetInstance();
        NumBottles      = ((NoBottles) ? 0 : (Bottles + ((DeliverLetter) ? 1 : 0)));
        HasBottle       = NumBottles >= 1;
        Slingshot       = (ProgressiveBulletBag >= 1) && (BuySeed || AmmoCanDrop);
        Ocarina         = ProgressiveOcarina    >= 1;
        OcarinaOfTime   = ProgressiveOcarina    >= 2;
        MagicMeter      = (ProgressiveMagic     >= 1) && (AmmoCanDrop || (HasBottle && BuyMagicPotion));
        BombBag         = (ProgressiveBombBag   >= 1) && (BuyBomb || AmmoCanDrop);
        Hookshot        = ProgressiveHookshot   >= 1;
        Longshot        = ProgressiveHookshot   >= 2;
        Bow             = (ProgressiveBow       >= 1) && (BuyArrow || AmmoCanDrop);
        GoronBracelet   = ProgressiveStrength   >= 1;
        SilverGauntlets = ProgressiveStrength   >= 2;
        GoldenGauntlets = ProgressiveStrength   >= 3;
        SilverScale     = ProgressiveScale      >= 1;
        GoldScale       = ProgressiveScale      >= 2;
        AdultsWallet    = ProgressiveWallet     >= 1;
        BiggoronSword   = BiggoronSword || ProgressiveGiantKnife >= 2;

        ScarecrowSong    = ScarecrowSong || ctx->GetOption(RSK_SKIP_SCARECROWS_SONG) || (ChildScarecrow && AdultScarecrow);
        Scarecrow        = Hookshot && CanPlay(ScarecrowSong);
        DistantScarecrow = Longshot && CanPlay(ScarecrowSong);

        //Drop Access
        DekuStickDrop = StickPot || DekuBabaSticks;
        DekuNutDrop   = (NutPot  || NutCrate         || DekuBabaNuts) && AmmoCanDrop;
        BugsAccess    = BugShrub || WanderingBugs    || BugRock;
        FishAccess    = LoneFish || FishGroup;
        FairyAccess   = FairyPot || GossipStoneFairy || BeanPlantFairy || ButterflyFairy || FreeFairies || FairyPond;


        //refills
        Bombs        = BombBag;
        Nuts         = DekuNutDrop || Nuts;
        Sticks       = DekuStickDrop || Sticks;
        Bugs         = HasBottle && BugsAccess;
        BlueFire     = (HasBottle && BlueFireAccess) || (ctx->GetOption(RSK_BLUE_FIRE_ARROWS) && CanUse(RG_ICE_ARROWS));
        Fish         = HasBottle && FishAccess;
        Fairy        = HasBottle && FairyAccess;

        FoundBombchus   = (BombchuDrop || Bombchus || Bombchus5 || Bombchus10 || Bombchus20);
        CanPlayBowling  = (ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && FoundBombchus) || (!ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && BombBag);
        // TODO: Implement Ammo Drop Setting in place of bombchu drops
        HasBombchus     = (BuyBombchus || (ctx->GetOption(RSK_ENABLE_BOMBCHU_DROPS).Is(RO_AMMO_DROPS_ON_PLUS_BOMBCHU) && FoundBombchus));

        HasExplosives =  Bombs || (ctx->GetOption(RSK_BOMBCHUS_IN_LOGIC) && HasBombchus);

        HasBoots = IronBoots || HoverBoots;

        //Unshuffled adult trade quest
        Eyedrops     = Eyedrops     || (!ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE) && ClaimCheck);
        EyeballFrog  = EyeballFrog  || (!ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE) && Eyedrops);
        Prescription = Prescription || (!ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE) && EyeballFrog);
        BrokenSword  = BrokenSword  || (!ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE) && Prescription);
        PoachersSaw  = PoachersSaw  || (!ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE) && BrokenSword);
        OddPoultice  = OddPoultice  || (!ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE) && PoachersSaw);
        OddMushroom  = OddMushroom  || (!ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE) && OddPoultice);
        Cojiro       = Cojiro       || (!ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE) && OddMushroom);
        PocketEgg    = PocketEgg    || (!ctx->GetOption(RSK_SHUFFLE_ADULT_TRADE) && Cojiro);

        // IsChild = Age == AGE_CHILD;
        // IsAdult = Age == AGE_ADULT;

        CanBlastOrSmash = HasExplosives || CanUse(RG_MEGATON_HAMMER);
        CanChildAttack  = IsChild && (Slingshot || Boomerang || Sticks || KokiriSword || HasExplosives || CanUse(RG_DINS_FIRE) || CanUse(RG_MASTER_SWORD) || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_BIGGORON_SWORD));
        CanChildDamage  = IsChild && (Slingshot ||              Sticks || KokiriSword || HasExplosives || CanUse(RG_DINS_FIRE) || CanUse(RG_MASTER_SWORD) || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_BIGGORON_SWORD));
        CanAdultAttack  = IsAdult && (CanUse(RG_FAIRY_BOW) || CanUse(RG_BOOMERANG)       || CanUse(RG_STICKS) || CanUse(RG_KOKIRI_SWORD) || HasExplosives || CanUse(RG_DINS_FIRE) || MasterSword || Hammer || BiggoronSword || Hookshot);
        CanAdultDamage  = IsAdult && (CanUse(RG_FAIRY_BOW) || CanUse(RG_STICKS)          || CanUse(RG_KOKIRI_SWORD) || HasExplosives || CanUse(RG_DINS_FIRE) || MasterSword || Hammer || BiggoronSword);
        CanStunDeku     = CanAdultAttack || CanChildAttack || Nuts || HasShield;
        CanCutShrubs    = CanUse(RG_KOKIRI_SWORD) || CanUse(RG_BOOMERANG) || HasExplosives || CanUse(RG_MASTER_SWORD) || CanUse(RG_MEGATON_HAMMER) || CanUse(RG_BIGGORON_SWORD);
        CanDive         = ProgressiveScale >= 1;
        CanLeaveForest  = ctx->GetOption(RSK_FOREST).IsNot(RO_FOREST_CLOSED) || IsAdult || DekuTreeClear || ctx->GetOption(RSK_SHUFFLE_INTERIOR_ENTRANCES) || ctx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES);
        CanPlantBugs    = IsChild && Bugs;
        CanRideEpona    = IsAdult && Epona && CanPlay(EponasSong);
        CanSummonGossipFairy            = Ocarina && (ZeldasLullaby || EponasSong || SongOfTime || SunsSong);
        CanSummonGossipFairyWithoutSuns = Ocarina && (ZeldasLullaby || EponasSong || SongOfTime);
        Hearts          = BaseHearts + HeartContainer + (PieceOfHeart >> 2);
        EffectiveHealth = ((Hearts << (2 + DoubleDefense)) >> Multiplier) + ((Hearts << (2 + DoubleDefense)) % (1 << Multiplier) > 0); //Number of half heart hits to die, ranges from 1 to 160
        FireTimer       = CanUse(RG_GORON_TUNIC) ? 255 : (ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS)) ? (Hearts * 8) : 0;
        WaterTimer      = CanUse(RG_ZORA_TUNIC) ? 255 : (ctx->GetTrickOption(RT_FEWER_TUNIC_REQUIREMENTS)) ? (Hearts * 8) : 0;
        NeedNayrusLove      = (EffectiveHealth == 1);
        CanSurviveDamage    = !NeedNayrusLove || CanUse(RG_NAYRUS_LOVE);
        CanTakeDamage       = Fairy || CanSurviveDamage;
        CanTakeDamageTwice  = (Fairy && NumBottles >= 2) || ((EffectiveHealth == 2) && (CanUse(RG_NAYRUS_LOVE) || Fairy)) || (EffectiveHealth > 2);
        //CanPlantBean        = IsChild && (MagicBean || MagicBeanPack);
        CanOpenBombGrotto   = CanBlastOrSmash       && (ShardOfAgony || ctx->GetTrickOption(RT_GROTTOS_WITHOUT_AGONY));
        CanOpenStormGrotto  = CanPlay(SongOfStorms) && (ShardOfAgony || ctx->GetTrickOption(RT_GROTTOS_WITHOUT_AGONY));
        HookshotOrBoomerang = CanUse(RG_HOOKSHOT) || CanUse(RG_BOOMERANG);
        CanGetNightTimeGS   = (CanPlay(SunsSong) || !ctx->GetOption(RSK_SKULLS_SUNS_SONG));

        GuaranteeTradePath     = ctx->GetOption(RSK_SHUFFLE_INTERIOR_ENTRANCES) || ctx->GetOption(RSK_SHUFFLE_OVERWORLD_ENTRANCES) || ctx->GetTrickOption(RT_DMT_BOLERO_BIGGORON) || CanBlastOrSmash || StopGCRollingGoronAsAdult;
        //GuaranteeHint          = (hints == "Mask" && MaskofTruth) || (hints == "Agony") || (hints != "Mask" && hints != "Agony");
        HasFireSource          = CanUse(RG_DINS_FIRE) || CanUse(RG_FIRE_ARROWS);
        HasFireSourceWithTorch = HasFireSource || CanUse(RG_STICKS);

        //Gerudo Fortress
        CanFinishGerudoFortress = (ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_NORMAL)    && GerudoFortressKeys >= 4 && (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD)) && (GerudoToken || CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || CanUse(RG_HOVER_BOOTS) || ctx->GetTrickOption(RT_GF_KITCHEN))) ||
                                    (ctx->GetOption(RSK_GERUDO_FORTRESS).Is(RO_GF_FAST)      && GerudoFortressKeys >= 1 && (CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD))) ||
                                    (ctx->GetOption(RSK_GERUDO_FORTRESS).IsNot(RO_GF_NORMAL) && ctx->GetOption(RSK_GERUDO_FORTRESS).IsNot(RO_GF_FAST));

        HasShield          = CanUse(RG_HYLIAN_SHIELD) || CanUse(RG_DEKU_SHIELD); //Mirror shield can't reflect attacks
        CanShield          = CanUse(RG_MIRROR_SHIELD) || HasShield;
        ChildShield        = IsChild && CanUse(RG_DEKU_SHIELD); //hylian shield is not helpful for child
        AdultReflectShield = IsAdult && CanUse(RG_HYLIAN_SHIELD); //Mirror shield can't reflect attacks
        AdultShield        = IsAdult && (CanUse(RG_HYLIAN_SHIELD) || CanUse(RG_MIRROR_SHIELD));
        CanShieldFlick     = ChildShield || AdultShield;
        CanJumpslash       = CanUse(RG_STICKS) || CanUse(RG_KOKIRI_SWORD) || CanUse(RG_MASTER_SWORD) || CanUse(RG_BIGGORON_SWORD); // Not including hammer as hammer jump attacks can be weird
        CanUseProjectile   = HasExplosives || CanUse(RG_FAIRY_BOW) || CanUse(RG_HOOKSHOT) || CanUse(RG_FAIRY_SLINGSHOT) || CanUse(RG_BOOMERANG);
        CanUseMagicArrow   = CanUse(RG_FIRE_ARROWS) || CanUse(RG_ICE_ARROWS) || CanUse(RG_LIGHT_ARROWS);

        //Bridge and LACS Requirements
        MedallionCount        = (ForestMedallion ? 1:0) + (FireMedallion ? 1:0) + (WaterMedallion ? 1:0) + (SpiritMedallion ? 1:0) + (ShadowMedallion ? 1:0) + (LightMedallion ? 1:0);
        StoneCount            = (KokiriEmerald ? 1:0) + (GoronRuby ? 1:0) + (ZoraSapphire ? 1:0);
        DungeonCount          = (DekuTreeClear ? 1:0) + (DodongosCavernClear ? 1:0) + (JabuJabusBellyClear ? 1:0) + (ForestTempleClear ? 1:0) + (FireTempleClear ? 1:0) + (WaterTempleClear ? 1:0) + (SpiritTempleClear ? 1:0) + (ShadowTempleClear ? 1:0);
        HasAllStones          = StoneCount == 3;
        HasAllMedallions      = MedallionCount == 6;
        GregInBridgeLogic     = ctx->GetOption(RSK_BRIDGE_OPTIONS).Is(RO_BRIDGE_GREG);
        GregInLacsLogic       = ctx->GetOption(RSK_LACS_OPTIONS).Is(RO_LACS_GREG_REWARD);

        CanBuildRainbowBridge = ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_ALWAYS_OPEN)                                                                         ||
                                (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_VANILLA)    && ShadowMedallion && SpiritMedallion && LightArrows)             ||
                                (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_STONES)     && StoneCount + (Greg && GregInBridgeLogic ? 1 : 0) >= ctx->GetOption(RSK_RAINBOW_BRIDGE_STONE_COUNT).Value<uint8_t>())                    ||
                                (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_MEDALLIONS) && MedallionCount + (Greg && GregInBridgeLogic ? 1 : 0) >= ctx->GetOption(RSK_RAINBOW_BRIDGE_MEDALLION_COUNT).Value<uint8_t>())            ||
                                (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEON_REWARDS)    && StoneCount + MedallionCount + (Greg && GregInBridgeLogic ? 1 : 0) >= ctx->GetOption(RSK_RAINBOW_BRIDGE_REWARD_COUNT).Value<uint8_t>())  ||
                                (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_DUNGEONS)   && DungeonCount + (Greg && GregInBridgeLogic ? 1 : 0) >= ctx->GetOption(RSK_RAINBOW_BRIDGE_DUNGEON_COUNT).Value<uint8_t>())                ||
                                (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_TOKENS)     && GoldSkulltulaTokens >= ctx->GetOption(RSK_RAINBOW_BRIDGE_TOKEN_COUNT).Value<uint8_t>()) ||
                                (ctx->GetOption(RSK_RAINBOW_BRIDGE).Is(RO_BRIDGE_GREG)       && Greg);

        CanTriggerLACS = (ctx->GetSettings()->LACSCondition() == RO_LACS_VANILLA    && ShadowMedallion && SpiritMedallion)                          ||
                            (ctx->GetSettings()->LACSCondition() == RO_LACS_STONES     && StoneCount + (Greg && GregInLacsLogic ? 1 : 0) >= ctx->GetOption(RSK_LACS_STONE_COUNT).Value<uint8_t>())                    ||
                            (ctx->GetSettings()->LACSCondition() == RO_LACS_MEDALLIONS && MedallionCount + (Greg && GregInLacsLogic ? 1 : 0) >= ctx->GetOption(RSK_LACS_MEDALLION_COUNT).Value<uint8_t>())            ||
                            (ctx->GetSettings()->LACSCondition() == RO_LACS_REWARDS    && StoneCount + MedallionCount + (Greg && GregInLacsLogic ? 1 : 0) >= ctx->GetOption(RSK_LACS_REWARD_COUNT).Value<uint8_t>())  ||
                            (ctx->GetSettings()->LACSCondition() == RO_LACS_DUNGEONS   && DungeonCount + (Greg && GregInLacsLogic ? 1 : 0) >= ctx->GetOption(RSK_LACS_DUNGEON_COUNT).Value<uint8_t>())                ||
                            (ctx->GetSettings()->LACSCondition() == RO_LACS_TOKENS     && GoldSkulltulaTokens >= ctx->GetOption(RSK_LACS_TOKEN_COUNT).Value<uint8_t>());
        CanCompleteTriforce = TriforcePieces >= ctx->GetOption(RSK_TRIFORCE_HUNT_PIECES_REQUIRED).Value<uint8_t>();
    }

    bool Logic::SmallKeys(RandomizerRegion dungeon, uint8_t requiredAmount) {
        return SmallKeys(dungeon, requiredAmount, requiredAmount);
    }

    bool Logic::SmallKeys(RandomizerRegion dungeon, uint8_t requiredAmountGlitchless, uint8_t requiredAmountGlitched) {
        switch (dungeon) {
            case RR_FOREST_TEMPLE:
                /*if (IsGlitched && (GetDifficultyValueFromString(GlitchHookshotJump_Boots) >= static_cast<uint8_t>(GlitchDifficulty::INTERMEDIATE) || GetDifficultyValueFromString(GlitchHoverBoost) >= static_cast<uint8_t>(GlitchDifficulty::NOVICE) ||
                                    (GetDifficultyValueFromString(GlitchHover) >= static_cast<uint8_t>(GlitchDifficulty::NOVICE) && GetDifficultyValueFromString(GlitchISG) >= static_cast<uint8_t>(GlitchDifficulty::INTERMEDIATE)))) {
                    return ForestTempleKeys >= requiredAmountGlitched;
                }*/
                return ForestTempleKeys >= requiredAmountGlitchless;

            case RR_FIRE_TEMPLE:
                /*if (IsGlitched && (GetDifficultyValueFromString(GlitchLedgeClip) >= static_cast<uint8_t>(GlitchDifficulty::INTERMEDIATE) || GetDifficultyValueFromString(GlitchHover) >= static_cast<uint8_t>(GlitchDifficulty::INTERMEDIATE))) {
                    return FireTempleKeys >= requiredAmountGlitched;
                }*/
                return FireTempleKeys >= requiredAmountGlitchless;

            case RR_WATER_TEMPLE:
                /*if (IsGlitched && (false)) {
                    return WaterTempleKeys >= requiredAmountGlitched;
                }*/
                return WaterTempleKeys >= requiredAmountGlitchless;

            case RR_SPIRIT_TEMPLE:
                /*if (IsGlitched && (false)) {
                    return SpiritTempleKeys >= requiredAmountGlitched;
                }*/
                return SpiritTempleKeys >= requiredAmountGlitchless;

            case RR_SHADOW_TEMPLE:
                /*if (IsGlitched && (GetDifficultyValueFromString(GlitchHookshotClip) >= static_cast<uint8_t>(GlitchDifficulty::NOVICE))) {
                    return ShadowTempleKeys >= requiredAmountGlitched;
                }*/
                return ShadowTempleKeys >= requiredAmountGlitchless;

            case RR_BOTTOM_OF_THE_WELL:
                /*if (IsGlitched && (false)) {
                    return BottomOfTheWellKeys >= requiredAmountGlitched;
                }*/
                return BottomOfTheWellKeys >= requiredAmountGlitchless;

            case RR_GERUDO_TRAINING_GROUNDS:
                /*if (IsGlitched && (false)) {
                    return GerudoTrainingGroundsKeys >= requiredAmountGlitched;
                }*/
                return GerudoTrainingGroundsKeys >= requiredAmountGlitchless;

            case RR_GANONS_CASTLE:
                /*if (IsGlitched && (false)) {
                    return GanonsCastleKeys >= requiredAmountGlitched;
                }*/
                return GanonsCastleKeys >= requiredAmountGlitchless;

            case RR_MARKET_TREASURE_CHEST_GAME:
                /*if (IsGlitched && (false)) {
                    return TreasureGameKeys >= requiredAmountGlitched;
                }*/
                return TreasureGameKeys >= requiredAmountGlitchless;

            default:
                return false;
        }
    }

    bool Logic::EventsUpdated() {
        if (DekuTreeClearPast        != DekuTreeClear        ||
            GoronRubyPast            != GoronRuby            ||
            ZoraSapphirePast         != ZoraSapphire         ||
            ForestTrialClearPast     != ForestTrialClear     ||
            FireTrialClearPast       != FireTrialClear       ||
            WaterTrialClearPast      != WaterTrialClear      ||
            ShadowTrialClearPast     != ShadowTrialClear     ||
            SpiritTrialClearPast     != SpiritTrialClear     ||
            LightTrialClearPast      != LightTrialClear      ||
            DrainWellPast            != DrainWell            ||
            DampesWindmillAccessPast != DampesWindmillAccess ||
            TimeTravelPast           != TimeTravel) {
            DekuTreeClearPast         = DekuTreeClear;
            GoronRubyPast             = GoronRuby;
            ZoraSapphirePast          = ZoraSapphire;
            ForestTrialClearPast      = ForestTrialClear;
            FireTrialClearPast        = FireTrialClear;
            WaterTrialClearPast       = WaterTrialClear;
            ShadowTrialClearPast      = ShadowTrialClear;
            SpiritTrialClearPast      = SpiritTrialClear;
            LightTrialClearPast       = LightTrialClear;
            DrainWellPast             = DrainWell;
            DampesWindmillAccessPast  = DampesWindmillAccess;
            return true;
        }
        return false;
    }
}
