#include "CrashHandler.h"
#include "Utils/StringHelper.h"

#if __has_include(<unistd.h>)
#define HAS_POSIX 1
#else
#define HAS_POSIX 0
#endif

#include <array>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#if HAS_POSIX == 1
#include <csignal>
#include <cxxabi.h>  // for __cxa_demangle
#include <dlfcn.h>   // for dladdr
#include <execinfo.h>
#include <unistd.h>
#elif defined(_MSC_VER)
#include <Windows.h>
#include <DbgHelp.h>

#include <inttypes.h>

#pragma comment(lib, "Dbghelp.lib")
#endif

// Feel free to add more crash messages.
static std::array<const char* const, 14> crashEasterEgg = {
	"\tYou've met with a terrible fate, haven't you?",
	"\tSEA BEARS FOAM. SLEEP BEARS DREAMS. \n\tBOTH END IN THE SAME WAY: CRASSSH!",
	"\tZAPD has fallen and cannot get up.",
	"\tIT'S A SECRET TO EVERYBODY. \n\tBut it shouldn't be, you'd better ask about it!",
	"\tI AM ERROR.",
	"\tGRUMBLE,GRUMBLE...",
	"\tDODONGO DISLIKES SMOKE \n\tAnd ZAPD dislikes whatever you fed it.",
	"\tMay the way of the Hero lead \n\tto the debugger.",
	"\tTHE WIND FISH SLUMBERS LONG... \n\tTHE HERO'S LIFE GONE... ",
	"\tSEA BEARS FOAM, SLEEP BEARS DREAMS. \n\tBOTH END IN THE SAME WAY CRASSSH!",
	"\tYou've met with a terrible fate, haven't you?",
	"\tMaster, I calculate a 100% probability that ZAPD has crashed. \n\tAdditionally, the "
	"batteries in your Wii Remote are nearly depleted.",
	"\t    CONGRATURATIONS!    \n"
	"\tAll Pages are displayed.\n"
	"\t       THANK YOU!       \n"
	"\t You are great debugger!",
	"\tRCP is HUNG UP!!\n"
	"\tOh! MY GOD!!",
};

#if HAS_POSIX == 1
void ErrorHandler(int sig)
{
	std::array<void*, 4096> arr;
	constexpr size_t nMaxFrames = arr.size();
	size_t size = backtrace(arr.data(), nMaxFrames);
	char** symbols = backtrace_symbols(arr.data(), nMaxFrames);

	fprintf(stderr, "\nZAPD crashed. (Signal: %i)\n", sig);

	srand(time(nullptr));
	auto easterIndex = rand() % crashEasterEgg.size();

	fprintf(stderr, "\n%s\n\n", crashEasterEgg[easterIndex]);

	fprintf(stderr, "Traceback:\n");
	for (size_t i = 1; i < size; i++)
	{
		Dl_info info;
		uint32_t gotAddress = dladdr(arr[i], &info);
		std::string functionName(symbols[i]);

		if (gotAddress != 0 && info.dli_sname != nullptr)
		{
			int32_t status;
			char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);
			const char* nameFound = info.dli_sname;

			if (status == 0)
			{
				nameFound = demangled;
			}

			functionName = StringHelper::Sprintf("%s (+0x%X)", nameFound,
			                                     (char*)arr[i] - (char*)info.dli_saddr);
			free(demangled);
		}

		fprintf(stderr, "%-3zd %s\n", i, functionName.c_str());
	}

	fprintf(stderr, "\n");

	free(symbols);
	exit(1);
}
#elif defined(_MSC_VER)

void printStack(CONTEXT* ctx)
{
	BOOL result;
	HANDLE process;
	HANDLE thread;
	HMODULE hModule;
	ULONG frame;
	DWORD64 displacement;
	DWORD disp;

	srand(time(nullptr));
	auto easterIndex = rand() % crashEasterEgg.size();

	fprintf(stderr, "\n%s\n\n", crashEasterEgg[easterIndex]);

#if defined(_M_AMD64)
	STACKFRAME64 stack;
	memset(&stack, 0, sizeof(STACKFRAME64));
#else
	STACKFRAME stack;
	memset(&stack, 0, sizeof(STACKFRAME));
#endif

	char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME + sizeof(TCHAR)];
	char module[512];

	PSYMBOL_INFO symbol = (PSYMBOL_INFO)buffer;

	CONTEXT ctx2;
	memcpy(&ctx2, ctx, sizeof(CONTEXT));

	process = GetCurrentProcess();
	thread = GetCurrentThread();
	SymInitialize(process, nullptr, TRUE);

	constexpr DWORD machineType =
#if defined(_M_AMD64)
		IMAGE_FILE_MACHINE_AMD64;
#else
		IMAGE_FILE_MACHINE_I386;
#endif

	displacement = 0;

	for (frame = 0;; frame++)
	{
		result = StackWalk(machineType, process, thread, &stack, &ctx2, nullptr,
		                   SymFunctionTableAccess, SymGetModuleBase, nullptr);
		if (!result)
		{
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
		if (SymGetLineFromAddr(process, stack.AddrPC.Offset, &disp, &line))
		{
			fprintf(stderr, "%u\t %s in %s: line: %lu: \n", frame, symbol->Name, line.FileName,
			        line.LineNumber);
		}

		else
		{
			fprintf(stderr, "%u\tat % s\n", frame, symbol->Name);
			hModule = nullptr;
			GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			                      GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			                  (LPCTSTR)(stack.AddrPC.Offset), &hModule);

			if (hModule != nullptr)
			{
				GetModuleFileNameA(hModule, module, 512 - 1);
			}
			fprintf(stderr, "%u\tIn %s\n", frame, module);
		}
	}
}

LONG seh_filter(_EXCEPTION_POINTERS* ex)
{
	fprintf(stderr, "EXCEPTION 0x%x occured\n", ex->ExceptionRecord->ExceptionCode);
	printStack(ex->ContextRecord);
	return EXCEPTION_EXECUTE_HANDLER;
}
#endif

void CrashHandler_Init()
{
#if HAS_POSIX == 1
	signal(SIGSEGV, ErrorHandler);
	signal(SIGABRT, ErrorHandler);
#elif defined(_MSC_VER)
	SetUnhandledExceptionFilter(seh_filter);
#else
	HANDLE_WARNING(WarningType::Always,
	               "tried to set error handler, but this ZAPD build lacks support for one", "");
#endif
}
