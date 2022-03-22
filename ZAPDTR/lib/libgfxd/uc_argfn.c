#define MDMASK(md)	((((uint32_t)1 << G_MDSIZ_##md) - 1) << G_MDSFT_##md)
#define MDMASK_RM_C1	((uint32_t)0xCCCC0000)
#define MDMASK_RM_C2	((uint32_t)0x33330000)
#define MDMASK_RM_LO	((uint32_t)0x0000FFF8)

UCFUNC int argfn_i(const gfxd_value_t *v)
{
	return gfxd_printf("%" PRIi32, v->i);
}

UCFUNC int argfn_u(const gfxd_value_t *v)
{
	return gfxd_printf("%" PRIu32, v->u);
}

UCFUNC int argfn_x8(const gfxd_value_t *v)
{
	return gfxd_printf("0x%02" PRIX32, v->u);
}

UCFUNC int argfn_x16(const gfxd_value_t *v)
{
	return gfxd_printf("0x%04" PRIX32, v->u);
}

UCFUNC int argfn_x32(const gfxd_value_t *v)
{
	return gfxd_printf("0x%08" PRIX32, v->u);
}

UCFUNC int argfn_color(const gfxd_value_t *v)
{
	if (config.emit_dec_color)
		return gfxd_printf("%" PRIu32, v->u);
	else
		return gfxd_printf("0x%02" PRIX32, v->u);
}

UCFUNC int argfn_qu08(const gfxd_value_t *v)
{
	if (v->u == 0)
		return gfxd_puts("0");
	else if (config.emit_q_macro)
		return gfxd_printf("qu08(%.16g)", v->u / 256.f);
	else
		return gfxd_printf("0x%02" PRIX32, v->u);
}

UCFUNC int argfn_qu016(const gfxd_value_t *v)
{
	if (v->u == 0)
		return gfxd_puts("0");
	else if (config.emit_q_macro)
		return gfxd_printf("qu016(%.16g)", v->u / 65536.f);
	else
		return gfxd_printf("0x%04" PRIX32, v->u);
}

UCFUNC int argfn_qs48(const gfxd_value_t *v)
{
	if (v->i == 0)
		return gfxd_puts("0");
	else if (config.emit_q_macro)
		return gfxd_printf("qs48(%.16g)", v->i / 256.f);
	else
	{
		if (v->i < 0)
			return gfxd_printf("-0x%04" PRIX32, (uint32_t)-v->i);
		else
			return gfxd_printf("0x%04" PRIX32, (uint32_t)v->i);
	}
}

UCFUNC int argfn_qs510(const gfxd_value_t *v)
{
	if (v->i == 0)
		return gfxd_puts("0");
	else if (config.emit_q_macro)
		return gfxd_printf("qs510(%.16g)", v->i / 1024.f);
	else
	{
		if (v->i < 0)
			return gfxd_printf("-0x%04" PRIX32, (uint32_t)-v->i);
		else
			return gfxd_printf("0x%04" PRIX32, (uint32_t)v->i);
	}
}

UCFUNC int argfn_qu102(const gfxd_value_t *v)
{
	if (v->u == 0)
		return gfxd_puts("0");
	else if (config.emit_q_macro)
		return gfxd_printf("qu102(%.16g)", v->u / 4.f);
	else
		return gfxd_printf("0x%04" PRIX32, v->u);
}

UCFUNC int argfn_qs105(const gfxd_value_t *v)
{
	if (v->i == 0)
		return gfxd_puts("0");
	else if (config.emit_q_macro)
		return gfxd_printf("qs105(%.16g)", v->i / 32.f);
	else
	{
		if (v->i < 0)
			return gfxd_printf("-0x%04" PRIX32, (uint32_t)-v->i);
		else
			return gfxd_printf("0x%04" PRIX32, (uint32_t)v->i);
	}
}

UCFUNC int argfn_qs1616(const gfxd_value_t *v)
{
	if (v->i == 0)
		return gfxd_puts("0");
	else if (config.emit_q_macro)
		return gfxd_printf("qs1616(%.16g)", v->i / 65536.f);
	else
	{
		if (v->i < 0)
			return gfxd_printf("-0x%08" PRIX32, (uint32_t)-v->i);
		else
			return gfxd_printf("0x%08" PRIX32, (uint32_t)v->i);
	}
}

UCFUNC int argfn_opc(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_SPNOOP:
			return gfxd_puts("G_SPNOOP");
		case G_MTX:
			return gfxd_puts("G_MTX");
		case G_MOVEMEM:
			return gfxd_puts("G_MOVEMEM");
		case G_VTX:
			return gfxd_puts("G_VTX");
		case G_DL:
			return gfxd_puts("G_DL");
		case G_RDPHALF_2:
			return gfxd_puts("G_RDPHALF_2");
		case G_RDPHALF_1:
			return gfxd_puts("G_RDPHALF_1");
#if defined(F3D_BETA) && (defined(F3D_GBI) || defined(F3DEX_GBI))
		case G_PERSPNORM:
			return gfxd_puts("G_PERSPNORM");
#endif
		case G_LINE3D:
			return gfxd_puts("G_LINE3D");
#if defined(F3D_GBI) || defined(F3DEX_GBI)
		case G_CLEARGEOMETRYMODE:
			return gfxd_puts("G_CLEARGEOMETRYMODE");
		case G_SETGEOMETRYMODE:
			return gfxd_puts("G_SETGEOMETRYMODE");
#endif
		case G_ENDDL:
			return gfxd_puts("G_ENDDL");
		case G_SETOTHERMODE_L:
			return gfxd_puts("G_SETOTHERMODE_L");
		case G_SETOTHERMODE_H:
			return gfxd_puts("G_SETOTHERMODE_H");
		case G_TEXTURE:
			return gfxd_puts("G_TEXTURE");
		case G_MOVEWORD:
			return gfxd_puts("G_MOVEWORD");
		case G_POPMTX:
			return gfxd_puts("G_POPMTX");
		case G_CULLDL:
			return gfxd_puts("G_CULLDL");
		case G_TRI1:
			return gfxd_puts("G_TRI1");
		case G_NOOP:
			return gfxd_puts("G_NOOP");
#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
		case G_LOAD_UCODE:
			return gfxd_puts("G_LOAD_UCODE");
		case G_BRANCH_Z:
			return gfxd_puts("G_BRANCH_Z");
		case G_TRI2:
			return gfxd_puts("G_TRI2");
# if !(defined(F3D_BETA) && defined(F3DEX_GBI))
		case G_MODIFYVTX:
			return gfxd_puts("G_MODIFYVTX");
# endif
#endif
#if defined(F3DEX_GBI_2)
		case G_QUAD:
			return gfxd_puts("G_QUAD");
		case G_SPECIAL_3:
			return gfxd_puts("G_SPECIAL_3");
		case G_SPECIAL_2:
			return gfxd_puts("G_SPECIAL_2");
		case G_SPECIAL_1:
			return gfxd_puts("G_SPECIAL_1");
		case G_DMA_IO:
			return gfxd_puts("G_DMA_IO");
		case G_GEOMETRYMODE:
			return gfxd_puts("G_GEOMETRYMODE");
#endif
		case G_TEXRECT:
			return gfxd_puts("G_TEXRECT");
		case G_TEXRECTFLIP:
			return gfxd_puts("G_TEXRECTFLIP");
		case G_RDPLOADSYNC:
			return gfxd_puts("G_RDPLOADSYNC");
		case G_RDPPIPESYNC:
			return gfxd_puts("G_RDPPIPESYNC");
		case G_RDPTILESYNC:
			return gfxd_puts("G_RDPTILESYNC");
		case G_RDPFULLSYNC:
			return gfxd_puts("G_RDPFULLSYNC");
		case G_SETKEYGB:
			return gfxd_puts("G_SETKEYGB");
		case G_SETKEYR:
			return gfxd_puts("G_SETKEYR");
		case G_SETCONVERT:
			return gfxd_puts("G_SETCONVERT");
		case G_SETSCISSOR:
			return gfxd_puts("G_SETSCISSOR");
		case G_SETPRIMDEPTH:
			return gfxd_puts("G_SETPRIMDEPTH");
		case G_RDPSETOTHERMODE:
			return gfxd_puts("G_RDPSETOTHERMODE");
		case G_LOADTLUT:
			return gfxd_puts("G_LOADTLUT");
		case G_SETTILESIZE:
			return gfxd_puts("G_SETTILESIZE");
		case G_LOADBLOCK:
			return gfxd_puts("G_LOADBLOCK");
		case G_LOADTILE:
			return gfxd_puts("G_LOADTILE");
		case G_SETTILE:
			return gfxd_puts("G_SETTILE");
		case G_FILLRECT:
			return gfxd_puts("G_FILLRECT");
		case G_SETFILLCOLOR:
			return gfxd_puts("G_SETFILLCOLOR");
		case G_SETFOGCOLOR:
			return gfxd_puts("G_SETFOGCOLOR");
		case G_SETBLENDCOLOR:
			return gfxd_puts("G_SETBLENDCOLOR");
		case G_SETPRIMCOLOR:
			return gfxd_puts("G_SETPRIMCOLOR");
		case G_SETENVCOLOR:
			return gfxd_puts("G_SETENVCOLOR");
		case G_SETCOMBINE:
			return gfxd_puts("G_SETCOMBINE");
		case G_SETTIMG:
			return gfxd_puts("G_SETTIMG");
		case G_SETZIMG:
			return gfxd_puts("G_SETZIMG");
		case G_SETCIMG:
			return gfxd_puts("G_SETCIMG");
		default:
			return gfxd_printf("0x%02" PRIX32, (uint32_t)v->i);
	}
}

