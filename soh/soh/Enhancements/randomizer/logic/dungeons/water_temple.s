def RR_WATER_TEMPLE_ENTRYWAY SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Entryway
//Exits
RR_WATER_TEMPLE_LOBBY (and (HasItem RG_BRONZE_SCALE) (IsDungeonVanilla WATER_TEMPLE))
RR_WATER_TEMPLE_MQ_3F_CENTRAL (and (HasItem RG_BRONZE_SCALE) (IsDungeonMQ WATER_TEMPLE))
RR_LH_FROM_WATER_TEMPLE true

//Water Temple logic currently assumes that the locked door leading to the upper water raising location is unlocked from the start
def RR_WATER_TEMPLE_LOBBY SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Lobby
//Checks
RC_WATER_TEMPLE_MAIN_LEVEL_2_POT_1 (and CanBreakPots (or CanWaterTempleLowFromHigh (or CanWaterTempleMiddle (and (CanUse RG_IRON_BOOTS) (CanUse RG_HOOKSHOT)))))
RC_WATER_TEMPLE_MAIN_LEVEL_2_POT_2 (and CanBreakPots (or CanWaterTempleLowFromHigh (or CanWaterTempleMiddle (and (CanUse RG_IRON_BOOTS) (CanUse RG_HOOKSHOT)))))
//Exits
RR_WATER_TEMPLE_ENTRYWAY true
RR_WATER_TEMPLE_EAST_LOWER (or CanWaterTempleLowFromHigh (and (or RT_FEWER_TUNIC_REQUIREMENTS (CanUse RG_ZORA_TUNIC)) (or (CanUse RG_IRON_BOOTS) (and (CanUse RG_LONGSHOT) RT_WATER_LONGSHOT_TORCH))))
RR_WATER_TEMPLE_NORTH_LOWER (or CanWaterTempleLowFromHigh (and (or RT_FEWER_TUNIC_REQUIREMENTS (CanUse RG_ZORA_TUNIC)) (CanUse RG_IRON_BOOTS)))
RR_WATER_TEMPLE_SOUTH_LOWER (and CanWaterTempleLowFromHigh (and HasExplosives (and (or (HasItem RG_SILVER_SCALE) (CanUse RG_IRON_BOOTS)) (or RT_FEWER_TUNIC_REQUIREMENTS (CanUse RG_ZORA_TUNIC)))))
RR_WATER_TEMPLE_WEST_LOWER (and CanWaterTempleLowFromHigh (and (HasItem RG_GORONS_BRACELET) (and (or IsChild (or (HasItem RG_SILVER_SCALE) (CanUse RG_IRON_BOOTS))) (or RT_FEWER_TUNIC_REQUIREMENTS (CanUse RG_ZORA_TUNIC)))))
RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER (and CanWaterTempleLowFromHigh (SmallKeys SCENE_WATER_TEMPLE 5))
RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER (and (or CanWaterTempleLowFromHigh CanWaterTempleMiddle) (or HasFireSourceWithTorch (CanUse RG_FAIRY_BOW)))
RR_WATER_TEMPLE_EAST_MIDDLE (and (or CanWaterTempleLowFromHigh (or CanWaterTempleMiddle (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))) (CanUse RG_HOOKSHOT))
RR_WATER_TEMPLE_WEST_MIDDLE CanWaterTempleMiddle
RR_WATER_TEMPLE_HIGH_WATER (and IsAdult (or (CanUse RG_HOVER_BOOTS) (and RT_DAMAGE_BOOST (and (CanUse RG_BOMB_BAG) TakeDamage))))
RR_WATER_TEMPLE_BLOCK_CORRIDOR (and (or CanWaterTempleLowFromHigh CanWaterTempleMiddle) (and (or (CanUse RG_FAIRY_SLINGSHOT) (CanUse RG_FAIRY_BOW)) (or (CanUse RG_LONGSHOT) (or (CanUse RG_HOVER_BOOTS) (and RT_WATER_CENTRAL_BOW (or IsAdult CanWaterTempleMiddle))))))
RR_WATER_TEMPLE_FALLING_PLATFORM_ROOM (and CanWaterTempleHigh (SmallKeys SCENE_WATER_TEMPLE 4))
RR_WATER_TEMPLE_PRE_BOSS_ROOM (or (and CanWaterTempleHigh (CanUse RG_LONGSHOT)) (and RT_HOVER_BOOST_SIMPLE (and RT_DAMAGE_BOOST_SIMPLE (and HasExplosives (CanUse RG_HOVER_BOOTS)))))

def RR_WATER_TEMPLE_EAST_LOWER SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple East Lower
//Events
CanWaterTempleLowFromHigh (CanUse RG_ZELDAS_LULLABY)
//Checks
RC_WATER_TEMPLE_TORCH_POT_1 (and CanBreakPots (or CanWaterTempleLowFromHigh (and (CanUse RG_HOOKSHOT) (CanUse RG_IRON_BOOTS))))
RC_WATER_TEMPLE_TORCH_POT_2 (and CanBreakPots (or CanWaterTempleLowFromHigh (and (CanUse RG_HOOKSHOT) (CanUse RG_IRON_BOOTS))))
//Exits
RR_WATER_TEMPLE_LOBBY (or CanWaterTempleLowFromHigh (and (or RT_FEWER_TUNIC_REQUIREMENTS (CanUse RG_ZORA_TUNIC)) (CanUse RG_IRON_BOOTS)))
RR_WATER_TEMPLE_MAP_ROOM CanWaterTempleHigh
RR_WATER_TEMPLE_CRACKED_WALL (or CanWaterTempleMiddle (and CanWaterTempleHigh (and CanWaterTempleLowFromHigh (or (and (CanUse RG_HOVER_BOOTS) RT_WATER_CRACKED_WALL_HOVERS) RT_WATER_CRACKED_WALL))))
RR_WATER_TEMPLE_TORCH_ROOM (and CanWaterTempleLowFromHigh (or HasFireSourceWithTorch (CanUse RG_FAIRY_BOW)))

def RR_WATER_TEMPLE_MAP_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Map Room
//Checks
RC_WATER_TEMPLE_MAP_CHEST (CanKillEnemy RE_SPIKE ED_CLOSE true 3)
//Exits
RR_WATER_TEMPLE_EAST_LOWER (CanKillEnemy RE_SPIKE ED_CLOSE true 3)

def RR_WATER_TEMPLE_CRACKED_WALL SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Cracked Wall
//Checks
RC_WATER_TEMPLE_CRACKED_WALL_CHEST HasExplosives
//Exits
RR_WATER_TEMPLE_EAST_LOWER true

def RR_WATER_TEMPLE_TORCH_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Torch Room
//Checks
RC_WATER_TEMPLE_TORCHES_CHEST (CanKillEnemy RE_SHELL_BLADE ED_CLOSE true 3)
//Exits
RR_WATER_TEMPLE_EAST_LOWER (CanKillEnemy RE_SHELL_BLADE ED_CLOSE true 3)

def RR_WATER_TEMPLE_NORTH_LOWER SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple North Lower
//Exits
RR_WATER_TEMPLE_LOBBY true
RR_WATER_TEMPLE_BOULDERS_LOWER (and (or (CanUse RG_LONGSHOT) (and RT_WATER_BK_REGION (CanUse RG_HOVER_BOOTS))) (SmallKeys SCENE_WATER_TEMPLE 4))

