## Installing
Run `make` for a single-threaded build, or `make MT=y` for a multi-threaded
build. Copy `libgfxd.a` to your lib directory, and `gfxd.h` to your include
directory.

## Example usage
Example source code:
```
#include <stdio.h>
#include <gfxd.h>

static int macro_fn(void)
{
	/* Print a tab before each macro, and a comma and newline after each
	   macro */
	gfxd_puts("\t");
	gfxd_macro_dflt(); /* Execute the default macro handler */
	gfxd_puts(",\n");

	return 0;
}

int main()
{
	/* Read from stdin and write to stdout */
	gfxd_input_fd(fileno(stdin));
	gfxd_output_fd(fileno(stdout));

	/* Override the default macro handler to make the output prettier */
	gfxd_macro_fn(macro_fn);

	/* Select F3DEX as the target microcode */
	gfxd_target(gfxd_f3dex);

	/* Set the input endianness to big endian, and the word size to 4 */
	gfxd_endian(gfxd_endian_big, 4);

	/* Print an opening brace */
	gfxd_puts("{\n");

	/* Execute until the end of input, or until encountering an invalid
	   command */
	gfxd_execute();

	/* Print a closing brace */
	gfxd_puts("}\n");
}
```

Example input (binary):
```
0xe7000000, 0x00000000,
0xfc127e03, 0xfffffdf8,
0xb900031d, 0xc8112078,
0xb6000000, 0x000e0000,
0xb7000000, 0x00012000,
0xfa000000, 0xffffffff,
0x040030bf, 0x000002e0,
0xb1000204, 0x00020604,
0xb1080a0c, 0x000a0e0c,
0xb10a1012, 0x000a120e,
0xb1140200, 0x00140016,
0xb8000000, 0x00000000,
```

Example output:
```
{
	gsDPPipeSync(),
	gsDPSetCombineLERP(TEXEL0, 0, SHADE, 0, 0, 0, 0, 1, COMBINED, 0, PRIMITIVE, 0, 0, 0, 0, COMBINED),
	gsDPSetRenderMode(G_RM_FOG_SHADE_A, G_RM_AA_ZB_OPA_SURF2),
	gsSPClearGeometryMode(G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR),
	gsSPSetGeometryMode(G_CULL_BACK | G_FOG),
	gsDPSetPrimColor(0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
	gsSPVertex(0x000002E0, 12, 0),
	gsSP2Triangles(0, 1, 2, 0, 1, 3, 2, 0),
	gsSP2Triangles(4, 5, 6, 0, 5, 7, 6, 0),
	gsSP1Quadrangle(5, 8, 9, 7, 0),
	gsSP1Quadrangle(10, 1, 0, 11, 0),
	gsSPEndDisplayList(),
}
```

## Input/output methods
The input consists of any number of `Gfx` packets, and the output is the
decompiled macros in plain-text. The endianness and microcode type of the input
can be set using `gfxd_endian` and `gfxd_target`.

Several methods of doing I/O are available. No method is selected by default,
meaning there will be no input, and any output will be discarded.

---

##### `void gfxd_input_buffer(const void *buf, int size)`
##### `void gfxd_output_buffer(char *buf, int size)`
Use the buffer pointed to by `buf`, of `size` bytes.

---

##### `void gfxd_input_fd(int fd)`
##### `void gfxd_output_fd(int fd)`
Use `read()` / `write()` with the provided file descriptor, `fd`.

---

##### `typedef int gfxd_input_fn_t(void *buf, int count)`
##### `typedef int gfxd_output_fn_t(const char *buf, int count)`
##### `void gfxd_input_callback(gfxd_input_fn_t *fn)`
##### `void gfxd_output_callback(gfxd_output_fn_t *fn)`
Use the provided callback function, `fn`. `fn` should copy at most `count`
bytes to/from `buf`, and return the number of bytes actually copied. The input
callback should return 0 to signal end of input.

## Handlers
The macro handler function is responsible for writing the output of each
decompiled macro. The default macro handler is `gfxd_macro_dflt`, but this can
be changed with `gfxd_macro_fn`. The new handler can extend the default
function by calling `gfxd_macro_dflt` within it, or it can override it
completely.

---

##### `int gfxd_macro_dflt()`
The default macro handler. Outputs the macro name, dynamic display list pointer
if one has been specified, and then each argument in order using the function
registered using `gfxd_arg_fn` (`gfxd_arg_dflt` by default), and returns zero.
Because it is designed to be extended, it only outputs the macro text, without
any whitespace or punctuation before or after. When this function is used as
the sole macro handler, it will output the entire display list on one line
without any separation between macros, which is probably not what you want.

