#pragma once

#include <array>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Utils/vt.h"
#include "ZFile.h"

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#elif not defined(__GNUC__)
#define __PRETTY_FUNCTION__ __func__
#endif

// =======================================
/* Formatting macros */

// TODO: move this somewhere else so it can be used by other help
#define HELP_DT_INDT "  "

/* Macros for formatting warnings/errors */
#define VT_HILITE VT_BOLD_FGCOL(WHITE)
#define VT_WARN VT_BOLD_FGCOL(PURPLE)
#define VT_ERR VT_BOLD_FGCOL(RED)

#define HILITE(string) (VT_HILITE string VT_RST)
#define WARN_FMT(string) (VT_WARN string VT_RST)
#define ERR_FMT(string) (VT_ERR string VT_RST)

// Maybe make WARN_LF instead
// Currently 8 spaces
#define WARN_INDT "        "
// Currently 16 spaces
#define HANG_INDT "                "

// =======================================
/* Warning and error macros */
// TODO: better names

// General-purpose, plain style (only prints function,file,line in the preamble)
#define HANDLE_ERROR(warningType, header, body)                                                    \
	WarningHandler::Error_Plain(__FILE__, __LINE__, __PRETTY_FUNCTION__, warningType, header, body)
#define HANDLE_WARNING(warningType, header, body)                                                  \
	WarningHandler::Warning_Plain(__FILE__, __LINE__, __PRETTY_FUNCTION__, warningType, header,    \
	                              body)

// For processing XMLs or textures/blobs (preamble contains function,file,line; processed file)
#define HANDLE_ERROR_PROCESS(warningType, header, body)                                            \
	WarningHandler::Error_Process(__FILE__, __LINE__, __PRETTY_FUNCTION__, warningType, header,    \
	                              body)
#define HANDLE_WARNING_PROCESS(warningType, header, body)                                          \
	WarningHandler::Warning_Process(__FILE__, __LINE__, __PRETTY_FUNCTION__, warningType, header,  \
	                                body)

// For ZResource-related stuff (preamble contains function,file,line; processed file; extracted file
// and offset)
#define HANDLE_ERROR_RESOURCE(warningType, parent, resource, offset, header, body)                 \
	WarningHandler::Error_Resource(__FILE__, __LINE__, __PRETTY_FUNCTION__, warningType, parent,   \
	                               resource, offset, header, body)
#define HANDLE_WARNING_RESOURCE(warningType, parent, resource, offset, header, body)               \
	WarningHandler::Warning_Resource(__FILE__, __LINE__, __PRETTY_FUNCTION__, warningType, parent, \
	                                 resource, offset, header, body)

// =======================================

enum class WarningType
{
	Always,  // Warnings of this type are always printed, cannot be disabled.
	Deprecated,
	Unaccounted,
	MissingOffsets,
	Intersection,
	MissingAttribute,
	InvalidAttributeValue,
	UnknownAttribute,
	InvalidXML,
	InvalidJPEG,
	InvalidPNG,
	InvalidExtractedData,
	MissingSegment,
	HardcodedPointer,
	NotImplemented,
	Max,
};

enum class WarningLevel
{
	Off,
	Warn,
	Err,
};

class WarningHandler
{
public:
	static void ConstructTypeToInfoMap();

	static void Init(int argc, char* argv[]);

	static bool IsWarningEnabled(WarningType warnType);
	static bool WasElevatedToError(WarningType warnType);

	static void FunctionPreamble(const char* filename, int32_t line, const char* function);
	static void ProcessedFilePreamble();
	static void ExtractedFilePreamble(const ZFile* parent, const ZResource* res,
	                                  const uint32_t offset);
	static std::string ConstructMessage(std::string message, const std::string& header,
	                                    const std::string& body);

	[[noreturn]] static void PrintErrorAndThrow(const std::string& header, const std::string& body);
	static void PrintWarningBody(const std::string& header, const std::string& body);

	[[noreturn]] static void ErrorType(WarningType warnType, const std::string& header,
	                                   const std::string& body);
	[[noreturn]] static void Error_Plain(const char* filename, int32_t line, const char* function,
	                                     WarningType warnType, const std::string& header,
	                                     const std::string& body);
	[[noreturn]] static void Error_Process(const char* filename, int32_t line, const char* function,
	                                       WarningType warnType, const std::string& header,
	                                       const std::string& body);
	[[noreturn]] static void Error_Resource(const char* filename, int32_t line,
	                                        const char* function, WarningType warnType,
	                                        const ZFile* parent, const ZResource* res,
	                                        const uint32_t offset, const std::string& header,
	                                        const std::string& body);

	static void WarningTypeAndChooseEscalate(WarningType warnType, const std::string& header,
	                                         const std::string& body);

	static void Warning_Plain(const char* filename, int32_t line, const char* function,
	                          WarningType warnType, const std::string& header,
	                          const std::string& body);
	static void Warning_Process(const char* filename, int32_t line, const char* function,
	                            WarningType warnType, const std::string& header,
	                            const std::string& body);
	static void Warning_Resource(const char* filename, int32_t line, const char* function,
	                             WarningType warnType, const ZFile* parent, const ZResource* res,
	                             const uint32_t offset, const std::string& header,
	                             const std::string& body);

	static void PrintHelp();
	static void PrintWarningsDebugInfo();
};
