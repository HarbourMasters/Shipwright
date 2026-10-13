#pragma once
#ifdef __cplusplus
#include <map>
#include <string>
#include <set>
#include <cstdint>

enum SeqType {
    SEQ_NOSHUFFLE = 0,
    SEQ_BGM_WORLD = 1 << 0,
    SEQ_BGM_EVENT = 1 << 1,
    SEQ_BGM_BATTLE = 1 << 2,
    SEQ_OCARINA = 1 << 3,
    SEQ_FANFARE = 1 << 4,
    SEQ_BGM_ERROR = 1 << 5,
    SEQ_SFX = 1 << 6,
    SEQ_INSTRUMENT = 1 << 7,
    SEQ_VOICE = 1 << 8,
    SEQ_BGM_SONGS = 1 << 9,
    SEQ_BGM_CUSTOM = SEQ_BGM_WORLD | SEQ_BGM_BATTLE,
    SEQ_BGM_CUSTOM_FANFARE = SEQ_FANFARE | SEQ_OCARINA | SEQ_BGM_SONGS | SEQ_BGM_EVENT,
};

enum SeqCategory {
    SEQ_CAT_NONE = 0,
    SEQ_CAT_FIELD = 1 << 0,
    SEQ_CAT_TOWN = 1 << 1,
    SEQ_CAT_DUNGEON = 1 << 2,
    SEQ_CAT_INDOOR = 1 << 3,
    SEQ_CAT_MINIGAME = 1 << 4,
    SEQ_CAT_ACTION = 1 << 5,
    SEQ_CAT_CALM = 1 << 6,
    SEQ_CAT_BOSS = 1 << 7,
    SEQ_CAT_FAN_GETITEM = 1 << 8,
    SEQ_CAT_FAN_GAMEOVER = 1 << 9,
    SEQ_CAT_FAN_CLEAR = 1 << 10,
    SEQ_CAT_TITLE = 1 << 16,
    SEQ_CAT_ID_REPLACEMENT = 1 << 17, // only for temporary use during selection
    SEQ_CAT_BGM = SEQ_CAT_FIELD | SEQ_CAT_TOWN | SEQ_CAT_DUNGEON | SEQ_CAT_INDOOR | SEQ_CAT_MINIGAME | SEQ_CAT_ACTION |
                  SEQ_CAT_CALM | SEQ_CAT_BOSS | SEQ_CAT_TITLE,
    SEQ_CAT_FAN = SEQ_CAT_FAN_GETITEM | SEQ_CAT_FAN_GAMEOVER | SEQ_CAT_FAN_CLEAR,
};

#define SEQUENCE_ID_REPLACEMENT_OFFSET 0x100

#define INSTRUMENT_OFFSET 0x81

struct SequenceInfo {
    uint16_t sequenceId;
    std::string label;
    std::string sfxKey;
    SeqType type;
    int categoryFlags;
    std::shared_ptr<std::vector<int>> seqIdReplacements;
    bool canBeReplaced;
    bool canBeUsedAsReplacement;
};

struct SequenceReplacement {
    SequenceInfo* seq;
    bool hasBeenUsed;
};

class AudioCollection {
  private:
    // All Loaded Audio
    std::map<uint16_t, SequenceInfo> mSequenceMap;

    // Sequences/SFX to include in/exclude from shuffle pool
    struct compareSequenceLabel {
        bool operator()(SequenceInfo* a, SequenceInfo* b) const {
            return a->label < b->label;
        };
    };
    std::set<SequenceInfo*, compareSequenceLabel> includedSequences;
    std::set<SequenceInfo*, compareSequenceLabel> excludedSequences;
    bool shufflePoolInitialized = false;

    std::map<SeqType, size_t> mSequenceTypeCounts;

  public:
    static AudioCollection* Instance;
    AudioCollection();
    const std::map<uint16_t, SequenceInfo>& GetAllSequences() const {
        return mSequenceMap;
    }
    std::set<SequenceInfo*, compareSequenceLabel> GetIncludedSequences() const {
        return includedSequences;
    };
    std::set<SequenceInfo*, compareSequenceLabel> GetExcludedSequences() const {
        return excludedSequences;
    };
    void AddToShufflePool(SequenceInfo*);
    void RemoveFromShufflePool(SequenceInfo*);
    void AddToCollection(char* otrPath, uint16_t seqNum);
    uint16_t GetReplacementSequence(uint16_t seqId);
    uint16_t GetOriginalSequence(uint16_t seqId);
    void InitializeShufflePool();
    const char* GetSequenceName(uint16_t seqId);
    bool HasSequenceNum(uint16_t seqId);
    size_t SequenceMapSize();
    std::string GetCvarKey(std::string sfxKey);
    std::string GetCvarLockKey(std::string sfxKey);
    size_t CountSequencesByType(SeqType type);
    uint16_t GetMaxOriginalSeqId() const;
    void ParseSequenceCategory(std::string token, int& compositeCategory, std::vector<int>& seqIds);
};
#else
void AudioCollection_AddToCollection(char* otrPath, uint16_t seqNum);
const char* AudioCollection_GetSequenceName(uint16_t seqId);
bool AudioCollection_HasSequenceNum(uint16_t seqId);
size_t AudioCollection_SequenceMapSize();
#endif