#include "2s2h/resource/importer/KeyFrameFactory.h"
#include "2s2h/resource/type/KeyFrame.h"
#include <ship/Context.h>
#include <ship/resource/ResourceManager.h>

namespace SOH {
std::shared_ptr<Ship::IResource>
ResourceFactoryBinaryKeyFrameSkel::ReadResource(std::shared_ptr<Ship::File> file,
                                                std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto skel = std::make_shared<KeyFrameSkel>(initData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    skel->skelData.limbCount = reader->ReadUByte();
    skel->skelData.dListCount = reader->ReadUByte();
    KeyframeSkelType skelType = (KeyframeSkelType)reader->ReadUByte();
    uint8_t numLimbs = reader->ReadUByte();

    if (skelType == KeyframeSkelType::Normal) {
        KeyFrameStandardLimb* limbs = new KeyFrameStandardLimb[numLimbs];

        for (uint32_t i = 0; i < numLimbs; i++) {
            std::string dlStr = reader->ReadString();
            auto dl = Ship::Context::GetRawInstance()->GetResourceManager()->LoadResourceProcess(dlStr.c_str());
            limbs[i].dList = nullptr;
            if (dl != nullptr) {
                limbs[i].dList = dl->GetRawPointer();
            }
            limbs[i].numChildren = reader->ReadUByte();
            limbs[i].flags = reader->ReadUByte();
            limbs[i].translation.x = reader->ReadInt16();
            limbs[i].translation.y = reader->ReadInt16();
            limbs[i].translation.z = reader->ReadInt16();
        }
        skel->skelData.limbsStandard = limbs;
    } else {
        KeyFrameFlexLimb* limbs = new KeyFrameFlexLimb[numLimbs];

        for (uint32_t i = 0; i < numLimbs; i++) {
            std::string dlStr = reader->ReadString();
            auto dl = Ship::Context::GetRawInstance()->GetResourceManager()->LoadResourceProcess(dlStr.c_str());
            limbs[i].dList = nullptr;
            if (dl != nullptr) {
                limbs[i].dList = dl->GetRawPointer();
            }
            limbs[i].numChildren = reader->ReadUByte();
            limbs[i].flags = reader->ReadUByte();
            limbs[i].callbackIndex = reader->ReadUByte();
        }
        skel->skelData.limbsFlex = limbs;
    }

    return skel;
}

std::shared_ptr<Ship::IResource>
ResourceFactoryBinaryKeyFrameAnim::ReadResource(std::shared_ptr<Ship::File> file,
                                                std::shared_ptr<Ship::ResourceInitData> initData) {
    if (!FileHasValidFormatAndReader(file, initData)) {
        return nullptr;
    }

    auto anim = std::make_shared<KeyFrameAnim>(initData);
    auto reader = std::get<std::shared_ptr<Ship::BinaryReader>>(file->Reader);

    const KeyframeSkelType skelType = (KeyframeSkelType)reader->ReadUByte();
    const uint32_t numBitFlags = reader->ReadUInt32();

    if (skelType == KeyframeSkelType::Normal) {
        anim->animData.bitFlags = new u8[numBitFlags];
        for (uint32_t i = 0; i < numBitFlags; i++) {
            anim->animData.bitFlags[i] = reader->ReadUByte();
        }
    } else {
        anim->animData.bitFlagsFlex = new u16[numBitFlags];
        for (uint32_t i = 0; i < numBitFlags; i++) {
            anim->animData.bitFlagsFlex[i] = reader->ReadUInt16();
        }
    }

    uint32_t numKf = reader->ReadUInt32();
    anim->animData.keyFrames = new KeyFrame[numKf];

    for (uint32_t i = 0; i < numKf; i++) {
        anim->animData.keyFrames[i].frame = reader->ReadInt16();
        anim->animData.keyFrames[i].value = reader->ReadInt16();
        anim->animData.keyFrames[i].velocity = reader->ReadInt16();
    }

    uint32_t numKfNums = reader->ReadUInt32();
    anim->animData.kfNums = new s16[numKfNums];

    for (uint32_t i = 0; i < numKfNums; i++) {
        anim->animData.kfNums[i] = reader->ReadInt16();
    }

    uint32_t numPresetValues = reader->ReadUInt32();
    anim->animData.presetValues = new s16[numPresetValues];

    for (uint32_t i = 0; i < numPresetValues; i++) {
        anim->animData.presetValues[i] = reader->ReadInt16();
    }

    anim->animData.unk_10[0] = reader->ReadUByte();
    anim->animData.unk_10[0] = reader->ReadUByte();

    anim->animData.duration = reader->ReadUInt16();

    return anim;
}
} // namespace SOH