def RR_WATER_TEMPLE_BOULDERS_LOWER SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Boulders Lower
//Checks
RC_WATER_TEMPLE_GS_NEAR_BOSS_KEY_CHEST (or (CanUse RG_LONGSHOT) (Here (or (and IsAdult HookshotOrBoomerang) (and (CanUse RG_IRON_BOOTS) (CanUse RG_HOOKSHOT)))))
//Exits
RR_WATER_TEMPLE_NORTH_LOWER (SmallKeys SCENE_WATER_TEMPLE 4)
RR_WATER_TEMPLE_BLOCK_ROOM true
RR_WATER_TEMPLE_BOULDERS_UPPER (or (and IsAdult (or (CanUse RG_HOVER_BOOTS) RT_WATER_NORTH_BASEMENT_LEDGE_JUMP)) (and (CanUse RG_HOVER_BOOTS) (CanUse RG_IRON_BOOTS)))

def RR_WATER_TEMPLE_BLOCK_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Block Room
//Checks
RC_WATER_TEMPLE_BASEMENT_BLOCK_PUZZLE_POT_1 CanBreakPots
RC_WATER_TEMPLE_BASEMENT_BLOCK_PUZZLE_POT_2 CanBreakPots
//Exits
RR_WATER_TEMPLE_BOULDERS_LOWER (or (and (HasItem RG_GORONS_BRACELET) HasExplosives) (CanUse RG_HOOKSHOT))
RR_WATER_TEMPLE_JETS_ROOM (or (and (HasItem RG_GORONS_BRACELET) HasExplosives) (and (CanUse RG_HOOKSHOT) (CanUse RG_HOVER_BOOTS)))

def RR_WATER_TEMPLE_JETS_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Jets Room
//Exits
RR_WATER_TEMPLE_BLOCK_ROOM (CanUse RG_HOOKSHOT)
RR_WATER_TEMPLE_BOULDERS_UPPER true

def RR_WATER_TEMPLE_BOULDERS_UPPER SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Boulders Upper
//Exits
RR_WATER_TEMPLE_BOULDERS_LOWER true
RR_WATER_TEMPLE_JETS_ROOM IsAdult
RR_WATER_TEMPLE_BOSS_KEY_ROOM (and (or (CanUse RG_IRON_BOOTS) (and IsAdult RT_WATER_BK_JUMP_DIVE)) (SmallKeys SCENE_WATER_TEMPLE 5))

def RR_WATER_TEMPLE_BOSS_KEY_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Boss Key Room
//Events
FairyPot true
//Checks
RC_WATER_TEMPLE_BOSS_KEY_CHEST true
RC_WATER_TEMPLE_BOSS_KEY_POT_1 CanBreakPots
RC_WATER_TEMPLE_BOSS_KEY_POT_2 CanBreakPots
//Exits
RR_WATER_TEMPLE_BOULDERS_UPPER (and (or (CanUse RG_IRON_BOOTS) (or (and IsAdult RT_WATER_BK_JUMP_DIVE) (or IsChild (HasItem RG_SILVER_SCALE)))) (SmallKeys SCENE_WATER_TEMPLE 5))

def RR_WATER_TEMPLE_SOUTH_LOWER SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple South Lower
//Checks
RC_WATER_TEMPLE_GS_BEHIND_GATE (and CanJumpslash (or (CanUse RG_HOOKSHOT) (and IsAdult (CanUse RG_HOVER_BOOTS))))
RC_WATER_TEMPLE_BEHIND_GATE_POT_1 (and CanJumpslash (or (CanUse RG_HOOKSHOT) (and IsAdult (CanUse RG_HOVER_BOOTS))))
RC_WATER_TEMPLE_BEHIND_GATE_POT_2 (and CanJumpslash (or (CanUse RG_HOOKSHOT) (and IsAdult (CanUse RG_HOVER_BOOTS))))
RC_WATER_TEMPLE_BEHIND_GATE_POT_3 (and CanJumpslash (or (CanUse RG_HOOKSHOT) (and IsAdult (CanUse RG_HOVER_BOOTS))))
RC_WATER_TEMPLE_BEHIND_GATE_POT_4 (and CanJumpslash (or (CanUse RG_HOOKSHOT) (and IsAdult (CanUse RG_HOVER_BOOTS))))
//Exits
RR_WATER_TEMPLE_LOBBY (CanUse RG_IRON_BOOTS)

def RR_WATER_TEMPLE_WEST_LOWER SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple West Lower
//Exits
RR_WATER_TEMPLE_LOBBY (and (CanUse RG_HOOKSHOT) (and (CanUse RG_IRON_BOOTS) (HasItem RG_GORONS_BRACELET)))
RR_WATER_TEMPLE_DRAGON_ROOM (or CanJumpslashExceptHammer CanUseProjectile)

def RR_WATER_TEMPLE_DRAGON_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Dragon Room
//Checks
RC_WATER_TEMPLE_DRAGON_CHEST (or (and (CanUse RG_HOOKSHOT) (CanUse RG_IRON_BOOTS)) (or (and (or (and IsAdult (and RT_WATER_ADULT_DRAGON (or (CanUse RG_HOOKSHOT) (or (CanUse RG_FAIRY_BOW) (CanUse RG_BOMBCHU_5))))) (and IsChild (and RT_WATER_CHILD_DRAGON (or (CanUse RG_FAIRY_SLINGSHOT) (or (CanUse RG_BOOMERANG) (CanUse RG_BOMBCHU_5)))))) (or (HasItem RG_SILVER_SCALE) (CanUse RG_IRON_BOOTS))) (Here (and IsAdult (and (CanUse RG_FAIRY_BOW) (or (and RT_WATER_ADULT_DRAGON (or (HasItem RG_SILVER_SCALE) (CanUse RG_IRON_BOOTS))) RT_WATER_DRAGON_JUMP_DIVE))))))
//Exits
RR_WATER_TEMPLE_WEST_LOWER true

def RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Central Pillar Lower
//Exits
RR_WATER_TEMPLE_LOBBY (SmallKeys SCENE_WATER_TEMPLE 5)
RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER (CanUse RG_HOOKSHOT)
RR_WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT (and CanWaterTempleMiddle (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 40)))

def RR_WATER_TEMPLE_CENTRAL_PILLAR_UPPER SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Central Pillar Upper
//Events
CanWaterTempleMiddle (CanUse RG_ZELDAS_LULLABY)
//Checks
RC_WATER_TEMPLE_GS_CENTRAL_PILLAR (or (CanUse RG_LONGSHOT) (and (or (and RT_WATER_FW_CENTRAL_GS (and (CanUse RG_FARORES_WIND) (or (CanUse RG_FAIRY_BOW) (or (CanUse RG_DINS_FIRE) (SmallKeys SCENE_WATER_TEMPLE 5))))) (and RT_WATER_IRONS_CENTRAL_GS (and (CanUse RG_IRON_BOOTS) (or (and (CanUse RG_HOOKSHOT) (CanUse RG_FAIRY_BOW)) (CanUse RG_DINS_FIRE))))) (and CanWaterTempleHigh HookshotOrBoomerang)))
//Exits
RR_WATER_TEMPLE_LOBBY true
RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER true

def RR_WATER_TEMPLE_CENTRAL_PILLAR_BASEMENT SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Central Pillar Basement
//Checks
RC_WATER_TEMPLE_CENTRAL_PILLAR_CHEST (and (CanUse RG_HOOKSHOT) (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 40)))
//Exits
RR_WATER_TEMPLE_CENTRAL_PILLAR_LOWER (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16))

