def RR_BOTTOM_OF_THE_WELL_ENTRYWAY SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well Entryway
//Exits
RR_BOTTOM_OF_THE_WELL_PERIMETER (and (IsDungeonVanilla BOTTOM_OF_THE_WELL) (and IsChild (CanPassEnemy RE_BIG_SKULLTULA)))
RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER (and (IsDungeonMQ BOTTOM_OF_THE_WELL) IsChild)
RR_KAK_WELL true

//Technically involves an fake wall, but passing it lensless is intended in vanilla and it is well telegraphed
def RR_BOTTOM_OF_THE_WELL_PERIMETER SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well Perimeter
//Events
StickPot true
NutPot true
LoweredWaterInsideBotw (CanUse RG_ZELDAS_LULLABY)
//Checks
RC_BOTTOM_OF_THE_WELL_FRONT_CENTER_BOMBABLE_CHEST HasExplosives
RC_BOTTOM_OF_THE_WELL_UNDERWATER_FRONT_CHEST (or LoweredWaterInsideBotw CanOpenUnderwaterChest)
RC_BOTTOM_OF_THE_WELL_UNDERWATER_LEFT_CHEST (or LoweredWaterInsideBotw CanOpenUnderwaterChest)
RC_BOTTOM_OF_THE_WELL_NEAR_ENTRANCE_POT_1 CanBreakPots
RC_BOTTOM_OF_THE_WELL_NEAR_ENTRANCE_POT_2 CanBreakPots
RC_BOTTOM_OF_THE_WELL_UNDERWATER_POT (or (and CanBreakPots LoweredWaterInsideBotw) (CanUse RG_BOOMERANG))
//Exits
RR_BOTTOM_OF_THE_WELL_ENTRYWAY (and IsChild (CanPassEnemy RE_BIG_SKULLTULA))
RR_BOTTOM_OF_THE_WELL_BEHIND_FAKE_WALLS (or RT_LENS_BOTW (CanUse RG_LENS_OF_TRUTH))
RR_BOTTOM_OF_THE_WELL_SOUTHWEST_ROOM (or RT_LENS_BOTW (CanUse RG_LENS_OF_TRUTH))
RR_BOTTOM_OF_THE_WELL_KEESE_BEAMOS_ROOM (and IsChild (SmallKeys SCENE_BOTTOM_OF_THE_WELL 3))
RR_BOTTOM_OF_THE_WELL_COFFIN_ROOM (or LoweredWaterInsideBotw (HasItem RG_BRONZE_SCALE))
RR_BOTTOM_OF_THE_WELL_DEAD_HAND_ROOM (and LoweredWaterInsideBotw IsChild)
//Falling down into basement requires nothing, but falling down somewhere specific requires lens or lens trick
//kinda questionable given several drops are blocked by rocks, but that's how it was handled before and on N64
RR_BOTTOM_OF_THE_WELL_BASEMENT true

//This region combines the Middle with the perimeter's hidden areas. If a warp puts link into the middle without crossing the perimeter or using lens, it will need it's own region
def RR_BOTTOM_OF_THE_WELL_BEHIND_FAKE_WALLS SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well Behind Fake Walls
//Checks
RC_BOTTOM_OF_THE_WELL_FRONT_LEFT_FAKE_WALL_CHEST true
RC_BOTTOM_OF_THE_WELL_RIGHT_BOTTOM_FAKE_WALL_CHEST true
RC_BOTTOM_OF_THE_WELL_COMPASS_CHEST true
//You can just barely pass the spider on the right side without damage or items, but it's probably tight enough to count as as a trick
RC_BOTTOM_OF_THE_WELL_CENTER_SKULLTULA_CHEST (or (CanPassEnemy RE_BIG_SKULLTULA) TakeDamage)
//Not technically behind a wall, but still logically needs lens due to pits
RC_BOTTOM_OF_THE_WELL_BACK_LEFT_BOMBABLE_CHEST HasExplosives
//Exits
RR_BOTTOM_OF_THE_WELL_PERIMETER (or RT_LENS_BOTW (CanUse RG_LENS_OF_TRUTH))
RR_BOTTOM_OF_THE_WELL_INNER_ROOMS (SmallKeys SCENE_BOTTOM_OF_THE_WELL 3)
RR_BOTTOM_OF_THE_WELL_BASEMENT true
RR_BOTTOM_OF_THE_WELL_BASEMENT_PLATFORM (or RT_LENS_BOTW (CanUse RG_LENS_OF_TRUTH))

