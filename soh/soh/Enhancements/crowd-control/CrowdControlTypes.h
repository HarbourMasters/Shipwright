typedef enum {

    // Spawn Enemies and Objects
    effectSpawnCuccoStorm,
    effectSpawnLitBomb,
    effectSpawnExplosion,
    effectSpawnArwing,
    effectSpawnDarklink,
    effectSpawnIronknuckle,
    effectSpawnStalfos,
    effectSpawnFreezard,
    effectSpawnLikelike,
    effectSpawnGibdo,
    effectSpawnKeese,
    effectSpawnIcekeese,
    effectSpawnFirekeese,
    effectSpawnWolfos,
    effectSpawnWallmaster,

    // Link Modifiers
    effectHalfDamageTaken,
    effectDoubleDamageTaken,
    effectOhko,
    effectInvincible,
    effectIncreaseSpeed,
    effectDecreaseSpeed,
    effectLowGravity,
    effectHighGravity,
    effectIronBoots,
    effectHoverBoots,
    effectSlipperyFloor,
    effectNoLedgeGrabs,
    effectRandomWind,
    effectRandomBonks,

    // Hurt or Heal Link
    effectDamage,
    effectHeal,
    effectKnockback,
    effectBurn,
    effectFreeze,
    effectElectrocute,
    effectKill,

    // Give Items and Consumables
    effectAddHeartContainer,
    effectFillMagic,
    effectAddRupees,
    effectGiveDekushield,
    effectGiveHylianshield,
    effectRefillSticks,
    effectRefillNuts,
    effectRefillBombs,
    effectRefillSeeds,
    effectRefillArrows,
    effectRefillBombchus,

    // Take Items and Consumables
    effectRemoveHeartContainer,
    effectEmptyMagic,
    effectRemoveRupees,
    effectTakeDekushield,
    effectTakeHylianshield,
    effectTakeSticks,
    effectTakeNuts,
    effectTakeBombs,
    effectTakeSeeds,
    effectTakeArrows,
    effectTakeBombchus,

    // Link Size Modifiers
    effectGiantLink,
    effectMinishLink,
    effectPaperLink,
    effectSquishedLink,
    effectInvisibleLink,

    // Generic Effects
    effectRandomBombTimer,
    effectTimeDawn,
    effectTimeDusk,

    // Visual Effects
    effectNoUi,
    effectRainstorm,
    effectDebugMode,
    effectRandomCosmetics,

    // Controls
    effectNoZ,
    effectReverseControls,
    effectPacifist,
    effectRandomButtons,
    effectClearCbuttons,
    effectClearDpad,

    // Teleport Player
    effectTpLinkshouse,
    effectTpMinuet,
    effectTpBolero,
    effectTpSerenade,
    effectTpRequiem,
    effectTpNocturne,
    effectTpPrelude,

    // Tunic Color (Bidding War)
    effectTunicRed,
    effectTunicGreen,
    effectTunicBlue,
    effectTunicOrange,
    effectTunicYellow,
    effectTunicPurple,
    effectTunicPink,
    effectTunicBrown,
    effectTunicBlack,

    // Navi Color (Bidding War)
    effectNaviRed,
    effectNaviGreen,
    effectNaviBlue,
    effectNaviOrange,
    effectNaviYellow,
    effectNaviPurple,
    effectNaviPink,
    effectNaviBrown,
    effectNaviBlack,

    // Link's Hair Color (Bidding War)
    effectHairRed,
    effectHairGreen,
    effectHairBlue,
    effectHairOrange,
    effectHairYellow,
    effectHairPurple,
    effectHairPink,
    effectHairBrown,
    effectHairBlack,
} CCStringEnumValues;

typedef enum {
    effectCatNone,
    effectCatGravity,
    effectCatLinkSize,
    effectCatRandomBombFuseTimer,
    effectCatPacifist,
    effectCatNoZ,
    effectCatRandomButtons,
    effectCatUi,
    effectCatWeather,
    effectCatDebugMode,
    effectCatNoLedgeGrabs,
    effectCatRandomWind,
    effectCatReverseControls,
    effectCatBoots,
    effectCatSpeed,
    effectCatDamageTaken,
    effectCatOhko,
    effectCatSpawnEnemy,
    effectCatSpawnActor
} CCCatEnumValues;

