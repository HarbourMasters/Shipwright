def RR_ZORAS_DOMAIN SCENE_ZORAS_DOMAIN false RA_ZORAS_DOMAIN
Zoras Domain
//Events
GossipStoneFairy CallGossipFairyExceptSuns
NutPot true
StickPot IsChild
FishGroup IsChild
KingZoraThawed (and IsAdult BlueFire)
DeliverLetter (and (CanUse RG_RUTOS_LETTER) (and IsChild (!= RSK_ZORAS_FOUNTAIN RO_ZF_OPEN)))
//Checks
RC_ZD_DIVING_MINIGAME (and (HasItem RG_BRONZE_SCALE) (and (HasItem RG_CHILD_WALLET) IsChild))
RC_ZD_CHEST (and IsChild (CanUse RG_STICKS))
RC_ZD_KING_ZORA_THAWED (and IsAdult KingZoraThawed)
RC_ZD_TRADE_PRESCRIPTION (and IsAdult (and KingZoraThawed (CanUse RG_PRESCRIPTION)))
RC_ZD_GS_FROZEN_WATERFALL (and IsAdult (and (or HookshotOrBoomerang (or (CanUse RG_FAIRY_SLINGSHOT) (or (CanUse RG_FAIRY_BOW) (or (and (CanUse RG_MAGIC_SINGLE) (or (CanUse RG_MASTER_SWORD) (or (CanUse RG_KOKIRI_SWORD) (CanUse RG_BIGGORON_SWORD)))) (and RT_ZD_GS CanJumpslashExceptHammer))))) CanGetNightTimeGS))
RC_ZD_FISH_1 (and IsChild HasBottle)
RC_ZD_FISH_2 (and IsChild HasBottle)
RC_ZD_FISH_3 (and IsChild HasBottle)
RC_ZD_FISH_4 (and IsChild HasBottle)
RC_ZD_FISH_5 (and IsChild HasBottle)
RC_ZD_GOSSIP_STONE_FAIRY CallGossipFairyExceptSuns
RC_ZD_GOSSIP_STONE_FAIRY_BIG (CanUse RG_SONG_OF_STORMS)
RC_ZD_GOSSIP_STONE true
RC_ZD_IN_FRONT_OF_KING_ZORA_BEEHIVE_LEFT (and IsChild CanBreakUpperBeehives)
RC_ZD_IN_FRONT_OF_KING_ZORA_BEEHIVE_RIGHT (and IsChild CanBreakUpperBeehives)
RC_ZD_NEAR_SHOP_POT_1 CanBreakPots
RC_ZD_NEAR_SHOP_POT_2 CanBreakPots
RC_ZD_NEAR_SHOP_POT_3 CanBreakPots
RC_ZD_NEAR_SHOP_POT_4 CanBreakPots
RC_ZD_NEAR_SHOP_POT_5 CanBreakPots
//Exits
RR_ZR_BEHIND_WATERFALL true
RR_LH_FROM_SHORTCUT (and IsChild (or (HasItem RG_SILVER_SCALE) (CanUse RG_IRON_BOOTS)))
RR_ZD_BEHIND_KING_ZORA (or DeliverLetter (or (== RSK_ZORAS_FOUNTAIN RO_ZF_OPEN) (or (and (== RSK_ZORAS_FOUNTAIN RO_ZF_CLOSED_CHILD) IsAdult) (and RT_ZD_KING_ZORA_SKIP IsAdult))))
RR_ZD_SHOP (or IsChild BlueFire)
RR_ZORAS_DOMAIN_ISLAND true

def RR_ZORAS_DOMAIN_ISLAND SCENE_ZORAS_DOMAIN false RA_ZORAS_DOMAIN
Zoras Domain Island
//Exits
RR_ZORAS_DOMAIN (or IsAdult (HasItem RG_BRONZE_SCALE))
RR_ZD_STORMS_GROTTO CanOpenStormsGrotto

def RR_ZD_BEHIND_KING_ZORA SCENE_ZORAS_DOMAIN false RA_ZORAS_DOMAIN
ZD Behind King Zora
//Events
KingZoraThawed (and IsAdult BlueFire)
//Checks
RC_ZD_BEHIND_KING_ZORA_BEEHIVE (and IsChild CanBreakUpperBeehives)
//Exits
RR_ZORAS_DOMAIN (or DeliverLetter (or (== RSK_ZORAS_FOUNTAIN RO_ZF_OPEN) (and (== RSK_ZORAS_FOUNTAIN RO_ZF_CLOSED_CHILD) IsAdult)))
RR_ZORAS_FOUNTAIN true

def RR_ZD_SHOP SCENE_ZORA_SHOP false
ZD Shop
//Checks
RC_ZD_SHOP_ITEM_1 true
RC_ZD_SHOP_ITEM_2 true
RC_ZD_SHOP_ITEM_3 true
RC_ZD_SHOP_ITEM_4 true
RC_ZD_SHOP_ITEM_5 true
RC_ZD_SHOP_ITEM_6 true
RC_ZD_SHOP_ITEM_7 true
RC_ZD_SHOP_ITEM_8 true
//Exits
RR_ZORAS_DOMAIN true

def RR_ZD_STORMS_GROTTO SCENE_GROTTOS false
ZD Storms Grotto
//Events
FreeFairies true
//Checks
RC_ZD_FAIRY_GROTTO_FAIRY_1 true
RC_ZD_FAIRY_GROTTO_FAIRY_2 true
RC_ZD_FAIRY_GROTTO_FAIRY_3 true
RC_ZD_FAIRY_GROTTO_FAIRY_4 true
RC_ZD_FAIRY_GROTTO_FAIRY_5 true
RC_ZD_FAIRY_GROTTO_FAIRY_6 true
RC_ZD_FAIRY_GROTTO_FAIRY_7 true
RC_ZD_FAIRY_GROTTO_FAIRY_8 true
//Exits
RR_ZORAS_DOMAIN_ISLAND true
