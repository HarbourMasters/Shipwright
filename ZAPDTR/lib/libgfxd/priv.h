#ifndef GFXD_PRIV_H
#define GFXD_PRIV_H
#include "gfxd.h"

#ifdef CONFIG_MT
# ifdef _MSC_VER
#  define TLOCAL __declspec(thread)
# else
#  define TLOCAL _Thread_local
# endif
#else
# define TLOCAL
#endif

#define UCFUNC static inline

#define config gfxd_config__

typedef int gfxd_argfn_t(const gfxd_value_t *v);

typedef struct
{
	int		fmt;
	gfxd_argfn_t *	fn;
} gfxd_arg_type_t;

typedef struct
{
	int		type;
	const char *	name;
	gfxd_value_t	value;
	int		bad;
} gfxd_arg_t;

typedef struct
{
	int		id;
	gfxd_arg_t	arg[18];
} gfxd_macro_t;

typedef int gfxd_disas_fn_t(gfxd_macro_t *macro, uint32_t hi, uint32_t lo);
typedef int gfxd_combine_fn_t(gfxd_macro_t *macro, int n_macro);

typedef struct
{
	const char *		prefix;
	const char *		suffix;
	int			opcode;
	int			n_arg;
	int			n_gfx;
	gfxd_disas_fn_t *	disas_fn;
	gfxd_combine_fn_t *	combine_fn;
	int			alias;
	int			ext;
} gfxd_macro_type_t;

struct gfxd_ucode
{
	gfxd_disas_fn_t *		disas_fn;
	gfxd_combine_fn_t *		combine_fn;
	const gfxd_arg_type_t *		arg_tbl;
	const gfxd_macro_type_t *	macro_tbl;
};

struct gfxd_state
{
	int			macro_offset;

	Gfx			gfx[9];
	int			n_byte;
	int			n_gfx;
	gfxd_macro_t		macro[9];

	int			end_input;
	int			ret;
};

struct gfxd_config
{
	gfxd_ucode_t		ucode;
	int			endian;
	int			wordsize;
	const char *		arg;
	void *			udata;

	int			stop_on_invalid;
	int			stop_on_end;
	int			emit_dec_color;
	int			emit_q_macro;
	int			emit_ext_macro;

	const char *		input_buf;
	int			input_buf_size;
	int			input_fd;
	gfxd_input_fn_t *	input_fn;

	char *			output_buf;
	int			output_buf_size;
	int			output_fd;
	gfxd_output_fn_t *	output_fn;

	gfxd_macro_fn_t *	macro_fn;
	gfxd_arg_fn_t *		arg_fn;

	gfxd_tlut_fn_t *	tlut_fn;
	gfxd_timg_fn_t *	timg_fn;
	gfxd_cimg_fn_t *	cimg_fn;
	gfxd_zimg_fn_t *	zimg_fn;
	gfxd_dl_fn_t *		dl_fn;
	gfxd_mtx_fn_t *		mtx_fn;
	gfxd_lookat_fn_t *	lookat_fn;
	gfxd_light_fn_t *	light_fn;
	gfxd_seg_fn_t *		seg_fn;
	gfxd_vtx_fn_t *		vtx_fn;
	gfxd_vp_fn_t *		vp_fn;
	gfxd_uctext_fn_t *	uctext_fn;
	gfxd_ucdata_fn_t *	ucdata_fn;
	gfxd_dram_fn_t *	dram_fn;
};

extern TLOCAL struct gfxd_config gfxd_config__;

#endif