def RR_WATER_TEMPLE_EAST_MIDDLE SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple East Middle
//Checks
RC_WATER_TEMPLE_COMPASS_CHEST CanUseProjectile
RC_WATER_TEMPLE_NEAR_COMPASS_POT_1 CanBreakPots
RC_WATER_TEMPLE_NEAR_COMPASS_POT_2 CanBreakPots
RC_WATER_TEMPLE_NEAR_COMPASS_POT_3 CanBreakPots
//Exits
RR_WATER_TEMPLE_LOBBY (CanUse RG_IRON_BOOTS)

def RR_WATER_TEMPLE_WEST_MIDDLE SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple West Middle
//Exits
RR_WATER_TEMPLE_LOBBY true
RR_WATER_TEMPLE_HIGH_WATER CanUseProjectile

def RR_WATER_TEMPLE_HIGH_WATER SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple High Water
//Events
CanWaterTempleHigh (CanUse RG_ZELDAS_LULLABY)
//Exits
RR_WATER_TEMPLE_LOBBY true

def RR_WATER_TEMPLE_BLOCK_CORRIDOR SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Block Corridor
//Checks
RC_WATER_TEMPLE_CENTRAL_BOW_TARGET_CHEST (and (HasItem RG_GORONS_BRACELET) (or CanWaterTempleLowFromHigh CanWaterTempleMiddle))
RC_WATER_TEMPLE_CENTRAL_BOW_POT_1 (and CanBreakPots (and (HasItem RG_GORONS_BRACELET) (or CanWaterTempleLowFromHigh CanWaterTempleMiddle)))
RC_WATER_TEMPLE_CENTRAL_BOW_POT_2 (and CanBreakPots (and (HasItem RG_GORONS_BRACELET) (or CanWaterTempleLowFromHigh CanWaterTempleMiddle)))
//Exits
RR_WATER_TEMPLE_LOBBY (CanUse RG_HOOKSHOT)

def RR_WATER_TEMPLE_FALLING_PLATFORM_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Falling Platform Room
//Checks
RC_WATER_TEMPLE_GS_FALLING_PLATFORM_ROOM (or (CanUse RG_LONGSHOT) (or (and RT_WATER_RANG_FALLING_PLATFORM_GS (and IsChild (CanUse RG_BOOMERANG))) (and RT_WATER_HOOKSHOT_FALLING_PLATFORM_GS (and IsAdult (CanUse RG_HOOKSHOT)))))
//Exits
RR_WATER_TEMPLE_LOBBY (and (CanUse RG_HOOKSHOT) (SmallKeys SCENE_WATER_TEMPLE 4))
RR_WATER_TEMPLE_DRAGON_PILLARS_ROOM (and (CanUse RG_HOOKSHOT) (SmallKeys SCENE_WATER_TEMPLE 5))

def RR_WATER_TEMPLE_DRAGON_PILLARS_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Dragon Pillars Room
//Checks
RC_WATER_TEMPLE_LIKE_LIKE_POT_1 (CanUse RG_HOOKSHOT)
RC_WATER_TEMPLE_LIKE_LIKE_POT_2 (CanUse RG_HOOKSHOT)
//Exits
RR_WATER_TEMPLE_FALLING_PLATFORM_ROOM CanUseProjectile
RR_WATER_TEMPLE_DARK_LINK_ROOM (CanUse RG_HOOKSHOT)

def RR_WATER_TEMPLE_DARK_LINK_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Dark Link Room
//Exits
RR_WATER_TEMPLE_DRAGON_PILLARS_ROOM (CanKillEnemy RE_DARK_LINK)
RR_WATER_TEMPLE_LONGSHOT_ROOM (CanKillEnemy RE_DARK_LINK)

def RR_WATER_TEMPLE_LONGSHOT_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Longshot Room
//Checks
RC_WATER_TEMPLE_LONGSHOT_CHEST true
//Exits
RR_WATER_TEMPLE_DARK_LINK_ROOM true
RR_WATER_TEMPLE_RIVER (or IsChild (CanUse RG_SONG_OF_TIME))

def RR_WATER_TEMPLE_RIVER SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple River
//Checks
RC_WATER_TEMPLE_RIVER_CHEST (and (or (CanUse RG_FAIRY_SLINGSHOT) (CanUse RG_FAIRY_BOW)) (or IsAdult (or (CanUse RG_HOVER_BOOTS) (CanUse RG_HOOKSHOT))))
RC_WATER_TEMPLE_GS_RIVER (or (and (CanUse RG_IRON_BOOTS) (CanUse RG_HOOKSHOT)) (and RT_WATER_RIVER_GS (CanUse RG_LONGSHOT)))
RC_WATER_TEMPLE_RIVER_POT_1 CanBreakPots
RC_WATER_TEMPLE_RIVER_POT_2 CanBreakPots
RC_WATER_TEMPLE_RIVER_HEART_1 (or (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)) (HasItem RG_BRONZE_SCALE))
RC_WATER_TEMPLE_RIVER_HEART_2 (or (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)) (HasItem RG_BRONZE_SCALE))
RC_WATER_TEMPLE_RIVER_HEART_3 (or (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)) (HasItem RG_BRONZE_SCALE))
RC_WATER_TEMPLE_RIVER_HEART_4 (or (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 24)) (HasItem RG_BRONZE_SCALE))
//Exits
RR_WATER_TEMPLE_DRAGON_ROOM (and (or (CanUse RG_FAIRY_SLINGSHOT) (CanUse RG_FAIRY_BOW)) (or IsAdult (or (CanUse RG_HOVER_BOOTS) (CanUse RG_HOOKSHOT))))

def RR_WATER_TEMPLE_PRE_BOSS_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Pre Boss Room
//Events
FairyPot true
//Checks
RC_WATER_TEMPLE_MAIN_LEVEL_1_POT_1 CanBreakPots
RC_WATER_TEMPLE_MAIN_LEVEL_1_POT_2 CanBreakPots
//Exits
RR_WATER_TEMPLE_LOBBY true
RR_WATER_TEMPLE_BOSS_ENTRYWAY true

def RR_WATER_TEMPLE_MQ_3F_SOUTH_LEDGE SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ 3F South Ledge
//Exits
RR_WATER_TEMPLE_ENTRYWAY (or (HasItem RG_BRONZE_SCALE) (CanUse RG_IRON_BOOTS))
RR_WATER_TEMPLE_MQ_MAIN true
//If we are not on WL_HIGH, we reach RR_WATER_TEMPLE_MQ_3F_MAIN with hookshot via 2F, otherwise we can reach the platform
RR_WATER_TEMPLE_MQ_3F_CENTRAL (or (CanUse RG_HOOKSHOT) (CanUse RG_HOVER_BOOTS))
RR_WATER_TEMPLE_MQ_2F_CENTRAL (MQWaterLevel WL_LOW_OR_MID)

