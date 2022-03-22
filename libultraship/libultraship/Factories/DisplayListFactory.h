#include "../DisplayList.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class DisplayListFactory
    {
    public:
        static DisplayList* ReadDisplayList(BinaryReader* reader);
    };
}