UCFUNC int argfn_fmt(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_IM_FMT_RGBA:
			return gfxd_puts("G_IM_FMT_RGBA");
		case G_IM_FMT_YUV:
			return gfxd_puts("G_IM_FMT_YUV");
		case G_IM_FMT_CI:
			return gfxd_puts("G_IM_FMT_CI");
		case G_IM_FMT_IA:
			return gfxd_puts("G_IM_FMT_IA");
		case G_IM_FMT_I:
			return gfxd_puts("G_IM_FMT_I");
		default:
			return gfxd_printf("%" PRIi32, v->i);
	}
}

UCFUNC int argfn_siz(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_IM_SIZ_4b:
			return gfxd_puts("G_IM_SIZ_4b");
		case G_IM_SIZ_8b:
			return gfxd_puts("G_IM_SIZ_8b");
		case G_IM_SIZ_16b:
			return gfxd_puts("G_IM_SIZ_16b");
		case G_IM_SIZ_32b:
			return gfxd_puts("G_IM_SIZ_32b");
		default:
			return gfxd_printf("%" PRIi32, v->i);
	}
}

UCFUNC int argfn_cm(const gfxd_value_t *v)
{
	int n = 0;
	if (v->u & G_TX_MIRROR)
		n += gfxd_puts("G_TX_MIRROR");
	else
		n += gfxd_puts("G_TX_NOMIRROR");
	if (v->u & G_TX_CLAMP)
		n += gfxd_puts(" | G_TX_CLAMP");
	else
		n += gfxd_puts(" | G_TX_WRAP");
	return n;
}

UCFUNC int argfn_tm(const gfxd_value_t *v)
{
	if (v->i == 0)
		return gfxd_puts("G_TX_NOMASK");
	else
		return gfxd_printf("%" PRIi32, v->i);
}

UCFUNC int argfn_ts(const gfxd_value_t *v)
{
	if (v->i == 0)
		return gfxd_puts("G_TX_NOLOD");
	else
		return gfxd_printf("%" PRIi32, v->i);
}

UCFUNC int argfn_switch(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_ON:
			return gfxd_puts("G_ON");
		case G_OFF:
			return gfxd_puts("G_OFF");
		default:
			return gfxd_printf("%" PRIi32, v->i);
	}
}

UCFUNC int argfn_tile(const gfxd_value_t *v)
{
	if (v->i == G_TX_LOADTILE)
		return gfxd_puts("G_TX_LOADTILE");
	else if (v->i == G_TX_RENDERTILE)
		return gfxd_puts("G_TX_RENDERTILE");
	else
		return gfxd_printf("%" PRIi32, v->i);
}

UCFUNC int argfn_gm(const gfxd_value_t *v)
{
	int n = 0;
	uint32_t arg = v->u;
	if (arg & G_ZBUFFER)
		n += gfxd_puts("G_ZBUFFER");
	if (arg & G_TEXTURE_ENABLE)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("G_TEXTURE_ENABLE");
	}
	if (arg & G_SHADE)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("G_SHADE");
	}
	if ((arg & G_CULL_BOTH) == G_CULL_BOTH)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("G_CULL_BOTH");
	}
	else
	{
		if (arg & G_CULL_FRONT)
		{
			if (n > 0)
				n += gfxd_puts(" | ");
			n += gfxd_puts("G_CULL_FRONT");
		}
		if (arg & G_CULL_BACK)
		{
			if (n > 0)
				n += gfxd_puts(" | ");
			n += gfxd_puts("G_CULL_BACK");
		}
	}
	if (arg & G_FOG)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("G_FOG");
	}
	if (arg & G_LIGHTING)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("G_LIGHTING");
	}
	if (arg & G_TEXTURE_GEN)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("G_TEXTURE_GEN");
	}
	if (arg & G_TEXTURE_GEN_LINEAR)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("G_TEXTURE_GEN_LINEAR");
	}
	if (arg & G_LOD)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("G_LOD");
	}
	if (arg & G_SHADING_SMOOTH)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("G_SHADING_SMOOTH");
	}
	if (arg & G_CLIPPING)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("G_CLIPPING");
	}
	arg = arg & ~(G_ZBUFFER | G_TEXTURE_ENABLE | G_SHADE | G_CULL_BOTH |
		G_FOG | G_LIGHTING | G_TEXTURE_GEN |
		G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH |
		G_CLIPPING);
	if (arg)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_printf("0x%08" PRIX32, arg);
	}
	return n;
}

UCFUNC int argfn_sftlo(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_MDSFT_ALPHACOMPARE:
			return gfxd_puts("G_MDSFT_ALPHACOMPARE");
		case G_MDSFT_ZSRCSEL:
			return gfxd_puts("G_MDSFT_ZSRCSEL");
		case G_MDSFT_RENDERMODE:
			return gfxd_puts("G_MDSFT_RENDERMODE");
		case G_MDSFT_BLENDER:
			return gfxd_puts("G_MDSFT_BLENDER");
		default:
			return gfxd_printf("%" PRIi32, v->i);
	}
}

UCFUNC int rm_mode_str(uint32_t arg)
{
	int n = 0;
	if (arg & AA_EN)
		n += gfxd_puts("AA_EN");
	if (arg & Z_CMP)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("Z_CMP");
	}
	if (arg & Z_UPD)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("Z_UPD");
	}
	if (arg & IM_RD)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("IM_RD");
	}
	if (arg & CLR_ON_CVG)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += gfxd_puts("CLR_ON_CVG");
	}
	if (n > 0)
		n += gfxd_puts(" | ");
	int cvg = arg & 0x00000300;
	switch (cvg)
	{
		case CVG_DST_CLAMP:
			n += gfxd_puts("CVG_DST_CLAMP");
			break;
		case CVG_DST_WRAP:
			n += gfxd_puts("CVG_DST_WRAP");
			break;
		case CVG_DST_FULL:
			n += gfxd_puts("CVG_DST_FULL");
			break;
		case CVG_DST_SAVE:
			n += gfxd_puts("CVG_DST_SAVE");
			break;
	}
	int zmode = arg & 0x00000C00;
	switch (zmode)
	{
		case ZMODE_OPA:
			n += gfxd_puts(" | ZMODE_OPA");
			break;
		case ZMODE_INTER:
			n += gfxd_puts(" | ZMODE_INTER");
			break;
		case ZMODE_XLU:
			n += gfxd_puts(" | ZMODE_XLU");
			break;
		case ZMODE_DEC:
			n += gfxd_puts(" | ZMODE_DEC");
			break;
	}
	if (arg & CVG_X_ALPHA)
		n += gfxd_puts(" | CVG_X_ALPHA");
	if (arg & ALPHA_CVG_SEL)
		n += gfxd_puts(" | ALPHA_CVG_SEL");
	if (arg & FORCE_BL)
		n += gfxd_puts(" | FORCE_BL");
	return n;
}

