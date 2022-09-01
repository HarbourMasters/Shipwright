#include "spdlog/spdlog.h"
#include "Utils/StringHelper.h"
#include "CrashHandler.h"

#if defined(__linux__)
#include <csignal>
#include <cstdio>
#include <cxxabi.h>  // for __cxa_demangle
#include <dlfcn.h>   // for dladdr
#include <execinfo.h>
#include <unistd.h>

extern "C" void DeinitOTR(void);


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

static void ErrorHandler(int sig, siginfo_t* sigInfo, void* data)
{
	std::array<void*, 4096> arr;
	ucontext_t* ctx = static_cast<ucontext_t*>(data);
	constexpr size_t nMaxFrames = arr.size();
	size_t size = backtrace(arr.data(), nMaxFrames);
	char** symbols = backtrace_symbols(arr.data(), nMaxFrames);

	SPDLOG_CRITICAL("(Signal: {})\n", sig);

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

	SPDLOG_CRITICAL("Traceback:\n");
	for (size_t i = 1; i < size; i++)
	{
		Dl_info info;
		int gotAddress = dladdr(arr[i], &info);
		std::string functionName(symbols[i]);

		if (gotAddress != 0 && info.dli_sname != nullptr)
		{
            FILE* pipe;
			int32_t status;
			char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
			const char* nameFound = info.dli_sname;

			if (status == 0)
			{
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
	DeinitOTR();
	exit(1);
}

static void ShutdownHandler(int sig, siginfo_t* sigInfo, void* data) {
	DeinitOTR();
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
#endif
