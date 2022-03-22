#include "AnimationFactory.h"

namespace Ship
{
    Animation* AnimationFactory::ReadAnimation(BinaryReader* reader)
    {
        Animation* anim = new Animation();
        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            AnimationV0 Anim = AnimationV0();
            Anim.ParseFileBinary(reader, anim);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return anim;
    }
};