static inline uint32_t getfield(uint32_t w, int n, int s)
{
	return (w >> s) & (((uint32_t)1 << n) - 1);
}

static inline int32_t argvi(gfxd_macro_t *m, int idx)
{
	return m->arg[idx].value.i;
}

static inline uint32_t argvu(gfxd_macro_t *m, int idx)
{
	return m->arg[idx].value.u;
}

static inline float argvf(gfxd_macro_t *m, int idx)
{
	return m->arg[idx].value.f;
}

static inline void argi(gfxd_macro_t *m, int idx, const char *name,
	int32_t value, int type)
{
	m->arg[idx].type = type;
	m->arg[idx].name = name;
	m->arg[idx].value.i = value;
	m->arg[idx].bad = 0;
}

static inline void argu(gfxd_macro_t *m, int idx, const char *name,
	uint32_t value, int type)
{
	m->arg[idx].type = type;
	m->arg[idx].name = name;
	m->arg[idx].value.u = value;
	m->arg[idx].bad = 0;
}

static inline void argf(gfxd_macro_t *m, int idx, const char *name,
	float value, int type)
{
	m->arg[idx].type = type;
	m->arg[idx].name = name;
	m->arg[idx].value.f = value;
	m->arg[idx].bad = 0;
}

static inline void badarg(gfxd_macro_t *m, int idx)
{
	m->arg[idx].bad = 1;
}

UCFUNC int32_t sx(uint32_t n, int bits)
{
	int32_t smin = (int32_t)1 << (bits - 1);
	int32_t smax = (int32_t)1 << bits;
	int32_t i = n;
	if (i < smin)
		return i;
	else
		return i - smax;
}

UCFUNC int d_Invalid(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_Invalid;
	argu(m, 0, "hi", hi, gfxd_Word);
	argu(m, 1, "lo", lo, gfxd_Word);
	return -1;
}

UCFUNC int d_DPFillRectangle(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPFillRectangle;
	argu(m, 0, "ulx", getfield(lo, 10, 14), gfxd_Coordi);
	argu(m, 1, "uly", getfield(lo, 10, 2), gfxd_Coordi);
	argu(m, 2, "lrx", getfield(hi, 10, 14), gfxd_Coordi);
	argu(m, 3, "lry", getfield(hi, 10, 2), gfxd_Coordi);
	return 0;
}

UCFUNC int d_DPFullSync(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPFullSync;
	return 0;
}

UCFUNC int d_DPLoadSync(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPLoadSync;
	return 0;
}

UCFUNC int d_DPTileSync(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPTileSync;
	return 0;
}

UCFUNC int d_DPPipeSync(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPPipeSync;
	return 0;
}

UCFUNC int c_DPLoadTLUT_pal16(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 6)
		return -1;
	if (m[0].id != gfxd_DPSetTextureImage
		|| argvi(&m[0], 0) != G_IM_FMT_RGBA
		|| argvi(&m[0], 1) != G_IM_SIZ_16b
		|| argvi(&m[0], 2) != 1)
	{
		return -1;
	}
	uint32_t dram = argvu(&m[0], 3);
	if (m[1].id != gfxd_DPTileSync)
		return -1;
	if (m[2].id != gfxd_DPSetTile
		|| argvi(&m[2], 0) != 0
		|| argvi(&m[2], 1) != 0
		|| argvi(&m[2], 2) != 0
		|| argvu(&m[2], 3) < 0x100
		|| argvu(&m[2], 3) % 0x10 != 0
		|| argvi(&m[2], 4) != G_TX_LOADTILE
		|| argvi(&m[2], 5) != 0
		|| argvu(&m[2], 6) != 0
		|| argvi(&m[2], 7) != 0
		|| argvi(&m[2], 8) != 0
		|| argvu(&m[2], 9) != 0
		|| argvi(&m[2], 10) != 0
		|| argvi(&m[2], 11) != 0)
	{
		return -1;
	}
	int pal = (argvu(&m[2], 3) - 0x100) / 0x10;
	if (m[3].id != gfxd_DPLoadSync)
		return -1;
	if (m[4].id != gfxd_DPLoadTLUTCmd
		|| argvi(&m[4], 0) != G_TX_LOADTILE
		|| argvi(&m[4], 1) != 15)
	{
		return -1;
	}
	if (m[5].id != gfxd_DPPipeSync)
		return -1;
	m->id = gfxd_DPLoadTLUT_pal16;
	argi(m, 0, "pal", pal, gfxd_Pal);
	argu(m, 1, "dram", dram, gfxd_Tlut);
	return 0;
}

UCFUNC int c_DPLoadTLUT_pal256(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 6)
		return -1;
	if (m[0].id != gfxd_DPSetTextureImage
		|| argvi(&m[0], 0) != G_IM_FMT_RGBA
		|| argvi(&m[0], 1) != G_IM_SIZ_16b
		|| argvi(&m[0], 2) != 1)
	{
		return -1;
	}
	uint32_t dram = argvu(&m[0], 3);
	if (m[1].id != gfxd_DPTileSync)
		return -1;
	if (m[2].id != gfxd_DPSetTile
		|| argvi(&m[2], 0) != 0
		|| argvi(&m[2], 1) != 0
		|| argvi(&m[2], 2) != 0
		|| argvu(&m[2], 3) != 0x100
		|| argvi(&m[2], 4) != G_TX_LOADTILE
		|| argvi(&m[2], 5) != 0
		|| argvu(&m[2], 6) != 0
		|| argvi(&m[2], 7) != 0
		|| argvi(&m[2], 8) != 0
		|| argvu(&m[2], 9) != 0
		|| argvi(&m[2], 10) != 0
		|| argvi(&m[2], 11) != 0)
	{
		return -1;
	}
	if (m[3].id != gfxd_DPLoadSync)
		return -1;
	if (m[4].id != gfxd_DPLoadTLUTCmd
		|| argvi(&m[4], 0) != G_TX_LOADTILE
		|| argvi(&m[4], 1) != 255)
	{
		return -1;
	}
	if (m[5].id != gfxd_DPPipeSync)
		return -1;
	m->id = gfxd_DPLoadTLUT_pal256;
	argu(m, 0, "dram", dram, gfxd_Tlut);
	return 0;
}

UCFUNC int c_ltb(gfxd_macro_t *m, int n_macro, int id, int mdxt, int mtmem,
	int mrt, int myuv, int m4b)
{
	if (n_macro < 7)
		return -1;
	if (m[0].id != gfxd_DPSetTextureImage || argvi(&m[0], 2) != 1)
		return -1;
	g_ifmt_t fmt = argvi(&m[0], 0);
	g_isiz_t ldsiz = argvi(&m[0], 1);
	uint32_t timg = argvu(&m[0], 3);
	if (myuv && fmt != G_IM_FMT_YUV)
		return -1;
	if (m[1].id != gfxd_DPSetTile
		|| argvi(&m[1], 0) != fmt
		|| argvi(&m[1], 1) != ldsiz
		|| argvi(&m[1], 2) != 0
		|| argvi(&m[1], 4) != G_TX_LOADTILE
		|| argvi(&m[1], 5) != 0)
	{
		return -1;
	}
	uint32_t tmem = argvu(&m[1], 3);
	unsigned cms = argvu(&m[1], 9);
	unsigned cmt = argvu(&m[1], 6);
	int masks = argvi(&m[1], 10);
	int maskt = argvi(&m[1], 7);
	int shifts = argvi(&m[1], 11);
	int shiftt = argvi(&m[1], 8);
	if (m[2].id != gfxd_DPLoadSync)
		return -1;
	if (m[3].id != gfxd_DPLoadBlock
		|| argvi(&m[3], 0) != G_TX_LOADTILE
		|| argvu(&m[3], 1) != 0
		|| argvu(&m[3], 2) != 0)
	{
		return -1;
	}
	qu102_t ldlrs = argvu(&m[3], 3);
	unsigned lddxt = argvu(&m[3], 4);
	if (m[4].id != gfxd_DPPipeSync)
		return -1;
	if (m[5].id != gfxd_DPSetTile
		|| argvi(&m[5], 0) != fmt
		|| G_SIZ_LDSIZ(argvi(&m[5], 1)) != ldsiz
		|| argvu(&m[5], 3) != tmem
		|| argvu(&m[5], 6) != cmt
		|| argvi(&m[5], 7) != maskt
		|| argvi(&m[5], 8) != shiftt
		|| argvu(&m[5], 9) != cms
		|| argvi(&m[5], 10) != masks
		|| argvi(&m[5], 11) != shifts)
	{
		return -1;
	}
	int siz = argvi(&m[5], 1);
	int rdline = argvi(&m[5], 2);
	int rt = argvi(&m[5], 4);
	int pal = argvi(&m[5], 5);
	if (m4b && siz != G_IM_SIZ_4b)
		return -1;
	if (!(mrt && rt != G_TX_RENDERTILE && tmem == 0)
		&& (tmem != 0) != mtmem)
	{
		return -1;
	}
	if ((rt != G_TX_RENDERTILE) != mrt)
		return -1;
	if (m[6].id != gfxd_DPSetTileSize
		|| argvi(&m[6], 0) != rt
		|| argvu(&m[6], 1) != 0
		|| argvu(&m[6], 2) != 0
		|| (argvu(&m[6], 3) & 3)
		|| (argvu(&m[6], 4) & 3))
	{
		return -1;
	}
	int width = (argvu(&m[6], 3) >> 2) + 1;
	int height = (argvu(&m[6], 4) >> 2) + 1;
	unsigned lrs = G_LDBLK_TXL(G_LTB_LRS(width, height, siz));
	unsigned dxt = 0;
	if (!mdxt)
		dxt = G_DXT(siz, width);
	int line;
	if (myuv)
		line = (width + 7) / 8;
	else
		line = (width * G_SIZ_LDBITS(siz) + 63) / 64;
	if (ldlrs != lrs || lddxt != dxt || rdline != line)
		return -1;
	m->id = id;
	int i = 0;
	argu(m, i++, "timg", timg, gfxd_Timg);
	if (mtmem)
		argu(m, i++, "tmem", tmem, gfxd_Tmem);
	if (mrt)
		argi(m, i++, "rtile", rt, gfxd_Tile);
	argi(m, i++, "fmt", fmt, gfxd_Fmt);
	if (!m4b)
		argi(m, i++, "siz", siz, gfxd_Siz);
	argi(m, i++, "width", width, gfxd_Dim);
	argi(m, i++, "height", height, gfxd_Dim);
	argi(m, i++, "pal", pal, gfxd_Pal);
	argu(m, i++, "cms", cms, gfxd_Cm);
	argu(m, i++, "cmt", cmt, gfxd_Cm);
	argi(m, i++, "masks", masks, gfxd_Tm);
	argi(m, i++, "maskt", maskt, gfxd_Tm);
	argi(m, i++, "shifts", shifts, gfxd_Ts);
	argi(m, i++, "shiftt", shiftt, gfxd_Ts);
	return 0;
}

UCFUNC int c_DPLoadMultiBlockYuvS(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd_DPLoadMultiBlockYuvS, 1, 1, 1, 1, 0);
}

UCFUNC int c_DPLoadMultiBlockYuv(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd_DPLoadMultiBlockYuv, 0, 1, 1, 1, 0);
}