UCFUNC int rm_cbl_str(uint32_t arg, int c)
{
	int n = 0;
	if (c == 2)
		arg <<= 2;
	int bp = (arg >> 30) & 0b11;
	switch (bp)
	{
		case G_BL_CLR_IN:
			n += gfxd_printf("GBL_c%i(G_BL_CLR_IN", c);
			break;
		case G_BL_CLR_MEM:
			n += gfxd_printf("GBL_c%i(G_BL_CLR_MEM", c);
			break;
		case G_BL_CLR_BL:
			n += gfxd_printf("GBL_c%i(G_BL_CLR_BL", c);
			break;
		case G_BL_CLR_FOG:
			n += gfxd_printf("GBL_c%i(G_BL_CLR_FOG", c);
			break;
	}
	int ba = (arg >> 26) & 0b11;
	switch (ba)
	{
		case G_BL_A_IN:
			n += gfxd_puts(", G_BL_A_IN");
			break;
		case G_BL_A_FOG:
			n += gfxd_puts(", G_BL_A_FOG");
			break;
		case G_BL_A_SHADE:
			n += gfxd_puts(", G_BL_A_SHADE");
			break;
		case G_BL_0:
			n += gfxd_puts(", G_BL_0");
			break;
	}
	int bm = (arg >> 22) & 0b11;
	switch (bm)
	{
		case G_BL_CLR_IN:
			n += gfxd_puts(", G_BL_CLR_IN");
			break;
		case G_BL_CLR_MEM:
			n += gfxd_puts(", G_BL_CLR_MEM");
			break;
		case G_BL_CLR_BL:
			n += gfxd_puts(", G_BL_CLR_BL");
			break;
		case G_BL_CLR_FOG:
			n += gfxd_puts(", G_BL_CLR_FOG");
			break;
	}
	int bb = (arg >> 18) & 0b11;
	switch (bb)
	{
		case G_BL_1MA:
			n += gfxd_puts(", G_BL_1MA)");
			break;
		case G_BL_A_MEM:
			n += gfxd_puts(", G_BL_A_MEM)");
			break;
		case G_BL_1:
			n += gfxd_puts(", G_BL_1)");
			break;
		case G_BL_0:
			n += gfxd_puts(", G_BL_0)");
			break;
	}
	return n;
}

struct rm_preset
{
	uint32_t	rm;
	const char *	name;
};

static const struct rm_preset rm_presets[] =
{
	{G_RM_OPA_SURF,		"G_RM_OPA_SURF"},
	{G_RM_OPA_SURF2,	"G_RM_OPA_SURF2"},
	{G_RM_AA_OPA_SURF,	"G_RM_AA_OPA_SURF"},
	{G_RM_AA_OPA_SURF2,	"G_RM_AA_OPA_SURF2"},
	{G_RM_RA_OPA_SURF,	"G_RM_RA_OPA_SURF"},
	{G_RM_RA_OPA_SURF2,	"G_RM_RA_OPA_SURF2"},
	{G_RM_ZB_OPA_SURF,	"G_RM_ZB_OPA_SURF"},
	{G_RM_ZB_OPA_SURF2,	"G_RM_ZB_OPA_SURF2"},
	{G_RM_AA_ZB_OPA_SURF,	"G_RM_AA_ZB_OPA_SURF"},
	{G_RM_AA_ZB_OPA_SURF2,	"G_RM_AA_ZB_OPA_SURF2"},
	{G_RM_RA_ZB_OPA_SURF,	"G_RM_RA_ZB_OPA_SURF"},
	{G_RM_RA_ZB_OPA_SURF2,	"G_RM_RA_ZB_OPA_SURF2"},
	{G_RM_XLU_SURF,		"G_RM_XLU_SURF"},
	{G_RM_XLU_SURF2,	"G_RM_XLU_SURF2"},
	{G_RM_AA_XLU_SURF,	"G_RM_AA_XLU_SURF"},
	{G_RM_AA_XLU_SURF2,	"G_RM_AA_XLU_SURF2"},
	{G_RM_ZB_XLU_SURF,	"G_RM_ZB_XLU_SURF"},
	{G_RM_ZB_XLU_SURF2,	"G_RM_ZB_XLU_SURF2"},
	{G_RM_AA_ZB_XLU_SURF,	"G_RM_AA_ZB_XLU_SURF"},
	{G_RM_AA_ZB_XLU_SURF2,	"G_RM_AA_ZB_XLU_SURF2"},
	{G_RM_ZB_OPA_DECAL,	"G_RM_ZB_OPA_DECAL"},
	{G_RM_ZB_OPA_DECAL2,	"G_RM_ZB_OPA_DECAL2"},
	{G_RM_AA_ZB_OPA_DECAL,	"G_RM_AA_ZB_OPA_DECAL"},
	{G_RM_AA_ZB_OPA_DECAL2,	"G_RM_AA_ZB_OPA_DECAL2"},
	{G_RM_RA_ZB_OPA_DECAL,	"G_RM_RA_ZB_OPA_DECAL"},
	{G_RM_RA_ZB_OPA_DECAL2,	"G_RM_RA_ZB_OPA_DECAL2"},
	{G_RM_ZB_XLU_DECAL,	"G_RM_ZB_XLU_DECAL"},
	{G_RM_ZB_XLU_DECAL2,	"G_RM_ZB_XLU_DECAL2"},
	{G_RM_AA_ZB_XLU_DECAL,	"G_RM_AA_ZB_XLU_DECAL"},
	{G_RM_AA_ZB_XLU_DECAL2,	"G_RM_AA_ZB_XLU_DECAL2"},
	{G_RM_AA_ZB_OPA_INTER,	"G_RM_AA_ZB_OPA_INTER"},
	{G_RM_AA_ZB_OPA_INTER2,	"G_RM_AA_ZB_OPA_INTER2"},
	{G_RM_RA_ZB_OPA_INTER,	"G_RM_RA_ZB_OPA_INTER"},
	{G_RM_RA_ZB_OPA_INTER2,	"G_RM_RA_ZB_OPA_INTER2"},
	{G_RM_AA_ZB_XLU_INTER,	"G_RM_AA_ZB_XLU_INTER"},
	{G_RM_AA_ZB_XLU_INTER2,	"G_RM_AA_ZB_XLU_INTER2"},
	{G_RM_AA_XLU_LINE,	"G_RM_AA_XLU_LINE"},
	{G_RM_AA_XLU_LINE2,	"G_RM_AA_XLU_LINE2"},
	{G_RM_AA_ZB_XLU_LINE,	"G_RM_AA_ZB_XLU_LINE"},
	{G_RM_AA_ZB_XLU_LINE2,	"G_RM_AA_ZB_XLU_LINE2"},
	{G_RM_AA_DEC_LINE,	"G_RM_AA_DEC_LINE"},
	{G_RM_AA_DEC_LINE2,	"G_RM_AA_DEC_LINE2"},
	{G_RM_AA_ZB_DEC_LINE,	"G_RM_AA_ZB_DEC_LINE"},
	{G_RM_AA_ZB_DEC_LINE2,	"G_RM_AA_ZB_DEC_LINE2"},
	{G_RM_TEX_EDGE,		"G_RM_TEX_EDGE"},
	{G_RM_TEX_EDGE2,	"G_RM_TEX_EDGE2"},
	{G_RM_AA_TEX_EDGE,	"G_RM_AA_TEX_EDGE"},
	{G_RM_AA_TEX_EDGE2,	"G_RM_AA_TEX_EDGE2"},
	{G_RM_AA_ZB_TEX_EDGE,	"G_RM_AA_ZB_TEX_EDGE"},
	{G_RM_AA_ZB_TEX_EDGE2,	"G_RM_AA_ZB_TEX_EDGE2"},
	{G_RM_AA_ZB_TEX_INTER,	"G_RM_AA_ZB_TEX_INTER"},
	{G_RM_AA_ZB_TEX_INTER2,	"G_RM_AA_ZB_TEX_INTER2"},
	{G_RM_AA_SUB_SURF,	"G_RM_AA_SUB_SURF"},
	{G_RM_AA_SUB_SURF2,	"G_RM_AA_SUB_SURF2"},
	{G_RM_AA_ZB_SUB_SURF,	"G_RM_AA_ZB_SUB_SURF"},
	{G_RM_AA_ZB_SUB_SURF2,	"G_RM_AA_ZB_SUB_SURF2"},
	{G_RM_PCL_SURF,		"G_RM_PCL_SURF"},
	{G_RM_PCL_SURF2,	"G_RM_PCL_SURF2"},
	{G_RM_AA_PCL_SURF,	"G_RM_AA_PCL_SURF"},
	{G_RM_AA_PCL_SURF2,	"G_RM_AA_PCL_SURF2"},
	{G_RM_ZB_PCL_SURF,	"G_RM_ZB_PCL_SURF"},
	{G_RM_ZB_PCL_SURF2,	"G_RM_ZB_PCL_SURF2"},
	{G_RM_AA_ZB_PCL_SURF,	"G_RM_AA_ZB_PCL_SURF"},
	{G_RM_AA_ZB_PCL_SURF2,	"G_RM_AA_ZB_PCL_SURF2"},
	{G_RM_AA_OPA_TERR,	"G_RM_AA_OPA_TERR"},
	{G_RM_AA_OPA_TERR2,	"G_RM_AA_OPA_TERR2"},
	{G_RM_AA_ZB_OPA_TERR,	"G_RM_AA_ZB_OPA_TERR"},
	{G_RM_AA_ZB_OPA_TERR2,	"G_RM_AA_ZB_OPA_TERR2"},
	{G_RM_AA_TEX_TERR,	"G_RM_AA_TEX_TERR"},
	{G_RM_AA_TEX_TERR2,	"G_RM_AA_TEX_TERR2"},
	{G_RM_AA_ZB_TEX_TERR,	"G_RM_AA_ZB_TEX_TERR"},
	{G_RM_AA_ZB_TEX_TERR2,	"G_RM_AA_ZB_TEX_TERR2"},
	{G_RM_AA_SUB_TERR,	"G_RM_AA_SUB_TERR"},
	{G_RM_AA_SUB_TERR2,	"G_RM_AA_SUB_TERR2"},
	{G_RM_AA_ZB_SUB_TERR,	"G_RM_AA_ZB_SUB_TERR"},
	{G_RM_AA_ZB_SUB_TERR2,	"G_RM_AA_ZB_SUB_TERR2"},
	{G_RM_CLD_SURF,		"G_RM_CLD_SURF"},
	{G_RM_CLD_SURF2,	"G_RM_CLD_SURF2"},
	{G_RM_ZB_CLD_SURF,	"G_RM_ZB_CLD_SURF"},
	{G_RM_ZB_CLD_SURF2,	"G_RM_ZB_CLD_SURF2"},
	{G_RM_ZB_OVL_SURF,	"G_RM_ZB_OVL_SURF"},
	{G_RM_ZB_OVL_SURF2,	"G_RM_ZB_OVL_SURF2"},
	{G_RM_ADD,		"G_RM_ADD"},
	{G_RM_ADD2,		"G_RM_ADD2"},
	{G_RM_VISCVG,		"G_RM_VISCVG"},
	{G_RM_VISCVG2,		"G_RM_VISCVG2"},
	{G_RM_OPA_CI,		"G_RM_OPA_CI"},
	{G_RM_OPA_CI2,		"G_RM_OPA_CI2"},
	{G_RM_RA_SPRITE,	"G_RM_RA_SPRITE"},
	{G_RM_RA_SPRITE2,	"G_RM_RA_SPRITE2"},
};

