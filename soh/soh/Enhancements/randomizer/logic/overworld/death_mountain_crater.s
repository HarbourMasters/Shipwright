def RR_DMC_UPPER_LOCAL SCENE_DEATH_MOUNTAIN_CRATER false RA_DEATH_MOUNTAIN_CRATER
DMC Upper Local
//Events
GossipStoneFairy (and HasExplosives (and CallGossipFairyExceptSuns (or (>= FireTimer 16) (>= Hearts 3))))
//Checks
RC_DMC_WALL_FREESTANDING_POH (or (>= FireTimer 16) (>= Hearts 3))
RC_DMC_GS_CRATE (and (or (>= FireTimer 8) (>= Hearts 3)) (and IsChild (and CanAttack CanBreakCrates)))
RC_DMC_GOSSIP_STONE_FAIRY (and CallGossipFairyExceptSuns (and HasExplosives (or (>= FireTimer 16) (>= Hearts 3))))
RC_DMC_GOSSIP_STONE_FAIRY_BIG (and (CanUse RG_SONG_OF_STORMS) (and HasExplosives (or (>= FireTimer 16) (>= Hearts 3))))
RC_DMC_GOSSIP_STONE (and HasExplosives (or (>= FireTimer 16) (>= Hearts 3)))
RC_DMC_CRATE (and (or (>= FireTimer 8) (>= Hearts 3)) (and IsChild CanBreakCrates))
//Exits
RR_DMC_UPPER_NEARBY true
RR_DMC_LADDER_AREA_NEARBY (or (>= FireTimer 16) (>= Hearts 3))
RR_DMC_CENTRAL_NEARBY (and IsAdult (and (CanUse RG_GORON_TUNIC) (and (CanUse RG_DISTANT_SCARECROW) (or (> EffectiveHealth 2) (or (and (CanUse RG_BOTTLE_WITH_FAIRY) (!= RSK_SHUFFLE_DUNGEON_ENTRANCES RO_DUNGEON_ENTRANCE_SHUFFLE_OFF)) (CanUse RG_NAYRUS_LOVE))))))
RR_DMC_LOWER_NEARBY false
RR_DMC_DISTANT_PLATFORM (or (and (>= FireTimer 48) (>= Hearts 2)) (>= Hearts 3))

def RR_DMC_CENTRAL_LOCAL SCENE_DEATH_MOUNTAIN_CRATER false RA_DEATH_MOUNTAIN_CRATER
DMC Central Local
//Events
BeanPlantFairy (and IsChild (and (CanUse RG_MAGIC_BEAN) (and (CanUse RG_SONG_OF_STORMS) (or (>= FireTimer 8) (>= Hearts 3)))))
//Checks
RC_DMC_GS_BEAN_PATCH (and (or (>= FireTimer 8) (>= Hearts 3)) (and CanSpawnSoilSkull CanAttack))
RC_DMC_NEAR_PLATFORM_RED_RUPEE IsChild
RC_DMC_MIDDLE_PLATFORM_RED_RUPEE (and IsChild (or (>= FireTimer 8) (>= Hearts 3)))
RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_1 (and IsChild (or (>= FireTimer 8) (>= Hearts 3)))
RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_2 (and IsChild (or (>= FireTimer 8) (>= Hearts 3)))
RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_3 (and IsChild (or (>= FireTimer 8) (>= Hearts 3)))
RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_4 (and IsChild (or (>= FireTimer 8) (>= Hearts 3)))
RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_5 (and IsChild (or (>= FireTimer 8) (>= Hearts 3)))
RC_DMC_MIDDLE_PLATFORM_BLUE_RUPEE_6 (and IsChild (or (>= FireTimer 8) (>= Hearts 3)))
RC_DMC_BEAN_SPROUT_FAIRY_1 (and IsChild (and (CanUse RG_MAGIC_BEAN) (and (CanUse RG_SONG_OF_STORMS) (or (>= FireTimer 8) (>= Hearts 3)))))
RC_DMC_BEAN_SPROUT_FAIRY_2 (and IsChild (and (CanUse RG_MAGIC_BEAN) (and (CanUse RG_SONG_OF_STORMS) (or (>= FireTimer 8) (>= Hearts 3)))))
RC_DMC_BEAN_SPROUT_FAIRY_3 (and IsChild (and (CanUse RG_MAGIC_BEAN) (and (CanUse RG_SONG_OF_STORMS) (or (>= FireTimer 8) (>= Hearts 3)))))
//Exits
RR_DMC_CENTRAL_NEARBY true
RR_DMC_LOWER_NEARBY (or (and IsAdult (CanPlantBean RR_DMC_CENTRAL_LOCAL)) (or (CanUse RG_HOVER_BOOTS) (CanUse RG_HOOKSHOT)))
RR_DMC_UPPER_NEARBY (and IsAdult (CanPlantBean RR_DMC_CENTRAL_LOCAL))
RR_FIRE_TEMPLE_ENTRYWAY (or (and IsChild (and (>= Hearts 3) (!= RSK_SHUFFLE_DUNGEON_ENTRANCES RO_DUNGEON_ENTRANCE_SHUFFLE_OFF))) (and IsAdult (>= FireTimer 24)))
RR_DMC_DISTANT_PLATFORM (and (>= FireTimer 48) (CanUse RG_DISTANT_SCARECROW))

