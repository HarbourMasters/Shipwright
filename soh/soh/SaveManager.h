#pragma once

#include <libultraship/libultra/gbi.h>

#define SECTION_PARENT_NONE -1
typedef struct {
    u8 valid;
    u16 deaths;
    char playerName[8];
    u16 healthCapacity;
    u32 questItems;
    s8 defense;
    u16 health;
    u32 requiresMasterQuest;
    u32 requiresOriginal;
    u8 seedHash[5];
    u8 randoSave;
    char buildVersion[50];
    s16 buildVersionMajor;
    s16 buildVersionMinor;
    s16 buildVersionPatch;

    u8 inventoryItems[24];
    u16 equipment;
    u32 upgrades;
    u8 isMagicAcquired;
    u8 isDoubleMagicAcquired;
    s16 rupees;
    s16 gsTokens;
    u8 isDoubleDefenseAcquired;
    u8 gregFound;
} SaveFileMetaInfo;

#ifdef __cplusplus

#include <map>
#include <string>
#include <tuple>
#include <functional>
#include <vector>
#include <filesystem>
#include "thread-pool/BS_thread_pool.hpp"

#include "z64save.h"

#include <nlohmann/json.hpp>

class SaveManager {
  public:
    static SaveManager* Instance;

    static void WriteSaveFile(const std::filesystem::path& savePath, uintptr_t addr, void* dramAddr, size_t size);
    static void ReadSaveFile(std::filesystem::path savePath, uintptr_t addr, void* dramAddr, size_t size);

    using InitFunc = void (*)(bool isDebug);
    using LoadFunc = void (*)();
    using SaveFunc = void (*)(SaveContext* saveContext, int sectionID, bool fullSave);
    using PostFunc = void (*)(int version);

    typedef struct {
        std::string name;
        int version;
        SaveManager::SaveFunc func;
        bool saveWithBase;
        int parentSection;
    } SaveFuncInfo;

    SaveManager();

    void Init();
    void InitFile(bool isDebug);
    void SaveFile(int fileNum);
    void SaveSection(int fileNum, int sectionID, bool threaded);
    int GetSaveSectionID(std::string& name);
    void SaveGlobal();
    void LoadFile(int fileNum);
    bool SaveFile_Exist(int fileNum);
    void ThreadPoolWait();

    // Adds a function that is called when we are intializing a save, including when we are loading a save.
    void AddInitFunction(InitFunc func);

    // Adds a function to handling loading a section
    void AddLoadFunction(const std::string& name, int version, LoadFunc func);

    // Adds a function that is called when saving. This should only be called once for each function, the version is
    // filled in automatically.
    int AddSaveFunction(const std::string& name, int version, SaveFunc func, bool saveWithBase, int parentSection);

    // Adds a function to be called after loading is complete. This is to handle any cleanup required from loading old
    // versions.
    void AddPostFunction(const std::string& name, PostFunc func);

    void CopyZeldaFile(int from, int to);
    void DeleteZeldaFile(int fileNum);
    bool IsRandoFile();

    // Use a name of "" to save to an array. You must be in a SaveArray callback.
    template <typename T> void SaveData(const std::string& name, const T& data) {
        if (name == "") {
            assert((*currentJsonContext).is_array());
            (*currentJsonContext).push_back(data);
        } else {
            (*currentJsonContext)[name.c_str()] = data;
        }
    }

    // In the SaveArrayFunc func, the name must be "" to save to the array.
    using SaveArrayFunc = std::function<void(size_t)>;
    void SaveArray(const std::string& name, const size_t size, SaveArrayFunc func);

    using SaveStructFunc = std::function<void()>;
    void SaveStruct(const std::string& name, SaveStructFunc func);

    // Use a name of "" to load from an array. You must be in a LoadArray callback.
    template <typename T> void LoadData(const std::string& name, T& data, const T& defaultValue = T{}) {
        if (name == "") {
            if (currentJsonArrayContext == currentJsonContext->end()) {
                // This array member is past the data in the json file. Therefore, default construct it
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

    // In the LoadArrayFunc func, the name must be "" to load from the array.
    using LoadArrayFunc = std::function<void(size_t)>;
    void LoadArray(const std::string& name, const size_t size, LoadArrayFunc func);

    using LoadStructFunc = std::function<void()>;
    void LoadStruct(const std::string& name, LoadStructFunc func);

    static const int MaxFiles = 3;
    std::array<SaveFileMetaInfo, MaxFiles> fileMetaInfo;

  private:
    std::filesystem::path GetFileName(int fileNum);
    nlohmann::json saveBlock;

    void ConvertFromUnversioned();
    void CreateDefaultGlobal();

    void SaveFileThreaded(int fileNum, SaveContext* saveContext, int sectionID);

    void InitMeta(int slotNum);
    static void InitFileImpl(bool isDebug);
    static void InitFileNormal();
    static void InitFileDebug();
    static void InitFileMaxed();

    static void LoadRandomizerVersion1();
    static void LoadRandomizerVersion2();
    static void SaveRandomizer(SaveContext* saveContext, int sectionID, bool fullSave);

    static void LoadBaseVersion1();
    static void LoadBaseVersion2();
    static void LoadBaseVersion3();
    static void LoadBaseVersion4();
    static void SaveBase(SaveContext* saveContext, int sectionID, bool fullSave);

    std::vector<InitFunc> initFuncs;

    using SectionLoadHandler = std::map<int, LoadFunc>;
    std::map<std::string, SectionLoadHandler> sectionLoadHandlers;

    int sectionIndex = SECTION_ID_MAX;
    std::map<std::string, int> coreSectionIDsByName;
    std::map<int, SaveFuncInfo> sectionSaveHandlers;
    std::map<std::string, int> sectionRegistry;

    std::map<std::string, PostFunc> postHandlers;

    nlohmann::json* currentJsonContext = nullptr;
    nlohmann::json::iterator currentJsonArrayContext;
    std::shared_ptr<BS::thread_pool> smThreadPool;
};

#else

// TODO feature parity to the C++ interface. We need Save_AddInitFunction and Save_AddPostFunction at least

typedef void (*Save_LoadFunc)(void);
typedef void (*Save_SaveFunc)(const SaveContext* saveContext, int sectionID);

void Save_Init(void);
void Save_InitFile(int isDebug);
void Save_SaveFile(void);
void Save_SaveSection(int sectionID);
void Save_SaveGlobal(void);
void Save_LoadGlobal(void);
void Save_AddLoadFunction(char* name, int version, Save_LoadFunc func);
void Save_AddSaveFunction(char* name, int version, Save_SaveFunc func, bool saveWithBase, int parentSection);
SaveFileMetaInfo* Save_GetSaveMetaInfo(int fileNum);
void Save_CopyFile(int from, int to);
void Save_DeleteFile(int fileNum);
bool Save_Exist(int fileNum);
#endif
