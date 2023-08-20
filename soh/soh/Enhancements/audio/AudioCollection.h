#ifdef __cplusplus
#include <map>
#include <string>
#include <set>
#include <cstdint>

enum SeqType {
    SEQ_NOSHUFFLE  = 0,
    SEQ_BGM_WORLD  = 1 << 0,
    SEQ_BGM_EVENT  = 1 << 1,
    SEQ_BGM_BATTLE = 1 << 2,
    SEQ_OCARINA    = 1 << 3,
    SEQ_FANFARE    = 1 << 4,
    SEQ_BGM_ERROR  = 1 << 5,
    SEQ_SFX        = 1 << 6,
    SEQ_INSTRUMENT = 1 << 7,
    SEQ_VOICE      = 1 << 8,
    SEQ_BGM_CUSTOM = SEQ_BGM_WORLD | SEQ_BGM_EVENT | SEQ_BGM_BATTLE,
};

#define INSTRUMENT_OFFSET 0x81

struct SequenceInfo {
    uint16_t sequenceId;
    std::string label;
    std::string sfxKey;
    SeqType category;
};

class AudioCollection {
    private:
        // All Loaded Audio
        std::map<uint16_t, SequenceInfo> sequenceMap;
        
        // Sequences/SFX to include in/exclude from shuffle pool
        struct compareSequenceLabel {
            bool operator() (SequenceInfo* a, SequenceInfo* b) const {
                return a->label < b->label;
            };
        };
        std::set<SequenceInfo*, compareSequenceLabel> includedSequences;
        std::set<SequenceInfo*, compareSequenceLabel> excludedSequences;
        bool shufflePoolInitialized = false;

    public:
        static AudioCollection* Instance;
        AudioCollection();
        std::map<uint16_t, SequenceInfo> GetAllSequences() const {
            return sequenceMap;
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
        void InitializeShufflePool();
        const char* GetSequenceName(uint16_t seqId);
        bool HasSequenceNum(uint16_t seqId);
        size_t SequenceMapSize();
};
#else
void AudioCollection_AddToCollection(char *otrPath, uint16_t seqNum);
const char* AudioCollection_GetSequenceName(uint16_t seqId);
bool AudioCollection_HasSequenceNum(uint16_t seqId);
size_t AudioCollection_SequenceMapSize();
#endif