//This area can be reached without lens in logic from basement, but that could require silver rupees if they are shuffled.
def RR_BOTTOM_OF_THE_WELL_SOUTHWEST_ROOM SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well Southwest Room
//Checks
RC_BOTTOM_OF_THE_WELL_LEFT_SIDE_POT_1 CanBreakPots
RC_BOTTOM_OF_THE_WELL_LEFT_SIDE_POT_2 CanBreakPots
RC_BOTTOM_OF_THE_WELL_LEFT_SIDE_POT_3 CanBreakPots
//Exits
RR_BOTTOM_OF_THE_WELL_PERIMETER (or RT_LENS_BOTW (CanUse RG_LENS_OF_TRUTH))

//Passing through this area needs lens, but entering doesn't, so that the fire keese can be killed without crossing the pits if enemy drops are ever shuffled
def RR_BOTTOM_OF_THE_WELL_KEESE_BEAMOS_ROOM SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well Keese-Beamos Room
//Checks
RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_CHEST (or RT_LENS_BOTW (CanUse RG_LENS_OF_TRUTH))
RC_BOTTOM_OF_THE_WELL_FIRE_KEESE_POT_1 (and CanBreakPots (or RT_LENS_BOTW (CanUse RG_LENS_OF_TRUTH)))
//Exits
RR_BOTTOM_OF_THE_WELL_PERIMETER (and IsChild (and (SmallKeys SCENE_BOTTOM_OF_THE_WELL 3) (or RT_LENS_BOTW (CanUse RG_LENS_OF_TRUTH))))
RR_BOTTOM_OF_THE_WELL_LIKE_LIKE_CAGE (or RT_LENS_BOTW (CanUse RG_LENS_OF_TRUTH))
//not sure if this lens check is needed, these holes are a bit too easy to find, but it matches existing logic
RR_BOTTOM_OF_THE_WELL_BASEMENT_USEFUL_BOMB_FLOWERS (or RT_LENS_BOTW (CanUse RG_LENS_OF_TRUTH))

def RR_BOTTOM_OF_THE_WELL_LIKE_LIKE_CAGE SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well Like-Like Cage
//Checks
RC_BOTTOM_OF_THE_WELL_LIKE_LIKE_CHEST true
RC_BOTTOM_OF_THE_WELL_GS_LIKE_LIKE_CAGE (CanGetEnemyDrop RE_GOLD_SKULLTULA ED_BOOMERANG)
//Exits
RR_BOTTOM_OF_THE_WELL_KEESE_BEAMOS_ROOM true

//If the player can voidwarp into one of these rooms they will need splitting up, and Fake walls will need specifying into middle and the rest moved to perimeter
def RR_BOTTOM_OF_THE_WELL_INNER_ROOMS SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well Inner Rooms
//Events
DekuBabaSticks CanGetDekuBabaSticks
DekuBabaNuts CanGetDekuBabaNuts
//Checks
RC_BOTTOM_OF_THE_WELL_GS_WEST_INNER_ROOM (CanGetEnemyDrop RE_GOLD_SKULLTULA ED_BOOMERANG)
RC_BOTTOM_OF_THE_WELL_GS_EAST_INNER_ROOM (CanGetEnemyDrop RE_GOLD_SKULLTULA ED_BOOMERANG)
//Exits
RR_BOTTOM_OF_THE_WELL_BEHIND_FAKE_WALLS (SmallKeys SCENE_BOTTOM_OF_THE_WELL 3)

def RR_BOTTOM_OF_THE_WELL_COFFIN_ROOM SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well Coffin Room
//Checks
RC_BOTTOM_OF_THE_WELL_FREESTANDING_KEY (or HasFireSourceWithTorch (CanUse RG_FAIRY_BOW))
RC_BOTTOM_OF_THE_WELL_COFFIN_ROOM_FRONT_LEFT_HEART true
RC_BOTTOM_OF_THE_WELL_COFFIN_ROOM_MIDDLE_RIGHT_HEART (or HasFireSourceWithTorch (CanUse RG_FAIRY_BOW))
//Exits
RR_BOTTOM_OF_THE_WELL_PERIMETER (or LoweredWaterInsideBotw (HasItem RG_BRONZE_SCALE))

