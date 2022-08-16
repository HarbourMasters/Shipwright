#include "spdlog/spdlog.h"
#include <csignal>
#include <cstdio>
#include <cxxabi.h>  // for __cxa_demangle
#include <dlfcn.h>   // for dladdr
#include <execinfo.h>
#include <unistd.h>

#include "Utils/StringHelper.h"

extern "C" void ErrorHandler(int sig)
{
	std::array<void*, 4096> arr;
	constexpr size_t nMaxFrames = arr.size();
	size_t size = backtrace(arr.data(), nMaxFrames);
	char** symbols = backtrace_symbols(arr.data(), nMaxFrames);

	SPDLOG_CRITICAL("(Signal: %i)\n", sig);

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