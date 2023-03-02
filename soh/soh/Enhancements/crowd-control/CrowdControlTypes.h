#pragma once

#include <unordered_map>

enum CCStringEnumValues {

    // Spawn Enemies and Objects
    EffectSpawnCuccoStorm,
    EffectSpawnLitBomb,
    EffectSpawnExplosion,
    EffectSpawnArwing,
    EffectSpawnDarklink,
    EffectSpawnIronKnuckle,
    EffectSpawnStalfos,
    EffectSpawnFreezard,
    EffectSpawnLikeLike,
    EffectSpawnGibdo,
    EffectSpawnKeese,
    EffectSpawnIceKeese,
    EffectSpawnFireKeese,
    EffectSpawnWolfos,
    EffectSpawnWallmaster,

    // Link Modifiers
    EffectTakeHalfDamage,
    EffectTakeDoubleDamage,
    EffectOneHitKo,
    EffectInvincibility,
    EffectIncreaseSpeed,
    EffectDecreaseSpeed,
    EffectLowGravity,
    EffectHighGravity,
    EffectForceIronBoots,
    EffectForceHoverBoots,
    EffectSlipperyFloor,
    EffectNoLedgeGrabs,
    EffectRandomWind,
    EffectRandomBonks,

    // Hurt or Heal Link
    EffectEmptyHeart,
    EffectFillHeart,
    EffectKnockbackLinkWeak,
    EffectKnockbackLinkStrong,
    EffectKnockbackLinkMega,
    EffectBurnLink,
    EffectFreezeLink,
    EffectElectrocuteLink,
    EffectKillLink,

    // Give Items and Consumables
    EffectAddHeartContainer,
    EffectFillMagic,
    EffectAddRupees,
    EffectGiveDekuShield,
    EffectGiveHylianShield,
    EffectRefillSticks,
    EffectRefillNuts,
    EffectRefillBombs,
    EffectRefillSeeds,
    EffectRefillArrows,
    EffectRefillBombchus,

    // Take Items and Consumables
    EffectRemoveHeartContainer,
    EffectEmptyMagic,
    EffectRemoveRupees,
    EffectTakeDekuShield,
    EffectTakeHylianShield,
    EffectTakeSticks,
    EffectTakeNuts,
    EffectTakeBombs,
    EffectTakeSeeds,
    EffectTakeArrows,
    EffectTakeBombchus,

    // Link Size Modifiers
    EffectGiantLink,
    EffectMinishLink,
    EffectPaperLink,
    EffectSquishedLink,
    EffectInvisibleLink,

    // Generic Effects
    EffectRandomBombTimer,
    EffectSetTimeToDawn,
    EffectSetTimeToDusk,

    // Visual Effects
    EffectNoUi,
    EffectRainstorm,
    EffectDebugMode,
    EffectRandomCosmetics,

    // Controls
    EffectNoZButton,
    EffectReverseControls,
    EffectPacifistMode,
    EffectPressRandomButtons,
    EffectClearCbuttons,
    EffectClearDpad,

    // Teleport Player
    EffectTpLinksHouse,
    EffectTpMinuet,
    EffectTpBolero,
    EffectTpSerenade,
    EffectTpRequiem,
    EffectTpNocturne,
    EffectTpPrelude,

    // Tunic Color (Bidding War)
    EffectTunicRed,
    EffectTunicGreen,
    EffectTunicBlue,
    EffectTunicOrange,
    EffectTunicYellow,
    EffectTunicPurple,
    EffectTunicPink,
    EffectTunicBrown,
    EffectTunicBlack,

    // Navi Color (Bidding War)
    EffectNaviRed,
    EffectNaviGreen,
    EffectNaviBlue,
    EffectNaviOrange,
    EffectNaviYellow,
    EffectNaviPurple,
    EffectNaviPink,
    EffectNaviBrown,
    EffectNaviBlack,

    // Link's Hair Color (Bidding War)
    EffectHairRed,
    EffectHairGreen,
    EffectHairBlue,
    EffectHairOrange,
    EffectHairYellow,
    EffectHairPurple,
    EffectHairPink,
    EffectHairBrown,
    EffectHairBlack,
};

enum CCCatEnumValues {
    EffectCatNone,
    EffectCatGravity,
    EffectCatLinkSize,
    EffectCatRandomBombFuseTimer,
    EffectCatPacifist,
    EffectCatNoZ,
    EffectCatRandomButtons,
    EffectCatUi,
    EffectCatWeather,
    EffectCatDebugMode,
    EffectCatNoLedgeGrabs,
    EffectCatRandomWind,
    EffectCatRandomBonks,
    EffectCatReverseControls,
    EffectCatBoots,
    EffectCatSlipperyFloor,
    EffectCatSpeed,
    EffectCatDamageTaken,
    EffectCatOhko,
    EffectCatInvincible,
    EffectCatSpawnEnemy,
    EffectCatSpawnActor
};