UCFUNC int c_DPLoadMultiBlock_4bS(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd_DPLoadMultiBlock_4bS, 1, 1, 1, 0, 1);
}

UCFUNC int c_DPLoadMultiBlock_4b(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd_DPLoadMultiBlock_4b, 0, 1, 1, 0, 1);
}

UCFUNC int c_DPLoadMultiBlockS(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd_DPLoadMultiBlockS, 1, 1, 1, 0, 0);
}

UCFUNC int c_DPLoadMultiBlock(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd_DPLoadMultiBlock, 0, 1, 1, 0, 0);
}

UCFUNC int c__DPLoadTextureBlockYuvS(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd__DPLoadTextureBlockYuvS, 1, 1, 0, 1, 0);
}

UCFUNC int c__DPLoadTextureBlockYuv(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd__DPLoadTextureBlockYuv, 0, 1, 0, 1, 0);
}

UCFUNC int c__DPLoadTextureBlock_4bS(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd__DPLoadTextureBlock_4bS, 1, 1, 0, 0, 1);
}

UCFUNC int c__DPLoadTextureBlock_4b(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd__DPLoadTextureBlock_4b, 0, 1, 0, 0, 1);
}

UCFUNC int c__DPLoadTextureBlockS(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd__DPLoadTextureBlockS, 1, 1, 0, 0, 0);
}

UCFUNC int c__DPLoadTextureBlock(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd__DPLoadTextureBlock, 0, 1, 0, 0, 0);
}

UCFUNC int c_DPLoadTextureBlockYuvS(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd_DPLoadTextureBlockYuvS, 1, 0, 0, 1, 0);
}

UCFUNC int c_DPLoadTextureBlockYuv(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd_DPLoadTextureBlockYuv, 0, 0, 0, 1, 0);
}

UCFUNC int c_DPLoadTextureBlock_4bS(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd_DPLoadTextureBlock_4bS, 1, 0, 0, 0, 1);
}

UCFUNC int c_DPLoadTextureBlock_4b(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd_DPLoadTextureBlock_4b, 0, 0, 0, 0, 1);
}

UCFUNC int c_DPLoadTextureBlockS(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd_DPLoadTextureBlockS, 1, 0, 0, 0, 0);
}

UCFUNC int c_DPLoadTextureBlock(gfxd_macro_t *m, int n_macro)
{
	return c_ltb(m, n_macro, gfxd_DPLoadTextureBlock, 0, 0, 0, 0, 0);
}

UCFUNC int c_ltt(gfxd_macro_t *m, int n_macro, int id, int mtmem, int mrt,
	int myuv, int m4b)
{
	if (n_macro < 7)
		return -1;
	if (m[0].id != gfxd_DPSetTextureImage)
		return -1;
	g_ifmt_t fmt = argvi(&m[0], 0);
	g_isiz_t ldsiz = argvi(&m[0], 1);
	int width = argvi(&m[0], 2);
	if (m4b)
	{
		if (ldsiz != G_IM_SIZ_8b)
			return -1;
		width *= 2;
	}
	uint32_t timg = argvu(&m[0], 3);
	if (myuv && fmt != G_IM_FMT_YUV)
		return -1;
	if (m[1].id != gfxd_DPSetTile
		|| argvi(&m[1], 0) != fmt
		|| argvi(&m[1], 1) != ldsiz
		|| argvi(&m[1], 4) != G_TX_LOADTILE
		|| argvi(&m[1], 5) != 0)
	{
		return -1;
	}
	int ldline = argvi(&m[1], 2);
	uint32_t tmem = argvu(&m[1], 3);
	unsigned cms = argvu(&m[1], 9);
	unsigned cmt = argvu(&m[1], 6);
	int masks = argvi(&m[1], 10);
	int maskt = argvi(&m[1], 7);
	int shifts = argvi(&m[1], 11);
	int shiftt = argvi(&m[1], 8);
	if (m[2].id != gfxd_DPLoadSync)
		return -1;
	if (m[3].id != gfxd_DPLoadTile
		|| argvi(&m[3], 0) != G_TX_LOADTILE
		|| (argvu(&m[3], 1) & 1)
		|| (argvu(&m[3], 2) & 3)
		|| (argvu(&m[3], 3) & 1)
		|| (argvu(&m[3], 4) & 3))
	{
		return -1;
	}
	qu102_t lduls = argvu(&m[3], 1);
	qu102_t ldult = argvu(&m[3], 2);
	qu102_t ldlrs = argvu(&m[3], 3);
	qu102_t ldlrt = argvu(&m[3], 4);
	if (m[4].id != gfxd_DPPipeSync)
		return -1;
	if (m[5].id != gfxd_DPSetTile
		|| argvi(&m[5], 0) != fmt
		|| argvi(&m[5], 2) != ldline
		|| argvu(&m[5], 3) != tmem
		|| argvu(&m[5], 6) != cmt
		|| argvi(&m[5], 7) != maskt
		|| argvi(&m[5], 8) != shiftt
		|| argvu(&m[5], 9) != cms
		|| argvi(&m[5], 10) != masks
		|| argvi(&m[5], 11) != shifts)
	{
		return -1;
	}
	int siz = argvi(&m[5], 1);
	int rt = argvi(&m[5], 4);
	int pal = argvi(&m[5], 5);
	if (m4b)
	{
		if (siz != G_IM_SIZ_4b)
			return -1;
	}
	else if (siz != ldsiz)
		return -1;
	if (!(mrt && rt != G_TX_RENDERTILE && tmem == 0)
		&& (tmem != 0) != mtmem)
	{
		return -1;
	}
	if ((rt != G_TX_RENDERTILE) != mrt)
		return -1;
	if (m[6].id != gfxd_DPSetTileSize
		|| argvi(&m[6], 0) != rt
		|| (argvu(&m[6], 1) & 3)
		|| (argvu(&m[6], 2) & 3)
		|| (argvu(&m[6], 3) & 3)
		|| (argvu(&m[6], 4) & 3))
	{
		return -1;
	}
	unsigned uls = argvu(&m[6], 1) >> 2;
	unsigned ult = argvu(&m[6], 2) >> 2;
	unsigned lrs = argvu(&m[6], 3) >> 2;
	unsigned lrt = argvu(&m[6], 4) >> 2;
	int line;
	if (myuv)
		line = ((lrs - uls + 1) + 7) / 8;
	else if (m4b)
		line = ((lrs - uls + 1) / 2 + 7) / 8;
	else
		line = ((lrs - uls + 1) * G_SIZ_LDBITS(siz) + 63) / 64;
	if (m4b)
	{
		if (lduls != qu102(uls) / 2 || ldlrs != qu102(lrs) / 2)
			return -1;
	}
	else if (lduls != qu102(uls) || ldlrs != qu102(lrs))
		return -1;
	if (ldult != qu102(ult) || ldlrt != qu102(lrt) || ldline != line)
		return -1;
	m->id = id;
	int i = 0;
	argu(m, i++, "timg", timg, gfxd_Timg);
	if (mtmem)
		argu(m, i++, "tmem", tmem, gfxd_Tmem);
	if (mrt)
		argi(m, i++, "rtile", rt, gfxd_Tile);
	argi(m, i++, "fmt", fmt, gfxd_Fmt);
	if (!m4b)
		argi(m, i++, "siz", siz, gfxd_Siz);
	argi(m, i++, "width", width, gfxd_Dim);
	argi(m, i++, "height", 0, gfxd_Dim);
	argu(m, i++, "uls", uls, gfxd_Coordi);
	argu(m, i++, "ult", ult, gfxd_Coordi);
	argu(m, i++, "lrs", lrs, gfxd_Coordi);
	argu(m, i++, "lrt", lrt, gfxd_Coordi);
	argi(m, i++, "pal", pal, gfxd_Pal);
	argu(m, i++, "cms", cms, gfxd_Cm);
	argu(m, i++, "cmt", cmt, gfxd_Cm);
	argi(m, i++, "masks", masks, gfxd_Tm);
	argi(m, i++, "maskt", maskt, gfxd_Tm);
	argi(m, i++, "shifts", shifts, gfxd_Ts);
	argi(m, i++, "shiftt", shiftt, gfxd_Ts);
	return 0;
}

UCFUNC int c_DPLoadMultiTileYuv(gfxd_macro_t *m, int n_macro)
{
	return c_ltt(m, n_macro, gfxd_DPLoadMultiTileYuv, 1, 1, 1, 0);
}

UCFUNC int c_DPLoadMultiTile_4b(gfxd_macro_t *m, int n_macro)
{
	return c_ltt(m, n_macro, gfxd_DPLoadMultiTile_4b, 1, 1, 0, 1);
}

UCFUNC int c_DPLoadMultiTile(gfxd_macro_t *m, int n_macro)
{
	return c_ltt(m, n_macro, gfxd_DPLoadMultiTile, 1, 1, 0, 0);
}

UCFUNC int c__DPLoadTextureTileYuv(gfxd_macro_t *m, int n_macro)
{
	return c_ltt(m, n_macro, gfxd__DPLoadTextureTileYuv, 1, 0, 1, 0);
}

UCFUNC int c__DPLoadTextureTile_4b(gfxd_macro_t *m, int n_macro)
{
	return c_ltt(m, n_macro, gfxd__DPLoadTextureTile_4b, 1, 0, 0, 1);
}

UCFUNC int c__DPLoadTextureTile(gfxd_macro_t *m, int n_macro)
{
	return c_ltt(m, n_macro, gfxd__DPLoadTextureTile, 1, 0, 0, 0);
}

UCFUNC int c_DPLoadTextureTileYuv(gfxd_macro_t *m, int n_macro)
{
	return c_ltt(m, n_macro, gfxd_DPLoadTextureTileYuv, 0, 0, 1, 0);
}

UCFUNC int c_DPLoadTextureTile_4b(gfxd_macro_t *m, int n_macro)
{
	return c_ltt(m, n_macro, gfxd_DPLoadTextureTile_4b, 0, 0, 0, 1);
}

UCFUNC int c_DPLoadTextureTile(gfxd_macro_t *m, int n_macro)
{
	return c_ltt(m, n_macro, gfxd_DPLoadTextureTile, 0, 0, 0, 0);
}

UCFUNC int d_DPLoadBlock(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPLoadBlock;
	argi(m, 0, "tile", getfield(lo, 3, 24), gfxd_Tile);
	argu(m, 1, "uls", getfield(hi, 12, 12), gfxd_Coordi);
	argu(m, 2, "ult", getfield(hi, 12, 0), gfxd_Coordi);
	argu(m, 3, "lrs", getfield(lo, 12, 12), gfxd_Coordi);
	argu(m, 4, "dxt", getfield(lo, 12, 0), gfxd_Dxt);
	if (argvu(m, 3) > G_TX_LDBLK_MAX_TXL) {
		badarg(m, 3);
		return -1;
	}
	else {
		return 0;
	}
}

UCFUNC int d_DPNoOp(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPNoOp;
	return 0;
}

UCFUNC int d_DPNoOpTag(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	if (lo == 0)
		return d_DPNoOp(m, hi, lo);
	else
	{
		m->id = gfxd_DPNoOpTag;
		argu(m, 0, "tag", lo, gfxd_Tag);
		return 0;
	}
}

