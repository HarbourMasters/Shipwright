static const gfxd_macro_type_t macro_tbl[] =
{
	[gfxd_Invalid] =
	{
		.prefix = NULL,
		.suffix = NULL,
		.opcode = -1,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_Invalid,
	},
	[gfxd_DPFillRectangle] =
	{
		.prefix = NULL,
		.suffix = "DPFillRectangle",
		.opcode = G_FILLRECT,
		.n_gfx = 1,
		.n_arg = 4,
		.disas_fn = d_DPFillRectangle,
	},
	[gfxd_DPFullSync] =
	{
		.prefix = NULL,
		.suffix = "DPFullSync",
		.opcode = G_RDPFULLSYNC,
		.n_gfx = 1,
		.n_arg = 0,
		.disas_fn = d_DPFullSync,
	},
	[gfxd_DPLoadSync] =
	{
		.prefix = NULL,
		.suffix = "DPLoadSync",
		.opcode = G_RDPLOADSYNC,
		.n_gfx = 1,
		.n_arg = 0,
		.disas_fn = d_DPLoadSync,
	},
	[gfxd_DPTileSync] =
	{
		.prefix = NULL,
		.suffix = "DPTileSync",
		.opcode = G_RDPTILESYNC,
		.n_gfx = 1,
		.n_arg = 0,
		.disas_fn = d_DPTileSync,
	},
	[gfxd_DPPipeSync] =
	{
		.prefix = NULL,
		.suffix = "DPPipeSync",
		.opcode = G_RDPPIPESYNC,
		.n_gfx = 1,
		.n_arg = 0,
		.disas_fn = d_DPPipeSync,
	},
	[gfxd_DPLoadTLUT_pal16] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTLUT_pal16",
		.opcode = G_SETTIMG,
		.n_gfx = 6,
		.n_arg = 2,
		.combine_fn = c_DPLoadTLUT_pal16,
	},
	[gfxd_DPLoadTLUT_pal256] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTLUT_pal256",
		.opcode = G_SETTIMG,
		.n_gfx = 6,
		.n_arg = 1,
		.combine_fn = c_DPLoadTLUT_pal256,
	},
	[gfxd_DPLoadMultiBlockYuvS] =
	{
		.prefix = NULL,
		.suffix = "DPLoadMultiBlockYuvS",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 14,
		.combine_fn = c_DPLoadMultiBlockYuvS,
		.ext = 1,
	},
	[gfxd_DPLoadMultiBlockYuv] =
	{
		.prefix = NULL,
		.suffix = "DPLoadMultiBlockYuv",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 14,
		.combine_fn = c_DPLoadMultiBlockYuv,
		.ext = 1,
	},
	[gfxd_DPLoadMultiBlock_4bS] =
	{
		.prefix = NULL,
		.suffix = "DPLoadMultiBlock_4bS",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 13,
		.combine_fn = c_DPLoadMultiBlock_4bS,
	},
	[gfxd_DPLoadMultiBlock_4b] =
	{
		.prefix = NULL,
		.suffix = "DPLoadMultiBlock_4b",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 13,
		.combine_fn = c_DPLoadMultiBlock_4b,
	},
	[gfxd_DPLoadMultiBlockS] =
	{
		.prefix = NULL,
		.suffix = "DPLoadMultiBlockS",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 14,
		.combine_fn = c_DPLoadMultiBlockS,
	},
	[gfxd_DPLoadMultiBlock] =
	{
		.prefix = NULL,
		.suffix = "DPLoadMultiBlock",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 14,
		.combine_fn = c_DPLoadMultiBlock,
	},
	[gfxd__DPLoadTextureBlockYuvS] =
	{
		.prefix = "_",
		.suffix = "DPLoadTextureBlockYuvS",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 13,
		.combine_fn = c__DPLoadTextureBlockYuvS,
		.ext = 1,
	},
	[gfxd__DPLoadTextureBlockYuv] =
	{
		.prefix = "_",
		.suffix = "DPLoadTextureBlockYuv",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 13,
		.combine_fn = c__DPLoadTextureBlockYuv,
		.ext = 1,
	},
	[gfxd__DPLoadTextureBlock_4bS] =
	{
		.prefix = "_",
		.suffix = "DPLoadTextureBlock_4bS",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 12,
		.combine_fn = c__DPLoadTextureBlock_4bS,
		.ext = 1,
	},
	[gfxd__DPLoadTextureBlock_4b] =
	{
		.prefix = "_",
		.suffix = "DPLoadTextureBlock_4b",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 12,
		.combine_fn = c__DPLoadTextureBlock_4b,
	},
	[gfxd__DPLoadTextureBlockS] =
	{
		.prefix = "_",
		.suffix = "DPLoadTextureBlockS",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 13,
		.combine_fn = c__DPLoadTextureBlockS,
		.ext = 1,
	},
	[gfxd__DPLoadTextureBlock] =
	{
		.prefix = "_",
		.suffix = "DPLoadTextureBlock",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 13,
		.combine_fn = c__DPLoadTextureBlock,
	},
	[gfxd_DPLoadTextureBlockYuvS] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTextureBlockYuvS",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 12,
		.combine_fn = c_DPLoadTextureBlockYuvS,
		.ext = 1,
	},
	[gfxd_DPLoadTextureBlockYuv] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTextureBlockYuv",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 12,
		.combine_fn = c_DPLoadTextureBlockYuv,
		.ext = 1,
	},
	[gfxd_DPLoadTextureBlock_4bS] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTextureBlock_4bS",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 11,
		.combine_fn = c_DPLoadTextureBlock_4bS,
	},
	[gfxd_DPLoadTextureBlock_4b] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTextureBlock_4b",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 11,
		.combine_fn = c_DPLoadTextureBlock_4b,
	},
	[gfxd_DPLoadTextureBlockS] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTextureBlockS",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 12,
		.combine_fn = c_DPLoadTextureBlockS,
	},
	[gfxd_DPLoadTextureBlock] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTextureBlock",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 12,
		.combine_fn = c_DPLoadTextureBlock,
	},
	[gfxd_DPLoadMultiTileYuv] =
	{
		.prefix = NULL,
		.suffix = "DPLoadMultiTileYuv",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 18,
		.combine_fn = c_DPLoadMultiTileYuv,
		.ext = 1,
	},
	[gfxd_DPLoadMultiTile_4b] =
	{
		.prefix = NULL,
		.suffix = "DPLoadMultiTile_4b",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 17,
		.combine_fn = c_DPLoadMultiTile_4b,
	},
	[gfxd_DPLoadMultiTile] =
	{
		.prefix = NULL,
		.suffix = "DPLoadMultiTile",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 18,
		.combine_fn = c_DPLoadMultiTile,
	},
	[gfxd__DPLoadTextureTileYuv] =
	{
		.prefix = "_",
		.suffix = "DPLoadTextureTileYuv",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 17,
		.combine_fn = c__DPLoadTextureTileYuv,
		.ext = 1,
	},
	[gfxd__DPLoadTextureTile_4b] =
	{
		.prefix = "_",
		.suffix = "DPLoadTextureTile_4b",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 16,
		.combine_fn = c__DPLoadTextureTile_4b,
		.ext = 1,
	},
	[gfxd__DPLoadTextureTile] =
	{
		.prefix = "_",
		.suffix = "DPLoadTextureTile",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 17,
		.combine_fn = c__DPLoadTextureTile,
		.ext = 1,
	},
	[gfxd_DPLoadTextureTileYuv] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTextureTileYuv",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 16,
		.combine_fn = c_DPLoadTextureTileYuv,
		.ext = 1,
	},
	[gfxd_DPLoadTextureTile_4b] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTextureTile_4b",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 15,
		.combine_fn = c_DPLoadTextureTile_4b,
	},
	[gfxd_DPLoadTextureTile] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTextureTile",
		.opcode = G_SETTIMG,
		.n_gfx = 7,
		.n_arg = 16,
		.combine_fn = c_DPLoadTextureTile,
	},
	[gfxd_DPLoadBlock] =
	{
		.prefix = NULL,
		.suffix = "DPLoadBlock",
		.opcode = G_LOADBLOCK,
		.n_gfx = 1,
		.n_arg = 5,
		.disas_fn = d_DPLoadBlock,
	},
	[gfxd_DPNoOp] =
	{
		.prefix = NULL,
		.suffix = "DPNoOp",
		.opcode = G_NOOP,
		.n_gfx = 1,
		.n_arg = 0,
		.alias = gfxd_DPNoOpTag,
	},
	[gfxd_DPNoOpTag] =
	{
		.prefix = NULL,
		.suffix = "DPNoOpTag",
		.opcode = G_NOOP,
		.n_gfx = 1,
		.n_arg = 1,
		.disas_fn = d_DPNoOpTag,
	},
	[gfxd_DPPipelineMode] =
	{
		.prefix = NULL,
		.suffix = "DPPipelineMode",
		.opcode = G_SETOTHERMODE_H,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPSetOtherModeHi,
	},
	[gfxd_DPSetBlendColor] =
	{
		.prefix = NULL,
		.suffix = "DPSetBlendColor",
		.opcode = G_SETBLENDCOLOR,
		.n_gfx = 1,
		.n_arg = 4,
		.disas_fn = d_DPSetBlendColor,
	},
	[gfxd_DPSetEnvColor] =
	{
		.prefix = NULL,
		.suffix = "DPSetEnvColor",
		.opcode = G_SETENVCOLOR,
		.n_gfx = 1,
		.n_arg = 4,
		.disas_fn = d_DPSetEnvColor,
	},
	[gfxd_DPSetFillColor] =
	{
		.prefix = NULL,
		.suffix = "DPSetFillColor",
		.opcode = G_SETFILLCOLOR,
		.n_gfx = 1,
		.n_arg = 1,
		.disas_fn = d_DPSetFillColor,
	},
	[gfxd_DPSetFogColor] =
	{
		.prefix = NULL,
		.suffix = "DPSetFogColor",
		.opcode = G_SETFOGCOLOR,
		.n_gfx = 1,
		.n_arg = 4,
		.disas_fn = d_DPSetFogColor,
	},
	[gfxd_DPSetPrimColor] =
	{
		.prefix = NULL,
		.suffix = "DPSetPrimColor",
		.opcode = G_SETPRIMCOLOR,
		.n_gfx = 1,
		.n_arg = 6,
		.disas_fn = d_DPSetPrimColor,
	},
	[gfxd_DPSetColorImage] =
	{
		.prefix = NULL,
		.suffix = "DPSetColorImage",
		.opcode = G_SETCIMG,
		.n_gfx = 1,
		.n_arg = 4,
		.disas_fn = d_DPSetColorImage,
	},
	[gfxd_DPSetDepthImage] =
	{
		.prefix = NULL,
		.suffix = "DPSetDepthImage",
		.opcode = G_SETZIMG,
		.n_gfx = 1,
		.n_arg = 1,
		.disas_fn = d_DPSetDepthImage,
	},
	[gfxd_DPSetTextureImage] =
	{
		.prefix = NULL,
		.suffix = "DPSetTextureImage",
		.opcode = G_SETTIMG,
		.n_gfx = 1,
		.n_arg = 4,
		.disas_fn = d_DPSetTextureImage,
	},
	[gfxd_DPSetAlphaCompare] =
	{
		.prefix = NULL,
		.suffix = "DPSetAlphaCompare",
		.opcode = G_SETOTHERMODE_L,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPSetOtherModeLo,
	},
	[gfxd_DPSetAlphaDither] =
	{
		.prefix = NULL,
		.suffix = "DPSetAlphaDither",
		.opcode = G_SETOTHERMODE_H,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPSetOtherModeHi,
	},
	[gfxd_DPSetColorDither] =
	{
		.prefix = NULL,
		.suffix = "DPSetColorDither",
		.opcode = G_SETOTHERMODE_H,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPSetOtherModeHi,
	},
	[gfxd_DPSetCombineMode] =
	{
		.prefix = NULL,
		.suffix = "DPSetCombineMode",
		.opcode = G_SETCOMBINE,
		.n_gfx = 1,
		.n_arg = 2,
		.alias = gfxd_DPSetCombineLERP,
	},
	[gfxd_DPSetCombineLERP] =
	{
		.prefix = NULL,
		.suffix = "DPSetCombineLERP",
		.opcode = G_SETCOMBINE,
		.n_gfx = 1,
		.n_arg = 16,
		.disas_fn = d_DPSetCombineLERP,
	},
	[gfxd_DPSetConvert] =
	{
		.prefix = NULL,
		.suffix = "DPSetConvert",
		.opcode = G_SETCONVERT,
		.n_gfx = 1,
		.n_arg = 6,
		.disas_fn = d_DPSetConvert,
	},
	[gfxd_DPSetTextureConvert] =
	{
		.prefix = NULL,
		.suffix = "DPSetTextureConvert",
		.opcode = G_SETOTHERMODE_H,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPSetOtherModeHi,
	},
	[gfxd_DPSetCycleType] =
	{
		.prefix = NULL,
		.suffix = "DPSetCycleType",
		.opcode = G_SETOTHERMODE_H,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPSetOtherModeHi,
	},
	[gfxd_DPSetDepthSource] =
	{
		.prefix = NULL,
		.suffix = "DPSetDepthSource",
		.opcode = G_SETOTHERMODE_L,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPSetOtherModeLo,
	},
	[gfxd_DPSetCombineKey] =
	{
		.prefix = NULL,
		.suffix = "DPSetCombineKey",
		.opcode = G_SETOTHERMODE_H,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPSetOtherModeHi,
	},
	[gfxd_DPSetKeyGB] =
	{
		.prefix = NULL,
		.suffix = "DPSetKeyGB",
		.opcode = G_SETKEYGB,
		.n_gfx = 1,
		.n_arg = 6,
		.disas_fn = d_DPSetKeyGB,
	},
	[gfxd_DPSetKeyR] =
	{
		.prefix = NULL,
		.suffix = "DPSetKeyR",
		.opcode = G_SETKEYR,
		.n_gfx = 1,
		.n_arg = 3,
		.disas_fn = d_DPSetKeyR,
	},
	[gfxd_DPSetPrimDepth] =
	{
		.prefix = NULL,
		.suffix = "DPSetPrimDepth",
		.opcode = G_SETPRIMDEPTH,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_DPSetPrimDepth,
	},
	[gfxd_DPSetRenderMode] =
	{
		.prefix = NULL,
		.suffix = "DPSetRenderMode",
		.opcode = G_SETOTHERMODE_L,
		.n_gfx = 1,
		.n_arg = 2,
		.alias = gfxd_SPSetOtherModeLo,
	},
	[gfxd_DPSetScissor] =
	{
		.prefix = NULL,
		.suffix = "DPSetScissor",
		.opcode = G_SETSCISSOR,
		.n_gfx = 1,
		.n_arg = 5,
		.alias = gfxd_DPSetScissorFrac,
	},
	[gfxd_DPSetScissorFrac] =
	{
		.prefix = NULL,
		.suffix = "DPSetScissorFrac",
		.opcode = G_SETSCISSOR,
		.n_gfx = 1,
		.n_arg = 5,
		.disas_fn = d_DPSetScissorFrac,
	},
	[gfxd_DPSetTextureDetail] =
	{
		.prefix = NULL,
		.suffix = "DPSetTextureDetail",
		.opcode = G_SETOTHERMODE_H,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPSetOtherModeHi,
	},
	[gfxd_DPSetTextureFilter] =
	{
		.prefix = NULL,
		.suffix = "DPSetTextureFilter",
		.opcode = G_SETOTHERMODE_H,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPSetOtherModeHi,
	},
	[gfxd_DPSetTextureLOD] =
	{
		.prefix = NULL,
		.suffix = "DPSetTextureLOD",
		.opcode = G_SETOTHERMODE_H,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPSetOtherModeHi,
	},
	[gfxd_DPSetTextureLUT] =
	{
		.prefix = NULL,
		.suffix = "DPSetTextureLUT",
		.opcode = G_SETOTHERMODE_H,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPSetOtherModeHi,
	},
	[gfxd_DPSetTexturePersp] =
	{
		.prefix = NULL,
		.suffix = "DPSetTexturePersp",
		.opcode = G_SETOTHERMODE_H,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPSetOtherModeHi,
	},
	[gfxd_DPSetTile] =
	{
		.prefix = NULL,
		.suffix = "DPSetTile",
		.opcode = G_SETTILE,
		.n_gfx = 1,
		.n_arg = 12,
		.disas_fn = d_DPSetTile,
	},
	[gfxd_DPSetTileSize] =
	{
		.prefix = NULL,
		.suffix = "DPSetTileSize",
		.opcode = G_SETTILESIZE,
		.n_gfx = 1,
		.n_arg = 5,
		.disas_fn = d_DPSetTileSize,
	},
	[gfxd_SP1Triangle] =
	{
		.prefix = NULL,
		.suffix = "SP1Triangle",
		.opcode = G_TRI1,
		.n_gfx = 1,
		.n_arg = 4,
		.disas_fn = d_SP1Triangle,
	},
