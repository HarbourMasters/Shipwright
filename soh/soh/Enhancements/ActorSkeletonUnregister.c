#include <stddef.h>
#include <stdint.h>

#include "soh/ResourceManagerHelpers.h"

#include "z64.h"
#include "include/z64player.h"
#include "src/overlays/actors/ovl_Bg_Dy_Yoseizo/z_bg_dy_yoseizo.h"
#include "src/overlays/actors/ovl_Boss_Sst/z_boss_sst.h"
#include "src/overlays/actors/ovl_Demo_Ik/z_demo_ik.h"
#include "src/overlays/actors/ovl_Demo_Im/z_demo_im.h"
#include "src/overlays/actors/ovl_Door_Warp1/z_door_warp1.h"
#include "src/overlays/actors/ovl_Door_Killer/z_door_killer.h"
#include "src/overlays/actors/ovl_En_Am/z_en_am.h"
#include "src/overlays/actors/ovl_En_Ani/z_en_ani.h"
#include "src/overlays/actors/ovl_En_Anubice/z_en_anubice.h"
#include "src/overlays/actors/ovl_En_Attack_Niw/z_en_attack_niw.h"
#include "src/overlays/actors/ovl_En_Bb/z_en_bb.h"
#include "src/overlays/actors/ovl_En_Bigokuta/z_en_bigokuta.h"
#include "src/overlays/actors/ovl_En_Bili/z_en_bili.h"
#include "src/overlays/actors/ovl_En_Bird/z_en_bird.h"
#include "src/overlays/actors/ovl_En_Bom_Bowl_Man/z_en_bom_bowl_man.h"
#include "src/overlays/actors/ovl_En_Box/z_en_box.h"
#include "src/overlays/actors/ovl_En_Brob/z_en_brob.h"
#include "src/overlays/actors/ovl_En_Butte/z_en_butte.h"
#include "src/overlays/actors/ovl_En_Bw/z_en_bw.h"
#include "src/overlays/actors/ovl_En_Cow/z_en_cow.h"
#include "src/overlays/actors/ovl_En_Crow/z_en_crow.h"
#include "src/overlays/actors/ovl_En_Cs/z_en_cs.h"
#include "src/overlays/actors/ovl_En_Daiku/z_en_daiku.h"
#include "src/overlays/actors/ovl_En_Daiku_Kakariko/z_en_daiku_kakariko.h"
#include "src/overlays/actors/ovl_En_Dekubaba/z_en_dekubaba.h"
#include "src/overlays/actors/ovl_En_Dekunuts/z_en_dekunuts.h"
#include "src/overlays/actors/ovl_En_Dh/z_en_dh.h"
#include "src/overlays/actors/ovl_En_Dha/z_en_dha.h"
#include "src/overlays/actors/ovl_En_Diving_Game/z_en_diving_game.h"
#include "src/overlays/actors/ovl_En_Dns/z_en_dns.h"
#include "src/overlays/actors/ovl_En_Dnt_Jiji/z_en_dnt_jiji.h"
#include "src/overlays/actors/ovl_En_Dnt_Nomal/z_en_dnt_nomal.h"
#include "src/overlays/actors/ovl_En_Dodojr/z_en_dodojr.h"
#include "src/overlays/actors/ovl_En_Dodongo/z_en_dodongo.h"
#include "src/overlays/actors/ovl_En_Dog/z_en_dog.h"
#include "src/overlays/actors/ovl_En_Door/z_en_door.h"
#include "src/overlays/actors/ovl_En_Ds/z_en_ds.h"
#include "src/overlays/actors/ovl_En_Eiyer/z_en_eiyer.h"
#include "src/overlays/actors/ovl_En_Elf/z_en_elf.h"
#include "src/overlays/actors/ovl_En_Fd/z_en_fd.h"
#include "src/overlays/actors/ovl_En_Firefly/z_en_firefly.h"
#include "src/overlays/actors/ovl_En_Fish/z_en_fish.h"
#include "src/overlays/actors/ovl_En_Floormas/z_en_floormas.h"
#include "src/overlays/actors/ovl_En_Fr/z_en_fr.h"
#include "src/overlays/actors/ovl_En_Fu/z_en_fu.h"
#include "src/overlays/actors/ovl_En_Fw/z_en_fw.h"
#include "src/overlays/actors/ovl_En_Gb/z_en_gb.h"
#include "src/overlays/actors/ovl_En_Ge1/z_en_ge1.h"
#include "src/overlays/actors/ovl_En_Ge2/z_en_ge2.h"
#include "src/overlays/actors/ovl_En_Ge3/z_en_ge3.h"
#include "src/overlays/actors/ovl_En_GeldB/z_en_geldb.h"
#include "src/overlays/actors/ovl_En_Gm/z_en_gm.h"
#include "src/overlays/actors/ovl_En_Go2/z_en_go2.h"
#include "src/overlays/actors/ovl_En_Goma/z_en_goma.h"
#include "src/overlays/actors/ovl_En_Guest/z_en_guest.h"
#include "src/overlays/actors/ovl_En_Heishi1/z_en_heishi1.h"
#include "src/overlays/actors/ovl_En_Heishi2/z_en_heishi2.h"
#include "src/overlays/actors/ovl_En_Heishi3/z_en_heishi3.h"
#include "src/overlays/actors/ovl_En_Heishi4/z_en_heishi4.h"
#include "src/overlays/actors/ovl_En_Hintnuts/z_en_hintnuts.h"
#include "src/overlays/actors/ovl_En_Hs/z_en_hs.h"
#include "src/overlays/actors/ovl_En_Hs2/z_en_hs2.h"
#include "src/overlays/actors/ovl_En_Hy/z_en_hy.h"
#include "src/overlays/actors/ovl_En_Ik/z_en_ik.h"
#include "src/overlays/actors/ovl_En_In/z_en_in.h"
#include "src/overlays/actors/ovl_En_Insect/z_en_insect.h"
#include "src/overlays/actors/ovl_En_Jj/z_en_jj.h"
#include "src/overlays/actors/ovl_En_Js/z_en_js.h"
#include "src/overlays/actors/ovl_En_Karebaba/z_en_karebaba.h"
#include "src/overlays/actors/ovl_En_Ko/z_en_ko.h"
#include "src/overlays/actors/ovl_En_Kz/z_en_kz.h"
#include "src/overlays/actors/ovl_En_Mb/z_en_mb.h"
#include "src/overlays/actors/ovl_En_Md/z_en_md.h"
#include "src/overlays/actors/ovl_En_Mk/z_en_mk.h"
#include "src/overlays/actors/ovl_En_Mm/z_en_mm.h"
#include "src/overlays/actors/ovl_En_Mm2/z_en_mm2.h"
#include "src/overlays/actors/ovl_En_Ms/z_en_ms.h"
#include "src/overlays/actors/ovl_En_Nb/z_en_nb.h"
#include "src/overlays/actors/ovl_En_Niw/z_en_niw.h"
#include "src/overlays/actors/ovl_En_Niw_Girl/z_en_niw_girl.h"
#include "src/overlays/actors/ovl_En_Niw_Lady/z_en_niw_lady.h"
#include "src/overlays/actors/ovl_En_Okuta/z_en_okuta.h"
#include "src/overlays/actors/ovl_En_Owl/z_en_owl.h"
#include "src/overlays/actors/ovl_En_Partner/z_en_partner.h"
#include "src/overlays/actors/ovl_En_Peehat/z_en_peehat.h"
#include "src/overlays/actors/ovl_En_Po_Desert/z_en_po_desert.h"
#include "src/overlays/actors/ovl_En_Po_Field/z_en_po_field.h"
#include "src/overlays/actors/ovl_En_Po_Relay/z_en_po_relay.h"
#include "src/overlays/actors/ovl_En_Po_Sisters/z_en_po_sisters.h"
#include "src/overlays/actors/ovl_En_Poh/z_en_poh.h"
#include "src/overlays/actors/ovl_En_Rd/z_en_rd.h"
#include "src/overlays/actors/ovl_En_Reeba/z_en_reeba.h"
#include "src/overlays/actors/ovl_En_Ru1/z_en_ru1.h"
#include "src/overlays/actors/ovl_En_Ru2/z_en_ru2.h"
#include "src/overlays/actors/ovl_En_Sa/z_en_sa.h"
#include "src/overlays/actors/ovl_En_Shopnuts/z_en_shopnuts.h"
#include "src/overlays/actors/ovl_En_Skb/z_en_skb.h"
#include "src/overlays/actors/ovl_En_Skj/z_en_skj.h"
#include "src/overlays/actors/ovl_En_Ssh/z_en_ssh.h"
#include "src/overlays/actors/ovl_En_St/z_en_st.h"
#include "src/overlays/actors/ovl_En_Sth/z_en_sth.h"
#include "src/overlays/actors/ovl_En_Sw/z_en_sw.h"
#include "src/overlays/actors/ovl_En_Syateki_Man/z_en_syateki_man.h"
#include "src/overlays/actors/ovl_En_Syateki_Niw/z_en_syateki_niw.h"
#include "src/overlays/actors/ovl_En_Ta/z_en_ta.h"
#include "src/overlays/actors/ovl_En_Takara_Man/z_en_takara_man.h"
#include "src/overlays/actors/ovl_En_Test/z_en_test.h"
#include "src/overlays/actors/ovl_En_Tite/z_en_tite.h"
#include "src/overlays/actors/ovl_En_Tk/z_en_tk.h"
#include "src/overlays/actors/ovl_En_Torch2/z_en_torch2.h"
#include "src/overlays/actors/ovl_En_Toryo/z_en_toryo.h"
#include "src/overlays/actors/ovl_En_Tr/z_en_tr.h"
#include "src/overlays/actors/ovl_En_Vali/z_en_vali.h"
#include "src/overlays/actors/ovl_En_Vm/z_en_vm.h"
#include "src/overlays/actors/ovl_En_Wallmas/z_en_wallmas.h"
#include "src/overlays/actors/ovl_En_Weiyer/z_en_weiyer.h"
#include "src/overlays/actors/ovl_En_Wf/z_en_wf.h"
#include "src/overlays/actors/ovl_En_Xc/z_en_xc.h"
#include "src/overlays/actors/ovl_En_Zf/z_en_zf.h"
#include "src/overlays/actors/ovl_En_Zl3/z_en_zl3.h"
#include "src/overlays/actors/ovl_En_Zl4/z_en_zl4.h"
#include "src/overlays/actors/ovl_En_Zo/z_en_zo.h"