static const struct rm_preset bl1_presets[] =
{
	{G_RM_FOG_SHADE_A,	"G_RM_FOG_SHADE_A"},
	{G_RM_FOG_PRIM_A,	"G_RM_FOG_PRIM_A"},
	{G_RM_PASS,		"G_RM_PASS"},
	{G_RM_NOOP,		"G_RM_NOOP"},
};

static const struct rm_preset bl2_presets[] =
{
	{G_RM_NOOP2,		"G_RM_NOOP2"},
};

UCFUNC int othermodelo_str(uint32_t arg, uint32_t which)
{
	int n = 0;
	uint32_t rm_c1_mask = MDMASK_RM_C1;
	uint32_t rm_c2_mask = MDMASK_RM_C2;
	uint32_t rm_mode_lo = MDMASK_RM_LO;
	uint32_t rm_mask = rm_c1_mask | rm_c2_mask | rm_mode_lo;
	const struct rm_preset *pre_c1 = NULL;
	const struct rm_preset *pre_c2 = NULL;
	int n_rm_presets = sizeof(rm_presets) / sizeof(*rm_presets);
	for (int i = 0; i < n_rm_presets; i++)
	{
		const struct rm_preset *pre = &rm_presets[i];
		uint32_t pre_extra = pre->rm & ~rm_mask;
		uint32_t rm_c1 = arg & (rm_c1_mask | rm_mode_lo | pre_extra);
		if (!pre_c1 && rm_c1 == pre->rm)
			pre_c1 = pre;
		uint32_t rm_c2 = arg & (rm_c2_mask | rm_mode_lo | pre_extra);
		if (!pre_c2 && rm_c2 == pre->rm)
			pre_c2 = pre;
	}
	if (!pre_c1 || !pre_c2 || pre_c1 + 1 != pre_c2)
	{
		int n_bl1_presets = sizeof(bl1_presets) / sizeof(*bl1_presets);
		for (int i = 0; i < n_bl1_presets; i++)
		{
			const struct rm_preset *pre = &bl1_presets[i];
			uint32_t pre_extra = pre->rm & ~rm_mask;
			uint32_t rm_c1 = arg & (rm_c1_mask | pre_extra);
			if (rm_c1 == pre->rm)
			{
				pre_c1 = pre;
				break;
			}
		}
		int n_bl2_presets = sizeof(bl2_presets) / sizeof(*bl2_presets);
		for (int i = 0; i < n_bl2_presets; i++)
		{
			const struct rm_preset *pre = &bl2_presets[i];
			uint32_t pre_extra = pre->rm & ~rm_mask;
			uint32_t rm_c2 = arg & (rm_c2_mask | pre_extra);
			if (rm_c2 == pre->rm)
			{
				pre_c2 = pre;
				break;
			}
		}
	}
	uint32_t pre_rm = 0;
	if (pre_c1)
		pre_rm |= pre_c1->rm;
	if (pre_c2)
		pre_rm |= pre_c2->rm;
	uint32_t ac_mask = MDMASK(ALPHACOMPARE);
	if (((arg & ~pre_rm) | which) & ac_mask)
	{
		uint32_t ac = arg & ac_mask;
		switch (ac)
		{
			case G_AC_NONE:
				n += gfxd_puts("G_AC_NONE");
				break;
			case G_AC_THRESHOLD:
				n += gfxd_puts("G_AC_THRESHOLD");
				break;
			case G_AC_DITHER:
				n += gfxd_puts("G_AC_DITHER");
				break;
			default:
				n += gfxd_printf("0x%08" PRIX32, ac);
				break;
		}
	}
	uint32_t zs_mask = MDMASK(ZSRCSEL);
	if (((arg & ~pre_rm) | which) & zs_mask)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		uint32_t zs = arg & zs_mask;
		switch (zs)
		{
			case G_ZS_PIXEL:
				n += gfxd_puts("G_ZS_PIXEL");
				break;
			case G_ZS_PRIM:
				n += gfxd_puts("G_ZS_PRIM");
				break;
			default:
				n += gfxd_printf("0x%08" PRIX32, zs);
				break;
		}
	}
	uint32_t rm = arg & (rm_mask | pre_rm);
	if (((arg & ~pre_rm) | which) & rm_mode_lo)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		n += rm_mode_str(rm);
	}
	int c = 0;
	if (which & rm_c1_mask)
		c |= 1;
	if (which & rm_c2_mask)
		c |= 2;
	if (c & 1 || (c == 0 && arg & rm_c1_mask))
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		if (pre_c1)
			n += gfxd_printf("%s", pre_c1->name);
		else
			n += rm_cbl_str(rm, 1);
	}
	if (c & 2 || (c == 0 && arg & rm_c2_mask))
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		if (pre_c2)
			n += gfxd_printf("%s", pre_c2->name);
		else
			n += rm_cbl_str(rm, 2);
	}
	uint32_t unk_mask = ~(rm_mask | ac_mask | zs_mask);
	if (arg & unk_mask)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		uint32_t unk = arg & unk_mask;
		n += gfxd_printf("0x%08" PRIX32, unk);
	}
	return n;
}

UCFUNC int argfn_ac(const gfxd_value_t *v)
{
	return othermodelo_str(v->u, MDMASK(ALPHACOMPARE));
}

UCFUNC int argfn_zs(const gfxd_value_t *v)
{
	return othermodelo_str(v->u, MDMASK(ZSRCSEL));
}

UCFUNC int argfn_rm1(const gfxd_value_t *v)
{
	return othermodelo_str(v->u, MDMASK_RM_C1);
}

UCFUNC int argfn_rm2(const gfxd_value_t *v)
{
	return othermodelo_str(v->u, MDMASK_RM_C2);
}

UCFUNC int argfn_othermodelo(const gfxd_value_t *v)
{
	uint32_t mask = MDMASK(ALPHACOMPARE) | MDMASK(ZSRCSEL) | MDMASK_RM_C1 |
			MDMASK_RM_C2;
	return othermodelo_str(v->u, mask);
}