std::unordered_map<std::string, CCStringEnumValues> effectStringToEnum = {

    // Spawn Enemies and Objects
    { "spawn_cucco_storm", EffectSpawnCuccoStorm },
    { "spawn_lit_bomb", EffectSpawnLitBomb },
    { "spawn_explosion", EffectSpawnExplosion },
    { "spawn_arwing", EffectSpawnArwing },
    { "spawn_darklink", EffectSpawnDarklink },
    { "spawn_iron_knuckle", EffectSpawnIronKnuckle },
    { "spawn_stalfos", EffectSpawnStalfos },
    { "spawn_freezard", EffectSpawnFreezard },
    { "spawn_like_like", EffectSpawnLikeLike },
    { "spawn_gibdo", EffectSpawnGibdo },
    { "spawn_keese", EffectSpawnKeese },
    { "spawn_ice_keese", EffectSpawnIceKeese },
    { "spawn_fire_keese", EffectSpawnFireKeese },
    { "spawn_wolfos", EffectSpawnWolfos },
    { "spawn_wallmaster", EffectSpawnWallmaster },

    // Link Modifiers
    { "take_half_damage", EffectTakeHalfDamage },
    { "take_double_damage", EffectTakeDoubleDamage },
    { "one_hit_ko", EffectOneHitKo },
    { "invincibility", EffectInvincibility },
    { "increase_speed", EffectIncreaseSpeed },
    { "decrease_speed", EffectDecreaseSpeed },
    { "low_gravity", EffectLowGravity },
    { "high_gravity", EffectHighGravity },
    { "force_iron_boots", EffectForceIronBoots },
    { "force_hover_boots", EffectForceHoverBoots },
    { "slippery_floor", EffectSlipperyFloor },
    { "no_ledge_grabs", EffectNoLedgeGrabs },
    { "random_wind", EffectRandomWind },
    { "random_bonks", EffectRandomBonks },

    // Hurt or Heal Link
    { "empty_heart", EffectEmptyHeart },
    { "fill_heart", EffectFillHeart },
    { "knockback_link_weak", EffectKnockbackLinkWeak },
    { "knockback_link_strong", EffectKnockbackLinkStrong },
    { "knockback_link_mega", EffectKnockbackLinkMega },
    { "burn_link", EffectBurnLink },
    { "freeze_link", EffectFreezeLink },
    { "electrocute_link", EffectElectrocuteLink },
    { "kill_link", EffectKillLink },

    // Give Items and Consumables
    { "add_heart_container", EffectAddHeartContainer },
    { "fill_magic", EffectFillMagic },
    { "add_rupees", EffectAddRupees },
    { "give_deku_shield", EffectGiveDekuShield },
    { "give_hylian_shield", EffectGiveHylianShield },
    { "refill_sticks", EffectRefillSticks },
    { "refill_nuts", EffectRefillNuts },
    { "refill_bombs", EffectRefillBombs },
    { "refill_seeds", EffectRefillSeeds },
    { "refill_arrows", EffectRefillArrows },
    { "refill_bombchus", EffectRefillBombchus },

    // Take Items and Consumables
    { "remove_heart_container", EffectRemoveHeartContainer },
    { "empty_magic", EffectEmptyMagic },
    { "remove_rupees", EffectRemoveRupees },
    { "take_deku_shield", EffectTakeDekuShield },
    { "take_hylian_shield", EffectTakeHylianShield },
    { "take_sticks", EffectTakeSticks },
    { "take_nuts", EffectTakeNuts },
    { "take_bombs", EffectTakeBombs },
    { "take_seeds", EffectTakeSeeds },
    { "take_arrows", EffectTakeArrows },
    { "take_bombchus", EffectTakeBombchus },

    // Link Size Modifiers
    { "giant_link", EffectGiantLink },
    { "minish_link", EffectMinishLink },
    { "paper_link", EffectPaperLink },
    { "squished_link", EffectSquishedLink },
    { "invisible_link", EffectInvisibleLink },

    // Generic Effects
    { "random_bomb_timer", EffectRandomBombTimer },
    { "set_time_to_dawn", EffectSetTimeToDawn },
    { "set_time_to_dusk", EffectSetTimeToDusk },

    // Visual Effects
    { "no_ui", EffectNoUi },
    { "rainstorm", EffectRainstorm },
    { "debug_mode", EffectDebugMode },
    { "random_cosmetics", EffectRandomCosmetics },

    // Controls
    { "no_z_button", EffectNoZButton },
    { "reverse_controls", EffectReverseControls },
    { "pacifist_mode", EffectPacifistMode },
    { "press_random_buttons", EffectPressRandomButtons },
    { "clear_cbuttons", EffectClearCbuttons },
    { "clear_dpad", EffectClearDpad },

    // Teleport Player
    { "tp_links_house", EffectTpLinksHouse },
    { "tp_minuet", EffectTpMinuet },
    { "tp_bolero", EffectTpBolero },
    { "tp_serenade", EffectTpSerenade },
    { "tp_requiem", EffectTpRequiem },
    { "tp_nocturne", EffectTpNocturne },
    { "tp_prelude", EffectTpPrelude },

    // Tunic Color (Bidding War)
    { "tunic_red", EffectTunicRed },
    { "tunic_green", EffectTunicGreen },
    { "tunic_blue", EffectTunicBlue },
    { "tunic_orange", EffectTunicOrange },
    { "tunic_yellow", EffectTunicYellow },
    { "tunic_purple", EffectTunicPurple },
    { "tunic_pink", EffectTunicPink },
    { "tunic_brown", EffectTunicBrown },
    { "tunic_black", EffectTunicBlack },

    // Navi Color (Bidding War)
    { "navi_red", EffectNaviRed },
    { "navi_green", EffectNaviGreen },
    { "navi_blue", EffectNaviBlue },
    { "navi_orange", EffectNaviOrange },
    { "navi_yellow", EffectNaviYellow },
    { "navi_purple", EffectNaviPurple },
    { "navi_pink", EffectNaviPink },
    { "navi_brown", EffectNaviBrown },
    { "navi_black", EffectNaviBlack },

    // Link's Hair Color (Bidding War)
    { "hair_red", EffectHairRed },
    { "hair_green", EffectHairGreen },
    { "hair_blue", EffectHairBlue },
    { "hair_orange", EffectHairOrange },
    { "hair_yellow", EffectHairYellow },
    { "hair_purple", EffectHairPurple },
    { "hair_pink", EffectHairPink },
    { "hair_brown", EffectHairBrown },
    { "hair_black", EffectHairBlack },
};
