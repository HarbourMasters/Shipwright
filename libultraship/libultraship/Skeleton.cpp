#include "Skeleton.h"

namespace Ship
{
    void SkeletonV0::ParseFileBinary(BinaryReader* reader, Resource* res)
    {
        Skeleton* skel = (Skeleton*)res;
        
        ResourceFile::ParseFileBinary(reader, skel);

        skel->type = (SkeletonType)reader->ReadInt8();
        skel->limbType = (LimbType)reader->ReadInt8();

        skel->limbCount = reader->ReadUInt32();
        skel->dListCount = reader->ReadUInt32();

        skel->limbTableType = (LimbType)reader->ReadInt8();

        uint32_t limbTblCnt = reader->ReadUInt32();
        skel->limbTable.reserve(limbTblCnt);

        for (uint32_t i = 0; i < limbTblCnt; i++)
        {
            std::string limbPath = reader->ReadString();

            skel->limbTable.push_back(limbPath);
        }
    }
}