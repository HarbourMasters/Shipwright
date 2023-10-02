# ZAPD: Zelda Asset Processor for Decomp

## Compiling

### Dependencies

ZAPD needs a compiler with C++17 support.

ZAPD has the following library dependencies:

- `libpng`

In a Debian/Ubuntu based environment, those could be installed with the following command:

```bash
sudo apt install libpng-dev
```

On a Mac, you will need to install libpng with Homebrew or MacPorts; we currently only support Homebrew. You can run

```bash
brew install libpng
```

to install it via Homebrew.

### Building

#### Linux / *nix

ZAPD uses the clasic `Makefile` approach. To build just run `make` (or even better `make -j` for faster compilations).

You can configure a bit your ZAPD build with the following options:

- `OPTIMIZATION_ON`: If set to `0` optimizations will be disabled (compile with `-O0`). Any other value compiles with `-O2`. Defaults to `1`.
- `ASAN`: If it is set to a non-zero then ZAPD will be compiled with Address Sanitizer enabled (`-fsanitize=address`). Defaults to `0`.
- `DEPRECATION_ON`: If it is set to a zero then deprecation warnings will be disabled. Defaults to `1`.
- `DEBUG`: If non-zero, ZAPD will be compiled in _development mode_. This implies the following:
  - Debugging symbols enabled (`-g3`). They are disabled by default.
  - `OPTIMIZATION_ON=0`: Disables optimizations (`-O0`).
  - `DEPRECATION_ON=0`: Disables deprecation warnings.
- `LLD=1`: builds with the LLVM linker `ld.lld` instead of the system default.

As an example, if you want to build ZAPD with optimizations disabled and use the address sanitizer, you could use the following command:

```bash
make -j OPTIMIZATION_ON=0 ASAN=1
```

#### Windows

This repository contains `vcxproj` files for compiling under Visual Studio environments. See `ZAPD/ZAPD.vcxproj`.

## Invoking ZAPD

ZAPD needs a _File parsing mode_ to be passed as first parameter. The options are:

- `e`: "Extraction" mode.
  - In this mode, ZAPD expects a XML file as input, a folder as ouput and a path to the baserom files.
  - ZAPD will read the XML and use it as a guide to extract the contents of the specified asset file from the baserom folder.
    - For more info of the format of those XMLs, see the [ZAPD extraction XML reference](docs/zapd_extraction_xml_reference.md).
- `bsf`: "Build source file" mode.
  - This is an experimental mode.
  - It was going to be used to let you have XMLs that aren't just for extraction. Might get used, might not. Still need to experiment on that.
- `btex`: "Build texture" mode.
  - In this mode, ZAPD expects a PNG file as input, a filename as ouput and a texture type parameter (`-tt`).
  - ZAPD will try to convert the given PNG into the contents of a `uint64_t` C array.
- `bren`: "Build (render) background" mode.
  - In this mode, ZAPD expects a JPG file as input and a filename as ouput.
  - ZAPD will try to convert the given JPG into the contents of a `uint64_t` C array.
- `blb`: "Build blob" mode.
  - In this mode, ZAPD expects a BIN file as input and a filename as ouput.
  - ZAPD will try to convert the given BIN into the contents of a `uint8_t` C array.

ZAPD also accepts the following list of extra parameters:

- `-i PATH` / `--inputpath PATH`: Set input path.
- `-o PATH` / `--outputpath PATH`: Set output path.
- `-b PATH` / `--baserompath`: Set baserom path.
  - Can be used only in `e` or `bsf` modes.
- `-osf PATH`: Set source output path. This is the path where the `.c` and `.h` files will be extracted to. If omitted, it will use the value passed to `--outputpath` parameter.
- `-gsf MODE`: Generate source file during extraction. If `MODE` is `1`, C source files will be generated.
  - Can be used only in `e` mode.
- `-crc` / `--output-crc`: Outputs a CRC file for each extracted texture.
  - Can be used only in `e` or `bsf` modes.
- `-ulzdl MODE`: Use "Legacy ZDisplayList" instead of `libgfxd`. Set `MODE` to `1` to enable it.
  - Can be used only in `e` or `bsf` modes.