#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
	[gfxd_SP2Triangles] =
	{
		.prefix = NULL,
		.suffix = "SP2Triangles",
		.opcode = G_TRI2,
		.n_gfx = 1,
		.n_arg = 8,
		.disas_fn = d_SP2Triangles,
	},
#endif
#if defined(F3DEX_GBI)
	[gfxd_SP1Quadrangle] =
	{
		.prefix = NULL,
		.suffix = "SP1Quadrangle",
		.opcode = G_TRI2,
		.n_gfx = 1,
		.n_arg = 5,
		.alias = gfxd_SP2Triangles,
	},
#elif defined(F3DEX_GBI_2)
	[gfxd_SP1Quadrangle] =
	{
		.prefix = NULL,
		.suffix = "SP1Quadrangle",
		.opcode = G_QUAD,
		.n_gfx = 1,
		.n_arg = 5,
		.disas_fn = d_SP1Quadrangle,
	},
#endif
#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
	[gfxd_SPBranchLessZraw] =
	{
		.prefix = NULL,
		.suffix = "SPBranchLessZraw",
		.opcode = G_RDPHALF_1,
		.n_gfx = 2,
		.n_arg = 3,
		.combine_fn = c_SPBranchLessZraw,
	},
#endif
	[gfxd_SPBranchList] =
	{
		.prefix = NULL,
		.suffix = "SPBranchList",
		.opcode = G_DL,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_DisplayList,
	},
	[gfxd_SPClipRatio] =
	{
		.prefix = NULL,
		.suffix = "SPClipRatio",
		.opcode = G_MOVEWORD,
		.n_gfx = 4,
		.n_arg = 1,
		.combine_fn = c_SPClipRatio,
	},
	[gfxd_SPCullDisplayList] =
	{
		.prefix = NULL,
		.suffix = "SPCullDisplayList",
		.opcode = G_CULLDL,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_SPCullDisplayList,
	},
	[gfxd_SPDisplayList] =
	{
		.prefix = NULL,
		.suffix = "SPDisplayList",
		.opcode = G_DL,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_DisplayList,
	},
	[gfxd_SPEndDisplayList] =
	{
		.prefix = NULL,
		.suffix = "SPEndDisplayList",
		.opcode = G_ENDDL,
		.n_gfx = 1,
		.n_arg = 0,
		.disas_fn = d_SPEndDisplayList,
	},
	[gfxd_SPFogFactor] =
	{
		.prefix = NULL,
		.suffix = "SPFogFactor",
		.opcode = G_MOVEWORD,
		.n_gfx = 1,
		.n_arg = 2,
		.alias = gfxd_SPFogPosition,
	},
	[gfxd_SPFogPosition] =
	{
		.prefix = NULL,
		.suffix = "SPFogPosition",
		.opcode = G_MOVEWORD,
		.n_gfx = 1,
		.n_arg = 2,
		.alias = gfxd_MoveWd,
	},
