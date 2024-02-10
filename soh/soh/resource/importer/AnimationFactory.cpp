#include "soh/resource/importer/AnimationFactory.h"
#include "soh/resource/type/Animation.h"
#include "spdlog/spdlog.h"

std::shared_ptr<LUS::IResource> ResourceFactoryBinaryAnimationV0::ReadResource(std::shared_ptr<LUS::File> file) {
    if (!FileHasValidFormatAndReader(file)) {
        return nullptr;
    }

    auto animation = std::make_shared<Animation>(file->InitData);

    AnimationType animType = (AnimationType)file->Reader->ReadUInt32();
    animation->type = animType;

    if (animType == AnimationType::Normal) {
        // Set frame count
        animation->animationData.animationHeader.common.frameCount = file->Reader->ReadInt16();

        // Populate frame data
        uint32_t rotValuesCnt = file->Reader->ReadUInt32();
        animation->rotationValues.reserve(rotValuesCnt);
        for (uint32_t i = 0; i < rotValuesCnt; i++) {
            animation->rotationValues.push_back(file->Reader->ReadUInt16());
        }
        animation->animationData.animationHeader.frameData = (int16_t*)animation->rotationValues.data();

        // Populate joint indices
        uint32_t rotIndCnt = file->Reader->ReadUInt32();
        animation->rotationIndices.reserve(rotIndCnt);
        for (size_t i = 0; i < rotIndCnt; i++) {
            uint16_t x = file->Reader->ReadUInt16();
            uint16_t y = file->Reader->ReadUInt16();
            uint16_t z = file->Reader->ReadUInt16();
            animation->rotationIndices.push_back(RotationIndex(x, y, z));
        }
        animation->animationData.animationHeader.jointIndices = (JointIndex*)animation->rotationIndices.data();

        // Set static index max
        animation->animationData.animationHeader.staticIndexMax = file->Reader->ReadInt16();
    } else if (animType == AnimationType::Curve) {
        // Read frame count (unused in this animation type)
        file->Reader->ReadInt16();

        // Set refIndex
        uint32_t refArrCnt = file->Reader->ReadUInt32();
        animation->refIndexArr.reserve(refArrCnt);
        for (uint32_t i = 0; i < refArrCnt; i++) {
            animation->refIndexArr.push_back(file->Reader->ReadUByte());
        }
        animation->animationData.transformUpdateIndex.refIndex = animation->refIndexArr.data();

        // Populate transform data
        uint32_t transformDataCnt = file->Reader->ReadUInt32();
        animation->transformDataArr.reserve(transformDataCnt);
        for (uint32_t i = 0; i < transformDataCnt; i++) {
            TransformData data;
            data.unk_00 = file->Reader->ReadUInt16();
            data.unk_02 = file->Reader->ReadInt16();
            data.unk_04 = file->Reader->ReadInt16();
            data.unk_06 = file->Reader->ReadInt16();
            data.unk_08 = file->Reader->ReadFloat();

            animation->transformDataArr.push_back(data);
        }
        animation->animationData.transformUpdateIndex.transformData = animation->transformDataArr.data();

        // Populate copy values
        uint32_t copyValuesCnt = file->Reader->ReadUInt32();
        animation->copyValuesArr.reserve(copyValuesCnt);
        for (uint32_t i = 0; i < copyValuesCnt; i++) {
            animation->copyValuesArr.push_back(file->Reader->ReadInt16());
        }
        animation->animationData.transformUpdateIndex.copyValues = animation->copyValuesArr.data();
    } else if (animType == AnimationType::Link) {
        // Read the frame count
        animation->animationData.linkAnimationHeader.common.frameCount = file->Reader->ReadInt16();

        // Read the segment pointer (always 32 bit, doesn't adjust for system pointer size)
        animation->animationData.linkAnimationHeader.segment = (void*)file->Reader->ReadUInt32();
    } else if (animType == AnimationType::Legacy) {
        SPDLOG_DEBUG("BEYTAH ANIMATION?!");
    }

    return animation;
}