---

##### `void gfxd_macro_fn(gfxd_macro_fn_t *fn)`
Set `fn` to be the macro handler function. `fn` can be null, in which case the
handler is reset to the default.

---

##### `void gfxd_arg_dflt(int arg_num)`
The default argument handler for `gfxd_macro_dflt`. For the argument with index
`arg_num`, calls `gfxd_arg_callbacks`, and prints the argument value if the
callback returns zero, or if there is no callback for the given argument.

---

##### `void gfxd_arg_fn(gfxd_arg_fn_t *fn)`
Set `fn` to be the argument handler function, called by `gfxd_macro_dflt`,
for each argument in the current macro, not counting the dynamic display list
pointer if one has been specified. `fn` can be null, in which case the handler
is reset to the default. This only affects the output of `gfxd_macro_dflt`, and
has no observable effect if `gfxd_macro_dflt` is overridden (not extended).

## Argument callbacks
Callbacks can be registered that will be executed when an argument of a certain
type is encountered. The default argument handler `gfxd_arg_dflt` will execute
callbacks as needed using `gfxd_arg_callbacks`. If a callback returns non-zero,
`gfxd_arg_dflt` will not output anything. This is to allow callbacks to
override the default argument output. Otherwise, `gfxd_arg_dflt` will output
the argument value after the callback function's output.

---

##### `int gfxd_arg_callbacks(int arg_num)`
Examines the argument with index `arg_num` and executes the callback function
for that argument type, if such a callback is supported and has been
registered. This function returns the value that was returned by the callback
function. If no callback function has been registered for the argument type,
zero is returned.

Most argument callbacks have some extra parameters containing information that
might be relevant to the argument that triggered the callback. The extra
information is extracted only from the current macro, as gfxd does not retain
any context information from previous or subsequent macros. If any of the extra
parameter values is not available in the current macro, the value for that
parameter is substituted with `-1` for signed parameters, and zero for unsigned
parameters.

---

##### `typedef int gfxd_tlut_fn_t(uint32_t tlut, int32_t idx, int32_t count)`
##### `void gfxd_tlut_callback(gfxd_tlut_fn_t *fn)`
Set the callback function for palette arguments. The argument type is
`gfxd_Tlut`. The palette index is in `idx` and the number of colors in `count`.

---

##### `typedef int gfxd_timg_fn_t(uint32_t timg, int32_t fmt, int32_t siz, int32_t width, int32_t height, int32_t pal)`
##### `void gfxd_timg_callback(gfxd_timg_fn_t *fn)`
Set the callback function for texture arguments. The argument type is
`gfxd_Timg`. The image format is in `fmt` and `siz`, the dimensions in `width`
and `height`, and the palette index in `pal`.

---

##### `typedef int gfxd_cimg_fn_t(uint32_t cimg, int32_t fmt, int32_t siz, int32_t width)`
##### `void gfxd_cimg_callback(gfxd_cimg_fn_t *fn)`
Set the callback function for frame buffer arguments. The argument type is
`gfxd_Cimg`. The image format is in `fmt` and `siz`, and the horizontal
resolution in `width`.

---

##### `typedef int gfxd_zimg_fn_t(uint32_t zimg)`
##### `void gfxd_zimg_callback(gfxd_zimg_fn_t *fn)`
Set the callback function for depth buffer arguments. The argument type is
`gfxd_Zimg`.

---

##### `typedef int gfxd_dl_fn_t(uint32_t dl)`
##### `void gfxd_dl_callback(gfxd_dl_fn_t *fn)`
Set the callback function for display list arguments. The argument type is
`gfxd_Dl`.

---

##### `typedef int gfxd_mtx_fn_t(uint32_t mtx)`
##### `void gfxd_mtx_callback(gfxd_mtx_fn_t *fn)`
Set the callback function for matrix arguments. The argument type is
`gfxd_Mtxptr`.

---

##### `typedef int gfxd_lookat_fn_t(uint32_t lookat, int32_t count)`
##### `void gfxd_lookat_callback(gfxd_lookat_fn_t *fn)`
Set the callback function for lookat array arguments. The argument type is
`gfxd_Lookatptr`. The number of lookat structures (1 or 2) is in `count`. 

---

##### `typedef int gfxd_light_fn_t(uint32_t light, int32_t count)`
##### `void gfxd_light_callback(gfxd_light_fn_t *fn)`
Set the callback function for light array arguments. The argument type is
`gfxd_Lightptr`. The number of light structures is in `count`.

