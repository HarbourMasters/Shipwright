/*****************************************************************************/
/* TLogHelper.cpp                         Copyright (c) Ladislav Zezula 2013 */
/*---------------------------------------------------------------------------*/
/* Helper class for reporting StormLib tests                                 */
/* This file should be included directly from StormTest.cpp using #include   */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 26.11.13  1.00  Lad  The first version of TLogHelper.cpp                  */
/*****************************************************************************/

//-----------------------------------------------------------------------------
// Definition of the TLogHelper class

class TLogHelper
{
    public:

    TLogHelper(const char * szNewMainTitle = NULL, const TCHAR * szNewSubTitle1 = NULL, const TCHAR * szNewSubTitle2 = NULL);
    ~TLogHelper();

#if defined(UNICODE) || defined(UNICODE)
    // TCHAR-based functions. They are only needed on UNICODE builds.
    // On ANSI builds is TCHAR = char, so we don't need them at all
    int  PrintWithClreol(const TCHAR * szFormat, va_list argList, bool bPrintPrefix, bool bPrintLastError, bool bPrintEndOfLine);
    void PrintProgress(const TCHAR * szFormat, ...);
    void PrintMessage(const TCHAR * szFormat, ...);
    int  PrintErrorVa(const TCHAR * szFormat, ...);
    int  PrintError(const TCHAR * szFormat, const TCHAR * szFileName = NULL);
#endif  // defined(UNICODE) || defined(UNICODE)

    // ANSI functions
    DWORD PrintWithClreol(const char * szFormat, va_list argList, bool bPrintPrefix, bool bPrintLastError, bool bPrintEndOfLine);
    void  PrintProgress(const char * szFormat, ...);
    void  PrintMessage(const char * szFormat, ...);
    DWORD PrintErrorVa(const char * szFormat, ...);
    DWORD PrintError(const char * szFormat, const char * szFileName = NULL);

    const char * UserString;
    unsigned int UserCount;
    unsigned int UserTotal;
    bool bDontPrintResult;

    protected:

#if defined(UNICODE) || defined(UNICODE)
    TCHAR * CopyFormatCharacter(TCHAR * szBuffer, const TCHAR *& szFormat);
#endif
    char * CopyFormatCharacter(char * szBuffer, const char *& szFormat);
    int  GetConsoleWidth();

    const char  * szMainTitle;                      // Title of the text (usually name)
    const TCHAR * szSubTitle1;                      // Title of the text (can be name of the tested file)
    const TCHAR * szSubTitle2;                      // Title of the text (can be name of the tested file)
    size_t nTextLength;                             // Length of the previous progress message
    bool bMessagePrinted;
};

//-----------------------------------------------------------------------------
// String replacements for format strings

#ifdef _MSC_VER
#define I64u_t _T("%I64u")
#define I64u_a "%I64u"
#define I64X_t _T("%I64X")
#define I64X_a "%I64X"
#else
#define I64u_t _T("%llu")
#define I64u_a "%llu"
#define I64X_t _T("%llX")
#define I64X_a "%llX"
#endif

//-----------------------------------------------------------------------------
// Constructor and destructor


TLogHelper::TLogHelper(const char * szNewMainTitle, const TCHAR * szNewSubTitle1, const TCHAR * szNewSubTitle2)
{
    TCHAR szMainTitleT[0x80];

    UserString = "";
    UserCount = 1;
    UserTotal = 1;

    // Fill the test line structure
    szMainTitle = szNewMainTitle;
    szSubTitle1 = szNewSubTitle1;
    szSubTitle2 = szNewSubTitle2;
    nTextLength = 0;
    bMessagePrinted = false;
    bDontPrintResult = false;

    // Copy the UNICODE main title
    StringCopy(szMainTitleT, _countof(szMainTitleT), szMainTitle);

    // Print the initial information
    if(szMainTitle != NULL)
    {
        if(szSubTitle1 != NULL && szSubTitle2 != NULL)
            _tprintf(_T("Running %s (%s+%s) ..."), szMainTitleT, szSubTitle1, szSubTitle2);
        else if(szSubTitle1 != NULL)
            _tprintf(_T("Running %s (%s) ..."), szMainTitleT, szSubTitle1);
        else
            _tprintf(_T("Running %s ..."), szMainTitleT);
    }
}

