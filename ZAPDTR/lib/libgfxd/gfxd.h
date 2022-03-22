#ifndef GFXD_H
#define GFXD_H
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

enum
{
	gfxd_Word,		/* generic word */
	gfxd_Opcode,		/* command opcode (G_*) */
	gfxd_Coordi,		/* integer coordinate */
	gfxd_Coordq,		/* fractional (q10.2) coordinate */
	gfxd_Pal,		/* palette index */
	gfxd_Tlut,		/* tlut pointer */
	gfxd_Timg,		/* texture image pointer */
	gfxd_Tmem,		/* tmem address */
	gfxd_Tile,		/* tile index */
	gfxd_Fmt,		/* texture format */
	gfxd_Siz,		/* texture pixel size */
	gfxd_Dim,		/* integer dimension (width / height) */
	gfxd_Cm,		/* clamp and mirror flags */
	gfxd_Tm,		/* tile mask */
	gfxd_Ts,		/* tile shift */
	gfxd_Dxt,		/* texture dxt */
	gfxd_Tag,		/* generic tag */
	gfxd_Pm,		/* pipeline mode */
	gfxd_Colorpart,		/* color component */
	gfxd_Color,		/* color */
	gfxd_Lodfrac,		/* lod fraction (q0.8) */
	gfxd_Cimg,		/* color image pointer */
	gfxd_Zimg,		/* depth image pointer */
	gfxd_Ac,		/* alpha compare mode */
	gfxd_Ad,		/* alpha dither mode */
	gfxd_Cd,		/* color dither mode */
	gfxd_Ccpre,		/* color combiner preset index */
	gfxd_Ccmuxa,		/* color mux operand (a) */
	gfxd_Ccmuxb,		/* color mux operand (b) */
	gfxd_Ccmuxc,		/* color mux operand (c) */
	gfxd_Ccmuxd,		/* color mux operand (d) */
	gfxd_Acmuxabd,		/* alpha mux operand (a, b, or d) */
	gfxd_Acmuxc,		/* alpha mux operand (c) */
	gfxd_Cv,		/* color convert operand */
	gfxd_Tc,		/* texture convert mode */
	gfxd_Cyc,		/* cycle type */
	gfxd_Zs,		/* depth source mode */
	gfxd_Ck,		/* combine key mode */
	gfxd_Keyscale,		/* combine key scale */
	gfxd_Keywidth,		/* combine key width */
	gfxd_Zi,		/* integer depth */
	gfxd_Rm1,		/* cycle 1 render mode */
	gfxd_Rm2,		/* cycle 2 render mode */
	gfxd_Sc,		/* scissor mode */
	gfxd_Td,		/* texture detail mode */
	gfxd_Tf,		/* texture filter mode */
	gfxd_Tl,		/* texture LOD mode */
	gfxd_Tt,		/* textuure LUT mode */
	gfxd_Tp,		/* texture perspective mode */
	gfxd_Line,		/* texture line size */
	gfxd_Vtx,		/* vertex index */
	gfxd_Vtxflag,		/* vertex flag */
	gfxd_Dl,		/* display list pointer */
	gfxd_Zraw,		/* raw depth value (q16.16) */
	gfxd_Dlflag,		/* display list flag */
	gfxd_Cr,		/* clip ratio */
	gfxd_Num,		/* element count */
	gfxd_Fogz,		/* fog factor */
	gfxd_Fogp,		/* fog position (0 - 1000) */
	gfxd_Mtxptr,		/* matrix pointer */
	gfxd_Gm,		/* geometry mode */
	gfxd_Mwo_matrix,	/* matrix moveword offset */
	gfxd_Linewd,		/* line width (1.5 + q7.1) */
	gfxd_Uctext,		/* microcode text pointer */
	gfxd_Ucdata,		/* microcode data pointer */
	gfxd_Size,		/* data size */
	gfxd_Lookatptr,		/* lookat pointer */
	gfxd_Mtxparam,		/* matrix param */
	gfxd_Mtxstack,		/* matrix param (stack select only) */
	gfxd_Mwo_point,		/* vertex moveword offset */
	gfxd_Wscale,		/* w-component scale (perspnorm) */
	gfxd_Seg,		/* segment number */
	gfxd_Segptr,		/* segment pointer */
	gfxd_Lightsn,		/* dereferenced Lighstn pointer */
	gfxd_Numlights,		/* light count (NUMLIGHTS_*) */
	gfxd_Lightnum,		/* light number (LIGHT_*) */
	gfxd_Lightptr,		/* light pointer */
	gfxd_Tcscale,		/* texture coordinate scale */
	gfxd_Switch,		/* on-off value */
	gfxd_St,		/* vertex coordinate (q10.5) */
	gfxd_Stdelta,		/* vertex coordinate delta (q5.10) */
	gfxd_Vtxptr,		/* vertex pointer */
	gfxd_Vpptr,		/* viewport pointer */
	gfxd_Dram,		/* generic dram address */
	gfxd_Sftlo,		/* othermode lo shift */
	gfxd_Othermodelo,	/* othermode lo value */
	gfxd_Sfthi,		/* othermode hi shift */
	gfxd_Othermodehi,	/* othermode hi value */
	gfxd_Mw,		/* moveword index */
	gfxd_Mwo,		/* moveword offset */
	gfxd_Mwo_clip,		/* clip ratio moveword offset */
	gfxd_Mwo_lightcol,	/* light color moveword offset */
	gfxd_Mv,		/* movemem index */
	gfxd_Mvo,		/* movemem offset */
	gfxd_Dmem,		/* dmem address */
	gfxd_Dmaflag,		/* dma io flag */
};

