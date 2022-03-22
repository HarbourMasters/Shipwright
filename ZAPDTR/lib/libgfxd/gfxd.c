#include <inttypes.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
# include <io.h>
# define read _read
# define write _write
#else
# include <unistd.h>
#endif
#include "gbi.h"
#include "gfxd.h"
#include "priv.h"

static TLOCAL struct gfxd_state state;

static int buffer_input_fn(void *buf, int count)
{
	if (count > config.input_buf_size)
		count = config.input_buf_size;
	memcpy(buf, config.input_buf, count);
	config.input_buf += count;
	config.input_buf_size -= count;
	return count;
}

static int buffer_output_fn(const char *buf, int count)
{
	if (count > config.output_buf_size)
		count = config.output_buf_size;
	memcpy(config.output_buf, buf, count);
	config.output_buf += count;
	config.output_buf_size -= count;
	return count;
}

static int fd_input_fn(void *buf, int count)
{
	return read(config.input_fd, buf, count);
}

static int fd_output_fn(const char *buf, int count)
{
	return write(config.output_fd, buf, count);
}

static void swap_words(Gfx *gfx)
{
	uint8_t b[8];
	uint8_t *pw = (void *) gfx;
	uint8_t *pb = b;

	int endian = config.endian;
	int wordsize = config.wordsize;

	for (int i = 0; i < 8 / wordsize; i++)
	{
		if (endian == gfxd_endian_host)
		{
			switch (wordsize)
			{
				case 1:
				{
					uint8_t w = *(uint8_t *) pw;
					*pb++ = w >> 0;
					break;
				}
				case 2:
				{
					uint16_t w = *(uint16_t *) pw;
					*pb++ = w >> 8;
					*pb++ = w >> 0;
					break;
				}
				case 4:
				{
					uint32_t w = *(uint32_t *) pw;
					*pb++ = w >> 24;
					*pb++ = w >> 16;
					*pb++ = w >> 8;
					*pb++ = w >> 0;
					break;
				}
				case 8:
				{
					uint64_t w = *(uint64_t *) pw;
					*pb++ = w >> 56;
					*pb++ = w >> 48;
					*pb++ = w >> 40;
					*pb++ = w >> 32;
					*pb++ = w >> 24;
					*pb++ = w >> 16;
					*pb++ = w >> 8;
					*pb++ = w >> 0;
					break;
				}
			}
		}
		else
		{
			for (int j = 0; j < wordsize; j++)
			{
				if (endian == gfxd_endian_little)
					*pb++ = pw[wordsize - 1 - j];
				else
					*pb++ = pw[j];
			}
		}
		pw += wordsize;
	}

	gfx->hi = ((uint32_t) b[0] << 24)
		| ((uint32_t) b[1] << 16)
		| ((uint32_t) b[2] << 8)
		| ((uint32_t) b[3] << 0);
	gfx->lo = ((uint32_t) b[4] << 24)
		| ((uint32_t) b[5] << 16)
		| ((uint32_t) b[6] << 8)
		| ((uint32_t) b[7] << 0);
}

static void get_more_input(void)
{
	if (state.end_input != 0)
		return;

	char *recv_buf = (void *) &state.gfx[0];

	while (state.n_gfx < sizeof(state.gfx) / sizeof(state.gfx[0]))
	{
		int n_read = sizeof(state.gfx) - state.n_byte;
		n_read = config.input_fn(&recv_buf[state.n_byte], n_read);
		if (n_read == 0)
			return;
		state.n_byte += n_read;

		while (state.n_gfx < state.n_byte / sizeof(Gfx))
		{
			Gfx gfx = state.gfx[state.n_gfx];
			gfxd_macro_t *m = &state.macro[state.n_gfx];

			swap_words(&gfx);

			int ret = config.ucode->disas_fn(m, gfx.hi, gfx.lo);
			if (ret != 0 && config.stop_on_invalid != 0)
			{
				state.end_input = 1;
				state.ret = ret;
				return;
			}

			state.n_gfx++;
		}
	}
}

static int32_t typed_arg_i(int type, int idx)
{
	const gfxd_value_t *v = gfxd_value_by_type(type, idx);
	if (v != NULL)
		return v->i;
	else
		return -1;
}

