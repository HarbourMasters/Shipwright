#include "soh/resource/importer/AnimationFactory.h"
#include "soh/resource/type/Animation.h"
#include "spdlog/spdlog.h"

namespace SOH {
std::shared_ptr<LUS::IResource> ResourceFactoryBinaryAnimationV0::ReadResource(std::shared_ptr<LUS::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto animation = std::make_shared<Animation>(file->InitData);
    auto reader = std::get<std::shared_ptr<LUS::BinaryReader>>(file->Reader);

    AnimationType animType = (AnimationType)reader->ReadUInt32();
    animation->type = animType;

    if (animType == AnimationType::Normal) {
        // Set frame count
        animation->animationData.animationHeader.common.frameCount = reader->ReadInt16();

        // Populate frame data
        uint32_t rotValuesCnt = reader->ReadUInt32();
        animation->rotationValues.reserve(rotValuesCnt);
        for (uint32_t i = 0; i < rotValuesCnt; i++) {
            animation->rotationValues.push_back(reader->ReadUInt16());
        }
        animation->animationData.animationHeader.frameData = (int16_t*)animation->rotationValues.data();

        // Populate joint indices
        uint32_t rotIndCnt = reader->ReadUInt32();
        animation->rotationIndices.reserve(rotIndCnt);
        for (size_t i = 0; i < rotIndCnt; i++) {
            uint16_t x = reader->ReadUInt16();
            uint16_t y = reader->ReadUInt16();
            uint16_t z = reader->ReadUInt16();
            animation->rotationIndices.push_back(RotationIndex(x, y, z));
        }
        animation->animationData.animationHeader.jointIndices = (JointIndex*)animation->rotationIndices.data();

        // Set static index max
        animation->animationData.animationHeader.staticIndexMax = reader->ReadInt16();
    } else if (animType == AnimationType::Curve) {
        // Read frame count (unused in this animation type)
        reader->ReadInt16();

        // Set refIndex
        uint32_t refArrCnt = reader->ReadUInt32();
        animation->refIndexArr.reserve(refArrCnt);
        for (uint32_t i = 0; i < refArrCnt; i++) {
            animation->refIndexArr.push_back(reader->ReadUByte());
        }
        animation->animationData.transformUpdateIndex.refIndex = animation->refIndexArr.data();

        // Populate transform data
        uint32_t transformDataCnt = reader->ReadUInt32();
        animation->transformDataArr.reserve(transformDataCnt);
        for (uint32_t i = 0; i < transformDataCnt; i++) {
            TransformData data;
            data.unk_00 = reader->ReadUInt16();
            data.unk_02 = reader->ReadInt16();
            data.unk_04 = reader->ReadInt16();
            data.unk_06 = reader->ReadInt16();
            data.unk_08 = reader->ReadFloat();

            animation->transformDataArr.push_back(data);
        }
        animation->animationData.transformUpdateIndex.transformData = animation->transformDataArr.data();

        // Populate copy values
        uint32_t copyValuesCnt = reader->ReadUInt32();
        animation->copyValuesArr.reserve(copyValuesCnt);
        for (uint32_t i = 0; i < copyValuesCnt; i++) {
            animation->copyValuesArr.push_back(reader->ReadInt16());
        }
        animation->animationData.transformUpdateIndex.copyValues = animation->copyValuesArr.data();
    } else if (animType == AnimationType::Link) {
        // Read the frame count
        animation->animationData.linkAnimationHeader.common.frameCount = reader->ReadInt16();

        // Read the segment pointer (always 32 bit, doesn't adjust for system pointer size)
        animation->animationData.linkAnimationHeader.segment = (void*)reader->ReadUInt32();
    } else if (animType == AnimationType::Legacy) {
        SPDLOG_DEBUG("BEYTAH ANIMATION?!");
    }

    return animation;
}
} // namespace SOH