TLogHelper::~TLogHelper()
{
    const TCHAR * szSaveSubTitle1 = szSubTitle1;
    const TCHAR * szSaveSubTitle2 = szSubTitle2;
    TCHAR szSaveMainTitle[0x80];

    // Set both to NULL so they won't be printed
    StringCopy(szSaveMainTitle, _countof(szSaveMainTitle), szMainTitle);
    szSubTitle1 = NULL;
    szSubTitle2 = NULL;
    szMainTitle = NULL;

    // Print the final information
    if(szSaveMainTitle != NULL && bMessagePrinted == false)
    {
        if(bDontPrintResult == false)
        {
            if(szSaveSubTitle1 != NULL && szSaveSubTitle2 != NULL)
                PrintMessage(_T("%s (%s+%s) succeeded."), szSaveMainTitle, szSaveSubTitle1, szSaveSubTitle2);
            else if(szSaveSubTitle1 != NULL)
                PrintMessage(_T("%s (%s) succeeded."), szSaveMainTitle, szSaveSubTitle1);
            else
                PrintMessage(_T("%s succeeded."), szSaveMainTitle);
        }
        else
        {
            PrintProgress(" ");
            printf("\r");
        }
    }

#if defined(_MSC_VER) && defined(_DEBUG)
    if(_CrtDumpMemoryLeaks())
    {
        PrintMessage(_T("Memory leak(s) detected after %s.\n"), szSaveMainTitle);
    }
#endif  // _MSC_VER
}

//-----------------------------------------------------------------------------
// TCHAR-based functions. They are only needed on UNICODE builds.
// On ANSI builds is TCHAR = char, so we don't need them at all

#if defined(UNICODE) || defined(UNICODE)
int TLogHelper::PrintWithClreol(const TCHAR * szFormat, va_list argList, bool bPrintPrefix, bool bPrintLastError, bool bPrintEndOfLine)
{
    TCHAR szFormatBuff[0x200];
    TCHAR szMessage[0x200];
    TCHAR * szBuffer = szFormatBuff;
    int nRemainingWidth;
    int nConsoleWidth = GetConsoleWidth();
    int nLength = 0;
    int nError = GetLastError();

    // Always start the buffer with '\r'
    *szBuffer++ = '\r';

    // Print the prefix, if needed
    if(szMainTitle != NULL && bPrintPrefix)
    {
        while(szMainTitle[nLength] != 0)
            *szBuffer++ = szMainTitle[nLength++];

        *szBuffer++ = ':';
        *szBuffer++ = ' ';
    }

    // Copy the message format itself. Replace %s with "%s", unless it's (%s)
    if(szFormat != NULL)
    {
        while(szFormat[0] != 0)
        {
            szBuffer = CopyFormatCharacter(szBuffer, szFormat);
        }
    }

    // Append the last error
    if(bPrintLastError)
    {
        nLength = _stprintf(szBuffer, _T(" (error code: %u)"), nError);
        szBuffer += nLength;
    }

    // Create the result string
    szBuffer[0] = 0;
    nLength = _vstprintf(szMessage, szFormatBuff, argList);
    szBuffer = szMessage + nLength;

    // Shall we pad the string?
    if(nLength < nConsoleWidth)
    {
        // Calculate the remaining width
        nRemainingWidth = nConsoleWidth - nLength - 1;

        // Pad the string with spaces to fill it up to the end of the line
        for(int i = 0; i < nRemainingWidth; i++)
            *szBuffer++ = 0x20;
    }

    // Put the newline, if requested
    *szBuffer++ = bPrintEndOfLine ? '\n' : 0;
    *szBuffer = 0;

    // Remember if we printed a message
    if(bPrintEndOfLine)
        bMessagePrinted = true;

    // Spit out the text in one single printf
    _tprintf(_T("%s"), szMessage);
    return nError;
}

void TLogHelper::PrintProgress(const TCHAR * szFormat, ...)
{
    va_list argList;

    va_start(argList, szFormat);
    PrintWithClreol(szFormat, argList, true, false, false);
    va_end(argList);
}

void TLogHelper::PrintMessage(const TCHAR * szFormat, ...)
{
    va_list argList;

    va_start(argList, szFormat);
    PrintWithClreol(szFormat, argList, true, false, true);
    va_end(argList);
}

int TLogHelper::PrintErrorVa(const TCHAR * szFormat, ...)
{
    va_list argList;
    int nResult;

    va_start(argList, szFormat);
    nResult = PrintWithClreol(szFormat, argList, true, true, true);
    va_end(argList);

    return nResult;
}

int TLogHelper::PrintError(const TCHAR * szFormat, const TCHAR * szFileName)
{
    return PrintErrorVa(szFormat, szFileName);
}
#endif  // defined(UNICODE) || defined(UNICODE)

//-----------------------------------------------------------------------------
// ANSI functions

