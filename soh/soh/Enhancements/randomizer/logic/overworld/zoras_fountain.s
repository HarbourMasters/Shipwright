def RR_ZORAS_FOUNTAIN SCENE_ZORAS_FOUNTAIN false RA_ZORAS_FOUNTAIN
Zoras Fountain
//Events
GossipStoneFairy CallGossipFairyExceptSuns
ButterflyFairy (and (CanUse RG_STICKS) AtDay)
//Checks
RC_ZF_GS_TREE IsChild
RC_ZF_GS_ABOVE_THE_LOG (and IsChild (and HookshotOrBoomerang CanGetNightTimeGS))
RC_ZF_FAIRY_GOSSIP_STONE_FAIRY CallGossipFairyExceptSuns
RC_ZF_FAIRY_GOSSIP_STONE_FAIRY_BIG (CanUse RG_SONG_OF_STORMS)
RC_ZF_JABU_GOSSIP_STONE_FAIRY CallGossipFairyExceptSuns
RC_ZF_JABU_GOSSIP_STONE_FAIRY_BIG (CanUse RG_SONG_OF_STORMS)
RC_ZF_FAIRY_GOSSIP_STONE true
RC_ZF_JABU_GOSSIP_STONE true
RC_ZF_NEAR_JABU_POT_1 (and IsChild CanBreakPots)
RC_ZF_NEAR_JABU_POT_2 (and IsChild CanBreakPots)
RC_ZF_NEAR_JABU_POT_3 (and IsChild CanBreakPots)
RC_ZF_NEAR_JABU_POT_4 (and IsChild CanBreakPots)
//Exits
RR_ZD_BEHIND_KING_ZORA true
RR_ZF_ICEBERGS IsAdult
RR_ZF_LAKEBED (CanUse RG_IRON_BOOTS)
//child can break the brown rock without lifting the silver rock and it stays gone for adult, but it's not intuitive and there's no reasonable case where it matters.
RR_ZF_HIDDEN_CAVE (and (CanUse RG_SILVER_GAUNTLETS) BlastOrSmash)
RR_ZF_ROCK (and IsAdult (CanUse RG_SCARECROW))
RR_JABU_JABUS_BELLY_ENTRYWAY (and IsChild (or (== RSK_JABU_OPEN RO_JABU_OPEN) (CanUse RG_BOTTLE_WITH_FISH)))
RR_ZF_GREAT_FAIRY_FOUNTAIN (or HasExplosives (and RT_ZF_GREAT_FAIRY_WITHOUT_EXPLOSIVES (and (CanUse RG_MEGATON_HAMMER) (CanUse RG_SILVER_GAUNTLETS))))

def RR_ZF_ICEBERGS SCENE_ZORAS_FOUNTAIN false RA_ZORAS_FOUNTAIN
ZF Icebergs
//Checks
RC_ZF_ICEBERG_FREESTANDING_POH IsAdult
//Exits
RR_ZORAS_FOUNTAIN (or (HasItem RG_BRONZE_SCALE) (CanUse RG_HOVER_BOOTS))
RR_ZF_LAKEBED (CanUse RG_IRON_BOOTS)
RR_ZF_LEDGE true

def RR_ZF_LAKEBED SCENE_ZORAS_FOUNTAIN false RA_ZORAS_FOUNTAIN
ZF Lakebed
//Checks
RC_ZF_BOTTOM_FREESTANDING_POH (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_NORTH_INNER_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_NORTHEAST_INNER_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_SOUTHEAST_INNER_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_SOUTH_INNER_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_SOUTHWEST_INNER_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_NORTHWEST_INNER_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_NORTH_MIDDLE_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_NORTHEAST_MIDDLE_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_SOUTHEAST_MIDDLE_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_SOUTH_MIDDLE_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_SOUTHWEST_MIDDLE_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_NORTHWEST_MIDDLE_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_NORTH_OUTER_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_NORTHEAST_OUTER_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_SOUTHEAST_OUTER_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_SOUTH_OUTER_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_SOUTHWEST_OUTER_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
RC_ZF_BOTTOM_NORTHWEST_OUTER_RUPEE (and IsAdult (and (CanUse RG_IRON_BOOTS) (>= WaterTimer 16)))
//Exits
RR_ZORAS_FOUNTAIN (HasItem RG_BRONZE_SCALE)

def RR_ZF_LEDGE SCENE_ZORAS_FOUNTAIN false RA_ZORAS_FOUNTAIN
ZF Ledge
//Exits
RR_ZORAS_FOUNTAIN (HasItem RG_BRONZE_SCALE)
RR_ZF_ICEBERGS IsAdult
RR_ZF_LAKEBED (CanUse RG_IRON_BOOTS)
RR_ICE_CAVERN_ENTRYWAY true

def RR_ZF_HIDDEN_CAVE SCENE_ZORAS_FOUNTAIN false RA_ZORAS_FOUNTAIN
ZF Hidden Cave
//Checks
RC_ZF_HIDDEN_CAVE_POT_1 (and IsAdult CanBreakPots)
RC_ZF_HIDDEN_CAVE_POT_2 (and IsAdult CanBreakPots)
RC_ZF_HIDDEN_CAVE_POT_3 (and IsAdult CanBreakPots)
//There are invisible big skultullas here as adult but they do not block the path and can be "seen" with Z-target
//Lens is not currently needed for this either, implying they are not considered blocking, but it's open for discussion long-term
//Exits
RR_ZF_HIDDEN_LEDGE true

def RR_ZF_HIDDEN_LEDGE SCENE_ZORAS_FOUNTAIN false RA_ZORAS_FOUNTAIN
ZF Hidden Ledge
//Checks
RC_ZF_GS_HIDDEN_CAVE (and IsAdult (and (CanGetEnemyDrop RE_GOLD_SKULLTULA ED_BOMB_THROW) CanGetNightTimeGS))
//It is possible to avoid fall damage by jumping towards the right and landing in deeper water, but this is basically never relevent
//Exits
RR_ZORAS_FOUNTAIN (or (HasItem RG_BRONZE_SCALE) TakeDamage)
RR_ZF_HIDDEN_CAVE true

def RR_ZF_ROCK SCENE_ZORAS_FOUNTAIN false RA_ZORAS_FOUNTAIN
ZF Rock
//Has a wonder item
//Exits
RR_ZORAS_FOUNTAIN true

def RR_ZF_GREAT_FAIRY_FOUNTAIN SCENE_GREAT_FAIRYS_FOUNTAIN_SPELLS false
ZF Great Fairy Fountain
//Checks
RC_ZF_GREAT_FAIRY_REWARD (CanUse RG_ZELDAS_LULLABY)
//Exits
RR_ZORAS_FOUNTAIN true