//This region covers simply existing in the area around the central pillar without being on a specific platform, either swimming or walking on the lakebed
//Entry should only include being in the correct area, taking any possible fall damage, and floating up to the surface of WL_HIGH if coming from below
//This area then leads to others based on level and worst-case water timers for follow-up exits from the water's surface
//remember that any solution that works for any level doesn't need to be given a level, even if that solution is overkill for a lower level
def RR_WATER_TEMPLE_MQ_MAIN SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Main
//Exits
RR_WATER_TEMPLE_MQ_3F_SOUTH_LEDGE (and (HasItem RG_BRONZE_SCALE) (MQWaterLevel WL_HIGH))
//Jumping across is possible but a trick due to the janky ledge
RR_WATER_TEMPLE_MQ_EAST_TOWER (or (and (>= WaterTimer 24) (CanUse RG_IRON_BOOTS)) (or (and (MQWaterLevel WL_MID) (and (HasItem RG_GOLDEN_SCALE) (>= WaterTimer 16))) (MQWaterLevel WL_LOW)))
RR_WATER_TEMPLE_MQ_3F_CENTRAL (and (MQWaterLevel WL_HIGH) (HasItem RG_BRONZE_SCALE))
//First water timer uses the hook to go from the top of center to storage room/central pillar as coming from the bottom
//Second water timer is simply diving down and entering the door as fast as possible from the surface
RR_WATER_TEMPLE_MQ_2F_CENTRAL (or (and (or (MQWaterLevel WL_LOW) (and (CanUse RG_IRON_BOOTS) (or (MQWaterLevel WL_MID) (>= WaterTimer 16)))) (CanUse RG_LONGSHOT)) (and (or (MQWaterLevel WL_MID) (and (MQWaterLevel WL_HIGH_OR_MID) (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 8)))) (HasItem RG_BRONZE_SCALE)))
RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_1F (MQWaterLevel WL_LOW)
//A special entry as we can't set it to high after entering at a lower height
RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH (and (MQWaterLevel WL_HIGH) (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) (or (HasItem RG_BRONZE_SCALE) (CanUse RG_LONGSHOT)))))
RR_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY (and (or (MQWaterLevel WL_MID) (and (MQWaterLevel WL_HIGH_OR_MID) (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))) (HasItem RG_BRONZE_SCALE))
RR_WATER_TEMPLE_MQ_B1_GATE_SWITCH (and MQWaterB1Switch (or (MQWaterLevel WL_LOW) (and (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 24)) (HasItem RG_BRONZE_SCALE))))
RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_ROOM (and MQWaterB1Switch (or (and (MQWaterLevel WL_LOW) (HasItem RG_SILVER_SCALE)) (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) (or (HasItem RG_BRONZE_SCALE) (CanUse RG_LONGSHOT))))))
//Adult needs to jump in instead of dive for swim access, but you just hold forward. RT_WATER_BK_REGION isn't relevant unless the Dark Link loop can be done without longshot with other tricks
RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_ROOM (and MQWaterB1Switch (and (or (and (MQWaterLevel WL_LOW) (HasItem RG_BRONZE_SCALE)) (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) (CanUse RG_HOOKSHOT)))) (or (CanUse RG_LONGSHOT) (and RT_WATER_BK_REGION (CanUse RG_HOVER_BOOTS)))))

//This region specifically covers the topmost platform around central pillar
def RR_WATER_TEMPLE_MQ_3F_CENTRAL SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ 3F Central
//Exits
RR_WATER_TEMPLE_MQ_MAIN true
RR_WATER_TEMPLE_MQ_3F_SOUTH_LEDGE (or (CanUse RG_LONGSHOT) (CanUse RG_HOVER_BOOTS))
RR_WATER_TEMPLE_MQ_2F_CENTRAL (and (or (MQWaterLevel WL_LOW_OR_MID) (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16))) (CanUse RG_HOOKSHOT))
RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH (and (MQWaterLevel WL_HIGH) (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) (CanUse RG_HOOKSHOT))))
RR_WATER_TEMPLE_MQ_3F_NORTH_LEDGE (or (and (MQWaterLevel WL_HIGH) (CanUse RG_LONGSHOT)) (and RT_HOVER_BOOST_SIMPLE (and RT_DAMAGE_BOOST_SIMPLE (and HasExplosives (CanUse RG_HOVER_BOOTS)))))
//Jumping across is possible but a trick due to the janky ledge
RR_WATER_TEMPLE_MQ_HIGH_EMBLEM (or (CanUse RG_HOOKSHOT) (and IsAdult (CanUse RG_HOVER_BOOTS)))
//room access is (logic->IsAdult || (logic->HasItem(RG_BRONZE_SCALE) || logic->CanUse(RG_HOVER_BOOTS) || logic->CanUse(RG_HOOKSHOT)))
RR_WATER_TEMPLE_MQ_WATERFALL (and (SmallKeys SCENE_WATER_TEMPLE 1) (and (MQWaterLevel WL_HIGH) (CanUse RG_LONGSHOT)))
//this swimless jump with irons may be a trick as you have to put irons on quite late.
RR_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY (or (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)) (MQWaterLevel WL_LOW_OR_MID))

//This region specifically covers walking on the lower platform around central pillar. This is underwater when WL_HIGH
//RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH should be accessed directly to use the central pillar door while at WL_HIGH
def RR_WATER_TEMPLE_MQ_2F_CENTRAL SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ 2F Central
//Exits
RR_WATER_TEMPLE_MQ_MAIN true
RR_WATER_TEMPLE_MQ_3F_CENTRAL (CanUse RG_HOOKSHOT)
RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_2F (MQWaterLevel WL_LOW_OR_MID)
RR_WATER_TEMPLE_MQ_STORAGE_ROOM (CanUse RG_HOOKSHOT)
RR_WATER_TEMPLE_MQ_BEHIND_BLUE_SWITCH_2F (and (MQWaterLevel WL_LOW_OR_MID) (and (or IsAdult (CanUse RG_HOVER_BOOTS)) (CanUse RG_HOOKSHOT)))
RR_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY (and (MQWaterLevel WL_LOW_OR_MID) (CanUse RG_HOVER_BOOTS))

def RR_WATER_TEMPLE_MQ_2F_CENTRAL_HIGH SCENE_ID_MAX false
Invalid Region

def RR_WATER_TEMPLE_MQ_HIGH_EMBLEM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ High Emblem
//Events
ReachedWaterHighEmblem true
CanWaterTempleHigh (CanUse RG_ZELDAS_LULLABY)
//Exits
RR_WATER_TEMPLE_MQ_3F_CENTRAL true
RR_WATER_TEMPLE_MQ_MAIN true

def RR_WATER_TEMPLE_MQ_3F_NORTH_LEDGE SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ 3F North Ledge
//what we need if WL_LOW, we can't guarantee repeated access otherwise.
//Exits
RR_WATER_TEMPLE_MQ_MAIN (or (HasItem RG_BRONZE_SCALE) TakeDamage)
RR_WATER_TEMPLE_MQ_3F_CENTRAL (CanUse RG_LONGSHOT)
RR_WATER_TEMPLE_MQ_BOSS_DOOR (or (CanUse RG_LONGSHOT) (or (CanUse RG_ICE_ARROWS) (CanUse RG_NAYRUS_LOVE)))

def RR_WATER_TEMPLE_MQ_BOSS_DOOR SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Main
//Exits
RR_WATER_TEMPLE_MQ_3F_NORTH_LEDGE (or (CanUse RG_ICE_ARROWS) TakeDamage)
RR_WATER_TEMPLE_BOSS_ENTRYWAY true

