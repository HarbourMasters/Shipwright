/**
 * gbi.h version 0.3.6
 * n64 graphics microcode interface library
 * compatible with fast3d, f3dex, f3dex2, s2dex, and s2dex2
 *
 * select a microcode with one of these preprocessor definitions;
 *   #define F3D_GBI
 * for fast3d (selected automatically by default), or
 *   #define F3DEX_GBI
 * for f3dex/s2dex, or
 *   #define F3DEX_GBI_2
 * for f3dex2/s2dex2
 *
 * for early versions of fast3d and f3dex, also define the following;
 *   #define F3D_BETA
 *
 * ido incompatibilities;
 * - use of c99 variadic macros
 * - use of c99 fixed-width integer types
 * - use of c99 designated initializers
 * - use of c99 compound literals
 * - use of c11 _Alignas
 * - use of gnu c compound expressions
 * - use of gnu c __typeof__
 *
 * libultra incompatibilities;
 * - many private, undocumented, or obsolete macros not commonly used by
 *   programmers are missing
 * - many different implementation details that will produce matching gbi,
 *   but not matching code
**/

#ifndef N64_GBI_H
#define N64_GBI_H

#include <stdint.h>

/* use fast3d by default */
#if !defined(F3D_GBI) && !defined(F3DEX_GBI) && !defined(F3DEX_GBI_2)
# define F3D_GBI
#endif

/* commands for fast3d and f3dex */
#if defined(F3D_GBI) || defined(F3DEX_GBI)
# define G_SPNOOP		0x00
# define G_MTX			0x01
# define G_MOVEMEM		0x03
# define G_VTX			0x04
# define G_DL			0x06
# if defined(F3D_BETA)
#  define G_RDPHALF_2		0xB2
#  define G_RDPHALF_1		0xB3
#  define G_PERSPNORM		0xB4
# else
#  define G_RDPHALF_2		0xB3
#  define G_RDPHALF_1		0xB4
# endif
# define G_LINE3D		0xB5
# define G_CLEARGEOMETRYMODE	0xB6
# define G_SETGEOMETRYMODE	0xB7
# define G_ENDDL		0xB8
# define G_SETOTHERMODE_L	0xB9
# define G_SETOTHERMODE_H	0xBA
# define G_TEXTURE		0xBB
# define G_MOVEWORD		0xBC
# define G_POPMTX		0xBD
# define G_CULLDL		0xBE
# define G_TRI1			0xBF
# define G_NOOP			0xC0
#endif

/* commands for f3dex */
#if defined(F3DEX_GBI)
# define G_LOAD_UCODE		0xAF
# define G_BRANCH_Z		0xB0
# define G_TRI2			0xB1
# if !defined(F3D_BETA)
#  define G_MODIFYVTX		0xB2
# endif
#endif

/* commands for f3dex2 */
#if defined(F3DEX_GBI_2)
# define G_NOOP			0x00
# define G_VTX			0x01
# define G_MODIFYVTX		0x02
# define G_CULLDL		0x03
# define G_BRANCH_Z		0x04
# define G_TRI1			0x05
# define G_TRI2			0x06
# define G_QUAD			0x07
# define G_LINE3D		0x08
# define G_SPECIAL_3		0xD3
# define G_SPECIAL_2		0xD4
# define G_SPECIAL_1		0xD5
# define G_DMA_IO		0xD6
# define G_TEXTURE		0xD7
# define G_POPMTX		0xD8
# define G_GEOMETRYMODE		0xD9
# define G_MTX			0xDA
# define G_MOVEWORD		0xDB
# define G_MOVEMEM		0xDC
# define G_LOAD_UCODE		0xDD
# define G_DL			0xDE
# define G_ENDDL		0xDF
# define G_SPNOOP		0xE0
# define G_RDPHALF_1		0xE1
# define G_SETOTHERMODE_L	0xE2
# define G_SETOTHERMODE_H	0xE3
# define G_RDPHALF_2		0xF1
#endif

/* rdp commands */
#define G_TEXRECT		0xE4
#define G_TEXRECTFLIP		0xE5
#define G_RDPLOADSYNC		0xE6
#define G_RDPPIPESYNC		0xE7
#define G_RDPTILESYNC		0xE8
#define G_RDPFULLSYNC		0xE9
#define G_SETKEYGB		0xEA
#define G_SETKEYR		0xEB
#define G_SETCONVERT		0xEC
#define G_SETSCISSOR		0xED
#define G_SETPRIMDEPTH		0xEE
#define G_RDPSETOTHERMODE	0xEF
#define G_LOADTLUT		0xF0
#define G_SETTILESIZE		0xF2
#define G_LOADBLOCK		0xF3
#define G_LOADTILE		0xF4
#define G_SETTILE		0xF5
#define G_FILLRECT		0xF6
#define G_SETFILLCOLOR		0xF7
#define G_SETFOGCOLOR		0xF8
#define G_SETBLENDCOLOR		0xF9
#define G_SETPRIMCOLOR		0xFA
#define G_SETENVCOLOR		0xFB
#define G_SETCOMBINE		0xFC
#define G_SETTIMG		0xFD
#define G_SETZIMG		0xFE
#define G_SETCIMG		0xFF

/* commands for s2dex */
#if defined(F3DEX_GBI)
# define G_BG_1CYC		0x01
# define G_BG_COPY		0x02
# define G_OBJ_RECTANGLE	0x03
# define G_OBJ_SPRITE		0x04
# define G_OBJ_MOVEMEM		0x05
# define G_SELECT_DL		0xB0
# define G_OBJ_RENDERMODE	0xB1
# define G_OBJ_RECTANGLE_R	0xB2
# define G_OBJ_LOADTXTR		0xC1
# define G_OBJ_LDTX_SPRITE	0xC2
# define G_OBJ_LDTX_RECT	0xC3
# define G_OBJ_LDTX_RECT_R	0xC4
#endif

/* commands for s2dex2 */
#if defined(F3DEX_GBI_2)
# define G_OBJ_RECTANGLE	0x01
# define G_OBJ_SPRITE		0x02
# define G_SELECT_DL		0x04
# define G_OBJ_LOADTXTR		0x05
# define G_OBJ_LDTX_SPRITE	0x06
# define G_OBJ_LDTX_RECT	0x07
# define G_OBJ_LDTX_RECT_R	0x08
# define G_BG_1CYC		0x09
# define G_BG_COPY		0x0A
# define G_OBJ_RENDERMODE	0x0B
# define G_OBJ_RECTANGLE_R	0xDA
# define G_OBJ_MOVEMEM		0xDC
#endif

/* commands for s2dex and s2dex2 */
#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
# define G_RDPHALF_0		0xE4
#endif

/* image formats */
#define G_IM_FMT_RGBA		0
#define G_IM_FMT_YUV		1
#define G_IM_FMT_CI		2
#define G_IM_FMT_IA		3
#define G_IM_FMT_I		4
#define G_IM_SIZ_4b		0
#define G_IM_SIZ_8b		1
#define G_IM_SIZ_16b		2
#define G_IM_SIZ_32b		3

/* texture settings */
#define G_TX_NOMIRROR		(gI_(0b0) << 0)
#define G_TX_MIRROR		(gI_(0b1) << 0)
#define G_TX_WRAP		(gI_(0b0) << 1)
#define G_TX_CLAMP		(gI_(0b1) << 1)
#define G_TX_NOMASK		gI_(0)
#define G_TX_NOLOD		gI_(0)
#define G_OFF			gI_(0)
#define G_ON			gI_(1)

/* tile indices */
#define G_TX_LOADTILE		7
#define G_TX_RENDERTILE		0

/* loadblock constants */
#define G_TX_DXT_FRAC		11
#define G_TX_LDBLK_MAX_TXL	2047

/* geometry mode */
#define G_ZBUFFER		(gI_(0b1) << 0)
#define G_SHADE			(gI_(0b1) << 2)
#define G_CULL_BOTH		(G_CULL_FRONT | G_CULL_BACK)
#define G_FOG			(gI_(0b1) << 16)
#define G_LIGHTING		(gI_(0b1) << 17)
#define G_TEXTURE_GEN		(gI_(0b1) << 18)
#define G_TEXTURE_GEN_LINEAR	(gI_(0b1) << 19)
#define G_LOD			(gI_(0b1) << 20)

/* geometry mode for fast3d */
#if defined(F3D_GBI)
# define G_CLIPPING		(gI_(0b0) << 0)
#endif

/* geometry mode for fast3d and f3dex */
#if defined(F3D_GBI) || defined(F3DEX_GBI)
# define G_TEXTURE_ENABLE	(gI_(0b1) << 1)
# define G_SHADING_SMOOTH	(gI_(0b1) << 9)
# define G_CULL_FRONT		(gI_(0b1) << 12)
# define G_CULL_BACK		(gI_(0b1) << 13)
#endif

/* geometry mode for f3dex and f3dex2 */
#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
# define G_CLIPPING		(gI_(0b1) << 23)
#endif

/* geometry mode for f3dex2 */
#if defined(F3DEX_GBI_2)
# define G_TEXTURE_ENABLE	(gI_(0b0) << 0)
# define G_CULL_FRONT		(gI_(0b1) << 9)
# define G_CULL_BACK		(gI_(0b1) << 10)
# define G_SHADING_SMOOTH	(gI_(0b1) << 21)
#endif

/* othermode lo */
#define G_MDSFT_ALPHACOMPARE	0
#define G_MDSFT_ZSRCSEL		2
#define G_MDSFT_RENDERMODE	3
#define G_MDSFT_BLENDER		16
#define G_MDSIZ_ALPHACOMPARE	2
#define G_MDSIZ_ZSRCSEL		1
#define G_MDSIZ_RENDERMODE	29
#define G_MDSIZ_BLENDER		13

#define G_AC_NONE		(gI_(0b00) << G_MDSFT_ALPHACOMPARE)
#define G_AC_THRESHOLD		(gI_(0b01) << G_MDSFT_ALPHACOMPARE)
#define G_AC_DITHER		(gI_(0b11) << G_MDSFT_ALPHACOMPARE)
#define G_ZS_PIXEL		(gI_(0b0) << G_MDSFT_ZSRCSEL)
#define G_ZS_PRIM		(gI_(0b1) << G_MDSFT_ZSRCSEL)
#define AA_EN			(gI_(0b1) << (G_MDSFT_RENDERMODE + 0))
#define Z_CMP			(gI_(0b1) << (G_MDSFT_RENDERMODE + 1))
#define Z_UPD			(gI_(0b1) << (G_MDSFT_RENDERMODE + 2))
#define IM_RD			(gI_(0b1) << (G_MDSFT_RENDERMODE + 3))
#define CLR_ON_CVG		(gI_(0b1) << (G_MDSFT_RENDERMODE + 4))
#define CVG_DST_CLAMP		(gI_(0b00) << (G_MDSFT_RENDERMODE + 5))
#define CVG_DST_WRAP		(gI_(0b01) << (G_MDSFT_RENDERMODE + 5))
#define CVG_DST_FULL		(gI_(0b10) << (G_MDSFT_RENDERMODE + 5))
#define CVG_DST_SAVE		(gI_(0b11) << (G_MDSFT_RENDERMODE + 5))
#define ZMODE_OPA		(gI_(0b00) << (G_MDSFT_RENDERMODE + 7))
#define ZMODE_INTER		(gI_(0b01) << (G_MDSFT_RENDERMODE + 7))
#define ZMODE_XLU		(gI_(0b10) << (G_MDSFT_RENDERMODE + 7))
#define ZMODE_DEC		(gI_(0b11) << (G_MDSFT_RENDERMODE + 7))
#define CVG_X_ALPHA		(gI_(0b1) << (G_MDSFT_RENDERMODE + 9))
#define ALPHA_CVG_SEL		(gI_(0b1) << (G_MDSFT_RENDERMODE + 10))
#define FORCE_BL		(gI_(0b1) << (G_MDSFT_RENDERMODE + 11))

#define G_BL_1MA		gI_(0b00)
#define G_BL_1			gI_(0b10)
#define G_BL_0			gI_(0b11)
#define G_BL_CLR_IN		gI_(0b00)
#define G_BL_CLR_MEM		gI_(0b01)
#define G_BL_CLR_BL		gI_(0b10)
#define G_BL_CLR_FOG		gI_(0b11)
#define G_BL_A_IN		gI_(0b00)
#define G_BL_A_FOG		gI_(0b01)
#define G_BL_A_MEM		gI_(0b01)
#define G_BL_A_SHADE		gI_(0b10)

#define GBL_c1(p, a, m, b) \
	( \
		gF_(p, 2, 30) | \
		gF_(a, 2, 26) | \
		gF_(m, 2, 22) | \
		gF_(b, 2, 18) \
	)
#define GBL_c2(p, a, m, b) \
	( \
		gF_(p, 2, 28) | \
		gF_(a, 2, 24) | \
		gF_(m, 2, 20) | \
		gF_(b, 2, 16) \
	)

/* render modes */
#define G_RM_OPA_SURF \
	( \
		CVG_DST_CLAMP | ZMODE_OPA | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1) \
	)
#define G_RM_OPA_SURF2 \
	( \
		CVG_DST_CLAMP | ZMODE_OPA | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1) \
	)