#if defined(F3D_GBI) || defined(F3DEX_GBI)
	[gfxd_SPForceMatrix] =
	{
		.prefix = NULL,
		.suffix = "SPForceMatrix",
		.opcode = G_MOVEMEM,
		.n_gfx = 4,
		.n_arg = 1,
		.combine_fn = c_SPForceMatrix,
	},
	[gfxd_SPSetGeometryMode] =
	{
		.prefix = NULL,
		.suffix = "SPSetGeometryMode",
		.opcode = G_SETGEOMETRYMODE,
		.n_gfx = 1,
		.n_arg = 1,
		.disas_fn = d_SPSetGeometryMode,
	},
	[gfxd_SPClearGeometryMode] =
	{
		.prefix = NULL,
		.suffix = "SPClearGeometryMode",
		.opcode = G_CLEARGEOMETRYMODE,
		.n_gfx = 1,
		.n_arg = 1,
		.disas_fn = d_SPClearGeometryMode,
	},
	[gfxd_SPLoadGeometryMode] =
	{
		.prefix = NULL,
		.suffix = "SPLoadGeometryMode",
		.opcode = G_CLEARGEOMETRYMODE,
		.n_gfx = 2,
		.n_arg = 1,
		.combine_fn = c_SPLoadGeometryMode,
	},
