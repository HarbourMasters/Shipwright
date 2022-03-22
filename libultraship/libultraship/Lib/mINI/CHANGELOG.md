## 0.9.11 (October 6, 2021)
- `BUGFIX` - Fixes various compiler warnings.

## 0.9.10 (March 4, 2021)
- `BUGFIX` - Change delimiter constants to `const char* const` to prevent unnecessary allocations. ([#5](https://github.com/pulzed/mINI/issues/5))

## 0.9.9 (February 22, 2021)
- `BUGFIX` Adds missing cctype header. ([#4](https://github.com/pulzed/mINI/pull/4))

## 0.9.8 (February 14, 2021)
- `BUGFIX` Avoid C4244 warning. ([#2](https://github.com/pulzed/mINI/pull/2))

## 0.9.7 (August 14, 2018)
- `FEATURE` Adds case sensitivity toggle via a macro definition.

## 0.9.6 (May 30, 2018)
- `BUGFIX` Changed how files are written / generated. Proper line endings are selected depending on the system.
- `FEATURE` Support UTF-8 encoding.

## 0.9.5 (May 28, 2018)
- `BUGFIX` Fixes a bug where writer would skip escaped `=` sequences for new sections.

## 0.9.4 (May 28, 2018)
- `BUGFIX / FEATURE` Equals (`=`) characters within key names are now allowed. When writing or generating a file, key values containing the `=` characters will be escaped with the `\=` sequence. Upon reading the file back, the escape sequences will again be converted back to `=`. Values do not use escape sequences and may contain `=` characters.
- `BUGFIX` Square bracket characters (`[` and `]`) are now valid within section names.
- `BUGFIX` Trailing comments on section lines are now parsed properly. Fixes a bug where a trailing comment containing the `]` character would break the parser.
- `BUGFIX` Values being written or generated are now stripped of leading and trailing whitespace to conform to the specified format.

## 0.9.3 (May 24, 2018)
- `BUGFIX` Fixes inconsistent behavior with empty section and key names where read would ignore empty names and write would allow them.
- `FEATURE` Empty key and section names are now allowed.

## 0.9.2 (May 24, 2018)
- `BUGFIX` Fixes the multiple definition bug [#1](/../../issues/1)
- `BUGFIX` Fixes a bug where a `write()` call to an empty file would begin writing at line 2 instead of 1 due to a reader bug.

## 0.9.1 (May 20, 2018)
- `BUGFIX` Fixed a bug where the writer would skip writing new keys and values following an empty section.

## 0.9.0 (May 20, 2018)
- Release v0.9.0