UCFUNC int argfn_sfthi(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_MDSFT_ALPHADITHER:
			return gfxd_puts("G_MDSFT_ALPHADITHER");
		case G_MDSFT_RGBDITHER:
			return gfxd_puts("G_MDSFT_RGBDITHER");
		case G_MDSFT_COMBKEY:
			return gfxd_puts("G_MDSFT_COMBKEY");
		case G_MDSFT_TEXTCONV:
			return gfxd_puts("G_MDSFT_TEXTCONV");
		case G_MDSFT_TEXTFILT:
			return gfxd_puts("G_MDSFT_TEXTFILT");
		case G_MDSFT_TEXTLUT:
			return gfxd_puts("G_MDSFT_TEXTLUT");
		case G_MDSFT_TEXTLOD:
			return gfxd_puts("G_MDSFT_TEXTLOD");
		case G_MDSFT_TEXTDETAIL:
			return gfxd_puts("G_MDSFT_TEXTDETAIL");
		case G_MDSFT_TEXTPERSP:
			return gfxd_puts("G_MDSFT_TEXTPERSP");
		case G_MDSFT_CYCLETYPE:
			return gfxd_puts("G_MDSFT_CYCLETYPE");
		case G_MDSFT_PIPELINE:
			return gfxd_puts("G_MDSFT_PIPELINE");
		default:
			return gfxd_printf("%" PRIi32, v->i);
	}
}

UCFUNC int othermodehi_str(uint32_t arg, uint32_t which)
{
	int n = 0;
	uint32_t ad_mask = MDMASK(ALPHADITHER);
	if ((arg | which) & ad_mask)
	{
		uint32_t ad = arg & ad_mask;
		switch (ad)
		{
			case G_AD_PATTERN:
				n += gfxd_puts("G_AD_PATTERN");
				break;
			case G_AD_NOTPATTERN:
				n += gfxd_puts("G_AD_NOTPATTERN");
				break;
			case G_AD_NOISE:
				n += gfxd_puts("G_AD_NOISE");
				break;
			case G_AD_DISABLE:
				n += gfxd_puts("G_AD_DISABLE");
				break;
			default:
				n += gfxd_printf("0x%08" PRIX32, ad);
				break;
		}
	}
	uint32_t cd_mask = MDMASK(RGBDITHER);
	if ((arg | which) & cd_mask)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		uint32_t cd = arg & cd_mask;
		switch (cd)
		{
			case G_CD_MAGICSQ:
				n += gfxd_puts("G_CD_MAGICSQ");
				break;
			case G_CD_BAYER:
				n += gfxd_puts("G_CD_BAYER");
				break;
			case G_CD_NOISE:
				n += gfxd_puts("G_CD_NOISE");
				break;
			case G_CD_DISABLE:
				n += gfxd_puts("G_CD_DISABLE");
				break;
			default:
				n += gfxd_printf("0x%08" PRIX32, cd);
				break;
		}
	}
	uint32_t ck_mask = MDMASK(COMBKEY);
	if ((arg | which) & ck_mask)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		uint32_t ck = arg & ck_mask;
		switch (ck)
		{
			case G_CK_NONE:
				n += gfxd_puts("G_CK_NONE");
				break;
			case G_CK_KEY:
				n += gfxd_puts("G_CK_KEY");
				break;
			default:
				n += gfxd_printf("0x%08" PRIX32, ck);
				break;
		}
	}
	uint32_t tc_mask = MDMASK(TEXTCONV);
	if ((arg | which) & tc_mask)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		uint32_t tc = arg & tc_mask;
		switch (tc)
		{
			case G_TC_CONV:
				n += gfxd_puts("G_TC_CONV");
				break;
			case G_TC_FILTCONV:
				n += gfxd_puts("G_TC_FILTCONV");
				break;
			case G_TC_FILT:
				n += gfxd_puts("G_TC_FILT");
				break;
			default:
				n += gfxd_printf("0x%08" PRIX32, tc);
				break;
		}
	}
	uint32_t tf_mask = MDMASK(TEXTFILT);
	if ((arg | which) & tf_mask)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		uint32_t tf = arg & tf_mask;
		switch (tf)
		{
			case G_TF_POINT:
				n += gfxd_puts("G_TF_POINT");
				break;
			case G_TF_BILERP:
				n += gfxd_puts("G_TF_BILERP");
				break;
			case G_TF_AVERAGE:
				n += gfxd_puts("G_TF_AVERAGE");
				break;
			default:
				n += gfxd_printf("0x%08" PRIX32, tf);
				break;
		}
	}
	uint32_t tt_mask = MDMASK(TEXTLUT);
	if ((arg | which) & tt_mask)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		uint32_t tt = arg & tt_mask;
		switch (tt)
		{
			case G_TT_NONE:
				n += gfxd_puts("G_TT_NONE");
				break;
			case G_TT_RGBA16:
				n += gfxd_puts("G_TT_RGBA16");
				break;
			case G_TT_IA16:
				n += gfxd_puts("G_TT_IA16");
				break;
			default:
				n += gfxd_printf("0x%08" PRIX32, tt);
				break;
		}
	}
	uint32_t tl_mask = MDMASK(TEXTLOD);
	if ((arg | which) & tl_mask)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		uint32_t tl = arg & tl_mask;
		switch (tl)
		{
			case G_TL_TILE:
				n += gfxd_puts("G_TL_TILE");
				break;
			case G_TL_LOD:
				n += gfxd_puts("G_TL_LOD");
				break;
			default:
				n += gfxd_printf("0x%08" PRIX32, tl);
				break;
		}
	}
	uint32_t td_mask = MDMASK(TEXTDETAIL);
	if ((arg | which) & td_mask)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		uint32_t td = arg & td_mask;
		switch (td)
		{
			case G_TD_CLAMP:
				n += gfxd_puts("G_TD_CLAMP");
				break;
			case G_TD_SHARPEN:
				n += gfxd_puts("G_TD_SHARPEN");
				break;
			case G_TD_DETAIL:
				n += gfxd_puts("G_TD_DETAIL");
				break;
			default:
				n += gfxd_printf("0x%08" PRIX32, td);
				break;
		}
	}
	uint32_t tp_mask = MDMASK(TEXTPERSP);
	if ((arg | which) & tp_mask)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		uint32_t tp = arg & tp_mask;
		switch (tp)
		{
			case G_TP_NONE:
				n += gfxd_puts("G_TP_NONE");
				break;
			case G_TP_PERSP:
				n += gfxd_puts("G_TP_PERSP");
				break;
			default:
				n += gfxd_printf("0x%08" PRIX32, tp);
				break;
		}
	}
	uint32_t cyc_mask = MDMASK(CYCLETYPE);
	if ((arg | which) & cyc_mask)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		uint32_t cyc = arg & cyc_mask;
		switch (cyc)
		{
			case G_CYC_1CYCLE:
				n += gfxd_puts("G_CYC_1CYCLE");
				break;
			case G_CYC_2CYCLE:
				n += gfxd_puts("G_CYC_2CYCLE");
				break;
			case G_CYC_COPY:
				n += gfxd_puts("G_CYC_COPY");
				break;
			case G_CYC_FILL:
				n += gfxd_puts("G_CYC_FILL");
				break;
			default:
				n += gfxd_printf("0x%08" PRIX32, cyc);
				break;
		}
	}
	uint32_t pm_mask = MDMASK(PIPELINE);
	if ((arg | which) & pm_mask)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		uint32_t pm = arg & pm_mask;
		switch (pm)
		{
			case G_PM_NPRIMITIVE:
				n += gfxd_puts("G_PM_NPRIMITIVE");
				break;
			case G_PM_1PRIMITIVE:
				n += gfxd_puts("G_PM_1PRIMITIVE");
				break;
			default:
				n += gfxd_printf("0x%08" PRIX32, pm);
				break;
		}
	}
	uint32_t unk_mask = ~(ad_mask | cd_mask | ck_mask | tc_mask | tf_mask |
				tt_mask | tl_mask | td_mask | tp_mask |
				cyc_mask | pm_mask);
	if (arg & unk_mask)
	{
		if (n > 0)
			n += gfxd_puts(" | ");
		uint32_t unk = arg & unk_mask;
		n += gfxd_printf("0x%08" PRIX32, unk);
	}
	return n;
}

UCFUNC int argfn_ad(const gfxd_value_t *v)
{
	return othermodehi_str(v->u, MDMASK(ALPHADITHER));
}

UCFUNC int argfn_cd(const gfxd_value_t *v)
{
	return othermodehi_str(v->u, MDMASK(RGBDITHER));
}

UCFUNC int argfn_ck(const gfxd_value_t *v)
{
	return othermodehi_str(v->u, MDMASK(COMBKEY));
}

UCFUNC int argfn_tc(const gfxd_value_t *v)
{
	return othermodehi_str(v->u, MDMASK(TEXTCONV));
}

UCFUNC int argfn_tf(const gfxd_value_t *v)
{
	return othermodehi_str(v->u, MDMASK(TEXTFILT));
}

UCFUNC int argfn_tt(const gfxd_value_t *v)
{
	return othermodehi_str(v->u, MDMASK(TEXTLUT));
}

UCFUNC int argfn_tl(const gfxd_value_t *v)
{
	return othermodehi_str(v->u, MDMASK(TEXTLOD));
}

UCFUNC int argfn_td(const gfxd_value_t *v)
{
	return othermodehi_str(v->u, MDMASK(TEXTDETAIL));
}

UCFUNC int argfn_tp(const gfxd_value_t *v)
{
	return othermodehi_str(v->u, MDMASK(TEXTPERSP));
}

UCFUNC int argfn_cyc(const gfxd_value_t *v)
{
	return othermodehi_str(v->u, MDMASK(CYCLETYPE));
}

UCFUNC int argfn_pm(const gfxd_value_t *v)
{
	return othermodehi_str(v->u, MDMASK(PIPELINE));
}

UCFUNC int argfn_othermodehi(const gfxd_value_t *v)
{
	uint32_t mask = MDMASK(ALPHADITHER) |
			MDMASK(RGBDITHER) |
			MDMASK(COMBKEY) |
			MDMASK(TEXTCONV) |
			MDMASK(TEXTFILT) |
			MDMASK(TEXTLUT) |
			MDMASK(TEXTLOD) |
			MDMASK(TEXTDETAIL) |
			MDMASK(TEXTPERSP) |
			MDMASK(CYCLETYPE) |
			MDMASK(PIPELINE);
	return othermodehi_str(v->u, mask);
}

UCFUNC int argfn_cv(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_CV_K0:
			return gfxd_puts("G_CV_K0");
		case G_CV_K1:
			return gfxd_puts("G_CV_K1");
		case G_CV_K2:
			return gfxd_puts("G_CV_K2");
		case G_CV_K3:
			return gfxd_puts("G_CV_K3");
		case G_CV_K4:
			return gfxd_puts("G_CV_K4");
		case G_CV_K5:
			return gfxd_puts("G_CV_K5");
		default:
			return gfxd_printf("%" PRIi32, v->i);
	}
}

UCFUNC int argfn_ccmuxa(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_CCMUX_COMBINED:
			return gfxd_puts("COMBINED");
		case G_CCMUX_TEXEL0:
			return gfxd_puts("TEXEL0");
		case G_CCMUX_TEXEL1:
			return gfxd_puts("TEXEL1");
		case G_CCMUX_PRIMITIVE:
			return gfxd_puts("PRIMITIVE");
		case G_CCMUX_SHADE:
			return gfxd_puts("SHADE");
		case G_CCMUX_ENVIRONMENT:
			return gfxd_puts("ENVIRONMENT");
		case G_CCMUX_1:
			return gfxd_puts("1");
		case G_CCMUX_NOISE:
			return gfxd_puts("NOISE");
		default:
			return gfxd_puts("0");
	}
}

UCFUNC int argfn_ccmuxb(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_CCMUX_COMBINED:
			return gfxd_puts("COMBINED");
		case G_CCMUX_TEXEL0:
			return gfxd_puts("TEXEL0");
		case G_CCMUX_TEXEL1:
			return gfxd_puts("TEXEL1");
		case G_CCMUX_PRIMITIVE:
			return gfxd_puts("PRIMITIVE");
		case G_CCMUX_SHADE:
			return gfxd_puts("SHADE");
		case G_CCMUX_ENVIRONMENT:
			return gfxd_puts("ENVIRONMENT");
		case G_CCMUX_CENTER:
			return gfxd_puts("CENTER");
		case G_CCMUX_K4:
			return gfxd_puts("K4");
		default:
			return gfxd_puts("0");
	}
}

UCFUNC int argfn_ccmuxc(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_CCMUX_COMBINED:
			return gfxd_puts("COMBINED");
		case G_CCMUX_TEXEL0:
			return gfxd_puts("TEXEL0");
		case G_CCMUX_TEXEL1:
			return gfxd_puts("TEXEL1");
		case G_CCMUX_PRIMITIVE:
			return gfxd_puts("PRIMITIVE");
		case G_CCMUX_SHADE:
			return gfxd_puts("SHADE");
		case G_CCMUX_ENVIRONMENT:
			return gfxd_puts("ENVIRONMENT");
		case G_CCMUX_SCALE:
			return gfxd_puts("SCALE");
		case G_CCMUX_COMBINED_ALPHA:
			return gfxd_puts("COMBINED_ALPHA");
		case G_CCMUX_TEXEL0_ALPHA:
			return gfxd_puts("TEXEL0_ALPHA");
		case G_CCMUX_TEXEL1_ALPHA:
			return gfxd_puts("TEXEL1_ALPHA");
		case G_CCMUX_PRIMITIVE_ALPHA:
			return gfxd_puts("PRIMITIVE_ALPHA");
		case G_CCMUX_SHADE_ALPHA:
			return gfxd_puts("SHADE_ALPHA");
		case G_CCMUX_ENV_ALPHA:
			return gfxd_puts("ENV_ALPHA");
		case G_CCMUX_LOD_FRACTION:
			return gfxd_puts("LOD_FRACTION");
		case G_CCMUX_PRIM_LOD_FRAC:
			return gfxd_puts("PRIM_LOD_FRAC");
		case G_CCMUX_K5:
			return gfxd_puts("K5");
		default:
			return gfxd_puts("0");
	}
}

UCFUNC int argfn_ccmuxd(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_CCMUX_COMBINED:
			return gfxd_puts("COMBINED");
		case G_CCMUX_TEXEL0:
			return gfxd_puts("TEXEL0");
		case G_CCMUX_TEXEL1:
			return gfxd_puts("TEXEL1");
		case G_CCMUX_PRIMITIVE:
			return gfxd_puts("PRIMITIVE");
		case G_CCMUX_SHADE:
			return gfxd_puts("SHADE");
		case G_CCMUX_ENVIRONMENT:
			return gfxd_puts("ENVIRONMENT");
		case G_CCMUX_1:
			return gfxd_puts("1");
		default:
			return gfxd_puts("0");
	}
}

UCFUNC int argfn_acmuxabd(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_ACMUX_COMBINED:
			return gfxd_puts("COMBINED");
		case G_ACMUX_TEXEL0:
			return gfxd_puts("TEXEL0");
		case G_ACMUX_TEXEL1:
			return gfxd_puts("TEXEL1");
		case G_ACMUX_PRIMITIVE:
			return gfxd_puts("PRIMITIVE");
		case G_ACMUX_SHADE:
			return gfxd_puts("SHADE");
		case G_ACMUX_ENVIRONMENT:
			return gfxd_puts("ENVIRONMENT");
		case G_ACMUX_1:
			return gfxd_puts("1");
		default:
			return gfxd_puts("0");
	}
}

UCFUNC int argfn_acmuxc(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_ACMUX_LOD_FRACTION:
			return gfxd_puts("LOD_FRACTION");
		case G_ACMUX_TEXEL0:
			return gfxd_puts("TEXEL0");
		case G_ACMUX_TEXEL1:
			return gfxd_puts("TEXEL1");
		case G_ACMUX_PRIMITIVE:
			return gfxd_puts("PRIMITIVE");
		case G_ACMUX_SHADE:
			return gfxd_puts("SHADE");
		case G_ACMUX_ENVIRONMENT:
			return gfxd_puts("ENVIRONMENT");
		case G_ACMUX_PRIM_LOD_FRAC:
			return gfxd_puts("PRIM_LOD_FRAC");
		default:
			return gfxd_puts("0");
	}
}

struct cc_mode
{
	int	a;
	int	b;
	int	c;
	int	d;
	int	Aa;
	int	Ab;
	int	Ac;
	int	Ad;
};

struct cc_preset
{
	struct cc_mode	mode;
	const char *	name;
};

#define CC_(a,b,c,d,Aa,Ab,Ac,Ad) \
	{ \
		G_CCMUX_##a, G_CCMUX_##b, \
		G_CCMUX_##c, G_CCMUX_##d, \
		G_ACMUX_##Aa, G_ACMUX_##Ab, \
		G_ACMUX_##Ac, G_ACMUX_##Ad \
	}