static uint32_t typed_arg_u(int type, int idx)
{
	const gfxd_value_t *v = gfxd_value_by_type(type, idx);
	if (v != NULL)
		return v->u;
	else
		return 0;
}


TLOCAL struct gfxd_config config =
{
	.ucode = NULL,
	.endian = gfxd_endian_big,
	.wordsize = 4,
	.arg = NULL,

	.stop_on_invalid = 1,
	.stop_on_end = 1,
	.emit_dec_color = 0,
	.emit_q_macro = 0,
	.emit_ext_macro = 0,

	.input_buf = NULL,
	.input_buf_size = 0,
	.input_fn = &buffer_input_fn,

	.output_buf = NULL,
	.output_buf_size = 0,
	.output_fn = &buffer_output_fn,

	.macro_fn = &gfxd_macro_dflt,
	.arg_fn = &gfxd_arg_dflt,

	.tlut_fn = NULL,
	.timg_fn = NULL,
	.cimg_fn = NULL,
	.zimg_fn = NULL,
	.dl_fn = NULL,
	.mtx_fn = NULL,
	.lookat_fn = NULL,
	.light_fn = NULL,
	.seg_fn = NULL,
	.vtx_fn = NULL,
	.vp_fn = NULL,
	.uctext_fn = NULL,
	.ucdata_fn = NULL,
	.dram_fn = NULL,
};

void gfxd_input_buffer(const void *buf, int size)
{
	config.input_buf = buf;
	config.input_buf_size = size;
	config.input_fn = &buffer_input_fn;
}

void gfxd_output_buffer(char *buf, int size)
{
	config.output_buf = buf;
	config.output_buf_size = size;
	config.output_fn = &buffer_output_fn;
}

void gfxd_input_fd(int fd)
{
	config.input_fd = fd;
	config.input_fn = &fd_input_fn;
}

void gfxd_output_fd(int fd)
{
	config.output_fd = fd;
	config.output_fn = &fd_output_fn;
}

void gfxd_input_callback(gfxd_input_fn_t *fn)
{
	if (fn != NULL)
		config.input_fn = fn;
	else
		gfxd_input_buffer(NULL, 0);
}

void gfxd_output_callback(gfxd_output_fn_t *fn)
{
	if (fn != NULL)
		config.output_fn = fn;
	else
		gfxd_output_buffer(NULL, 0);
}

void gfxd_macro_fn(gfxd_macro_fn_t *fn)
{
	if (fn != NULL)
		config.macro_fn = fn;
	else
		config.macro_fn = gfxd_macro_dflt;
}

void gfxd_arg_fn(gfxd_arg_fn_t *fn)
{
	if (fn != NULL)
		config.arg_fn = fn;
	else
		config.arg_fn = gfxd_arg_dflt;
}

int gfxd_write(const void *buf, int count)
{
	return config.output_fn(buf, count);
}

int gfxd_puts(const char *str)
{
	return gfxd_write(str, strlen(str));
}

int gfxd_printf(const char *fmt, ...)
{
	char s[256];

	va_list arg;
	va_start(arg, fmt);
	int n = vsnprintf(s, sizeof(s), fmt, arg);
	va_end(arg);

	if (n > sizeof(s) - 1)
		n = sizeof(s) - 1;

	return gfxd_write(s, n);
}

int gfxd_print_value(int type, const gfxd_value_t *value)
{
	return config.ucode->arg_tbl[type].fn(value);
}

int gfxd_macro_dflt(void)
{
	gfxd_macro_t *m = &state.macro[0];
	const gfxd_macro_type_t *t = &config.ucode->macro_tbl[m->id];

	const char *name = gfxd_macro_name();
	if (name == NULL)
	{
		if (config.arg != NULL)
		{
			gfxd_puts(config.arg);
			gfxd_puts(" = ");
		}

		gfxd_puts("(Gfx){");
	}
	else
	{
		gfxd_puts(name);
		gfxd_puts("(");

		if (config.arg != NULL)
		{
			gfxd_puts(config.arg);
			if (t->n_arg != 0)
				gfxd_puts(", ");
		}
	}

	for (int i = 0; i < t->n_arg; i++)
	{
		if (i != 0)
			gfxd_puts(", ");

		config.arg_fn(i);
	}

	if (name == NULL)
		gfxd_puts("}");
	else
		gfxd_puts(")");

	return 0;
}