enum
{
	gfxd_Invalid,
	gfxd_DPFillRectangle,
	gfxd_DPFullSync,
	gfxd_DPLoadSync,
	gfxd_DPTileSync,
	gfxd_DPPipeSync,
	gfxd_DPLoadTLUT_pal16,
	gfxd_DPLoadTLUT_pal256,
	gfxd_DPLoadMultiBlockYuvS,
	gfxd_DPLoadMultiBlockYuv,
	gfxd_DPLoadMultiBlock_4bS,
	gfxd_DPLoadMultiBlock_4b,
	gfxd_DPLoadMultiBlockS,
	gfxd_DPLoadMultiBlock,
	gfxd__DPLoadTextureBlockYuvS,
	gfxd__DPLoadTextureBlockYuv,
	gfxd__DPLoadTextureBlock_4bS,
	gfxd__DPLoadTextureBlock_4b,
	gfxd__DPLoadTextureBlockS,
	gfxd__DPLoadTextureBlock,
	gfxd_DPLoadTextureBlockYuvS,
	gfxd_DPLoadTextureBlockYuv,
	gfxd_DPLoadTextureBlock_4bS,
	gfxd_DPLoadTextureBlock_4b,
	gfxd_DPLoadTextureBlockS,
	gfxd_DPLoadTextureBlock,
	gfxd_DPLoadMultiTileYuv,
	gfxd_DPLoadMultiTile_4b,
	gfxd_DPLoadMultiTile,
	gfxd__DPLoadTextureTileYuv,
	gfxd__DPLoadTextureTile_4b,
	gfxd__DPLoadTextureTile,
	gfxd_DPLoadTextureTileYuv,
	gfxd_DPLoadTextureTile_4b,
	gfxd_DPLoadTextureTile,
	gfxd_DPLoadBlock,
	gfxd_DPNoOp,
	gfxd_DPNoOpTag,
	gfxd_DPPipelineMode,
	gfxd_DPSetBlendColor,
	gfxd_DPSetEnvColor,
	gfxd_DPSetFillColor,
	gfxd_DPSetFogColor,
	gfxd_DPSetPrimColor,
	gfxd_DPSetColorImage,
	gfxd_DPSetDepthImage,
	gfxd_DPSetTextureImage,
	gfxd_DPSetAlphaCompare,
	gfxd_DPSetAlphaDither,
	gfxd_DPSetColorDither,
	gfxd_DPSetCombineMode,
	gfxd_DPSetCombineLERP,
	gfxd_DPSetConvert,
	gfxd_DPSetTextureConvert,
	gfxd_DPSetCycleType,
	gfxd_DPSetDepthSource,
	gfxd_DPSetCombineKey,
	gfxd_DPSetKeyGB,
	gfxd_DPSetKeyR,
	gfxd_DPSetPrimDepth,
	gfxd_DPSetRenderMode,
	gfxd_DPSetScissor,
	gfxd_DPSetScissorFrac,
	gfxd_DPSetTextureDetail,
	gfxd_DPSetTextureFilter,
	gfxd_DPSetTextureLOD,
	gfxd_DPSetTextureLUT,
	gfxd_DPSetTexturePersp,
	gfxd_DPSetTile,
	gfxd_DPSetTileSize,
	gfxd_SP1Triangle,
	gfxd_SP2Triangles,
	gfxd_SP1Quadrangle,
	gfxd_SPBranchLessZraw,
	gfxd_SPBranchList,
	gfxd_SPClipRatio,
	gfxd_SPCullDisplayList,
	gfxd_SPDisplayList,
	gfxd_SPEndDisplayList,
	gfxd_SPFogFactor,
	gfxd_SPFogPosition,
	gfxd_SPForceMatrix,
	gfxd_SPSetGeometryMode,
	gfxd_SPClearGeometryMode,
	gfxd_SPLoadGeometryMode,
	gfxd_SPInsertMatrix,
	gfxd_SPLine3D,
	gfxd_SPLineW3D,
	gfxd_SPLoadUcode,
	gfxd_SPLookAtX,
	gfxd_SPLookAtY,
	gfxd_SPLookAt,
	gfxd_SPMatrix,
	gfxd_SPModifyVertex,
	gfxd_SPPerspNormalize,
	gfxd_SPPopMatrix,
	gfxd_SPPopMatrixN,
	gfxd_SPSegment,
	gfxd_SPSetLights1,
	gfxd_SPSetLights2,
	gfxd_SPSetLights3,
	gfxd_SPSetLights4,
	gfxd_SPSetLights5,
	gfxd_SPSetLights6,
	gfxd_SPSetLights7,
	gfxd_SPNumLights,
	gfxd_SPLight,
	gfxd_SPLightColor,
	gfxd_SPTexture,
	gfxd_SPTextureRectangle,
	gfxd_SPTextureRectangleFlip,
	gfxd_SPVertex,
	gfxd_SPViewport,
	gfxd_DPLoadTLUTCmd,
	gfxd_DPLoadTLUT,
	gfxd_BranchZ,
	gfxd_DisplayList,
	gfxd_DPHalf1,
	gfxd_DPHalf2,
	gfxd_DPWord,
	gfxd_DPLoadTile,
	gfxd_SPGeometryMode,
	gfxd_SPSetOtherMode,
	gfxd_SPSetOtherModeLo,
	gfxd_SPSetOtherModeHi,
	gfxd_DPSetOtherMode,
	gfxd_MoveWd,
	gfxd_MoveMem,
	gfxd_SPDma_io,
	gfxd_SPDmaRead,
	gfxd_SPDmaWrite,
	gfxd_LoadUcode,
	gfxd_SPLoadUcodeEx,
	gfxd_TexRect,
	gfxd_TexRectFlip,
	gfxd_SPNoOp,
	gfxd_Special3,
	gfxd_Special2,
	gfxd_Special1,
};

