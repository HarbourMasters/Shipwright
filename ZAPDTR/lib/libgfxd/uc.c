#include <inttypes.h>
#include <string.h>
#include <stdint.h>
#include "gbi.h"
#include "gfxd.h"
#include "priv.h"

#include "uc_argfn.c"
#include "uc_argtbl.c"
#include "uc_macrofn.c"
#include "uc_macrotbl.c"

UCFUNC int disas(gfxd_macro_t *m, uint32_t hi, uint32_t lo)
{
	int opcode = (hi >> 24) & 0xFF;

	for (int i = 0; i < sizeof(macro_tbl) / sizeof(macro_tbl[0]); i++)
	{
		const gfxd_macro_type_t *t = &macro_tbl[i];
		if (t->disas_fn != NULL && t->opcode == opcode)
			return t->disas_fn(m, hi, lo);
	}

	return d_Invalid(m, hi, lo);
}

UCFUNC int combine(gfxd_macro_t *m, int num)
{
	int opcode = macro_tbl[m->id].opcode;

	for (int i = 0; i < sizeof(macro_tbl) / sizeof(macro_tbl[0]); i++)
	{
		const gfxd_macro_type_t *t = &macro_tbl[i];
		if (t->combine_fn != NULL
			&& t->opcode == opcode
			&& (t->ext == 0 || config.emit_ext_macro != 0))
		{
			if (t->combine_fn(m, num) == 0)
				return 0;
		}
	}

	return -1;
}

static const struct gfxd_ucode uc =
{
	.disas_fn = disas,
	.combine_fn = combine,
	.arg_tbl = arg_tbl,
	.macro_tbl = macro_tbl,
};

const gfxd_ucode_t uc_name = &uc;
