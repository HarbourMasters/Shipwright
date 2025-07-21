def RR_ICE_CAVERN_ENTRYWAY SCENE_ICE_CAVERN false RA_ICE_CAVERN
Ice Cavern Entryway
//Exits
RR_ICE_CAVERN_BEGINNING (IsDungeonVanilla ICE_CAVERN)
RR_ICE_CAVERN_MQ_BEGINNING (and (IsDungeonMQ ICE_CAVERN) CanUseProjectile)
RR_ZF_LEDGE true

def RR_ICE_CAVERN_BEGINNING SCENE_ICE_CAVERN false RA_ICE_CAVERN
Ice Cavern Beginning
//Checks
RC_ICE_CAVERN_ENTRANCE_STORMS_FAIRY (CanUse RG_SONG_OF_STORMS)
//Exits
RR_ICE_CAVERN_ENTRYWAY true
RR_ICE_CAVERN_MAIN (Here (CanKillEnemy RE_FREEZARD ED_CLOSE true 4))

def RR_ICE_CAVERN_MAIN SCENE_ICE_CAVERN false RA_ICE_CAVERN
Ice Cavern
//Events
BlueFireAccess IsAdult
//Checks
RC_ICE_CAVERN_MAP_CHEST (and BlueFire IsAdult)
RC_ICE_CAVERN_COMPASS_CHEST BlueFire
RC_ICE_CAVERN_IRON_BOOTS_CHEST (and BlueFire (CanKillEnemy RE_WOLFOS))
RC_SHEIK_IN_ICE_CAVERN (and BlueFire (and (CanKillEnemy RE_WOLFOS) IsAdult))
RC_ICE_CAVERN_FREESTANDING_POH BlueFire
RC_ICE_CAVERN_GS_SPINNING_SCYTHE_ROOM HookshotOrBoomerang
RC_ICE_CAVERN_GS_HEART_PIECE_ROOM (and BlueFire HookshotOrBoomerang)
RC_ICE_CAVERN_GS_PUSH_BLOCK_ROOM (and BlueFire (or HookshotOrBoomerang (and RT_ICE_BLOCK_GS (and IsAdult (CanUse RG_HOVER_BOOTS)))))
RC_ICE_CAVERN_HALL_POT_1 CanBreakPots
RC_ICE_CAVERN_HALL_POT_2 CanBreakPots
RC_ICE_CAVERN_SPINNING_BLADE_POT_1 CanBreakPots
RC_ICE_CAVERN_SPINNING_BLADE_POT_2 CanBreakPots
RC_ICE_CAVERN_SPINNING_BLADE_POT_3 CanBreakPots
RC_ICE_CAVERN_NEAR_END_POT_1 (and CanBreakPots BlueFire)
RC_ICE_CAVERN_NEAR_END_POT_2 (and CanBreakPots BlueFire)
RC_ICE_CAVERN_FROZEN_POT_1 (and CanBreakPots (and BlueFire IsAdult))
RC_ICE_CAVERN_LOBBY_RUPEE BlueFire
RC_ICE_CAVERN_MAP_ROOM_LEFT_HEART IsAdult
RC_ICE_CAVERN_MAP_ROOM_MIDDLE_HEART IsAdult
RC_ICE_CAVERN_MAP_ROOM_RIGHT_HEART IsAdult
RC_ICE_CAVERN_SLIDING_BLOCK_RUPEE_1 (and BlueFire (or (CanUse RG_SONG_OF_TIME) (CanUse RG_BOOMERANG)))
RC_ICE_CAVERN_SLIDING_BLOCK_RUPEE_2 (and BlueFire (or (CanUse RG_SONG_OF_TIME) (CanUse RG_BOOMERANG)))
RC_ICE_CAVERN_SLIDING_BLOCK_RUPEE_3 (and BlueFire (or (CanUse RG_SONG_OF_TIME) (CanUse RG_BOOMERANG)))

def RR_ICE_CAVERN_MQ_BEGINNING SCENE_ICE_CAVERN false RA_ICE_CAVERN
Ice Cavern MQ Beginning
//Checks
RC_ICE_CAVERN_MQ_ENTRANCE_POT CanBreakPots
//Exits
RR_ICE_CAVERN_ENTRYWAY true
//It is in logic to use a pot to hit the toggle switch here.
RR_ICE_CAVERN_MQ_HUB true

