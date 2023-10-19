#pragma once

#ifndef Z64SCENE_ENUM_H
#define Z64SCENE_ENUM_H
#define DEFINE_SCENE(_0, _1, enum, _3, _4, _5) enum,

#ifdef __cplusplus
enum SceneID : int {
#else
enum SceneID {
#endif
#include "tables/scene_table.h"
    /* 0x6E */ SCENE_ID_MAX
};

#undef DEFINE_SCENE
#endif