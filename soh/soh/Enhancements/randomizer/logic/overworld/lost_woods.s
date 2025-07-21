def RR_THE_LOST_WOODS SCENE_LOST_WOODS false RA_THE_LOST_WOODS
Lost Woods
//Events
GossipStoneFairy CallGossipFairyExceptSuns
BeanPlantFairy (and IsChild (and (CanUse RG_MAGIC_BEAN) (CanUse RG_SONG_OF_STORMS)))
BugShrub (and IsChild CanCutShrubs)
BorrowSpookyMask (and IsChild (and BorrowSkullMask (and (CanUse RG_SARIAS_SONG) (HasItem RG_CHILD_WALLET))))
//Checks
RC_LW_SKULL_KID (and IsChild (CanUse RG_SARIAS_SONG))
RC_LW_TRADE_COJIRO (and IsAdult (CanUse RG_COJIRO))
//I cannot think of a case where you can use Odd pot but not Cojiro to reset the quadrant should you have both. If one exists, add it to logic
RC_LW_TRADE_ODD_POTION (and IsAdult (CanUse RG_ODD_POTION))
//all 5 buttons are logically required for memory game
//because the chances of being able to beat it
//every time you attempt it are as follows:
//0 or 1 button(s) => 0%
//2 buttons        => 0.15625%
//3 buttons        => 3.75%
//4 buttons        => 25.3125%
//5 buttons        => 100%
RC_LW_OCARINA_MEMORY_GAME (and IsChild (and (HasItem RG_FAIRY_OCARINA) (>= OcarinaButtons 5)))
RC_LW_TARGET_IN_WOODS (and IsChild (CanUse RG_FAIRY_SLINGSHOT))
RC_LW_DEKU_SCRUB_NEAR_BRIDGE (and IsChild CanStunDeku)
RC_LW_GS_BEAN_PATCH_NEAR_BRIDGE (and CanSpawnSoilSkull CanAttack)
//RANDOTODO handle collecting some of these as you leave the shortcut from the other side
RC_LW_SHORTCUT_RUPEE_1 (and IsChild (or (HasItem RG_SILVER_SCALE) (CanUse RG_IRON_BOOTS)))
RC_LW_SHORTCUT_RUPEE_2 (and IsChild (or (HasItem RG_SILVER_SCALE) (CanUse RG_IRON_BOOTS)))
RC_LW_SHORTCUT_RUPEE_3 (and IsChild (or (HasItem RG_SILVER_SCALE) (CanUse RG_IRON_BOOTS)))
RC_LW_SHORTCUT_RUPEE_4 (and IsChild (or (HasItem RG_SILVER_SCALE) (CanUse RG_IRON_BOOTS)))
RC_LW_SHORTCUT_RUPEE_5 (and IsChild (or (HasItem RG_SILVER_SCALE) (CanUse RG_IRON_BOOTS)))
RC_LW_SHORTCUT_RUPEE_6 (and IsChild (or (HasItem RG_SILVER_SCALE) (CanUse RG_IRON_BOOTS)))
RC_LW_SHORTCUT_RUPEE_7 (and IsChild (or (HasItem RG_SILVER_SCALE) (CanUse RG_IRON_BOOTS)))
RC_LW_SHORTCUT_RUPEE_8 (and IsChild (or (HasItem RG_SILVER_SCALE) (CanUse RG_IRON_BOOTS)))
RC_LW_BEAN_SPROUT_NEAR_BRIDGE_FAIRY_1 (and IsChild (and (CanUse RG_MAGIC_BEAN) (CanUse RG_SONG_OF_STORMS)))
RC_LW_BEAN_SPROUT_NEAR_BRIDGE_FAIRY_2 (and IsChild (and (CanUse RG_MAGIC_BEAN) (CanUse RG_SONG_OF_STORMS)))
RC_LW_BEAN_SPROUT_NEAR_BRIDGE_FAIRY_3 (and IsChild (and (CanUse RG_MAGIC_BEAN) (CanUse RG_SONG_OF_STORMS)))
RC_LW_GOSSIP_STONE_FAIRY CallGossipFairyExceptSuns
RC_LW_GOSSIP_STONE_FAIRY_BIG (CanUse RG_SONG_OF_STORMS)
RC_LW_SHORTCUT_STORMS_FAIRY (CanUse RG_SONG_OF_STORMS)
RC_LW_GOSSIP_STONE true
RC_LW_GRASS_1 CanCutShrubs
RC_LW_GRASS_2 CanCutShrubs
RC_LW_GRASS_3 CanCutShrubs
//Exits
RR_LW_FOREST_EXIT true
RR_GC_WOODS_WARP true
RR_LW_BRIDGE (or (and IsAdult (or (CanPlantBean RR_THE_LOST_WOODS) RT_LW_BRIDGE)) (or (CanUse RG_HOVER_BOOTS) (CanUse RG_LONGSHOT)))
RR_ZR_FROM_SHORTCUT (or (HasItem RG_SILVER_SCALE) (or (CanUse RG_IRON_BOOTS) (and RT_LOST_WOOD_NAVI_DIVE (and IsChild (and (HasItem RG_BRONZE_SCALE) CanJumpslash)))))
RR_LW_BEYOND_MIDO (or IsChild (or (CanUse RG_SARIAS_SONG) RT_LW_MIDO_BACKFLIP))
RR_LW_NEAR_SHORTCUTS_GROTTO (Here BlastOrSmash)

