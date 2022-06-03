#include "music.hpp"
#include <cstdlib>

namespace Music {
    const std::array<SeqType, SEQ_COUNT> seqTypesMusic = {
        /* NA_BGM_FIELD */              SEQ_BGM_WORLD,
        /* NA_BGM_DUNGEON */            SEQ_BGM_WORLD,
        /* NA_BGM_KAKARIKO_ADULT */     SEQ_BGM_WORLD,
        /* NA_BGM_ENEMY */              SEQ_NOSHUFFLE, // Temporarily unshuffled: Override plays incorrect in some areas, like Lake Hylia, by continuously repeating the start
        /* NA_BGM_BOSS00 */             SEQ_BGM_BATTLE,
        /* NA_BGM_FAIRY_DUNGEON */      SEQ_BGM_WORLD,
        /* NA_BGM_MARKET */             SEQ_BGM_WORLD,
        /* NA_BGM_TITLE */              SEQ_BGM_WORLD,
        /* NA_BGM_LINK_HOUSE */         SEQ_BGM_WORLD,
        /* NA_BGM_GAME_OVER */          SEQ_FANFARE,
        /* NA_BGM_BOSS_CLEAR */         SEQ_FANFARE,
        /* NA_BGM_ITEM_GET */           SEQ_FANFARE,
        /* NA_BGM_OPENING_GANON */      SEQ_FANFARE,
        /* NA_BGM_HEART_GET */          SEQ_FANFARE,
        /* NA_BGM_OCA_LIGHT */          SEQ_OCARINA,
        /* NA_BGM_BUYO_DUNGEON */       SEQ_BGM_WORLD,
        /* NA_BGM_KAKARIKO_KID */       SEQ_BGM_WORLD,
        /* NA_BGM_GODESS */             SEQ_BGM_EVENT,
        /* NA_BGM_HIME */               SEQ_BGM_EVENT,
        /* NA_BGM_FIRE_DUNGEON */       SEQ_BGM_WORLD,
        /* NA_BGM_OPEN_TRE_BOX */       SEQ_FANFARE,
        /* NA_BGM_FORST_DUNGEON */      SEQ_BGM_WORLD,
        /* NA_BGM_HIRAL_GARDEN */       SEQ_BGM_WORLD,
        /* NA_BGM_GANON_TOWER */        SEQ_BGM_WORLD,
        /* NA_BGM_RONRON */             SEQ_BGM_WORLD,
        /* NA_BGM_GORON */              SEQ_BGM_WORLD,
        /* NA_BGM_SPIRIT_STONE */       SEQ_FANFARE,
        /* NA_BGM_OCA_FLAME */          SEQ_OCARINA,
        /* NA_BGM_OCA_WIND */           SEQ_OCARINA,
        /* NA_BGM_OCA_WATER */          SEQ_OCARINA,
        /* NA_BGM_OCA_SOUL */           SEQ_OCARINA,
        /* NA_BGM_OCA_DARKNESS */       SEQ_OCARINA,
        /* NA_BGM_MIDDLE_BOSS */        SEQ_BGM_ERROR,
        /* NA_BGM_S_ITEM_GET */         SEQ_FANFARE,
        /* NA_BGM_SHRINE_OF_TIME */     SEQ_BGM_WORLD,
        /* NA_BGM_EVENT_CLEAR */        SEQ_FANFARE,
        /* NA_BGM_KOKIRI */             SEQ_BGM_WORLD,
        /* NA_BGM_OCA_YOUSEI */         SEQ_FANFARE,
        /* NA_BGM_MAYOIMORI */          SEQ_BGM_WORLD,
        /* NA_BGM_SOUL_DUNGEON */       SEQ_BGM_WORLD,
        /* NA_BGM_HORSE */              SEQ_BGM_EVENT,
        /* NA_BGM_HORSE_GOAL */         SEQ_FANFARE,
        /* NA_BGM_INGO */               SEQ_BGM_WORLD,
        /* NA_BGM_MEDAL_GET */          SEQ_FANFARE,
        /* NA_BGM_OCA_SARIA */          SEQ_OCARINA,
        /* NA_BGM_OCA_EPONA */          SEQ_OCARINA,
        /* NA_BGM_OCA_ZELDA */          SEQ_OCARINA,
        /* NA_BGM_OCA_SUNMOON */        SEQ_NOSHUFFLE, /* Remove Sun's Song from the Ocarina pool for now due to bugs */
        /* NA_BGM_OCA_TIME */           SEQ_OCARINA,
        /* NA_BGM_OCA_STORM */          SEQ_OCARINA,
        /* NA_BGM_NAVI */               SEQ_BGM_EVENT,
        /* NA_BGM_DEKUNOKI */           SEQ_BGM_EVENT,
        /* NA_BGM_FUSHA */              SEQ_BGM_WORLD,
        /* NA_BGM_HIRAL_DEMO */         SEQ_NOSHUFFLE,
        /* NA_BGM_MINI_GAME */          SEQ_BGM_EVENT,
        /* NA_BGM_SEAK */               SEQ_BGM_EVENT,
        /* NA_BGM_ZORA */               SEQ_BGM_WORLD,
        /* NA_BGM_APPEAR */             SEQ_FANFARE,
        /* NA_BGM_ADULT_LINK */         SEQ_BGM_EVENT,
        /* NA_BGM_MASTER_SWORD */       SEQ_FANFARE,
        /* NA_BGM_INTRO_GANON */        SEQ_BGM_EVENT,
        /* NA_BGM_SHOP */               SEQ_BGM_WORLD,
        /* NA_BGM_KENJA */              SEQ_BGM_EVENT,
        /* NA_BGM_FILE_SELECT */        SEQ_NOSHUFFLE,
        /* NA_BGM_ICE_DUNGEON */        SEQ_BGM_WORLD,
        /* NA_BGM_GATE_OPEN */          SEQ_NOSHUFFLE,
        /* NA_BGM_OWL */                SEQ_BGM_EVENT,
        /* NA_BGM_DARKNESS_DUNGEON */   SEQ_BGM_WORLD,
        /* NA_BGM_AQUA_DUNGEON */       SEQ_BGM_WORLD,
        /* NA_BGM_BRIDGE */             SEQ_NOSHUFFLE,
        /* NA_BGM_SARIA */              SEQ_NOSHUFFLE,
        /* NA_BGM_GERUDO */             SEQ_BGM_WORLD,
        /* NA_BGM_DRUGSTORE */          SEQ_BGM_WORLD,
        /* NA_BGM_KOTAKE_KOUME */       SEQ_BGM_EVENT,
        /* NA_BGM_ESCAPE */             SEQ_BGM_EVENT,
        /* NA_BGM_UNDERGROUND */        SEQ_BGM_WORLD,
        /* NA_BGM_GANON_BATTLE_1 */     SEQ_BGM_BATTLE,
        /* NA_BGM_GANON_BATTLE_2 */     SEQ_BGM_BATTLE,
        /* NA_BGM_END_DEMO */           SEQ_NOSHUFFLE,
        /* NA_BGM_STAFF_1 */            SEQ_NOSHUFFLE,
        /* NA_BGM_STAFF_2 */            SEQ_NOSHUFFLE,
        /* NA_BGM_STAFF_3 */            SEQ_NOSHUFFLE,
        /* NA_BGM_STAFF_4 */            SEQ_NOSHUFFLE,
        /* NA_BGM_BOSS01 */             SEQ_BGM_BATTLE,
        /* NA_BGM_MINI_GAME_2 */        SEQ_BGM_ERROR,
    };

    std::array<uint32_t, SEQ_COUNT> seqOverridesMusic;

    /* Initializes the list of music overrides to unshuffled */
    void InitMusicRandomizer() {
        for(int i = 0; i < SEQ_COUNT; i++)
            seqOverridesMusic[i] = BGM_BASE + i;
    }

    /* Shuffles the sequences grouping them by type */
    /* type is a bitmask of SeqType */
    void ShuffleSequences(int type) {
        std::vector<uint32_t> seqs;

        // Get all sequences of the desired type(s) into a vector
        for (int i = 0; i < SEQ_COUNT; i++) {
            if (seqTypesMusic[i] & type) {
                seqs.push_back(seqOverridesMusic[i]);
            }
        }

        // Shuffle the vector...
        for (std::size_t i = 0; i < seqs.size(); i++)
        {
            std::swap(seqs[i], seqs[rand() %  seqs.size()]);
        }

        // ...and feed it back into the overrides array
        for (int i = 0; i < SEQ_COUNT; i++) {
            if (seqTypesMusic[i] & type)
            {
                seqOverridesMusic[i] = seqs.back();
                seqs.pop_back();
            }
        }
    }
} // namespace Music