def RR_WATER_TEMPLE_MQ_EAST_TOWER SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ East Tower
//if we can't reach these, we can't move the water at all, so no need to specify level or account for WL_LOW access here
//review is some way to play ocarina underwater exists
//Events
CouldWaterTempleLow true
CanWaterTempleLowFromHigh (CanUse RG_ZELDAS_LULLABY)
//Checks
RC_WATER_TEMPLE_MQ_MAP_CHEST (and (MQWaterLevel WL_HIGH) (and HasFireSource (CanUse RG_HOOKSHOT)))
RC_WATER_TEMPLE_MQ_LONGSHOT_CHEST (or (and (MQWaterLevel WL_MID) (CanUse RG_HOOKSHOT)) (and (MQWaterLevel WL_HIGH_OR_MID) CanOpenUnderwaterChest))
RC_WATER_TEMPLE_MQ_LOWER_TORCHES_POT_1 (or (and (MQWaterLevel WL_LOW) CanBreakPots) (and (CanUse RG_IRON_BOOTS) (and (CanUse RG_HOOKSHOT) (>= WaterTimer 16))))
RC_WATER_TEMPLE_MQ_LOWER_TORCHES_POT_2 (or (and (MQWaterLevel WL_LOW) CanBreakPots) (and (CanUse RG_IRON_BOOTS) (and (CanUse RG_HOOKSHOT) (>= WaterTimer 16))))
//Exits
RR_WATER_TEMPLE_MQ_EAST_TOWER_1F_ROOM (and (MQWaterLevel WL_LOW) (or (CanUse RG_FAIRY_BOW) (or (CanUse RG_DINS_FIRE) (CanUse RG_STICKS))))

//Raising the targets by clearing this room achieves nothing logically because it requires WL_LOW to do and hookshot to use, which implies access to WL_MID and WL_HIGH already
def RR_WATER_TEMPLE_MQ_EAST_TOWER_1F_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ East Tower 1F Room
//Checks
RC_WATER_TEMPLE_MQ_COMPASS_CHEST (and (CanKillEnemy RE_LIZALFOS) (CanKillEnemy RE_SPIKE))
//Exits
RR_WATER_TEMPLE_MQ_EAST_TOWER true

//This area assumes we entered through the lower door, so water is low and cannot be changed without leaving.
def RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_1F SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Central Pillar 1F
//This is harder than the other possibilities as you have to move between shots on top of the extra range, but there's basically no universe this should matter.
//Events
MQWaterB1Switch (and RT_WATER_MQ_CENTRAL_PILLAR (CanUse RG_FIRE_ARROWS))
//Exits
RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH (and (MQWaterLevel WL_HIGH) (and RT_WATER_FW_CENTRAL_GS (and (CanUse RG_FARORES_WIND) (HasItem RG_BRONZE_SCALE))))
//I don't know if this FW trick can ever matter but maybe it's needed to get child to CENTRAL_2F or something
RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_2F (or (CanUse RG_HOOKSHOT) (and (MQWaterLevel WL_MID) (and RT_WATER_FW_CENTRAL_GS (and (CanUse RG_FARORES_WIND) (HasItem RG_BRONZE_SCALE)))))
//if the gate is open, you sink straight in, so you can't climb up this way in logic without swimming
RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_B1 (and MQWaterOpenedPillarB1 (and (MQWaterLevel WL_HIGH_OR_MID) (and RT_WATER_FW_CENTRAL_GS (and (CanUse RG_FARORES_WIND) (and (CanUse RG_IRON_BOOTS) (CanUse RG_ZORA_TUNIC))))))

//If we enter here in WL_HIGH, go to RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH instead, Assumes WL_MID_OR_LOW
def RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_2F SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Central Pillar 2F
//Events
CouldWaterTempleMiddle true
CanWaterTempleMiddle (CanUse RG_ZELDAS_LULLABY)
//It's possible to do this even on low water, but more awkward. I'm not sure if it's even possible for it to be relevant though.
MQWaterOpenedPillarB1 (and RT_WATER_MQ_CENTRAL_PILLAR (CanUse RG_FIRE_ARROWS))
//Exits
RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH (and (MQWaterLevel WL_HIGH) (and (CanUse RG_FARORES_WIND) (or (HasItem RG_BRONZE_SCALE) (CanUse RG_IRON_BOOTS))))
RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_B1 (and MQWaterOpenedPillarB1 (and (MQWaterLevel WL_MID) (and (CanUse RG_IRON_BOOTS) (CanUse RG_ZORA_TUNIC))))

def RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_HIGH SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Central Pillar High
//Events
MQWaterOpenedPillarB1 (and (or (and (CanUse RG_SONG_OF_TIME) (CanUse RG_DINS_FIRE)) (and RT_WATER_MQ_CENTRAL_PILLAR (CanUse RG_FIRE_ARROWS))) (or (HasItem RG_BRONZE_SCALE) (and (CanUse RG_IRON_BOOTS) (and (CanUse RG_LONGSHOT) CanJumpslash))))
//Checks
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_UPPER_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_UPPER_CRATE_2 CanBreakCrates
//Exits
RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_B1 (and MQWaterB1Switch (and (CanUse RG_IRON_BOOTS) (CanUse RG_ZORA_TUNIC)))

//Assuming tunic and irons was checked on entry
def RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_B1 SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Central Pillar B1
//Can't know water level, so we'll just assume any possibility and skip to MAIN
//Exits
RR_WATER_TEMPLE_MQ_MAIN (and MQWaterOpenedPillarB1 (and (CanUse RG_IRON_BOOTS) (HasItem RG_BRONZE_SCALE)))
//Child needs to release irons for height to push down the larger "peg", however they can push the lower one down by climbing and then hit the switch through the larger peg, but it's a trick
RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_B1_FINAL (or (and IsAdult (CanUse RG_LONGSHOT)) (and (CanUse RG_HOOKSHOT) (HasItem RG_BRONZE_SCALE)))

def RR_WATER_TEMPLE_MQ_CENTRAL_PILLAR_B1_FINAL SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Central Pillar B1 Final
//Checks
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_CHEST (CanUse RG_HOOKSHOT)
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_2 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_3 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_4 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_5 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_6 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_7 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_8 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_9 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_10 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_11 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_12 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_13 CanBreakCrates
RC_WATER_TEMPLE_MQ_CENTRAL_PILLAR_LOWER_CRATE_14 CanBreakCrates

def RR_WATER_TEMPLE_MQ_BEHIND_BLUE_SWITCH_2F SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Behind Blue Switch 2F
//Checks
RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_POT_1 CanBreakPots
RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_POT_2 CanBreakPots
RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_POT_3 CanBreakPots
RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_CRATE_2 CanBreakCrates
RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_CRATE_3 CanBreakCrates
RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_CRATE_4 CanBreakCrates
RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_CRATE_5 CanBreakCrates
RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_CRATE_6 CanBreakCrates
RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_LOWER_SMALL_CRATE CanBreakSmallCrates
RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_UPPER_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_UPPER_CRATE_2 CanBreakCrates
RC_WATER_TEMPLE_MQ_GS_STORAGE_ROOM_UPPER_SMALL_CRATE CanBreakSmallCrates
//Exits
RR_WATER_TEMPLE_MQ_MAIN true
RR_WATER_TEMPLE_MQ_BEHIND_BLUE_SWITCH_3F (CanUse RG_LONGSHOT)

def RR_WATER_TEMPLE_MQ_BEHIND_BLUE_SWITCH_3F SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Behind Blue Switch 2F
//Checks
RC_WATER_TEMPLE_MQ_GS_BEFORE_UPPER_WATER_SWITCH (CanGetEnemyDrop RE_GOLD_SKULLTULA)
//Exits
RR_WATER_TEMPLE_MQ_BEHIND_BLUE_SWITCH_2F true
RR_WATER_TEMPLE_MQ_HIGH_EMBLEM true

