def RR_GORON_CITY SCENE_GORON_CITY false RA_GORON_CITY
Goron City
//Events
GossipStoneFairy CallGossipFairyExceptSuns
StickPot IsChild
BugRock (or BlastOrSmash (CanUse RG_SILVER_GAUNTLETS))
GoronCityChildFire (and IsChild (CanUse RG_DINS_FIRE))
GCWoodsWarpOpen (or CanDetonateUprightBombFlower (or (CanUse RG_MEGATON_HAMMER) GoronCityChildFire))
GCDaruniasDoorOpenChild (and IsChild (CanUse RG_ZELDAS_LULLABY))
StopGCRollingGoronAsAdult (and IsAdult (or (HasItem RG_GORONS_BRACELET) (or HasExplosives (or (CanUse RG_FAIRY_BOW) (and RT_GC_LINK_GORON_DINS (or (CanUse RG_DINS_FIRE) (and RT_BLUE_FIRE_MUD_WALLS (CanUse RG_BOTTLE_WITH_BLUE_FIRE))))))))
//Checks
RC_GC_MAZE_LEFT_CHEST (or (CanUse RG_MEGATON_HAMMER) (or (CanUse RG_SILVER_GAUNTLETS) (and RT_GC_LEFTMOST (and HasExplosives (CanUse RG_HOVER_BOOTS)))))
RC_GC_MAZE_CENTER_CHEST (or BlastOrSmash (CanUse RG_SILVER_GAUNTLETS))
RC_GC_MAZE_RIGHT_CHEST (or BlastOrSmash (CanUse RG_SILVER_GAUNTLETS))
RC_GC_POT_FREESTANDING_POH (and IsChild (and GoronCityChildFire (or (CanUse RG_BOMB_BAG) (or (and (HasItem RG_GORONS_BRACELET) RT_GC_POT_STRENGTH) (and (CanUse RG_BOMBCHU_5) RT_GC_POT)))))
RC_GC_ROLLING_GORON_AS_CHILD (and IsChild (or HasExplosives (and (HasItem RG_GORONS_BRACELET) RT_GC_ROLLING_STRENGTH)))
RC_GC_ROLLING_GORON_AS_ADULT StopGCRollingGoronAsAdult
RC_GC_GS_BOULDER_MAZE (and IsChild BlastOrSmash)
RC_GC_GS_CENTER_PLATFORM (and IsAdult CanAttack)
RC_GC_MEDIGORON (and IsAdult (or CanBreakMudWalls (HasItem RG_GORONS_BRACELET)))
RC_GC_MAZE_GOSSIP_STONE_FAIRY (and (or BlastOrSmash (CanUse RG_SILVER_GAUNTLETS)) CallGossipFairyExceptSuns)
RC_GC_MAZE_GOSSIP_STONE_FAIRY_BIG (and (or BlastOrSmash (CanUse RG_SILVER_GAUNTLETS)) (CanUse RG_SONG_OF_STORMS))
RC_GC_MAZE_GOSSIP_STONE (or BlastOrSmash (CanUse RG_SILVER_GAUNTLETS))
RC_GC_LOWER_STAIRCASE_POT_1 CanBreakPots
RC_GC_LOWER_STAIRCASE_POT_2 CanBreakPots
RC_GC_UPPER_STAIRCASE_POT_1 CanBreakPots
RC_GC_UPPER_STAIRCASE_POT_2 CanBreakPots
RC_GC_UPPER_STAIRCASE_POT_3 CanBreakPots
RC_GC_MAZE_CRATE (or BlastOrSmash (and (CanUse RG_SILVER_GAUNTLETS) CanBreakCrates))
//Exits
RR_DEATH_MOUNTAIN_TRAIL true
RR_GC_MEDIGORON (or CanBreakMudWalls (HasItem RG_GORONS_BRACELET))
RR_GC_WOODS_WARP GCWoodsWarpOpen
RR_GC_SHOP (or (and IsAdult StopGCRollingGoronAsAdult) (and IsChild (or BlastOrSmash (or (HasItem RG_GORONS_BRACELET) (or GoronCityChildFire (CanUse RG_FAIRY_BOW))))))
RR_GC_DARUNIAS_CHAMBER (or (and IsAdult StopGCRollingGoronAsAdult) (and IsChild GCDaruniasDoorOpenChild))
RR_GC_GROTTO_PLATFORM (and IsAdult (or (and (CanUse RG_SONG_OF_TIME) (or (> EffectiveHealth 2) (or (CanUse RG_GORON_TUNIC) (or (CanUse RG_LONGSHOT) (CanUse RG_NAYRUS_LOVE))))) (or (and (> EffectiveHealth 1) (and (CanUse RG_GORON_TUNIC) (CanUse RG_HOOKSHOT))) (or (and (CanUse RG_NAYRUS_LOVE) (CanUse RG_HOOKSHOT)) (and (> EffectiveHealth 2) (and (CanUse RG_HOOKSHOT) RT_GC_GROTTO))))))

