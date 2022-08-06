#include "CrashHandler.h"
#include "spdlog/spdlog.h"
#include "Utils/Stringhelper.h"

#ifdef _MSC_VER

#pragma comment(lib, "Dbghelp.lib")

static void PrintRegisters(CONTEXT* ctx) {
    SPDLOG_CRITICAL("Register dump");
#if defined(_M_AMD64)
    SPDLOG_CRITICAL("Rax: {}", StringHelper::Sprintf("0x%016llX", ctx->Rax));
    SPDLOG_CRITICAL("Rcx: {}", StringHelper::Sprintf("0x%016llX", ctx->Rcx));
    SPDLOG_CRITICAL("Rdx: {}", StringHelper::Sprintf("0x%016llX", ctx->Rdx));
    SPDLOG_CRITICAL("Rbx: {}", StringHelper::Sprintf("0x%016llX", ctx->Rbx));
    SPDLOG_CRITICAL("Rsp: {}", StringHelper::Sprintf("0x%016llX", ctx->Rsp));
    SPDLOG_CRITICAL("Rbp: {}", StringHelper::Sprintf("0x%016llX", ctx->Rbp));
    SPDLOG_CRITICAL("Rsi: {}", StringHelper::Sprintf("0x%016llX", ctx->Rsi));
    SPDLOG_CRITICAL("Rdi: {}", StringHelper::Sprintf("0x%016llX", ctx->Rdi));
    SPDLOG_CRITICAL("R9:  {}", StringHelper::Sprintf("0x%016llX", ctx->R9));
    SPDLOG_CRITICAL("R10: {}", StringHelper::Sprintf("0x%016llX", ctx->R10));
    SPDLOG_CRITICAL("R11: {}", StringHelper::Sprintf("0x%016llX", ctx->R11));
    SPDLOG_CRITICAL("R12: {}", StringHelper::Sprintf("0x%016llX", ctx->R12));
    SPDLOG_CRITICAL("R13: {}", StringHelper::Sprintf("0x%016llX", ctx->R13));
    SPDLOG_CRITICAL("R14: {}", StringHelper::Sprintf("0x%016llX", ctx->R14));
    SPDLOG_CRITICAL("R15: {}", StringHelper::Sprintf("0x%016llX", ctx->R15));
    SPDLOG_CRITICAL("PC:  {}", StringHelper::Sprintf("0x%016llX", ctx->Rip));
    SPDLOG_CRITICAL("EFlags: {}", StringHelper::Sprintf("0x%08llX", ctx->EFlags));
#else
    SPDLOG_CRITICAL("Edi: 0x{}", ctx->Edi);
    SPDLOG_CRITICAL("Esi: 0x{}", ctx->Esi);
    SPDLOG_CRITICAL("Ebx: 0x{}", ctx->Ebx);
    SPDLOG_CRITICAL("Ecx: 0x{}", ctx->Ecx);
    SPDLOG_CRITICAL("Eax: 0x{}", ctx->Eax);
    SPDLOG_CRITICAL("Ebp: 0x{}", ctx->Ebp);
    SPDLOG_CRITICAL("Esp: 0x{}", ctx->Esp);
    SPDLOG_CRITICAL("EFlags: 0x{}", ctx->EFlags);
    SPDLOG_CRITICAL("Eip: 0x{}", ctx->Eip);
#endif
}

static void printStack(CONTEXT* ctx) {
    BOOL result;
    HANDLE process;
    HANDLE thread;
    HMODULE hModule;
    ULONG frame;
    DWORD64 displacement;
    DWORD disp;

#if defined(_M_AMD64)
    STACKFRAME64 stack;
    memset(&stack, 0, sizeof(STACKFRAME64));
#else
    STACKFRAME stack;
    memset(&stack, 0, sizeof(STACKFRAME));
    stack.AddrPC.Offset = (*ctx).Eip;
    stack.AddrPC.Mode = AddrModeFlat;
    stack.AddrStack.Offset = (*ctx).Esp;
    stack.AddrStack.Mode = AddrModeFlat;
    stack.AddrFrame.Offset = (*ctx).Ebp;
    stack.AddrFrame.Mode = AddrModeFlat;
#endif

    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME + sizeof(TCHAR)];
    char module[512];

    PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;

    CONTEXT ctx2;
    memcpy(&ctx2, ctx, sizeof(CONTEXT));

    PrintRegisters(&ctx2);

    process = GetCurrentProcess();
    thread = GetCurrentThread();
    SymInitialize(process, nullptr, true);


    constexpr DWORD machineType =
#if defined(_M_AMD64)
        IMAGE_FILE_MACHINE_AMD64;
#else
        IMAGE_FILE_MACHINE_I386;
#endif

    displacement = 0;
    for (frame = 0;; frame++) {
        result = StackWalk(machineType, process, thread, &stack, &ctx2, nullptr, SymFunctionTableAccess,
            SymGetModuleBase, nullptr);
        if (!result) {
            break;
        }
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen = MAX_SYM_NAME;
        SymFromAddr(process, (ULONG64)stack.AddrPC.Offset, &displacement, symbol);
#if defined(_M_AMD64)
        IMAGEHLP_LINE64 line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
#else
        IMAGEHLP_LINE line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
#endif
        if (SymGetLineFromAddr(process, stack.AddrPC.Offset, &disp, &line)) {
            SPDLOG_CRITICAL("{} in {}: line: {}: ", symbol->Name, line.FileName, line.LineNumber);
        }
        else {
            SPDLOG_CRITICAL("at {}, addr 0x{}", symbol->Name, symbol->Address);
            hModule = nullptr;
            GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCTSTR)(stack.AddrPC.Offset), &hModule);

            if (hModule != nullptr) {
                GetModuleFileNameA(hModule, module, sizeof(module));
            }
            SPDLOG_CRITICAL("In {}\n", module);
        }
    }
    spdlog::shutdown();
}

extern "C" int seh_filter(EXCEPTION_POINTERS * ex) {
    SPDLOG_CRITICAL("EXCEPTION 0x{:X} occured\n", ex->ExceptionRecord->ExceptionCode);
    printStack(ex->ContextRecord);
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif
