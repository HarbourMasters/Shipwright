#pragma once

#include <unordered_map>

enum CCStringEnumValues {

    // Spawn Enemies and Objects
    kEffectSpawnCuccoStorm,
    kEffectSpawnLitBomb,
    kEffectSpawnExplosion,
    kEffectSpawnArwing,
    kEffectSpawnDarklink,
    kEffectSpawnIronKnuckle,
    kEffectSpawnStalfos,
    kEffectSpawnFreezard,
    kEffectSpawnLikeLike,
    kEffectSpawnGibdo,
    kEffectSpawnKeese,
    kEffectSpawnIceKeese,
    kEffectSpawnFireKeese,
    kEffectSpawnWolfos,
    kEffectSpawnWallmaster,

    // Link Modifiers
    kEffectTakeHalfDamage,
    kEffectTakeDoubleDamage,
    kEffectOneHitKo,
    kEffectInvincibility,
    kEffectIncreaseSpeed,
    kEffectDecreaseSpeed,
    kEffectLowGravity,
    kEffectHighGravity,
    kEffectForceIronBoots,
    kEffectForceHoverBoots,
    kEffectSlipperyFloor,
    kEffectNoLedgeGrabs,
    kEffectRandomWind,
    kEffectRandomBonks,

    // Hurt or Heal Link
    kEffectEmptyHeart,
    kEffectFillHeart,
    kEffectKnockbackLinkWeak,
    kEffectKnockbackLinkStrong,
    kEffectKnockbackLinkMega,
    kEffectBurnLink,
    kEffectFreezeLink,
    kEffectElectrocuteLink,
    kEffectKillLink,

    // Give Items and Consumables
    kEffectAddHeartContainer,
    kEffectFillMagic,
    kEffectAddRupees,
    kEffectGiveDekuShield,
    kEffectGiveHylianShield,
    kEffectRefillSticks,
    kEffectRefillNuts,
    kEffectRefillBombs,
    kEffectRefillSeeds,
    kEffectRefillArrows,
    kEffectRefillBombchus,

    // Take Items and Consumables
    kEffectRemoveHeartContainer,
    kEffectEmptyMagic,
    kEffectRemoveRupees,
    kEffectTakeDekuShield,
    kEffectTakeHylianShield,
    kEffectTakeSticks,
    kEffectTakeNuts,
    kEffectTakeBombs,
    kEffectTakeSeeds,
    kEffectTakeArrows,
    kEffectTakeBombchus,

    // Link Size Modifiers
    kEffectGiantLink,
    kEffectMinishLink,
    kEffectPaperLink,
    kEffectSquishedLink,
    kEffectInvisibleLink,

    // Generic kEffects
    kEffectRandomBombTimer,
    kEffectSetTimeToDawn,
    kEffectSetTimeToDusk,

    // Visual kEffects
    kEffectNoUi,
    kEffectRainstorm,
    kEffectDebugMode,
    kEffectRandomCosmetics,

    // Controls
    kEffectNoZButton,
    kEffectReverseControls,
    kEffectPacifistMode,
    kEffectPressRandomButtons,
    kEffectClearCbuttons,
    kEffectClearDpad,

    // Teleport Player
    kEffectTpLinksHouse,
    kEffectTpMinuet,
    kEffectTpBolero,
    kEffectTpSerenade,
    kEffectTpRequiem,
    kEffectTpNocturne,
    kEffectTpPrelude,

    // Tunic Color (Bidding War)
    kEffectTunicRed,
    kEffectTunicGreen,
    kEffectTunicBlue,
    kEffectTunicOrange,
    kEffectTunicYellow,
    kEffectTunicPurple,
    kEffectTunicPink,
    kEffectTunicBrown,
    kEffectTunicBlack,

    // Navi Color (Bidding War)
    kEffectNaviRed,
    kEffectNaviGreen,
    kEffectNaviBlue,
    kEffectNaviOrange,
    kEffectNaviYellow,
    kEffectNaviPurple,
    kEffectNaviPink,
    kEffectNaviBrown,
    kEffectNaviBlack,

    // Link's Hair Color (Bidding War)
    kEffectHairRed,
    kEffectHairGreen,
    kEffectHairBlue,
    kEffectHairOrange,
    kEffectHairYellow,
    kEffectHairPurple,
    kEffectHairPink,
    kEffectHairBrown,
    kEffectHairBlack,
};

enum CCCatEnumValues {
    kEffectCatNone,
    kEffectCatGravity,
    kEffectCatLinkSize,
    kEffectCatRandomBombFuseTimer,
    kEffectCatPacifist,
    kEffectCatNoZ,
    kEffectCatRandomButtons,
    kEffectCatUi,
    kEffectCatWeather,
    kEffectCatDebugMode,
    kEffectCatNoLedgeGrabs,
    kEffectCatRandomWind,
    kEffectCatRandomBonks,
    kEffectCatReverseControls,
    kEffectCatBoots,
    kEffectCatSlipperyFloor,
    kEffectCatSpeed,
    kEffectCatDamageTaken,
    kEffectCatOhko,
    kEffectCatInvincible,
    kEffectCatSpawnEnemy,
    kEffectCatSpawnActor
};

std::unordered_map<std::string, CCStringEnumValues> effectStringToEnum = {

    // Spawn Enemies and Objects
    { "spawn_cucco_storm", kEffectSpawnCuccoStorm },
    { "spawn_lit_bomb", kEffectSpawnLitBomb },
    { "spawn_explosion", kEffectSpawnExplosion },
    { "spawn_arwing", kEffectSpawnArwing },
    { "spawn_darklink", kEffectSpawnDarklink },
    { "spawn_iron_knuckle", kEffectSpawnIronKnuckle },
    { "spawn_stalfos", kEffectSpawnStalfos },
    { "spawn_freezard", kEffectSpawnFreezard },
    { "spawn_like_like", kEffectSpawnLikeLike },
    { "spawn_gibdo", kEffectSpawnGibdo },
    { "spawn_keese", kEffectSpawnKeese },
    { "spawn_ice_keese", kEffectSpawnIceKeese },
    { "spawn_fire_keese", kEffectSpawnFireKeese },
    { "spawn_wolfos", kEffectSpawnWolfos },
    { "spawn_wallmaster", kEffectSpawnWallmaster },

    // Link Modifiers
    { "take_half_damage", kEffectTakeHalfDamage },
    { "take_double_damage", kEffectTakeDoubleDamage },
    { "one_hit_ko", kEffectOneHitKo },
    { "invincibility", kEffectInvincibility },
    { "increase_speed", kEffectIncreaseSpeed },
    { "decrease_speed", kEffectDecreaseSpeed },
    { "low_gravity", kEffectLowGravity },
    { "high_gravity", kEffectHighGravity },
    { "force_iron_boots", kEffectForceIronBoots },
    { "force_hover_boots", kEffectForceHoverBoots },
    { "slippery_floor", kEffectSlipperyFloor },
    { "no_ledge_grabs", kEffectNoLedgeGrabs },
    { "random_wind", kEffectRandomWind },
    { "random_bonks", kEffectRandomBonks },

    // Hurt or Heal Link
    { "empty_heart", kEffectEmptyHeart },
    { "fill_heart", kEffectFillHeart },
    { "knockback_link_weak", kEffectKnockbackLinkWeak },
    { "knockback_link_strong", kEffectKnockbackLinkStrong },
    { "knockback_link_mega", kEffectKnockbackLinkMega },
    { "burn_link", kEffectBurnLink },
    { "freeze_link", kEffectFreezeLink },
    { "electrocute_link", kEffectElectrocuteLink },
    { "kill_link", kEffectKillLink },

    // Give Items and Consumables
    { "add_heart_container", kEffectAddHeartContainer },
    { "fill_magic", kEffectFillMagic },
    { "add_rupees", kEffectAddRupees },
    { "give_deku_shield", kEffectGiveDekuShield },
    { "give_hylian_shield", kEffectGiveHylianShield },
    { "refill_sticks", kEffectRefillSticks },
    { "refill_nuts", kEffectRefillNuts },
    { "refill_bombs", kEffectRefillBombs },
    { "refill_seeds", kEffectRefillSeeds },
    { "refill_arrows", kEffectRefillArrows },
    { "refill_bombchus", kEffectRefillBombchus },

    // Take Items and Consumables
    { "remove_heart_container", kEffectRemoveHeartContainer },
    { "empty_magic", kEffectEmptyMagic },
    { "remove_rupees", kEffectRemoveRupees },
    { "take_deku_shield", kEffectTakeDekuShield },
    { "take_hylian_shield", kEffectTakeHylianShield },
    { "take_sticks", kEffectTakeSticks },
    { "take_nuts", kEffectTakeNuts },
    { "take_bombs", kEffectTakeBombs },
    { "take_seeds", kEffectTakeSeeds },
    { "take_arrows", kEffectTakeArrows },
    { "take_bombchus", kEffectTakeBombchus },

    // Link Size Modifiers
    { "giant_link", kEffectGiantLink },
    { "minish_link", kEffectMinishLink },
    { "paper_link", kEffectPaperLink },
    { "squished_link", kEffectSquishedLink },
    { "invisible_link", kEffectInvisibleLink },

    // Generic kEffects
    { "random_bomb_timer", kEffectRandomBombTimer },
    { "set_time_to_dawn", kEffectSetTimeToDawn },
    { "set_time_to_dusk", kEffectSetTimeToDusk },

    // Visual kEffects
    { "no_ui", kEffectNoUi },
    { "rainstorm", kEffectRainstorm },
    { "debug_mode", kEffectDebugMode },
    { "random_cosmetics", kEffectRandomCosmetics },

    // Controls
    { "no_z_button", kEffectNoZButton },
    { "reverse_controls", kEffectReverseControls },
    { "pacifist_mode", kEffectPacifistMode },
    { "press_random_buttons", kEffectPressRandomButtons },
    { "clear_cbuttons", kEffectClearCbuttons },
    { "clear_dpad", kEffectClearDpad },

    // Teleport Player
    { "tp_links_house", kEffectTpLinksHouse },
    { "tp_minuet", kEffectTpMinuet },
    { "tp_bolero", kEffectTpBolero },
    { "tp_serenade", kEffectTpSerenade },
    { "tp_requiem", kEffectTpRequiem },
    { "tp_nocturne", kEffectTpNocturne },
    { "tp_prelude", kEffectTpPrelude },

    // Tunic Color (Bidding War)
    { "tunic_red", kEffectTunicRed },
    { "tunic_green", kEffectTunicGreen },
    { "tunic_blue", kEffectTunicBlue },
    { "tunic_orange", kEffectTunicOrange },
    { "tunic_yellow", kEffectTunicYellow },
    { "tunic_purple", kEffectTunicPurple },
    { "tunic_pink", kEffectTunicPink },
    { "tunic_brown", kEffectTunicBrown },
    { "tunic_black", kEffectTunicBlack },

    // Navi Color (Bidding War)
    { "navi_red", kEffectNaviRed },
    { "navi_green", kEffectNaviGreen },
    { "navi_blue", kEffectNaviBlue },
    { "navi_orange", kEffectNaviOrange },
    { "navi_yellow", kEffectNaviYellow },
    { "navi_purple", kEffectNaviPurple },
    { "navi_pink", kEffectNaviPink },
    { "navi_brown", kEffectNaviBrown },
    { "navi_black", kEffectNaviBlack },

    // Link's Hair Color (Bidding War)
    { "hair_red", kEffectHairRed },
    { "hair_green", kEffectHairGreen },
    { "hair_blue", kEffectHairBlue },
    { "hair_orange", kEffectHairOrange },
    { "hair_yellow", kEffectHairYellow },
    { "hair_purple", kEffectHairPurple },
    { "hair_pink", kEffectHairPink },
    { "hair_brown", kEffectHairBrown },
    { "hair_black", kEffectHairBlack },
};