def RR_GC_MEDIGORON SCENE_GORON_CITY false RA_GORON_CITY
GC Medigoron
//Checks
RC_GC_MEDIGORON_GOSSIP_STONE_FAIRY CallGossipFairyExceptSuns
RC_GC_MEDIGORON_GOSSIP_STONE_FAIRY_BIG (CanUse RG_SONG_OF_STORMS)
RC_GC_MEDIGORON_GOSSIP_STONE true
RC_GC_MEDIGORON_POT_1 CanBreakPots
//Exits
RR_GORON_CITY true

def RR_GC_WOODS_WARP SCENE_GORON_CITY false RA_GORON_CITY
GC Woods Warp
//Events
GCWoodsWarpOpen (or BlastOrSmash (CanUse RG_DINS_FIRE))
//Exits
RR_GORON_CITY GCWoodsWarpOpen
RR_THE_LOST_WOODS true

def RR_GC_DARUNIAS_CHAMBER SCENE_GORON_CITY false RA_GORON_CITY
GC Darunias Chamber
//Events
GoronCityChildFire (and IsChild (CanUse RG_STICKS))
//Checks
RC_GC_DARUNIAS_JOY (and IsChild (CanUse RG_SARIAS_SONG))
RC_GC_DARUNIA_POT_1 CanBreakPots
RC_GC_DARUNIA_POT_2 CanBreakPots
RC_GC_DARUNIA_POT_3 CanBreakPots
//Exits
RR_GORON_CITY true
RR_DMC_LOWER_LOCAL IsAdult

def RR_GC_GROTTO_PLATFORM SCENE_GORON_CITY false RA_GORON_CITY
GC Grotto Platform
//Exits
RR_GC_GROTTO true
RR_GORON_CITY (or (> EffectiveHealth 2) (or (CanUse RG_GORON_TUNIC) (or (CanUse RG_NAYRUS_LOVE) (and (or IsChild (CanUse RG_SONG_OF_TIME)) (CanUse RG_LONGSHOT)))))

def RR_GC_SHOP SCENE_GORON_SHOP false
GC Shop
//Checks
RC_GC_SHOP_ITEM_1 true
RC_GC_SHOP_ITEM_2 true
RC_GC_SHOP_ITEM_3 true
RC_GC_SHOP_ITEM_4 true
RC_GC_SHOP_ITEM_5 true
RC_GC_SHOP_ITEM_6 true
RC_GC_SHOP_ITEM_7 true
RC_GC_SHOP_ITEM_8 true
//Exits
RR_GORON_CITY true

def RR_GC_GROTTO SCENE_GROTTOS false
GC Grotto
//Checks
RC_GC_DEKU_SCRUB_GROTTO_LEFT CanStunDeku
RC_GC_DEKU_SCRUB_GROTTO_RIGHT CanStunDeku
RC_GC_DEKU_SCRUB_GROTTO_CENTER CanStunDeku
RC_GC_GROTTO_BEEHIVE CanBreakUpperBeehives
//Exits
RR_GC_GROTTO_PLATFORM true
