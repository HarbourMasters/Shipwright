#pragma once

#include "Resource.h"
#include "ResourceFactoryBinary.h"

class ResourceFactoryBinaryAudioSoundFontV0 : public LUS::ResourceFactoryBinary {
  public:
    std::shared_ptr<LUS::IResource> ReadResource(std::shared_ptr<LUS::File> file) override;
};
