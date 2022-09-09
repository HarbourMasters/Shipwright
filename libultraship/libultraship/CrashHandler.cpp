#include "spdlog/spdlog.h"
#include "Utils/StringHelper.h"
#include "CrashHandler.h"
#include "Window.h"

#if defined(__linux__)
#include <csignal>
#include <cstdio>
#include <cxxabi.h>  // for __cxa_demangle
#include <dlfcn.h>   // for dladdr
#include <execinfo.h>
#include <unistd.h>



static void PrintRegisters(ucontext_t* ctx) {
    char regbuffer[1024];
    SPDLOG_CRITICAL("Registers:");
#if defined(__x86_64__)
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_RAX]);
    SPDLOG_CRITICAL("RAX: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_RDI]);
    SPDLOG_CRITICAL("RDI: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_RSI]);
    SPDLOG_CRITICAL("RSI: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_RDX]);
    SPDLOG_CRITICAL("RDX: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_RCX]);
    SPDLOG_CRITICAL("RCX: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_R8]);
    SPDLOG_CRITICAL("R8 : {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_R9]);
    SPDLOG_CRITICAL("R9 : {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_R10]);
    SPDLOG_CRITICAL("R10: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_R11]);
    SPDLOG_CRITICAL("R11: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_RSP]);
    SPDLOG_CRITICAL("RSP: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_RBX]);
    SPDLOG_CRITICAL("RBX: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_RBP]);
    SPDLOG_CRITICAL("RBP: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_R12]);
    SPDLOG_CRITICAL("R12: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_R13]);
    SPDLOG_CRITICAL("R13: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_R14]);
    SPDLOG_CRITICAL("R14: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_R15]);
    SPDLOG_CRITICAL("R15: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_RIP]);
    SPDLOG_CRITICAL("RIP: {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "0x%016llX", ctx->uc_mcontext.gregs[REG_EFL]);
    SPDLOG_CRITICAL("EFLAGS: {} ", regbuffer);
#else
    snprintf(regbuffer, std::size(regbuffer),"0x%08lX", ctx->uc_mcontext.gregs[REG_EDI]);
    SPDLOG_CRITICAL("EDI : {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"0x%08lX", ctx->uc_mcontext.gregs[REG_ESI]);
    SPDLOG_CRITICAL("ESI : {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"0x%08lX", ctx->uc_mcontext.gregs[REG_EBP]);
    SPDLOG_CRITICAL("EBP : {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"0x%08lX", ctx->uc_mcontext.gregs[REG_ESP]);
    SPDLOG_CRITICAL("ESP : {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"0x%08lX", ctx->uc_mcontext.gregs[REG_EBX]);
    SPDLOG_CRITICAL("EBX : {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"0x%08lX", ctx->uc_mcontext.gregs[REG_EDX]);
    SPDLOG_CRITICAL("EDX : {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"0x%08lX", ctx->uc_mcontext.gregs[REG_ECX]);
    SPDLOG_CRITICAL("ECX : {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"0x%08lX", ctx->uc_mcontext.gregs[REG_EAX]);
    SPDLOG_CRITICAL("EAX : {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"0x%08lX", ctx->uc_mcontext.gregs[REG_EIP]);
    SPDLOG_CRITICAL("EIP : {} ", regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"0x%08lX", ctx->uc_mcontext.gregs[REG_EFL]);
    SPDLOG_CRITICAL("EFL : {} ", regbuffer);
#endif
}

static void ErrorHandler(int sig, siginfo_t* sigInfo, void* data) {
    std::array<void*, 4096> arr;
    ucontext_t* ctx = static_cast<ucontext_t*>(data);
    constexpr size_t nMaxFrames = arr.size();
    size_t size = backtrace(arr.data(), nMaxFrames);
    char** symbols = backtrace_symbols(arr.data(), nMaxFrames);

    SPDLOG_CRITICAL("(Signal: {})", sig);

    switch (sig) {
        case SIGILL:
            SPDLOG_CRITICAL("ILLEGAL INSTRUCTION");
            break;
        case SIGABRT:
            SPDLOG_CRITICAL("ABORT");
            break;
        case SIGFPE:
            SPDLOG_CRITICAL("ERRONEUS ARITHEMETIC OPERATION");
            break;
        case SIGSEGV:
            SPDLOG_CRITICAL("INVALID ACCESS TO STORAGE");
            break;
    }

    PrintRegisters(ctx);

    SPDLOG_CRITICAL("Traceback:");
    for (size_t i = 1; i < size; i++) {
        Dl_info info;
        int gotAddress = dladdr(arr[i], &info);
        std::string functionName(symbols[i]);

        if (gotAddress != 0 && info.dli_sname != nullptr) {
            FILE* pipe;
            int32_t status;
            char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
            const char* nameFound = info.dli_sname;

            if (status == 0) {
                nameFound = demangled;
            }
            #if 0
            char command[256];
            char addrLine[128];
            snprintf(command, sizeof(command), "addr2line -e soh.elf %s + 0x%lX", nameFound, (uintptr_t)arr[i] - (uintptr_t)info.dli_saddr);
            pipe = popen(command, "r");
            fgets(addrLine, 128, pipe);
            #endif
            
            functionName = StringHelper::Sprintf("%s (+0x%X)", nameFound,
                                                 (char*)arr[i] - (char*)info.dli_saddr);
            free(demangled);
        }

        SPDLOG_CRITICAL("{} {}", i, functionName.c_str());
    }

    free(symbols);
    //DeinitOTR();
    exit(1);
}

static void ShutdownHandler(int sig, siginfo_t* sigInfo, void* data) {
    //DeinitOTR();
    exit(1);
}

extern "C" void SetupHandlerLinux() {
    struct sigaction action;
    struct sigaction shutdownAction;

    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = ErrorHandler;

    sigaction(SIGILL, &action, nullptr);
    sigaction(SIGABRT, &action, nullptr);
    sigaction(SIGFPE, &action, nullptr);
    sigaction(SIGSEGV, &action, nullptr);

    shutdownAction.sa_flags = SA_SIGINFO;
    shutdownAction.sa_sigaction = ShutdownHandler;
    sigaction(SIGINT, &shutdownAction, nullptr);
    sigaction(SIGTERM, &shutdownAction, nullptr);
    sigaction(SIGQUIT, &shutdownAction, nullptr);
    sigaction(SIGKILL, &shutdownAction, nullptr);
}
#elif _WIN32

#if defined(_WIN32) && !defined(_WIN64)
#define WINDOWS_32_BIT
#endif

static void PrintRegisters(CONTEXT* ctx) {
    SPDLOG_CRITICAL("Register dump");
    char regBuff[50];
#if defined(_M_AMD64)
    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->Rax);
    SPDLOG_CRITICAL("RAX: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->Rcx);
    SPDLOG_CRITICAL("RCX: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->Rdx);
    SPDLOG_CRITICAL("RDX: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->Rbx);
    SPDLOG_CRITICAL("RBX: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->Rsp);
    SPDLOG_CRITICAL("RSP: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->Rbp);
    SPDLOG_CRITICAL("RBP: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->Rsi);
    SPDLOG_CRITICAL("RSI: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->Rdi);
    SPDLOG_CRITICAL("RDI: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->R9);
    SPDLOG_CRITICAL("R9:  {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->R10);
    SPDLOG_CRITICAL("R10: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->R11);
    SPDLOG_CRITICAL("R11: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->R12);
    SPDLOG_CRITICAL("R12: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->R13);
    SPDLOG_CRITICAL("R13: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->R14);
    SPDLOG_CRITICAL("R14: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->R15);
    SPDLOG_CRITICAL("R15: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%016llX", ctx->Rip);
    SPDLOG_CRITICAL("RIP: {}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%08lX", ctx->EFlags);
    SPDLOG_CRITICAL("EFLAGS: {}", regBuff);
#elif WINDOWS_32_BIT
    snprintf(regBuff, std::size(regBuff), "0x%08lX", ctx->Edi);
    SPDLOG_CRITICAL("EDI: 0x{}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%08lX", ctx->Esi);
    SPDLOG_CRITICAL("ESI: 0x{}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%08lX", ctx->Ebx);
    SPDLOG_CRITICAL("EBX: 0x{}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%08lX", ctx->Ecx);
    SPDLOG_CRITICAL("ECX: 0x{}", ctx->Ecx);

    snprintf(regBuff, std::size(regBuff), "0x%08lX", ctx->Eax);
    SPDLOG_CRITICAL("EAX: 0x{}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%08lX", ctx->Ebp);
    SPDLOG_CRITICAL("EBP: 0x{}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%08lX", ctx->Esp);
    SPDLOG_CRITICAL("ESP: 0x{}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%08lX", ctx->EFlags);
    SPDLOG_CRITICAL("EFLAGS: 0x{}", regBuff);

    snprintf(regBuff, std::size(regBuff), "0x%08lX", ctx->Eip);
    SPDLOG_CRITICAL("EIP: 0x{}", regBuff);
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
#elif WINDOWS_32_BIT
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

    SymSetOptions(SYMOPT_NO_IMAGE_SEARCH | SYMOPT_IGNORE_IMAGEDIR);
    SymInitialize(process, "debug", true);

    constexpr DWORD machineType =
#if defined(_M_AMD64)
        IMAGE_FILE_MACHINE_AMD64;
#elif WINDOWS_32_BIT
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
#elif WINDOWS_32_BIT
        IMAGEHLP_LINE line;
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
#endif
        if (SymGetLineFromAddr(process, stack.AddrPC.Offset, &disp, &line)) {
            SPDLOG_CRITICAL("{} in {}: line: {}: ", symbol->Name, line.FileName, line.LineNumber);
        }
        else {
            char addrString[25];
            snprintf(addrString, std::size(addrString), "0x%016llX", symbol->Address);
            SPDLOG_CRITICAL("at {}, addr 0x{}", symbol->Name, addrString);
            hModule = nullptr;
            GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                (LPCTSTR)(stack.AddrPC.Offset), &hModule);

            if (hModule != nullptr) {
                GetModuleFileNameA(hModule, module, sizeof(module));
            }
            SPDLOG_CRITICAL("In {}", module);
        }
    }
    Ship::GlobalCtx2::GetInstance()->GetLogger()->flush();
    spdlog::shutdown();
}

extern "C" LONG seh_filter(struct _EXCEPTION_POINTERS* ex) {
    char exceptionString[20];
    
    snprintf(exceptionString, std::size(exceptionString), "0x%x", ex->ExceptionRecord->ExceptionCode);
    
    SPDLOG_CRITICAL("EXCEPTION {} occurred", exceptionString);
    printStack(ex->ContextRecord);
    MessageBox(nullptr, L"SoH Has crashed. Please upload the logs to the support channel in discord.", L"Crash", MB_OK | MB_ICONERROR);

    return EXCEPTION_EXECUTE_HANDLER;
}


#endif