#define CC(m) CC_(m)
static const struct cc_preset cc_presets[] =
{
	{CC(G_CC_MODULATEI),			"G_CC_MODULATEI"},
	{CC(G_CC_MODULATEIA),			"G_CC_MODULATEIA"},
	{CC(G_CC_MODULATEIDECALA),		"G_CC_MODULATEIDECALA"},
	{CC(G_CC_MODULATERGB),			"G_CC_MODULATERGB"},
	{CC(G_CC_MODULATERGBA),			"G_CC_MODULATERGBA"},
	{CC(G_CC_MODULATERGBDECALA),		"G_CC_MODULATERGBDECALA"},
	{CC(G_CC_MODULATEI_PRIM),		"G_CC_MODULATEI_PRIM"},
	{CC(G_CC_MODULATEIA_PRIM),		"G_CC_MODULATEIA_PRIM"},
	{CC(G_CC_MODULATEIDECALA_PRIM),		"G_CC_MODULATEIDECALA_PRIM"},
	{CC(G_CC_MODULATERGB_PRIM),		"G_CC_MODULATERGB_PRIM"},
	{CC(G_CC_MODULATERGBA_PRIM),		"G_CC_MODULATERGBA_PRIM"},
	{CC(G_CC_MODULATERGBDECALA_PRIM),	"G_CC_MODULATERGBDECALA_PRIM"},
	{CC(G_CC_DECALRGB),			"G_CC_DECALRGB"},
	{CC(G_CC_DECALRGBA),			"G_CC_DECALRGBA"},
	{CC(G_CC_BLENDI),			"G_CC_BLENDI"},
	{CC(G_CC_BLENDIA),			"G_CC_BLENDIA"},
	{CC(G_CC_BLENDIDECALA),			"G_CC_BLENDIDECALA"},
	{CC(G_CC_BLENDRGBA),			"G_CC_BLENDRGBA"},
	{CC(G_CC_BLENDRGBDECALA),		"G_CC_BLENDRGBDECALA"},
	{CC(G_CC_REFLECTRGB),			"G_CC_REFLECTRGB"},
	{CC(G_CC_REFLECTRGBDECALA),		"G_CC_REFLECTRGBDECALA"},
	{CC(G_CC_HILITERGB),			"G_CC_HILITERGB"},
	{CC(G_CC_HILITERGBA),			"G_CC_HILITERGBA"},
	{CC(G_CC_HILITERGBDECALA),		"G_CC_HILITERGBDECALA"},
	{CC(G_CC_1CYUV2RGB),			"G_CC_1CYUV2RGB"},
	{CC(G_CC_PRIMITIVE),			"G_CC_PRIMITIVE"},
	{CC(G_CC_SHADE),			"G_CC_SHADE"},
	{CC(G_CC_ADDRGB),			"G_CC_ADDRGB"},
	{CC(G_CC_ADDRGBDECALA),			"G_CC_ADDRGBDECALA"},
	{CC(G_CC_SHADEDECALA),			"G_CC_SHADEDECALA"},
	{CC(G_CC_BLENDPE),			"G_CC_BLENDPE"},
	{CC(G_CC_BLENDPEDECALA),		"G_CC_BLENDPEDECALA"},
	{CC(G_CC_TRILERP),			"G_CC_TRILERP"},
	{CC(G_CC_TEMPLERP),			"G_CC_TEMPLERP"},
	{CC(G_CC_INTERFERENCE),			"G_CC_INTERFERENCE"},
	{CC(_G_CC_BLENDPE),			"_G_CC_BLENDPE"},
	{CC(_G_CC_BLENDPEDECALA),		"_G_CC_BLENDPEDECALA"},
	{CC(_G_CC_SPARSEST),			"_G_CC_SPARSEST"},
	{CC(_G_CC_TWOCOLORTEX),			"_G_CC_TWOCOLORTEX"},
	{CC(G_CC_MODULATEI2),			"G_CC_MODULATEI2"},
	{CC(G_CC_MODULATEIA2),			"G_CC_MODULATEIA2"},
	{CC(G_CC_MODULATERGB2),			"G_CC_MODULATERGB2"},
	{CC(G_CC_MODULATERGBA2),		"G_CC_MODULATERGBA2"},
	{CC(G_CC_MODULATEI_PRIM2),		"G_CC_MODULATEI_PRIM2"},
	{CC(G_CC_MODULATEIA_PRIM2),		"G_CC_MODULATEIA_PRIM2"},
	{CC(G_CC_MODULATERGB_PRIM2),		"G_CC_MODULATERGB_PRIM2"},
	{CC(G_CC_MODULATERGBA_PRIM2),		"G_CC_MODULATERGBA_PRIM2"},
	{CC(G_CC_DECALRGB2),			"G_CC_DECALRGB2"},
	{CC(G_CC_BLENDI2),			"G_CC_BLENDI2"},
	{CC(G_CC_BLENDIA2),			"G_CC_BLENDIA2"},
	{CC(G_CC_HILITERGB2),			"G_CC_HILITERGB2"},
	{CC(G_CC_HILITERGBA2),			"G_CC_HILITERGBA2"},
	{CC(G_CC_HILITERGBDECALA2),		"G_CC_HILITERGBDECALA2"},
	{CC(G_CC_HILITERGBPASSA2),		"G_CC_HILITERGBPASSA2"},
	{CC(G_CC_CHROMA_KEY2),			"G_CC_CHROMA_KEY2"},
	{CC(G_CC_YUV2RGB),			"G_CC_YUV2RGB"},
	{CC(G_CC_PASS2),			"G_CC_PASS2"},
};
#undef CC_
#undef CC

UCFUNC int argfn_ccpre(const gfxd_value_t *v)
{
	return gfxd_printf("%s", cc_presets[v->i].name);
}

UCFUNC int argfn_sc(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_SC_NON_INTERLACE:
			return gfxd_puts("G_SC_NON_INTERLACE");
		case G_SC_EVEN_INTERLACE:
			return gfxd_puts("G_SC_EVEN_INTERLACE");
		case G_SC_ODD_INTERLACE:
			return gfxd_puts("G_SC_ODD_INTERLACE");
		default:
			return gfxd_printf("%" PRIi32, v->i);
	}
}

#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
UCFUNC int argfn_bz(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_BZ_PERSP:
			return gfxd_puts("G_BZ_PERSP");
		default:
			return gfxd_puts("G_BZ_ORTHO");
	}
}
#endif

UCFUNC int argfn_dlf(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_DL_PUSH:
			return gfxd_puts("G_DL_PUSH");
		case G_DL_NOPUSH:
			return gfxd_puts("G_DL_NOPUSH");
		default:
			return gfxd_printf("%" PRIi32, v->i);;
	}
}

UCFUNC int argfn_mp(const gfxd_value_t *v)
{
	int n = 0;
	if (v->u & G_MTX_PUSH)
		n += gfxd_puts("G_MTX_PUSH");
	else
		n += gfxd_puts("G_MTX_NOPUSH");
	if (v->u & G_MTX_LOAD)
		n += gfxd_puts(" | G_MTX_LOAD");
	else
		n += gfxd_puts(" | G_MTX_MUL");
	if (v->u & G_MTX_PROJECTION)
		n += gfxd_puts(" | G_MTX_PROJECTION");
	else
		n += gfxd_puts(" | G_MTX_MODELVIEW");
	for (int i = 3; i < 8; i++)
		if (v->u & (1 << i))
			n += gfxd_printf(" | 0x%02x", 1 << i);
	return n;
}

UCFUNC int argfn_ms(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_MTX_MODELVIEW:
			return gfxd_puts("G_MTX_MODELVIEW");
		case G_MTX_PROJECTION:
			return gfxd_puts("G_MTX_PROJECTION");
		default:
			return gfxd_printf("%" PRIi32, v->i);
	}
}

UCFUNC int argfn_mw(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_MW_MATRIX:
			return gfxd_puts("G_MW_MATRIX");
		case G_MW_NUMLIGHT:
			return gfxd_puts("G_MW_NUMLIGHT");
		case G_MW_CLIP:
			return gfxd_puts("G_MW_CLIP");
		case G_MW_SEGMENT:
			return gfxd_puts("G_MW_SEGMENT");
		case G_MW_FOG:
			return gfxd_puts("G_MW_FOG");
		case G_MW_LIGHTCOL:
			return gfxd_puts("G_MW_LIGHTCOL");
		case G_MW_PERSPNORM:
			return gfxd_puts("G_MW_PERSPNORM");
#if defined(F3D_GBI) || defined(F3DEX_GBI)
		case G_MW_POINTS:
			return gfxd_puts("G_MW_POINTS");
#endif
#if defined(F3DEX_GBI_2)
		case G_MW_FORCEMTX:
			return gfxd_puts("G_MW_FORCEMTX");
#endif
		default:
			return gfxd_printf("%" PRIi32, v->i);
	}
}