---

##### `typedef int gfxd_seg_fn_t(uint32_t seg, int32_t num)`
##### `void gfxd_seg_callback(gfxd_seg_fn_t *fn)`
Set the callback function for segment base arguments. The argument type is
`gfxd_Segptr`. The segment number is in `num`.

---

##### `typedef int gfxd_vtx_fn_t(uint32_t vtx, int32_t num)`
##### `void gfxd_vtx_callback(gfxd_vtx_fn_t *fn)`
Set the callback function for vertex array arguments. The argument type is
`gfxd_Vtxptr`. The number of vertex structures is in `num`.

---

##### `typedef int gfxd_vp_fn_t(uint32_t vp)`
##### `void gfxd_vp_callback(gfxd_vp_fn_t *fn)`
Set the callback function for viewport arguments. The argument type is
`gfxd_Vp`.

---

##### `typedef int gfxd_uctext_fn_t(uint32_t text, uint32_t size)`
##### `void gfxd_uctext_callback(gfxd_uctext_fn_t *fn)`
Set the callback function for microcode text arguments. The argument type is
`gfxd_Uctext`. The size of the text segment is in `size`.

---

##### `typedef int gfxd_ucdata_fn_t(uint32_t data, uint32_t size)`
##### `void gfxd_ucdata_callback(gfxd_ucdata_fn_t *fn)`
Set the callback function for microcode data arguments. The argument type is
`gfxd_Ucdata`. The size of the data segment is in `size`.

---

##### `typedef int gfxd_dram_fn_t(uint32_t dram, uint32_t size)`
##### `void gfxd_dram_callback(gfxd_dram_fn_t *fn)`
Set the callback function for generic pointer arguments. The argument type is
`gfxd_Dram`. The size of the data is in `size`.

## General settings
These functions control general input and output settings.

---

##### `void gfxd_target(gfxd_ucode_t ucode)`
Select `ucode` as the target microcode. `ucode` can be `gfxd_f3d`, `gfxd_f3db`,
`gfxd_f3dex`, `gfxd_f3dexb`, or `gfxd_f3dex2`. The microcode must be selected
before `gfxd_execute`, as no microcode is selected by default.

---

##### `void gfxd_endian(int endian, int wordsize)`
Select `endian` as the endianness of the input, and `wordsize` as the size of
each word in number of bytes. `endian` can be `gfxd_endian_big`,
`gfxd_endian_little`, or `gfxd_endian_host` (the endianness of the host
machine). `wordsize` can be 1, 2, 4, or 8. Big endian is selected by default,
with a word size of 4.

---

##### `void gfxd_dynamic(const char *arg)`
Enable or disable the use of dynamic `g` macros instead of static `gs` macros,
and select the dynamic display list pointer argument to be used. `arg` will be
used by `gfxd_macro_dflt` as the first argument to dynamic macros. If `arg` is
null, dynamic macros are disabled, and `gs` macros are used. Also affects the
result of `gfxd_macro_name`, as it will return either the dynamic or static
version of the macro name as selected by this setting.

---

##### `void gfxd_enable(int cap)`
##### `void gfxd_disable(int cap)`
Enable or disable the feature specified by `cap`. Can be one of the following;
- `gfxd_stop_on_invalid`: Stop execution when encountering an invalid macro.
Enabled by default.
- `gfxd_stop_on_end`: Stop execution when encountering a `SPBranchList` or
`SPEndDisplayList`. Enabled by default.
- `gfxd_emit_dec_color`: Print color components as decimal instead of
hexadecimal. Disabled by default.
- `gfxd_emit_q_macro`: Print fixed-point conversion `q` macros for fixed-point
values. Disabled by default.
- `gfxd_emit_ext_macro`: Emit non-standard macros. Some commands are valid
(though possibly meaningless), but have no macros associated with them, such as
a standalone `G_RDPHALF_1`. When this feature is enabled, such a command will
produce a non-standard `gsDPHalf1` macro instead of a raw hexadecimal command.
Also enables some non-standard multi-packet texture loading macros. Disabled by
default.

---

##### `void gfxd_udata_set(void *ptr)`
##### `void *gfxd_udata_get(void)`
Set or get a generic pointer that can be used to pass user-defined data in and
out of callback functions.

## Execution
Decompilation is started using the `gfxd_execute` function. When gfxd is
executing (i.e. after `gfxd_execute` has been entered, and before it returns),
the general settings and the I/O settings should not be changed.

---