// Use SkelAnime_Free only for the actors that pass NULL joint/morph tables to SkelAnime_Init, so the
// tables really are arena allocations. Everything else keeps its tables inside the actor struct, and
// freeing those interior pointers corrupts the zelda arena. Those actors want UNREGISTER_SKEL.
#define UNREGISTER_SKEL(type, field) \
    ResourceMgr_UnregisterSkeleton((SkelAnime*)((uintptr_t)(actor) + offsetof(type, field)))

void UnregisterActorSkeletons(Actor* actor) {
    switch (actor->id) {
        case ACTOR_BG_DY_YOSEIZO:
            UNREGISTER_SKEL(BgDyYoseizo, skelAnime);
            break;
        case ACTOR_BOSS_SST:
            UNREGISTER_SKEL(BossSst, skelAnime);
            break;
        case ACTOR_DEMO_IK:
            UNREGISTER_SKEL(DemoIk, skelAnime);
            break;
        case ACTOR_DEMO_IM:
            UNREGISTER_SKEL(DemoIm, skelAnime);
            break;
        case ACTOR_DOOR_WARP1: {
            DoorWarp1* doorWarp = (DoorWarp1*)actor;
            switch (doorWarp->actor.params) {
                case WARP_DUNGEON_ADULT:
                case WARP_BLUE_CRYSTAL:
                case WARP_PURPLE_CRYSTAL:
                    SkelAnime_Free(&doorWarp->skelAnime, gPlayState);
                    break;
                default:
                    break;
            }
            break;
        }
        case ACTOR_DOOR_KILLER:
            UNREGISTER_SKEL(DoorKiller, skelAnime);
            break;
        case ACTOR_EN_AM:
            UNREGISTER_SKEL(EnAm, skelAnime);
            break;
        case ACTOR_EN_ANI:
            UNREGISTER_SKEL(EnAni, skelAnime);
            break;
        case ACTOR_EN_ANUBICE:
            UNREGISTER_SKEL(EnAnubice, skelAnime);
            break;
        case ACTOR_EN_ATTACK_NIW:
            UNREGISTER_SKEL(EnAttackNiw, skelAnime);
            break;
        case ACTOR_EN_BB:
            UNREGISTER_SKEL(EnBb, skelAnime);
            break;
        case ACTOR_EN_BIGOKUTA:
            UNREGISTER_SKEL(EnBigokuta, skelAnime);
            break;
        case ACTOR_EN_BILI:
            UNREGISTER_SKEL(EnBili, skelAnime);
            break;
        case ACTOR_EN_BIRD:
            SkelAnime_Free(&((EnBird*)actor)->skelAnime, gPlayState);
            break;
        case ACTOR_EN_BOM_BOWL_MAN:
            UNREGISTER_SKEL(EnBomBowlMan, skelAnime);
            break;
        case ACTOR_EN_BOX:
            UNREGISTER_SKEL(EnBox, skelanime);
            break;
        case ACTOR_EN_BROB:
            UNREGISTER_SKEL(EnBrob, skelAnime);
            break;
        case ACTOR_EN_BUTTE:
            UNREGISTER_SKEL(EnButte, skelAnime);
            break;
        case ACTOR_EN_BW:
            UNREGISTER_SKEL(EnBw, skelAnime);
            break;
        case ACTOR_EN_COW:
            UNREGISTER_SKEL(EnCow, skelAnime);
            break;
        case ACTOR_EN_CROW:
            UNREGISTER_SKEL(EnCrow, skelAnime);
            break;
        case ACTOR_EN_CS:
            UNREGISTER_SKEL(EnCs, skelAnime);
            break;
        case ACTOR_EN_DAIKU:
            UNREGISTER_SKEL(EnDaiku, skelAnime);
            break;
        case ACTOR_EN_DAIKU_KAKARIKO:
            UNREGISTER_SKEL(EnDaikuKakariko, skelAnime);
            break;
        case ACTOR_EN_DEKUBABA:
            UNREGISTER_SKEL(EnDekubaba, skelAnime);
            break;
        case ACTOR_EN_DEKUNUTS:
            UNREGISTER_SKEL(EnDekunuts, skelAnime);
            break;
        case ACTOR_EN_DH:
            UNREGISTER_SKEL(EnDh, skelAnime);
            break;
        case ACTOR_EN_DHA:
            UNREGISTER_SKEL(EnDha, skelAnime);
            break;
        case ACTOR_EN_DIVING_GAME:
            UNREGISTER_SKEL(EnDivingGame, skelAnime);
            break;
        case ACTOR_EN_DNS:
            UNREGISTER_SKEL(EnDns, skelAnime);
            break;
        case ACTOR_EN_DNT_JIJI:
            UNREGISTER_SKEL(EnDntJiji, skelAnime);
            break;
        case ACTOR_EN_DNT_NOMAL:
            UNREGISTER_SKEL(EnDntNomal, skelAnime);
            break;
        case ACTOR_EN_DODOJR:
            UNREGISTER_SKEL(EnDodojr, skelAnime);
            break;
        case ACTOR_EN_DODONGO:
            UNREGISTER_SKEL(EnDodongo, skelAnime);
            break;
        case ACTOR_EN_DOG:
            UNREGISTER_SKEL(EnDog, skelAnime);
            break;
        case ACTOR_EN_DOOR:
            UNREGISTER_SKEL(EnDoor, skelAnime);
            break;
        case ACTOR_EN_DS:
            UNREGISTER_SKEL(EnDs, skelAnime);
            break;
        case ACTOR_EN_EIYER:
            UNREGISTER_SKEL(EnEiyer, skelanime);
            break;
        case ACTOR_EN_ELF:
            UNREGISTER_SKEL(EnElf, skelAnime);
            break;
        case ACTOR_EN_FD:
            UNREGISTER_SKEL(EnFd, skelAnime);
            break;
        case ACTOR_EN_FIREFLY:
            UNREGISTER_SKEL(EnFirefly, skelAnime);
            break;
        case ACTOR_EN_FISH:
            UNREGISTER_SKEL(EnFish, skelAnime);
            break;
        case ACTOR_EN_FLOORMAS:
            UNREGISTER_SKEL(EnFloormas, skelAnime);
            break;
        case ACTOR_EN_FR:
            UNREGISTER_SKEL(EnFr, skelAnime);
            UNREGISTER_SKEL(EnFr, skelAnimeButterfly);
            break;
        case ACTOR_EN_FU:
            UNREGISTER_SKEL(EnFu, skelanime);
            break;
        case ACTOR_EN_FW:
            UNREGISTER_SKEL(EnFw, skelAnime);
            break;
        case ACTOR_EN_GB:
            UNREGISTER_SKEL(EnGb, skelAnime);
            break;
        case ACTOR_EN_GE1:
            UNREGISTER_SKEL(EnGe1, skelAnime);
            break;
        case ACTOR_EN_GE2:
            UNREGISTER_SKEL(EnGe2, skelAnime);
            break;
        case ACTOR_EN_GE3:
            UNREGISTER_SKEL(EnGe3, skelAnime);
            break;
        case ACTOR_EN_GELDB:
            UNREGISTER_SKEL(EnGeldB, skelAnime);
            break;
        case ACTOR_EN_GM:
            UNREGISTER_SKEL(EnGm, skelAnime);
            break;
        case ACTOR_EN_GO2:
            UNREGISTER_SKEL(EnGo2, skelAnime);
            break;
        case ACTOR_EN_GOMA:
            UNREGISTER_SKEL(EnGoma, skelanime);
            break;
        case ACTOR_EN_GUEST:
            UNREGISTER_SKEL(EnGuest, skelAnime);
            break;
        case ACTOR_EN_HEISHI1:
            UNREGISTER_SKEL(EnHeishi1, skelAnime);
            break;
        case ACTOR_EN_HEISHI2:
            UNREGISTER_SKEL(EnHeishi2, skelAnime);
            break;
        case ACTOR_EN_HEISHI3:
            UNREGISTER_SKEL(EnHeishi3, skelAnime);
            break;
        case ACTOR_EN_HEISHI4:
            UNREGISTER_SKEL(EnHeishi4, skelAnime);
            break;
        case ACTOR_EN_HINTNUTS:
            UNREGISTER_SKEL(EnHintnuts, skelAnime);
            break;
        case ACTOR_EN_HS:
            UNREGISTER_SKEL(EnHs, skelAnime);
            break;
        case ACTOR_EN_HS2:
            UNREGISTER_SKEL(EnHs2, skelAnime);
            break;
        case ACTOR_EN_HY:
            UNREGISTER_SKEL(EnHy, skelAnime);
            break;
        case ACTOR_EN_IK:
            UNREGISTER_SKEL(EnIk, skelAnime);
            break;
        case ACTOR_EN_IN:
            UNREGISTER_SKEL(EnIn, skelAnime);
            break;
        case ACTOR_EN_INSECT:
            UNREGISTER_SKEL(EnInsect, skelAnime);
            break;
        case ACTOR_EN_JJ:
            UNREGISTER_SKEL(EnJj, skelAnime);
            break;
        case ACTOR_EN_JS:
            UNREGISTER_SKEL(EnJs, skelAnime);
            break;
        case ACTOR_EN_KAREBABA:
            UNREGISTER_SKEL(EnKarebaba, skelAnime);
            break;
        case ACTOR_EN_KO:
            UNREGISTER_SKEL(EnKo, skelAnime);
            break;
        case ACTOR_EN_KZ:
            UNREGISTER_SKEL(EnKz, skelanime);
            break;
        case ACTOR_EN_MB:
            UNREGISTER_SKEL(EnMb, skelAnime);
            break;
        case ACTOR_EN_MD:
            UNREGISTER_SKEL(EnMd, skelAnime);
            break;
        case ACTOR_EN_MK:
            UNREGISTER_SKEL(EnMk, skelAnime);
            break;
        case ACTOR_EN_MM:
            UNREGISTER_SKEL(EnMm, skelAnime);
            break;
        case ACTOR_EN_MM2:
            UNREGISTER_SKEL(EnMm2, skelAnime);
            break;
        case ACTOR_EN_MS:
            UNREGISTER_SKEL(EnMs, skelAnime);
            break;
        case ACTOR_EN_NB:
            UNREGISTER_SKEL(EnNb, skelAnime);
            break;
        case ACTOR_EN_NIW:
            UNREGISTER_SKEL(EnNiw, skelAnime);
            break;
        case ACTOR_EN_NIW_GIRL:
            UNREGISTER_SKEL(EnNiwGirl, skelAnime);
            break;
        case ACTOR_EN_NIW_LADY:
            UNREGISTER_SKEL(EnNiwLady, skelAnime);
            break;
        case ACTOR_EN_OKUTA:
            if (actor->params == 0) {
                UNREGISTER_SKEL(EnOkuta, skelAnime);
            }
            break;
        case ACTOR_EN_OWL:
            UNREGISTER_SKEL(EnOwl, skelAnime);
            UNREGISTER_SKEL(EnOwl, skelAnime2);
            break;
        case ACTOR_EN_PARTNER:
            UNREGISTER_SKEL(EnPartner, skelAnime);
            break;
        case ACTOR_EN_PEEHAT:
            UNREGISTER_SKEL(EnPeehat, skelAnime);
            break;
        case ACTOR_EN_PO_DESERT:
            UNREGISTER_SKEL(EnPoDesert, skelAnime);
            break;
        case ACTOR_EN_PO_FIELD:
            UNREGISTER_SKEL(EnPoField, skelAnime);
            break;
        case ACTOR_EN_PO_RELAY:
            UNREGISTER_SKEL(EnPoRelay, skelAnime);
            break;
        case ACTOR_EN_PO_SISTERS:
            UNREGISTER_SKEL(EnPoSisters, skelAnime);
            break;
        case ACTOR_EN_POH:
            UNREGISTER_SKEL(EnPoh, skelAnime);
            break;
        case ACTOR_EN_RD:
            UNREGISTER_SKEL(EnRd, skelAnime);
            break;
        case ACTOR_EN_REEBA:
            UNREGISTER_SKEL(EnReeba, skelanime);
            break;
        case ACTOR_EN_RU1:
            UNREGISTER_SKEL(EnRu1, skelAnime);
            break;
        case ACTOR_EN_RU2:
            UNREGISTER_SKEL(EnRu2, skelAnime);
            break;
        case ACTOR_EN_SA:
            UNREGISTER_SKEL(EnSa, skelAnime);
            break;
        case ACTOR_EN_SHOPNUTS:
            UNREGISTER_SKEL(EnShopnuts, skelAnime);
            break;
        case ACTOR_EN_SKB:
            UNREGISTER_SKEL(EnSkb, skelAnime);
            break;
        case ACTOR_EN_SKJ:
            UNREGISTER_SKEL(EnSkj, skelAnime);
            break;
        case ACTOR_EN_SSH:
            UNREGISTER_SKEL(EnSsh, skelAnime);
            break;
        case ACTOR_EN_ST:
            UNREGISTER_SKEL(EnSt, skelAnime);
            break;
        case ACTOR_EN_STH:
            UNREGISTER_SKEL(EnSth, skelAnime);
            break;
        case ACTOR_EN_SW:
            UNREGISTER_SKEL(EnSw, skelAnime);
            break;
        case ACTOR_EN_SYATEKI_MAN:
            UNREGISTER_SKEL(EnSyatekiMan, skelAnime);
            break;
        case ACTOR_EN_SYATEKI_NIW:
            UNREGISTER_SKEL(EnSyatekiNiw, skelAnime);
            break;
        case ACTOR_EN_TA:
            UNREGISTER_SKEL(EnTa, skelAnime);
            break;
        case ACTOR_EN_TAKARA_MAN:
            UNREGISTER_SKEL(EnTakaraMan, skelAnime);
            break;
        case ACTOR_EN_TEST:
            UNREGISTER_SKEL(EnTest, skelAnime);
            UNREGISTER_SKEL(EnTest, upperSkelanime);
            break;
        case ACTOR_EN_TITE:
            UNREGISTER_SKEL(EnTite, skelAnime);
            break;
        case ACTOR_EN_TK:
            UNREGISTER_SKEL(EnTk, skelAnime);
            break;
        case ACTOR_EN_TORCH2:
            UNREGISTER_SKEL(Player, skelAnime);
            UNREGISTER_SKEL(Player, upperSkelAnime);
            break;
        case ACTOR_EN_TORYO:
            UNREGISTER_SKEL(EnToryo, skelAnime);
            break;
        case ACTOR_EN_TR:
            UNREGISTER_SKEL(EnTr, skelAnime);
            break;
        case ACTOR_EN_VALI:
            UNREGISTER_SKEL(EnVali, skelAnime);
            break;
        case ACTOR_EN_VM:
            UNREGISTER_SKEL(EnVm, skelAnime);
            break;
        case ACTOR_EN_WALLMAS:
            UNREGISTER_SKEL(EnWallmas, skelAnime);
            break;
        case ACTOR_EN_WEIYER:
            UNREGISTER_SKEL(EnWeiyer, skelAnime);
            break;
        case ACTOR_EN_WF:
            UNREGISTER_SKEL(EnWf, skelAnime);
            break;
        case ACTOR_EN_XC:
            UNREGISTER_SKEL(EnXc, skelAnime);
            break;
        case ACTOR_EN_ZF:
            UNREGISTER_SKEL(EnZf, skelAnime);
            break;
        case ACTOR_EN_ZL3:
            UNREGISTER_SKEL(EnZl3, skelAnime);
            break;
        case ACTOR_EN_ZL4:
            UNREGISTER_SKEL(EnZl4, skelAnime);
            break;
        case ACTOR_EN_ZO:
            UNREGISTER_SKEL(EnZo, skelAnime);
            break;
        case ACTOR_PLAYER:
            UNREGISTER_SKEL(Player, skelAnime);
            UNREGISTER_SKEL(Player, upperSkelAnime);
            break;
        default:
            break;
    }
}
