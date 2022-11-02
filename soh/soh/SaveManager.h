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
    u32 requiresMasterQuest;
    u32 requiresOriginal;
    u8 seedHash[5];
    u8 randoSave;
} SaveFileMetaInfo;

#ifdef __cplusplus

#include <map>
#include <string>
#include <tuple>
#include <functional>
#include <vector>
#include <filesystem>

#include <nlohmann/json.hpp>

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
    bool SaveFile_Exist(int fileNum);

    // Adds a function that is called when we are intializing a save, including when we are loading a save.
    void AddInitFunction(InitFunc func);

    // Adds a function to handling loading a section
    void AddLoadFunction(const std::string& name, int version, LoadFunc func);

    // Adds a function that is called when saving. This should only be called once for each function, the version is filled in automatically.
    void AddSaveFunction(const std::string& name, int version, SaveFunc func);

    // Adds a function to be called after loading is complete. This is to handle any cleanup required from loading old versions.
    void AddPostFunction(const std::string& name, PostFunc func);

    void CopyZeldaFile(int from, int to);
    void DeleteZeldaFile(int fileNum);
    bool IsRandoFile();

    // Use a name of "" to save to an array. You must be in a SaveArray callback.
    template<typename T>
    void SaveData(const std::string& name, const T& data) {
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
    template<typename T> void LoadData(const std::string& name, T& data, const T& defaultValue = T{}) {
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

    void ConvertFromUnversioned();
    void CreateDefaultGlobal();

    void InitMeta(int slotNum);
    static void InitFileImpl(bool isDebug);
    static void InitFileNormal();
    static void InitFileDebug();

    static void LoadRandomizerVersion1();
    static void LoadRandomizerVersion2();
    static void SaveRandomizer();

    static void LoadBaseVersion1();
    static void LoadBaseVersion2();
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

// TODO feature parity to the C++ interface. We need Save_AddInitFunction and Save_AddPostFunction at least

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
bool Save_Exist(int fileNum);
#endif