enum
{
	gfxd_stop_on_invalid,
	gfxd_stop_on_end,
	gfxd_emit_dec_color,
	gfxd_emit_q_macro,
	gfxd_emit_ext_macro,
};

enum
{
	gfxd_endian_big,
	gfxd_endian_little,
	gfxd_endian_host,
};

enum
{
	gfxd_argfmt_i,
	gfxd_argfmt_u,
	gfxd_argfmt_f,
};

typedef union
{
	int32_t		i;
	uint32_t	u;
	float		f;
} gfxd_value_t;

typedef const struct gfxd_ucode *gfxd_ucode_t;

typedef int gfxd_input_fn_t(void *buf, int count);
void gfxd_input_buffer(const void *buf, int size);
void gfxd_input_fd(int fd);
void gfxd_input_callback(gfxd_input_fn_t *fn);

typedef int gfxd_output_fn_t(const char *buf, int count);
void gfxd_output_buffer(char *buf, int size);
void gfxd_output_fd(int fd);
void gfxd_output_callback(gfxd_output_fn_t *fn);

typedef int gfxd_macro_fn_t(void);
void gfxd_macro_fn(gfxd_macro_fn_t *fn);
gfxd_macro_fn_t gfxd_macro_dflt;

typedef void gfxd_arg_fn_t(int arg_num);
void gfxd_arg_fn(gfxd_arg_fn_t *fn);
gfxd_arg_fn_t gfxd_arg_dflt;