UCFUNC int d_DPPipelineMode(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPPipelineMode;
	argu(m, 0, "mode", lo, gfxd_Pm);
	return 0;
}

UCFUNC int d_DPSetBlendColor(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetBlendColor;
	argu(m, 0, "r", getfield(lo, 8, 24), gfxd_Colorpart);
	argu(m, 1, "g", getfield(lo, 8, 16), gfxd_Colorpart);
	argu(m, 2, "b", getfield(lo, 8, 8), gfxd_Colorpart);
	argu(m, 3, "a", getfield(lo, 8, 0), gfxd_Colorpart);
	return 0;
}

UCFUNC int d_DPSetEnvColor(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetEnvColor;
	argu(m, 0, "r", getfield(lo, 8, 24), gfxd_Colorpart);
	argu(m, 1, "g", getfield(lo, 8, 16), gfxd_Colorpart);
	argu(m, 2, "b", getfield(lo, 8, 8), gfxd_Colorpart);
	argu(m, 3, "a", getfield(lo, 8, 0), gfxd_Colorpart);
	return 0;
}

UCFUNC int d_DPSetFillColor(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetFillColor;
	argu(m, 0, "c", lo, gfxd_Color);
	return 0;
}

UCFUNC int d_DPSetFogColor(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetFogColor;
	argu(m, 0, "r", getfield(lo, 8, 24), gfxd_Colorpart);
	argu(m, 1, "g", getfield(lo, 8, 16), gfxd_Colorpart);
	argu(m, 2, "b", getfield(lo, 8, 8), gfxd_Colorpart);
	argu(m, 3, "a", getfield(lo, 8, 0), gfxd_Colorpart);
	return 0;
}

UCFUNC int d_DPSetPrimColor(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetPrimColor;
	argu(m, 0, "m", getfield(hi, 8, 8), gfxd_Lodfrac);
	argu(m, 1, "l", getfield(hi, 8, 0), gfxd_Lodfrac);
	argu(m, 2, "r", getfield(lo, 8, 24), gfxd_Colorpart);
	argu(m, 3, "g", getfield(lo, 8, 16), gfxd_Colorpart);
	argu(m, 4, "b", getfield(lo, 8, 8), gfxd_Colorpart);
	argu(m, 5, "a", getfield(lo, 8, 0), gfxd_Colorpart);
	return 0;
}

UCFUNC int d_DPSetColorImage(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetColorImage;
	argi(m, 0, "fmt", getfield(hi, 3, 21), gfxd_Fmt);
	argi(m, 1, "siz", getfield(hi, 2, 19), gfxd_Siz);
	argi(m, 2, "width", getfield(hi, 12, 0) + 1, gfxd_Dim);
	argu(m, 3, "cimg", lo, gfxd_Cimg);
	return 0;
}

UCFUNC int d_DPSetDepthImage(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetDepthImage;
	argu(m, 0, "zimg", lo, gfxd_Zimg);
	return 0;
}

UCFUNC int d_DPSetTextureImage(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetTextureImage;
	argi(m, 0, "fmt", getfield(hi, 3, 21), gfxd_Fmt);
	argi(m, 1, "siz", getfield(hi, 2, 19), gfxd_Siz);
	argi(m, 2, "width", getfield(hi, 12, 0) + 1, gfxd_Dim);
	argu(m, 3, "timg", lo, gfxd_Timg);
	return 0;
}

UCFUNC int d_DPSetAlphaCompare(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetAlphaCompare;
	argu(m, 0, "mode", lo, gfxd_Ac);
	return 0;
}

UCFUNC int d_DPSetAlphaDither(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetAlphaDither;
	argu(m, 0, "mode", lo, gfxd_Ad);
	return 0;
}

UCFUNC int d_DPSetColorDither(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetColorDither;
	argu(m, 0, "mode", lo, gfxd_Cd);
	return 0;
}

UCFUNC void cc_unpack(struct cc_mode *m0, struct cc_mode *m1, uint32_t hi,
			uint32_t lo)
{
	m0->a = getfield(hi, 4, 20);
	m0->b = getfield(lo, 4, 28);
	m0->c = getfield(hi, 5, 15);
	m0->d = getfield(lo, 3, 15);
	m0->Aa = getfield(hi, 3, 12);
	m0->Ab = getfield(lo, 3, 12);
	m0->Ac = getfield(hi, 3, 9);
	m0->Ad = getfield(lo, 3, 9);
	m1->a = getfield(hi, 4, 5);
	m1->b = getfield(lo, 4, 24);
	m1->c = getfield(hi, 5, 0);
	m1->d = getfield(lo, 3, 6);
	m1->Aa = getfield(lo, 3, 21);
	m1->Ab = getfield(lo, 3, 3);
	m1->Ac = getfield(lo, 3, 18);
	m1->Ad = getfield(lo, 3, 0);
}

UCFUNC int cc_lookup(const struct cc_mode *m)
{
	struct cc_mode m_norm = *m;
	if (m_norm.a > 0x7) m_norm.a = G_CCMUX_0;
	if (m_norm.b > 0x7) m_norm.b = G_CCMUX_0;
	if (m_norm.c > 0xF) m_norm.c = G_CCMUX_0;
	if (m_norm.d > 0x6) m_norm.d = G_CCMUX_0;
	m = &m_norm;
	int n_presets = sizeof(cc_presets) / sizeof(*cc_presets);
	for (int i = 0; i < n_presets; i++)
	{
		const struct cc_mode *p = &cc_presets[i].mode;
		if (m->a == p->a
			&& m->b == p->b
			&& m->c == p->c
			&& m->d == p->d
			&& m->Aa == p->Aa
			&& m->Ab == p->Ab
			&& m->Ac == p->Ac
			&& m->Ad == p->Ad)
		{
			return i;
		}
	}
	return -1;
}

UCFUNC int d_DPSetCombineMode(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetCombineMode;
	struct cc_mode m0;
	struct cc_mode m1;
	cc_unpack(&m0, &m1, hi, lo);
	int p0 = cc_lookup(&m0);
	int p1 = cc_lookup(&m1);
	argi(m, 0, "mode1", p0, gfxd_Ccpre);
	argi(m, 1, "mode2", p1, gfxd_Ccpre);
	int ret = 0;
	if (p0 == -1)
	{
		badarg(m, 0);
		ret = -1;
	}
	if (p1 == -1)
	{
		badarg(m, 1);
		ret = -1;
	}
	return ret;
}

UCFUNC int d_DPSetCombineLERP(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	struct cc_mode m0;
	struct cc_mode m1;
	cc_unpack(&m0, &m1, hi, lo);
	int p0 = cc_lookup(&m0);
	int p1 = cc_lookup(&m1);
	if (p0 != -1 && p1 != -1)
		return d_DPSetCombineMode(m, hi, lo);
	else
	{
		m->id = gfxd_DPSetCombineLERP;
		argi(m, 0, "a0", m0.a, gfxd_Ccmuxa);
		argi(m, 1, "b0", m0.b, gfxd_Ccmuxb);
		argi(m, 2, "c0", m0.c, gfxd_Ccmuxc);
		argi(m, 3, "d0", m0.d, gfxd_Ccmuxd);
		argi(m, 4, "Aa0", m0.Aa, gfxd_Acmuxabd);
		argi(m, 5, "Ab0", m0.Ab, gfxd_Acmuxabd);
		argi(m, 6, "Ac0", m0.Ac, gfxd_Acmuxc);
		argi(m, 7, "Ad0", m0.Ad, gfxd_Acmuxabd);
		argi(m, 8, "a1", m1.a, gfxd_Ccmuxa);
		argi(m, 9, "b1", m1.b, gfxd_Ccmuxb);
		argi(m, 10, "c1", m1.c, gfxd_Ccmuxc);
		argi(m, 11, "d1", m1.d, gfxd_Ccmuxd);
		argi(m, 12, "Aa1", m1.Aa, gfxd_Acmuxabd);
		argi(m, 13, "Ab1", m1.Ab, gfxd_Acmuxabd);
		argi(m, 14, "Ac1", m1.Ac, gfxd_Acmuxc);
		argi(m, 15, "Ad1", m1.Ad, gfxd_Acmuxabd);
		return 0;
	}
}

UCFUNC int d_DPSetConvert(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetConvert;
	argi(m, 0, "k0", sx(getfield(hi, 9, 13), 9), gfxd_Cv);
	argi(m, 1, "k1", sx(getfield(hi, 9, 4), 9), gfxd_Cv);
	argi(m, 2, "k2", sx((getfield(hi, 4, 0) << 5) | getfield(lo, 5, 27), 9),
		gfxd_Cv);
	argi(m, 3, "k3", sx(getfield(lo, 9, 18), 9), gfxd_Cv);
	argi(m, 4, "k4", sx(getfield(lo, 9, 9), 9), gfxd_Cv);
	argi(m, 5, "k5", sx(getfield(lo, 9, 0), 9), gfxd_Cv);
	return 0;
}

UCFUNC int d_DPSetTextureConvert(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetTextureConvert;
	argu(m, 0, "mode", lo, gfxd_Tc);
	return 0;
}

UCFUNC int d_DPSetCycleType(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetCycleType;
	argu(m, 0, "mode", lo, gfxd_Cyc);
	return 0;
}

UCFUNC int d_DPSetDepthSource(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetDepthSource;
	argu(m, 0, "mode", lo, gfxd_Zs);
	return 0;
}

UCFUNC int d_DPSetCombineKey(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetCombineKey;
	argu(m, 0, "mode", lo, gfxd_Ck);
	return 0;
}

UCFUNC int d_DPSetKeyGB(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetKeyGB;
	argu(m, 0, "cG", getfield(lo, 8, 24), gfxd_Color);
	argu(m, 1, "sG", getfield(lo, 8, 16), gfxd_Keyscale);
	argi(m, 2, "wG", sx(getfield(hi, 12, 12), 12), gfxd_Keywidth);
	argu(m, 3, "cB", getfield(lo, 8, 8), gfxd_Color);
	argu(m, 4, "sB", getfield(lo, 8, 0), gfxd_Keyscale);
	argi(m, 5, "wB", sx(getfield(hi, 12, 0), 12), gfxd_Keywidth);
	return 0;
}

UCFUNC int d_DPSetKeyR(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetKeyR;
	argu(m, 0, "cR", getfield(lo, 8, 8), gfxd_Color);
	argu(m, 1, "sR", getfield(lo, 8, 0), gfxd_Keyscale);
	argi(m, 2, "wR", sx(getfield(lo, 12, 16), 12), gfxd_Keywidth);
	return 0;
}

UCFUNC int d_DPSetPrimDepth(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetPrimDepth;
	argi(m, 0, "z", sx(getfield(lo, 16, 16), 16), gfxd_Zi);
	argi(m, 1, "dz", sx(getfield(lo, 16, 0), 16), gfxd_Zi);
	return 0;
}

UCFUNC int d_DPSetRenderMode(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetRenderMode;
	argu(m, 0, "mode1", lo, gfxd_Rm1);
	argu(m, 1, "mode2", lo, gfxd_Rm2);
	return 0;
}