#elif defined(F3DEX_GBI_2)
	[gfxd_SPForceMatrix] =
	{
		.prefix = NULL,
		.suffix = "SPForceMatrix",
		.opcode = G_MOVEMEM,
		.n_gfx = 2,
		.n_arg = 1,
		.combine_fn = c_SPForceMatrix,
	},
	[gfxd_SPSetGeometryMode] =
	{
		.prefix = NULL,
		.suffix = "SPSetGeometryMode",
		.opcode = G_GEOMETRYMODE,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPGeometryMode,
	},
	[gfxd_SPClearGeometryMode] =
	{
		.prefix = NULL,
		.suffix = "SPClearGeometryMode",
		.opcode = G_GEOMETRYMODE,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPGeometryMode,
	},
	[gfxd_SPLoadGeometryMode] =
	{
		.prefix = NULL,
		.suffix = "SPLoadGeometryMode",
		.opcode = G_GEOMETRYMODE,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPGeometryMode,
	},
#endif
#if defined(F3D_GBI) || defined(F3DEX_GBI)
	[gfxd_SPInsertMatrix] =
	{
		.prefix = NULL,
		.suffix = "SPInsertMatrix",
		.opcode = G_MOVEWORD,
		.n_gfx = 1,
		.n_arg = 2,
		.alias = gfxd_MoveWd,
	},