int gfxd_arg_callbacks(int arg_num)
{
	int id = gfxd_macro_id();

	switch (gfxd_arg_type(arg_num))
	{
		case gfxd_Tlut:
		{
			if (config.tlut_fn != NULL)
			{
				int32_t num;
				if (id == gfxd_DPLoadTLUT_pal16)
					num = 16;
				else if (id == gfxd_DPLoadTLUT_pal256)
					num = 256;
				else
					num = typed_arg_i(gfxd_Num, 0);
				return config.tlut_fn(
					typed_arg_u(gfxd_Tlut, 0),
					typed_arg_i(gfxd_Pal, 0),
					num);
			}
			break;
		}
		case gfxd_Timg:
		{
			if (config.timg_fn != NULL)
			{
				int32_t siz = typed_arg_i(gfxd_Siz, 0);
				if (siz == -1)
					siz = G_IM_SIZ_4b;
				return config.timg_fn(
					typed_arg_u(gfxd_Timg, 0),
					typed_arg_i(gfxd_Fmt, 0),
					siz,
					typed_arg_i(gfxd_Dim, 0),
					typed_arg_i(gfxd_Dim, 1),
					typed_arg_i(gfxd_Pal, 0));
			}
			break;
		}
		case gfxd_Cimg:
		{
			if (config.cimg_fn != NULL)
			{
				return config.cimg_fn(
					typed_arg_u(gfxd_Cimg, 0),
					typed_arg_i(gfxd_Fmt, 0),
					typed_arg_i(gfxd_Siz, 0),
					typed_arg_i(gfxd_Dim, 0));
			}
			break;
		}
		case gfxd_Zimg:
		{
			if (config.zimg_fn != NULL)
			{
				return config.zimg_fn(
					typed_arg_u(gfxd_Zimg, 0));
			}
			break;
		}
		case gfxd_Dl:
		{
			if (config.dl_fn != NULL)
			{
				return config.dl_fn(
					typed_arg_u(gfxd_Dl, 0));
			}
			break;
		}
		case gfxd_Mtxptr:
		{
			if (config.mtx_fn != NULL)
			{
				return config.mtx_fn(
					typed_arg_u(gfxd_Mtxptr, 0));
			}
			break;
		}
		case gfxd_Lookatptr:
		{
			if (config.lookat_fn != NULL)
			{
				int32_t num;
				if (id == gfxd_SPLookAt)
					num = 2;
				else
					num = 1;
				return config.lookat_fn(
					typed_arg_u(gfxd_Lookatptr, 0),
					num);
			}
			break;
		}
		case gfxd_Lightptr:
		{
			if (config.light_fn != NULL)
			{
				int32_t num;
				if (id == gfxd_SPSetLights1)
					num = 1;
				else if (id == gfxd_SPSetLights2)
					num = 2;
				else if (id == gfxd_SPSetLights3)
					num = 3;
				else if (id == gfxd_SPSetLights4)
					num = 4;
				else if (id == gfxd_SPSetLights5)
					num = 5;
				else if (id == gfxd_SPSetLights6)
					num = 6;
				else if (id == gfxd_SPSetLights7)
					num = 7;
				else
					num = 1;
				return config.light_fn(
					typed_arg_u(gfxd_Lightptr, 0),
					num);
			}
			break;
			
		}
		case gfxd_Segptr:
		{
			if (config.seg_fn != NULL)
			{
				return config.seg_fn(
					typed_arg_u(gfxd_Segptr, 0),
					typed_arg_i(gfxd_Seg, 0));
			}
			break;
		}
		case gfxd_Vtxptr:
		{
			if (config.vtx_fn != NULL)
			{
				return config.vtx_fn(
					typed_arg_u(gfxd_Vtxptr, 0),
					typed_arg_i(gfxd_Num, 0));
			}
			break;
		}
		case gfxd_Vpptr:
		{
			if (config.vp_fn != NULL)
			{
				return config.vp_fn(
					typed_arg_u(gfxd_Vpptr, 0));
			}
			break;
		}
		case gfxd_Uctext:
		{
			if (config.uctext_fn != NULL)
			{
				return config.uctext_fn(
					typed_arg_u(gfxd_Uctext, 0),
					0x1000);
			}
			break;
		}
		case gfxd_Ucdata:
		{
			if (config.ucdata_fn != NULL)
			{
				uint32_t size;
				if (id == gfxd_SPLoadUcodeEx)
					size = typed_arg_u(gfxd_Size, 0);
				else
					size = 0x800;
				return config.ucdata_fn(
					typed_arg_u(gfxd_Ucdata, 0),
					size);
			}
			break;
		}
		case gfxd_Dram:
		{
			if (config.dram_fn != NULL)
			{
				return config.dram_fn(
					typed_arg_u(gfxd_Dram, 0),
					typed_arg_u(gfxd_Size, 0));
			}
			break;
		}
	}

	return 0;
}