#define G_RM_AA_OPA_SURF \
	( \
		AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_AA_OPA_SURF2 \
	( \
		AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_RA_OPA_SURF \
	( \
		AA_EN | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_RA_OPA_SURF2 \
	( \
		AA_EN | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_ZB_OPA_SURF \
	( \
		Z_CMP | Z_UPD | CVG_DST_FULL | ZMODE_OPA | ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_ZB_OPA_SURF2 \
	( \
		Z_CMP | Z_UPD | CVG_DST_FULL | ZMODE_OPA | ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_AA_ZB_OPA_SURF \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | \
		ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_AA_ZB_OPA_SURF2 \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | \
		ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_RA_ZB_OPA_SURF \
	( \
		AA_EN | Z_CMP | Z_UPD | CVG_DST_CLAMP | ZMODE_OPA | \
		ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_RA_ZB_OPA_SURF2 \
	( \
		AA_EN | Z_CMP | Z_UPD | CVG_DST_CLAMP | ZMODE_OPA | \
		ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)

#define G_RM_XLU_SURF \
	( \
		IM_RD | CVG_DST_FULL | ZMODE_OPA | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_XLU_SURF2 \
	( \
		IM_RD | CVG_DST_FULL | ZMODE_OPA | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_XLU_SURF \
	( \
		AA_EN | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_OPA | \
		FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_XLU_SURF2 \
	( \
		AA_EN | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | ZMODE_OPA | \
		FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_ZB_XLU_SURF \
	( \
		Z_CMP | IM_RD | CVG_DST_FULL | ZMODE_XLU | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_ZB_XLU_SURF2 \
	( \
		Z_CMP | IM_RD | CVG_DST_FULL | ZMODE_XLU | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_XLU_SURF \
	( \
		AA_EN | Z_CMP | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | \
		ZMODE_XLU | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_XLU_SURF2 \
	( \
		AA_EN | Z_CMP | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | \
		ZMODE_XLU | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)

#define G_RM_ZB_OPA_DECAL \
	( \
		Z_CMP | CVG_DST_FULL | ZMODE_DEC | ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_ZB_OPA_DECAL2 \
	( \
		Z_CMP | CVG_DST_FULL | ZMODE_DEC | ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_AA_ZB_OPA_DECAL \
	( \
		AA_EN | Z_CMP | IM_RD | CVG_DST_WRAP | ZMODE_DEC | \
		ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_AA_ZB_OPA_DECAL2 \
	( \
		AA_EN | Z_CMP | IM_RD | CVG_DST_WRAP | ZMODE_DEC | \
		ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_RA_ZB_OPA_DECAL \
	( \
		AA_EN | Z_CMP | CVG_DST_WRAP | ZMODE_DEC | ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_RA_ZB_OPA_DECAL2 \
	( \
		AA_EN | Z_CMP | CVG_DST_WRAP | ZMODE_DEC | ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)

#define G_RM_ZB_XLU_DECAL \
	( \
		Z_CMP | IM_RD | CVG_DST_FULL | ZMODE_DEC | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_ZB_XLU_DECAL2 \
	( \
		Z_CMP | IM_RD | CVG_DST_FULL | ZMODE_DEC | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_XLU_DECAL \
	( \
		AA_EN | Z_CMP | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | \
		ZMODE_DEC | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_XLU_DECAL2 \
	( \
		AA_EN | Z_CMP | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | \
		ZMODE_DEC | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)

#define G_RM_AA_ZB_OPA_INTER \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_INTER | \
		ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_AA_ZB_OPA_INTER2 \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_INTER | \
		ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_RA_ZB_OPA_INTER \
	( \
		AA_EN | Z_CMP | Z_UPD | CVG_DST_CLAMP | ZMODE_INTER | \
		ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_RA_ZB_OPA_INTER2 \
	( \
		AA_EN | Z_CMP | Z_UPD | CVG_DST_CLAMP | ZMODE_INTER | \
		ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)

#define G_RM_AA_ZB_XLU_INTER \
	( \
		AA_EN | Z_CMP | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | \
		ZMODE_INTER | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_XLU_INTER2 \
	( \
		AA_EN | Z_CMP | IM_RD | CLR_ON_CVG | CVG_DST_WRAP | \
		ZMODE_INTER | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)

#define G_RM_AA_XLU_LINE \
	( \
		AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | \
		ALPHA_CVG_SEL | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_XLU_LINE2 \
	( \
		AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | \
		ALPHA_CVG_SEL | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_XLU_LINE \
	( \
		AA_EN | Z_CMP | IM_RD | CVG_DST_CLAMP | ZMODE_XLU | \
		CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_XLU_LINE2 \
	( \
		AA_EN | Z_CMP | IM_RD | CVG_DST_CLAMP | ZMODE_XLU | \
		CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)

#define G_RM_AA_DEC_LINE \
	( \
		AA_EN | IM_RD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA | \
		ALPHA_CVG_SEL | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_DEC_LINE2 \
	( \
		AA_EN | IM_RD | CVG_DST_FULL | ZMODE_OPA | CVG_X_ALPHA | \
		ALPHA_CVG_SEL | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_DEC_LINE \
	( \
		AA_EN | Z_CMP | IM_RD | CVG_DST_SAVE | ZMODE_DEC | \
		CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_DEC_LINE2 \
	( \
		AA_EN | Z_CMP | IM_RD | CVG_DST_SAVE | ZMODE_DEC | \
		CVG_X_ALPHA | ALPHA_CVG_SEL | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)


#define G_RM_TEX_EDGE \
	( \
		AA_EN | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | \
		ALPHA_CVG_SEL | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1) \
	)
#define G_RM_TEX_EDGE2 \
	( \
		AA_EN | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | \
		ALPHA_CVG_SEL | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1) \
	)
#define G_RM_AA_TEX_EDGE \
	( \
		AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | \
		ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_AA_TEX_EDGE2 \
	( \
		AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | \
		ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_AA_ZB_TEX_EDGE \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | \
		CVG_X_ALPHA | ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_AA_ZB_TEX_EDGE2 \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | \
		CVG_X_ALPHA | ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)

#define G_RM_AA_ZB_TEX_INTER \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_INTER | \
		CVG_X_ALPHA | ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_AA_ZB_TEX_INTER2 \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_INTER | \
		CVG_X_ALPHA | ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)

#define G_RM_AA_SUB_SURF \
	( \
		AA_EN | IM_RD | CVG_DST_FULL | ZMODE_OPA | ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_AA_SUB_SURF2 \
	( \
		AA_EN | IM_RD | CVG_DST_FULL | ZMODE_OPA | ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_AA_ZB_SUB_SURF \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_FULL | ZMODE_OPA | \
		ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)
#define G_RM_AA_ZB_SUB_SURF2 \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_FULL | ZMODE_OPA | \
		ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_A_MEM) \
	)

#define G_RM_PCL_SURF \
	( \
		G_AC_DITHER | CVG_DST_FULL | ZMODE_OPA | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1) \
	)
#define G_RM_PCL_SURF2 \
	( \
		G_AC_DITHER | CVG_DST_FULL | ZMODE_OPA | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1) \
	)
#define G_RM_AA_PCL_SURF \
	( \
		G_AC_DITHER | AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_PCL_SURF2 \
	( \
		G_AC_DITHER | AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_ZB_PCL_SURF \
	( \
		G_AC_DITHER | Z_CMP | Z_UPD | CVG_DST_FULL | ZMODE_OPA | \
		GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1) \
	)
#define G_RM_ZB_PCL_SURF2 \
	( \
		G_AC_DITHER | Z_CMP | Z_UPD | CVG_DST_FULL | ZMODE_OPA | \
		GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1) \
	)
#define G_RM_AA_ZB_PCL_SURF \
	( \
		G_AC_DITHER | AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | \
		ZMODE_OPA | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_PCL_SURF2 \
	( \
		G_AC_DITHER | AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | \
		ZMODE_OPA | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)

#define G_RM_AA_OPA_TERR \
	( \
		AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_OPA_TERR2 \
	( \
		AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_OPA_TERR \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | \
		ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_OPA_TERR2 \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | \
		ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)

#define G_RM_AA_TEX_TERR \
	( \
		AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | \
		ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_TEX_TERR2 \
	( \
		AA_EN | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | \
		ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_TEX_TERR \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | \
		CVG_X_ALPHA | ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_TEX_TERR2 \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_CLAMP | ZMODE_OPA | \
		CVG_X_ALPHA | ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)

#define G_RM_AA_SUB_TERR \
	( \
		AA_EN | IM_RD | CVG_DST_FULL | ZMODE_OPA | ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_SUB_TERR2 \
	( \
		AA_EN | IM_RD | CVG_DST_FULL | ZMODE_OPA | ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_SUB_TERR \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_FULL | ZMODE_OPA | \
		ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_ZB_SUB_TERR2 \
	( \
		AA_EN | Z_CMP | Z_UPD | IM_RD | CVG_DST_FULL | ZMODE_OPA | \
		ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)

#define G_RM_CLD_SURF \
	( \
		IM_RD | CVG_DST_SAVE | ZMODE_OPA | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_CLD_SURF2 \
	( \
		IM_RD | CVG_DST_SAVE | ZMODE_OPA | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_ZB_CLD_SURF \
	( \
		Z_CMP | IM_RD | CVG_DST_SAVE | ZMODE_XLU | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_ZB_CLD_SURF2 \
	( \
		Z_CMP | IM_RD | CVG_DST_SAVE | ZMODE_XLU | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)

#define G_RM_ZB_OVL_SURF \
	( \
		Z_CMP | IM_RD | CVG_DST_SAVE | ZMODE_DEC | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_ZB_OVL_SURF2 \
	( \
		Z_CMP | IM_RD | CVG_DST_SAVE | ZMODE_DEC | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)

#define G_RM_ADD \
	( \
		IM_RD | CVG_DST_SAVE | ZMODE_OPA | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_FOG, G_BL_CLR_MEM, G_BL_1) \
	)
#define G_RM_ADD2 \
	( \
		IM_RD | CVG_DST_SAVE | ZMODE_OPA | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_FOG, G_BL_CLR_MEM, G_BL_1) \
	)

#define G_RM_FOG_SHADE_A \
	GBL_c1(G_BL_CLR_FOG, G_BL_A_SHADE, G_BL_CLR_IN, G_BL_1MA)

#define G_RM_FOG_PRIM_A \
	GBL_c1(G_BL_CLR_FOG, G_BL_A_FOG, G_BL_CLR_IN, G_BL_1MA)

#define G_RM_PASS \
	GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1)

#define G_RM_VISCVG \
	( \
		IM_RD | FORCE_BL | \
		GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_BL, G_BL_A_MEM) \
	)
#define G_RM_VISCVG2 \
	( \
		IM_RD | FORCE_BL | \
		GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_BL, G_BL_A_MEM) \
	)

#define G_RM_OPA_CI \
	( \
		CVG_DST_CLAMP | ZMODE_OPA | \
		GBL_c1(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1) \
	)
#define G_RM_OPA_CI2 \
	( \
		CVG_DST_CLAMP | ZMODE_OPA | \
		GBL_c2(G_BL_CLR_IN, G_BL_0, G_BL_CLR_IN, G_BL_1) \
	)

#define G_RM_NOOP		GBL_c1(0, 0, 0, 0)
#define G_RM_NOOP2		GBL_c2(0, 0, 0, 0)

#define G_RM_SPRITE		G_RM_OPA_SURF
#define G_RM_SPRITE2		G_RM_OPA_SURF2
#define G_RM_RA_SPRITE \
	( \
		AA_EN | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | \
		ALPHA_CVG_SEL | \
		GBL_c1(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_RA_SPRITE2 \
	( \
		AA_EN | CVG_DST_CLAMP | ZMODE_OPA | CVG_X_ALPHA | \
		ALPHA_CVG_SEL | \
		GBL_c2(G_BL_CLR_IN, G_BL_A_IN, G_BL_CLR_MEM, G_BL_1MA) \
	)
#define G_RM_AA_SPRITE		G_RM_AA_TEX_TERR
#define G_RM_AA_SPRITE2		G_RM_AA_TEX_TERR2
#define G_RM_XLU_SPRITE		G_RM_XLU_SURF
#define G_RM_XLU_SPRITE2	G_RM_XLU_SURF2
#define G_RM_AA_XLU_SPRITE	G_RM_AA_XLU_SURF
#define G_RM_AA_XLU_SPRITE2	G_RM_AA_XLU_SURF2

#define G_OBJRM_NOTXCLAMP	(gI_(0b1) << 0)
#define G_OBJRM_XLU		(gI_(0b1) << 1)
#define G_OBJRM_ANTIALIAS	(gI_(0b1) << 2)
#define G_OBJRM_BILERP		(gI_(0b1) << 3)
#define G_OBJRM_SHRINKSIZE_1	(gI_(0b1) << 4)
#define G_OBJRM_SHRINKSIZE_2	(gI_(0b1) << 5)
#define G_OBJRM_WIDEN		(gI_(0b1) << 6)

/* othermode hi */
#define G_MDSFT_ALPHADITHER	4
#define G_MDSFT_RGBDITHER	6
#define G_MDSFT_COMBKEY		8
#define G_MDSFT_TEXTCONV	9
#define G_MDSFT_TEXTFILT	12
#define G_MDSFT_TEXTLUT		14
#define G_MDSFT_TEXTLOD		16
#define G_MDSFT_TEXTDETAIL	17
#define G_MDSFT_TEXTPERSP	19
#define G_MDSFT_CYCLETYPE	20
#define G_MDSFT_PIPELINE	23
#define G_MDSIZ_ALPHADITHER	2
#define G_MDSIZ_RGBDITHER	2
#define G_MDSIZ_COMBKEY		1
#define G_MDSIZ_TEXTCONV	3
#define G_MDSIZ_TEXTFILT	2
#define G_MDSIZ_TEXTLUT		2
#define G_MDSIZ_TEXTLOD		1
#define G_MDSIZ_TEXTDETAIL	2
#define G_MDSIZ_TEXTPERSP	1
#define G_MDSIZ_CYCLETYPE	2
#define G_MDSIZ_PIPELINE	1

#define G_AD_PATTERN		(gI_(0b00) << G_MDSFT_ALPHADITHER)
#define G_AD_NOTPATTERN		(gI_(0b01) << G_MDSFT_ALPHADITHER)
#define G_AD_NOISE		(gI_(0b10) << G_MDSFT_ALPHADITHER)
#define G_AD_DISABLE		(gI_(0b11) << G_MDSFT_ALPHADITHER)
#define G_CD_MAGICSQ		(gI_(0b00) << G_MDSFT_RGBDITHER)
#define G_CD_BAYER		(gI_(0b01) << G_MDSFT_RGBDITHER)
#define G_CD_NOISE		(gI_(0b10) << G_MDSFT_RGBDITHER)
#define G_CD_DISABLE		(gI_(0b11) << G_MDSFT_RGBDITHER)
#define G_CD_ENABLE		(gI_(0b10) << G_MDSFT_RGBDITHER)
#define G_CK_NONE		(gI_(0b0) << G_MDSFT_COMBKEY)
#define G_CK_KEY		(gI_(0b1) << G_MDSFT_COMBKEY)
#define G_TC_CONV		(gI_(0b000) << G_MDSFT_TEXTCONV)
#define G_TC_FILTCONV		(gI_(0b101) << G_MDSFT_TEXTCONV)
#define G_TC_FILT		(gI_(0b110) << G_MDSFT_TEXTCONV)
#define G_TF_POINT		(gI_(0b00) << G_MDSFT_TEXTFILT)
#define G_TF_BILERP		(gI_(0b10) << G_MDSFT_TEXTFILT)
#define G_TF_AVERAGE		(gI_(0b11) << G_MDSFT_TEXTFILT)
#define G_TT_NONE		(gI_(0b00) << G_MDSFT_TEXTLUT)
#define G_TT_RGBA16		(gI_(0b10) << G_MDSFT_TEXTLUT)
#define G_TT_IA16		(gI_(0b11) << G_MDSFT_TEXTLUT)
#define G_TL_TILE		(gI_(0b0) << G_MDSFT_TEXTLOD)
#define G_TL_LOD		(gI_(0b1) << G_MDSFT_TEXTLOD)
#define G_TD_CLAMP		(gI_(0b00) << G_MDSFT_TEXTDETAIL)
#define G_TD_SHARPEN		(gI_(0b01) << G_MDSFT_TEXTDETAIL)
#define G_TD_DETAIL		(gI_(0b10) << G_MDSFT_TEXTDETAIL)
#define G_TP_NONE		(gI_(0b0) << G_MDSFT_TEXTPERSP)
#define G_TP_PERSP		(gI_(0b1) << G_MDSFT_TEXTPERSP)
#define G_CYC_1CYCLE		(gI_(0b00) << G_MDSFT_CYCLETYPE)
#define G_CYC_2CYCLE		(gI_(0b01) << G_MDSFT_CYCLETYPE)
#define G_CYC_COPY		(gI_(0b10) << G_MDSFT_CYCLETYPE)
#define G_CYC_FILL		(gI_(0b11) << G_MDSFT_CYCLETYPE)
#define G_PM_NPRIMITIVE		(gI_(0b0) << G_MDSFT_PIPELINE)
#define G_PM_1PRIMITIVE		(gI_(0b1) << G_MDSFT_PIPELINE)

/* color conversion constants */
#define G_CV_K0			(175)
#define G_CV_K1			(-43)
#define G_CV_K2			(-89)
#define G_CV_K3			(222)
#define G_CV_K4			(114)
#define G_CV_K5			(42)

/* color combiner */
#define G_CCMUX_COMBINED	0
#define G_CCMUX_TEXEL0		1
#define G_CCMUX_TEXEL1		2
#define G_CCMUX_PRIMITIVE	3
#define G_CCMUX_SHADE		4
#define G_CCMUX_ENVIRONMENT	5
#define G_CCMUX_1		6
#define G_CCMUX_NOISE		7
#define G_CCMUX_0		31
#define G_CCMUX_CENTER		6
#define G_CCMUX_K4		7
#define G_CCMUX_SCALE		6
#define G_CCMUX_COMBINED_ALPHA	7
#define G_CCMUX_TEXEL0_ALPHA	8
#define G_CCMUX_TEXEL1_ALPHA	9
#define G_CCMUX_PRIMITIVE_ALPHA	10
#define G_CCMUX_SHADE_ALPHA	11
#define G_CCMUX_ENV_ALPHA	12
#define G_CCMUX_LOD_FRACTION	13
#define G_CCMUX_PRIM_LOD_FRAC	14
#define G_CCMUX_K5		15
#define G_ACMUX_COMBINED	0
#define G_ACMUX_TEXEL0		1
#define G_ACMUX_TEXEL1		2
#define G_ACMUX_PRIMITIVE	3
#define G_ACMUX_SHADE		4
#define G_ACMUX_ENVIRONMENT	5
#define G_ACMUX_1		6
#define G_ACMUX_0		7
#define G_ACMUX_LOD_FRACTION	0
#define G_ACMUX_PRIM_LOD_FRAC	6

/*
 * combine modes
 *	( A		- B )		* C		+ D
*/
#define G_CC_MODULATEI \
	TEXEL0,		0,		SHADE,		0, \
	0,		0,		0,		SHADE
#define G_CC_MODULATEIA \
	TEXEL0,		0,		SHADE,		0, \
	TEXEL0,		0,		SHADE,		0
#define G_CC_MODULATEIDECALA \
	TEXEL0,		0,		SHADE,		0, \
	0,		0,		0,		TEXEL0
#define G_CC_MODULATERGB \
	G_CC_MODULATEI
#define G_CC_MODULATERGBA \
	G_CC_MODULATEIA
#define G_CC_MODULATERGBDECALA \
	G_CC_MODULATEIDECALA
#define G_CC_MODULATEI_PRIM \
	TEXEL0,		0,		PRIMITIVE,	0, \
	0,		0,		0,		PRIMITIVE
#define G_CC_MODULATEIA_PRIM \
	TEXEL0,		0,		PRIMITIVE,	0, \
	TEXEL0,		0,		PRIMITIVE,	0
#define G_CC_MODULATEIDECALA_PRIM \
	TEXEL0,		0,		PRIMITIVE,	0, \
	0,		0,		0,		TEXEL0
#define G_CC_MODULATERGB_PRIM \
	G_CC_MODULATEI_PRIM
#define G_CC_MODULATERGBA_PRIM \
	G_CC_MODULATEIA_PRIM
#define G_CC_MODULATERGBDECALA_PRIM \
	G_CC_MODULATEIDECALA_PRIM
#define G_CC_DECALRGB \
	0,		0,		0,		TEXEL0, \
	0,		0,		0,		SHADE
#define G_CC_DECALRGBA \
	0,		0,		0,		TEXEL0, \
	0,		0,		0,		TEXEL0
#define G_CC_BLENDI \
	ENVIRONMENT,	SHADE,		TEXEL0,		SHADE, \
	0,		0,		0,		SHADE
#define G_CC_BLENDIA \
	ENVIRONMENT,	SHADE,		TEXEL0,		SHADE, \
	TEXEL0,		0,		SHADE,		0
#define G_CC_BLENDIDECALA \
	ENVIRONMENT,	SHADE,		TEXEL0,		SHADE, \
	0,		0,		0,		TEXEL0
#define G_CC_BLENDRGBA \
	TEXEL0,		SHADE,		TEXEL0_ALPHA,	SHADE, \
	0,		0,		0,		SHADE
#define G_CC_BLENDRGBDECALA \
	TEXEL0,		SHADE,		TEXEL0_ALPHA,	SHADE, \
	0,		0,		0,		TEXEL0
#define G_CC_REFLECTRGB \
	ENVIRONMENT,	0,		TEXEL0,		SHADE, \
	0,		0,		0,		SHADE
#define G_CC_REFLECTRGBDECALA \
	ENVIRONMENT,	0,		TEXEL0,		SHADE, \
	0,		0,		0,		TEXEL0
#define G_CC_HILITERGB \
	PRIMITIVE,	SHADE,		TEXEL0,		SHADE, \
	0,		0,		0,		SHADE
#define G_CC_HILITERGBA \
	PRIMITIVE,	SHADE,		TEXEL0,		SHADE, \
	PRIMITIVE,	SHADE,		TEXEL0,		SHADE
#define G_CC_HILITERGBDECALA \
	PRIMITIVE,	SHADE,		TEXEL0,		SHADE, \
	0,		0,		0,		TEXEL0
#define G_CC_1CYUV2RGB \
	TEXEL0,		K4,		K5,		TEXEL0, \
	0,		0,		0,		SHADE
#define G_CC_PRIMITIVE \
	0,		0,		0,		PRIMITIVE, \
	0,		0,		0,		PRIMITIVE
#define G_CC_SHADE \
	0,		0,		0,		SHADE, \
	0,		0,		0,		SHADE
#define G_CC_ADDRGB \
	1,		0,		TEXEL0,		SHADE, \
	0,		0,		0,		SHADE
#define G_CC_ADDRGBDECALA \
	1,		0,		TEXEL0,		SHADE, \
	0,		0,		0,		TEXEL0
#define G_CC_SHADEDECALA \
	0,		0,		0,		SHADE, \
	0,		0,		0,		TEXEL0
#define G_CC_BLENDPE \
	PRIMITIVE,	ENVIRONMENT,	TEXEL0,		ENVIRONMENT, \
	TEXEL0,		0,		SHADE,		0
#define G_CC_BLENDPEDECALA \
	PRIMITIVE,	ENVIRONMENT,	TEXEL0,		ENVIRONMENT, \
	0,		0,		0,		TEXEL0
#define G_CC_TRILERP \
	TEXEL1,		TEXEL0,		LOD_FRACTION,	TEXEL0, \
	TEXEL1,		TEXEL0,		LOD_FRACTION,	TEXEL0
#define G_CC_TEMPLERP \
	TEXEL1,		TEXEL0,		PRIM_LOD_FRAC,	TEXEL0, \
	TEXEL1,		TEXEL0,		PRIM_LOD_FRAC,	TEXEL0
#define G_CC_INTERFERENCE \
	TEXEL0,		0,		TEXEL1,		0, \
	TEXEL0,		0,		TEXEL1,		0
#define _G_CC_BLENDPE \
	ENVIRONMENT,	PRIMITIVE,	TEXEL0,		PRIMITIVE, \
	TEXEL0,		0,		SHADE,		0
#define _G_CC_BLENDPEDECALA \
	ENVIRONMENT,	PRIMITIVE,	TEXEL0,		PRIMITIVE, \
	0,		0,		0,		TEXEL0
#define _G_CC_SPARSEST \
	PRIMITIVE,	TEXEL0,		LOD_FRACTION,	TEXEL0, \
	PRIMITIVE,	TEXEL0,		LOD_FRACTION,	TEXEL0
#define _G_CC_TWOCOLORTEX \
	PRIMITIVE,	SHADE,		TEXEL0,		SHADE, \
	0,		0,		0,		SHADE
#define G_CC_MODULATEI2 \
	COMBINED,	0,		SHADE,		0, \
	0,		0,		0,		SHADE
#define G_CC_MODULATEIA2 \
	COMBINED,	0,		SHADE,		0, \
	COMBINED,	0,		SHADE,		0
#define G_CC_MODULATERGB2 \
	G_CC_MODULATEI2
#define G_CC_MODULATERGBA2 \
	G_CC_MODULATEIA2
#define G_CC_MODULATEI_PRIM2 \
	COMBINED,	0,		PRIMITIVE,	0, \
	0,		0,		0,		PRIMITIVE
#define G_CC_MODULATEIA_PRIM2 \
	COMBINED,	0,		PRIMITIVE,	0, \
	COMBINED,	0,		PRIMITIVE,	0
#define G_CC_MODULATERGB_PRIM2 \
	G_CC_MODULATEI_PRIM2
#define G_CC_MODULATERGBA_PRIM2 \
	G_CC_MODULATEIA_PRIM2
#define G_CC_DECALRGB2 \
	0,		0,		0,		COMBINED, \
	0,		0,		0,		SHADE
#define G_CC_BLENDI2 \
	ENVIRONMENT,	SHADE,		COMBINED,	SHADE, \
	0,		0,		0,		SHADE
#define G_CC_BLENDIA2 \
	ENVIRONMENT,	SHADE,		COMBINED,	SHADE, \
	COMBINED,	0,		SHADE,		0
#define G_CC_HILITERGB2 \
	ENVIRONMENT,	COMBINED,	TEXEL0,		COMBINED, \
	0,		0,		0,		SHADE
#define G_CC_HILITERGBA2 \
	ENVIRONMENT,	COMBINED,	TEXEL0,		COMBINED, \
	ENVIRONMENT,	COMBINED,	TEXEL0,		COMBINED
#define G_CC_HILITERGBDECALA2 \
	ENVIRONMENT,	COMBINED,	TEXEL0,		COMBINED, \
	0,		0,		0,		TEXEL0
#define G_CC_HILITERGBPASSA2 \
	ENVIRONMENT,	COMBINED,	TEXEL0,		COMBINED, \
	0,		0,		0,		COMBINED
#define G_CC_CHROMA_KEY2 \
	TEXEL0,		CENTER,		SCALE,		0, \
	0,		0,		0,		0
#define G_CC_YUV2RGB \
	TEXEL1,		K4,		K5,		TEXEL1, \
	0,		0,		0,		0
#define G_CC_PASS2 \
	0,		0,		0,		COMBINED, \
	0,		0,		0,		COMBINED
#define G_CC_LERP(a0, b0, c0, d0, Aa0, Ab0, Ac0, Ad0, \
		a1, b1, c1, d1, Aa1, Ab1, Ac1, Ad1) \
	( \
		gFL_(G_CCMUX_##a0, 4, 52) | \
		gFL_(G_CCMUX_##c0, 5, 47) | \
		gFL_(G_ACMUX_##Aa0, 3, 44) | \
		gFL_(G_ACMUX_##Ac0, 3, 41) | \
		gFL_(G_CCMUX_##a1, 4, 37) | \
		gFL_(G_CCMUX_##c1, 5, 32) | \
		gFL_(G_CCMUX_##b0, 4, 28) | \
		gFL_(G_CCMUX_##b1, 4, 24) | \
		gFL_(G_ACMUX_##Aa1, 3, 21) | \
		gFL_(G_ACMUX_##Ac1, 3, 18) | \
		gFL_(G_CCMUX_##d0, 3, 15) | \
		gFL_(G_ACMUX_##Ab0, 3, 12) | \
		gFL_(G_ACMUX_##Ad0, 3, 9) | \
		gFL_(G_CCMUX_##d1, 3, 6) | \
		gFL_(G_ACMUX_##Ab1, 3, 3) | \
		gFL_(G_ACMUX_##Ad1, 3, 0) \
	)
#define G_CC_MODE(mode1, mode2)	G_CC_LERP(mode1, mode2)

/* scissor modes */
#define G_SC_NON_INTERLACE	gI_(0b00)
#define G_SC_EVEN_INTERLACE	gI_(0b10)
#define G_SC_ODD_INTERLACE	gI_(0b11)

/* display list branch flags */
#define G_DL_PUSH		gI_(0b0)
#define G_DL_NOPUSH		gI_(0b1)

/* conditional branching flags (f3dex and f3dex2) */
#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
# define G_BZ_PERSP		0
# define G_BZ_ORTHO		1
#endif

/* matrix params */
#define G_MTX_MUL		(gI_(0b0) << 1)
#define G_MTX_LOAD		(gI_(0b1) << 1)

/* matrix params for fast3d and f3dex */
#if defined(F3D_GBI) || defined(F3DEX_GBI)
# define G_MTX_MODELVIEW	(gI_(0b0) << 0)
# define G_MTX_PROJECTION	(gI_(0b1) << 0)
# define G_MTX_NOPUSH		(gI_(0b0) << 2)
# define G_MTX_PUSH		(gI_(0b1) << 2)
#endif

/* matrix params for f3dex2 */
#if defined(F3DEX_GBI_2)
# define G_MTX_NOPUSH		(gI_(0b0) << 0)
# define G_MTX_PUSH		(gI_(0b1) << 0)
# define G_MTX_MODELVIEW	(gI_(0b0) << 2)
# define G_MTX_PROJECTION	(gI_(0b1) << 2)
#endif

/* moveword indices */
#define G_MW_MATRIX		0
#define G_MW_NUMLIGHT		2
#define G_MW_CLIP		4
#define G_MW_SEGMENT		6
#define G_MW_FOG		8
#define G_MW_GENSTAT		8
#define G_MW_LIGHTCOL		10
#define G_MW_PERSPNORM		14

/* moveword indices for fast3d and f3dex */
#if defined(F3D_GBI) || defined(F3DEX_GBI)
# define G_MW_POINTS		12
#endif

/* moveword indices for f3dex2 */
#if defined(F3DEX_GBI_2)
# define G_MW_FORCEMTX		12
#endif

/* moveword offsets */
#define G_MWO_NUMLIGHT		gI_(0x00)
#define G_MWO_CLIP_RNX		gI_(0x04)
#define G_MWO_CLIP_RNY		gI_(0x0C)
#define G_MWO_CLIP_RPX		gI_(0x14)
#define G_MWO_CLIP_RPY		gI_(0x1C)
#define G_MWO_SEGMENT_0		gI_(0x00)
#define G_MWO_SEGMENT_1		gI_(0x04)
#define G_MWO_SEGMENT_2		gI_(0x08)
#define G_MWO_SEGMENT_3		gI_(0x0C)
#define G_MWO_SEGMENT_4		gI_(0x10)
#define G_MWO_SEGMENT_5		gI_(0x14)
#define G_MWO_SEGMENT_6		gI_(0x18)
#define G_MWO_SEGMENT_7		gI_(0x1C)
#define G_MWO_SEGMENT_8		gI_(0x20)
#define G_MWO_SEGMENT_9		gI_(0x24)
#define G_MWO_SEGMENT_A		gI_(0x28)
#define G_MWO_SEGMENT_B		gI_(0x2C)
#define G_MWO_SEGMENT_C		gI_(0x30)
#define G_MWO_SEGMENT_D		gI_(0x34)
#define G_MWO_SEGMENT_E		gI_(0x38)
#define G_MWO_SEGMENT_F		gI_(0x3C)
#define G_MWO_FOG		gI_(0x00)
#define G_MWO_aLIGHT_1		gI_(0x00)
#define G_MWO_bLIGHT_1		gI_(0x04)
#define G_MWO_MATRIX_XX_XY_I	gI_(0x00)
#define G_MWO_MATRIX_XZ_XW_I	gI_(0x04)
#define G_MWO_MATRIX_YX_YY_I	gI_(0x08)
#define G_MWO_MATRIX_YZ_YW_I	gI_(0x0C)
#define G_MWO_MATRIX_ZX_ZY_I	gI_(0x10)
#define G_MWO_MATRIX_ZZ_ZW_I	gI_(0x14)
#define G_MWO_MATRIX_WX_WY_I	gI_(0x18)
#define G_MWO_MATRIX_WZ_WW_I	gI_(0x1C)
#define G_MWO_MATRIX_XX_XY_F	gI_(0x20)
#define G_MWO_MATRIX_XZ_XW_F	gI_(0x24)
#define G_MWO_MATRIX_YX_YY_F	gI_(0x28)
#define G_MWO_MATRIX_YZ_YW_F	gI_(0x2C)
#define G_MWO_MATRIX_ZX_ZY_F	gI_(0x30)
#define G_MWO_MATRIX_ZZ_ZW_F	gI_(0x34)
#define G_MWO_MATRIX_WX_WY_F	gI_(0x38)
#define G_MWO_MATRIX_WZ_WW_F	gI_(0x3C)
#define G_MWO_POINT_RGBA	gI_(0x10)
#define G_MWO_POINT_ST		gI_(0x14)
#define G_MWO_POINT_XYSCREEN	gI_(0x18)
#define G_MWO_POINT_ZSCREEN	gI_(0x1C)

/* moveword offsets for fast3d and f3dex */
#if defined(F3D_GBI) || defined(F3DEX_GBI)
# define G_MWO_aLIGHT_2		gI_(0x20)
# define G_MWO_bLIGHT_2		gI_(0x24)
# define G_MWO_aLIGHT_3		gI_(0x40)
# define G_MWO_bLIGHT_3		gI_(0x44)
# define G_MWO_aLIGHT_4		gI_(0x60)
# define G_MWO_bLIGHT_4		gI_(0x64)
# define G_MWO_aLIGHT_5		gI_(0x80)
# define G_MWO_bLIGHT_5		gI_(0x84)
# define G_MWO_aLIGHT_6		gI_(0xA0)
# define G_MWO_bLIGHT_6		gI_(0xA4)
# define G_MWO_aLIGHT_7		gI_(0xC0)
# define G_MWO_bLIGHT_7		gI_(0xC4)
# define G_MWO_aLIGHT_8		gI_(0xE0)
# define G_MWO_bLIGHT_8		gI_(0xE4)
#endif

/* moveword offsets for f3dex2 */
#if defined(F3DEX_GBI_2)
# define G_MWO_aLIGHT_2		gI_(0x18)
# define G_MWO_bLIGHT_2		gI_(0x1C)
# define G_MWO_aLIGHT_3		gI_(0x30)
# define G_MWO_bLIGHT_3		gI_(0x34)
# define G_MWO_aLIGHT_4		gI_(0x48)
# define G_MWO_bLIGHT_4		gI_(0x4C)
# define G_MWO_aLIGHT_5		gI_(0x60)
# define G_MWO_bLIGHT_5		gI_(0x64)
# define G_MWO_aLIGHT_6		gI_(0x78)
# define G_MWO_bLIGHT_6		gI_(0x7C)
# define G_MWO_aLIGHT_7		gI_(0x90)
# define G_MWO_bLIGHT_7		gI_(0x94)
# define G_MWO_aLIGHT_8		gI_(0xA8)
# define G_MWO_bLIGHT_8		gI_(0xAC)
#endif

/* movemem params for fast3d and f3dex */
#if defined(F3D_GBI) || defined(F3DEX_GBI)
# define G_MV_VIEWPORT		128
# define G_MV_LOOKATY		130
# define G_MV_LOOKATX		132
# define G_MV_L0		134
# define G_MV_L1		136
# define G_MV_L2		138
# define G_MV_L3		140
# define G_MV_L4		142
# define G_MV_L5		144
# define G_MV_L6		146
# define G_MV_L7		148
# define G_MV_TXTATT		150
# define G_MV_MATRIX_2		152
# define G_MV_MATRIX_3		154
# define G_MV_MATRIX_4		156
# define G_MV_MATRIX_1		158
#endif

/* movemem params for f3dex2 */
#if defined(F3DEX_GBI_2)
# define G_MV_MMTX		2
# define G_MV_PMTX		6
# define G_MV_VIEWPORT		8
# define G_MV_LIGHT		10
# define G_MV_POINT		12
# define G_MV_MATRIX		14
# define G_MVO_LOOKATX		gI_(0 * 0x18)
# define G_MVO_LOOKATY		gI_(1 * 0x18)
# define G_MVO_L0		gI_(2 * 0x18)
# define G_MVO_L1		gI_(3 * 0x18)
# define G_MVO_L2		gI_(4 * 0x18)
# define G_MVO_L3		gI_(5 * 0x18)
# define G_MVO_L4		gI_(6 * 0x18)
# define G_MVO_L5		gI_(7 * 0x18)
# define G_MVO_L6		gI_(8 * 0x18)
# define G_MVO_L7		gI_(9 * 0x18)
#endif

/* frustum ratios */
#define FRUSTRATIO_1		gI_(1)
#define FRUSTRATIO_2		gI_(2)
#define FRUSTRATIO_3		gI_(3)
#define FRUSTRATIO_4		gI_(4)
#define FRUSTRATIO_5		gI_(5)
#define FRUSTRATIO_6		gI_(6)

/* light params */
#define NUMLIGHTS_0		1
#define NUMLIGHTS_1		1
#define NUMLIGHTS_2		2
#define NUMLIGHTS_3		3
#define NUMLIGHTS_4		4
#define NUMLIGHTS_5		5
#define NUMLIGHTS_6		6
#define NUMLIGHTS_7		7
#define LIGHT_1			1
#define LIGHT_2			2
#define LIGHT_3			3
#define LIGHT_4			4
#define LIGHT_5			5
#define LIGHT_6			6
#define LIGHT_7			7
#define LIGHT_8			8

/* light params for fast3d and f3dex */
#if defined(F3D_GBI) || defined(F3DEX_GBI)
# define NUML(n)		(((n) + 1) * 32 + 0x80000000)
#endif

/* light params for f3dex2 */
#if defined(F3DEX_GBI_2)
# define NUML(n)		((n) * 0x18)
#endif

/* background load types */
#define G_BGLT_LOADBLOCK	gI_(0x0033)
#define G_BGLT_LOADTILE		gI_(0xFFF4)

/* background flags */
#define G_BG_FLAG_FLIPS		(gI_(0b1) << 0)
#define G_BG_FLAG_FLIPT		(gI_(0b1) << 1)

/* object load types */
#define G_OBJLT_TXTRBLOCK	gI_(0x00001033)
#define G_OBJLT_TXTRTILE	gI_(0x00FC1034)
#define G_OBJLT_TLUT		gI_(0x00000030)

/* object flags */
#define G_OBJ_FLAG_FLIPS	(gI_(0b1) << 0)
#define G_OBJ_FLAG_FLIPT	(gI_(0b1) << 4)

/* color macros */
#define G_MAXZ			0x03FF
#define G_MAXFBZ		0x3FFF
#define GPACK_RGBA5551(r, g, b, a) \
	( \
		gF_(r, 5, 11) | \
		gF_(g, 5, 6) | \
		gF_(b, 5, 1) | \
		gF_(a, 1, 0) \
	)
#define GPACK_RGBA8888(r, g, b, a) \
	( \
		gF_(r, 8, 24) | \
		gF_(g, 8, 16) | \
		gF_(b, 8, 8) | \
		gF_(a, 8, 0) \
	)
#define GPACK_RGB24A8(rgb, a)	(gF_(rgb, 24, 8) | gF_(a, 8, 0))
#define GPACK_ZDZ(z, dz)	(gF_(z, 14, 2) | gF_(dz, 2, 0))

/* structure definition macros */
#define gdSPDefMtx(xx, xy, xz, xw, \
		yx, yy, yz, yw, \
		zx, zy, zz, zw, \
		wx, wy, wz, ww) \
	( \
		(Mtx) \
		{ \
			.i = \
			{ \
				(qs1616(xx) >> 16) & 0xFFFF, \
				(qs1616(xy) >> 16) & 0xFFFF, \
				(qs1616(xz) >> 16) & 0xFFFF, \
				(qs1616(xw) >> 16) & 0xFFFF, \
				(qs1616(yx) >> 16) & 0xFFFF, \
				(qs1616(yy) >> 16) & 0xFFFF, \
				(qs1616(yz) >> 16) & 0xFFFF, \
				(qs1616(yw) >> 16) & 0xFFFF, \
				(qs1616(zx) >> 16) & 0xFFFF, \
				(qs1616(zy) >> 16) & 0xFFFF, \
				(qs1616(zz) >> 16) & 0xFFFF, \
				(qs1616(zw) >> 16) & 0xFFFF, \
				(qs1616(wx) >> 16) & 0xFFFF, \
				(qs1616(wy) >> 16) & 0xFFFF, \
				(qs1616(wz) >> 16) & 0xFFFF, \
				(qs1616(ww) >> 16) & 0xFFFF, \
			}, \
			.f = \
			{ \
				qs1616(xx) & 0xFFFF, \
				qs1616(xy) & 0xFFFF, \
				qs1616(xz) & 0xFFFF, \
				qs1616(xw) & 0xFFFF, \
				qs1616(yx) & 0xFFFF, \
				qs1616(yy) & 0xFFFF, \
				qs1616(yz) & 0xFFFF, \
				qs1616(yw) & 0xFFFF, \
				qs1616(zx) & 0xFFFF, \
				qs1616(zy) & 0xFFFF, \
				qs1616(zz) & 0xFFFF, \
				qs1616(zw) & 0xFFFF, \
				qs1616(wx) & 0xFFFF, \
				qs1616(wy) & 0xFFFF, \
				qs1616(wz) & 0xFFFF, \
				qs1616(ww) & 0xFFFF, \
			} \
		} \
	)
#define gdSPDefLookAt(rx, ry, rz, ux, uy, uz) \
	( \
		(LookAt) \
		{ \
			.l[0].l = \
			{ \
				.col = {0, 0, 0}, \
				.colc = {0, 0, 0}, \
				.dir = {rx, ry, rz}, \
			}, \
			.l[1].l = \
			{ \
				.col = {0, 0x80, 0}, \
				.colc = {0, 0x80, 0}, \
				.dir = {ux, uy, uz}, \
			}, \
		} \
	)
#define gdSPDefLights0(ar, ag, ab) \
	( \
		(Lights0) \
		{ \
			.a.l = \
			{ \
				.col = {ar, ag, ab}, \
				.colc = {ar, ag, ab}, \
			}, \
			.l[0].l = \
			{ \
			} \
		} \
	)
#define gdSPDefLights1(ar, ag, ab, \
		r1, g1, b1, x1, y1, z1) \
	( \
		(Lights1) \
		{ \
			.a.l = \
			{ \
				.col = {ar, ag, ab}, \
				.colc = {ar, ag, ab}, \
			}, \
			.l[0].l = \
			{ \
				.col = {r1, g1, b1}, \
				.colc = {r1, g1, b1}, \
				.dir = {x1, y1, z1}, \
			} \
		} \
	)
#define gdSPDefLights2(ar, ag, ab, \
		r1, g1, b1, x1, y1, z1, \
		r2, g2, b2, x2, y2, z2) \
	( \
		(Lights2) \
		{ \
			.a.l = \
			{ \
				.col = {ar, ag, ab}, \
				.colc = {ar, ag, ab}, \
			}, \
			.l[0].l = \
			{ \
				.col = {r1, g1, b1}, \
				.colc = {r1, g1, b1}, \
				.dir = {x1, y1, z1}, \
			} \
			.l[1].l = \
			{ \
				.col = {r2, g2, b2}, \
				.colc = {r2, g2, b2}, \
				.dir = {x2, y2, z2}, \
			} \
		} \
	)
#define gdSPDefLights3(ar, ag, ab, \
		r1, g1, b1, x1, y1, z1, \
		r2, g2, b2, x2, y2, z2, \
		r3, g3, b3, x3, y3, z3) \
	( \
		(Lights3) \
		{ \
			.a.l = \
			{ \
				.col = {ar, ag, ab}, \
				.colc = {ar, ag, ab}, \
			}, \
			.l[0].l = \
			{ \
				.col = {r1, g1, b1}, \
				.colc = {r1, g1, b1}, \
				.dir = {x1, y1, z1}, \
			} \
			.l[1].l = \
			{ \
				.col = {r2, g2, b2}, \
				.colc = {r2, g2, b2}, \
				.dir = {x2, y2, z2}, \
			} \
			.l[2].l = \
			{ \
				.col = {r3, g3, b3}, \
				.colc = {r3, g3, b3}, \
				.dir = {x3, y3, z3}, \
			} \
		} \
	)
#define gdSPDefLights4(ar, ag, ab, \
		r1, g1, b1, x1, y1, z1, \
		r2, g2, b2, x2, y2, z2, \
		r3, g3, b3, x3, y3, z3, \
		r4, g4, b4, x4, y4, z4) \
	( \
		(Lights4) \
		{ \
			.a.l = \
			{ \
				.col = {ar, ag, ab}, \
				.colc = {ar, ag, ab}, \
			}, \
			.l[0].l = \
			{ \
				.col = {r1, g1, b1}, \
				.colc = {r1, g1, b1}, \
				.dir = {x1, y1, z1}, \
			} \
			.l[1].l = \
			{ \
				.col = {r2, g2, b2}, \
				.colc = {r2, g2, b2}, \
				.dir = {x2, y2, z2}, \
			} \
			.l[2].l = \
			{ \
				.col = {r3, g3, b3}, \
				.colc = {r3, g3, b3}, \
				.dir = {x3, y3, z3}, \
			} \
			.l[3].l = \
			{ \
				.col = {r4, g4, b4}, \
				.colc = {r4, g4, b4}, \
				.dir = {x4, y4, z4}, \
			} \
		} \
	)
#define gdSPDefLights5(ar, ag, ab, \
		r1, g1, b1, x1, y1, z1, \
		r2, g2, b2, x2, y2, z2, \
		r3, g3, b3, x3, y3, z3, \
		r4, g4, b4, x4, y4, z4, \
		r5, g5, b5, x5, y5, z5) \
	( \
		(Lights5) \
		{ \
			.a.l = \
			{ \
				.col = {ar, ag, ab}, \
				.colc = {ar, ag, ab}, \
			}, \
			.l[0].l = \
			{ \
				.col = {r1, g1, b1}, \
				.colc = {r1, g1, b1}, \
				.dir = {x1, y1, z1}, \
			} \
			.l[1].l = \
			{ \
				.col = {r2, g2, b2}, \
				.colc = {r2, g2, b2}, \
				.dir = {x2, y2, z2}, \
			} \
			.l[2].l = \
			{ \
				.col = {r3, g3, b3}, \
				.colc = {r3, g3, b3}, \
				.dir = {x3, y3, z3}, \
			} \
			.l[3].l = \
			{ \
				.col = {r4, g4, b4}, \
				.colc = {r4, g4, b4}, \
				.dir = {x4, y4, z4}, \
			} \
			.l[4].l = \
			{ \
				.col = {r5, g5, b5}, \
				.colc = {r5, g5, b5}, \
				.dir = {x5, y5, z5}, \
			} \
		} \
	)
#define gdSPDefLights6(ar, ag, ab, \
		r1, g1, b1, x1, y1, z1, \
		r2, g2, b2, x2, y2, z2, \
		r3, g3, b3, x3, y3, z3, \
		r4, g4, b4, x4, y4, z4, \
		r5, g5, b5, x5, y5, z5, \
		r6, g6, b6, x6, y6, z6)\
	( \
		(Lights6) \
		{ \
			.a.l = \
			{ \
				.col = {ar, ag, ab}, \
				.colc = {ar, ag, ab}, \
			}, \
			.l[0].l = \
			{ \
				.col = {r1, g1, b1}, \
				.colc = {r1, g1, b1}, \
				.dir = {x1, y1, z1}, \
			} \
			.l[1].l = \
			{ \
				.col = {r2, g2, b2}, \
				.colc = {r2, g2, b2}, \
				.dir = {x2, y2, z2}, \
			} \
			.l[2].l = \
			{ \
				.col = {r3, g3, b3}, \
				.colc = {r3, g3, b3}, \
				.dir = {x3, y3, z3}, \
			} \
			.l[3].l = \
			{ \
				.col = {r4, g4, b4}, \
				.colc = {r4, g4, b4}, \
				.dir = {x4, y4, z4}, \
			} \
			.l[4].l = \
			{ \
				.col = {r5, g5, b5}, \
				.colc = {r5, g5, b5}, \
				.dir = {x5, y5, z5}, \
			} \
			.l[5].l = \
			{ \
				.col = {r6, g6, b6}, \
				.colc = {r6, g6, b6}, \
				.dir = {x6, y6, z6}, \
			} \
		} \
	)
#define gdSPDefLights7(ar, ag, ab, \
		r1, g1, b1, x1, y1, z1, \
		r2, g2, b2, x2, y2, z2, \
		r3, g3, b3, x3, y3, z3, \
		r4, g4, b4, x4, y4, z4, \
		r5, g5, b5, x5, y5, z5, \
		r6, g6, b6, x6, y6, z6, \
		r7, g7, b7, x7, y7, z7) \
	( \
		(Lights7) \
		{ \
			.a.l = \
			{ \
				.col = {ar, ag, ab}, \
				.colc = {ar, ag, ab}, \
			}, \
			.l[0].l = \
			{ \
				.col = {r1, g1, b1}, \
				.colc = {r1, g1, b1}, \
				.dir = {x1, y1, z1}, \
			} \
			.l[1].l = \
			{ \
				.col = {r2, g2, b2}, \
				.colc = {r2, g2, b2}, \
				.dir = {x2, y2, z2}, \
			} \
			.l[2].l = \
			{ \
				.col = {r3, g3, b3}, \
				.colc = {r3, g3, b3}, \
				.dir = {x3, y3, z3}, \
			} \
			.l[3].l = \
			{ \
				.col = {r4, g4, b4}, \
				.colc = {r4, g4, b4}, \
				.dir = {x4, y4, z4}, \
			} \
			.l[4].l = \
			{ \
				.col = {r5, g5, b5}, \
				.colc = {r5, g5, b5}, \
				.dir = {x5, y5, z5}, \
			} \
			.l[5].l = \
			{ \
				.col = {r6, g6, b6}, \
				.colc = {r6, g6, b6}, \
				.dir = {x6, y6, z6}, \
			} \
			.l[6].l = \
			{ \
				.col = {r7, g7, b7}, \
				.colc = {r7, g7, b7}, \
				.dir = {x7, y7, z7}, \
			} \
		} \
	)
#define gdSPDefVtx(x, y, z, s, t) \
	( \
		(Vtx) \
		{ \
			.v = \
			{ \
				.ob = {x, y, z}, \
				.tc = {qs105(s), qs105(t)}, \
			} \
		} \
	)
#define gdSPDefVtxC(x, y, z, s, t, cr, cg, cb, ca) \
	( \
		(Vtx) \
		{ \
			.v = \
			{ \
				.ob = {x, y, z}, \
				.tc = {qs105(s), qs105(t)}, \
				.cn = {cr, cg, cb, ca}, \
			} \
		} \
	)
#define gdSPDefVtxN(x, y, z, s, t, nx, ny, nz, ca) \
	( \
		(Vtx) \
		{ \
			.n = \
			{ \
				.ob = {x, y, z}, \
				.tc = {qs105(s), qs105(t)}, \
				.n = {nx, ny, nz}, \
				.a = ca \
			} \
		} \
	)

/* instruction macros */

#define gsDPFillRectangle(ulx, uly, lrx, lry) \
	gO_( \
		G_FILLRECT, \
		gF_(lrx, 10, 14) | \
		gF_(lry, 10, 2), \
		gF_(ulx, 10, 14) | \
		gF_(uly, 10, 2))

#define gsDPScisFillRectangle(ulx, uly, lrx, lry) \
	gsDPFillRectangle(gScC_(ulx), gScC_(uly), gScC_(lrx), gScC_(lry))

#define gsDPFullSync() \
	gO_(G_RDPFULLSYNC, 0, 0)

#define gsDPLoadSync() \
	gO_(G_RDPLOADSYNC, 0, 0)

#define gsDPTileSync() \
	gO_(G_RDPTILESYNC, 0, 0)

#define gsDPPipeSync() \
	gO_(G_RDPPIPESYNC, 0, 0)

#define gsDPLoadTLUT_pal16(pal, dram) \
	gsDPLoadTLUT(16, 256 + (gI_(pal) & 0xF) * 16, dram)

#define gsDPLoadTLUT_pal256(dram) \
	gsDPLoadTLUT(256, 256, dram)

#define gLTB_(timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		dxt, tmem, rt, line) \
	gsDPSetTextureImage(fmt, G_SIZ_LDSIZ(siz), 1, timg), \
	gsDPSetTile( \
		fmt, G_SIZ_LDSIZ(siz), 0, tmem, G_TX_LOADTILE, 0, \
		cmt, maskt, shiftt, \
		cms, masks, shifts), \
	gsDPLoadSync(), \
	gsDPLoadBlock( \
		G_TX_LOADTILE, 0, 0, \
		G_LTB_LRS(width, height, siz), \
		dxt), \
	gsDPPipeSync(), \
	gsDPSetTile( \
		fmt, siz, line, tmem, rt, pal, \
		cmt, maskt, shiftt, \
		cms, masks, shifts), \
	gsDPSetTileSize(rt, 0, 0, qu102((width) - 1), qu102((height) - 1))

#define gsDPLoadTextureBlock(timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		G_DXT(siz, width), 0x0, G_TX_RENDERTILE, \
		((width) * G_SIZ_LDBITS(siz) + 63) / 64)

#define gsDPLoadTextureBlockS(timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		0, 0x0, G_TX_RENDERTILE, \
		((width) * G_SIZ_LDBITS(siz) + 63) / 64)

#define gsDPLoadTextureBlock_4b(timg, fmt, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, G_IM_SIZ_4b, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		G_DXT(G_IM_SIZ_4b, width), 0x0, G_TX_RENDERTILE, \
		((width) * 4 + 63) / 64)

#define gsDPLoadTextureBlock_4bS(timg, fmt, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, G_IM_SIZ_4b, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		0, 0x0, G_TX_RENDERTILE, \
		((width) * 4 + 63) / 64)

#define gsDPLoadTextureBlockYuv(timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		G_DXT(siz, width), 0x0, G_TX_RENDERTILE, \
		((width) + 7) / 8)

#define gsDPLoadTextureBlockYuvS(timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		0, 0x0, G_TX_RENDERTILE, \
		((width) + 7) / 8)

#define _gsDPLoadTextureBlock(timg, tmem, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		G_DXT(siz, width), tmem, G_TX_RENDERTILE, \
		((width) * G_SIZ_LDBITS(siz) + 63) / 64)

#define _gsDPLoadTextureBlockS(timg, tmem, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		0, tmem, G_TX_RENDERTILE, \
		((width) * G_SIZ_LDBITS(siz) + 63) / 64)

#define _gsDPLoadTextureBlock_4b(timg, tmem, fmt, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, G_IM_SIZ_4b, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		G_DXT(G_IM_SIZ_4b, width), tmem, G_TX_RENDERTILE, \
		((width) * 4 + 63) / 64)

#define _gsDPLoadTextureBlock_4bS(timg, tmem, fmt, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, G_IM_SIZ_4b, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		0, tmem, G_TX_RENDERTILE, \
		((width) * 4 + 63) / 64)

#define _gsDPLoadTextureBlockYuv(timg, tmem, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		G_DXT(siz, width), tmem, G_TX_RENDERTILE, \
		((width) + 7) / 8)

#define _gsDPLoadTextureBlockYuvS(timg, tmem, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		0, tmem, G_TX_RENDERTILE, \
		((width) + 7) / 8)

#define gsDPLoadMultiBlock(timg, tmem, rt, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		G_DXT(siz, width), tmem, rt, \
		((width) * G_SIZ_LDBITS(siz) + 63) / 64)

#define gsDPLoadMultiBlockS(timg, tmem, rt, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		0, tmem, rt, \
		((width) * G_SIZ_LDBITS(siz) + 63) / 64)

#define gsDPLoadMultiBlock_4b(timg, tmem, rt, fmt, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, G_IM_SIZ_4b, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		G_DXT(G_IM_SIZ_4b, width), tmem, rt, \
		((width) * 4 + 63) / 64)

#define gsDPLoadMultiBlock_4bS(timg, tmem, rt, fmt, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, G_IM_SIZ_4b, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		0, tmem, rt, \
		((width) * 4 + 63) / 64)

#define gsDPLoadMultiBlockYuv(timg, tmem, rt, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		G_DXT(siz, width), tmem, rt, \
		((width) + 7) / 8)

#define gsDPLoadMultiBlockYuvS(timg, tmem, rt, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTB_( \
		timg, fmt, siz, width, height, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		0, tmem, rt, \
		((width) + 7) / 8)

#define gLTT_(timg, fmt, siz, width, height, uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		tmem, rt, line) \
	gsDPSetTextureImage(fmt, siz, width, timg), \
	gsDPSetTile( \
		fmt, siz, line, tmem, \
		G_TX_LOADTILE, 0, \
		cmt, maskt, shiftt, \
		cms, masks, shifts), \
	gsDPLoadSync(), \
	gsDPLoadTile( \
		G_TX_LOADTILE, \
		qu102(uls), qu102(ult), \
		qu102(lrs), qu102(lrt)), \
	gsDPPipeSync(), \
	gsDPSetTile( \
		fmt, siz, line, \
		tmem, rt, pal, \
		cmt, maskt, shiftt, \
		cms, masks, shifts), \
	gsDPSetTileSize( \
		rt, \
		qu102(uls), qu102(ult), \
		qu102(lrs), qu102(lrt))

#define gLTT4_(timg, fmt, width, height, uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		tmem, rt) \
	gsDPSetTextureImage(fmt, G_IM_SIZ_8b, (width) / 2, timg), \
	gsDPSetTile( \
		fmt, G_IM_SIZ_8b, \
		(((lrs) - (uls) + 1) / 2 + 7) / 8, \
		tmem, G_TX_LOADTILE, 0, \
		cmt, maskt, shiftt, \
		cms, masks, shifts), \
	gsDPLoadSync(), \
	gsDPLoadTile( \
		G_TX_LOADTILE, \
		qu102(uls) / 2, qu102(ult), \
		qu102(lrs) / 2, qu102(lrt)), \
	gsDPPipeSync(), \
	gsDPSetTile( \
		fmt, G_IM_SIZ_4b, \
		(((lrs) - (uls) + 1) / 2 + 7) / 8, \
		tmem, rt, pal, \
		cmt, maskt, shiftt, \
		cms, masks, shifts), \
	gsDPSetTileSize( \
		rt, \
		qu102(uls), qu102(ult), \
		qu102(lrs), qu102(lrt))

#define gsDPLoadTextureTile(timg, fmt, siz, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTT_( \
		timg, fmt, siz, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		0x0, G_TX_RENDERTILE, \
		(((lrs) - (uls) + 1) * G_SIZ_LDBITS(siz) + 63) / 64)

#define gsDPLoadTextureTile_4b(timg, fmt, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTT4_( \
		timg, fmt, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		0x0, G_TX_RENDERTILE)

#define gsDPLoadTextureTileYuv(timg, fmt, siz, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTT_( \
		timg, fmt, siz, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		0x0, G_TX_RENDERTILE, \
		(((lrs) - (uls) + 1) + 7) / 8)

#define _gsDPLoadTextureTile(timg, tmem, fmt, siz, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTT_( \
		timg, fmt, siz, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		tmem, G_TX_RENDERTILE, \
		(((lrs) - (uls) + 1) * G_SIZ_LDBITS(siz) + 63) / 64)

#define _gsDPLoadTextureTile_4b(timg, tmem, fmt, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTT4_( \
		timg, fmt, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		tmem, G_TX_RENDERTILE)

#define _gsDPLoadTextureTileYuv(timg, tmem, fmt, siz, width, height, \
		uls, ult, lrs, lrt, pal, cms, cmt, \
		masks, maskt, shifts, shiftt) \
	gLTT_( \
		timg, fmt, siz, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		tmem, G_TX_RENDERTILE, \
		(((lrs) - (uls) + 1) + 7) / 8)

#define gsDPLoadMultiTile(timg, tmem, rt, fmt, siz, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTT_( \
		timg, fmt, siz, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		tmem, rt, \
		(((lrs) - (uls) + 1) * G_SIZ_LDBITS(siz) + 63) / 64)

#define gsDPLoadMultiTile_4b(timg, tmem, rt, fmt, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTT4_( \
		timg, fmt, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		tmem, rt)

#define gsDPLoadMultiTileYuv(timg, tmem, rt, fmt, siz, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt) \
	gLTT_( \
		timg, fmt, siz, width, height, \
		uls, ult, lrs, lrt, pal, \
		cms, cmt, masks, maskt, shifts, shiftt, \
		tmem, rt, \
		(((lrs) - (uls) + 1) + 7) / 8)

#define gsDPLoadBlock(tile, uls, ult, lrs, dxt) \
	gO_( \
		G_LOADBLOCK, \
		gF_(uls, 12, 12) | \
		gF_(ult, 12, 0), \
		gF_(tile, 3, 24) | \
		gF_(G_LDBLK_TXL(lrs), 12, 12) | \
		gF_(dxt, 12, 0))

#define gsDPNoOp() \
	gsDPNoOpTag(0)

#define gsDPNoOpTag(tag) \
	gO_(G_NOOP, 0, tag)

#define gsDPPipelineMode(mode) \
	gsSPSetOtherModeHi(G_MDSFT_PIPELINE, G_MDSIZ_PIPELINE, mode)

#define gsDPSetBlendColor(r, g, b, a) \
	gO_( \
		G_SETBLENDCOLOR, \
		0, \
		gF_(r, 8, 24) | \
		gF_(g, 8, 16) | \
		gF_(b, 8, 8) | \
		gF_(a, 8, 0))

#define gsDPSetEnvColor(r, g, b, a) \
	gO_( \
		G_SETENVCOLOR, \
		0, \
		gF_(r, 8, 24) | \
		gF_(g, 8, 16) | \
		gF_(b, 8, 8) | \
		gF_(a, 8, 0))

#define gsDPSetFillColor(c) \
	gO_(G_SETFILLCOLOR, 0, c)

#define gsDPSetFogColor(r, g, b, a) \
	gO_( \
		G_SETFOGCOLOR, 0, \
		gF_(r, 8, 24) | \
		gF_(g, 8, 16) | \
		gF_(b, 8, 8) | \
		gF_(a, 8, 0))

#define gsDPSetPrimColor(m, l, r, g, b, a) \
	gO_( \
		G_SETPRIMCOLOR, \
		gF_(m, 8, 8) | \
		gF_(l, 8, 0), \
		gF_(r, 8, 24) | \
		gF_(g, 8, 16) | \
		gF_(b, 8, 8) | \
		gF_(a, 8, 0))

#define gsDPSetColorImage(fmt, siz, width, img) \
	gO_( \
		G_SETCIMG, \
		gF_(fmt, 3, 21) | \
		gF_(siz, 2, 19) | \
		gF_((width) - 1, 12, 0), \
		img)

#define gsDPSetDepthImage(img) \
	gO_(G_SETZIMG, 0, img)

#define gsDPSetTextureImage(fmt, siz, width, img) \
	gO_( \
		G_SETTIMG, \
		gF_(fmt, 3, 21) | \
		gF_(siz, 2, 19) | \
		gF_((width) - 1, 12, 0), \
		img)

#define gsDPSetHilite1Tile(tile, hilite, width, height) \
	gsDPSetTileSize( \
		tile, \
		((Hilite *)(hilite))->h.x1 & 0xFFF, \
		((Hilite *)(hilite))->h.y1 & 0xFFF, \
		(((width) - 1) * 4 + ((Hilite *)(hilite))->h.x1) & 0xFFF, \
		(((height) - 1) * 4 + ((Hilite *)(hilite))->h.y1) & 0xFFF)

#define gsDPSetHilite2Tile(tile, hilite, width, height) \
	gsDPSetTileSize( \
		tile, \
		((Hilite *)(hilite))->h.x2 & 0xFFF, \
		((Hilite *)(hilite))->h.y2 & 0xFFF, \
		(((width) - 1) * 4 + ((Hilite *)(hilite))->h.x2) & 0xFFF, \
		(((height) - 1) * 4 + ((Hilite *)(hilite))->h.y2) & 0xFFF)

#define gsDPSetAlphaCompare(mode) \
	gsSPSetOtherModeLo(G_MDSFT_ALPHACOMPARE, G_MDSIZ_ALPHACOMPARE, mode)

#define gsDPSetAlphaDither(type) \
	gsSPSetOtherModeHi(G_MDSFT_ALPHADITHER, G_MDSIZ_ALPHADITHER, type)

#define gsDPSetColorDither(type) \
	gsSPSetOtherModeHi(G_MDSFT_RGBDITHER, G_MDSIZ_RGBDITHER, type)

#define gsDPSetCombineMode(mode1, mode2) \
	gsDPSetCombineLERP(mode1, mode2)

#define gsDPSetCombineLERP(a0, b0, c0, d0, Aa0, Ab0, Ac0, Ad0, \
		a1, b1, c1, d1, Aa1, Ab1, Ac1, Ad1) \
	gO_( \
		G_SETCOMBINE, \
		gF_(G_CCMUX_##a0, 4, 20) | \
		gF_(G_CCMUX_##c0, 5, 15) | \
		gF_(G_ACMUX_##Aa0, 3, 12) | \
		gF_(G_ACMUX_##Ac0, 3, 9) | \
		gF_(G_CCMUX_##a1, 4, 5) | \
		gF_(G_CCMUX_##c1, 5, 0), \
		gF_(G_CCMUX_##b0, 4, 28) | \
		gF_(G_CCMUX_##b1, 4, 24) | \
		gF_(G_ACMUX_##Aa1, 3, 21) | \
		gF_(G_ACMUX_##Ac1, 3, 18) | \
		gF_(G_CCMUX_##d0, 3, 15) | \
		gF_(G_ACMUX_##Ab0, 3, 12) | \
		gF_(G_ACMUX_##Ad0, 3, 9) | \
		gF_(G_CCMUX_##d1, 3, 6) | \
		gF_(G_ACMUX_##Ab1, 3, 3) | \
		gF_(G_ACMUX_##Ad1, 3, 0))

#define gsDPSetConvert(k0, k1, k2, k3, k4, k5) \
	gO_( \
		G_SETCONVERT, \
		gF_(k0, 9, 13) | \
		gF_(k1, 9, 4) | \
		gF_(gI_(k2) >> 5, 4, 0), \
		gF_(k2, 5, 27) | \
		gF_(k3, 9, 18) | \
		gF_(k4, 9, 9) | \
		gF_(k5, 9, 0))

#define gsDPSetTextureConvert(type) \
	gsSPSetOtherModeHi(G_MDSFT_TEXTCONV, G_MDSIZ_TEXTCONV, type)

#define gsDPSetCycleType(type) \
	gsSPSetOtherModeHi(G_MDSFT_CYCLETYPE, G_MDSIZ_CYCLETYPE, type)

#define gsDPSetDepthSource(source) \
	gsSPSetOtherModeLo(G_MDSFT_ZSRCSEL, G_MDSIZ_ZSRCSEL, source)

#define gsDPSetCombineKey(type) \
	gsSPSetOtherModeHi(G_MDSFT_COMBKEY, G_MDSIZ_COMBKEY, type)

#define gsDPSetKeyGB(cG, sG, wG, cB, sB, wB) \
	gO_( \
		G_SETKEYGB, \
		gF_(wG, 12, 12) | \
		gF_(wB, 12, 0), \
		gF_(cG, 8, 24) | \
		gF_(sG, 8, 16) | \
		gF_(cB, 8, 8) | \
		gF_(sB, 8, 0))

#define gsDPSetKeyR(cR, sR, wR) \
	gO_( \
		G_SETKEYR, 0, \
		gF_(wR, 12, 16) | \
		gF_(cR, 8, 8) | \
		gF_(sR, 8, 0))

#define gsDPSetPrimDepth(z, dz) \
	gO_( \
		G_SETPRIMDEPTH, \
		0, \
		gF_(z, 16, 16) | \
		gF_(dz, 16, 0))

#define gsDPSetRenderMode(mode1, mode2) \
	gsSPSetOtherModeLo( \
		G_MDSFT_RENDERMODE, \
		G_MDSIZ_RENDERMODE, \
		gI_(mode1) | \
		gI_(mode2))

#define gsDPSetScissor(mode, ulx, uly, lrx, lry) \
	gsDPSetScissorFrac( \
		mode, \
		qu102(gI_(ulx)), \
		qu102(gI_(uly)), \
		qu102(gI_(lrx)), \
		qu102(gI_(lry)))

#define gsDPSetScissorFrac(mode, ulx, uly, lrx, lry) \
	gO_( \
		G_SETSCISSOR, \
		gF_(ulx, 12, 12) | \
		gF_(uly, 12, 0), \
		gF_(mode, 2, 24) | \
		gF_(lrx, 12, 12) | \
		gF_(lry, 12, 0))

#define gsDPSetTextureDetail(type) \
	gsSPSetOtherModeHi(G_MDSFT_TEXTDETAIL, G_MDSIZ_TEXTDETAIL, type)

#define gsDPSetTextureFilter(mode) \
	gsSPSetOtherModeHi(G_MDSFT_TEXTFILT, G_MDSIZ_TEXTFILT, mode)

#define gsDPSetTextureLOD(mode) \
	gsSPSetOtherModeHi(G_MDSFT_TEXTLOD, G_MDSIZ_TEXTLOD, mode)

#define gsDPSetTextureLUT(mode) \
	gsSPSetOtherModeHi(G_MDSFT_TEXTLUT, G_MDSIZ_TEXTLUT, mode)

#define gsDPSetTexturePersp(enable) \
	gsSPSetOtherModeHi(G_MDSFT_TEXTPERSP, G_MDSIZ_TEXTPERSP, enable)

#define gsDPSetTile(fmt, siz, line, tmem, tile, palette, \
		cmt, maskt, shiftt, cms, masks, shifts) \
	gO_( \
		G_SETTILE, \
		gF_(fmt, 3, 21) | \
		gF_(siz, 2, 19) | \
		gF_(line, 9, 9) | \
		gF_(tmem, 9, 0), \
		gF_(tile, 3, 24) | \
		gF_(palette, 4, 20) | \
		gF_(cmt, 2, 18) | \
		gF_(maskt, 4, 14) | \
		gF_(shiftt, 4, 10) | \
		gF_(cms, 2, 8) | \
		gF_(masks, 4, 4) | \
		gF_(shifts, 4, 0))

#define gsDPSetTileSize(tile, uls, ult, lrs, lrt) \
	gO_( \
		G_SETTILESIZE, \
		gF_(uls, 12, 12) | \
		gF_(ult, 12, 0), \
		gF_(tile, 3, 24) | \
		gF_(lrs, 12, 12) | \
		gF_(lrt, 12, 0))

#define gsSPBranchList(dl) \
	gsDisplayList(dl, 1)

#define gsSPClipRatio(r) \
	gsMoveWd(G_MW_CLIP, G_MWO_CLIP_RNX, (uint16_t)(r)), \
	gsMoveWd(G_MW_CLIP, G_MWO_CLIP_RNY, (uint16_t)(r)), \
	gsMoveWd(G_MW_CLIP, G_MWO_CLIP_RPX, (uint16_t)-(r)), \
	gsMoveWd(G_MW_CLIP, G_MWO_CLIP_RPY, (uint16_t)-(r))

#define gsSPDisplayList(dl) \
	gsDisplayList(dl, 0)

#define gsSPEndDisplayList() \
	gO_(G_ENDDL, 0, 0)

#define gsSPFogFactor(fm, fo) \
	gsMoveWd( \
		G_MW_FOG, \
		G_MWO_FOG, \
		gF_(fm, 16, 16) | \
		gF_(fo, 16, 0))

#define gsSPFogPosition(min, max) \
	gsSPFogFactor( \
		(500 * 0x100) / ((max) - (min)), \
		(500 - (min)) * 0x100 / ((max) - (min)))

#define gsSPLine3D(v0, v1, flag) \
	gsSPLineW3D(v0, v1, 0, flag)

#define gsSPLookAt(l) \
	gsSPLookAtX(l), \
	gsSPLookAtY(gI_(l) + 0x10)

#define gsSPSegment(seg, base) \
	gsMoveWd(G_MW_SEGMENT, (seg) * 4, base)

#define gsSPSetLights0(lites) \
	gsSPNumLights(NUMLIGHTS_0), \
	gsSPLight(&(lites).l[0], 1), \
	gsSPLight(&(lites).a, 2)

#define gsSPSetLights1(lites) \
	gsSPNumLights(NUMLIGHTS_1), \
	gsSPLight(&(lites).l[0], 1), \
	gsSPLight(&(lites).a, 2)

#define gsSPSetLights2(lites) \
	gsSPNumLights(NUMLIGHTS_2), \
	gsSPLight(&(lites).l[0], 1), \
	gsSPLight(&(lites).l[1], 2), \
	gsSPLight(&(lites).a, 3)

#define gsSPSetLights3(lites) \
	gsSPNumLights(NUMLIGHTS_3), \
	gsSPLight(&(lites).l[0], 1), \
	gsSPLight(&(lites).l[1], 2), \
	gsSPLight(&(lites).l[2], 3), \
	gsSPLight(&(lites).a, 4)

#define gsSPSetLights4(lites) \
	gsSPNumLights(NUMLIGHTS_4), \
	gsSPLight(&(lites).l[0], 1), \
	gsSPLight(&(lites).l[1], 2), \
	gsSPLight(&(lites).l[2], 3), \
	gsSPLight(&(lites).l[3], 4), \
	gsSPLight(&(lites).a, 5)

#define gsSPSetLights5(lites) \
	gsSPNumLights(NUMLIGHTS_5), \
	gsSPLight(&(lites).l[0], 1), \
	gsSPLight(&(lites).l[1], 2), \
	gsSPLight(&(lites).l[2], 3), \
	gsSPLight(&(lites).l[3], 4), \
	gsSPLight(&(lites).l[4], 5), \
	gsSPLight(&(lites).a, 6)

#define gsSPSetLights6(lites) \
	gsSPNumLights(NUMLIGHTS_6), \
	gsSPLight(&(lites).l[0], 1), \
	gsSPLight(&(lites).l[1], 2), \
	gsSPLight(&(lites).l[2], 3), \
	gsSPLight(&(lites).l[3], 4), \
	gsSPLight(&(lites).l[4], 5), \
	gsSPLight(&(lites).l[5], 6), \
	gsSPLight(&(lites).a, 7)

#define gsSPSetLights7(lites) \
	gsSPNumLights(NUMLIGHTS_7), \
	gsSPLight(&(lites).l[0], 1), \
	gsSPLight(&(lites).l[1], 2), \
	gsSPLight(&(lites).l[2], 3), \
	gsSPLight(&(lites).l[3], 4), \
	gsSPLight(&(lites).l[4], 5), \
	gsSPLight(&(lites).l[5], 6), \
	gsSPLight(&(lites).l[6], 7), \
	gsSPLight(&(lites).a, 8)

#define gsSPSetStatus(sid, val) \
	gsMoveWd(G_MW_GENSTAT, sid, val)

#define gsSPNumLights(n) \
	gsMoveWd(G_MW_NUMLIGHT, G_MWO_NUMLIGHT, NUML(n))

#define gsSPLightColor(Lightnum, packedcolor) \
	gsMoveWd(G_MW_LIGHTCOL, G_MWO_a##Lightnum, packedcolor), \
	gsMoveWd(G_MW_LIGHTCOL, G_MWO_b##Lightnum, packedcolor)

#define gsSPTextureRectangle(ulx, uly, lrx, lry, tile, s, t, dsdx, dtdy) \
	gsTexRect(ulx, uly, lrx, lry, tile), \
	gsDPHalf1(gF_(s, 16, 16) | gF_(t, 16, 0)), \
	gsDPHalf2(gF_(dsdx, 16, 16) | gF_(dtdy, 16, 0))

#define gsSPScisTextureRectangle(ulx, uly, lrx, lry, tile, s, t, dsdx, dtdy) \
	gsTexRect(gScC_(ulx), gScC_(uly), gScC_(lrx), gScC_(lry), tile), \
	gsDPHalf1( \
		gF_(gScD_(s, ulx, dsdx), 16, 16) | \
		gF_(gScD_(t, uly, dtdy), 16, 0)), \
	gsDPHalf2(gF_(dsdx, 16, 16) | gF_(dtdy, 16, 0))

#define gsSPTextureRectangleFlip(ulx, uly, lrx, lry, tile, s, t, dsdx, dtdy) \
	gsTexRectFlip(ulx, uly, lrx, lry, tile), \
	gsDPHalf1(gF_(s, 16, 16) | gF_(t, 16, 0)), \
	gsDPHalf2(gF_(dsdx, 16, 16) | gF_(dtdy, 16, 0))

#define gsSPScisTextureRectangleFlip( \
		ulx, uly, lrx, lry, tile, s, t, dsdx, dtdy) \
	gsTexRectFlip(gScC_(ulx), gScC_(uly), gScC_(lrx), gScC_(lry), tile), \
	gsDPHalf1( \
		gF_(gScD_(s, ulx, dsdx), 16, 16) | \
		gF_(gScD_(t, uly, dtdy), 16, 0)), \
		gsDPHalf2(gF_(dsdx, 16, 16) | gF_(dtdy, 16, 0))

#define gsSPBgRectCopy(bg) \
	gO_(G_BG_COPY, 0, bg)

#define gsSPBgRect1Cyc(bg) \
	gO_(G_BG_1CYC, 0, bg)

#define gsSPObjRectangle(sp) \
	gO_(G_OBJ_RECTANGLE, 0, sp)

#define gsSPObjRectangleR(sp) \
	gO_(G_OBJ_RECTANGLE_R, 0, sp)

#define gsSPObjSprite(sp) \
	gO_(G_OBJ_SPRITE, 0, sp)

#define gsSPObjMatrix(mtx) \
	gO_( \
		G_OBJ_MOVEMEM, \
		gF_(sizeof(uObjMtx) - 1, 8, 16), \
		mtx)

#define gsSPObjSubMatrix(mtx) \
	gO_( \
		G_OBJ_MOVEMEM, \
		gF_(sizeof(uObjSubMtx) - 1, 8, 16) | \
		gF_(2, 16, 0), \
		mtx)

#define gsSPObjRenderMode(mode) \
	gO_(G_OBJ_RENDERMODE, 0, mode)

#define gsSPObjLoadTxtr(tx) \
	gO_(G_OBJ_LOADTXTR, 23, tx)

#define gsSPObjLoadTxRect(txsp) \
	gO_(G_OBJ_LDTX_RECT, 47, txsp)

#define gsSPObjLoadTxRectR(txsp) \
	gO_(G_OBJ_LDTX_RECT_R, 47, txsp)

#define gsSPObjLoadTxSprite(txsp) \
	gO_(G_OBJ_LDTX_SPRITE, 47, txsp)

#define gsSPSelectDL(ldl, sid, flag, mask) \
	gO_( \
		G_RDPHALF_0, \
		gF_(sid, 8, 16) | \
		gF_(ldl, 16, 0), \
		flag), \
	gO_( \
		G_SELECT_DL, \
		gF_(0x00, 8, 16) | \
		gF_(gI_(ldl) >> 16, 16, 0), \
		mask)

#define gsSPSelectBranchDL(bdl, sid, flag, mask) \
	gO_( \
		G_RDPHALF_0, \
		gF_(sid, 8, 16) | \
		gF_(bdl, 16, 0), \
		flag), \
	gO_( \
		G_SELECT_DL, \
		gF_(0x01, 8, 16) | \
		gF_(gI_(bdl) >> 16, 16, 0), \
		mask)

/* unlisted instructions */

#define gsDPLoadTLUTCmd(tile, count) \
	gO_( \
		G_LOADTLUT, \
		0, \
		gF_(tile, 3, 24) | \
		gF_(count, 10, 14))

#define gsDPLoadTLUT(count, tmem, dram) \
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, dram), \
	gsDPTileSync(), \
	gsDPSetTile(0, 0, 0, tmem, G_TX_LOADTILE, 0, 0, 0, 0, 0, 0, 0), \
	gsDPLoadSync(), \
	gsDPLoadTLUTCmd(G_TX_LOADTILE, (count) - 1), \
	gsDPPipeSync()

#define gsDisplayList(dl, branch) \
	gO_(G_DL, gF_(branch, 8, 16), dl)

#define gsDPLoadTile(tile, uls, ult, lrs, lrt) \
	gO_( \
		G_LOADTILE, \
		gF_(uls, 12, 12) | \
		gF_(ult, 12, 0), \
		gF_(tile, 3, 24) | \
		gF_(lrs, 12, 12) | \
		gF_(lrt, 12, 0))

#define gsDPSetCombine(c) \
	gO_( \
		G_SETCOMBINE, \
		(gL_(c) >> 32) & 0xFFFFFFFF, \
		(gL_(c) >> 0) & 0xFFFFFFFF)

#define gsSPSetOtherModeLo(shift, length, data) \
	gsSPSetOtherMode(G_SETOTHERMODE_L, shift, length, data)

#define gsSPSetOtherModeHi(shift, length, data) \
	gsSPSetOtherMode(G_SETOTHERMODE_H, shift, length, data)

#define gsDPSetOtherMode(mode0, mode1) \
	gO_(G_RDPSETOTHERMODE, gF_(mode0, 24, 0), mode1)

#define gsTexRect(ulx, uly, lrx, lry, tile) \
	gO_( \
		G_TEXRECT, \
		gF_(lrx, 12, 12) | \
		gF_(lry, 12, 0), \
		gF_(tile, 3, 24) | \
		gF_(ulx, 12, 12) | \
		gF_(uly, 12, 0))

#define gsTexRectFlip(ulx, uly, lrx, lry, tile) \
	gO_( \
		G_TEXRECTFLIP, \
		gF_(lrx, 12, 12) | \
		gF_(lry, 12, 0), \
		gF_(tile, 3, 24) | \
		gF_(ulx, 12, 12) | \
		gF_(uly, 12, 0))

#define gsSPNoOp() \
	gO_(G_SPNOOP, 0, 0)

#define gsDPHalf1(wordhi) \
	gO_(G_RDPHALF_1, 0, wordhi)

#define gsDPHalf2(wordlo) \
	gO_(G_RDPHALF_2, 0, wordlo)

#define gsDPWord(wordhi, wordlo) \
	gsDPHalf1(wordhi), \
	gsDPHalf2(wordlo)

/* instruction macros for fast3d */

#if defined(F3D_GBI)

# define gsSP1Triangle(v0, v1, v2, flag) \
	gO_( \
		G_TRI1, \
		0, \
		gF_(flag, 8, 24) | \
		gF_(gI_(v0) * 10, 8, 16) | \
		gF_(gI_(v1) * 10, 8, 8) | \
		gF_(gI_(v2) * 10, 8, 0))

# define gsSPCullDisplayList(v0, vn) \
	gO_( \
		G_CULLDL, \
		(gI_(v0) & 0xF) * 40, \
		gI_((vn) + 1) & 0xF) * 40)

# define gsSPLineW3D(v0, v1, wd, flag) \
	gO_( \
		G_LINE3D, \
		0, \
		gF_(flag, 8, 24) | \
		gF_(gI_(v0) * 10, 8, 16) | \
		gF_(gI_(v1) * 10, 8, 8) | \
		gF_(wd, 8, 0))

# define gsSPVertex(v, n, v0) \
	gO_( \
		G_VTX, \
		gF_((n) - 1, 4, 20) | \
		gF_(v0, 4, 16) | \
		gF_(sizeof(Vtx) * (n), 16, 0), \
		v)

#endif

/* instruction macros for fast3d and beta f3dex */
#if defined(F3D_GBI) || (defined(F3D_BETA) && defined(F3DEX_GBI))

# define gsSPModifyVertex(vtx, where, val) \
	gsMoveWd(G_MW_POINTS, (vtx) * 40 + (where), val)

#endif

/* instruction macros for fast3d and f3dex */

#if defined(F3D_GBI) || defined(F3DEX_GBI)

# define gsSPForceMatrix(mptr) \
	gsMoveMem(16, G_MV_MATRIX_1, (char *)(mptr)), \
	gsMoveMem(16, G_MV_MATRIX_2, (char *)(mptr) + 16), \
	gsMoveMem(16, G_MV_MATRIX_3, (char *)(mptr) + 32), \
	gsMoveMem(16, G_MV_MATRIX_4, (char *)(mptr) + 48)

# define gsSPSetGeometryMode(mode) \
	gO_(G_SETGEOMETRYMODE, 0, gI_(mode))

# define gsSPClearGeometryMode(mode) \
	gO_(G_CLEARGEOMETRYMODE, 0, gI_(mode))

# define gsSPLoadGeometryMode(mode) \
	gsSPClearGeometryMode(~gI_(0)), \
	gsSPSetGeometryMode(mode)

# define gsSPInsertMatrix(where, num) \
	gsMoveWd(G_MW_MATRIX, where, num)

# define gsSPLookAtX(l) \
	gsMoveMem(sizeof(Light), G_MV_LOOKATX, l)

# define gsSPLookAtY(l) \
	gsMoveMem(sizeof(Light), G_MV_LOOKATY, l)

# define gsSPMatrix(matrix, param) \
	gO_( \
		G_MTX, \
		gF_(param, 8, 16) | \
		gF_(sizeof(Mtx), 16, 0), \
		matrix)

# define gsSPPopMatrix(param) \
	gO_(G_POPMTX, 0, param)

# define gsSPLight(l, n) \
	gsMoveMem(sizeof(Light), G_MV_L0 + ((n) - 1) * 2, l)

# define gsSPTexture(sc, tc, level, tile, on) \
	gO_( \
		G_TEXTURE, \
		gF_(level, 3, 11) | \
		gF_(tile, 3, 8) | \
		gF_(on, 8, 0), \
		gF_(sc, 16, 16) | \
		gF_(tc, 16, 0))

# define gsSPViewport(v) \
	gsMoveMem(sizeof(Vp), G_MV_VIEWPORT, v)

# define gsSPSetOtherMode(opc, shift, length, data) \
	gO_( \
		opc, \
		gF_(shift, 8, 8) | \
		gF_(length, 8, 0), \
		data)

# define gsMoveWd(index, offset, data) \
	gO_( \
		G_MOVEWORD, \
		gF_(offset, 16, 8) | \
		gF_(index, 8, 0), \
		data)

# define gsMoveMem(size, index, address) \
	gO_( \
		G_MOVEMEM, \
		gF_(index, 8, 16) | \
		gF_(size, 16, 0), \
		address)

#endif

/* instruction macros for f3dex */

#if defined(F3DEX_GBI)

# define gsSP1Triangle(v0, v1, v2, flag) \
	gO_( \
		G_TRI1, \
		0, \
		gF_(gV3_(v0, v1, v2, flag) * 2, 8, 16) | \
		gF_(gV3_(v1, v2, v0, flag) * 2, 8, 8) | \
		gF_(gV3_(v2, v0, v1, flag) * 2, 8, 0))

# define gsSP1Quadrangle(v0, v1, v2, v3, flag) \
	gO_( \
		G_TRI2, \
		gF_(gV4_(v0, v1, v2, v3, flag) * 2, 8, 16) | \
		gF_(gV4_(v1, v2, v3, v0, flag) * 2, 8, 8) | \
		gF_(gV4_(v2, v3, v0, v1, flag) * 2, 8, 0), \
		gF_(gV4_(v0, v1, v2, v3, flag) * 2, 8, 16) | \
		gF_(gV4_(v2, v3, v0, v1, flag) * 2, 8, 8) | \
		gF_(gV4_(v3, v0, v1, v2, flag) * 2, 8, 0))

# define gsSPLineW3D(v0, v1, wd, flag) \
	gO_( \
		G_LINE3D, \
		0, \
		gF_(gV2_(v0, v1, flag) * 2, 8, 16) | \
		gF_(gV2_(v1, v0, flag) * 2, 8, 8) | \
		gF_(wd, 8, 0))

# define gsSPVertex(v, n, v0) \
	gO_( \
		G_VTX, \
		gF_((v0) * 2, 8, 16) | \
		gF_(n, 6, 10) | \
		gF_(sizeof(Vtx) * (n) - 1, 10, 0), \
		v)

#endif

/* instruction macros for f3dex and f3dex2 */

#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)

# define gsSP2Triangles(v00, v01, v02, flag0, v10, v11, v12, flag1) \
	gO_( \
		G_TRI2, \
		gF_(gV3_(v00, v01, v02, flag0) * 2, 8, 16) | \
		gF_(gV3_(v01, v02, v00, flag0) * 2, 8, 8) | \
		gF_(gV3_(v02, v00, v01, flag0) * 2, 8, 0), \
		gF_(gV3_(v10, v11, v12, flag1) * 2, 8, 16) | \
		gF_(gV3_(v11, v12, v10, flag1) * 2, 8, 8) | \
		gF_(gV3_(v12, v10, v11, flag1) * 2, 8, 0))

# define gsSPBranchLessZ(branchdl, vtx, zval, near, far, flag) \
	gsSPBranchLessZrg(branchdl, vtx, zval, near, far, flag, 0, G_MAXZ)

# define gsSPBranchLessZrg(branchdl, vtx, zval, near, far, flag, zmin, zmax) \
	gsSPBranchLessZraw(branchdl, vtx, \
		G_DEPTOZSrg(zval, near, far, flag, zmin, zmax))

# define gsSPBranchLessZraw(branchdl, vtx, zval) \
	gsDPHalf1(branchdl), \
	gsBranchZ(vtx, zval)

# define gsSPCullDisplayList(v0, vn) \
	gO_( \
		G_CULLDL, \
		gF_((v0) * 2, 16, 0), \
		gF_((vn) * 2, 16, 0))

# define gsSPLoadUcode(uc_start, uc_dstart) \
	gsSPLoadUcodeEx(uc_start, uc_dstart, 0x800)

# define gsSPLoadUcodeL(ucode) \
	gsSPLoadUcode( \
		gI_(&ucode##TextStart) & 0x1FFFFFFF, \
		gI_(&ucode##DataStart) & 0x1FFFFFFF)

# if !(defined(F3D_BETA) && defined(F3DEX_GBI))
#  define gsSPModifyVertex(vtx, where, val) \
	gO_( \
		G_MODIFYVTX, \
		gF_(where, 8, 16) | \
		gF_((vtx) * 2, 16, 0), \
		val)
# endif

# define gsBranchZ(vtx, zval) \
	gO_( \
		G_BRANCH_Z, \
		gF_((vtx) * 5, 12, 12) | \
		gF_((vtx) * 2, 12, 0), \
		zval)

# define gsLoadUcode(uc_start, uc_dsize) \
	gO_( \
		G_LOAD_UCODE, \
		gF_((uc_dsize) - 1, 16, 0), \
		uc_start)

# define gsSPLoadUcodeEx(uc_start, uc_dstart, uc_dsize) \
	gsDPHalf1(uc_dstart), \
	gsLoadUcode(uc_start, uc_dsize)

#endif

/* instruction macros for f3dex2 */

#if defined(F3DEX_GBI_2)

# define gsSP1Triangle(v0, v1, v2, flag) \
	gO_( \
		G_TRI1, \
		gF_(gV3_(v0, v1, v2, flag) * 2, 8, 16) | \
		gF_(gV3_(v1, v2, v0, flag) * 2, 8, 8) | \
		gF_(gV3_(v2, v0, v1, flag) * 2, 8, 0), \
		0)

# define gsSP1Quadrangle(v0, v1, v2, v3, flag) \
	gO_( \
		G_QUAD, \
		gF_(gV4_(v0, v1, v2, v3, flag) * 2, 8, 16) | \
		gF_(gV4_(v1, v2, v3, v0, flag) * 2, 8, 8) | \
		gF_(gV4_(v2, v3, v0, v1, flag) * 2, 8, 0), \
		gF_(gV4_(v0, v1, v2, v3, flag) * 2, 8, 16) | \
		gF_(gV4_(v2, v3, v0, v1, flag) * 2, 8, 8) | \
		gF_(gV4_(v3, v0, v1, v2, flag) * 2, 8, 0))

# define gsSPForceMatrix(mptr) \
	gsMoveMem(sizeof(Mtx), G_MV_MATRIX, 0, mptr), \
	gsMoveWd(G_MW_FORCEMTX, 0, 0x10000)

# define gsSPSetGeometryMode(mode) \
	gsSPGeometryMode(0, mode)

# define gsSPClearGeometryMode(mode) \
	gsSPGeometryMode(mode, 0)

# define gsSPLoadGeometryMode(mode) \
	gsSPGeometryMode(~gI_(0), mode)

# define gsSPLineW3D(v0, v1, wd, flag) \
	gO_( \
		G_LINE3D, \
		gF_(gV2_(v0, v1, flag) * 2, 8, 16) | \
		gF_(gV2_(v1, v0, flag) * 2, 8, 8) | \
		gF_(wd, 8, 0), \
		0)

# define gsSPLookAtX(l) \
	gsMoveMem(sizeof(Light), G_MV_LIGHT, G_MVO_LOOKATX, l)

# define gsSPLookAtY(l) \
	gsMoveMem(sizeof(Light), G_MV_LIGHT, G_MVO_LOOKATY, l)

# define gsSPMatrix(matrix, param) \
	gO_( \
		G_MTX, \
		gF_((sizeof(Mtx) - 1) / 8, 5, 19) | \
		gF_(gI_(param) ^ G_MTX_PUSH, 8, 0), \
		matrix)

# define gsSPPopMatrix(param) \
		gsSPPopMatrixN(param, 1)

# define gsSPPopMatrixN(param, n) \
	gO_( \
		G_POPMTX, \
		gF_((sizeof(Mtx) - 1) / 8, 5, 19) | \
		gF_(2, 8, 0), \
		sizeof(Mtx) * (n))

# define gsSPLight(l, n) \
	gsMoveMem(sizeof(Light), G_MV_LIGHT, ((n) + 1) * 0x18, l)

# define gsSPTexture(sc, tc, level, tile, on) \
	gO_( \
		G_TEXTURE, \
		gF_(level, 3, 11) | \
		gF_(tile, 3, 8) | \
		gF_(on, 7, 1), \
		gF_(sc, 16, 16) | \
		gF_(tc, 16, 0))

# define gsSPVertex(v, n, v0) \
	gO_( \
		G_VTX, \
		gF_(n, 8, 12) | \
		gF_((v0) + (n), 7, 1), \
		v)

# define gsSPViewport(v) \
	gsMoveMem(sizeof(Vp), G_MV_VIEWPORT, 0, v)

# define gsSPGeometryMode(clearbits, setbits) \
	gO_( \
		G_GEOMETRYMODE, \
		gF_(~gI_(clearbits), 24, 0), \
		setbits)

# define gsSPSetOtherMode(opc, shift, length, data) \
	gO_( \
		opc, \
		gF_(32 - (shift) - (length), 8, 8) | \
		gF_((length) - 1, 8, 0), \
		data)

# define gsMoveWd(index, offset, data) \
	gO_( \
		G_MOVEWORD, \
		gF_(index, 8, 16) | \
		gF_(offset, 16, 0), \
		data)

# define gsMoveMem(size, index, offset, address) \
	gO_( \
		G_MOVEMEM, \
		gF_((size - 1) / 8, 5, 19) | \
		gF_((offset) / 8, 8, 8) | \
		gF_(index, 8, 0), \
		address)

# define gsSPDma_io(flag, dmem, dram, size) \
	gO_( \
		G_DMA_IO, \
		gF_(flag, 1, 23) | \
		gF_((dmem) / 8, 10, 13) | \
		gF_((size) - 1, 12, 0), \
		dram)

# define gsSPDmaRead(dmem, dram, size) \
	gsSPDma_io(0, dmem, dram, size)

# define gsSPDmaWrite(dmem, dram, size) \
	gsSPDma_io(1, dmem, dram, size)

# define gsSpecial3(hi, lo) \
	gO_(G_SPECIAL_3, hi, lo)

# define gsSpecial2(hi, lo) \
	gO_(G_SPECIAL_2, hi, lo)

# define gsSpecial1(hi, lo) \
	gO_(G_SPECIAL_1, hi, lo)

#endif

/* instruction macros for beta fast3d and f3dex */

#if defined(F3D_BETA) && (defined(F3D_GBI) || defined(F3DEX_GBI))

# define gsSPPerspNormalize(scale) \
	gO_(G_PERSPNORM, 0, scale)

#else

# define gsSPPerspNormalize(scale) \
	gsMoveWd(G_MW_PERSPNORM, 0, scale)

#endif

/* dynamic instruction macros */

#define gDisplayListPut(gdl, ...) \
	({ \
		Gfx Gd_[] = {__VA_ARGS__}; \
		for(size_t Gi_ = 0; Gi_ < sizeof(Gd_) / sizeof(Gfx); Gi_++) \
		{ \
			*(Gfx *)(gdl) = Gd_[Gi_]; \
		} \
		(void)0; \
	})
#define gDisplayListAppend(pgdl, ...) \
	({ \
		Gfx Gd_[] = {__VA_ARGS__}; \
		for(size_t Gi_ = 0; Gi_ < sizeof(Gd_) / sizeof(Gfx); Gi_++) \
		{ \
			*(*(Gfx **)(pgdl))++ = Gd_[Gi_]; \
		} \
		(void)0; \
	})
#define gDisplayListData(pgdl, d) \
	({ \
		Gfx **Gp_ = (void *)(pgdl); \
		struct \
		{ \
			__typeof__(d) v; \
		} *Gd_, *Gs_; \
		*Gp_ -= (sizeof(*Gd_) + sizeof(Gfx) - 1) / sizeof(Gfx); \
		Gd_ = (void *)*Gp_; \
		Gs_ = (void *)&(d); \
		*Gd_ = *Gs_; \
		&Gd_->v; \
	})
#define gDisplayListAlloc(pgdl, s) \
	({ \
		Gfx **Gp_ = (void *)(pgdl); \
		*Gp_ -= ((s) + sizeof(Gfx) - 1) / sizeof(Gfx); \
		(void *)*Gp_; \
	})

#define gDPFillRectangle(gdl, ...) \
	gD_(gdl, gsDPFillRectangle, __VA_ARGS__)
#define gDPScisFillRectangle(gdl, ...) \
	gD_(gdl, gsDPScisFillRectangle, __VA_ARGS__)
#define gDPFullSync(gdl) \
	gDisplayListPut(gdl, gsDPFullSync())
#define gDPLoadSync(gdl) \
	gDisplayListPut(gdl, gsDPLoadSync())
#define gDPTileSync(gdl) \
	gDisplayListPut(gdl, gsDPTileSync())
#define gDPPipeSync(gdl) \
	gDisplayListPut(gdl, gsDPPipeSync())
#define gDPLoadTLUT_pal16(gdl, ...) \
	gD_(gdl, gsDPLoadTLUT_pal16, __VA_ARGS__)
#define gDPLoadTLUT_pal256(gdl, ...) \
	gD_(gdl, gsDPLoadTLUT_pal256, __VA_ARGS__)
#define gDPLoadTextureBlock(gdl, ...) \
	gD_(gdl, gsDPLoadTextureBlock, __VA_ARGS__)
#define gDPLoadTextureBlockS(gdl, ...) \
	gD_(gdl, gsDPLoadTextureBlockS, __VA_ARGS__)
#define gDPLoadTextureBlock_4b(gdl, ...) \
	gD_(gdl, gsDPLoadTextureBlock_4b, __VA_ARGS__)
#define gDPLoadTextureBlock_4bS(gdl, ...) \
	gD_(gdl, gsDPLoadTextureBlock_4bS, __VA_ARGS__)
#define gDPLoadTextureBlockYuv(gdl, ...) \
	gD_(gdl, gsDPLoadTextureBlockYuv, __VA_ARGS__)
#define gDPLoadTextureBlockYuvS(gdl, ...) \
	gD_(gdl, gsDPLoadTextureBlockYuvS, __VA_ARGS__)
#define _gDPLoadTextureBlock(gdl, ...) \
	gD_(gdl, _gsDPLoadTextureBlock, __VA_ARGS__)
#define _gDPLoadTextureBlockS(gdl, ...) \
	gD_(gdl, _gsDPLoadTextureBlockS, __VA_ARGS__)
#define _gDPLoadTextureBlock_4b(gdl, ...) \
	gD_(gdl, _gsDPLoadTextureBlock_4b, __VA_ARGS__)
#define _gDPLoadTextureBlock_4bS(gdl, ...) \
	gD_(gdl, _gsDPLoadTextureBlock_4bS, __VA_ARGS__)
#define _gDPLoadTextureBlockYuv(gdl, ...) \
	gD_(gdl, _gsDPLoadTextureBlockYuv, __VA_ARGS__)
#define _gDPLoadTextureBlockYuvS(gdl, ...) \
	gD_(gdl, _gsDPLoadTextureBlockYuvS, __VA_ARGS__)
#define gDPLoadMultiBlock(gdl, ...) \
	gD_(gdl, gsDPLoadMultiBlock, __VA_ARGS__)
#define gDPLoadMultiBlockS(gdl, ...) \
	gD_(gdl, gsDPLoadMultiBlockS, __VA_ARGS__)
#define gDPLoadMultiBlock_4b(gdl, ...) \
	gD_(gdl, gsDPLoadMultiBlock_4b, __VA_ARGS__)
#define gDPLoadMultiBlock_4bS(gdl, ...) \
	gD_(gdl, gsDPLoadMultiBlock_4bS, __VA_ARGS__)
#define gDPLoadMultiBlockYuv(gdl, ...) \
	gD_(gdl, gsDPLoadMultiBlockYuv, __VA_ARGS__)
#define gDPLoadMultiBlockYuvS(gdl, ...) \
	gD_(gdl, gsDPLoadMultiBlockYuvS, __VA_ARGS__)
#define gDPLoadTextureTile(gdl, ...) \
	gD_(gdl, gsDPLoadTextureTile, __VA_ARGS__)
#define gDPLoadTextureTile_4b(gdl, ...) \
	gD_(gdl, gsDPLoadTextureTile_4b, __VA_ARGS__)
#define gDPLoadTextureTileYuv(gdl, ...) \
	gD_(gdl, gsDPLoadTextureTileYuv, __VA_ARGS__)
#define _gDPLoadTextureTile(gdl, ...) \
	gD_(gdl, _gsDPLoadTextureTile, __VA_ARGS__)
#define _gDPLoadTextureTile_4b(gdl, ...) \
	gD_(gdl, _gsDPLoadTextureTile_4b, __VA_ARGS__)
#define _gDPLoadTextureTileYuv(gdl, ...) \
	gD_(gdl, _gsDPLoadTextureTileYuv, __VA_ARGS__)
#define gDPLoadMultiTile(gdl, ...) \
	gD_(gdl, gsDPLoadMultiTile, __VA_ARGS__)
#define gDPLoadMultiTile_4b(gdl, ...) \
	gD_(gdl, gsDPLoadMultiTile_4b, __VA_ARGS__)
#define gDPLoadMultiTileYuv(gdl, ...) \
	gD_(gdl, gsDPLoadMultiTileYuv, __VA_ARGS__)
#define gDPLoadBlock(gdl, ...) \
	gD_(gdl, gsDPLoadBlock, __VA_ARGS__)
#define gDPNoOp(gdl) \
	gDisplayListPut(gdl, gsDPNoOp())
#define gDPNoOpTag(gdl, ...) \
	gD_(gdl, gsDPNoOpTag, __VA_ARGS__)
#define gDPPipelineMode(gdl, ...) \
	gD_(gdl, gsDPPipelineMode, __VA_ARGS__)
#define gDPSetBlendColor(gdl, ...) \
	gD_(gdl, gsDPSetBlendColor, __VA_ARGS__)
#define gDPSetEnvColor(gdl, ...) \
	gD_(gdl, gsDPSetEnvColor, __VA_ARGS__)
#define gDPSetFillColor(gdl, ...) \
	gD_(gdl, gsDPSetFillColor, __VA_ARGS__)
#define gDPSetFogColor(gdl, ...) \
	gD_(gdl, gsDPSetFogColor, __VA_ARGS__)
#define gDPSetPrimColor(gdl, ...) \
	gD_(gdl, gsDPSetPrimColor, __VA_ARGS__)
#define gDPSetColorImage(gdl, ...) \
	gD_(gdl, gsDPSetColorImage, __VA_ARGS__)
#define gDPSetDepthImage(gdl, ...) \
	gD_(gdl, gsDPSetDepthImage, __VA_ARGS__)
#define gDPSetTextureImage(gdl, ...) \
	gD_(gdl, gsDPSetTextureImage, __VA_ARGS__)
#define gDPSetHilite1Tile(gdl, ...) \
	gD_(gdl, gsDPSetHilite1Tile, __VA_ARGS__)
#define gDPSetHilite2Tile(gdl, ...) \
	gD_(gdl, gsDPSetHilite2Tile, __VA_ARGS__)
#define gDPSetAlphaCompare(gdl, ...) \
	gD_(gdl, gsDPSetAlphaCompare, __VA_ARGS__)
#define gDPSetAlphaDither(gdl, ...) \
	gD_(gdl, gsDPSetAlphaDither, __VA_ARGS__)
#define gDPSetColorDither(gdl, ...) \
	gD_(gdl, gsDPSetColorDither, __VA_ARGS__)
#define gDPSetCombineMode(gdl, ...) \
	gD_(gdl, gsDPSetCombineLERP, __VA_ARGS__)
#define gDPSetCombineLERP(gdl, ...) \
	gD_(gdl, gsDPSetCombineLERP, __VA_ARGS__)
#define gDPSetConvert(gdl, ...) \
	gD_(gdl, gsDPSetConvert, __VA_ARGS__)
#define gDPSetTextureConvert(gdl, ...) \
	gD_(gdl, gsDPSetTextureConvert, __VA_ARGS__)
#define gDPSetCycleType(gdl, ...) \
	gD_(gdl, gsDPSetCycleType, __VA_ARGS__)
#define gDPSetDepthSource(gdl, ...) \
	gD_(gdl, gsDPSetDepthSource, __VA_ARGS__)
#define gDPSetCombineKey(gdl, ...) \
	gD_(gdl, gsDPSetCombineKey, __VA_ARGS__)
#define gDPSetKeyGB(gdl, ...) \
	gD_(gdl, gsDPSetKeyGB, __VA_ARGS__)
#define gDPSetKeyR(gdl, ...) \
	gD_(gdl, gsDPSetKeyR, __VA_ARGS__)
#define gDPSetPrimDepth(gdl, ...) \
	gD_(gdl, gsDPSetPrimDepth, __VA_ARGS__)
#define gDPSetRenderMode(gdl, ...) \
	gD_(gdl, gsDPSetRenderMode, __VA_ARGS__)
#define gDPSetScissor(gdl, ...) \
	gD_(gdl, gsDPSetScissor, __VA_ARGS__)
#define gDPSetScissorFrac(gdl, ...) \
	gD_(gdl, gsDPSetScissorFrac, __VA_ARGS__)
#define gDPSetTextureDetail(gdl, ...) \
	gD_(gdl, gsDPSetTextureDetail, __VA_ARGS__)
#define gDPSetTextureFilter(gdl, ...) \
	gD_(gdl, gsDPSetTextureFilter, __VA_ARGS__)
#define gDPSetTextureLOD(gdl, ...) \
	gD_(gdl, gsDPSetTextureLOD, __VA_ARGS__)
#define gDPSetTextureLUT(gdl, ...) \
	gD_(gdl, gsDPSetTextureLUT, __VA_ARGS__)
#define gDPSetTexturePersp(gdl, ...) \
	gD_(gdl, gsDPSetTexturePersp, __VA_ARGS__)
#define gDPSetTile(gdl, ...) \
	gD_(gdl, gsDPSetTile, __VA_ARGS__)
#define gDPSetTileSize(gdl, ...) \
	gD_(gdl, gsDPSetTileSize, __VA_ARGS__)
#define gSP1Triangle(gdl, ...) \
	gD_(gdl, gsSP1Triangle, __VA_ARGS__)
#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
# define gSP2Triangles(gdl, ...) \
	gD_(gdl, gsSP2Triangles, __VA_ARGS__)
# define gSP1Quadrangle(gdl, ...) \
	gD_(gdl, gsSP1Quadrangle, __VA_ARGS__)
# define gSPBranchLessZ(gdl, ...) \
	gD_(gdl, gsSPBranchLessZ, __VA_ARGS__)
# define gSPBranchLessZrg(gdl, ...) \
	gD_(gdl, gsSPBranchLessZrg, __VA_ARGS__)
# define gSPBranchLessZraw(gdl, ...) \
	gD_(gdl, gsSPBranchLessZraw, __VA_ARGS__)
#endif
#define gSPBranchList(gdl, ...) \
	gD_(gdl, gsSPBranchList, __VA_ARGS__)
#define gSPClipRatio(gdl, ...) \
	gD_(gdl, gsSPClipRatio, __VA_ARGS__)
#define gSPCullDisplayList(gdl, ...) \
	gD_(gdl, gsSPCullDisplayList, __VA_ARGS__)
#define gSPDisplayList(gdl, ...) \
	gD_(gdl, gsSPDisplayList, __VA_ARGS__)
#define gSPEndDisplayList(gdl) \
	gDisplayListPut(gdl, gsSPEndDisplayList())
#define gSPFogFactor(gdl, ...) \
	gD_(gdl, gsSPFogFactor, __VA_ARGS__)
#define gSPFogPosition(gdl, ...) \
	gD_(gdl, gsSPFogPosition, __VA_ARGS__)
#define gSPForceMatrix(gdl, ...) \
	gD_(gdl, gsSPForceMatrix, __VA_ARGS__)
#define gSPSetGeometryMode(gdl, ...) \
	gD_(gdl, gsSPSetGeometryMode, __VA_ARGS__)
#define gSPClearGeometryMode(gdl, ...) \
	gD_(gdl, gsSPClearGeometryMode, __VA_ARGS__)
#define gSPLoadGeometryMode(gdl, ...) \
	gD_(gdl, gsSPLoadGeometryMode, __VA_ARGS__)
#if defined(F3D_GBI) || defined(F3DEX_GBI)
# define gSPInsertMatrix(gdl, ...) \
	gD_(gdl, gsSPInsertMatrix, __VA_ARGS__)
#endif
#define gSPLine3D(gdl, ...) \
	gD_(gdl, gsSPLine3D, __VA_ARGS__)
#define gSPLineW3D(gdl, ...) \
	gD_(gdl, gsSPLineW3D, __VA_ARGS__)
#define gSPLoadUcode(gdl, ...) \
	gD_(gdl, gsSPLoadUcode, __VA_ARGS__)
#define gSPLoadUcodeL(gdl, ...) \
	gD_(gdl, gsSPLoadUcodeL, __VA_ARGS__)
#define gSPLookAtX(gdl, ...) \
	gD_(gdl, gsSPLookAtX, __VA_ARGS__)
#define gSPLookAtY(gdl, ...) \
	gD_(gdl, gsSPLookAtY, __VA_ARGS__)
#define gSPLookAt(gdl, ...) \
	gD_(gdl, gsSPLookAt, __VA_ARGS__)
#define gSPMatrix(gdl, ...) \
	gD_(gdl, gsSPMatrix, __VA_ARGS__)
#define gSPModifyVertex(gdl, ...) \
	gD_(gdl, gsSPModifyVertex, __VA_ARGS__)
#define gSPPerspNormalize(gdl, ...) \
	gD_(gdl, gsSPPerspNormalize, __VA_ARGS__)
#define gSPPopMatrix(gdl, ...) \
	gD_(gdl, gsSPPopMatrix, __VA_ARGS__)
#if defined(F3DEX_GBI_2)
# define gSPPopMatrixN(gdl, ...) \
	gD_(gdl, gsSPPopMatrixN, __VA_ARGS__)
#endif
#define gSPSegment(gdl, ...) \
	gD_(gdl, gsSPSegment, __VA_ARGS__)
#define gSPSetLights0(gdl, ...) \
	gD_(gdl, gsSPSetLights0, __VA_ARGS__)
#define gSPSetLights1(gdl, ...) \
	gD_(gdl, gsSPSetLights1, __VA_ARGS__)
#define gSPSetLights2(gdl, ...) \
	gD_(gdl, gsSPSetLights2, __VA_ARGS__)
#define gSPSetLights3(gdl, ...) \
	gD_(gdl, gsSPSetLights3, __VA_ARGS__)
#define gSPSetLights4(gdl, ...) \
	gD_(gdl, gsSPSetLights4, __VA_ARGS__)
#define gSPSetLights5(gdl, ...) \
	gD_(gdl, gsSPSetLights5, __VA_ARGS__)
#define gSPSetLights6(gdl, ...) \
	gD_(gdl, gsSPSetLights6, __VA_ARGS__)
#define gSPSetLights7(gdl, ...) \
	gD_(gdl, gsSPSetLights7, __VA_ARGS__)
#define gSPSetStatus(gdl, ...) \
	gD_(gdl, gsSPSetStatus, __VA_ARGS__)
#define gSPNumLights(gdl, ...) \
	gD_(gdl, gsSPNumLights, __VA_ARGS__)
#define gSPLight(gdl, ...) \
	gD_(gdl, gsSPLight, __VA_ARGS__)
#define gSPLightColor(gdl, ...) \
	gD_(gdl, gsSPLightColor, __VA_ARGS__)
#define gSPTexture(gdl, ...) \
	gD_(gdl, gsSPTexture, __VA_ARGS__)
#define gSPTextureRectangle(gdl, ...) \
	gD_(gdl, gsSPTextureRectangle, __VA_ARGS__)
#define gSPScisTextureRectangle(gdl, ...) \
	gD_(gdl, gsSPScisTextureRectangle, __VA_ARGS__)
#define gSPTextureRectangleFlip(gdl, ...) \
	gD_(gdl, gsSPTextureRectangleFlip, __VA_ARGS__)
#define gSPScisTextureRectangleFlip(gdl, ...) \
	gD_(gdl, gsSPScisTextureRectangleFlip, __VA_ARGS__)
#define gSPVertex(gdl, ...) \
	gD_(gdl, gsSPVertex, __VA_ARGS__)
#define gSPViewport(gdl, ...) \
	gD_(gdl, gsSPViewport, __VA_ARGS__)
#define gSPBgRectCopy(gdl, ...) \
	gD_(gdl, gsSPBgRectCopy, __VA_ARGS__)
#define gSPBgRect1Cyc(gdl, ...) \
	gD_(gdl, gsSPBgRect1Cyc, __VA_ARGS__)
#define gSPObjRectangle(gdl, ...) \
	gD_(gdl, gsSPObjRectangle, __VA_ARGS__)
#define gSPObjRectangleR(gdl, ...) \
	gD_(gdl, gsSPObjRectangleR, __VA_ARGS__)
#define gSPObjSprite(gdl, ...) \
	gD_(gdl, gsSPObjSprite, __VA_ARGS__)
#define gSPObjMatrix(gdl, ...) \
	gD_(gdl, gsSPObjMatrix, __VA_ARGS__)
#define gSPObjSubMatrix(gdl, ...) \
	gD_(gdl, gsSPObjSubMatrix, __VA_ARGS__)
#define gSPObjRenderMode(gdl, ...) \
	gD_(gdl, gsSPObjRenderMode, __VA_ARGS__)
#define gSPObjLoadTxtr(gdl, ...) \
	gD_(gdl, gsSPObjLoadTxtr, __VA_ARGS__)
#define gSPObjLoadTxRect(gdl, ...) \
	gD_(gdl, gsSPObjLoadTxRect, __VA_ARGS__)
#define gSPObjLoadTxRectR(gdl, ...) \
	gD_(gdl, gsSPObjLoadTxRectR, __VA_ARGS__)
#define gSPObjLoadTxSprite(gdl, ...) \
	gD_(gdl, gsSPObjLoadTxSprite, __VA_ARGS__)
#define gSPSelectDL(gdl, ...) \
	gD_(gdl, gsSPSelectDL, __VA_ARGS__)
#define gSPSelectBranchDL(gdl, ...) \
	gD_(gdl, gsSPSelectBranchDL, __VA_ARGS__)
#define gDPLoadTLUTCmd(gdl, ...) \
	gD_(gdl, gsDPLoadTLUTCmd, __VA_ARGS__)
#define gDPLoadTLUT(gdl, ...) \
	gD_(gdl, gsDPLoadTLUT, __VA_ARGS__)
#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
# define gBranchZ(gdl, ...) \
	gD_(gdl, gsBranchZ, __VA_ARGS__)
#endif
#define gDisplayList(gdl, ...) \
	gD_(gdl, gsDisplayList, __VA_ARGS__)
#define gDPHalf1(gdl, ...) \
	gD_(gdl, gsDPHalf1, __VA_ARGS__)
#define gDPHalf2(gdl, ...) \
	gD_(gdl, gsDPHalf2, __VA_ARGS__)
#define gDPLoadTile(gdl, ...) \
	gD_(gdl, gsDPLoadTile, __VA_ARGS__)
#define gDPSetCombine(gdl, ...) \
	gD_(gdl, gsDPSetCombine, __VA_ARGS__)
#if defined(F3DEX_GBI_2)
# define gSPGeometryMode(gdl, ...) \
	gD_(gdl, gsSPGeometryMode, __VA_ARGS__)
#endif
#define gSPSetOtherMode(gdl, ...) \
	gD_(gdl, gsSPSetOtherMode, __VA_ARGS__)
#define gSPSetOtherModeLo(gdl, ...) \
	gD_(gdl, gsSPSetOtherModeLo, __VA_ARGS__)
#define gSPSetOtherModeHi(gdl, ...) \
	gD_(gdl, gsSPSetOtherModeHi, __VA_ARGS__)
#define gDPSetOtherMode(gdl, ...) \
	gD_(gdl, gsDPSetOtherMode, __VA_ARGS__)
#define gMoveWd(gdl, ...) \
	gD_(gdl, gsMoveWd, __VA_ARGS__)
#define gMoveMem(gdl, ...) \
	gD_(gdl, gsMoveMem, __VA_ARGS__)
#if defined(F3DEX_GBI_2)
# define gSPDma_io(gdl, ...) \
	gD_(gdl, gsSPDma_io, __VA_ARGS__)
# define gSPDmaRead(gdl, ...) \
	gD_(gdl, gsSPDmaRead, __VA_ARGS__)
# define gSPDmaWrite(gdl, ...) \
	gD_(gdl, gsSPDmaWrite, __VA_ARGS__)
#endif
#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
# define gLoadUcode(gdl, ...) \
	gD_(gdl, gsLoadUcode, __VA_ARGS__)
# define gSPLoadUcodeEx(gdl, ...) \
	gD_(gdl, gsSPLoadUcodeEx, __VA_ARGS__)
#endif
#define gTexRect(gdl, ...) \
	gD_(gdl, gsTexRect, __VA_ARGS__)
#define gTexRectFlip(gdl, ...) \
	gD_(gdl, gsTexRectFlip, __VA_ARGS__)
#define gSPNoOp(gdl) \
	gDisplayListPut(gdl, gsSPNoOp())
#define gDPWord(gdl, ...) \
	gD_(gdl, gsDPWord, __VA_ARGS__)
#if defined(F3DEX_GBI_2)
# define gSpecial3(gdl, ...) \
	gD_(gdl, gsSpecial3, __VA_ARGS__)
# define gSpecial2(gdl, ...) \
	gD_(gdl, gsSpecial2, __VA_ARGS__)
# define gSpecial1(gdl, ...) \
	gD_(gdl, gsSpecial1, __VA_ARGS__)
#endif

/* data types and structures */
typedef uint8_t		qu08_t;
typedef uint16_t	qu016_t;
typedef int16_t		qs48_t;
typedef int16_t		qs510_t;
typedef uint16_t	qu510_t;
typedef int16_t		qs102_t;
typedef uint16_t	qu102_t;
typedef int16_t		qs105_t;
typedef uint16_t	qu105_t;
typedef int16_t		qs132_t;
typedef int16_t		qs142_t;
typedef int32_t		qs1516_t;
typedef int32_t		qs1616_t;
typedef int32_t		qs205_t;

typedef uint16_t	g_bglt_t;
typedef uint8_t		g_ifmt_t;
typedef uint8_t		g_isiz_t;
typedef uint16_t	g_bgf_t;
typedef uint8_t		g_objf_t;
typedef uint32_t	g_objlt_t;

typedef struct
{
	_Alignas(8)
	uint32_t	hi;
	uint32_t	lo;
} Gfx;

typedef struct
{
	int32_t		x1;
	int32_t		y1;
	int32_t		x2;
	int32_t		y2;
} Hilite_t;

typedef union
{
	_Alignas(8)
	Hilite_t	h;
} Hilite;

typedef int32_t Mtx_t[4][4];

typedef union
{
	_Alignas(8)
	Mtx_t		m;
	int32_t		l[16];
	struct
	{
		int16_t		i[16];
		uint16_t	f[16];
	};
} Mtx;

typedef struct
{
	uint8_t		col[3];
	char		pad1;
	uint8_t		colc[3];
	char		pad2;
	int8_t		dir[3];
	char		pad3;
} Light_t;

typedef union
{
	_Alignas(8)
	Light_t		l;
} Light;

typedef struct
{
	Light		l[2];
} LookAt;

typedef struct
{
	uint8_t		col[3];
	char		pad1;
	uint8_t		colc[3];
	char		pad2;
} Ambient_t;

typedef union
{
	_Alignas(8)
	Ambient_t	l;
} Ambient;

typedef struct
{
	Ambient		a;
	Light		l[1];
} Lights0, Lights1;

typedef struct
{
	Ambient		a;
	Light		l[2];
} Lights2;

typedef struct
{
	Ambient		a;
	Light		l[3];
} Lights3;

typedef struct
{
	Ambient		a;
	Light		l[4];
} Lights4;

typedef struct
{
	Ambient		a;
	Light		l[5];
} Lights5;

typedef struct
{
	Ambient		a;
	Light		l[6];
} Lights6;

typedef struct
{
	Ambient		a;
	Light		l[7];
} Lightsn, Lights7;

typedef struct
{
	int16_t		ob[3];
	uint16_t	flag;
	qs105_t		tc[2];
	uint8_t		cn[4];
} Vtx_t;

typedef struct
{
	int16_t		ob[3];
	uint16_t	flag;
	qs105_t		tc[2];
	int8_t		n[3];
	uint8_t		a;
} Vtx_tn;

typedef union
{
	_Alignas(8)
	Vtx_t		v;
	Vtx_tn		n;
} Vtx;

typedef struct
{
	qs142_t		vscale[4];
	qs142_t		vtrans[4];
} Vp_t;

typedef union
{
	_Alignas(8)
	Vp_t		vp;
} Vp;

typedef struct
{
	qs1516_t	A;
	qs1516_t	B;
	qs1516_t	C;
	qs1516_t	D;
	qs102_t		X;
	qs102_t		Y;
	qu510_t		BaseScaleX;
	qu510_t		BaseScaleY;
} uObjMtx_t;

typedef union
{
	_Alignas(8)
	uObjMtx_t	m;
} uObjMtx;

typedef struct
{
	qs102_t		X;
	qs102_t		Y;
	qu510_t		BaseScaleX;
	qu510_t		BaseScaleY;
} uObjSubMtx_t;

typedef union
{
	_Alignas(8)
	uObjSubMtx_t	m;
} uObjSubMtx;

typedef struct
{
	qu105_t		imageX;
	qu102_t		imageW;
	qs102_t		frameX;
	qu102_t		frameW;
	qu105_t		imageY;
	qu102_t		imageH;
	qs102_t		frameY;
	qu102_t		frameH;
	uint64_t *	imagePtr;
	g_bglt_t	imageLoad;
	g_ifmt_t	imageFmt;
	g_isiz_t	imageSiz;
	uint16_t	imagePal;
	g_bgf_t		imageFlip;
	uint16_t	tmemW;
	qs132_t		tmemH;
	uint16_t	tmemLoadSH;
	uint16_t	tmemLoadTH;
	uint16_t	tmemSizeW;
	uint16_t	tmemSize;
} uObjBg_t;

typedef struct
{
	qu105_t		imageX;
	qu102_t		imageW;
	qs102_t		frameX;
	qu102_t		frameW;
	qu105_t		imageY;
	qu102_t		imageH;
	qs102_t		frameY;
	qu102_t		frameH;
	uint64_t *	imagePtr;
	g_bglt_t	imageLoad;
	g_ifmt_t	imageFmt;
	g_isiz_t	imageSiz;
	uint16_t	imagePal;
	g_bgf_t		imageFlip;
	qu510_t		scaleW;
	qu510_t		scaleH;
	qs205_t		imageYorig;
	char		padding[4];
} uObjScaleBg_t;

typedef union
{
	_Alignas(8)
	uObjBg_t	b;
	uObjScaleBg_t	s;
} uObjBg;

typedef struct
{
	qs102_t		objX;
	qu510_t		scaleW;
	qu105_t		imageW;
	uint16_t	paddingX;
	qs102_t		objY;
	qu510_t		scaleH;
	qu105_t		imageH;
	uint16_t	paddingY;
	uint16_t	imageStride;
	uint16_t	imageAdrs;
	g_ifmt_t	imageFmt;
	g_isiz_t	imageSiz;
	uint16_t	imagePal;
	g_objf_t	imageFlags;
} uObjSprite_t;

typedef union
{
	_Alignas(8)
	uObjSprite_t	s;
} uObjSprite;

typedef struct
{
	g_objlt_t	type;
	uint64_t *	image;
	uint16_t	tmem;
	uint16_t	tsize;
	uint16_t	tline;
	uint16_t	sid;
	uint32_t	flag;
	uint32_t	mask;
} uObjTxtrBlock_t;

typedef struct
{
	g_objlt_t	type;
	uint64_t *	image;
	uint16_t	tmem;
	uint16_t	twidth;
	uint16_t	theight;
	uint16_t	sid;
	uint32_t	flag;
	uint32_t	mask;
} uObjTxtrTile_t;

typedef struct
{
	g_objlt_t	type;
	uint64_t *	image;
	uint16_t	phead;
	uint16_t	pnum;
	uint16_t	zero;
	uint16_t	sid;
	uint32_t	flag;
	uint32_t	mask;
} uObjTxtrTLUT_t;

typedef union
{
	_Alignas(8)
	uObjTxtrBlock_t	block;
	uObjTxtrTile_t	tile;
	uObjTxtrTLUT_t	tlut;
} uObjTxtr;

typedef struct
{
	uObjTxtr	txtr;
	uObjSprite	sprite;
} uObjTxSprite;

/* rectangle scissoring macros */
#define gScC_(c) ((c) < 0 ? 0 : (c))
#define gScD_(t, c, d) \
	( \
		(c) < 0 ? \
		( \
			(d) < 0 ? \
				(t) + (c) * (d) / 0x80 : \
				(t) - (c) * (d) / 0x80 \
		) : \
		(t) \
	)

/* texture loading helper macros */
#define G_SIZ_LDSIZ(siz)	((siz) < G_IM_SIZ_16b ? G_IM_SIZ_16b : (siz))
#define G_SIZ_BITS(siz)		(4 << gI_(siz))
#define G_SIZ_LDBITS(siz)	((siz) < G_IM_SIZ_16b ? G_SIZ_BITS(siz) : 16)
#define G_DXT(siz, width) \
	( \
		(width) * G_SIZ_BITS(siz) > 64 ? \
			((1 << 11) + (width) * G_SIZ_BITS(siz) / 64 - 1) / \
			((width) * G_SIZ_BITS(siz) / 64) : \
			(1 << 11) \
	)
#define G_LTB_LRS(width, height, siz) \
	( \
		(((width) * (height) + 1) * G_SIZ_BITS(siz) - 1) / \
		G_SIZ_BITS(G_SIZ_LDSIZ(siz)) - 1 \
	)
#define G_LDBLK_TXL(txl) \
	( \
		(txl) > G_TX_LDBLK_MAX_TXL ? \
			G_TX_LDBLK_MAX_TXL : \
			(txl) \
	)

/* depth value macros */
#define gZp_(zval, near, far) \
	( \
		(1.f - (float)(near) / (float)(zval)) / \
		(1.f - (float)(near) / (float)(far)) \
	)
#define gZo_(zval, near, far) \
	( \
		((float)(zval) - (float)(near)) / \
		((float)(far) - (float)(near)) \
	)
#define gZf_(zval, near, far, flag) \
	qs1616 \
	( \
		(flag) == G_BZ_PERSP ? \
			gZp_(zval, near, far) : \
			gZo_(zval, near, far) \
	)
#define G_DEPTOZSrg(zval, near, far, flag, zmin, zmax) \
	( \
		gZf_(zval, near, far, flag) * \
		((int32_t)((zmax) - (zmin)) & ~(int32_t)1) + \
		qs1616(zmin) \
	)
#define G_DEPTOZS(zval, near, far, flag) \
	G_DEPTOZSrg(zval, near, far, flag, 0, G_MAXZ)

/* vertex ordering macros */
#define gV2_(v0, v1, flag) \
	( \
		(flag) == 0 ? gI_(v0) : \
		gI_(v1) \
	)
#define gV3_(v0, v1, v2, flag) \
	( \
		(flag) == 0 ? gI_(v0) : \
		(flag) == 1 ? gI_(v1) : \
		gI_(v2) \
	)
#define gV4_(v0, v1, v2, v3, flag) \
	( \
		(flag) == 0 ? gI_(v0) : \
		(flag) == 1 ? gI_(v1) : \
		(flag) == 2 ? gI_(v2) : \
		gI_(v3) \
	)

/* sprite texture parameter macros */
#define GS_PIX2TMEM(pix, siz)	((pix) * G_SIZ_BITS(siz) / 64)
#define GS_TB_TSIZE(pix, siz)	(GS_PIX2TMEM(pix, siz) - 1)
#define GS_TB_TLINE(pix, siz)	(((1 << 11) - 1) / GS_PIX2TMEM(pix, siz) + 1)
#define GS_TT_TWIDTH(pix, siz)	(GS_PIX2TMEM(pix, siz) * 4 - 1)
#define GS_TT_THEIGHT(pix, siz)	((pix) * 4 - 1)
#define GS_PAL_HEAD(head)	((head) + 256)
#define GS_PAL_NUM(num)		((num) - 1)

/* fixed-point conversion macros */
#define qu08(n)			((qu08_t)((n) * 0x100))
#define qu016(n)		((qu016_t)((n) * 0x10000))
#define qs48(n)			((qs48_t)((n) * 0x0100))
#define qs510(n)		((qs510_t)((n) * 0x0400))
#define qu510(n)		((qu510_t)((n) * 0x0400))
#define qs102(n)		((qs102_t)((n) * 0x0004))
#define qu102(n)		((qu102_t)((n) * 0x0004))
#define qs105(n)		((qs105_t)((n) * 0x0020))
#define qu105(n)		((qu105_t)((n) * 0x0020))
#define qs132(n)		((qs132_t)((n) * 0x0004))
#define qs142(n)		((qs142_t)((n) * 0x0004))
#define qs1516(n)		((qs1516_t)((n) * 0x00010000))
#define qs1616(n)		((qs1616_t)((n) * 0x00010000))
#define qs205(n)		((qs205_t)((n) * 0x00000020))

/* private helper macros */
#define gI_(i)			((uint32_t)(i))
#define gL_(l)			((uint64_t)(l))
#define gF_(i, n, s)		((gI_(i) & ((gI_(1) << (n)) - 1)) << (s))
#define gFL_(l, n, s)		((gL_(l) & ((gL_(1) << (n)) - 1)) << (s))
#define gO_(opc, hi, lo)	((Gfx){gF_(opc, 8, 24) | gI_(hi), gI_(lo)})
#define gD_(gdl, m, ...)	gDisplayListPut(gdl, m(__VA_ARGS__))

#endif
