#pragma once
#include "z64actor.h"

typedef struct {
    const char* name;
    const char* desc;
    u32 valid;
    s32 id;
    s32 category;
    u32 flags;
    s32 objectId;
    size_t instanceSize;
    ActorFunc init;
    ActorFunc destroy;
    ActorFunc update;
    ActorFunc draw;
    ActorResetFunc reset;
    s32 numLoaded;
} ActorDBEntry;

#ifdef __cplusplus

#include <string>
#include <unordered_map>
#include <vector>

struct ActorDBInit {
    std::string name;
    std::string desc;
    int category = 0;
    u32 flags = 0;
    int objectId = 0;
    size_t instanceSize = sizeof(Actor);
    ActorFunc init = nullptr;
    ActorFunc destroy = nullptr;
    ActorFunc update = nullptr;
    ActorFunc draw = nullptr;
    ActorResetFunc reset = nullptr;
};

class ActorDB {
public:
    static ActorDB* Instance;

    ActorDB();

    // Wrapper around ActorDBEntry so we get C++isms for the entries
    struct Entry {
        Entry();
        Entry(const Entry& other);
        Entry& operator=(const Entry& other);

        void SetName(const std::string& newName);
        void SetDesc(const std::string& newDesc);

        std::string name;
        std::string desc;
        ActorDBEntry entry;
    };
    Entry& AddEntry(const ActorDBInit& init);

    Entry& RetrieveEntry(const int id);
    int RetrieveId(const std::string& name);

    static void AddBuiltInCustomActors();

    int GetEntryCount();
private:
    Entry& AddEntry(const std::string& name, const std::string& desc, size_t index);
    Entry& AddEntry(const std::string& name, const std::string& desc, const ActorInit& init);

    std::vector<Entry> db;
    std::unordered_map<std::string, int> nameTable;
    size_t nextFreeId = 0;
};

#else

ActorDBEntry* ActorDB_Retrieve(const int id);
int ActorDB_RetrieveId(const char* name);

#endif
