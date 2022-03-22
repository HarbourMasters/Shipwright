#include "../CollisionHeader.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class CollisionHeaderFactory
    {
    public:
        static CollisionHeader* ReadCollisionHeader(BinaryReader* reader);
    };
}