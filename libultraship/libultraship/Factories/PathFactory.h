#include "../Path.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class PathFactory
    {
    public:
        static Path* ReadPath(BinaryReader* reader);
    };
}