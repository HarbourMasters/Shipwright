#include "ActorExtension.h"

static ActorExtensionId sNextId = 0;

struct ActorExtensionKeyHash {
    std::size_t operator()(const std::pair<Actor*, ActorExtensionId>& key) const {
        return std::hash<Actor*>{}(key.first) ^ (std::hash<ActorExtensionId>{}(key.second) << 1);
    }
};

static std::unordered_map<ActorExtensionId, size_t> sGlobalSizes;
static std::unordered_map<s16, std::unordered_map<ActorExtensionId, size_t>> sSizes;
static std::unordered_map<std::pair<Actor*, ActorExtensionId>, void*, ActorExtensionKeyHash> sData;

void* ActorExtension_Get(Actor* actor, ActorExtensionId id) {
    if (actor == nullptr) {
        return nullptr;
    }

    auto it = sData.find(std::make_pair(actor, id));
    if (it == sData.end()) {
        return nullptr;
    }

    return it->second;
}

ActorExtensionId ActorExtension_CreateForId(int16_t actorId, size_t size) {
    ActorExtensionId id = ++sNextId;
    sSizes[actorId][id] = size;
    return id;
}

ActorExtensionId ActorExtension_CreateForAll(size_t size) {
    ActorExtensionId id = ++sNextId;
    sGlobalSizes[id] = size;
    return id;
}

void ActorExtension_Alloc(Actor* actor, int16_t actorId) {
    if (actor == nullptr) {
        return;
    }

    for (auto& [id, size] : sGlobalSizes) {
        void* data = malloc(size);
        memset(data, 0, size);
        sData[std::make_pair(actor, id)] = data;
    }

    for (auto& [id, size] : sSizes[actorId]) {
        void* data = malloc(size);
        memset(data, 0, size);
        sData[std::make_pair(actor, id)] = data;
    }
}

void ActorExtension_Free(Actor* actor) {
    if (actor == nullptr) {
        return;
    }

    for (auto& [ id, size ] : sGlobalSizes) {
        auto it = sData.find(std::make_pair(actor, id));
        if (it != sData.end()) {
            free(it->second);
            sData.erase(it);
        }
    }

    for (auto& [id, size] : sSizes[actor->id]) {
        auto it = sData.find(std::make_pair(actor, id));
        if (it != sData.end()) {
            free(it->second);
            sData.erase(it);
        }
    }
}