#include "Resource.h"
#include "StrHash.h"

namespace Ship
{
    enum class MaterialCmt
    {
        Wrap = 0,
        Mirror = 1,
        Clamp = 2
    };

    class ShaderParam
    {
    public:
        strhash name;
        DataType dataType;
        uint64_t value;

        ShaderParam(BinaryReader* reader);
    };

    class MaterialV0 : public ResourceFile
    {
    public:
        // Typical N64 Stuff
        MaterialCmt cmtH, cmtV;
        uint8_t clrR, clrG, clrB, clrA, clrM, clrL;

        // Modern Stuff
        strhash shaderID;
        uint32_t shaderParamsCnt;
        uint32_t offsetToShaderEntries;

        void ParseFileBinary(BinaryReader* reader, Resource* res) override;
    };

    class Material : public Resource
    {
    public:
        // Typical N64 Stuff
        MaterialCmt cmtH, cmtV;
        uint8_t clrR, clrG, clrB, clrA, clrM, clrL;

        // Modern Stuff
        strhash shaderID;
        std::vector<ShaderParam*> shaderParams;
    };
}