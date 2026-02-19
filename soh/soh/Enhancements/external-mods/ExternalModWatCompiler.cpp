#include "ExternalModWatCompiler.h"

#include <algorithm>
#include <chrono>
#include <limits>

#ifdef SOH_HAS_WABT
#include <wabt/binary-writer.h>
#include <wabt/error-formatter.h>
#include <wabt/resolve-names.h>
#include <wabt/shared-validator.h>
#include <wabt/stream.h>
#include <wabt/validator.h>
#include <wabt/wast-lexer.h>
#include <wabt/wast-parser.h>
#endif

namespace SOH {

namespace {

std::string StripUtf8Bom(const std::string& value) {
    if (value.size() >= 3 && static_cast<uint8_t>(value[0]) == 0xEF && static_cast<uint8_t>(value[1]) == 0xBB &&
        static_cast<uint8_t>(value[2]) == 0xBF) {
        return value.substr(3);
    }
    return value;
}

} // namespace

bool CompileWatToWasm(const std::string& sourceText, const std::string& sourceName, std::vector<uint8_t>& outWasm,
                      int32_t& outCompileMs, std::string& outDiagnostics) {
    outWasm.clear();
    outCompileMs = 0;
    outDiagnostics.clear();

#ifndef SOH_HAS_WABT
    (void)sourceText;
    (void)sourceName;
    outDiagnostics = "This build does not include WABT support (.wat runtime.module is unavailable).";
    return false;
#else
    const auto compileStart = std::chrono::steady_clock::now();
    const auto sanitizedSource = StripUtf8Bom(sourceText);
    if (sanitizedSource.empty()) {
        outDiagnostics = "WAT source is empty";
        return false;
    }

    wabt::Errors errors;
    auto lexer = wabt::WastLexer::CreateBufferLexer(sourceName, sanitizedSource.data(), sanitizedSource.size(), &errors);
    if (lexer == nullptr) {
        outDiagnostics = "Failed to create WAT lexer";
        return false;
    }

    wabt::Features features;
    wabt::WastParseOptions parseOptions(features);
    parseOptions.parse_binary_modules = false;

    std::unique_ptr<wabt::Module> module;
    if (wabt::Failed(wabt::ParseWatModule(lexer.get(), &module, &errors, &parseOptions)) || module == nullptr) {
        if (errors.empty()) {
            outDiagnostics = "Failed to parse WAT module";
        } else {
            outDiagnostics = wabt::FormatErrorsToString(errors, wabt::Location::Type::Text);
        }
        return false;
    }

    if (wabt::Failed(wabt::ResolveNamesModule(module.get(), &errors))) {
        if (errors.empty()) {
            outDiagnostics = "Failed to resolve WAT names";
        } else {
            outDiagnostics = wabt::FormatErrorsToString(errors, wabt::Location::Type::Text);
        }
        return false;
    }

    wabt::ValidateOptions validateOptions(features);
    if (wabt::Failed(wabt::ValidateModule(module.get(), &errors, validateOptions))) {
        if (errors.empty()) {
            outDiagnostics = "Failed to validate WAT module";
        } else {
            outDiagnostics = wabt::FormatErrorsToString(errors, wabt::Location::Type::Text);
        }
        return false;
    }

    wabt::MemoryStream wasmStream;
    wabt::WriteBinaryOptions writeOptions(features, true, false, false);
    if (wabt::Failed(wabt::WriteBinaryModule(&wasmStream, module.get(), writeOptions)) ||
        wabt::Failed(wasmStream.result())) {
        outDiagnostics = "Failed to write wasm binary";
        return false;
    }

    const auto& wasmData = wasmStream.output_buffer().data;
    outWasm.assign(wasmData.begin(), wasmData.end());

    const auto elapsedMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - compileStart).count();
    outCompileMs = static_cast<int32_t>(
        std::clamp<int64_t>(elapsedMs, 0, static_cast<int64_t>(std::numeric_limits<int32_t>::max())));
    return true;
#endif
}

} // namespace SOH