- `-profile MODE`: Enable profiling. Set `MODE` to `1` to enable it.
- `-uer MODE`: Split resources into their individual components (enabled by default). Set `MODE` to non-`1` to disable it.
- `-tt TYPE`: Set texture type.
  - Can be used only in mode `btex`.
  - Valid values:
    - `rgba32`
    - `rgb5a1`
    - `i4`
    - `i8`
    - `ia4`
    - `ia8`
    - `ia16`
    - `ci4`
    - `ci8`
- `-rconf PATH` Read Config File.
- `-eh`: Enable error handler.
  - Only available in non-Windows environments.
- `-v MODE`: Enable verbosity. Currently there are 3 possible values:
  - `0`: Default. Completely silent (except for warnings and errors).
  - `1`: Information.
  - `2` (and higher): Debug.
- `-wu` / `--warn-unaccounted`: Enable warnings for each unaccounted block of data found.
  - Can be used only in `e` or `bsf` modes.
- `-vu` / `--verbose-unaccounted`: Changes how unaccounteds are outputted. Max 4 bytes per line (a word) and add a comment with the offset of each of those lines.
  - Could be useful for looking at raw data or testing.
  - Can be used only in `e` or `bsf` modes.
- `-tm MODE`: Test Mode (enables certain experimental features). To enable it, set `MODE` to `1`.
- `-se` / `--set-exporter` : Sets which exporter to use.
- `--gcc-compat` : Enables GCC compatibly mode. Slower.
- `-us` / `--unaccounted-static` : Mark unaccounted data as `static` 
- `-s` / `--static` : Mark every asset as `static`.
  - This behaviour can be overridden per asset using `Static=` in the respective XML node.
- `-W...`: warning flags, see below

Additionally, you can pass the flag `--version` to see the current ZAPD version. If that flag is passed, ZAPD will ignore any other parameter passed.

### Warning flags

ZAPD contains a variety of warning types, with similar syntax to GCC or Clang's compiler warnings. Warnings can have three levels:

- Off (does not display anything)
- Warn (print a warning but continue processing)
- Err (behave like an error, i.e. print and throw an exception to crash ZAPD when occurs)

Each warning type uses one of these by default, but can be modified with flags, similarly to GCC or Clang:

- `-Wfoo` enables warnings of type `foo`
- `-Wno-foo` disables warnings of type `foo`
- `-Werror=foo` escalates `foo` to behave like an error
- `-Weverything` enables all warnings (they may be turned off using `-Wno-` flags afterwards)
- `-Werror` escalates all enabled warnings to errors

All warning types currently implemented, with their default levels:

| Warning type                | Default level | Description                                                              |
| --------------------------- | ------------- | ------------------------------------------------------------------------ |
| `-Wdeprecated`              | Warn          | Deprecated features                                                      |
| `-Whardcoded-pointer`       | Warn          | ZAPD lacks the info to make a symbol, so must output a hardcoded pointer |
| `-Wintersection`            | Warn          | Two assets intersect                                                     |
| `-Winvalid-attribute-value` | Err           | Attribute declared in XML is wrong                                       |
| `-Winvalid-extracted-data`  | Err           | Extracted data does not have correct form                                |
| `-Winvalid-jpeg`            | Err           | JPEG file does not conform to the game's format requirements             |
| `-Winvalid-png`             | Err           | Issues arising when processing PNG data                                  |
| `-Winvalid-xml`             | Err           | XML has syntax errors                                                    |
| `-Wmissing-attribute`       | Warn          | Required attribute missing in XML tag                                    |
| `-Wmissing-offsets`         | Warn          | Offset attribute missing in XML tag                                      |
| `-Wmissing-segment`         | Warn          | Segment not given in File tag in XML                                     |
| `-Wnot-implemented`         | Warn          | ZAPD does not currently support this feature                             |
| `-Wunaccounted`             | Off           | Large blocks of unaccounted                                              |
| `-Wunknown-attribute`       | Warn          | Unknown attribute in XML entry tag                                       |

There are also errors that do not have a type, and cannot be disabled.

For example, here we have invoked ZAPD in the usual way to extract using a (rather badly-written) XML, but escalating `-Wintersection` to an error:

![ZAPD warnings example](docs/zapd_warning_example.png?raw=true)
