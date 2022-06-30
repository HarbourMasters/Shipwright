#include "../Scalar.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class ScalarFactory
    {
    public:
        static Scalar* ReadScalar(BinaryReader* reader, bool readFullHeader);
    };
}