def RR_DMC_LOWER_LOCAL SCENE_DEATH_MOUNTAIN_CRATER false RA_DEATH_MOUNTAIN_CRATER
DMC Lower Local
//Exits
RR_DMC_LOWER_NEARBY true
RR_DMC_LADDER_AREA_NEARBY (or (>= FireTimer 8) (>= Hearts 3))
RR_DMC_CENTRAL_NEARBY (and (or (CanUse RG_HOVER_BOOTS) (CanUse RG_HOOKSHOT)) (or (>= FireTimer 8) (>= Hearts 3)))
RR_DMC_CENTRAL_LOCAL (and (or (CanUse RG_HOVER_BOOTS) (or (CanUse RG_HOOKSHOT) (and IsAdult (and CanShield RT_DMC_BOLERO_JUMP)))) (>= FireTimer 24))

def RR_DMC_LOWER_NEARBY SCENE_DEATH_MOUNTAIN_CRATER false RA_DEATH_MOUNTAIN_CRATER
DMC Lower Nearby
//Checks
RC_DMC_NEAR_GC_POT_1 CanBreakPots
RC_DMC_NEAR_GC_POT_2 CanBreakPots
RC_DMC_NEAR_GC_POT_3 CanBreakPots
RC_DMC_NEAR_GC_POT_4 CanBreakPots
//Exits
RR_DMC_LOWER_LOCAL (>= FireTimer 48)
RR_GC_DARUNIAS_CHAMBER true
RR_DMC_GREAT_FAIRY_FOUNTAIN (CanUse RG_MEGATON_HAMMER)
RR_DMC_HAMMER_GROTTO (and IsAdult (CanUse RG_MEGATON_HAMMER))

def RR_DMC_UPPER_NEARBY SCENE_DEATH_MOUNTAIN_CRATER false RA_DEATH_MOUNTAIN_CRATER
DMC Upper Nearby
//Exits
RR_DMC_UPPER_LOCAL (>= FireTimer 48)
RR_DEATH_MOUNTAIN_SUMMIT true
RR_DMC_UPPER_GROTTO (Here (and BlastOrSmash (or (>= FireTimer 8) (>= Hearts 3))))

def RR_DMC_CENTRAL_NEARBY SCENE_DEATH_MOUNTAIN_CRATER false RA_DEATH_MOUNTAIN_CRATER
DMC Central Nearby
//Checks
RC_DMC_VOLCANO_FREESTANDING_POH (and IsAdult (and (>= Hearts 3) (or (CanPlantBean RR_DMC_CENTRAL_LOCAL) (and RT_DMC_HOVER_BEAN_POH (CanUse RG_HOVER_BOOTS)))))
RC_SHEIK_IN_CRATER (and IsAdult (or (>= FireTimer 8) (>= Hearts 3)))
//Exits
RR_DMC_CENTRAL_LOCAL (>= FireTimer 48)