void gfxd_arg_dflt(int arg_num)
{
	if (gfxd_arg_callbacks(arg_num) == 0)
	{
		gfxd_arg_t *a = &state.macro[0].arg[arg_num];

		gfxd_print_value(a->type, &a->value);
	}
}

void gfxd_tlut_callback(gfxd_tlut_fn_t *fn)
{
	config.tlut_fn = fn;
}

void gfxd_timg_callback(gfxd_timg_fn_t *fn)
{
	config.timg_fn = fn;
}

void gfxd_cimg_callback(gfxd_cimg_fn_t *fn)
{
	config.cimg_fn = fn;
}

void gfxd_zimg_callback(gfxd_zimg_fn_t *fn)
{
	config.zimg_fn = fn;
}

void gfxd_dl_callback(gfxd_dl_fn_t *fn)
{
	config.dl_fn = fn;
}

void gfxd_mtx_callback(gfxd_mtx_fn_t *fn)
{
	config.mtx_fn = fn;
}

void gfxd_lookat_callback(gfxd_lookat_fn_t *fn)
{
	config.lookat_fn = fn;
}

void gfxd_light_callback(gfxd_light_fn_t *fn)
{
	config.light_fn = fn;
}

void gfxd_seg_callback(gfxd_seg_fn_t *fn)
{
	config.seg_fn = fn;
}

void gfxd_vtx_callback(gfxd_vtx_fn_t *fn)
{
	config.vtx_fn = fn;
}

void gfxd_vp_callback(gfxd_vp_fn_t *fn)
{
	config.vp_fn = fn;
}

void gfxd_uctext_callback(gfxd_uctext_fn_t *fn)
{
	config.uctext_fn = fn;
}

void gfxd_ucdata_callback(gfxd_ucdata_fn_t *fn)
{
	config.ucdata_fn = fn;
}

void gfxd_dram_callback(gfxd_dram_fn_t *fn)
{
	config.dram_fn = fn;
}

void gfxd_target(gfxd_ucode_t ucode)
{
	config.ucode = ucode;
}

void gfxd_endian(int endian, int wordsize)
{
	config.endian = endian;
	config.wordsize = wordsize;
}

void gfxd_dynamic(const char *arg)
{
	config.arg = arg;
}

void gfxd_enable(int cap)
{
	switch (cap)
	{
		case gfxd_stop_on_invalid:
			config.stop_on_invalid = 1;
			break;

		case gfxd_stop_on_end:
			config.stop_on_end = 1;
			break;

		case gfxd_emit_dec_color:
			config.emit_dec_color = 1;
			break;

		case gfxd_emit_q_macro:
			config.emit_q_macro = 1;
			break;

		case gfxd_emit_ext_macro:
			config.emit_ext_macro = 1;
			break;
	}
}

void gfxd_disable(int cap)
{
	switch (cap)
	{
		case gfxd_stop_on_invalid:
			config.stop_on_invalid = 0;
			return;

		case gfxd_stop_on_end:
			config.stop_on_end = 0;
			return;

		case gfxd_emit_dec_color:
			config.emit_dec_color = 0;
			break;

		case gfxd_emit_q_macro:
			config.emit_q_macro = 0;
			break;

		case gfxd_emit_ext_macro:
			config.emit_ext_macro = 0;
			break;
	}
}