#endif
	[gfxd_SPLine3D] =
	{
		.prefix = NULL,
		.suffix = "SPLine3D",
		.opcode = G_LINE3D,
		.n_gfx = 1,
		.n_arg = 3,
		.alias = gfxd_SPLineW3D,
	},
	[gfxd_SPLineW3D] =
	{
		.prefix = NULL,
		.suffix = "SPLineW3D",
		.opcode = G_LINE3D,
		.n_gfx = 1,
		.n_arg = 4,
		.disas_fn = d_SPLineW3D,
	},
#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
	[gfxd_SPLoadUcode] =
	{
		.prefix = NULL,
		.suffix = "SPLoadUcode",
		.opcode = G_RDPHALF_1,
		.n_gfx = 2,
		.n_arg = 2,
		.combine_fn = c_SPLoadUcode,
	},
#endif
	[gfxd_SPLookAtX] =
	{
		.prefix = NULL,
		.suffix = "SPLookAtX",
		.opcode = G_MOVEMEM,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_MoveMem,
	},
	[gfxd_SPLookAtY] =
	{
		.prefix = NULL,
		.suffix = "SPLookAtY",
		.opcode = G_MOVEMEM,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_MoveMem,
	},
	[gfxd_SPLookAt] =
	{
		.prefix = NULL,
		.suffix = "SPLookAt",
		.opcode = G_MOVEMEM,
		.n_gfx = 2,
		.n_arg = 1,
		.combine_fn = c_SPLookAt,
	},
	[gfxd_SPMatrix] =
	{
		.prefix = NULL,
		.suffix = "SPMatrix",
		.opcode = G_MTX,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_SPMatrix,
	},