DWORD TLogHelper::PrintWithClreol(const char * szFormat, va_list argList, bool bPrintPrefix, bool bPrintLastError, bool bPrintEndOfLine)
{
    char szFormatBuff[0x200];
    char szMessage[0x200];
    char * szBuffer = szFormatBuff;
    int nRemainingWidth;
    int nConsoleWidth = GetConsoleWidth();
    int nLength = 0;
    DWORD dwErrCode = GetLastError();

    // Always start the buffer with '\r'
    *szBuffer++ = '\r';

    // Print the prefix, if needed
    if(szMainTitle != NULL && bPrintPrefix)
    {
        while(szMainTitle[nLength] != 0)
            *szBuffer++ = (char)szMainTitle[nLength++];

        *szBuffer++ = ':';
        *szBuffer++ = ' ';
    }

    // Copy the message format itself. Replace %s with "%s", unless it's (%s)
    if(szFormat != NULL)
    {
        while(szFormat[0] != 0)
        {
            szBuffer = CopyFormatCharacter(szBuffer, szFormat);
        }
    }

    // Append the last error
    if(bPrintLastError)
    {
        nLength = sprintf(szBuffer, " (error code: %u)", dwErrCode);
        szBuffer += nLength;
    }

    // Create the result string
    szBuffer[0] = 0;
    nLength = vsprintf(szMessage, szFormatBuff, argList);

    // Shall we pad the string?
    szBuffer = szMessage + nLength;
    if(nLength < nConsoleWidth)
    {
        // Calculate the remaining width
        nRemainingWidth = nConsoleWidth - nLength - 1;

        // Pad the string with spaces to fill it up to the end of the line
        for(int i = 0; i < nRemainingWidth; i++)
            *szBuffer++ = 0x20;
    }

    // Put the newline, if requested
    *szBuffer++ = bPrintEndOfLine ? '\n' : '\r';
    *szBuffer = 0;

    // Remember if we printed a message
    if(bPrintEndOfLine)
        bMessagePrinted = true;

    // Spit out the text in one single printf
    printf("%s", szMessage);
    return dwErrCode;
}

void TLogHelper::PrintProgress(const char * szFormat, ...)
{
    va_list argList;

    va_start(argList, szFormat);
    PrintWithClreol(szFormat, argList, true, false, false);
    va_end(argList);
}

void TLogHelper::PrintMessage(const char * szFormat, ...)
{
    va_list argList;

    va_start(argList, szFormat);
    PrintWithClreol(szFormat, argList, true, false, true);
    va_end(argList);
}

DWORD TLogHelper::PrintErrorVa(const char * szFormat, ...)
{
    va_list argList;
    DWORD dwErrCode;

    va_start(argList, szFormat);
    dwErrCode = PrintWithClreol(szFormat, argList, true, true, true);
    va_end(argList);

    return dwErrCode;
}

DWORD TLogHelper::PrintError(const char * szFormat, const char * szFileName)
{
    return PrintErrorVa(szFormat, szFileName);
}

//-----------------------------------------------------------------------------
// Protected functions

#ifdef _UNICODE
TCHAR * TLogHelper::CopyFormatCharacter(TCHAR * szBuffer, const TCHAR *& szFormat)
{
    static const TCHAR * szStringFormat = _T("\"%s\"");
    static const TCHAR * szUint64Format = I64u_t;

    // String format
    if(szFormat[0] == '%')
    {
        if(szFormat[1] == 's' && szFormat[2] != ')')
        {
            _tcscpy(szBuffer, szStringFormat);
            szFormat += 2;
            return szBuffer + _tcslen(szStringFormat);
        }

        // Replace %I64u with the proper platform-dependent suffix
        if(szFormat[1] == 'I' && szFormat[2] == '6' && szFormat[3] == '4' && szFormat[4] == 'u')
        {
            _tcscpy(szBuffer, szUint64Format);
            szFormat += 5;
            return szBuffer + _tcslen(szUint64Format);
        }
    }

    // Copy the character as-is
    *szBuffer++ = *szFormat++;
    return szBuffer;
}
#endif

char * TLogHelper::CopyFormatCharacter(char * szBuffer, const char *& szFormat)
{
    static const char * szStringFormat = "\"%s\"";
    static const char * szUint64Format = I64u_a;

    // String format
    if(szFormat[0] == '%')
    {
        if(szFormat[1] == 's' && szFormat[2] != ')')
        {
            strcpy(szBuffer, szStringFormat);
            szFormat += 2;
            return szBuffer + strlen(szStringFormat);
        }

        // Replace %I64u with the proper platform-dependent suffix
        if(szFormat[1] == 'I' && szFormat[2] == '6' && szFormat[3] == '4' && szFormat[4] == 'u')
        {
            strcpy(szBuffer, szUint64Format);
            szFormat += 5;
            return szBuffer + strlen(szUint64Format);
        }
    }

    // Copy the character as-is
    *szBuffer++ = *szFormat++;
    return szBuffer;
}

int TLogHelper::GetConsoleWidth()
{
#ifdef STORMLIB_WINDOWS

    CONSOLE_SCREEN_BUFFER_INFO ScreenInfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ScreenInfo);
    return (int)(ScreenInfo.srWindow.Right - ScreenInfo.srWindow.Left);

#else

    // On non-Windows platforms, we assume that width of the console line
    // is 80 characters
    return 120;

#endif
}