##### `int gfxd_execute()`
Start executing gfxd with the current settings. For each macro, the macro
handler registered with `gfxd_macro_fn` is called. Execution ends when the
input ends, the macro handler returns non-zero, when an invalid macro is
encountered and `gfxd_stop_on_invalid` is enabled, or when `SPBranchList` or
`SPEndDisplayList` is encountered and `gfxd_stop_on_end` is enabled. If
execution ends due to an invalid macro, `-1` is returned. If execution ends
because the macro handler returns non-zero, the return value from the macro
handler is returned. Otherwise zero is returned.

## Macro information
The following functions can be used to obtain information about the current
macro and its arguments. They should only be used in custom handlers and
callbacks from within `gfxd_execute`. If used elsewhere, their behavior is
undefined.

---

##### `int gfxd_macro_offset()`
Returns the offset in the input data of the current macro. The offset starts
at zero when `gfxd_execute` is called.

---

##### `int gfxd_macro_packets()`
Returns the number of `Gfx` packets within the current macro.

---

##### `const void *gfxd_macro_data()`
Returns a pointer to the input data for the current macro. The data is not
byte-swapped. The data has a length of `sizeof(Gfx) * gfxd_macro_packets()`.

---

##### `int gfxd_macro_id()`
Returns a number that uniquely identifies the current macro. The number will
be one of the constants in `gfxd.h`.

---

##### `const char *gfxd_macro_name()`
Returns the name of the current macro. If the macro does not have a name (i.e.
it's invalid), null is returned. If a dynamic display list pointer has been
specified, the dynamic `g` version is returned. Otherwise the static `gs`
version is returned. The returned pointer is invalidated by a subsequent call
to `gfxd_macro_name`.

---

##### `int gfxd_arg_count()`
Returns the number of arguments to the current macro, not including a dynamic
display list pointer if one has been specified.

---

##### `int gfxd_arg_type(int arg_num)`
Returns a number that identifies the type of the argument with index `arg_num`.
The number will be one of the constants in `gfxd.h`.

---

##### `const char *gfxd_arg_name(int arg_num)`
Returns the name of the argument with index `arg_num`. Argument names are not
canonical, nor are they needed for macro disassembly, but they can be useful
for informational and diagnostic purposes.

---

##### `int gfxd_arg_fmt(int arg_num)`
Returns the data format of the argument with index `arg_num`. The return value
will be `gfxd_argfmt_i` for `int32_t`, `gfxd_argfmt_u` for `uint32_t`, or
`gfxd_argfmt_f` for `float`. When accessing the value of the argument with
`gfxd_arg_value`, the member with the corresponding type should be used.

---

##### `const gfxd_value_t *gfxd_arg_value(int arg_num)`
Returns a pointer to the value of the argument with index `arg_num`. The value
is a union of type `gfxd_value_t` with the following layout;
```
typedef union
{
	int32_t		i;
	uint32_t	u;
	float		f;
} gfxd_value_t
```

---

##### `const gfxd_value_t *gfxd_value_by_type(int type, int idx)`
Returns a pointer to the value of the argument that is of `type`, and has order
`idx` in all arguments of that type. An `idx` of zero returns the first
argument that has the specified type. If there is no argument with the given
type and order, null is returned.

---

##### `int gfxd_arg_valid(int arg_num)`
Returns non-zero if the argument with index `arg_num` is "valid", for some
definition of valid. An invalid argument generally means that the disassembler
found inconsistencies in the input data, or that the data can not be reproduced
by the current macro type. The argument still has a value that can be printed,
though the value is not guaranteed to make any sense.

## Custom output
When the default handlers are overridden or extended, the custom handler
functions will want to do some output of their own. The following methods are
available for inserting custom text into the gfxd output.

---

##### `int gfxd_write(const void *buf, int count)`
Insert `count` bytes from the buffer at `buf` into the output. The number of
characters written is returned.

---

##### `int gfxd_puts(const char *str)`
Insert the null-terminated string at `str` into the output. The number of
characters written is returned.

---

##### `int gfxd_printf(const char *fmt, ...)`
Insert the printf-formatted string described by `fmt` and additional arguments
into the output. Limited to 255 characters. The number of characters written is
returned.

---

##### `int gfxd_print_value(int type, const gfxd_value_t *value)`
Insert the type-formatted value into the output. The type should be one of the
constants in `gfxd.h`. The number of characters written is returned. The
macro argument with index `n` can be printed with
`gfxd_print_value(gfxd_arg_type(n), gfxd_arg_value(n))`.