#if defined(F3D_GBI) || (defined(F3D_BETA) && defined(F3DEX_GBI))
	[gfxd_SPModifyVertex] =
	{
		.prefix = NULL,
		.suffix = "SPModifyVertex",
		.opcode = G_MOVEWORD,
		.n_gfx = 1,
		.n_arg = 3,
		.alias = gfxd_MoveWd,
	},
#elif defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
	[gfxd_SPModifyVertex] =
	{
		.prefix = NULL,
		.suffix = "SPModifyVertex",
		.opcode = G_MODIFYVTX,
		.n_gfx = 1,
		.n_arg = 3,
		.disas_fn = d_SPModifyVertex,
	},
#endif
#if defined(F3D_BETA) && (defined(F3D_GBI) || defined(F3DEX_GBI))
	[gfxd_SPPerspNormalize] =
	{
		.prefix = NULL,
		.suffix = "SPPerspNormalize",
		.opcode = G_PERSPNORM,
		.n_gfx = 1,
		.n_arg = 1,
		.disas_fn = d_SPPerspNormalize,
	},
#else
	[gfxd_SPPerspNormalize] =
	{
		.prefix = NULL,
		.suffix = "SPPerspNormalize",
		.opcode = G_MOVEWORD,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_MoveWd,
	},
#endif
#if defined(F3D_GBI) || defined(F3DEX_GBI)
	[gfxd_SPPopMatrix] =
	{
		.prefix = NULL,
		.suffix = "SPPopMatrix",
		.opcode = G_POPMTX,
		.n_gfx = 1,
		.n_arg = 1,
		.disas_fn = d_SPPopMatrix,
	},
#elif defined(F3DEX_GBI_2)
	[gfxd_SPPopMatrix] =
	{
		.prefix = NULL,
		.suffix = "SPPopMatrix",
		.opcode = G_POPMTX,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_SPPopMatrixN,
	},
	[gfxd_SPPopMatrixN] =
	{
		.prefix = NULL,
		.suffix = "SPPopMatrixN",
		.opcode = G_POPMTX,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_SPPopMatrixN,
	},