void gfxd_udata_set(void *ptr)
{
	config.udata = ptr;
}

void *gfxd_udata_get(void)
{
	return config.udata;
}

int gfxd_execute(void)
{
	state.macro_offset = 0;
	state.n_byte = 0;
	state.n_gfx = 0;
	state.end_input = 0;
	state.ret = 0;

	for (;;)
	{
		get_more_input();
		if (state.n_gfx == 0)
			break;

		gfxd_macro_t *m = &state.macro[0];
		config.ucode->combine_fn(m, state.n_gfx);

		const gfxd_macro_type_t *t = &config.ucode->macro_tbl[m->id];
		if (t->ext != 0 && config.emit_ext_macro == 0)
		{
			Gfx gfx = state.gfx[0];
			swap_words(&gfx);

			t = &config.ucode->macro_tbl[gfxd_Invalid];
			t->disas_fn(m, gfx.hi, gfx.lo);
		}

		int ret = config.macro_fn();
		if (ret != 0)
		{
			state.ret = ret;
			break;
		}

		if (config.stop_on_end != 0
			&& (m->id == gfxd_SPBranchList
				|| m->id == gfxd_SPEndDisplayList))
		{
			break;
		}

		int n_pop = config.ucode->macro_tbl[m->id].n_gfx;
		int n_rem = state.n_gfx - n_pop;
		{
			int n_byte = n_rem * sizeof(gfxd_macro_t);
			memmove(&state.macro[0], &state.macro[n_pop], n_byte);
			state.n_gfx = n_rem;
		}
		{
			int n_byte = n_rem * sizeof(Gfx);
			memmove(&state.gfx[0], &state.gfx[n_pop], n_byte);
			state.n_byte = n_byte;
		}
		state.macro_offset += n_pop * sizeof(Gfx);
	}

	return state.ret;
}

int gfxd_macro_offset(void)
{
	return state.macro_offset;
}

int gfxd_macro_packets(void)
{
	return config.ucode->macro_tbl[state.macro[0].id].n_gfx;
}

const void *gfxd_macro_data(void)
{
	return state.gfx;
}

int gfxd_macro_id(void)
{
	return state.macro[0].id;
}

const char *gfxd_macro_name(void)
{
	int id = state.macro[0].id;
	const gfxd_macro_type_t *t = &config.ucode->macro_tbl[id];

	if (t->prefix == NULL && t->suffix == NULL)
	{
		return NULL;
	}
	else
	{
		static TLOCAL char buf[32];

		char *p = buf;
		if (t->prefix != NULL)
		{
			const char *s = t->prefix;
			while (*s != '\0')
				*p++ = *s++;
		}
		*p++ = 'g';
		if (config.arg == NULL)
			*p++ = 's';
		if (t->suffix != NULL)
		{
			const char *s = t->suffix;
			while (*s != '\0')
				*p++ = *s++;
		}
		*p++ = '\0';

		return buf;
	}
}

int gfxd_arg_count(void)
{
	return config.ucode->macro_tbl[state.macro[0].id].n_arg;
}

int gfxd_arg_type(int arg_num)
{
	return state.macro[0].arg[arg_num].type;
}

const char *gfxd_arg_name(int arg_num)
{
	return state.macro[0].arg[arg_num].name;
}

int gfxd_arg_fmt(int arg_num)
{
	return config.ucode->arg_tbl[state.macro[0].arg[arg_num].type].fmt;
}

const gfxd_value_t *gfxd_arg_value(int arg_num)
{
	return &state.macro[0].arg[arg_num].value;
}

const gfxd_value_t *gfxd_value_by_type(int type, int idx)
{
	gfxd_macro_t *m = &state.macro[0];
	const gfxd_macro_type_t *t = &config.ucode->macro_tbl[m->id];

	for (int i = 0; i < t->n_arg; i++)
	{
		gfxd_arg_t *a = &m->arg[i];
		if (a->type == type)
		{
			if (idx == 0)
				return &a->value;
			else
				idx--;
		}
	}

	return NULL;
}

int gfxd_arg_valid(int arg_num)
{
	return state.macro[0].arg[arg_num].bad == 0;
}
