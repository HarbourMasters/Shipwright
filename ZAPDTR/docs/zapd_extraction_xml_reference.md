# ZAPD extraction XML reference

This document aims to be a small reference of how to create a compatible xml file for ZAPD.

## Table of contents

- [ZAPD extraction XML reference](#zapd-extraction-xml-reference)
  - [Table of contents](#table-of-contents)
  - [Basic XML](#basic-xml)
  - [Resources types](#resources-types)
    - [File](#file)
    - [Texture](#texture)
    - [Background](#background)
    - [Blob](#blob)
    - [DList](#dlist)
    - [TextureAnimation](#textureanimation)
    - [Scene and Room](#scene-and-room)
    - [AltHeader](#altheader)
    - [Animation](#animation)
    - [PlayerAnimation](#playeranimation)
    - [CurveAnimation](#curveanimation)
    - [LegacyAnimation](#legacyanimation)
    - [Skeleton](#skeleton)
    - [LimbTable](#limbtable)
    - [Limb](#limb)
    - [Symbol](#symbol)
    - [Collision](#collision)
    - [Scalar](#scalar)
    - [Vector](#vector)
    - [Vtx](#vtx)
    - [Mtx](#mtx)
    - [Cutscene](#cutscene)
    - [Array](#array)
    - [Path](#path)
    - [PlayerAnimationData](#playeranimationdata)

## Basic XML

An example of an object xml:

```xml
<Root>
    <File Name="object_jj" Segment="6">

        <Animation Name="gJabuJabuAnim" Offset="0x1F4C"/>

        <Skeleton Name="gJabuJabuSkel" Type="Flex" LimbType="Standard" Offset="0xB9A8"/>

        <!-- Jabu Jabu eye textures -->
        <Texture Name="gJabuJabuEyeOpenTex" OutName="jabu_jabu_eye_open" Format="rgba16" Width="16" Height="32" Offset="0x7698"/>
        <Texture Name="gJabuJabuEyeHalfTex" OutName="jabu_jabu_eye_half" Format="rgba16" Width="16" Height="32" Offset="0x7A98"/>
        <Texture Name="gJabuJabuEyeClosedTex" OutName="jabu_jabu_eye_closed" Format="rgba16" Width="16" Height="32" Offset="0x7E98"/>


        <Collision Name="gJabuJabu1Col" Offset="0x0A1C"/>
        <Collision Name="gJabuJabu2Col" Offset="0x1830"/>
        <Collision Name="gJabuJabu3Col" Offset="0xBA8C"/>

    </File>
</Root>
```

Every xml must have a `<Root>` tag. It must have at least one `<File>` child.

## Resources types

The following is a list of the resources/tags supported by ZAPD, and the attributes needed by each one.

For most resources inside a `<File>` tag **you should also set an `Offset` attribute**. This is the offset (within the file) of the resource you are exporting. The `Offset` attribute is expected to be in hexadecimal, for example `Offset="0x41F0"`.

It's worth noting that every tag expects a `Name="gNameOfTheAsset"`. This is will be the name of the extracted variable in the output C code. Every asset must be prefixed with `g` and the suffix should represent the type of the variable.

Every tag can accept a `Static` attribute to specify if the asset should be marked as `static` or not.
There are 3 valid values (defaults to `Global`):

- `Global`: Mark static if the flag `--static` was used.
- `On`: Override the global config and **always mark** as `static`.
- `Off`: Override the global config and **don't mark** as `static`.

This table summarizes if the asset will be marked `static` (✅) or not (❌)
| `Static=""` attribute in XML | Without `--static` flag | With `--static` flag |
| ---------------------------- | ----------------------- | -------------------- |
| `On`                         | ✅                       | ✅                    |
| `Global` (default)           | ❌                       | ✅                    |
| `Off`                        | ❌                       | ❌                    |

-------------------------

### File

- Example of this tag:

```xml
<File Name="object_gi_fire" Segment="6">
```

- Attributes:

  - `Name`: Required. The name of the file in `baserom/` which will be extracted.
  - `OutName`: Optional. The output name of the generated C source file. Defaults to the value passed to `Name`.
  - `Segment`: Optional. This is the segment number of the current file. Expects a decimal number between 0 and 15 inclusive, usually 6 if it is an object. If not specified, the file will use VRAM instead of segmented addresses.
  - `BaseAddress`: Optional. RAM address of the file. Expects a hex number (with `0x` prefix). Default value: `0`.
  - `RangeStart`: Optional. File offset where the extraction will begin. Hex. Default value: `0x000000000`.
  - `RangeEnd`: Optional. File offset where the extraction will end. Hex. Default value: `0xFFFFFFFF`.
  - `Game`: Optional. Valid values: `OOT`, `MM`, `SW97` and `OOTSW97`. Default value: `OOT`.

-------------------------

### ExternalFile

Allows ZAPD to map segmented addresses to variables declared in other files by using its XML.

It is useful for objects that use variables from `gameplay_keep`, `gameplay_dangeon_keep`, `gameplay_field_keep`, etc.

This tag can be used in the global `config.xml` file.

- Example of this tag:

```xml
<ExternalFile XmlPath="objects/gameplay_keep.xml" OutPath="objects/gameplay_keep/"/>
```

- Attributes:

  - `XmlPath`: Required. The path of the XML, relative to the value set by `ExternalXMLFolder` in the configuration file.
  - `OutPath`: Required. The path were the header for the corresponding external file is. It is used to `#include` it in the generated `.c` file.

-------------------------

### Texture

Textures are extracted as `.png` files.

- Example:

```xml
<Texture Name="gCraterSmokeConeTex" OutName="crater_smoke_cone" Format="ia8" Width="32" Height="32" Offset="0xC30"/>
```

Will be defined as:

```c
u64 gCraterSmokeConeTex[] = {
#include "assets/objects/object_spot17_obj/crater_smoke_cone.ia8.inc.c"
};
```

- Attributes:

  - `Name`: Required. Suxffixed by `Tex`, unless it is a palette, in that case it is suffixed by `TLUT`.
  - `OutName`: Required. The filename of the extracted `.png` file.
  - `Format`: Required. The format of the image. Valid values: `rgba32`, `rgba16`, `i4`, `i8`, `ia4`, `ia8`, `ia16`, `ci4` and `ci8`.
  - `Width`: Required. Width in pixels of the image.
  - `Height`: Required. Height in pixels of the image.
  - `TlutOffset`: Optional. Specifies the tlut's offset used by this texture. This attribute is only valid if `Format` is either `ci4` or `ci8`, otherwise an exception would be thrown.

The following is a list of the texture formats the Nintendo 64 supports, with their gfxdis names and ZAPD format names.

| Format name                                     | Typing in `gsDPLoadTextureBlock` | "Format" in xml |
| ----------------------------------------------- | -------------------------------- | --------------- |
| 4-bit intensity (I)                             | `G_IM_FMT_I, G_IM_SIZ_4b`        | `i4`            |
| 4-bit intensity with alpha (I/A) (3/1)          | `G_IM_FMT_IA, G_IM_SIZ_4b`       | `ia4`           |
| 4-bit color index (CI)                          | `G_IM_FMT_CI, G_IM_SIZ_4b`       | `ci4`           |
| 8-bit I                                         | `G_IM_FMT_I, G_IM_SIZ_8b`        | `i8`            |
| 8-bit IA (4/4)                                  | `G_IM_FMT_IA, G_IM_SIZ_8b`       | `ia8`           |
| 8-bit CI                                        | `G_IM_FMT_CI, G_IM_SIZ_8b`       | `ci8`           |
| 16-bit red, green, blue, alpha (RGBA) (5/5/5/1) | `G_IM_FMT_RGBA, G_IM_SIZ_16b`    | `rgba16`        |
| 16-bit IA (8/8)                                 | `G_IM_FMT_IA, G_IM_SIZ_16b`      | `ia16`          |
| 16-bit YUV (Luminance, Blue-Y, Red-Y)           | `G_IM_FMT_YUV, G_IM_SIZ_16b`     | (not used)      |
| 32-bit RGBA (8/8/8/8)                           | `G_IM_FMT_RGBA, G_IM_SIZ_32b`    | `rgba32`        |

If you want to know more about this formats, you can check [`gsDPLoadTextureBlock`](http://n64devkit.square7.ch/n64man/gdp/gDPLoadTextureBlock.htm) for most formats, or [`gDPLoadTextureBlock_4b`](http://n64devkit.square7.ch/n64man/gdp/gDPLoadTextureBlock_4b.htm) for the 4-bit formats.

-------------------------

### Background

- Example:

```xml
<Background Name="gGoronShopBackground" OutName="goron_shop_background" Offset="0x9D0"/>
```

- Attributes:

  - `Name`: Required. Suxffixed by `Background`.
  - `OutName`: Required. The filename of the extracted `.jpg` file.

※ Explicit use of this tag isn't often necesary because it would probably be extracted automatically by another extracted element. You can use this to name them if you don't like the autogenerated name.

-------------------------

### Blob

Blob are binary data that will be extracted as `.bin` files.

- Example:

```xml
<Blob Name="gFireTempleBlob_00CCC0" Size="0x10E" Offset="0xCCC0"/>
```

Will be defined as:

```c

u8 gFireTempleBlob_00CCC0[] = {
#include "assets/objects/object_hidan_objects/gFireTempleBlob_00CCC0.bin.inc.c"
};
```

- Attributes:

  - `Name`: Required. Suxffixed by `Blob`.
  - `Size`: Required. Amount of bytes to extract. Hex.

※ We usually use blobs when we can't figure out the content's type of chunk of data.

-------------------------

### DList

A.k.a. Display list, or Gfx.

- Example:

```xml
<DList Name="gGiGreenRupeeInnerColorDL" Offset="0x04A0"/>
```

- Attributes:

  - `Name`: Required. Suxffixed by `DL`.

-------------------------

### TextureAnimation

A data type exclusive to Majora's Mask, that has scrolling, color changing, and texture changing capabilities. Declaring the main array will generate everything else; textures for the TextureCycle type must be declared manually in the XML to use symbols. (If it does reference any undeclared textures, ZAPD will warn and give the their offsets.)

```xml
<TextureAnimation Name="gRosaSistersTexAnim" Offset="0xD768"/>
```

- Attributes:

  - `Name`: Required. Suxffixed by `TexAnim`.

-------------------------

### Scene and Room

`Scene`s and `Room`s are a bit special, because `Room`s usually needs assets declared in their respective `Scene` (which is in a different file), so they need to be extracted together.

To accomplish this, the scene and each of their rooms must be declared in the same XML.

- Example:

```xml
<Root>
    <File Name="spot12_scene" Segment="2">
        <Cutscene Name="gSpot12Cs_006490" Offset="0x6490"/>

        <Scene Name="spot12_scene" Offset="0x0"/>
    </File>
    <File Name="spot12_room_0" Segment="3">
        <Room Name="spot12_room_0" Offset="0x0"/>
    </File>
    <File Name="spot12_room_1" Segment="3">
        <Room Name="spot12_room_1" Offset="0x0"/>
    </File>
</Root>

```

- Attributes:

  - `HackMode`: Optional. This is a simple non-hardcoded way to handle some edge cases. Valid values: `syotes_room`.

-------------------------

### AltHeader

Like `Scene`s and `Room`s, `AltHeader`s is special too. It should always be declared in the same `File` as a `Scene` or a `Room`.

- Example:

```xml
<File Name="spot01_scene" Segment="2">
    <Path Name="gSpot01Path_0003D0" NumPaths="3" Offset="0x3D0"/>

    <Texture Name="gSpot01TLUT_00A870" OutName="spot01_tlut_A870" Format="rgb5a1" Width="220" Height="1" Offset="0xA870"/>
    <Texture Name="gSpot01GrassTex" OutName="spot01_grass" Format="rgb5a1" Width="32" Height="32" Offset="0xAA50"/>

    <Texture Name="gSpot01WindowDayTex" OutName="spot01_window_day" Format="rgb5a1" Width="32" Height="64" Offset="0x15b50"/>
    <Texture Name="gSpot01WindowNightTex" OutName="spot01_window_night" Format="rgb5a1" Width="32" Height="64" Offset="0x16b50"/>

    <Cutscene Name="gKakarikoFirstTimeCs" Offset="0xA540"/>

    <Scene Name="spot01_scene" Offset="0x0"/>
    <AltHeader Name="gKakarikoAltHeader_009980" Offset="0x9980"/>
</File>
```

- Attributes:

  - `Name`: Required. Suxffixed by `AltHeader`.

-------------------------

### Animation

- Example:

```xml
<Animation Name="gWallmasterDamageAnim" Offset="0x590"/>
```

- Attributes:

  - `Name`: Required. Suxffixed by `Anim`.

-------------------------

### PlayerAnimation

- Example:

```xml
<PlayerAnimation Name="gPlayer3Anim" Offset="0x2310"/>
```

- Attributes:

  - `Name`: Required. Suxffixed by `Anim`.

-------------------------

### CurveAnimation

- Example:

```xml
<CurveAnimation Name="gEnBoxCurveAnim_4B60" SkelOffset="0x5EB8" Offset="0x4B60"/>
```

- Attributes:

  - `Name`: Required. Suxffixed by `Anim`.
  - `SkelOffset`: Required. Offset of the `CurveSkeleton` (I.e. a [`Skeleton`](#skeleton) resource with `Type="Curve"`) related to this animation.

-------------------------

### LegacyAnimation

Useful only for the unused `object_human`'s animation data.

- Example:

```xml
<LegacyAnimation Name="gHumanAnim_011A9C" Offset="0x11A9C"/>
```

- Attributes:

  - `Name`: Required. Suxffixed by `Anim`.

-------------------------

### Skeleton

- Example:

```xml
<Skeleton Name="gEnBoxCurveSkel" Type="Curve" LimbType="Curve" Offset="0x5EB8"/>
```

- Attributes:

  - `Name`: Required. Suxffixed by `Skel`.
  - `Type`: Required. Valid values: `Normal`, `Flex` and `Curve`.
  - `LimbType`: Required. Valid values: `Standard`, `LOD`, `Skin`, `Curve` and `Legacy`.

※ There are no restrictions in the `Type` and `LimbType` attributes besides the valid values, so any skeleton type can be combined with any limb type.

-------------------------

### LimbTable

- Example:

```xml
<LimbTable Name="gHumanLimbTable_011FC8" LimbType="Legacy" Count="41" Offset="0x11FC8"/>
```

- Attributes:

  - `Name`: Required. Suxffixed by `Skel`.
  - `LimbType`: Required. Valid values: `Standard`, `LOD`, `Skin`, `Curve` and `Legacy`.
  - `Count`: Required. Amount of limbs. Integer.

-------------------------

### Limb

- Example:

```xml
<Limb Name="gChuGirlHeadLimb" LimbType="Standard" Offset="0x6E34"/>
```

- Attributes:

  - `Name`: Required. Suxffixed by `Limb`.
  - `LimbType`: Required. Valid values: `Standard`, `LOD`, `Skin`, `Curve` and `Legacy`.

-------------------------

### Symbol

A special element that allows declaring a variable without actually extracting it from the current file. Useful when a resource references an element from another file. The symbol will be declared as `extern`.

- Example:

```xml
<Symbol Name="gJsjutanShadowTex" Type="u8" Size="1" Count="0x800" Offset="0x4E70"/>
```

Will be declared as:

```c
extern u8 gJsjutanShadowTex[2048];
```

- Attributes:

  - `Type`: The type of the declared variable. If missing, it will default to `void*`.
  - `TypeSize`: The size in bytes of the type. If missing, it will default to `4` (the size of a word and a pointer). Integer or hex value.
  - `Count`: Optional. If it is present, the variable will be declared as an array instead of a plain variable. The value of this attribute specifies the length of the array. If `Count` is present but it has no value (`Count=""`), then the length of the array will not be specified either in the declared variable. Integer or hex value.
  - `Static`: This attribute can't be enabled on a Symbol node. A warning will be showed in this case.

-------------------------

### Collision

- Example:

```xml
<Collision Name="gDesertColossusBombableWallCol" Offset="0x1A58"/>
```

- Attributes:

  - `Name`: Required. Suxffixed by `Col`.

-------------------------

### Scalar

Allows the extraction of a single number.

- Example:

```xml
<Scalar Type="u64" Name="pad34F8"/>
```

Will be extracted as:

```c
u64 pad34F8 = { 0 }; 
```

- Attributes:

  - `Name`: Required. Suxffixed by ~~`TBD`~~.
  - `Type`: Required. Valid values: `s8`, `u8`, `x8`, `s16`, `u16`, `x16`, `s32`, `u32`, `x32`, `s64`, `u64`, `x64`, `f32` and `f64`.

※ Can be wrapped in an [`Array`](#array) tag.

-------------------------

### Vector

Extracts a vector.

Current supported types are `Vec3s`, `Vec3i` or `Vec3f`.

- Example:

```xml
<Array Name="D_04002040" Count="24" Offset="0x2040">
    <Vector Type="s16" Dimensions="3" />
</Array>
```

Will be extracted as:

```c
Vec3s D_04002040[24] = {
    { -37, 2346, 93 },
    { 0, 11995, 0 },
    { -16385, -305, -16333 },
    { 0, 51, 12 },
    { 3761, 2263, -384 },
    { 0, 0, 3786 },
    { 1594, 1384, -18344 },
    { -2288, -2428, -1562 },
    { 0, 0, 3219 },
    { -2148, -5, -16840 },
    { 15365, -1708, 15611 },
    { 1761, 8365, 17711 },
    { 0, 0, 18859 },
    { 0, 0, 0 },
    { -9392, -9579, 28686 },
    { 0, 0, -7093 },
    { -2748, 685, -14092 },
    { 213, 6553, -32212 },
    { 0, 0, -1877 },
    { 3267, 3309, -16090 },
    { -18101, 25946, -2670 },
    { -104, 0, 0 },
    { 0, 0, 0 },
    { 0, 0, 0 }
}; 
```

- Attributes:

  - `Name`: Required. Suxffixed by ~~`TBD`~~.
  - `Type`: Required. Specifies the vector's type (`Vec3s`, `Vec3i` and `Vec3f`). Valid values: `s16`, `s32` and `f32`.
  - `Dimensions`: Required. The amount of dimensions of the vector. Valid values: `3`.

※ Can be wrapped in an [`Array`](#array) tag.

-------------------------

### Vtx

- Example:

```xml
<Array Name="gTriforceVtx" Count="32" Offset="0x0000">
    <Vtx/>
</Array>
```

- Attributes:

  - `Name`: Required. Suxffixed by `Vtx`.

※ Can be wrapped in an [`Array`](#array) tag.

-------------------------

### Mtx

- Example:

```xml
<Mtx Name="gBdanMtx_000C40" Offset="0xC40" />
```

- Attributes:

  - `Name`: Required. Suxffixed by `Mtx`.

※ Explicit use of this tag isn't often necesary because it would probably be extracted automatically by another extracted element.

-------------------------

### Cutscene

- Example:

```xml
<Cutscene TODO/> <!-- Currently there are 0 xml in the repo using this tag. -->
```

- Attributes:

  - `Name`: Required. Suxffixed by `Cs`.

※ Explicit use of this tag isn't often necesary because it would probably be extracted automatically by another extracted element.

-------------------------

### Array

The `Array` element is special, because it needs an inner element to work. It will declare an array of its inner element.

Currently, only [`Scalar`](#scalar), [`Vector`](#vector) and [`Vtx`](#vtx) support being wrapped in an array.

- Example:

```xml
<Array Name="gTrialBarrierEnergyVtx" Count="102" Offset="0x4FD0">
    <Vtx/>
</Array>
```

- Attributes:

  - `Name`: Required. How the variable will be named. By our convention it should be prefixed by `g`. The sufix is mandated by the element contained.
  - `Count`: Required. Amount of elements. Integer.

-------------------------

### Path

- Example:

```xml
<Path Name="gSpot09Path_002F60" Offset="0x2F60"/>
```

- Attributes:

  - `Name`: Required. Suxffixed by `Path`.
  - `NumPaths`: Optional. The amount of paths contained in the array. It must be a positive integer.

-------------------------

### PlayerAnimationData

Allows the extraction of the specific data of the player animations which are found in the `link_animetion` file.

- Example:

```xml
<PlayerAnimationData Name="gPlayerAnimData_000000" FrameCount="20" Offset="0x0"/>
```

- Attributes:

  - `Name`: Required. Suxffixed by `AnimData`.
  - `FrameCount`: Required. The length of the animation in frames. It must be a positive integer.

-------------------------