#endif
	[gfxd_SPSegment] =
	{
		.prefix = NULL,
		.suffix = "SPSegment",
		.opcode = G_MOVEWORD,
		.n_gfx = 1,
		.n_arg = 2,
		.alias = gfxd_MoveWd,
	},
	[gfxd_SPSetLights1] =
	{
		.prefix = NULL,
		.suffix = "SPSetLights1",
		.opcode = G_MOVEWORD,
		.n_gfx = 3,
		.n_arg = 1,
		.combine_fn = c_SPSetLights1,
	},
	[gfxd_SPSetLights2] =
	{
		.prefix = NULL,
		.suffix = "SPSetLights2",
		.opcode = G_MOVEWORD,
		.n_gfx = 4,
		.n_arg = 1,
		.combine_fn = c_SPSetLights2,
	},
	[gfxd_SPSetLights3] =
	{
		.prefix = NULL,
		.suffix = "SPSetLights3",
		.opcode = G_MOVEWORD,
		.n_gfx = 5,
		.n_arg = 1,
		.combine_fn = c_SPSetLights3,
	},
	[gfxd_SPSetLights4] =
	{
		.prefix = NULL,
		.suffix = "SPSetLights4",
		.opcode = G_MOVEWORD,
		.n_gfx = 6,
		.n_arg = 1,
		.combine_fn = c_SPSetLights4,
	},
	[gfxd_SPSetLights5] =
	{
		.prefix = NULL,
		.suffix = "SPSetLights5",
		.opcode = G_MOVEWORD,
		.n_gfx = 7,
		.n_arg = 1,
		.combine_fn = c_SPSetLights5,
	},
	[gfxd_SPSetLights6] =
	{
		.prefix = NULL,
		.suffix = "SPSetLights6",
		.opcode = G_MOVEWORD,
		.n_gfx = 8,
		.n_arg = 1,
		.combine_fn = c_SPSetLights6,
	},
	[gfxd_SPSetLights7] =
	{
		.prefix = NULL,
		.suffix = "SPSetLights7",
		.opcode = G_MOVEWORD,
		.n_gfx = 9,
		.n_arg = 1,
		.combine_fn = c_SPSetLights7,
	},
	[gfxd_SPNumLights] =
	{
		.prefix = NULL,
		.suffix = "SPNumLights",
		.opcode = G_MOVEWORD,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_MoveWd,
	},
	[gfxd_SPLight] =
	{
		.prefix = NULL,
		.suffix = "SPLight",
		.opcode = G_MOVEMEM,
		.n_gfx = 1,
		.n_arg = 2,
		.alias = gfxd_MoveMem,
	},
	[gfxd_SPLightColor] =
	{
		.prefix = NULL,
		.suffix = "SPLightColor",
		.opcode = G_MOVEWORD,
		.n_gfx = 2,
		.n_arg = 2,
		.combine_fn = c_SPLightColor,
	},
	[gfxd_SPTexture] =
	{
		.prefix = NULL,
		.suffix = "SPTexture",
		.opcode = G_TEXTURE,
		.n_gfx = 1,
		.n_arg = 5,
		.disas_fn = d_SPTexture,
	},
	[gfxd_SPTextureRectangle] =
	{
		.prefix = NULL,
		.suffix = "SPTextureRectangle",
		.opcode = G_TEXRECT,
		.n_gfx = 3,
		.n_arg = 9,
		.combine_fn = c_SPTextureRectangle,
	},
	[gfxd_SPTextureRectangleFlip] =
	{
		.prefix = NULL,
		.suffix = "SPTextureRectangleFlip",
		.opcode = G_TEXRECTFLIP,
		.n_gfx = 3,
		.n_arg = 9,
		.combine_fn = c_SPTextureRectangleFlip,
	},
	[gfxd_SPVertex] =
	{
		.prefix = NULL,
		.suffix = "SPVertex",
		.opcode = G_VTX,
		.n_gfx = 1,
		.n_arg = 3,
		.disas_fn = d_SPVertex,
	},
	[gfxd_SPViewport] =
	{
		.prefix = NULL,
		.suffix = "SPViewport",
		.opcode = G_MOVEMEM,
		.n_gfx = 1,
		.n_arg = 1,
		.alias = gfxd_MoveMem,
	},
	[gfxd_DPLoadTLUTCmd] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTLUTCmd",
		.opcode = G_LOADTLUT,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_DPLoadTLUTCmd,
	},
	[gfxd_DPLoadTLUT] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTLUT",
		.opcode = G_SETTIMG,
		.n_gfx = 6,
		.n_arg = 3,
		.combine_fn = c_DPLoadTLUT,
	},
#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
	[gfxd_BranchZ] =
	{
		.prefix = NULL,
		.suffix = "BranchZ",
		.opcode = G_BRANCH_Z,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_BranchZ,
		.ext = 1,
	},
#endif
	[gfxd_DisplayList] =
	{
		.prefix = NULL,
		.suffix = "DisplayList",
		.opcode = G_DL,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_DisplayList,
		.ext = 1,
	},
	[gfxd_DPHalf1] =
	{
		.prefix = NULL,
		.suffix = "DPHalf1",
		.opcode = G_RDPHALF_1,
		.n_gfx = 1,
		.n_arg = 1,
		.disas_fn = d_DPHalf1,
		.ext = 1,
	},
	[gfxd_DPHalf2] =
	{
		.prefix = NULL,
		.suffix = "DPHalf2",
		.opcode = G_RDPHALF_2,
		.n_gfx = 1,
		.n_arg = 1,
		.disas_fn = d_DPHalf2,
		.ext = 1,
	},
	[gfxd_DPWord] =
	{
		.prefix = NULL,
		.suffix = "DPWord",
		.opcode = G_RDPHALF_1,
		.n_gfx = 2,
		.n_arg = 2,
		.combine_fn = c_DPWord,
	},
	[gfxd_DPLoadTile] =
	{
		.prefix = NULL,
		.suffix = "DPLoadTile",
		.opcode = G_LOADTILE,
		.n_gfx = 1,
		.n_arg = 5,
		.disas_fn = d_DPLoadTile,
	},
#if defined(F3DEX_GBI_2)
	[gfxd_SPGeometryMode] =
	{
		.prefix = NULL,
		.suffix = "SPGeometryMode",
		.opcode = G_GEOMETRYMODE,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_SPGeometryMode,
	},
