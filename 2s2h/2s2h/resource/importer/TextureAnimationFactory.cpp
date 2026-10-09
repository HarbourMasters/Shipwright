#include "2s2h/resource/importer/TextureAnimationFactory.h"
#include "2s2h/resource/type/TextureAnimation.h"
#include "gbi.h"
#include <spdlog/spdlog.h>

namespace SOH {

std::shared_ptr<Ship::IResource>
ResourceFactoryBinaryTextureAnimationV0::ReadResource(std::shared_ptr<Ship::File> file,
                                                      std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto tAnim = std::make_shared<TextureAnimation>(initData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    const size_t numEntries = reader->ReadUInt32();

    // `e` is a void* and must be allocated and freed with malloc/free
    for (size_t i = 0; i < numEntries; i++) {
        AnimatedMaterial anim;
        anim.segment = reader->ReadInt8();
        anim.type = reader->ReadInt16();

        switch ((TextureAnimationParamsType)anim.type) {
            case TextureAnimationParamsType::SingleScroll: {
                auto* e = (AnimatedMatTexScrollParams*)malloc(sizeof(AnimatedMatTexScrollParams));
                if (e == nullptr) {
                    SPDLOG_CRITICAL("Failed to allocate memory for AnimatedMatTexScrollParams");
                    std::bad_alloc ex;
                    throw ex;
                }

                e->xStep = reader->ReadInt8();
                e->yStep = reader->ReadInt8();
                e->width = reader->ReadUByte();
                e->height = reader->ReadUByte();
                anim.params = e;
                break;
            }
            case TextureAnimationParamsType::DualScroll: {
                auto* e = (AnimatedMatTexScrollParams*)malloc(sizeof(AnimatedMatTexScrollParams) * 2);
                if (e == nullptr) {
                    SPDLOG_CRITICAL("Failed to allocate memory for AnimatedMatTexScrollParams");
                    std::bad_alloc ex;
                    throw ex;
                }

                e[0].xStep = reader->ReadInt8();
                e[0].yStep = reader->ReadInt8();
                e[0].width = reader->ReadUByte();
                e[0].height = reader->ReadUByte();
                e[1].xStep = reader->ReadInt8();
                e[1].yStep = reader->ReadInt8();
                e[1].width = reader->ReadUByte();
                e[1].height = reader->ReadUByte();
                anim.params = e;
                break;
            }
            case TextureAnimationParamsType::ColorChange:
            case TextureAnimationParamsType::ColorChangeLERP:
            case TextureAnimationParamsType::ColorChangeLagrange: {
                auto* e = (AnimatedMatColorParams*)malloc(sizeof(AnimatedMatColorParams));
                if (e == nullptr) {
                    SPDLOG_CRITICAL("Failed to allocate memory for AnimatedMatColorParams");
                    std::bad_alloc ex;
                    throw ex;
                }

                e->keyFrameLength = reader->ReadUInt16();
                e->keyFrameCount = reader->ReadUInt16();

                size_t keyFrameSize = reader->ReadUInt16();
                if (keyFrameSize != 0) {
                    e->keyFrames = new uint16_t[keyFrameSize];
                    for (size_t i = 0; i < keyFrameSize; i++) {
                        e->keyFrames[i] = reader->ReadUInt16();
                    }
                } else {
                    e->keyFrames = nullptr;
                }

                size_t primColorSize = reader->ReadUInt16();
                if (primColorSize != 0) {
                    e->primColors = new F3DPrimColor[primColorSize];
                    for (size_t i = 0; i < primColorSize; i++) {
                        e->primColors[i].r = reader->ReadUByte();
                        e->primColors[i].g = reader->ReadUByte();
                        e->primColors[i].b = reader->ReadUByte();
                        e->primColors[i].a = reader->ReadUByte();
                        e->primColors[i].lodFrac = reader->ReadUByte();
                    }
                } else {
                    e->primColors = nullptr;
                }

                size_t envColorSize = reader->ReadUInt16();
                if (envColorSize != 0) {
                    e->envColors = new F3DEnvColor[envColorSize];
                    for (size_t i = 0; i < envColorSize; i++) {
                        e->envColors[i].r = reader->ReadUByte();
                        e->envColors[i].g = reader->ReadUByte();
                        e->envColors[i].b = reader->ReadUByte();
                        e->envColors[i].a = reader->ReadUByte();
                    }
                } else {
                    e->envColors = nullptr;
                }

                anim.params = e;
                break;
            }
            case TextureAnimationParamsType::TextureCycle: {
                auto* e = (AnimatedMatTexCycleParams*)malloc(sizeof(AnimatedMatTexCycleParams));
                if (e == nullptr) {
                    SPDLOG_CRITICAL("Failed to allocate memory for AnimatedMatTexCycleParams");
                    std::bad_alloc ex;
                    throw ex;
                }
                e->keyFrameLength = reader->ReadUInt16();
                uint32_t textureListSize = reader->ReadUInt32();
                e->textureList = new void*[textureListSize];
                e->textureIndexList = new uint8_t[e->keyFrameLength];

                tAnim->textureCycleTextures.reserve(textureListSize);

                for (size_t i = 0; i < textureListSize; i++) {
                    tAnim->textureCycleTextures.emplace_back("__OTR__" + reader->ReadString());
                    e->textureList[i] = (Gfx*)tAnim->textureCycleTextures[i].c_str();
                }
                for (size_t i = 0; i < e->keyFrameLength; i++) {
                    e->textureIndexList[i] = reader->ReadUByte();
                }
                anim.params = e;
                break;
            }
            case TextureAnimationParamsType::Empty: {
                anim.params = nullptr;
                break;
            }
        }
        tAnim->anims.emplace_back(anim);
    }

    return tAnim;
}
} // namespace SOH