def RR_WATER_TEMPLE_MQ_STORAGE_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Storage Room
//Checks
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_POT_1 CanBreakPots
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_POT_2 CanBreakPots
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_POT_3 CanBreakPots
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_2 CanBreakCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_3 CanBreakCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_4 CanBreakCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_5 CanBreakCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_6 CanBreakCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_CRATE_7 CanBreakCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_SMALL_CRATE_1 CanBreakSmallCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_SMALL_CRATE_2 CanBreakSmallCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_SMALL_CRATE_3 CanBreakSmallCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_A_SMALL_CRATE_4 CanBreakSmallCrates
//Exits
RR_WATER_TEMPLE_MQ_MAIN (or (MQWaterLevel WL_LOW_OR_MID) (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 8)))

def RR_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Lizalfos Hallway
//Checks
RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_WEST_POT CanBreakPots
RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_SOUTH_POT CanBreakPots
RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_SE_POT CanBreakPots
RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_CRATE_2 CanBreakCrates
RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_CRATE_3 CanBreakCrates
RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_2 CanBreakCrates
RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_3 CanBreakCrates
RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_4 CanBreakCrates
RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_ROOM_CRATE_5 CanBreakCrates
//Exits
RR_WATER_TEMPLE_MQ_LIZALFOS_CAGE (and (MQWaterLevel WL_LOW_OR_MID) (CanUse RG_DINS_FIRE))
//this technically exists, but only complicates things, uncomment if some edge case/glitch can use RR_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY to reach RR_WATER_TEMPLE_MQ_3F_CENTRAL, or if a void warp goes here
//RR_WATER_TEMPLE_MQ_3F_EAST_LEDGE (or (and (CanUse RG_HOOKSHOT) (or (HasItem RG_BRONZE_SCALE) (CanUse RG_IRON_BOOTS))) (and (MQWaterLevel WL_LOW_OR_MID) (CanUse RG_HOOKSHOT)) (and (MQWaterLevel WL_HIGH) (HasItem RG_BRONZE_SCALE)))

def RR_WATER_TEMPLE_MQ_LIZALFOS_CAGE SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Lizalfos Cage
//Checks
RC_WATER_TEMPLE_MQ_GS_LIZALFOS_HALLWAY (CanKillEnemy RE_GOLD_SKULLTULA)
RC_WATER_TEMPLE_MQ_LIZALFOS_CAGE_SOUTH_POT CanBreakPots
RC_WATER_TEMPLE_MQ_LIZALFOS_CAGE_NORTH_POT CanBreakPots
RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_GATE_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_LIZALFOS_HALLWAY_GATE_CRATE_2 CanBreakCrates

def RR_WATER_TEMPLE_MQ_3F_EAST_LEDGE SCENE_ID_MAX false
Invalid Region

//This room exists to hold the wonderitems that drop from the emblems here. Specifically this assumes you are standing on the final ledge
def RR_WATER_TEMPLE_MQ_WATERFALL SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Waterfall
//Exits
RR_WATER_TEMPLE_MQ_3F_CENTRAL (and (SmallKeys SCENE_WATER_TEMPLE 1) (CanUse RG_LONGSHOT))
RR_WATER_TEMPLE_MQ_STALFOS_PIT true
RR_WATER_TEMPLE_MQ_STALFOS_PIT_POTS (or (and MQWaterStalfosPit (and IsAdult (CanUse RG_HOOKSHOT))) (CanUse RG_HOVER_BOOTS))
RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER (and MQWaterStalfosPit (CanUse RG_LONGSHOT))

def RR_WATER_TEMPLE_MQ_STALFOS_PIT SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Stalfos Pit
//Events
MQWaterStalfosPit (or (and IsAdult (CanKillEnemy RE_STALFOS ED_CLOSE true 3 false true)) (and (CanUse RG_IRON_BOOTS) (and (CanUse RG_HOOKSHOT) (CanKillEnemy RE_STALFOS ED_BOMB_THROW true 3 false true))))
//Exits
RR_WATER_TEMPLE_MQ_WATERFALL (and MQWaterStalfosPit (and (CanUse RG_HOOKSHOT) (or IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 8)))))
RR_WATER_TEMPLE_MQ_STALFOS_PIT_POTS (or (and IsAdult (CanUse RG_HOOKSHOT)) (and (CanUse RG_HOOKSHOT) (and (or IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 8))) (or (CanUse RG_HOVER_BOOTS) MQWaterStalfosPit))))
RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER (and MQWaterStalfosPit (and (or IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 8))) (CanUse RG_HOOKSHOT)))

//also includes the suns fairy in the middle
def RR_WATER_TEMPLE_MQ_STALFOS_PIT_POTS SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Stalfos Pit Pots
//Events
FairyPot true
NutPot true
//Checks
RC_WATER_TEMPLE_MQ_STALFOS_PIT_SOUTH_POT CanBreakPots
RC_WATER_TEMPLE_MQ_STALFOS_PIT_MIDDLE_POT CanBreakPots
RC_WATER_TEMPLE_MQ_STALFOS_PIT_NORTH_POT CanBreakPots
RC_WATER_TEMPLE_MQ_DARK_LINK_PILAR_SUN_FAIRY (CanUse RG_SUNS_SONG)
//Exits
RR_WATER_TEMPLE_MQ_WATERFALL (and MQWaterStalfosPit (or (CanUse RG_HOVER_BOOTS) (CanUse RG_LONGSHOT)))
RR_WATER_TEMPLE_MQ_STALFOS_PIT true
RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER (and MQWaterStalfosPit (CanUse RG_HOOKSHOT))

//specifically the area past the spikes
def RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Stalfos Pit Upper
//Checks
RC_WATER_TEMPLE_MQ_BEFORE_DARK_LINK_POT_1 CanBreakPots
RC_WATER_TEMPLE_MQ_BEFORE_DARK_LINK_POT_2 CanBreakPots
RC_WATER_TEMPLE_MQ_DARK_LINK_LEFT_STORM_FAIRY (CanUse RG_SONG_OF_STORMS)
RC_WATER_TEMPLE_MQ_DARK_LINK_RIGHT_SUN_FAIRY (CanUse RG_SUNS_SONG)
//Exits
RR_WATER_TEMPLE_MQ_STALFOS_PIT (or IsAdult TakeDamage)
RR_WATER_TEMPLE_MQ_STALFOS_PIT_POTS (or IsAdult TakeDamage)
RR_WATER_TEMPLE_MQ_AFTER_DARK_LINK (CanKillEnemy RE_DARK_LINK)

def RR_WATER_TEMPLE_MQ_AFTER_DARK_LINK SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ After Dark Link
//Events
FairyPot true
//Checks
RC_WATER_TEMPLE_MQ_AFTER_DARK_LINK_POT_1 CanBreakPots
RC_WATER_TEMPLE_MQ_AFTER_DARK_LINK_POT_2 CanBreakPots
//Exits
RR_WATER_TEMPLE_MQ_STALFOS_PIT_UPPER (CanKillEnemy RE_DARK_LINK)
RR_WATER_TEMPLE_MQ_RIVER_SKULL (and (CanUse RG_HOOKSHOT) (or (HasItem RG_BRONZE_SCALE) (or (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 8)) (CanUse RG_LONGSHOT))))

//if we can use hookshot, we are standing on the targets, otherwise assume we're in the water
def RR_WATER_TEMPLE_MQ_RIVER_SKULL SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ River Skull
//Checks
RC_WATER_TEMPLE_MQ_GS_RIVER (or (CanUse RG_LONGSHOT) (and (CanUse RG_IRON_BOOTS) (CanUse RG_HOOKSHOT)))
//Exits
RR_WATER_TEMPLE_MQ_RIVER_POTS (or (HasItem RG_BRONZE_SCALE) (CanUse RG_LONGSHOT))