#endif
	[gfxd_SPSetOtherMode] =
	{
		.prefix = NULL,
		.suffix = "SPSetOtherMode",
		.opcode = -1,
		.n_gfx = 1,
		.n_arg = 4,
		.disas_fn = d_SPSetOtherMode,
	},
	[gfxd_SPSetOtherModeLo] =
	{
		.prefix = NULL,
		.suffix = "SPSetOtherModeLo",
		.opcode = G_SETOTHERMODE_L,
		.n_gfx = 1,
		.n_arg = 3,
		.disas_fn = d_SPSetOtherModeLo,
		.ext = 1,
	},
	[gfxd_SPSetOtherModeHi] =
	{
		.prefix = NULL,
		.suffix = "SPSetOtherModeHi",
		.opcode = G_SETOTHERMODE_H,
		.n_gfx = 1,
		.n_arg = 3,
		.disas_fn = d_SPSetOtherModeHi,
		.ext = 1,
	},
	[gfxd_DPSetOtherMode] =
	{
		.prefix = NULL,
		.suffix = "DPSetOtherMode",
		.opcode = G_RDPSETOTHERMODE,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_DPSetOtherMode,
	},
	[gfxd_MoveWd] =
	{
		.prefix = NULL,
		.suffix = "MoveWd",
		.opcode = G_MOVEWORD,
		.n_gfx = 1,
		.n_arg = 3,
		.disas_fn = d_MoveWd,
	},
#if defined(F3D_GBI) || defined(F3DEX_GBI)
	[gfxd_MoveMem] =
	{
		.prefix = NULL,
		.suffix = "MoveMem",
		.opcode = G_MOVEMEM,
		.n_gfx = 1,
		.n_arg = 3,
		.disas_fn = d_MoveMem,
		.ext = 1,
	},
#elif defined(F3DEX_GBI_2)
	[gfxd_MoveMem] =
	{
		.prefix = NULL,
		.suffix = "MoveMem",
		.opcode = G_MOVEMEM,
		.n_gfx = 1,
		.n_arg = 4,
		.disas_fn = d_MoveMem,
		.ext = 1,
	},
#endif
#if defined(F3DEX_GBI_2)
	[gfxd_SPDma_io] =
	{
		.prefix = NULL,
		.suffix = "SPDma_io",
		.opcode = G_DMA_IO,
		.n_gfx = 1,
		.n_arg = 4,
		.disas_fn = d_SPDma_io,
	},
	[gfxd_SPDmaRead] =
	{
		.prefix = NULL,
		.suffix = "SPDmaRead",
		.opcode = G_DMA_IO,
		.n_gfx = 1,
		.n_arg = 3,
		.alias = gfxd_SPDma_io,
	},
	[gfxd_SPDmaWrite] =
	{
		.prefix = NULL,
		.suffix = "SPDmaWrite",
		.opcode = G_DMA_IO,
		.n_gfx = 1,
		.n_arg = 3,
		.alias = gfxd_SPDma_io,
	},
#endif
#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
	[gfxd_LoadUcode] =
	{
		.prefix = NULL,
		.suffix = "LoadUcode",
		.opcode = G_LOAD_UCODE,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_LoadUcode,
	},
	[gfxd_SPLoadUcodeEx] =
	{
		.prefix = NULL,
		.suffix = "SPLoadUcodeEx",
		.opcode = G_RDPHALF_1,
		.n_gfx = 2,
		.n_arg = 3,
		.combine_fn = c_SPLoadUcodeEx,
	},
#endif
	[gfxd_TexRect] =
	{
		.prefix = NULL,
		.suffix = "TexRect",
		.opcode = G_TEXRECT,
		.n_gfx = 1,
		.n_arg = 5,
		.disas_fn = d_TexRect,
		.ext = 1,
	},
	[gfxd_TexRectFlip] =
	{
		.prefix = NULL,
		.suffix = "TexRectFlip",
		.opcode = G_TEXRECTFLIP,
		.n_gfx = 1,
		.n_arg = 5,
		.disas_fn = d_TexRectFlip,
		.ext = 1,
	},
	[gfxd_SPNoOp] =
	{
		.prefix = NULL,
		.suffix = "SPNoOp",
		.opcode = G_SPNOOP,
		.n_gfx = 1,
		.n_arg = 0,
		.disas_fn = d_SPNoOp,
	},
#if defined(F3DEX_GBI_2)
	[gfxd_Special3] =
	{
		.prefix = NULL,
		.suffix = "Special3",
		.opcode = G_SPECIAL_3,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_Special3,
		.ext = 1,
	},
	[gfxd_Special2] =
	{
		.prefix = NULL,
		.suffix = "Special2",
		.opcode = G_SPECIAL_2,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_Special2,
		.ext = 1,
	},
	[gfxd_Special1] =
	{
		.prefix = NULL,
		.suffix = "Special1",
		.opcode = G_SPECIAL_1,
		.n_gfx = 1,
		.n_arg = 2,
		.disas_fn = d_Special1,
		.ext = 1,
	},
#endif
};