def RR_BOTTOM_OF_THE_WELL_DEAD_HAND_ROOM SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well Dead Hand Room
//Checks
RC_BOTTOM_OF_THE_WELL_LENS_OF_TRUTH_CHEST (CanKillEnemy RE_DEAD_HAND)
RC_BOTTOM_OF_THE_WELL_INVISIBLE_CHEST (or RT_LENS_BOTW (CanUse RG_LENS_OF_TRUTH))
//This assumes we spawned in dead hand's room, if whatever trick made this relevant instead puts us in the previous room, remove the kill Dead Hand check.
//Exits
RR_BOTTOM_OF_THE_WELL_PERIMETER (and IsChild (CanKillEnemy RE_DEAD_HAND))

def RR_BOTTOM_OF_THE_WELL_BASEMENT SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well Basement
//Checks
RC_BOTTOM_OF_THE_WELL_MAP_CHEST BlastOrSmash
RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_1 CanBreakPots
RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_2 CanBreakPots
RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_3 CanBreakPots
RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_4 CanBreakPots
RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_5 CanBreakPots
RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_6 CanBreakPots
RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_7 CanBreakPots
RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_8 CanBreakPots
RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_9 CanBreakPots
RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_10 CanBreakPots
RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_11 CanBreakPots
RC_BOTTOM_OF_THE_WELL_BASEMENT_POT_12 CanBreakPots
RC_BOTTOM_OF_THE_WELL_BASEMENT_SUN_FAIRY (CanUse RG_SUNS_SONG)
RC_BOTTOM_OF_THE_WELL_BASEMENT_GRASS_1 CanCutShrubs
RC_BOTTOM_OF_THE_WELL_BASEMENT_GRASS_2 CanCutShrubs
RC_BOTTOM_OF_THE_WELL_BASEMENT_GRASS_3 CanCutShrubs
RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_1 (and CanCutShrubs BlastOrSmash)
RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_2 (and CanCutShrubs BlastOrSmash)
RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_3 (and CanCutShrubs BlastOrSmash)
RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_4 (and CanCutShrubs BlastOrSmash)
RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_5 (and CanCutShrubs BlastOrSmash)
RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_6 (and CanCutShrubs BlastOrSmash)
RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_7 (and CanCutShrubs BlastOrSmash)
RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_8 (and CanCutShrubs BlastOrSmash)
RC_BOTTOM_OF_THE_WELL_BASEMENT_BEHIND_ROCKS_GRASS_9 (and CanCutShrubs BlastOrSmash)
//Exits
RR_BOTTOM_OF_THE_WELL_SOUTHWEST_ROOM (and IsChild (CanPassEnemy RE_BIG_SKULLTULA))
//It's possible to abuse boulder's limited range of collision detection to detonate the flowers through the boulder with bow, but this is a glitch
//the exact range is just past the furthest away plank in the green goo section
RR_BOTTOM_OF_THE_WELL_BASEMENT_USEFUL_BOMB_FLOWERS (Here (or BlastOrSmash (or (CanUse RG_DINS_FIRE) (and (CanUse RG_STICKS) RT_BOTW_BASEMENT))))

def RR_BOTTOM_OF_THE_WELL_BASEMENT_USEFUL_BOMB_FLOWERS SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well Basement Useful Bomb Flowers
//Assumes RR_BOTTOM_OF_THE_WELL_BASEMENT access
//Checks
RC_BOTTOM_OF_THE_WELL_MAP_CHEST (HasItem RG_GORONS_BRACELET)
//Exits
RR_BOTTOM_OF_THE_WELL_BASEMENT CanDetonateUprightBombFlower

def RR_BOTTOM_OF_THE_WELL_BASEMENT_PLATFORM SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well Basement Platform
//Checks
RC_BOTTOM_OF_THE_WELL_BASEMENT_PLATFORM_LEFT_RUPEE true
RC_BOTTOM_OF_THE_WELL_BASEMENT_PLATFORM_BACK_LEFT_RUPEE true
RC_BOTTOM_OF_THE_WELL_BASEMENT_PLATFORM_MIDDLE_RUPEE true
RC_BOTTOM_OF_THE_WELL_BASEMENT_PLATFORM_BACK_RIGHT_RUPEE true
RC_BOTTOM_OF_THE_WELL_BASEMENT_PLATFORM_RIGHT_RUPEE true
//Exits
RR_BOTTOM_OF_THE_WELL_BASEMENT true

def RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well MQ Perimeter
//technically obsolete due to a wonder item fairy which only needs a projectile, but we don't have an event var for it yet
//Events
FairyPot (and (Here BlastOrSmash) CanHitEyeTargets)
//It is possible to hit the water switch with a pot from RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE, however the hitbox for making it activate is very unintuitive
//You have to throw the pot from further back to hit the switch from the front instead of the top, trying to hit the "fingers" directly
//This unintuitiveness means it should be a trick. ZL is needed to get a clear path to carry the pot
LoweredWaterInsideBotw (or CanJumpslash CanUseProjectile)
//Implies CanBreakPots()
//Checks
RC_BOTTOM_OF_THE_WELL_MQ_OUTER_LOBBY_POT (and (Here BlastOrSmash) CanHitEyeTargets)
RC_BOTTOM_OF_THE_WELL_MQ_BOMB_LEFT_HEART HasExplosives
RC_BOTTOM_OF_THE_WELL_MQ_BOMB_RIGHT_HEART HasExplosives
//Exits
RR_BOTTOM_OF_THE_WELL_ENTRYWAY IsChild
RR_BOTTOM_OF_THE_WELL_MQ_WEST_ROOM_SWITCH (and (Here BlastOrSmash) (CanPassEnemy RE_BIG_SKULLTULA))
RR_BOTTOM_OF_THE_WELL_MQ_COFFIN_ROOM (and (or LoweredWaterInsideBotw (HasItem RG_BRONZE_SCALE)) (SmallKeys SCENE_BOTTOM_OF_THE_WELL 2))
RR_BOTTOM_OF_THE_WELL_MQ_LOCKED_CAGE (and IsChild (and (SmallKeys SCENE_BOTTOM_OF_THE_WELL 2) CanUseProjectile))
RR_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_ROOM (and IsChild LoweredWaterInsideBotw)
RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE (CanUse RG_ZELDAS_LULLABY)
RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT true

def RR_BOTTOM_OF_THE_WELL_MQ_WEST_ROOM_SWITCH SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well MQ West Room Switch
//Events
OpenedWestRoomMQBotw true
//Exits
RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER (and BlastOrSmash (or (CanPassEnemy RE_BIG_SKULLTULA) RT_BOTW_MQ_PITS))
RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE RT_BOTW_MQ_PITS
RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT true

def RR_BOTTOM_OF_THE_WELL_MQ_COFFIN_ROOM SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well MQ Coffin Room
//Checks
RC_BOTTOM_OF_THE_WELL_MQ_GS_COFFIN_ROOM (CanGetEnemyDrop RE_GOLD_SKULLTULA)
RC_BOTTOM_OF_THE_WELL_MQ_COFFIN_ROOM_FRONT_RIGHT_HEART (or HasFireSourceWithTorch (CanUse RG_FAIRY_BOW))
RC_BOTTOM_OF_THE_WELL_MQ_COFFIN_ROOM_MIDDLE_LEFT_HEART (or HasFireSourceWithTorch (CanUse RG_FAIRY_BOW))
//Exits
RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER (and (or LoweredWaterInsideBotw (HasItem RG_BRONZE_SCALE)) (SmallKeys SCENE_BOTTOM_OF_THE_WELL 2))

def RR_BOTTOM_OF_THE_WELL_MQ_LOCKED_CAGE SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well MQ Locked Cage
//Events
OpenedMiddleHoleMQBotw HasExplosives
//Exits
RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER (and IsChild (SmallKeys SCENE_BOTTOM_OF_THE_WELL 2))