def RR_WATER_TEMPLE_MQ_RIVER_POTS SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ River Pots
//Events
FairyPot true
//Checks
RC_WATER_TEMPLE_MQ_RIVER_POT_1 CanBreakPots
RC_WATER_TEMPLE_MQ_RIVER_POT_2 CanBreakPots
//Exits
RR_WATER_TEMPLE_MQ_RIVER_SKULL (or (CanUse RG_LONGSHOT) (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 8) (CanUse RG_HOOKSHOT))))
//You don't need to swim for this if you put irons on in midair and hold forward while aiming for the tunnel with a tight angle, but if you miss you have to void unless you have a hook. It's only relevant with glitches anyway
RR_WATER_TEMPLE_MQ_DRAGON_ROOM_TUNNEL (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16))
RR_WATER_TEMPLE_MQ_DRAGON_ROOM_ALCOVE (or (HasItem RG_SILVER_SCALE) (and IsAdult (and (HasItem RG_BRONZE_SCALE) RT_WATER_DRAGON_JUMP_DIVE)))
RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR (or (HasItem RG_BRONZE_SCALE) (or (CanUse RG_LONGSHOT) (and (CanUse RG_HOVER_BOOTS) CanJumpslash)))

def RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Dragon Room Door
//Checks
RC_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR_CRATE_2 CanBreakCrates
//Exits
RR_WATER_TEMPLE_MQ_RIVER_POTS (CanUse RG_LONGSHOT)
RR_WATER_TEMPLE_MQ_DRAGON_ROOM_TUNNEL (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) (CanUse RG_HOOKSHOT)))
RR_WATER_TEMPLE_MQ_DRAGON_ROOM_ALCOVE (HasItem RG_SILVER_SCALE)
RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH MQWaterDragonTorches

//This region assumes Iron boots to access
def RR_WATER_TEMPLE_MQ_DRAGON_ROOM_TUNNEL SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Dragon Room Tunnel
//Checks
RC_WATER_TEMPLE_MQ_DRAGON_ROOM_SUBMERGED_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_DRAGON_ROOM_SUBMERGED_CRATE_2 CanBreakCrates
RC_WATER_TEMPLE_MQ_DRAGON_ROOM_SUBMERGED_CRATE_3 CanBreakCrates
RC_WATER_TEMPLE_MQ_DRAGON_ROOM_SUBMERGED_CRATE_4 CanBreakCrates
//Exits
RR_WATER_TEMPLE_MQ_RIVER_POTS (CanUse RG_LONGSHOT)
RR_WATER_TEMPLE_MQ_DRAGON_ROOM_ALCOVE (or (HasItem RG_BRONZE_SCALE) (CanUse RG_HOOKSHOT))
RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR (or (HasItem RG_BRONZE_SCALE) (CanUse RG_LONGSHOT))

def RR_WATER_TEMPLE_MQ_DRAGON_ROOM_ALCOVE SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Dragon Room Alcove
//Events
MQWaterDragonTorches HasFireSource
//Checks
RC_WATER_TEMPLE_MQ_DRAGON_ROOM_TORCHES_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_DRAGON_ROOM_TORCHES_CRATE_2 CanBreakCrates
RC_WATER_TEMPLE_MQ_DRAGON_ROOM_TORCHES_SMALL_CRATE_1 CanBreakSmallCrates
RC_WATER_TEMPLE_MQ_DRAGON_ROOM_TORCHES_SMALL_CRATE_2 CanBreakSmallCrates
RC_WATER_TEMPLE_MQ_DRAGON_ROOM_TORCHES_SMALL_CRATE_3 CanBreakSmallCrates
//Exits
RR_WATER_TEMPLE_MQ_DRAGON_ROOM_TUNNEL (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16))
RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR (HasItem RG_SILVER_SCALE)

def RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Boss Key Room Switch
//Checks
RC_WATER_TEMPLE_MQ_BOSS_KEY_POT CanBreakPots
RC_WATER_TEMPLE_MQ_BK_ROOM_UPPER_CRATE CanBreakCrates
//Exits
RR_WATER_TEMPLE_MQ_DRAGON_ROOM_DOOR true
RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_PIT true
RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_CHEST (and CanHitSwitch (Here (CanUse RG_DINS_FIRE)))

//this exists for the crates in preparation for clips through the grate
def RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_PIT SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Boss Key Room Pit
//Checks
RC_WATER_TEMPLE_MQ_BK_ROOM_LOWER_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_BK_ROOM_LOWER_CRATE_2 CanBreakCrates
RC_WATER_TEMPLE_MQ_BK_ROOM_LOWER_CRATE_3 CanBreakCrates
RC_WATER_TEMPLE_MQ_BK_ROOM_LOWER_CRATE_4 CanBreakCrates
//Exits
RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH (CanHitSwitch ED_BOOMERANG)

def RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_CHEST SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Boss Key Room Chest
//Checks
RC_WATER_TEMPLE_MQ_BOSS_KEY_CHEST true
//Exits
RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_SWITCH (or (CanHitSwitch ED_BOMB_THROW) (CanUse RG_HOVER_BOOTS))
RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_PIT true
RR_WATER_TEMPLE_MQ_B1_GATE_SWITCH (or (HasItem RG_SILVER_SCALE) (and (CanUse RG_IRON_BOOTS) (or (HasItem RG_BRONZE_SCALE) (and (>= WaterTimer 24) (CanUse RG_LONGSHOT)))))

def RR_WATER_TEMPLE_MQ_B1_GATE_SWITCH SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ B1 Gate Switch
//If the water is low, the switch is underwater and needs irons to press, otherwise, the water is too low to climb up and you need irons to hookshot a target
//If a glitch clips through the gate on low, have it logically press the switch and let entrance logic enter
//Events
MQWaterB1Switch (CanUse RG_IRON_BOOTS)
//Exits
RR_WATER_TEMPLE_MQ_MAIN (and MQWaterB1Switch (or (MQWaterLevel WL_LOW) (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16))))
RR_WATER_TEMPLE_MQ_BOSS_KEY_ROOM_CHEST (and (CanUse RG_IRON_BOOTS) (and (HasItem RG_BRONZE_SCALE) (or (MQWaterLevel WL_LOW) (>= WaterTimer 24))))

def RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Triangle Torch Room
//Checks
RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_1 (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) CanBreakCrates))
RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_2 (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) CanBreakCrates))
RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_3 (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) CanBreakCrates))
RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_4 (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) CanBreakCrates))
RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_5 (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) CanBreakCrates))
RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_SUBMERGED_CRATE_6 (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) CanBreakCrates))
//we can backflip over the spikes, but land in water.
//Exits
RR_WATER_TEMPLE_MQ_MAIN (and MQWaterB1Switch (or (and (MQWaterLevel WL_LOW) (HasItem RG_GOLDEN_SCALE)) (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 40) (or (HasItem RG_BRONZE_SCALE) (CanUse RG_LONGSHOT))))))
RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_CAGE (and (CanUse RG_FIRE_ARROWS) (or (and IsAdult (CanUse RG_HOVER_BOOTS)) (and (CanUse RG_LONGSHOT) (Here ScarecrowsSong))))

