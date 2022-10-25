#pragma once

#include <stdint.h>
#include "BinaryReader.h"
#include "Utils/BinaryWriter.h"
#include "File.h"
#include "Lib/tinyxml2/tinyxml2.h"
#include "spdlog/spdlog.h"

namespace Ship {
    class ResourceMgr;

    enum class ResourceType {
        Archive          =   0x4F415243,     // OARC (UNUSED)
        Model            =   0x4F4D444C,     // OMDL (WIP)
        Texture          =   0x4F544558,     // OTEX
        Material         =   0x4F4D4154,     // OMAT (WIP)
        Animation        =   0x4F414E4D,     // OANM
        PlayerAnimation  =   0x4F50414D,     // OPAM
        DisplayList      =   0x4F444C54,     // ODLT
        Room             =   0x4F524F4D,     // OROM
        CollisionHeader  =   0x4F434F4C,     // OCOL
        Skeleton         =   0x4F534B4C,     // OSKL
        SkeletonLimb     =   0x4F534C42,     // OSLB
        Matrix           =   0x4F4D5458,     // OMTX
        Path             =   0x4F505448,     // OPTH
        Vertex           =   0x4F565458,     // OVTX
        Cutscene         =   0x4F435654,     // OCUT
        Array            =   0x4F415252,     // OARR
        Text             =   0x4F545854,     // OTXT
        Blob             =   0x4F424C42,     // OBLB
        Audio            =   0x4F415544,     // OAUD
        AudioSample      =   0x4F534D50,     // OSMP
        AudioSoundFont   =   0x4F534654,     // OSFT
        AudioSequence    =   0x4F534551,     // OSEQ
    };

    enum class DataType {
        U8 = 0,
        S8 = 1,
        U16 = 2,
        S16 = 3,
        U32 = 4,
        S32 = 5,
        U64 = 6,
        S64 = 7,
        F16 = 8,
        F32 = 9,
        F64 = 10
    };

    enum class Version {
        // BR
        Deckard     = 0,
        Roy         = 1,
        Rachael     = 2,
        Zhora       = 3,
        // ...
    };

    struct Patch {
        uint64_t crc;
        uint32_t index;
        uintptr_t origData;
    };

    class Resource {
    public:
        ResourceMgr* resMgr;
        uint64_t id; // Unique Resource ID
        ResourceType resType;
        bool isDirty = false;
        void* cachedGameAsset = 0; // Conversion to OoT friendly struct cached...
        std::shared_ptr<File> file;
        std::vector<Patch> patches;
        virtual ~Resource();
    };

    class ResourceFile {
    public:
        Endianness endianness;    // 0x00 - Endianness of the file
        uint32_t resourceType;  // 0x01 - 4 byte MAGIC
        Version version;     // 0x05 - Based on Ship release numbers
        uint64_t id;            // 0x09 - Unique Resource ID
        uint32_t resourceVersion; // 0x11 - Resource Minor Version Number

        virtual void ParseFileBinary(BinaryReader* reader, Resource* res);
        virtual void ParseFileXML(tinyxml2::XMLElement* reader, Resource* res);
        virtual void WriteFileBinary(BinaryWriter* writer, Resource* res);
        virtual void WriteFileXML(tinyxml2::XMLElement* writer, Resource* res);
    };

    class ResourcePromise {
    public:
        std::shared_ptr<Resource> resource;
        std::shared_ptr<File> file;
        std::condition_variable resourceLoadNotifier;
        std::mutex resourceLoadMutex;
        bool bHasResourceLoaded = false;
    };
}