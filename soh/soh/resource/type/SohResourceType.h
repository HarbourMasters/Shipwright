#pragma once

namespace SOH {
enum class ResourceType {
    SOH_Array = 0x4F415252,           // OARR
    SOH_Animation = 0x4F414E4D,       // OANM
    SOH_PlayerAnimation = 0x4F50414D, // OPAM
    SOH_Room = 0x4F524F4D,            // OROM
    SOH_CollisionHeader = 0x4F434F4C, // OCOL
    SOH_Skeleton = 0x4F534B4C,        // OSKL
    SOH_SkeletonLimb = 0x4F534C42,    // OSLB
    SOH_Path = 0x4F505448,            // OPTH
    SOH_Cutscene = 0x4F435654,        // OCUT
    SOH_Text = 0x4F545854,            // OTXT
    SOH_Audio = 0x4F415544,           // OAUD
    SOH_AudioSample = 0x4F534D50,     // OSMP
    SOH_AudioSoundFont = 0x4F534654,  // OSFT
    SOH_AudioSequence = 0x4F534551,   // OSEQ
    SOH_Background = 0x4F424749,      // OBGI
    SOH_SceneCommand = 0x4F52434D,    // ORCM
};
} // namespace SOH