def RR_DMC_LADDER_AREA_NEARBY SCENE_DEATH_MOUNTAIN_CRATER false RA_DEATH_MOUNTAIN_CRATER
DMC Ladder Region Nearby
//Checks
RC_DMC_DEKU_SCRUB (and IsChild CanStunDeku)
//Exits
RR_DMC_UPPER_NEARBY (>= Hearts 3)
RR_DMC_LOWER_NEARBY (and (>= Hearts 3) (or (CanUse RG_HOVER_BOOTS) (or (and RT_DMC_BOULDER_JS (and IsAdult (CanUse RG_MEGATON_HAMMER))) (and RT_DMC_BOULDER_SKIP IsAdult))))

def RR_DMC_UPPER_GROTTO SCENE_GROTTOS false
DMC Upper Grotto
//Events
GossipStoneFairy CallGossipFairy
ButterflyFairy (CanUse RG_STICKS)
BugShrub CanCutShrubs
LoneFish true
//Checks
RC_DMC_UPPER_GROTTO_CHEST true
RC_DMC_UPPER_GROTTO_FISH HasBottle
RC_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY CallGossipFairy
RC_DMC_UPPER_GROTTO_GOSSIP_STONE_FAIRY_BIG (CanUse RG_SONG_OF_STORMS)
RC_DMC_UPPER_GROTTO_GOSSIP_STONE true
RC_DMC_UPPER_GROTTO_BEEHIVE_LEFT CanBreakLowerBeehives
RC_DMC_UPPER_GROTTO_BEEHIVE_RIGHT CanBreakLowerBeehives
RC_DMC_UPPER_GROTTO_GRASS_1 CanCutShrubs
RC_DMC_UPPER_GROTTO_GRASS_2 CanCutShrubs
RC_DMC_UPPER_GROTTO_GRASS_3 CanCutShrubs
RC_DMC_UPPER_GROTTO_GRASS_4 CanCutShrubs
//Exits
RR_DMC_UPPER_LOCAL true

def RR_DMC_HAMMER_GROTTO SCENE_GROTTOS false
DMC Hammer Grotto
//Checks
RC_DMC_DEKU_SCRUB_GROTTO_LEFT CanStunDeku
RC_DMC_DEKU_SCRUB_GROTTO_RIGHT CanStunDeku
RC_DMC_DEKU_SCRUB_GROTTO_CENTER CanStunDeku
RC_DMC_HAMMER_GROTTO_BEEHIVE CanBreakUpperBeehives
//Exits
RR_DMC_LOWER_LOCAL true

def RR_DMC_GREAT_FAIRY_FOUNTAIN SCENE_GREAT_FAIRYS_FOUNTAIN_MAGIC false
DMC Great Fairy Fountain
//Checks
RC_DMC_GREAT_FAIRY_REWARD (CanUse RG_ZELDAS_LULLABY)
//Exits
RR_DMC_LOWER_LOCAL true

def RR_DMC_DISTANT_PLATFORM SCENE_DEATH_MOUNTAIN_CRATER false RA_DEATH_MOUNTAIN_CRATER
DMC Distant Platform
//Checks
RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_1 IsAdult
RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_2 IsAdult
RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_3 IsAdult
RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_4 IsAdult
RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_5 IsAdult
RC_DMC_DISTANT_PLATFORM_GREEN_RUPEE_6 IsAdult
RC_DMC_DISTANT_PLATFORM_RED_RUPEE IsAdult
//Exits
RR_DMC_CENTRAL_LOCAL (and (>= FireTimer 48) (CanUse RG_DISTANT_SCARECROW))