def RR_LW_BRIDGE_FROM_FOREST SCENE_LOST_WOODS false RA_THE_LOST_WOODS
LW Bridge From Forest
//Checks
RC_LW_GIFT_FROM_SARIA true
//Exits
RR_LW_BRIDGE true

def RR_LW_BRIDGE SCENE_LOST_WOODS false RA_THE_LOST_WOODS
LW Bridge
//Exits
RR_KOKIRI_FOREST true
RR_HYRULE_FIELD true
RR_THE_LOST_WOODS (CanUse RG_LONGSHOT)

def RR_LW_FOREST_EXIT SCENE_LOST_WOODS false RA_THE_LOST_WOODS
LW Forest Exit
//Exits
RR_KOKIRI_FOREST true

def RR_LW_BEYOND_MIDO SCENE_LOST_WOODS false RA_THE_LOST_WOODS
LW Beyond Mido
//Events
ButterflyFairy (CanUse RG_STICKS)
//Checks
RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_RIGHT (and IsChild CanStunDeku)
RC_LW_DEKU_SCRUB_NEAR_DEKU_THEATER_LEFT (and IsChild CanStunDeku)
RC_LW_GS_ABOVE_THEATER (and IsAdult (and (or (and (CanPlantBean RR_LW_BEYOND_MIDO) CanAttack) (and RT_LW_GS_BEAN (and (CanUse RG_HOOKSHOT) (or (CanUse RG_LONGSHOT) (or (CanUse RG_FAIRY_BOW) (or (CanUse RG_FAIRY_SLINGSHOT) (or (CanUse RG_BOMBCHU_5) (CanUse RG_DINS_FIRE)))))))) CanGetNightTimeGS))
RC_LW_GS_BEAN_PATCH_NEAR_THEATER (and CanSpawnSoilSkull (or CanAttack (and (== RSK_SHUFFLE_SCRUBS RO_SCRUBS_OFF) CanReflectNuts)))
RC_LW_BOULDER_RUPEE BlastOrSmash
RC_LW_BEAN_SPROUT_NEAR_THEATER_FAIRY_1 (and IsChild (and (HasItem RG_MAGIC_BEAN) (CanUse RG_SONG_OF_STORMS)))
RC_LW_BEAN_SPROUT_NEAR_THEATER_FAIRY_2 (and IsChild (and (HasItem RG_MAGIC_BEAN) (CanUse RG_SONG_OF_STORMS)))
RC_LW_BEAN_SPROUT_NEAR_THEATER_FAIRY_3 (and IsChild (and (HasItem RG_MAGIC_BEAN) (CanUse RG_SONG_OF_STORMS)))
RC_LW_GRASS_4 CanCutShrubs
RC_LW_GRASS_5 CanCutShrubs
RC_LW_GRASS_6 CanCutShrubs
RC_LW_GRASS_7 CanCutShrubs
RC_LW_GRASS_8 CanCutShrubs
RC_LW_GRASS_9 CanCutShrubs
//Exits
RR_LW_FOREST_EXIT true
RR_THE_LOST_WOODS (or IsChild (CanUse RG_SARIAS_SONG))
RR_SFM_ENTRYWAY true
RR_DEKU_THEATER true
RR_LW_SCRUBS_GROTTO (Here BlastOrSmash)

def RR_LW_NEAR_SHORTCUTS_GROTTO SCENE_GROTTOS false
LW Near Shortcuts Grotto
//Events
GossipStoneFairy CallGossipFairy
ButterflyFairy (CanUse RG_STICKS)
BugShrub CanCutShrubs
LoneFish true
//Checks
RC_LW_NEAR_SHORTCUTS_GROTTO_CHEST true
RC_LW_NEAR_SHORTCUTS_GROTTO_FISH HasBottle
RC_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE_FAIRY CallGossipFairy
RC_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE_FAIRY_BIG (CanUse RG_SONG_OF_STORMS)
RC_LW_NEAR_SHORTCUTS_GROTTO_GOSSIP_STONE true
RC_LW_NEAR_SHORTCUTS_GROTTO_BEEHIVE_LEFT CanBreakLowerBeehives
RC_LW_NEAR_SHORTCUTS_GROTTO_BEEHIVE_RIGHT CanBreakLowerBeehives
RC_LW_NEAR_SHORTCUTS_GROTTO_GRASS_1 CanCutShrubs
RC_LW_NEAR_SHORTCUTS_GROTTO_GRASS_2 CanCutShrubs
RC_LW_NEAR_SHORTCUTS_GROTTO_GRASS_3 CanCutShrubs
RC_LW_NEAR_SHORTCUTS_GROTTO_GRASS_4 CanCutShrubs
//Exits
RR_THE_LOST_WOODS true

def RR_DEKU_THEATER SCENE_GROTTOS false
Deku Theater
//Checks
RC_DEKU_THEATER_SKULL_MASK (and IsChild BorrowSkullMask)
RC_DEKU_THEATER_MASK_OF_TRUTH (and IsChild BorrowRightMasks)
//Exits
RR_LW_BEYOND_MIDO true

def RR_LW_SCRUBS_GROTTO SCENE_GROTTOS false
LW Scrubs Grotto
//Checks
RC_LW_DEKU_SCRUB_GROTTO_REAR CanStunDeku
RC_LW_DEKU_SCRUB_GROTTO_FRONT CanStunDeku
RC_LW_DEKU_SCRUB_GROTTO_BEEHIVE CanBreakUpperBeehives
RC_LW_DEKU_SCRUB_GROTTO_SUN_FAIRY (CanUse RG_SUNS_SONG)
//Exits
RR_LW_BEYOND_MIDO true

