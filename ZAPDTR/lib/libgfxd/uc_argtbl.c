static const gfxd_arg_type_t arg_tbl[] =
{
	[gfxd_Word] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Opcode] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_opc,
	},
	[gfxd_Coordi] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_u,
	},
	[gfxd_Coordq] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_qu102,
	},
	[gfxd_Pal] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_i,
	},
	[gfxd_Tlut] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Timg] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Tmem] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x16,
	},
	[gfxd_Tile] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_tile,
	},
	[gfxd_Fmt] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_fmt,
	},
	[gfxd_Siz] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_siz,
	},
	[gfxd_Dim] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_i,
	},
	[gfxd_Cm] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_cm,
	},
	[gfxd_Tm] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_tm,
	},
	[gfxd_Ts] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_ts,
	},
	[gfxd_Dxt] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_u,
	},
	[gfxd_Tag] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Pm] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_pm,
	},
	[gfxd_Colorpart] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_color,
	},
	[gfxd_Color] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Lodfrac] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_qu08,
	},
	[gfxd_Cimg] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Zimg] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Ac] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_ac,
	},
	[gfxd_Ad] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_ad,
	},
	[gfxd_Cd] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_cd,
	},
	[gfxd_Ccpre] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_ccpre,
	},
	[gfxd_Ccmuxa] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_ccmuxa,
	},
	[gfxd_Ccmuxb] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_ccmuxb,
	},
	[gfxd_Ccmuxc] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_ccmuxc,
	},
	[gfxd_Ccmuxd] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_ccmuxd,
	},
	[gfxd_Acmuxabd] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_acmuxabd,
	},
	[gfxd_Acmuxc] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_acmuxc,
	},
	[gfxd_Cv] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_cv,
	},
	[gfxd_Tc] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_tc,
	},
	[gfxd_Cyc] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_cyc,
	},
	[gfxd_Zs] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_zs,
	},
	[gfxd_Ck] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_ck,
	},
	[gfxd_Keyscale] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_color,
	},
	[gfxd_Keywidth] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_qs48,
	},
	[gfxd_Zi] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_i,
	},
	[gfxd_Rm1] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_rm1,
	},
	[gfxd_Rm2] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_rm2,
	},
	[gfxd_Sc] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_sc,
	},
	[gfxd_Td] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_td,
	},
	[gfxd_Tf] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_tf,
	},
	[gfxd_Tl] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_tl,
	},
	[gfxd_Tt] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_tt,
	},
	[gfxd_Tp] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_tp,
	},
	[gfxd_Line] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_i,
	},
	[gfxd_Vtx] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_i,
	},
	[gfxd_Vtxflag] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_i,
	},
	[gfxd_Dl] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
	[gfxd_Zraw] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_qs1616,
	},
#endif
	[gfxd_Dlflag] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_dlf,
	},
	[gfxd_Cr] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_cr,
	},
	[gfxd_Num] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_i,
	},
	[gfxd_Fogz] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_i,
	},
	[gfxd_Fogp] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_i,
	},
	[gfxd_Mtxptr] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Gm] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_gm,
	},
	[gfxd_Mwo_matrix] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_mwo_matrix,
	},
	[gfxd_Linewd] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_i,
	},
	[gfxd_Uctext] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Ucdata] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Size] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x16,
	},
	[gfxd_Lookatptr] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Mtxparam] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_mp,
	},
	[gfxd_Mtxstack] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_ms,
	},
	[gfxd_Mwo_point] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_mwo_point,
	},
	[gfxd_Wscale] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_qu016,
	},
	[gfxd_Seg] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x8,
	},
	[gfxd_Segptr] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Lightsn] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_lightsn,
	},
	[gfxd_Numlights] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_i,
	},
	[gfxd_Lightnum] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_lightnum,
	},
	[gfxd_Lightptr] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Tcscale] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_qu016,
	},
	[gfxd_Switch] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_switch,
	},
	[gfxd_St] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_qs105,
	},
	[gfxd_Stdelta] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_qs510,
	},
	[gfxd_Vtxptr] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Vpptr] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Dram] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x32,
	},
	[gfxd_Sftlo] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_sftlo,
	},
	[gfxd_Othermodelo] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_othermodelo,
	},
	[gfxd_Sfthi] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_sfthi,
	},
	[gfxd_Othermodehi] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_othermodehi,
	},
	[gfxd_Mw] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_mw,
	},
	[gfxd_Mwo] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x16,
	},
	[gfxd_Mwo_clip] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_mwo_clip,
	},
	[gfxd_Mwo_lightcol] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_mwo_lightcol,
	},
	[gfxd_Mv] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_mv,
	},
	[gfxd_Mvo] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x16,
	},
	[gfxd_Dmem] =
	{
		.fmt = gfxd_argfmt_u,
		.fn = argfn_x16,
	},
	[gfxd_Dmaflag] =
	{
		.fmt = gfxd_argfmt_i,
		.fn = argfn_i,
	},
};
