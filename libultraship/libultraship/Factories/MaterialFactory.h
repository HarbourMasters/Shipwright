#include "../Material.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class MaterialFactory
    {
    public:
        static Material* ReadMaterial(BinaryReader* reader);
    };
}