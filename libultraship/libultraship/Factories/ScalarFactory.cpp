#include "ArrayFactory.h"
#include "ScalarFactory.h"

namespace Ship
{
    Scalar* ScalarFactory::ReadScalar(BinaryReader* reader, bool readFullHeader)
    {
        Scalar* scal = new Scalar();
        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Rachael:
        {
            ScalarV2 scalObj = ScalarV2();
            scalObj.ParseFileBinary(reader, scal, readFullHeader);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return scal;
    }
};