#include "PlayerAnimationFactory.h"

namespace Ship
{
    PlayerAnimation* PlayerAnimationFactory::ReadPlayerAnimation(BinaryReader* reader)
    {
        PlayerAnimation* anim = new PlayerAnimation();

        Version version = (Version)reader->ReadUInt32();

        switch (version)
        {
        case Version::Deckard:
        {
            PlayerAnimationV0 Anim = PlayerAnimationV0();
            Anim.ParseFileBinary(reader, anim);
        }
        break;
        default:
            // VERSION NOT SUPPORTED
            break;
        }

        return anim;

    }
}