UCFUNC int d_DPSetScissor(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetScissor;
	argi(m, 0, "mode", getfield(lo, 2, 24), gfxd_Sc);
	argu(m, 1, "ulx", getfield(hi, 10, 14), gfxd_Coordi);
	argu(m, 2, "uly", getfield(hi, 10, 2), gfxd_Coordi);
	argu(m, 3, "lrx", getfield(lo, 10, 14), gfxd_Coordi);
	argu(m, 4, "lry", getfield(lo, 10, 2), gfxd_Coordi);
	return 0;
}

UCFUNC int d_DPSetScissorFrac(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	qu102_t ulx = getfield(hi, 12, 12);
	qu102_t uly = getfield(hi, 12, 0);
	qu102_t lrx = getfield(lo, 12, 12);
	qu102_t lry = getfield(lo, 12, 0);
	if ((ulx & 3) || (uly & 3) || (lrx & 3) || (lry & 3))
	{
		m->id = gfxd_DPSetScissorFrac;
		argi(m, 0, "mode", getfield(lo, 2, 24), gfxd_Sc);
		argu(m, 1, "ulx", ulx, gfxd_Coordq);
		argu(m, 2, "uly", uly, gfxd_Coordq);
		argu(m, 3, "lrx", lrx, gfxd_Coordq);
		argu(m, 4, "lry", lry, gfxd_Coordq);
		return 0;
	}
	else
		return d_DPSetScissor(m, hi, lo);
}

UCFUNC int d_DPSetTextureDetail(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetTextureDetail;
	argu(m, 0, "mode", lo, gfxd_Td);
	return 0;
}

UCFUNC int d_DPSetTextureFilter(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetTextureFilter;
	argu(m, 0, "mode", lo, gfxd_Tf);
	return 0;
}

UCFUNC int d_DPSetTextureLOD(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetTextureLOD;
	argu(m, 0, "mode", lo, gfxd_Tl);
	return 0;
}

UCFUNC int d_DPSetTextureLUT(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetTextureLUT;
	argu(m, 0, "mode", lo, gfxd_Tt);
	return 0;
}

UCFUNC int d_DPSetTexturePersp(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetTexturePersp;
	argu(m, 0, "mode", lo, gfxd_Tp);
	return 0;
}

UCFUNC int d_DPSetTile(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetTile;
	argi(m, 0, "fmt", getfield(hi, 3, 21), gfxd_Fmt);
	argi(m, 1, "siz", getfield(hi, 2, 19), gfxd_Siz);
	argi(m, 2, "line", getfield(hi, 9, 9), gfxd_Line);
	argu(m, 3, "tmem", getfield(hi, 9, 0), gfxd_Tmem);
	argi(m, 4, "tile", getfield(lo, 3, 24), gfxd_Tile);
	argi(m, 5, "pal", getfield(lo, 4, 20), gfxd_Pal);
	argu(m, 6, "cmt", getfield(lo, 2, 18), gfxd_Cm);
	argi(m, 7, "maskt", getfield(lo, 4, 14), gfxd_Tm);
	argi(m, 8, "shiftt", getfield(lo, 4, 10), gfxd_Ts);
	argu(m, 9, "cms", getfield(lo, 2, 8), gfxd_Cm);
	argi(m, 10, "masks", getfield(lo, 4, 4), gfxd_Tm);
	argi(m, 11, "shifts", getfield(lo, 4, 0), gfxd_Ts);
	return 0;
}

UCFUNC int d_DPSetTileSize(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetTileSize;
	argi(m, 0, "tile", getfield(lo, 3, 24), gfxd_Tile);
	argu(m, 1, "uls", getfield(hi, 12, 12), gfxd_Coordq);
	argu(m, 2, "ult", getfield(hi, 12, 0), gfxd_Coordq);
	argu(m, 3, "lrs", getfield(lo, 12, 12), gfxd_Coordq);
	argu(m, 4, "lrt", getfield(lo, 12, 0), gfxd_Coordq);
	return 0;
}

