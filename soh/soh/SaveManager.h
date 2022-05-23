#pragma once

#include <ultra64/gbi.h>

typedef struct {
    u8 valid;
    u16 deaths;
    char playerName[8];
    u16 healthCapacity;
    u32 questItems;
    s8 defense;
    u16 health;
} SaveFileMetaInfo;

#ifdef __cplusplus

#include <map>
#include <string>
#include <tuple>
#include <functional>
#include <vector>
#include <filesystem>

#include "Lib/nlohmann/json.hpp"

class SaveManager {
public:
    static SaveManager* Instance;

    using InitFunc = void(*)(bool isDebug);
    using LoadFunc = void(*)();
    using SaveFunc = void(*)();
    using PostFunc = void(*)(int version);

    SaveManager();

    void Init();
    void InitFile(bool isDebug);
    void SaveFile(int fileNum);
    void SaveGlobal();
    void LoadFile(int fileNum);
    void AddInitFunction(InitFunc func);
    void AddLoadFunction(const std::string& name, int version, LoadFunc func);
    void AddSaveFunction(const std::string& name, int version, SaveFunc func);
    void AddPostFunction(const std::string& name, PostFunc func);

    void CopyZeldaFile(int from, int to);
    void DeleteZeldaFile(int fileNum);

    std::filesystem::path GetFileName(int fileNum);

    template<typename T>
    void SaveData(const std::string& name, const T& data) {
        if (name == "") {
            assert((*currentJsonContext).is_array());
            (*currentJsonContext).push_back(data);
        } else {
            (*currentJsonContext)[name.c_str()] = data;
        }
    }
    
    using SaveArrayFunc = std::function<void(size_t)>;
    void SaveArray(const std::string& name, const size_t size, SaveArrayFunc func);
    
    using SaveStructFunc = std::function<void()>;
    void SaveStruct(const std::string& name, SaveStructFunc func);

    template<typename T> void LoadData(const std::string& name, T& data, const T& defaultValue = T{}) {
        if (name == "") {
            if (currentJsonArrayContext == currentJsonContext->end()) {
                // This array member is past the data in the json file. Therefor, default construct it
                data = defaultValue;
            } else {
                currentJsonArrayContext.value().get_to(data);
            }
        } else if (!currentJsonContext->contains(name.c_str())) {
            data = defaultValue;
        } else {
            (*currentJsonContext)[name.c_str()].get_to(data);
        }
    }

    using LoadArrayFunc = std::function<void(size_t)>;
    void LoadArray(const std::string& name, const size_t size, LoadArrayFunc func);

    using LoadStructFunc = std::function<void()>;
    void LoadStruct(const std::string& name, LoadStructFunc func);

    static const int MaxFiles = 3;
    std::array<SaveFileMetaInfo, MaxFiles> fileMetaInfo;

private:
    void ConvertFromUnversioned();
    void CreateDefaultGlobal();
    void CreateDefaultSlot(int slotNum);

    void InitMeta(int slotNum);
    static void InitFileImpl(bool isDebug);
    static void InitFileNormal();
    static void InitFileDebug();

    static void LoadBaseVersion1();
    static void SaveBase();

    std::vector<InitFunc> initFuncs;

    using SectionLoadHandler = std::map<int, LoadFunc>;
    std::map<std::string, SectionLoadHandler> sectionLoadHandlers;

    using SectionSaveHandler = std::pair<int, SaveFunc>;
    std::map<std::string, SectionSaveHandler> sectionSaveHandlers;

    std::map<std::string, PostFunc> postHandlers;

    nlohmann::json* currentJsonContext = nullptr;
    nlohmann::json::iterator currentJsonArrayContext;
};

#else

typedef void (*Save_LoadFunc)(void);
typedef void (*Save_SaveFunc)(void);

void Save_Init(void);
void Save_InitFile(int isDebug);
void Save_SaveFile(void);
void Save_SaveGlobal(void);
void Save_LoadGlobal(void);
void Save_AddLoadFunction(char* name, int version, Save_LoadFunc func);
void Save_AddSaveFunction(char* name, int version, Save_SaveFunc func);
SaveFileMetaInfo* Save_GetSaveMetaInfo(int fileNum);
void Save_CopyFile(int from, int to);
void Save_DeleteFile(int fileNum);

#endif