std::unordered_map<std::string, CCStringEnumValues> effectStringToEnum = {

    // Spawn Enemies and Objects
    { "spawn_cuccostorm", effectSpawnCuccoStorm },
    { "spawn_litbomb", effectSpawnLitBomb },
    { "spawn_explosion", effectSpawnExplosion },
    { "spawn_arwing", effectSpawnArwing },
    { "spawn_darklink", effectSpawnDarklink },
    { "spawn_ironknuckle", effectSpawnIronknuckle },
    { "spawn_stalfos", effectSpawnStalfos },
    { "spawn_freezard", effectSpawnFreezard },
    { "spawn_likelike", effectSpawnLikelike },
    { "spawn_gibdo", effectSpawnGibdo },
    { "spawn_keese", effectSpawnKeese },
    { "spawn_icekeese", effectSpawnIcekeese },
    { "spawn_firekeese", effectSpawnFirekeese },
    { "spawn_wolfos", effectSpawnWolfos },
    { "spawn_wallmaster", effectSpawnWallmaster },

    // Link Modifiers
    { "half_damage_taken", effectHalfDamageTaken },
    { "double_damage_taken", effectDoubleDamageTaken },
    { "ohko", effectOhko },
    { "invincible", effectInvincible },
    { "increase_speed", effectIncreaseSpeed },
    { "decrease_speed", effectDecreaseSpeed },
    { "low_gravity", effectLowGravity },
    { "high_gravity", effectHighGravity },
    { "iron_boots", effectIronBoots },
    { "hover_boots", effectHoverBoots },
    { "slippery_floor", effectSlipperyFloor },
    { "no_ledge_grabs", effectNoLedgeGrabs },
    { "random_wind", effectRandomWind },
    { "random_bonks", effectRandomBonks },

    // Hurt or Heal Link
    { "damage", effectDamage },
    { "heal", effectHeal },
    { "knockback", effectKnockback },
    { "burn", effectBurn },
    { "freeze", effectFreeze },
    { "electrocute", effectElectrocute },
    { "kill", effectKill },

    // Give Items and Consumables
    { "add_heart_container", effectAddHeartContainer },
    { "fill_magic", effectFillMagic },
    { "add_rupees", effectAddRupees },
    { "give_dekushield", effectGiveDekushield },
    { "give_hylianshield", effectGiveHylianshield },
    { "refill_sticks", effectRefillSticks },
    { "refill_nuts", effectRefillNuts },
    { "refill_bombs", effectRefillBombs },
    { "refill_seeds", effectRefillSeeds },
    { "refill_arrows", effectRefillArrows },
    { "refill_bombchus", effectRefillBombchus },

    // Take Items and Consumables
    { "remove_heart_container", effectRemoveHeartContainer },
    { "empty_magic", effectEmptyMagic },
    { "remove_rupees", effectRemoveRupees },
    { "take_dekushield", effectTakeDekushield },
    { "take_hylianshield", effectTakeHylianshield },
    { "take_sticks", effectTakeSticks },
    { "take_nuts", effectTakeNuts },
    { "take_bombs", effectTakeBombs },
    { "take_seeds", effectTakeSeeds },
    { "take_arrows", effectTakeArrows },
    { "take_bombchus", effectTakeBombchus },

    // Link Size Modifiers
    { "giant_link", effectGiantLink },
    { "minish_link", effectMinishLink },
    { "paper_link", effectPaperLink },
    { "squished_link", effectSquishedLink },
    { "invisible_link", effectInvisibleLink },

    // Generic Effects
    { "random_bomb_timer", effectRandomBombTimer },
    { "time_dawn", effectTimeDawn },
    { "time_dusk", effectTimeDusk },

    // Visual Effects
    { "no_ui", effectNoUi },
    { "rainstorm", effectRainstorm },
    { "debug_mode", effectDebugMode },
    { "random_cosmetics", effectRandomCosmetics },

    // Controls
    { "no_z", effectNoZ },
    { "reverse_controls", effectReverseControls },
    { "pacifist", effectPacifist },
    { "random_buttons", effectRandomButtons },
    { "clear_cbuttons", effectClearCbuttons },
    { "clear_dpad", effectClearDpad },

    // Teleport Player
    { "tp_linkshouse", effectTpLinkshouse },
    { "tp_minuet", effectTpMinuet },
    { "tp_bolero", effectTpBolero },
    { "tp_serenade", effectTpSerenade },
    { "tp_requiem", effectTpRequiem },
    { "tp_nocturne", effectTpNocturne },
    { "tp_prelude", effectTpPrelude },

    // Tunic Color (Bidding War)
    { "tunic_red", effectTunicRed },
    { "tunic_green", effectTunicGreen },
    { "tunic_blue", effectTunicBlue },
    { "tunic_orange", effectTunicOrange },
    { "tunic_yellow", effectTunicYellow },
    { "tunic_purple", effectTunicPurple },
    { "tunic_pink", effectTunicPink },
    { "tunic_brown", effectTunicBrown },
    { "tunic_black", effectTunicBlack },

    // Navi Color (Bidding War)
    { "navi_red", effectNaviRed },
    { "navi_green", effectNaviGreen },
    { "navi_blue", effectNaviBlue },
    { "navi_orange", effectNaviOrange },
    { "navi_yellow", effectNaviYellow },
    { "navi_purple", effectNaviPurple },
    { "navi_pink", effectNaviPink },
    { "navi_brown", effectNaviBrown },
    { "navi_black", effectNaviBlack },

    // Link's Hair Color (Bidding War)
    { "hair_red", effectHairRed },
    { "hair_green", effectHairGreen },
    { "hair_blue", effectHairBlue },
    { "hair_orange", effectHairOrange },
    { "hair_yellow", effectHairYellow },
    { "hair_purple", effectHairPurple },
    { "hair_pink", effectHairPink },
    { "hair_brown", effectHairBrown },
    { "hair_black", effectHairBlack },
};
