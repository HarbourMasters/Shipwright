#include "spdlog/spdlog.h"
#include "Utils/StringHelper.h"
#include "CrashHandler.h"
#include "Window.h"
#include "ResourceMgr.h"
#include "GameVersions.h"
#include "Cvar.h"

#define WRITE_VAR_LINE(buff, len, varName, varValue) \
    append_str(buff, len, varName);                  \
    append_line(buff, len, varValue);
#define WRITE_VAR(buff, len, varName, varValue) \
    append_str(buff, len, varName);             \
    append_str(buff, len, varValue);

extern "C" void DeinitOTR(void);

static CrashHandlerCallback sCallbackFunc;

static const char* GetGameVersionString() {
    if(Ship::Window::GetInstance()->GetResourceManager()->IsRunning()) {
        uint32_t version = Ship::Window::GetInstance()->GetResourceManager()->GetGameVersion();
        switch (version) {
            case OOT_NTSC_10:
                return "NTSC 1.0";
            case OOT_NTSC_11:
                return "NTSC 1.1";
            case OOT_NTSC_12:
                return "NTSC 1.2";
            case OOT_PAL_10:
                return "PAL 1.0";
            case OOT_PAL_11:
                return "PAL 1.1";
            case OOT_NTSC_JP_GC_CE:
                return "NTSC JP_GC_CE";
            case OOT_NTSC_JP_GC:
                return "NTSC_JP_GC";
            case OOT_NTSC_US_GC:
                return "NTSC_US_GC";
            case OOT_PAL_GC:
                return "PAL_GC";
            case OOT_NTSC_JP_MQ:
                return "NTSC_JP_MQ";
            case OOT_NTSC_US_MQ:
                return "NTSC_US_MQ";
            case OOT_PAL_MQ:
                return "PAL_MQ";
            case OOT_PAL_GC_DBG1:
                return "DBG_1";
            case OOT_PAL_GC_DBG2:
                return "DBG_2";
            case OOT_PAL_GC_MQ_DBG:
                return "MQ_DBG";
            case OOT_IQUE_TW:
                return "IQUE_TW";
            case OOT_IQUE_CN:
                return "IQUE_CN";
            case OOT_UNKNOWN:
                return "UNKNOWN";
        }

    }
    return "ResourceMGR not ready";
}


static void append_str(char* buf, size_t* len, const char* str) {
    while (*str != '\0')
        buf[(*len)++] = *str++;
}

static void append_line(char* buf, size_t* len, const char* str) {
    while (*str != '\0')
        buf[(*len)++] = *str++;
    buf[(*len)++] = '\n';
}


/**
 * @brief Prints common data relevant to the crash
 * 
 * @param buffer 
 */
static void CrashHandler_PrintCommon(char* buffer, size_t* curBufferPos) {
    WRITE_VAR_LINE(buffer, curBufferPos, "OTR Generated with: ", GetGameVersionString());
    if (sCallbackFunc != nullptr) {
        sCallbackFunc(buffer, curBufferPos);
    }


    SPDLOG_CRITICAL(buffer);
}

#if defined(__linux__)
#include <csignal>
#include <cstdio>
#include <cxxabi.h>  // for __cxa_demangle
#include <dlfcn.h>   // for dladdr
#include <execinfo.h>
#include <unistd.h>
#include <SDL.h>


static void PrintRegisters(ucontext_t* ctx, char* buffer, size_t* pos) {
    char regbuffer[128];
    append_line(buffer, pos, "Registers:");
#if defined(__x86_64__)
    snprintf(regbuffer, std::size(regbuffer), "RAX: 0x%016llX", ctx->uc_mcontext.gregs[REG_RAX]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RDI: 0x%016llX", ctx->uc_mcontext.gregs[REG_RDI]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RSI: 0x%016llX", ctx->uc_mcontext.gregs[REG_RSI]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RDX: 0x%016llX", ctx->uc_mcontext.gregs[REG_RDX]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RCX: 0x%016llX", ctx->uc_mcontext.gregs[REG_RCX]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R8:  0x%016llX", ctx->uc_mcontext.gregs[REG_R8]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R9:  0x%016llX", ctx->uc_mcontext.gregs[REG_R9]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R10: 0x%016llX", ctx->uc_mcontext.gregs[REG_R10]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R11: 0x%016llX", ctx->uc_mcontext.gregs[REG_R11]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RSP: 0x%016llX", ctx->uc_mcontext.gregs[REG_RSP]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RBX: 0x%016llX", ctx->uc_mcontext.gregs[REG_RBX]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RBP: 0x%016llX", ctx->uc_mcontext.gregs[REG_RBP]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R12: 0x%016llX", ctx->uc_mcontext.gregs[REG_R12]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R13: 0x%016llX", ctx->uc_mcontext.gregs[REG_R13]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R14: 0x%016llX", ctx->uc_mcontext.gregs[REG_R14]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "R15: 0x%016llX", ctx->uc_mcontext.gregs[REG_R15]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "RIP: 0x%016llX", ctx->uc_mcontext.gregs[REG_RIP]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer), "EFL: 0x%016llX", ctx->uc_mcontext.gregs[REG_EFL]);
    append_line(buffer, pos, regbuffer);
