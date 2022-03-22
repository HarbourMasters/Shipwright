#include "../Array.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class ArrayFactory
    {
    public:
        static Array* ReadArray(BinaryReader* reader);
    };
}