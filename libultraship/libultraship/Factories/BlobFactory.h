#include "../Blob.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class BlobFactory
    {
    public:
        static Blob* ReadBlob(BinaryReader* reader);
    };
}