def RR_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_ROOM SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well MQ Dead Hand Room
//Checks
RC_BOTTOM_OF_THE_WELL_MQ_COMPASS_CHEST (CanKillEnemy RE_DEAD_HAND)
RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_FREESTANDING_KEY (or HasExplosives (and RT_BOTW_MQ_DEADHAND_KEY (CanUse RG_BOOMERANG)))
RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_GRASS_1 CanCutShrubs
RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_GRASS_2 CanCutShrubs
RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_GRASS_3 CanCutShrubs
RC_BOTTOM_OF_THE_WELL_MQ_DEAD_HAND_GRASS_4 CanCutShrubs
//This assumes we spawned in dead hand's room, if whatever trick made this relevant instead puts us in the previous room, remove the kill Dead Hand check.
//Exits
RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER (and IsChild (CanKillEnemy RE_DEAD_HAND))

def RR_BOTTOM_OF_THE_WELL_MQ_MIDDLE SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well MQ Middle
//Checks
RC_BOTTOM_OF_THE_WELL_MQ_MAP_CHEST true
//This location technically involves an invisible platform, but it's intended to do lensless in vanilla and is clearly signposted by pots.
RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_FREESTANDING_KEY true
//The enemies in this room are invisible and crowd around the player, being awkward to deal with blind unless you already know how.
//the right wall is safe, and can be followed to get behind the grave which you can then pull easily assuming you can tank invisible keese
//Using a deku nut however stuns everything easily. and if you have a melee weapon you can kill the skull through the grave then grab the drop
//though it can be hard to tell where the safe direct path to the grave is without lens.
//Also you get cheap shotted on entry sometimes.
//An MQ lens trick is recommended here, and a review of this room for OHKO logic what that is added is advised.
//In the meantime I assume damage taken or the easy answer (nuts)
RC_BOTTOM_OF_THE_WELL_MQ_GS_WEST_INNER_ROOM (and OpenedWestRoomMQBotw (and (or TakeDamage (CanUse RG_NUTS)) (CanGetEnemyDrop RE_GOLD_SKULLTULA)))
RC_BOTTOM_OF_THE_WELL_MQ_INNER_LOBBY_POT_1 CanBreakPots
RC_BOTTOM_OF_THE_WELL_MQ_INNER_LOBBY_POT_2 CanBreakPots
RC_BOTTOM_OF_THE_WELL_MQ_INNER_LOBBY_POT_3 CanBreakPots
RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_POT_1 CanBreakPots
RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_POT_2 CanBreakPots
RC_BOTTOM_OF_THE_WELL_MQ_EAST_INNER_ROOM_POT_3 CanBreakPots
RC_BOTTOM_OF_THE_WELL_MQ_CELL_SUN_FAIRY (CanUse RG_SUNS_SONG)
//If a relevant trick causes you to be able to warp into here without going through PERIMETER, a new eventAccess will be needed for lowering the gates with ZL
//Exits
RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT_SWITCH_PLATFORM OpenedMiddleHoleMQBotw
RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT true

def RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well MQ Basement
//behind invisible big skulltulas, but with navi spotting it's easy to avoid them, or at worst, tank your way through as they do not block the path
//Checks
RC_BOTTOM_OF_THE_WELL_MQ_GS_BASEMENT (CanGetEnemyDrop RE_GOLD_SKULLTULA)
RC_BOTTOM_OF_THE_WELL_MQ_BASEMENT_HALLWAY_FRONT_HEART true
RC_BOTTOM_OF_THE_WELL_MQ_BASEMENT_HALLWAY_LEFT_HEART true
RC_BOTTOM_OF_THE_WELL_MQ_BASEMENT_HALLWAY_RIGHT_HEART true
RC_BOTTOM_OF_THE_WELL_MQ_BASEMENT_SUN_FAIRY (CanUse RG_SUNS_SONG)
//Exits
RR_BOTTOM_OF_THE_WELL_MQ_PERIMETER true

def RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT_SWITCH_PLATFORM SCENE_BOTTOM_OF_THE_WELL false RA_BOTTOM_OF_THE_WELL
Bottom of the Well MQ Basement Switch Platform
//Assumes RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT access
//it is technically possible to get the chest before you get screamed at without rolling, but hard enough to be a trick if that is the requirement for something to be logical
//With some kind of movement tech it's much easier, easy enough to be default logic, as the redeads don't lock on immediately in addition to the extra speed
//leaving with no requirements for now but up for discussion.
//Checks
RC_BOTTOM_OF_THE_WELL_MQ_LENS_OF_TRUTH_CHEST true
//Exits
RR_BOTTOM_OF_THE_WELL_MQ_BASEMENT true