#if defined(F3D_GBI)
UCFUNC int d_SP1Triangle(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SP1Triangle;
	int n0 = getfield(lo, 8, 16);
	int n1 = getfield(lo, 8, 8);
	int n2 = getfield(lo, 8, 0);
	argi(m, 0, "v0", n0 / 10, gfxd_Vtx);
	argi(m, 1, "v1", n1 / 10, gfxd_Vtx);
	argi(m, 2, "v2", n2 / 10, gfxd_Vtx);
	argi(m, 3, "flag", getfield(lo, 8, 24), gfxd_Vtxflag);
	int ret = 0;
	if (n0 % 10 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	if (n1 % 10 != 0)
	{
		badarg(m, 1);
		ret = -1;
	}
	if (n2 % 10 != 0)
	{
		badarg(m, 2);
		ret = -1;
	}
	return ret;
}
#elif defined(F3DEX_GBI)
UCFUNC int d_SP1Triangle(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SP1Triangle;
	int n0 = getfield(lo, 8, 16);
	int n1 = getfield(lo, 8, 8);
	int n2 = getfield(lo, 8, 0);
	argi(m, 0, "v0", n0 / 2, gfxd_Vtx);
	argi(m, 1, "v1", n1 / 2, gfxd_Vtx);
	argi(m, 2, "v2", n2 / 2, gfxd_Vtx);
	argi(m, 3, "flag", 0, gfxd_Vtxflag);
	int ret = 0;
	if (n0 % 2 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	if (n1 % 2 != 0)
	{
		badarg(m, 1);
		ret = -1;
	}
	if (n2 % 2 != 0)
	{
		badarg(m, 2);
		ret = -1;
	}
	return ret;
}
#elif defined(F3DEX_GBI_2)
UCFUNC int d_SP1Triangle(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SP1Triangle;
	int n0 = getfield(hi, 8, 16);
	int n1 = getfield(hi, 8, 8);
	int n2 = getfield(hi, 8, 0);
	argi(m, 0, "v0", n0 / 2, gfxd_Vtx);
	argi(m, 1, "v1", n1 / 2, gfxd_Vtx);
	argi(m, 2, "v2", n2 / 2, gfxd_Vtx);
	argi(m, 3, "flag", 0, gfxd_Vtxflag);
	int ret = 0;
	if (n0 % 2 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	if (n1 % 2 != 0)
	{
		badarg(m, 1);
		ret = -1;
	}
	if (n2 % 2 != 0)
	{
		badarg(m, 2);
		ret = -1;
	}
	return ret;
}
#endif

#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
UCFUNC int d_SP1Quadrangle(gfxd_macro_t *m, uint32_t hi, uint32_t lo);
UCFUNC int d_SP2Triangles(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	int n00 = getfield(hi, 8, 16);
	int n01 = getfield(hi, 8, 8);
	int n02 = getfield(hi, 8, 0);
	int n10 = getfield(lo, 8, 16);
	int n11 = getfield(lo, 8, 8);
	int n12 = getfield(lo, 8, 0);
#if defined(F3DEX_GBI)
	if (n00 == n10 && n02 == n11)
		return d_SP1Quadrangle(m, hi, lo);
#endif
	m->id = gfxd_SP2Triangles;
	argi(m, 0, "v00", n00 / 2, gfxd_Vtx);
	argi(m, 1, "v01", n01 / 2, gfxd_Vtx);
	argi(m, 2, "v02", n02 / 2, gfxd_Vtx);
	argi(m, 3, "flag0", 0, gfxd_Vtxflag);
	argi(m, 4, "v10", n10 / 2, gfxd_Vtx);
	argi(m, 5, "v11", n11 / 2, gfxd_Vtx);
	argi(m, 6, "v12", n12 / 2, gfxd_Vtx);
	argi(m, 7, "flag1", 0, gfxd_Vtxflag);
	int ret = 0;
	if (n00 % 2 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	if (n01 % 2 != 0)
	{
		badarg(m, 1);
		ret = -1;
	}
	if (n02 % 2 != 0)
	{
		badarg(m, 2);
		ret = -1;
	}
	if (n10 % 2 != 0)
	{
		badarg(m, 4);
		ret = -1;
	}
	if (n11 % 2 != 0)
	{
		badarg(m, 5);
		ret = -1;
	}
	if (n12 % 2 != 0)
	{
		badarg(m, 6);
		ret = -1;
	}
	return ret;
}

UCFUNC int d_SP1Quadrangle(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SP1Quadrangle;
	int n00 = getfield(hi, 8, 16);
	int n01 = getfield(hi, 8, 8);
	int n02 = getfield(hi, 8, 0);
	int n10 = getfield(lo, 8, 16);
	int n11 = getfield(lo, 8, 8);
	int n12 = getfield(lo, 8, 0);
	int v00 = n00 / 2;
	int v01 = n01 / 2;
	int v02 = n02 / 2;
	int v10 = n10 / 2;
	int v11 = n11 / 2;
	int v12 = n12 / 2;
	argi(m, 0, "v0", v00, gfxd_Vtx);
	argi(m, 1, "v1", v01, gfxd_Vtx);
	argi(m, 2, "v2", v11, gfxd_Vtx);
	argi(m, 3, "v3", v12, gfxd_Vtx);
	argi(m, 4, "flag", 0, gfxd_Vtxflag);
	int ret = 0;
	if (v00 != v10 || n00 % 2 != 0 || n10 % 2 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	if (n01 % 2 != 0)
	{
		badarg(m, 1);
		ret = -1;
	}
	if (v02 != v11 || n02 % 2 != 0 || n11 % 2 != 0)
	{
		badarg(m, 2);
		ret = -1;
	}
	if (n12 % 2 != 0)
	{
		badarg(m, 3);
		ret = -1;
	}
	return ret;
}

UCFUNC int c_SPBranchLessZraw(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 2)
		return -1;
	if (m[0].id != gfxd_DPHalf1)
		return -1;
	uint32_t branchdl = argvu(&m[0], 0);
	if (m[1].id != gfxd_BranchZ)
		return -1;
	int32_t vtx = argvi(&m[1], 0);
	int32_t zval = argvi(&m[1], 1);
	m->id = gfxd_SPBranchLessZraw;
	argu(m, 0, "dl", branchdl, gfxd_Dl);
	argi(m, 1, "vtx", vtx, gfxd_Vtx);
	argi(m, 2, "zval", zval, gfxd_Zraw);
	return 0;
}
#endif

UCFUNC int d_SPBranchList(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPBranchList;
	argu(m, 0, "dl", lo, gfxd_Dl);
	return 0;
}

UCFUNC int c_SPClipRatio(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 4)
		return -1;
	if (m[0].id != gfxd_MoveWd
		|| argvi(&m[0], 0) != G_MW_CLIP
		|| argvu(&m[0], 1) != G_MWO_CLIP_RNX)
	{
		return -1;
	}
	uint32_t r = argvu(&m[0], 2);
	if (m[1].id != gfxd_MoveWd
		|| argvi(&m[1], 0) != G_MW_CLIP
		|| argvu(&m[1], 1) != G_MWO_CLIP_RNY
		|| argvu(&m[1], 2) != r)
	{
		return -1;
	}
	if (m[2].id != gfxd_MoveWd
		|| argvi(&m[2], 0) != G_MW_CLIP
		|| argvu(&m[2], 1) != G_MWO_CLIP_RPX
		|| ((uint32_t)1 << 16) - argvu(&m[2], 2) != r)
	{
		return -1;
	}
	if (m[3].id != gfxd_MoveWd
		|| argvi(&m[3], 0) != G_MW_CLIP
		|| argvu(&m[3], 1) != G_MWO_CLIP_RPY
		|| ((uint32_t)1 << 16) - argvu(&m[3], 2) != r)
	{
		return -1;
	}
	m->id = gfxd_SPClipRatio;
	argi(m, 0, "r", r, gfxd_Cr);
	return 0;
}

#if defined(F3D_GBI)
UCFUNC int d_SPCullDisplayList(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPCullDisplayList;
	int n0 = getfield(hi, 24, 0);
	int nn = getfield(lo, 16, 0);
	argi(m, 0, "v0", n0 / 40, gfxd_Vtx);
	argi(m, 1, "vn", nn / 40 - 1, gfxd_Num);
	int ret = 0;
	if (n0 % 40 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	if (nn % 40 != 0)
	{
		badarg(m, 1);
		ret = -1;
	}
	return ret;
}
#elif defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
UCFUNC int d_SPCullDisplayList(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPCullDisplayList;
	int n0 = getfield(hi, 16, 0);
	int nn = getfield(lo, 16, 0);
	argi(m, 0, "v0", n0 / 2, gfxd_Vtx);
	argi(m, 1, "vn", nn / 2, gfxd_Num);
	int ret = 0;
	if (n0 % 2 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	if (nn % 2 != 0)
	{
		badarg(m, 1);
		ret = -1;
	}
	return ret;
}
#endif

UCFUNC int d_SPDisplayList(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPDisplayList;
	argu(m, 0, "dl", lo, gfxd_Dl);
	return 0;
}

UCFUNC int d_SPEndDisplayList(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPEndDisplayList;
	return 0;
}

UCFUNC int d_SPFogFactor(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPFogFactor;
	argi(m, 0, "fm", sx(getfield(lo, 16, 16), 16), gfxd_Fogz);
	argi(m, 1, "fo", sx(getfield(lo, 16, 0), 16), gfxd_Fogz);
	return 0;
}

UCFUNC int d_SPFogPosition(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	int x = sx(getfield(lo, 16, 16), 16);
	int y = sx(getfield(lo, 16, 0), 16);
	if (x == 0)
		return d_SPFogFactor(m, hi, lo);
	else
	{
		int d = 128000 / x;
		int yd = y * d;
		if (yd > 0)
			yd += 255;
		else if (yd < 0)
			yd -= 255;
		int min = 500 - yd / 256;
		int max = d + min;

		if (min >= 0 && min <= 1000 && max >= 0 && max <= 1000)
		{
			m->id = gfxd_SPFogPosition;
			argi(m, 0, "min", min, gfxd_Fogp);
			argi(m, 1, "max", max, gfxd_Fogp);
			return 0;
		}
		else
			return d_SPFogFactor(m, hi, lo);
	}
}

#if defined(F3D_GBI) || defined(F3DEX_GBI)
UCFUNC int c_SPForceMatrix(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 4)
		return -1;
	for (int i = 0; i < 4; i++)
		if (m[i].id != gfxd_MoveMem
			|| argvu(&m[i], 0) != 16
			|| argvu(&m[i], 2) != argvu(&m[0], 2) + i * 16)
		{
			return -1;
		}
	if (argvi(&m[0], 1) != G_MV_MATRIX_1
		|| argvi(&m[1], 1) != G_MV_MATRIX_2
		|| argvi(&m[2], 1) != G_MV_MATRIX_3
		|| argvi(&m[3], 1) != G_MV_MATRIX_4)
	{
		return -1;
	}
	uint32_t mptr = argvu(&m[0], 2);
	m->id = gfxd_SPForceMatrix;
	argu(m, 0, "mptr", mptr, gfxd_Mtxptr);
	return 0;
}
#elif defined(F3DEX_GBI_2)
UCFUNC int c_SPForceMatrix(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 2)
		return -1;
	if (m[0].id != gfxd_MoveMem
		|| argvu(&m[0], 0) != sizeof(Mtx)
		|| argvi(&m[0], 1) != G_MV_MATRIX
		|| argvu(&m[0], 2) != 0)
	{
		return -1;
	}
	uint32_t mptr = argvu(&m[0], 3);
	if (m[1].id != gfxd_MoveWd
		|| argvi(&m[1], 0) != G_MW_FORCEMTX
		|| argvu(&m[1], 1) != 0
		|| argvu(&m[1], 2) != 0x10000)
	{
		return -1;
	}
	m->id = gfxd_SPForceMatrix;
	argu(m, 0, "mptr", mptr, gfxd_Mtxptr);
	return 0;
}
#endif

UCFUNC int d_SPSetGeometryMode(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPSetGeometryMode;
	argu(m, 0, "mode", lo, gfxd_Gm);
	return 0;
}

UCFUNC int d_SPClearGeometryMode(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPClearGeometryMode;
#if defined(F3D_GBI) || defined(F3DEX_GBI)
	argu(m, 0, "mode", lo, gfxd_Gm);
#elif defined(F3DEX_GBI_2)
	argu(m, 0, "mode", getfield(~hi, 24, 0), gfxd_Gm);
#endif
	return 0;
}

#if defined(F3D_GBI) || defined(F3DEX_GBI)
UCFUNC int c_SPLoadGeometryMode(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 2)
		return -1;
	if (m[0].id != gfxd_SPClearGeometryMode
		|| argvu(&m[0], 0) != 0xFFFFFFFF
		|| m[1].id != gfxd_SPSetGeometryMode)
	{
		return -1;
	}
	uint32_t mode = argvu(&m[1], 0);
	m->id = gfxd_SPLoadGeometryMode;
	argu(m, 0, "mode", mode, gfxd_Gm);
	return 0;
}
#elif defined(F3DEX_GBI_2)
UCFUNC int d_SPLoadGeometryMode(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPLoadGeometryMode;
	argu(m, 0, "mode", lo, gfxd_Gm);
	return 0;
}
#endif

#if defined(F3D_GBI) || defined(F3DEX_GBI)
UCFUNC int d_SPInsertMatrix(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPInsertMatrix;
	argu(m, 0, "where", getfield(hi, 16, 8), gfxd_Mwo_matrix);
	argu(m, 1, "val", lo, gfxd_Word);
	return 0;
}
#endif

#if defined(F3D_GBI)
UCFUNC int d_SPLine3D(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPLine3D;
	int n0 = getfield(lo, 8, 16);
	int n1 = getfield(lo, 8, 8);
	argi(m, 0, "v0", n0 / 10, gfxd_Vtx);
	argi(m, 1, "v1", n1 / 10, gfxd_Vtx);
	argi(m, 2, "flag", getfield(lo, 8, 24), gfxd_Vtxflag);
	int ret = 0;
	if (n0 % 10 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	if (n1 % 10 != 0)
	{
		badarg(m, 1);
		ret = -1;
	}
	return ret;
}
#elif defined(F3DEX_GBI)
UCFUNC int d_SPLine3D(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPLine3D;
	int n0 = getfield(lo, 8, 16);
	int n1 = getfield(lo, 8, 8);
	argi(m, 0, "v0", n0 / 2, gfxd_Vtx);
	argi(m, 1, "v1", n1 / 2, gfxd_Vtx);
	argi(m, 2, "flag", 0, gfxd_Vtxflag);
	int ret = 0;
	if (n0 % 2 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	if (n1 % 2 != 0)
	{
		badarg(m, 1);
		ret = -1;
	}
	return ret;
}
#elif defined(F3DEX_GBI_2)
UCFUNC int d_SPLine3D(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPLine3D;
	int n0 = getfield(hi, 8, 16);
	int n1 = getfield(hi, 8, 8);
	argi(m, 0, "v0", n0 / 2, gfxd_Vtx);
	argi(m, 1, "v1", n1 / 2, gfxd_Vtx);
	argi(m, 2, "flag", 0, gfxd_Vtxflag);
	int ret = 0;
	if (n0 % 2 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	if (n1 % 2 != 0)
	{
		badarg(m, 1);
		ret = -1;
	}
	return ret;
}
#endif

#if defined(F3D_GBI)
UCFUNC int d_SPLineW3D(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	int wd = getfield(lo, 8, 0);
	if (wd == 0)
		return d_SPLine3D(m, hi, lo);
	else
	{
		m->id = gfxd_SPLineW3D;
		int n0 = getfield(lo, 8, 16);
		int n1 = getfield(lo, 8, 8);
		argi(m, 0, "v0", n0 / 10, gfxd_Vtx);
		argi(m, 1, "v1", n1 / 10, gfxd_Vtx);
		argi(m, 2, "wd", wd, gfxd_Linewd);
		argi(m, 3, "flag", getfield(lo, 8, 24), gfxd_Vtxflag);
		int ret = 0;
		if (n0 % 10 != 0)
		{
			badarg(m, 0);
			ret = -1;
		}
		if (n1 % 10 != 0)
		{
			badarg(m, 1);
			ret = -1;
		}
		return ret;
	}
}
#elif defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
UCFUNC int d_SPLineW3D(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	int wd = getfield(hi, 8, 0);
	if (wd == 0)
		return d_SPLine3D(m, hi, lo);
	else
	{
		m->id = gfxd_SPLineW3D;
		int n0 = getfield(hi, 8, 16);
		int n1 = getfield(hi, 8, 8);
		argi(m, 0, "v0", n0 / 2, gfxd_Vtx);
		argi(m, 1, "v1", n1 / 2, gfxd_Vtx);
		argi(m, 2, "wd", wd, gfxd_Linewd);
		argi(m, 3, "flag", 0, gfxd_Vtxflag);
		int ret = 0;
		if (n0 % 2 != 0)
		{
			badarg(m, 0);
			ret = -1;
		}
		if (n1 % 2 != 0)
		{
			badarg(m, 1);
			ret = -1;
		}
		return ret;
	}
}
#endif

#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
UCFUNC int c_SPLoadUcode(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 2)
		return -1;
	if (m[0].id != gfxd_DPHalf1)
		return -1;
	uint32_t uc_dstart = argvu(&m[0], 0);
	if (m[1].id != gfxd_LoadUcode)
		return -1;
	uint32_t uc_start = argvu(&m[1], 0);
	uint32_t uc_dsize = argvu(&m[1], 1);
	if (uc_dsize != 0x800)
		return -1;
	m->id = gfxd_SPLoadUcode;
	argu(m, 0, "uc_start", uc_start, gfxd_Uctext);
	argu(m, 1, "uc_dstart", uc_dstart, gfxd_Ucdata);
	return 0;
}
#endif

UCFUNC int d_SPLookAtX(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPLookAtX;
	argu(m, 0, "l", lo, gfxd_Lookatptr);
	return 0;
}

UCFUNC int d_SPLookAtY(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPLookAtY;
	argu(m, 0, "l", lo, gfxd_Lookatptr);
	return 0;
}

UCFUNC int c_SPLookAt(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 2)
		return -1;
	if (m[0].id != gfxd_SPLookAtX)
		return -1;
	uint32_t l = argvu(&m[0], 0);
	if (m[1].id != gfxd_SPLookAtY || argvu(&m[1], 0) != l + 0x10)
		return -1;
	m->id = gfxd_SPLookAt;
	argu(m, 0, "l", l, gfxd_Lookatptr);
	return 0;
}

#if defined(F3D_GBI) || defined(F3DEX_GBI)
UCFUNC int d_SPMatrix(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPMatrix;
	int x = getfield(hi, 16, 0);
	argu(m, 0, "matrix", lo, gfxd_Mtxptr);
	argi(m, 1, "param", getfield(hi, 8, 16), gfxd_Mtxparam);
	if (x != sizeof(Mtx))
		return -1;
	else
		return 0;
}
#elif defined(F3DEX_GBI_2)
UCFUNC int d_SPMatrix(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPMatrix;
	int x = getfield(hi, 5, 19);
	argu(m, 0, "matrix", lo, gfxd_Mtxptr);
	argi(m, 1, "param", getfield(hi, 8, 0) ^ G_MTX_PUSH, gfxd_Mtxparam);
	if (x != (sizeof(Mtx) - 1) / 8)
		return -1;
	else
		return 0;
}
#endif

#if defined(F3D_GBI) || (defined(F3D_BETA) && defined(F3DEX_GBI))
UCFUNC int d_SPModifyVertex(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPModifyVertex;
	int offset = getfield(hi, 16, 8);
	argi(m, 0, "vtx", offset / 40, gfxd_Vtx);
	argu(m, 1, "where", offset % 40, gfxd_Mwo_point);
	argu(m, 2, "val", lo, gfxd_Word);
	return 0;
}
#elif defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
UCFUNC int d_SPModifyVertex(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPModifyVertex;
	int vtx = getfield(hi, 16, 0);
	argi(m, 0, "vtx", vtx / 2, gfxd_Vtx);
	argu(m, 1, "where", getfield(hi, 8, 16), gfxd_Mwo_point);
	argu(m, 2, "val", lo, gfxd_Word);
	int ret = 0;
	if (vtx % 2 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	return ret;
}
#endif

UCFUNC int d_SPPerspNormalize(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPPerspNormalize;
	argu(m, 0, "scale", getfield(lo, 16, 0), gfxd_Wscale);
	return 0;
}

UCFUNC int d_SPPopMatrix(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPPopMatrix;
#if defined(F3D_GBI) || defined(F3DEX_GBI)
	argi(m, 0, "param", lo, gfxd_Mtxstack);
#elif defined(F3DEX_GBI_2)
	argi(m, 0, "param", G_MTX_MODELVIEW, gfxd_Mtxstack);
#endif
	return 0;
}

#if defined(F3DEX_GBI_2)
UCFUNC int d_SPPopMatrixN(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	int len = (getfield(hi, 5, 19) + 1) * 8;
	int ofs = getfield(hi, 8, 8) * 8;
	int idx = getfield(hi, 8, 0);
	int n = lo / sizeof(Mtx);
	if (lo % sizeof(Mtx) == 0
		&& len == sizeof(Mtx)
		&& ofs == 0
		&& idx == 2
		&& n == 1)
	{
		return d_SPPopMatrix(m, hi, lo);
	}
	m->id = gfxd_SPPopMatrixN;
	argi(m, 0, "param", G_MTX_MODELVIEW, gfxd_Mtxstack);
	argi(m, 1, "num", n, gfxd_Num);
	int ret = 0;
	if (lo % sizeof(Mtx) != 0)
	{
		badarg(m, 1);
		ret = -1;
	}
	if (len != sizeof(Mtx) || ofs != 0 || idx != 2)
		ret = -1;
	return ret;
}
#endif

UCFUNC int d_SPSegment(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPSegment;
#if defined(F3D_GBI) || defined(F3DEX_GBI)
	int offset = getfield(hi, 16, 8);
#elif defined(F3DEX_GBI_2)
	int offset = getfield(hi, 16, 0);
#endif
	argu(m, 0, "seg", offset / 4, gfxd_Seg);
	argu(m, 1, "base", lo, gfxd_Segptr);
	int ret = 0;
	if (offset % 4 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	return ret;
}

UCFUNC int c_SPSetLightsN(gfxd_macro_t *m, int n_macro, int id, int numlights)
{
	if (n_macro < 2 + numlights)
		return -1;
	if (m[0].id != gfxd_SPNumLights || argvi(&m[0], 0) != numlights)
		return -1;
	int a = 1 + numlights;
	if (m[a].id != gfxd_SPLight || argvi(&m[a], 1) != a)
		return -1;
	uint32_t l = argvu(&m[a], 0);
	for (int i = 1; i <= numlights; i++)
	{
		int offset = sizeof(Ambient) + sizeof(Light) * (i - 1);
		if (m[i].id != gfxd_SPLight
			|| argvu(&m[i], 0) != l + offset
			|| argvi(&m[i], 1) != i)
		{
			return -1;
		}
	}
	m->id = id;
	argu(m, 0, "l", l, gfxd_Lightsn);
	return 0;
}

UCFUNC int c_SPSetLights1(gfxd_macro_t *m, int n_macro)
{
	return c_SPSetLightsN(m, n_macro, gfxd_SPSetLights1, NUMLIGHTS_1);
}

UCFUNC int c_SPSetLights2(gfxd_macro_t *m, int n_macro)
{
	return c_SPSetLightsN(m, n_macro, gfxd_SPSetLights2, NUMLIGHTS_2);
}

UCFUNC int c_SPSetLights3(gfxd_macro_t *m, int n_macro)
{
	return c_SPSetLightsN(m, n_macro, gfxd_SPSetLights3, NUMLIGHTS_3);
}

UCFUNC int c_SPSetLights4(gfxd_macro_t *m, int n_macro)
{
	return c_SPSetLightsN(m, n_macro, gfxd_SPSetLights4, NUMLIGHTS_4);
}

UCFUNC int c_SPSetLights5(gfxd_macro_t *m, int n_macro)
{
	return c_SPSetLightsN(m, n_macro, gfxd_SPSetLights5, NUMLIGHTS_5);
}

UCFUNC int c_SPSetLights6(gfxd_macro_t *m, int n_macro)
{
	return c_SPSetLightsN(m, n_macro, gfxd_SPSetLights6, NUMLIGHTS_6);
}

UCFUNC int c_SPSetLights7(gfxd_macro_t *m, int n_macro)
{
	return c_SPSetLightsN(m, n_macro, gfxd_SPSetLights7, NUMLIGHTS_7);
}

#if defined(F3D_GBI) || defined(F3DEX_GBI)
UCFUNC int d_SPNumLights(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPNumLights;
	argi(m, 0, "n", (lo - 0x80000000) / 32 - 1, gfxd_Numlights);
	int ret = 0;
	if (lo < 0x80000040 || lo % 32 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	return ret;
}
#elif defined(F3DEX_GBI_2)
UCFUNC int d_SPNumLights(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPNumLights;
	argi(m, 0, "n", lo / 24, gfxd_Numlights);
	int ret = 0;
	if (lo < 24 || lo % 24 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	return ret;
}
#endif

#if defined(F3D_GBI) || defined(F3DEX_GBI)
UCFUNC int d_SPLight(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	int n = (getfield(hi, 8, 16) - G_MV_L0) / 2 + 1;
	m->id = gfxd_SPLight;
	argu(m, 0, "l", lo, gfxd_Lightptr);
	argi(m, 1, "n", n, gfxd_Num);
	return 0;
}
#elif defined(F3DEX_GBI_2)
UCFUNC int d_SPLight(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	int n = (getfield(hi, 8, 8) * 8 / 24) - 1;
	m->id = gfxd_SPLight;
	argu(m, 0, "l", lo, gfxd_Lightptr);
	argi(m, 1, "n", n, gfxd_Num);
	return 0;
}
#endif

UCFUNC int c_SPLightColor(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 2)
		return -1;
	if (m[0].id != gfxd_MoveWd
		|| argvi(&m[0], 0) != G_MW_LIGHTCOL
		|| argvu(&m[0], 1) % 0x18 != 0
		|| argvu(&m[0], 1) > G_MWO_aLIGHT_8)
	{
		return -1;
	}
	uint32_t offset = argvu(&m[0], 1);
	uint32_t packedcolor = argvu(&m[0], 2);
	if (m[1].id != gfxd_MoveWd
		|| argvi(&m[1], 0) != G_MW_LIGHTCOL
		|| argvu(&m[1], 1) != offset + 4
		|| argvu(&m[1], 2) != packedcolor)
	{
		return -1;
	}
	m->id = gfxd_SPLightColor;
	argi(m, 0, "n", offset / 0x18 + 1, gfxd_Lightnum);
	argu(m, 1, "c", packedcolor, gfxd_Color);
	return 0;
}

UCFUNC int d_SPTexture(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPTexture;
	argu(m, 0, "sc", getfield(lo, 16, 16), gfxd_Tcscale);
	argu(m, 1, "tc", getfield(lo, 16, 0), gfxd_Tcscale);
	argi(m, 2, "level", getfield(hi, 3, 11), gfxd_Num);
	argi(m, 3, "tile", getfield(hi, 3, 8), gfxd_Tile);
#if defined(F3D_GBI) || defined(F3DEX_GBI)
	argi(m, 4, "on", getfield(hi, 8, 0), gfxd_Switch);
#elif defined(F3DEX_GBI_2)
	argi(m, 4, "on", getfield(hi, 7, 1), gfxd_Switch);
#endif
	return 0;
}

UCFUNC int c_SPTextureRectangle(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 3)
		return -1;
	if (m[0].id != gfxd_TexRect)
		return -1;
	qu102_t ulx = argvu(&m[0], 0);
	qu102_t uly = argvu(&m[0], 1);
	qu102_t lrx = argvu(&m[0], 2);
	qu102_t lry = argvu(&m[0], 3);
	int tile = argvi(&m[0], 4);
	if (m[1].id != gfxd_DPHalf1)
		return -1;
	qs105_t s = sx(getfield(argvu(&m[1], 0), 16, 16), 16);
	qs105_t t = sx(getfield(argvu(&m[1], 0), 16, 0), 16);
	if (m[2].id != gfxd_DPHalf2)
		return -1;
	qs510_t dsdx = sx(getfield(argvu(&m[2], 0), 16, 16), 16);
	qs510_t dtdy = sx(getfield(argvu(&m[2], 0), 16, 0), 16);
	m->id = gfxd_SPTextureRectangle;
	argu(m, 0, "ulx", ulx, gfxd_Coordq);
	argu(m, 1, "uly", uly, gfxd_Coordq);
	argu(m, 2, "lrx", lrx, gfxd_Coordq);
	argu(m, 3, "lry", lry, gfxd_Coordq);
	argi(m, 4, "tile", tile, gfxd_Tile);
	argi(m, 5, "s", s, gfxd_St);
	argi(m, 6, "t", t, gfxd_St);
	argi(m, 7, "dsdx", dsdx, gfxd_Stdelta);
	argi(m, 8, "dtdy", dtdy, gfxd_Stdelta);
	return 0;
}

UCFUNC int c_SPTextureRectangleFlip(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 3)
		return -1;
	if (m[0].id != gfxd_TexRectFlip)
		return -1;
	qu102_t ulx = argvu(&m[0], 0);
	qu102_t uly = argvu(&m[0], 1);
	qu102_t lrx = argvu(&m[0], 2);
	qu102_t lry = argvu(&m[0], 3);
	int tile = argvi(&m[0], 4);
	if (m[1].id != gfxd_DPHalf1)
		return -1;
	qs105_t s = sx(getfield(argvu(&m[1], 0), 16, 16), 16);
	qs105_t t = sx(getfield(argvu(&m[1], 0), 16, 0), 16);
	if (m[2].id != gfxd_DPHalf2)
		return -1;
	qs510_t dsdx = sx(getfield(argvu(&m[2], 0), 16, 16), 16);
	qs510_t dtdy = sx(getfield(argvu(&m[2], 0), 16, 0), 16);
	m->id = gfxd_SPTextureRectangleFlip;
	argu(m, 0, "ulx", ulx, gfxd_Coordq);
	argu(m, 1, "uly", uly, gfxd_Coordq);
	argu(m, 2, "lrx", lrx, gfxd_Coordq);
	argu(m, 3, "lry", lry, gfxd_Coordq);
	argi(m, 4, "tile", tile, gfxd_Tile);
	argi(m, 5, "s", s, gfxd_St);
	argi(m, 6, "t", t, gfxd_St);
	argi(m, 7, "dsdx", dsdx, gfxd_Stdelta);
	argi(m, 8, "dtdy", dtdy, gfxd_Stdelta);
	return 0;
}

#if defined(F3D_GBI)
UCFUNC int d_SPVertex(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPVertex;
	int n = getfield(hi, 4, 20) + 1;
	int v0 = getfield(hi, 4, 16);
	int size = getfield(hi, 16, 0);
	argu(m, 0, "v", lo, gfxd_Vtxptr);
	argi(m, 1, "n", n, gfxd_Num);
	argi(m, 2, "v0", v0, gfxd_Vtx);
	int ret = 0;
	if (size != sizeof(Vtx) * n)
	{
		badarg(m, 1);
		ret = -1;
	}
	return ret;
}
#elif defined(F3DEX_GBI)
UCFUNC int d_SPVertex(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPVertex;
	int n = getfield(hi, 6, 10);
	int v0 = getfield(hi, 8, 16);
	int size = getfield(hi, 10, 0);
	argu(m, 0, "v", lo, gfxd_Vtxptr);
	argi(m, 1, "n", n, gfxd_Num);
	argi(m, 2, "v0", v0 / 2, gfxd_Vtx);
	int ret = 0;
	if (size != sizeof(Vtx) * n - 1)
	{
		badarg(m, 1);
		ret = -1;
	}
	if (v0 % 2 != 0)
	{
		badarg(m, 2);
		ret = -1;
	}
	return ret;
}
#elif defined(F3DEX_GBI_2)
UCFUNC int d_SPVertex(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPVertex;
	int n = getfield(hi, 8, 12);
	int v0 = getfield(hi, 7, 1) - n;
	argu(m, 0, "v", lo, gfxd_Vtxptr);
	argi(m, 1, "n", n, gfxd_Num);
	argi(m, 2, "v0", v0, gfxd_Vtx);
	return 0;
}
#endif

UCFUNC int d_SPViewport(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPViewport;
	argu(m, 0, "v", lo, gfxd_Vpptr);
	return 0;
}

UCFUNC int d_DPLoadTLUTCmd(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPLoadTLUTCmd;
	argi(m, 0, "tile", getfield(lo, 3, 24), gfxd_Tile);
	argi(m, 1, "count", getfield(lo, 10, 14), gfxd_Num);
	return 0;
}

UCFUNC int c_DPLoadTLUT(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 6)
		return -1;
	if (m[0].id != gfxd_DPSetTextureImage
		|| argvi(&m[0], 0) != G_IM_FMT_RGBA
		|| argvi(&m[0], 1) != G_IM_SIZ_16b
		|| argvi(&m[0], 2) != 1)
	{
		return -1;
	}
	uint32_t dram = argvu(&m[0], 3);
	if (m[1].id != gfxd_DPTileSync)
		return -1;
	if (m[2].id != gfxd_DPSetTile
		|| argvi(&m[2], 0) != 0
		|| argvi(&m[2], 1) != 0
		|| argvi(&m[2], 2) != 0
		|| argvi(&m[2], 4) != G_TX_LOADTILE
		|| argvi(&m[2], 5) != 0
		|| argvu(&m[2], 6) != 0
		|| argvi(&m[2], 7) != 0
		|| argvi(&m[2], 8) != 0
		|| argvu(&m[2], 9) != 0
		|| argvi(&m[2], 10) != 0
		|| argvi(&m[2], 11) != 0)
	{
		return -1;
	}
	uint32_t tmem = argvu(&m[2], 3);
	if (m[3].id != gfxd_DPLoadSync)
		return -1;
	if (m[4].id != gfxd_DPLoadTLUTCmd || argvi(&m[4], 0) != G_TX_LOADTILE)
		return -1;
	int count = argvi(&m[4], 1) + 1;
	if (m[5].id != gfxd_DPPipeSync)
		return -1;
	m->id = gfxd_DPLoadTLUT;
	argi(m, 0, "count", count, gfxd_Num);
	argu(m, 1, "tmem", tmem, gfxd_Tmem);
	argu(m, 2, "dram", dram, gfxd_Tlut);
	return 0;
}

#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
UCFUNC int d_BranchZ(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_BranchZ;
	int na = getfield(hi, 12, 12);
	int nb = getfield(hi, 12, 0);
	int32_t zval;
	if (lo > 0x7FFFFFFF)
		zval = INT32_MIN + (int32_t)(lo & 0x7FFFFFFF);
	else
		zval = lo;
	argi(m, 0, "vtx", nb / 2, gfxd_Vtx);
	argi(m, 1, "zval", zval, gfxd_Zraw);
	int ret = 0;
	if (nb % 2 != 0 || na / 5 != nb / 2 || na % 5 != 0)
	{
		badarg(m, 0);
		ret = -1;
	}
	return ret;
}
#endif

UCFUNC int d_DisplayList(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	int flag = getfield(hi, 8, 16);
	if (flag == 0)
		return d_SPDisplayList(m, hi, lo);
	else if (flag == 1)
		return d_SPBranchList(m, hi, lo);
	else
	{
		m->id = gfxd_DisplayList;
		argu(m, 0, "dl", lo, gfxd_Dl);
		argi(m, 1, "flag", flag, gfxd_Dlflag);
		return 0;
	}
}

UCFUNC int d_DPHalf1(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPHalf1;
	argu(m, 0, "hi", lo, gfxd_Word);
	return 0;
}

UCFUNC int d_DPHalf2(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPHalf2;
	argu(m, 0, "lo", lo, gfxd_Word);
	return 0;
}

UCFUNC int c_DPWord(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 2)
		return -1;
	if (m[0].id != gfxd_DPHalf1 || m[1].id != gfxd_DPHalf2)
		return -1;
	uint32_t wordhi = argvu(&m[0], 0);
	uint32_t wordlo = argvu(&m[1], 0);
	m->id = gfxd_DPWord;
	argu(m, 0, "wordhi", wordhi, gfxd_Word);
	argu(m, 1, "wordlo", wordlo, gfxd_Word);
	return 0;
}

UCFUNC int d_DPLoadTile(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPLoadTile;
	argi(m, 0, "tile", getfield(lo, 3, 24), gfxd_Tile);
	argu(m, 1, "uls", getfield(hi, 12, 12), gfxd_Coordq);
	argu(m, 2, "ult", getfield(hi, 12, 0), gfxd_Coordq);
	argu(m, 3, "lrs", getfield(lo, 12, 12), gfxd_Coordq);
	argu(m, 4, "lrt", getfield(lo, 12, 0), gfxd_Coordq);
	return 0;
}

#if defined(F3DEX_GBI_2)
UCFUNC int d_SPGeometryMode(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	uint32_t clearbits = getfield(~hi, 24, 0);
	uint32_t setbits = lo;
	if (clearbits == 0 && setbits != 0)
		return d_SPSetGeometryMode(m, hi, lo);
	else if (clearbits != 0 && setbits == 0)
		return d_SPClearGeometryMode(m, hi, lo);
	else if (clearbits == 0x00FFFFFF)
		return d_SPLoadGeometryMode(m, hi, lo);
	else
	{
		m->id = gfxd_SPGeometryMode;
		argu(m, 0, "c", clearbits, gfxd_Gm);
		argu(m, 1, "s", setbits, gfxd_Gm);
		return 0;
	}
}
#endif

UCFUNC int d_SPSetOtherMode(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPSetOtherMode;
	int opc = getfield(hi, 8, 24);
#if defined(F3D_GBI) || defined(F3DEX_GBI)
	int length = getfield(hi, 8, 0);
	int shift = getfield(hi, 8, 8);
#elif defined(F3DEX_GBI_2)
	int length = getfield(hi, 8, 0) + 1;
	int shift = 32 - (getfield(hi, 8, 8) + length);
#endif
	argi(m, 0, "opc", opc, gfxd_Opcode);
	argi(m, 1, "sft", shift, gfxd_Sftlo);
	argi(m, 2, "len", length, gfxd_Num);
	if (opc == G_SETOTHERMODE_H)
		argu(m, 3, "mode", lo, gfxd_Othermodehi);
	else if (opc == G_SETOTHERMODE_L)
		argu(m, 3, "mode", lo, gfxd_Othermodelo);
	else
		argu(m, 3, "mode", lo, gfxd_Word);
	return 0;
}

UCFUNC int d_SPSetOtherModeLo(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
#if defined(F3D_GBI) || defined(F3DEX_GBI)
	int length = getfield(hi, 8, 0);
	int shift = getfield(hi, 8, 8);
#elif defined(F3DEX_GBI_2)
	int length = getfield(hi, 8, 0) + 1;
	int shift = 32 - (getfield(hi, 8, 8) + length);
#endif
	if (shift == G_MDSFT_ALPHACOMPARE && length == G_MDSIZ_ALPHACOMPARE)
		return d_DPSetAlphaCompare(m, hi, lo);
	else if (shift == G_MDSFT_ZSRCSEL && length == G_MDSIZ_ZSRCSEL)
		return d_DPSetDepthSource(m, hi, lo);
	else if (shift == G_MDSFT_RENDERMODE && length == G_MDSIZ_RENDERMODE)
		return d_DPSetRenderMode(m, hi, lo);
	else if (config.emit_ext_macro)
	{
		m->id = gfxd_SPSetOtherModeLo;
		argi(m, 0, "sft", shift, gfxd_Sftlo);
		argi(m, 1, "len", length, gfxd_Num);
		argu(m, 2, "mode", lo, gfxd_Othermodelo);
		return 0;
	}
	else
		return d_SPSetOtherMode(m, hi, lo);
}

UCFUNC int d_SPSetOtherModeHi(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
#if defined(F3D_GBI) || defined(F3DEX_GBI)
	int length = getfield(hi, 8, 0);
	int shift = getfield(hi, 8, 8);
#elif defined(F3DEX_GBI_2)
	int length = getfield(hi, 8, 0) + 1;
	int shift = 32 - (getfield(hi, 8, 8) + length);
#endif
	if (shift == G_MDSFT_ALPHADITHER && length == G_MDSIZ_ALPHADITHER)
		return d_DPSetAlphaDither(m, hi, lo);
	else if (shift == G_MDSFT_RGBDITHER && length == G_MDSIZ_RGBDITHER)
		return d_DPSetColorDither(m, hi, lo);
	else if (shift == G_MDSFT_COMBKEY && length == G_MDSIZ_COMBKEY)
		return d_DPSetCombineKey(m, hi, lo);
	else if (shift == G_MDSFT_TEXTCONV && length == G_MDSIZ_TEXTCONV)
		return d_DPSetTextureConvert(m, hi, lo);
	else if (shift == G_MDSFT_TEXTFILT && length == G_MDSIZ_TEXTFILT)
		return d_DPSetTextureFilter(m, hi, lo);
	else if (shift == G_MDSFT_TEXTLUT && length == G_MDSIZ_TEXTLUT)
		return d_DPSetTextureLUT(m, hi, lo);
	else if (shift == G_MDSFT_TEXTLOD && length == G_MDSIZ_TEXTLOD)
		return d_DPSetTextureLOD(m, hi, lo);
	else if (shift == G_MDSFT_TEXTDETAIL && length == G_MDSIZ_TEXTDETAIL)
		return d_DPSetTextureDetail(m, hi, lo);
	else if (shift == G_MDSFT_TEXTPERSP && length == G_MDSIZ_TEXTPERSP)
		return d_DPSetTexturePersp(m, hi, lo);
	else if (shift == G_MDSFT_CYCLETYPE && length == G_MDSIZ_CYCLETYPE)
		return d_DPSetCycleType(m, hi, lo);
	else if (shift == G_MDSFT_PIPELINE && length == G_MDSIZ_PIPELINE)
		return d_DPPipelineMode(m, hi, lo);
	else if (config.emit_ext_macro)
	{
		m->id = gfxd_SPSetOtherModeHi;
		argi(m, 0, "sft", shift, gfxd_Sfthi);
		argi(m, 1, "len", length, gfxd_Num);
		argu(m, 2, "mode", lo, gfxd_Othermodehi);
		return 0;
	}
	else
		return d_SPSetOtherMode(m, hi, lo);
}

UCFUNC int d_DPSetOtherMode(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_DPSetOtherMode;
	argu(m, 0, "hi", getfield(hi, 24, 0), gfxd_Othermodehi);
	argu(m, 1, "lo", lo, gfxd_Othermodelo);
	return 0;
}

UCFUNC int d_MoveWd(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
#if defined(F3D_GBI) || defined(F3DEX_GBI)
	int index = getfield(hi, 8, 0);
	int offset = getfield(hi, 16, 8);
#elif defined(F3DEX_GBI_2)
	int index = getfield(hi, 8, 16);
	int offset = getfield(hi, 16, 0);
#endif
	if (index == G_MW_FOG && offset == G_MWO_FOG)
		return d_SPFogPosition(m, hi, lo);
#if !(defined(F3D_BETA) && (defined(F3D_GBI) || defined(F3DEX_GBI)))
	else if (index == G_MW_PERSPNORM && offset == 0)
		return d_SPPerspNormalize(m, hi, lo);
#endif
	else if (index == G_MW_SEGMENT)
		return d_SPSegment(m, hi, lo);
	else if (index == G_MW_NUMLIGHT && offset == G_MWO_NUMLIGHT)
		return d_SPNumLights(m, hi, lo);
#if defined(F3D_GBI) || (defined(F3D_BETA) && defined(F3DEX_GBI))
	else if (index == G_MW_POINTS)
		return d_SPModifyVertex(m, hi, lo);
#endif
#if defined(F3D_GBI) || defined(F3DEX_GBI)
	else if (index == G_MW_MATRIX)
		return d_SPInsertMatrix(m, hi, lo);
#endif
	else
	{
		m->id = gfxd_MoveWd;
		argi(m, 0, "index", index, gfxd_Mw);
		if (index == G_MW_MATRIX)
			argu(m, 1, "offset", offset, gfxd_Mwo_matrix);
		else if (index == G_MW_CLIP)
			argu(m, 1, "offset", offset, gfxd_Mwo_clip);
		else if (index == G_MW_LIGHTCOL)
			argu(m, 1, "offset", offset, gfxd_Mwo_lightcol);
		else
			argu(m, 1, "offset", offset, gfxd_Mwo);
		argu(m, 2, "value", lo, gfxd_Word);
	}
	return 0;
}

#if defined(F3D_GBI) || defined(F3DEX_GBI)
UCFUNC int d_MoveMem(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	int size = getfield(hi, 16, 0);
	int index = getfield(hi, 8, 16);
	if (size == sizeof(Light)
		&& index >= G_MV_L0
		&& index <= G_MV_L7
		&& index % 2 == 0)
	{
		return d_SPLight(m, hi, lo);
	}
	else if (size == sizeof(Light) && index == G_MV_LOOKATX)
		return d_SPLookAtX(m, hi, lo);
	else if (size == sizeof(Light) && index == G_MV_LOOKATY)
		return d_SPLookAtY(m, hi, lo);
	else if (size == sizeof(Vp) && index == G_MV_VIEWPORT)
		return d_SPViewport(m, hi, lo);
	else
	{
		m->id = gfxd_MoveMem;
		argu(m, 0, "size", size, gfxd_Size);
		argi(m, 1, "index", index, gfxd_Mv);
		argu(m, 2, "dram", lo, gfxd_Dram);
		return 0;
	}
}
#elif defined(F3DEX_GBI_2)
UCFUNC int d_MoveMem(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	int size = (getfield(hi, 5, 19) + 1) * 8;
	int index = getfield(hi, 8, 0);
	int offset = getfield(hi, 8, 8) * 8;
	if (size == sizeof(Light)
		&& index == G_MV_LIGHT
		&& offset >= G_MVO_L0
		&& offset <= G_MVO_L7
		&& offset % 0x18 == 0)
	{
		return d_SPLight(m, hi, lo);
	}
	else if (size == sizeof(Light)
			&& index == G_MV_LIGHT
			&& offset == G_MVO_LOOKATX)
	{
		return d_SPLookAtX(m, hi, lo);
	}
	else if (size == sizeof(Light)
			&& index == G_MV_LIGHT
			&& offset == G_MVO_LOOKATY)
	{
		return d_SPLookAtY(m, hi, lo);
	}
	else if (size == sizeof(Vp)
			&& index == G_MV_VIEWPORT
			&& offset == 0)
	{
		return d_SPViewport(m, hi, lo);
	}
	else
	{
		m->id = gfxd_MoveMem;
		argu(m, 0, "size", size, gfxd_Size);
		argi(m, 1, "index", index, gfxd_Mv);
		argu(m, 2, "offset", offset, gfxd_Size);
		argu(m, 3, "dram", lo, gfxd_Dram);
		return 0;
	}
}
#endif

#if defined(F3DEX_GBI_2)
UCFUNC int d_SPDmaRead(gfxd_macro_t *m, uint32_t hi, uint32_t lo);
UCFUNC int d_SPDmaWrite(gfxd_macro_t *m, uint32_t hi, uint32_t lo);
UCFUNC int d_SPDma_io(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	int flag = getfield(hi, 1, 23);
	if (flag == 0)
		return d_SPDmaRead(m, hi, lo);
	else if (flag == 1)
		return d_SPDmaWrite(m, hi, lo);
	else
	{
		m->id = gfxd_SPDma_io;
		argi(m, 0, "flag", flag, gfxd_Dmaflag);
		argu(m, 1, "dmem", getfield(hi, 10, 13) * 8, gfxd_Dmem);
		argu(m, 2, "dram", lo, gfxd_Dram);
		argu(m, 3, "size", getfield(hi, 12, 10) + 1, gfxd_Size);
		return 0;
	}
}

UCFUNC int d_SPDmaRead(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPDmaRead;
	argu(m, 0, "dmem", getfield(hi, 10, 13) * 8, gfxd_Dmem);
	argu(m, 1, "dram", lo, gfxd_Dram);
	argu(m, 2, "size", getfield(hi, 12, 10) + 1, gfxd_Size);
	return 0;
}

UCFUNC int d_SPDmaWrite(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPDmaWrite;
	argu(m, 0, "dmem", getfield(hi, 10, 13) * 8, gfxd_Dmem);
	argu(m, 1, "dram", lo, gfxd_Dram);
	argu(m, 2, "size", getfield(hi, 12, 10) + 1, gfxd_Size);
	return 0;
}
#endif

#if defined(F3DEX_GBI) || defined(F3DEX_GBI_2)
UCFUNC int d_LoadUcode(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_LoadUcode;
	argu(m, 0, "uc_start", lo, gfxd_Uctext);
	argu(m, 1, "uc_dsize", getfield(hi, 16, 0) + 1, gfxd_Size);
	return 0;
}

UCFUNC int c_SPLoadUcodeEx(gfxd_macro_t *m, int n_macro)
{
	if (n_macro < 2)
		return -1;
	if (m[0].id != gfxd_DPHalf1)
		return -1;
	uint32_t uc_dstart = argvu(&m[0], 0);
	if (m[1].id != gfxd_LoadUcode)
		return -1;
	uint32_t uc_start = argvu(&m[1], 0);
	uint32_t uc_dsize = argvu(&m[1], 1);
	m->id = gfxd_SPLoadUcodeEx;
	argu(m, 0, "uc_start", uc_start, gfxd_Uctext);
	argu(m, 1, "uc_dstart", uc_dstart, gfxd_Ucdata);
	argu(m, 2, "uc_dsize", uc_dsize, gfxd_Size);
	return 0;
}
#endif

UCFUNC int d_TexRect(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_TexRect;
	argu(m, 0, "ulx", getfield(lo, 12, 12), gfxd_Coordq);
	argu(m, 1, "uly", getfield(lo, 12, 0), gfxd_Coordq);
	argu(m, 2, "lrx", getfield(hi, 12, 12), gfxd_Coordq);
	argu(m, 3, "lry", getfield(hi, 12, 0), gfxd_Coordq);
	argi(m, 4, "tile", getfield(lo, 3, 24), gfxd_Tile);
	return 0;
}

UCFUNC int d_TexRectFlip(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_TexRectFlip;
	argu(m, 0, "ulx", getfield(lo, 12, 12), gfxd_Coordq);
	argu(m, 1, "uly", getfield(lo, 12, 0), gfxd_Coordq);
	argu(m, 2, "lrx", getfield(hi, 12, 12), gfxd_Coordq);
	argu(m, 3, "lry", getfield(hi, 12, 0), gfxd_Coordq);
	argi(m, 4, "tile", getfield(lo, 3, 24), gfxd_Tile);
	return 0;
}

UCFUNC int d_SPNoOp(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_SPNoOp;
	return 0;
}

#if defined(F3DEX_GBI_2)
UCFUNC int d_Special3(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_Special3;
	argu(m, 0, "hi", getfield(hi, 24, 0), gfxd_Word);
	argu(m, 1, "lo", lo, gfxd_Word);
	return 0;
}

UCFUNC int d_Special2(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_Special2;
	argu(m, 0, "hi", getfield(hi, 24, 0), gfxd_Word);
	argu(m, 1, "lo", lo, gfxd_Word);
	return 0;
}

UCFUNC int d_Special1(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	m->id = gfxd_Special1;
	argu(m, 0, "hi", getfield(hi, 24, 0), gfxd_Word);
	argu(m, 1, "lo", lo, gfxd_Word);
	return 0;
}
#endif
