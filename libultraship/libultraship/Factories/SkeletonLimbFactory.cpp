#include "SkeletonLimbFactory.h"

namespace Ship
{
    SkeletonLimb* SkeletonLimbFactory::ReadSkeletonLimb(BinaryReader* reader, bool readFullHeader)
    {
        SkeletonLimb* limb = new SkeletonLimb();

        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            SkeletonLimbV0 Limb = SkeletonLimbV0();
            Limb.ParseFileBinary(reader, limb, readFullHeader);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return limb;
    }
}