def RR_ICE_CAVERN_MQ_HUB SCENE_ICE_CAVERN false RA_ICE_CAVERN
Ice Cavern MQ Hub
//Events
FairyPot true
//Checks
RC_ICE_CAVERN_MQ_FIRST_CRYSTAL_POT_1 CanBreakPots
RC_ICE_CAVERN_MQ_FIRST_CRYSTAL_POT_2 CanBreakPots
RC_ICE_CAVERN_MQ_EARLY_WOLFOS_POT_1 CanBreakPots
RC_ICE_CAVERN_MQ_EARLY_WOLFOS_POT_2 CanBreakPots
RC_ICE_CAVERN_MQ_EARLY_WOLFOS_POT_3 CanBreakPots
RC_ICE_CAVERN_MQ_EARLY_WOLFOS_POT_4 CanBreakPots
//the switch for the glass blocking the entrance is linked to the switch that controls the glass around the skulltulla in RR_ICE_CAVERN_MQ_SCARECROW_ROOM
//if you clear the ice, you can hit it with a pot from here.
//Exits
RR_ICE_CAVERN_MQ_BEGINNING BlueFire
RR_ICE_CAVERN_MQ_MAP_ROOM (Here (and (CanKillEnemy RE_WHITE_WOLFOS) (CanKillEnemy RE_FREEZARD)))
RR_ICE_CAVERN_MQ_COMPASS_ROOM (and IsAdult BlueFire)
RR_ICE_CAVERN_MQ_SCARECROW_ROOM BlueFire

def RR_ICE_CAVERN_MQ_MAP_ROOM SCENE_ICE_CAVERN false RA_ICE_CAVERN
Ice Cavern MQ Map Room
//Child can fit between the stalagmites on the left hand side
//Events
BlueFireAccess (or IsChild (or CanJumpslash HasExplosives))
//Checks
RC_ICE_CAVERN_MQ_MAP_CHEST (and BlueFire (Here CanHitSwitch))

def RR_ICE_CAVERN_MQ_SCARECROW_ROOM SCENE_ICE_CAVERN false RA_ICE_CAVERN
Ice Cavern MQ Scarecrow Room
//Checks
RC_ICE_CAVERN_MQ_GS_ICE_BLOCK (or (and BlueFire (CanGetEnemyDrop RE_GOLD_SKULLTULA)) (and IsAdult (CanHitSwitch ED_LONG_JUMPSLASH)))
RC_ICE_CAVERN_MQ_GS_SCARECROW (or (CanUse RG_SCARECROW) (and IsAdult (or (CanUse RG_LONGSHOT) RT_ICE_MQ_SCARECROW)))
//Exits
RR_ICE_CAVERN_MQ_HUB BlueFire
//Assumes RR_ICE_CAVERN_MQ_HUB access for a pot if using blue fire
RR_ICE_CAVERN_MQ_WEST_CORRIDOR (and IsAdult BlueFire)

def RR_ICE_CAVERN_MQ_STALFOS_ROOM SCENE_ICE_CAVERN false RA_ICE_CAVERN
Ice Cavern MQ Stalfos Room
//Checks
RC_ICE_CAVERN_MQ_IRON_BOOTS_CHEST (CanKillEnemy RE_STALFOS)
RC_SHEIK_IN_ICE_CAVERN (CanKillEnemy RE_STALFOS)
//Exits
RR_ICE_CAVERN_MQ_WEST_CORRIDOR (Here (CanKillEnemy RE_STALFOS))
RR_ICE_CAVERN_MQ_BEGINNING (and (CanUse RG_IRON_BOOTS) (Here (CanKillEnemy RE_STALFOS)))

def RR_ICE_CAVERN_MQ_WEST_CORRIDOR SCENE_ICE_CAVERN false RA_ICE_CAVERN
Ice Cavern MQ West Corridor
//Checks
RC_ICE_CAVERN_MQ_PUSH_BLOCK_POT_1 CanBreakPots
RC_ICE_CAVERN_MQ_PUSH_BLOCK_POT_2 CanBreakPots
//Exits
RR_ICE_CAVERN_MQ_SCARECROW_ROOM BlueFire
RR_ICE_CAVERN_MQ_STALFOS_ROOM true

def RR_ICE_CAVERN_MQ_COMPASS_ROOM SCENE_ICE_CAVERN false RA_ICE_CAVERN
Ice Cavern MQ Compass Room
//Events
BlueFireAccess true
//Checks
RC_ICE_CAVERN_MQ_COMPASS_CHEST true
//It is possible for child with master, BGS or sticks, or adult with BGS, to hit this switch through the ice with a crouchstab, but it's precise and unintuitive for a trick
RC_ICE_CAVERN_MQ_FREESTANDING_POH HasExplosives
//doing RT_ICE_MQ_RED_ICE_GS as child is untested, as I could not perform the trick reliably even as adult
RC_ICE_CAVERN_MQ_GS_RED_ICE (or (and RSK_BLUE_FIRE_ARROWS (CanUse RG_ICE_ARROWS)) (and (CanUse RG_BOTTLE_WITH_BLUE_FIRE) (and (or (CanUse RG_SONG_OF_TIME) (and IsAdult RT_ICE_MQ_RED_ICE_GS)) (CanGetEnemyDrop RE_GOLD_SKULLTULA))))
RC_ICE_CAVERN_MQ_COMPASS_POT_1 CanBreakPots
RC_ICE_CAVERN_MQ_COMPASS_POT_2 CanBreakPots
