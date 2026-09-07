# Code Guidelines for Ship of Harkinian

> [!IMPORTANT]
> These guidelines do not apply to the files under the `src/` directory. See [this](#original-source) section.

> [!NOTE]
> Keep in mind that these guidelines have not always been here so some code has not been updated to follow these guidelines yet. Contributions to bring those cases inline with these guidelines are welcome.

## General
Code must be C++, not C.

Only use features supported by the latest versions of GCC, Clang & MSVC.
See these compatibility tables ([C](https://en.cppreference.com/c/compiler_support), [C++](https://en.cppreference.com/cpp/compiler_support)) for which features are supported by each compiler.

Do not use:
- [`goto`](https://en.cppreference.com/cpp/language/goto).
- Octal [integer literals](https://en.cppreference.com/cpp/language/integer_literal).
- Octal [character escape sequences](https://en.cppreference.com/cpp/language/escape).
- [`const_cast`](https://en.cppreference.com/cpp/language/const_cast) to cast away constness or volatility.
- [`new`](https://en.cppreference.com/cpp/language/new) & [`delete`](https://en.cppreference.com/cpp/language/delete) expressions.
- [Alternative operator representations](https://en.cppreference.com/cpp/language/operator_alternative).
- [Inline assembly](https://en.cppreference.com/cpp/language/asm).
- [`void` as an argument type](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#nl25-dont-use-void-as-an-argument-type).

Use parenthesis when [operator precedence](https://en.cppreference.com/cpp/language/operator_precedence) is not obvious (this includes between `||` & `&&`).

Always use braces (`{}`) for `if` statements, `for` statements, `while` statements and `case` statements.

Make things `const`/`constexpr`/`consteval`/`noexcept` when possible.

Prefer:
- [Post-increment operators](https://en.cppreference.com/cpp/language/operator_incdec#Postfix_operators) over [pre-increment operators](https://en.cppreference.com/cpp/language/operator_incdec#Prefix_operators).
- Explicit conversions over implicit conversions.
- The `at` method over the `[]` operator for types such as [`std::array`](https://en.cppreference.com/cpp/container/array) & [`std::vector`](https://en.cppreference.com/cpp/container/vector).
- C++ style casts over C style casts.
- [`SPDLOG` macros](https://github.com/gabime/spdlog/blob/v1.x/include/spdlog/spdlog.h) over [`LUSLOG` macros](../libultraship/include/libultraship/log/luslog.h).

[Clang Format](https://clang.llvm.org/docs/ClangFormat.html) (see [FORMATTING.md](./FORMATTING.md) for more details) is used to maintain code formatting, it may be disabled for a region of code with `// clang-format off` & `// clang-format on`.
This must only be used for things such as tables which are clearer with whitespace alignment.

TODO comments are generally used with one of these prefixes:
- `TODO`: General TODO comments.
- `Upstream TODO`: TODOs relating to syncing code with decomp.
- `RANDOTODO`: TODOs relating to randomizer (there are some that have the `RANDTODO` prefix).

Files and folder names must be in UpperCamelCase.

ShipInit declarations must be at the bottom of the file.

Files must:
- Have a trailing newline.
- Be encoded in [UTF-8](https://en.wikipedia.org/wiki/UTF-8).
- Use 4 spaces for indentation.

Instead of using regular `assert`s which can't prevent issues in releases, use an if statement with:
- A log message.
- `assert(false);`.
- An early return from the function when appropriate.

## [Attributes](https://en.cppreference.com/cpp/language/attributes)

Do not use the attribute `using` syntax introduzed in C++ 17.

Do not include multiple attibutes in one attribute specifier sequence.

Do not use the `[[assume(expression)]]` or `[[indeterminate]]` attributes as they can easily lead to UB.


Functions that do not return must use the `[[noreturn]]` attribute.

Functions that don't have any side effects and return a value must use the `[[nodiscard]]` attribute, prefferably with a reason specified.

Switch case fallthroughs must me marked with the `[[fallthrough]]` attribute.


Use all other attributes as appropiate.

## Types
<!--
// This is a possible future addition but there's currently debate over if it is a good idea.

Use the libultra types when possible (`u64` instead of `uint64_t`, `f32` instead of `float`, etc.).
-->

Use `bool`, `true` & `false` when appropriate instead of an integer type, `1` & `0`.

Do not use `inptr_t` & `uintptr_t` as they can cause issues with pointer provenance, use `void*` (or a concrete pointer) instead if possible.

Use [`std::optional`](https://en.cppreference.com/cpp/utility/optional), [`std::variant`](https://en.cppreference.com/cpp/utility/variant) or [`std::expected`](https://en.cppreference.com/cpp/utility/expected) instead of sentinel values.

Use references instead of pointers (with [`std::optional`](https://en.cppreference.com/cpp/utility/optional) for nullable references) if possible.

> [!NOTE]
> `std::optional<T&>` is not allowed until C++ 26, use `std::optional<T*>` instead in the meanwhile.

Prefer error values over exceptions.

Prefer factory functions over public constructors (or [two phase initialization](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#rnr-two-phase-init)) and execute as much of the initialization logic as possible in the factory function.
This avoid issues such as accidental default initialization and referencing class members before they are valid.

If possible, make invalid states unrepresentable.

## Headers
Headers must have a `.h` extension.

Include the least amount of other headers needed, as an example, if possible, use extern declarations for types (such as `Rando::Context`) instead of including the corresponding header if that is the only reason for its inclusion.

Headers must not silently depend on other headers to compile.

Use the `#pragma once` pre-processor directive instead of an inclusion macro.

Do not expose function implementations in headers.

## Original Source
We want to keep the files from the original source (`src/`) as similar to the [original decomp](https://github.com/zeldaret/oot) ones as possible, as such, the only changes that should be done (aside from removing differences previously introduced) are the addition of `GameInteractor_Should` calls (also reffered to as the "Vanilla Behaviour" system) and its associated header.

> [!NOTE]
> We're not fully up to date with decomp so there might be some differences, contributions to bring our files closer to decomp's are welcome.