#else
    snprintf(regbuffer, std::size(regbuffer),"EDI: 0x%08lX", ctx->uc_mcontext.gregs[REG_EDI]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"ESI: 0x%08lX", ctx->uc_mcontext.gregs[REG_ESI]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"EBP: 0x%08lX", ctx->uc_mcontext.gregs[REG_EBP]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"ESP: 0x%08lX", ctx->uc_mcontext.gregs[REG_ESP]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"EBX: 0x%08lX", ctx->uc_mcontext.gregs[REG_EBX]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"EDX: 0x%08lX", ctx->uc_mcontext.gregs[REG_EDX]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"ECX: 0x%08lX", ctx->uc_mcontext.gregs[REG_ECX]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"EAX: 0x%08lX", ctx->uc_mcontext.gregs[REG_EAX]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"EIP: 0x%08lX", ctx->uc_mcontext.gregs[REG_EIP]);
    append_line(buffer, pos, regbuffer);
    snprintf(regbuffer, std::size(regbuffer),"EFL: 0x%08lX", ctx->uc_mcontext.gregs[REG_EFL]);
    append_line(buffer, pos, regbuffer);
#endif
}

static void ErrorHandler(int sig, siginfo_t* sigInfo, void* data) {
    constexpr size_t MAX_BUFF_SIZE = 32768;
    size_t curBufferPos = 0;
    char buffer[MAX_BUFF_SIZE];
    char intToCharBuffer[16];

    std::array<void*, 4096> arr;
    ucontext_t* ctx = static_cast<ucontext_t*>(data);
    constexpr size_t nMaxFrames = arr.size();
    size_t size = backtrace(arr.data(), nMaxFrames);
    char** symbols = backtrace_symbols(arr.data(), nMaxFrames);

    snprintf(intToCharBuffer, sizeof(intToCharBuffer), "Signal: %i", sig);
    append_line(buffer, &curBufferPos, intToCharBuffer);

    switch (sig) {
        case SIGILL:
            append_line(buffer, &curBufferPos, "ILLEGAL INSTRUCTION");
            break;
        case SIGABRT:
            append_line(buffer, &curBufferPos, "ABORT");
            break;
        case SIGFPE:
            append_line(buffer, &curBufferPos, "ERRONEUS ARITHEMETIC OPERATION");
            break;
        case SIGSEGV:
            append_line(buffer, &curBufferPos, "INVALID ACCESS TO STORAGE");
            break;
    }

    PrintRegisters(ctx, buffer, &curBufferPos);

    append_line(buffer, &curBufferPos, "Traceback:");
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
        snprintf(intToCharBuffer, sizeof(intToCharBuffer), "%i ", (int)i);
        WRITE_VAR_LINE(buffer, &curBufferPos, intToCharBuffer, functionName.c_str());
    }
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SoH has crashed", "SoH Has crashed. Please upload the logs to the support channel in discord.", nullptr);
    free(symbols);
    CrashHandler_PrintCommon(buffer, &curBufferPos);

    DeinitOTR();
    Ship::Window::GetInstance()->GetLogger()->flush();
    spdlog::shutdown();
    exit(1);
}

static void ShutdownHandler(int sig, siginfo_t* sigInfo, void* data) {
    DeinitOTR();
    exit(1);
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
    constexpr size_t MAX_BUFF_SIZE = 32768;
    size_t curBufferPos = 0;
    char buffer[MAX_BUFF_SIZE];

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
    SymInitialize(process, nullptr, true);


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
    CrashHandler_PrintCommon();
    Ship::Window::GetInstance()->GetLogger()->flush();
    spdlog::shutdown();
    DeinitOTR();
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

extern "C" void CrashHandler_Init(CrashHandlerCallback callback) {
    #if defined(__linux__)
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
    #elif _WIN32
    SetUnhandledExceptionFilter(seh_filter);
    #endif

    sCallbackFunc = callback;


}
