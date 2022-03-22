#include "../Texture.h"
#include "Utils/BinaryReader.h"

namespace Ship
{
    class TextureFactory
    {
    public:
        static Texture* ReadTexture(BinaryReader* reader);
    };
}