#include "spdlog/spdlog.h"
#include "Utils/StringHelper.h"

#if defined(_linux_)
#include <csignal>
#include <cstdio>
#include <cxxabi.h>  // for __cxa_demangle
#include <dlfcn.h>   // for dladdr
#include <execinfo.h>
#include <unistd.h>


static void PrintRegisters(ucontext_t* ctx) {
#if defined(__x86_64__)
	SPDLOG_CRITICAL("Registers:");
	SPDLOG_CRITICAL("RAX: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_RAX]));
	SPDLOG_CRITICAL("RDI: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_RDI]));
	SPDLOG_CRITICAL("RSI: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_RSI]));
	SPDLOG_CRITICAL("RDX: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_RDX]));
	SPDLOG_CRITICAL("RCX: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_RCX]));
	SPDLOG_CRITICAL("R8 : {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_R8]));
	SPDLOG_CRITICAL("R9 : {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_R9]));
	SPDLOG_CRITICAL("R10: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_R10]));
	SPDLOG_CRITICAL("R11: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_R11]));
	SPDLOG_CRITICAL("RSP: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_RSP]));
	SPDLOG_CRITICAL("RBX: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_RBX]));
	SPDLOG_CRITICAL("RBP: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_RBP]));
	SPDLOG_CRITICAL("R12: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_R12]));
	SPDLOG_CRITICAL("R13: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_R13]));
	SPDLOG_CRITICAL("R14: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_R14]));
	SPDLOG_CRITICAL("R15: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_R15]));
	SPDLOG_CRITICAL("RIP: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_RIP]));
	SPDLOG_CRITICAL("EFLAGS: {} ", StringHelper::Sprintf("0x%016llX", ctx->uc_mcontext.gregs[REG_EFL]));
#endif
}

static void ErrorHandler(int sig, siginfo_t* sigInfo, void* data)
{
	std::array<void*, 4096> arr;
	ucontext_t* ctx = static_cast<ucontext_t*>(data);
	constexpr size_t nMaxFrames = arr.size();
	size_t size = backtrace(arr.data(), nMaxFrames);
	char** symbols = backtrace_symbols(arr.data(), nMaxFrames);

	SPDLOG_CRITICAL("(Signal: %i)\n", sig);

	PrintRegisters(ctx);

	SPDLOG_CRITICAL("Traceback:\n");
	for (size_t i = 1; i < size; i++)
	{
		Dl_info info;
		uint32_t gotAddress = dladdr(arr[i], &info);
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
	exit(1);
}

extern "C" void SetupHandlerLinux() {
	struct sigaction action;
	action.sa_flags = SA_SIGINFO;
	action.sa_sigaction = ErrorHandler;
	/*
	signal(SIGSEGV, ErrorHandler);
			signal(SIGABRT, ErrorHandler);*/
	sigaction(SIGSEGV, &action, nullptr);
	sigaction(SIGABRT, &action, nullptr);

}
#endif