typedef int gfxd_tlut_fn_t(uint32_t tlut, int32_t idx, int32_t count);
void gfxd_tlut_callback(gfxd_tlut_fn_t *fn);

typedef int gfxd_timg_fn_t(uint32_t timg, int32_t fmt, int32_t siz,
	int32_t width, int32_t height, int32_t pal);
void gfxd_timg_callback(gfxd_timg_fn_t *fn);

typedef int gfxd_cimg_fn_t(uint32_t cimg, int32_t fmt, int32_t siz,
	int32_t width);
void gfxd_cimg_callback(gfxd_cimg_fn_t *fn);

typedef int gfxd_zimg_fn_t(uint32_t zimg);
void gfxd_zimg_callback(gfxd_zimg_fn_t *fn);

typedef int gfxd_dl_fn_t(uint32_t dl);
void gfxd_dl_callback(gfxd_dl_fn_t *fn);

typedef int gfxd_mtx_fn_t(uint32_t mtx);
void gfxd_mtx_callback(gfxd_mtx_fn_t *fn);

typedef int gfxd_lookat_fn_t(uint32_t lookat, int32_t count);
void gfxd_lookat_callback(gfxd_lookat_fn_t *fn);

typedef int gfxd_light_fn_t(uint32_t light, int32_t count);
void gfxd_light_callback(gfxd_light_fn_t *fn);

typedef int gfxd_seg_fn_t(uint32_t seg, int32_t num);
void gfxd_seg_callback(gfxd_seg_fn_t *fn);

typedef int gfxd_vtx_fn_t(uint32_t vtx, int32_t num);
void gfxd_vtx_callback(gfxd_vtx_fn_t *fn);

typedef int gfxd_vp_fn_t(uint32_t vp);
void gfxd_vp_callback(gfxd_vp_fn_t *fn);

typedef int gfxd_uctext_fn_t(uint32_t text, uint32_t size);
void gfxd_uctext_callback(gfxd_uctext_fn_t *fn);

typedef int gfxd_ucdata_fn_t(uint32_t data, uint32_t size);
void gfxd_ucdata_callback(gfxd_ucdata_fn_t *fn);

typedef int gfxd_dram_fn_t(uint32_t dram, uint32_t size);
void gfxd_dram_callback(gfxd_dram_fn_t *fn);

int gfxd_write(const void *buf, int count);
int gfxd_puts(const char *str);
int gfxd_printf(const char *fmt, ...);
int gfxd_print_value(int type, const gfxd_value_t *value);

void gfxd_target(gfxd_ucode_t ucode);
void gfxd_endian(int endian, int wordsize);
void gfxd_dynamic(const char *arg);
void gfxd_enable(int cap);
void gfxd_disable(int cap);
void gfxd_udata_set(void *ptr);
void *gfxd_udata_get(void);

int gfxd_execute(void);

int gfxd_macro_offset(void);
int gfxd_macro_packets(void);
const void *gfxd_macro_data(void);
int gfxd_macro_id(void);
const char *gfxd_macro_name(void);

int gfxd_arg_count(void);
int gfxd_arg_type(int arg_num);
const char *gfxd_arg_name(int arg_num);
int gfxd_arg_fmt(int arg_num);
const gfxd_value_t *gfxd_arg_value(int arg_num);
const gfxd_value_t *gfxd_value_by_type(int type, int idx);
int gfxd_arg_valid(int arg_num);
int gfxd_arg_callbacks(int arg_num);

extern const gfxd_ucode_t gfxd_f3d;
extern const gfxd_ucode_t gfxd_f3db;
extern const gfxd_ucode_t gfxd_f3dex;
extern const gfxd_ucode_t gfxd_f3dexb;
extern const gfxd_ucode_t gfxd_f3dex2;

#ifdef __cplusplus
}
#endif

#endif