def RR_WATER_TEMPLE_MQ_TRIANGLE_TORCH_CAGE SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Triangle Torch Cage
//Checks
RC_WATER_TEMPLE_MQ_GS_TRIPLE_WALL_TORCH (CanGetEnemyDrop RE_GOLD_SKULLTULA ED_BOOMERANG)
RC_WATER_TEMPLE_MQ_LOWEST_GS_POT_1 CanBreakPots
RC_WATER_TEMPLE_MQ_LOWEST_GS_POT_2 CanBreakPots
RC_WATER_TEMPLE_MQ_LOWEST_GS_POT_3 CanBreakPots
RC_WATER_TEMPLE_MQ_LOWEST_GS_POT_4 CanBreakPots
RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_GATE_CRATE_3 CanBreakCrates
RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_GATE_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_TRIPLE_TORCH_ROOM_GATE_CRATE_2 CanBreakCrates

def RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Crates Whirlpools Room
//Checks
RC_WATER_TEMPLE_MQ_WHIRLPOOL_FRONT_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_WHIRLPOOL_FRONT_CRATE_2 CanBreakCrates
RC_WATER_TEMPLE_MQ_WHIRLPOOL_SUBMERGED_CRATE_1 (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) CanBreakCrates))
RC_WATER_TEMPLE_MQ_WHIRLPOOL_SUBMERGED_CRATE_2 (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) CanBreakCrates))
RC_WATER_TEMPLE_MQ_WHIRLPOOL_SUBMERGED_CRATE_3 (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) CanBreakCrates))
RC_WATER_TEMPLE_MQ_WHIRLPOOL_SUBMERGED_CRATE_4 (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) CanBreakCrates))
RC_WATER_TEMPLE_MQ_WHIRLPOOL_SUBMERGED_CRATE_5 (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) CanBreakCrates))
RC_WATER_TEMPLE_MQ_WHIRLPOOL_SUBMERGED_CRATE_6 (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 16) CanBreakCrates))
//Exits
RR_WATER_TEMPLE_MQ_MAIN (and MQWaterB1Switch (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 24) (or (CanUse RG_LONGSHOT) (HasItem RG_BRONZE_SCALE)))))
//Child can use crate to get height to make it with hovers, but it's annoyingly tight so would be a trick
RR_WATER_TEMPLE_MQ_SINGLE_STALFOS_ROOM (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 8)
	//putting requirement to get out of the water here as scarecrow method in includes hook which satisfies it
	(or (and IsAdult (and (or (CanUse RG_HOVER_BOOTS) RT_WATER_NORTH_BASEMENT_LEDGE_JUMP) (or (CanUse RG_HOOKSHOT) (HasItem RG_BRONZE_SCALE)))) (and (Here ScarecrowsSong) (CanUse RG_HOOKSHOT)))
)
RR_WATER_TEMPLE_MQ_4_TORCH_ROOM (and IsAdult (or (CanUse RG_HOVER_BOOTS) (or RT_WATER_NORTH_BASEMENT_LEDGE_JUMP (and (Here ScarecrowsSong) (CanUse RG_HOOKSHOT)))))
RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_CAGE (and RT_WATER_MQ_LOCKED_GS (and (CanUse RG_IRON_BOOTS) (CanUse RG_HOOKSHOT)))

def RR_WATER_TEMPLE_MQ_SINGLE_STALFOS_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Single Stalfos Room
//Checks
RC_WATER_TEMPLE_MQ_FREESTANDING_KEY CanBreakCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_POT_1 CanBreakPots
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_POT_2 CanBreakPots
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_CRATE_2 CanBreakCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_CRATE_3 CanBreakCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_CRATE_4 CanBreakCrates
RC_WATER_TEMPLE_MQ_STORAGE_ROOM_B_CRATE_5 CanBreakCrates
//Exits
RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_ROOM (or (HasItem RG_SILVER_SCALE) (or (and IsChild (HasItem RG_BRONZE_SCALE)) (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 8) (or (HasItem RG_BRONZE_SCALE) (CanUse RG_HOOKSHOT))))))

def RR_WATER_TEMPLE_MQ_4_TORCH_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ 4 Torch Room
//Exits
RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_ROOM (or (and IsAdult (or (CanUse RG_HOVER_BOOTS) CanJumpslash)) (or (HasItem RG_BRONZE_SCALE) (and (CanUse RG_IRON_BOOTS) (and (>= WaterTimer 8) (CanUse RG_HOOKSHOT)))))
RR_WATER_TEMPLE_MQ_DODONGO_ROOM (and CanHitSwitch HasFireSource)

def RR_WATER_TEMPLE_MQ_DODONGO_ROOM SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Dodongo Room
//Checks
RC_WATER_TEMPLE_MQ_MINI_DODONGO_POT_1 CanBreakPots
RC_WATER_TEMPLE_MQ_MINI_DODONGO_POT_2 CanBreakPots
RC_WATER_TEMPLE_MQ_DODONGO_ROOM_UPPER_CRATE CanBreakCrates
RC_WATER_TEMPLE_MQ_DODONGO_ROOM_HALL_CRATE CanBreakCrates
RC_WATER_TEMPLE_MQ_DODONGO_ROOM_LOWER_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_DODONGO_ROOM_LOWER_CRATE_2 CanBreakCrates
RC_WATER_TEMPLE_MQ_DODONGO_ROOM_LOWER_CRATE_3 CanBreakCrates
//Exits
RR_WATER_TEMPLE_MQ_4_TORCH_ROOM (and (or (CanUse RG_HOOKSHOT) (CanUse RG_HOVER_BOOTS)) (Here (CanKillEnemy RE_DODONGO ED_CLOSE true 5)))
RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_CAGE (and (or (CanUse RG_HOOKSHOT) (CanUse RG_HOVER_BOOTS)) (Here (CanKillEnemy RE_DODONGO ED_CLOSE true 5)))

def RR_WATER_TEMPLE_MQ_CRATES_WHIRLPOOLS_CAGE SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple MQ Basement Gated Areas
//Checks
RC_WATER_TEMPLE_MQ_GS_FREESTANDING_KEY_AREA (and (CanGetEnemyDrop RE_GOLD_SKULLTULA) CanBreakCrates)
RC_WATER_TEMPLE_MQ_WHIRLPOOL_BEHIND_GATE_CRATE_1 CanBreakCrates
RC_WATER_TEMPLE_MQ_WHIRLPOOL_BEHIND_GATE_CRATE_2 CanBreakCrates
RC_WATER_TEMPLE_MQ_WHIRLPOOL_BEHIND_GATE_CRATE_3 CanBreakCrates
RC_WATER_TEMPLE_MQ_WHIRLPOOL_BEHIND_GATE_CRATE_4 CanBreakCrates
//Exits
RR_WATER_TEMPLE_MQ_DODONGO_ROOM true

def RR_WATER_TEMPLE_BOSS_ENTRYWAY SCENE_WATER_TEMPLE false RA_WATER_TEMPLE
Water Temple Boss Entryway
//Exits
RR_WATER_TEMPLE_PRE_BOSS_ROOM (and (IsDungeonVanilla WATER_TEMPLE) false)
RR_WATER_TEMPLE_MQ_BOSS_DOOR (and (IsDungeonMQ WATER_TEMPLE) false)
RR_WATER_TEMPLE_BOSS_ROOM (HasItem RG_WATER_TEMPLE_BOSS_KEY)

def RR_WATER_TEMPLE_BOSS_ROOM SCENE_WATER_TEMPLE_BOSS false
Water Temple Boss Room
//Events
WaterTempleClear (CanKillEnemy RE_MORPHA)
//Checks
RC_WATER_TEMPLE_MORPHA_HEART WaterTempleClear
RC_MORPHA WaterTempleClear
//Exits
RR_WATER_TEMPLE_BOSS_ENTRYWAY false
RR_LAKE_HYLIA @deprioritize WaterTempleClear