UCFUNC int argfn_mwo_clip(const gfxd_value_t *v)
{
	switch (v->u)
	{
		case G_MWO_CLIP_RNX:
			return gfxd_puts("G_MWO_CLIP_RNX");
		case G_MWO_CLIP_RNY:
			return gfxd_puts("G_MWO_CLIP_RNY");
		case G_MWO_CLIP_RPX:
			return gfxd_puts("G_MWO_CLIP_RPX");
		case G_MWO_CLIP_RPY:
			return gfxd_puts("G_MWO_CLIP_RPY");
		default:
			return gfxd_printf("0x%04" PRIX32, v->u);
	}
}

UCFUNC int argfn_mwo_lightcol(const gfxd_value_t *v)
{
	switch (v->u)
	{
		case G_MWO_aLIGHT_1:
			return gfxd_puts("G_MWO_aLIGHT_1");
		case G_MWO_bLIGHT_1:
			return gfxd_puts("G_MWO_bLIGHT_1");
		case G_MWO_aLIGHT_2:
			return gfxd_puts("G_MWO_aLIGHT_2");
		case G_MWO_bLIGHT_2:
			return gfxd_puts("G_MWO_bLIGHT_2");
		case G_MWO_aLIGHT_3:
			return gfxd_puts("G_MWO_aLIGHT_3");
		case G_MWO_bLIGHT_3:
			return gfxd_puts("G_MWO_bLIGHT_3");
		case G_MWO_aLIGHT_4:
			return gfxd_puts("G_MWO_aLIGHT_4");
		case G_MWO_bLIGHT_4:
			return gfxd_puts("G_MWO_bLIGHT_4");
		case G_MWO_aLIGHT_5:
			return gfxd_puts("G_MWO_aLIGHT_5");
		case G_MWO_bLIGHT_5:
			return gfxd_puts("G_MWO_bLIGHT_5");
		case G_MWO_aLIGHT_6:
			return gfxd_puts("G_MWO_aLIGHT_6");
		case G_MWO_bLIGHT_6:
			return gfxd_puts("G_MWO_bLIGHT_6");
		case G_MWO_aLIGHT_7:
			return gfxd_puts("G_MWO_aLIGHT_7");
		case G_MWO_bLIGHT_7:
			return gfxd_puts("G_MWO_bLIGHT_7");
		case G_MWO_aLIGHT_8:
			return gfxd_puts("G_MWO_aLIGHT_8");
		case G_MWO_bLIGHT_8:
			return gfxd_puts("G_MWO_bLIGHT_8");
		default:
			return gfxd_printf("0x%04" PRIX32, v->u);
	}
}

UCFUNC int argfn_lightnum(const gfxd_value_t *v)
{
	return gfxd_printf("LIGHT_%" PRIi32, v->i);
}

UCFUNC int argfn_lightsn(const gfxd_value_t *v)
{
	return gfxd_printf("*(Lightsn *)0x%08" PRIX32, v->u);
}

UCFUNC int argfn_mwo_matrix(const gfxd_value_t *v)
{
	switch (v->u)
	{
		case G_MWO_MATRIX_XX_XY_I:
			return gfxd_puts("G_MWO_MATRIX_XX_XY_I");
		case G_MWO_MATRIX_XZ_XW_I:
			return gfxd_puts("G_MWO_MATRIX_XZ_XW_I");
		case G_MWO_MATRIX_YX_YY_I:
			return gfxd_puts("G_MWO_MATRIX_YX_YY_I");
		case G_MWO_MATRIX_YZ_YW_I:
			return gfxd_puts("G_MWO_MATRIX_YZ_YW_I");
		case G_MWO_MATRIX_ZX_ZY_I:
			return gfxd_puts("G_MWO_MATRIX_ZX_ZY_I");
		case G_MWO_MATRIX_ZZ_ZW_I:
			return gfxd_puts("G_MWO_MATRIX_ZZ_ZW_I");
		case G_MWO_MATRIX_WX_WY_I:
			return gfxd_puts("G_MWO_MATRIX_WX_WY_I");
		case G_MWO_MATRIX_WZ_WW_I:
			return gfxd_puts("G_MWO_MATRIX_WZ_WW_I");
		case G_MWO_MATRIX_XX_XY_F:
			return gfxd_puts("G_MWO_MATRIX_XX_XY_F");
		case G_MWO_MATRIX_XZ_XW_F:
			return gfxd_puts("G_MWO_MATRIX_XZ_XW_F");
		case G_MWO_MATRIX_YX_YY_F:
			return gfxd_puts("G_MWO_MATRIX_YX_YY_F");
		case G_MWO_MATRIX_YZ_YW_F:
			return gfxd_puts("G_MWO_MATRIX_YZ_YW_F");
		case G_MWO_MATRIX_ZX_ZY_F:
			return gfxd_puts("G_MWO_MATRIX_ZX_ZY_F");
		case G_MWO_MATRIX_ZZ_ZW_F:
			return gfxd_puts("G_MWO_MATRIX_ZZ_ZW_F");
		case G_MWO_MATRIX_WX_WY_F:
			return gfxd_puts("G_MWO_MATRIX_WX_WY_F");
		case G_MWO_MATRIX_WZ_WW_F:
			return gfxd_puts("G_MWO_MATRIX_WZ_WW_F");
		default:
			return gfxd_printf("0x%04" PRIX32, v->u);
	}
}

UCFUNC int argfn_mwo_point(const gfxd_value_t *v)
{
	switch (v->u)
	{
		case G_MWO_POINT_RGBA:
			return gfxd_puts("G_MWO_POINT_RGBA");
		case G_MWO_POINT_ST:
			return gfxd_puts("G_MWO_POINT_ST");
		case G_MWO_POINT_XYSCREEN:
			return gfxd_puts("G_MWO_POINT_XYSCREEN");
		case G_MWO_POINT_ZSCREEN:
			return gfxd_puts("G_MWO_POINT_ZSCREEN");
		default:
			return gfxd_printf("0x%04" PRIX32, v->u);
	}
}

UCFUNC int argfn_mv(const gfxd_value_t *v)
{
	switch (v->i)
	{
		case G_MV_VIEWPORT:
			return gfxd_puts("G_MV_VIEWPORT");
#if defined(F3D_GBI) || defined(F3DEX_GBI)
		case G_MV_LOOKATY:
			return gfxd_puts("G_MV_LOOKATY");
		case G_MV_LOOKATX:
			return gfxd_puts("G_MV_LOOKATX");
		case G_MV_L0:
			return gfxd_puts("G_MV_L0");
		case G_MV_L1:
			return gfxd_puts("G_MV_L1");
		case G_MV_L2:
			return gfxd_puts("G_MV_L2");
		case G_MV_L3:
			return gfxd_puts("G_MV_L3");
		case G_MV_L4:
			return gfxd_puts("G_MV_L4");
		case G_MV_L5:
			return gfxd_puts("G_MV_L5");
		case G_MV_L6:
			return gfxd_puts("G_MV_L6");
		case G_MV_L7:
			return gfxd_puts("G_MV_L7");
		case G_MV_TXTATT:
			return gfxd_puts("G_MV_TXTATT");
		case G_MV_MATRIX_2:
			return gfxd_puts("G_MV_MATRIX_2");
		case G_MV_MATRIX_3:
			return gfxd_puts("G_MV_MATRIX_3");
		case G_MV_MATRIX_4:
			return gfxd_puts("G_MV_MATRIX_4");
		case G_MV_MATRIX_1:
			return gfxd_puts("G_MV_MATRIX_1");
#elif defined(F3DEX_GBI_2)
		case G_MV_MMTX:
			return gfxd_puts("G_MV_MMTX");
		case G_MV_PMTX:
			return gfxd_puts("G_MV_PMTX");
		case G_MV_LIGHT:
			return gfxd_puts("G_MV_LIGHT");
		case G_MV_POINT:
			return gfxd_puts("G_MV_POINT");
		case G_MV_MATRIX:
			return gfxd_puts("G_MV_MATRIX");
#endif
		default:
			return gfxd_printf("%" PRIi32, v->i);
	}
}

UCFUNC int argfn_cr(const gfxd_value_t *v)
{
	switch (v->u)
	{
		case FRUSTRATIO_1:
			return gfxd_puts("FRUSTRATIO_1");
		case FRUSTRATIO_2:
			return gfxd_puts("FRUSTRATIO_2");
		case FRUSTRATIO_3:
			return gfxd_puts("FRUSTRATIO_3");
		case FRUSTRATIO_4:
			return gfxd_puts("FRUSTRATIO_4");
		case FRUSTRATIO_5:
			return gfxd_puts("FRUSTRATIO_5");
		case FRUSTRATIO_6:
			return gfxd_puts("FRUSTRATIO_6");
		default:
			return gfxd_printf("%" PRIu32, v->u);
	}
}
