/*****************************************************************************/
/* StormTest.cpp                          Copyright (c) Ladislav Zezula 2003 */
/*---------------------------------------------------------------------------*/
/* Test module for StormLib                                                  */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 25.03.03  1.00  Lad  The first version of StormTest.cpp                   */
/*****************************************************************************/

#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_DEPRECATE
#define __INCLUDE_CRYPTOGRAPHY__
#define __STORMLIB_SELF__                   // Don't use StormLib.lib
#include <stdio.h>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include "../src/StormLib.h"
#include "../src/StormCommon.h"

#include "TLogHelper.cpp"                   // Helper class for showing test results

#ifdef _MSC_VER
#pragma warning(disable: 4505)              // 'XXX' : unreferenced local function has been removed
#pragma comment(lib, "winmm.lib")
#endif

#ifndef STORMLIB_WINDOWS
#include <dirent.h>
#endif

//------------------------------------------------------------------------------
// Local structures

#define TEST_FLAG_PROTECTED  0x01000000
#define TEST_FLAG_FILE_COUNT 0x00FFFFFF

typedef struct _TEST_INFO
{
    LPCTSTR szMpqName1;
    LPCTSTR szMpqName2;
    DWORD   dwFlags;
    LPCSTR  szFileName1;
    LPCSTR  szFileName2;
} TEST_INFO, *PTEST_INFO;

typedef struct _LINE_INFO
{
    LONG  nLinePos;
    DWORD nLineLen;
    const char * szLine;
} LINE_INFO, *PLINE_INFO;

//------------------------------------------------------------------------------
// Local variables

#ifdef STORMLIB_WINDOWS
#define WORK_PATH_ROOT _T("\\Multimedia\\MPQs")
static const TCHAR szListFileDir[] = { '1', '9', '9', '5', ' ', '-', ' ', 'T', 'e', 's', 't', ' ', 'M', 'P', 'Q', 's', '\\', 'l', 'i', 's', 't', 'f', 'i', 'l', 'e', 's', '-', (TCHAR)0x65B0, (TCHAR)0x5EFA, (TCHAR)0x6587, (TCHAR)0x4EF6, (TCHAR)0x5939, 0 };
#endif

#ifdef STORMLIB_LINUX
#define WORK_PATH_ROOT "/media/ladik/CascStorages/MPQs"
static const TCHAR szListFileDir[] = { '1', '9', '9', '5', ' ', '-', ' ', 'T', 'e', 's', 't', ' ', 'M', 'P', 'Q', 's', '\\', 'l', 'i', 's', 't', 'f', 'i', 'l', 'e', 's', '-', (TCHAR)0xe6, (TCHAR)0x96, (TCHAR)0xB0, (TCHAR)0xE5, (TCHAR)0xBB, (TCHAR)0xBA, (TCHAR)0xE6, (TCHAR)0x96, (TCHAR)0x87, (TCHAR)0xE4, (TCHAR)0xBB, (TCHAR)0xB6, (TCHAR)0xE5, (TCHAR)0xA4, (TCHAR)0xB9, 0 };
#endif

#ifdef STORMLIB_MAC
#define WORK_PATH_ROOT "/home/sam/StormLib/test"
static const TCHAR szListFileDir[] = { '1', '9', '9', '5', ' ', '-', ' ', 'T', 'e', 's', 't', ' ', 'M', 'P', 'Q', 's', '\\', 'l', 'i', 's', 't', 'f', 'i', 'l', 'e', 's', '-', (TCHAR)0xe6, (TCHAR)0x96, (TCHAR)0xB0, (TCHAR)0xE5, (TCHAR)0xBB, (TCHAR)0xBA, (TCHAR)0xE6, (TCHAR)0x96, (TCHAR)0x87, (TCHAR)0xE4, (TCHAR)0xBB, (TCHAR)0xB6, (TCHAR)0xE5, (TCHAR)0xA4, (TCHAR)0xB9, 0 };
#endif

#ifdef STORMLIB_HAIKU
#define WORK_PATH_ROOT "~/StormLib/test"
static const TCHAR szListFileDir[] = { '1', '9', '9', '5', ' ', '-', ' ', 'T', 'e', 's', 't', ' ', 'M', 'P', 'Q', 's', '\\', 'l', 'i', 's', 't', 'f', 'i', 'l', 'e', 's', '-', (TCHAR)0xe6, (TCHAR)0x96, (TCHAR)0xB0, (TCHAR)0xE5, (TCHAR)0xBB, (TCHAR)0xBA, (TCHAR)0xE6, (TCHAR)0x96, (TCHAR)0x87, (TCHAR)0xE4, (TCHAR)0xBB, (TCHAR)0xB6, (TCHAR)0xE5, (TCHAR)0xA4, (TCHAR)0xB9, 0 };
#endif

// Global for the work MPQ
static LPCTSTR szMpqSubDir   = _T("1995 - Test MPQs");
static LPCTSTR szMpqPatchDir = _T("1995 - Test MPQs\\patches");

typedef DWORD (*FIND_FILE_CALLBACK)(LPCTSTR szFullPath);

#define ERROR_UNDETERMINED_RESULT 0xC000FFFF

//-----------------------------------------------------------------------------
// Testing data

static DWORD AddFlags[] =
{
//  Compression          Encryption             Fixed key           Single Unit            Sector CRC
    0                 |  0                   |  0                 | 0                    | 0,
    0                 |  MPQ_FILE_ENCRYPTED  |  0                 | 0                    | 0,
    0                 |  MPQ_FILE_ENCRYPTED  |  MPQ_FILE_FIX_KEY  | 0                    | 0,
    0                 |  0                   |  0                 | MPQ_FILE_SINGLE_UNIT | 0,
    0                 |  MPQ_FILE_ENCRYPTED  |  0                 | MPQ_FILE_SINGLE_UNIT | 0,
    0                 |  MPQ_FILE_ENCRYPTED  |  MPQ_FILE_FIX_KEY  | MPQ_FILE_SINGLE_UNIT | 0,
    MPQ_FILE_IMPLODE  |  0                   |  0                 | 0                    | 0,
    MPQ_FILE_IMPLODE  |  MPQ_FILE_ENCRYPTED  |  0                 | 0                    | 0,
    MPQ_FILE_IMPLODE  |  MPQ_FILE_ENCRYPTED  |  MPQ_FILE_FIX_KEY  | 0                    | 0,
    MPQ_FILE_IMPLODE  |  0                   |  0                 | MPQ_FILE_SINGLE_UNIT | 0,
    MPQ_FILE_IMPLODE  |  MPQ_FILE_ENCRYPTED  |  0                 | MPQ_FILE_SINGLE_UNIT | 0,
    MPQ_FILE_IMPLODE  |  MPQ_FILE_ENCRYPTED  |  MPQ_FILE_FIX_KEY  | MPQ_FILE_SINGLE_UNIT | 0,
    MPQ_FILE_IMPLODE  |  0                   |  0                 | 0                    | MPQ_FILE_SECTOR_CRC,
    MPQ_FILE_IMPLODE  |  MPQ_FILE_ENCRYPTED  |  0                 | 0                    | MPQ_FILE_SECTOR_CRC,
    MPQ_FILE_IMPLODE  |  MPQ_FILE_ENCRYPTED  |  MPQ_FILE_FIX_KEY  | 0                    | MPQ_FILE_SECTOR_CRC,
    MPQ_FILE_COMPRESS |  0                   |  0                 | 0                    | 0,
    MPQ_FILE_COMPRESS |  MPQ_FILE_ENCRYPTED  |  0                 | 0                    | 0,
    MPQ_FILE_COMPRESS |  MPQ_FILE_ENCRYPTED  |  MPQ_FILE_FIX_KEY  | 0                    | 0,
    MPQ_FILE_COMPRESS |  0                   |  0                 | MPQ_FILE_SINGLE_UNIT | 0,
    MPQ_FILE_COMPRESS |  MPQ_FILE_ENCRYPTED  |  0                 | MPQ_FILE_SINGLE_UNIT | 0,
    MPQ_FILE_COMPRESS |  MPQ_FILE_ENCRYPTED  |  MPQ_FILE_FIX_KEY  | MPQ_FILE_SINGLE_UNIT | 0,
    MPQ_FILE_COMPRESS |  0                   |  0                 | 0                    | MPQ_FILE_SECTOR_CRC,
    MPQ_FILE_COMPRESS |  MPQ_FILE_ENCRYPTED  |  0                 | 0                    | MPQ_FILE_SECTOR_CRC,
    MPQ_FILE_COMPRESS |  MPQ_FILE_ENCRYPTED  |  MPQ_FILE_FIX_KEY  | 0                    | MPQ_FILE_SECTOR_CRC,
    0xFFFFFFFF
};

static DWORD WaveCompressions[] =
{
    MPQ_COMPRESSION_ADPCM_MONO | MPQ_COMPRESSION_HUFFMANN,
    MPQ_COMPRESSION_ADPCM_STEREO | MPQ_COMPRESSION_HUFFMANN,
    MPQ_COMPRESSION_PKWARE,
    MPQ_COMPRESSION_ZLIB,
    MPQ_COMPRESSION_BZIP2
};

static const wchar_t szUnicodeName1[] = {   // Czech
    0x010C, 0x0065, 0x0073, 0x006B, 0x00FD, _T('.'), _T('m'), _T('p'), _T('q'), 0
};

static const wchar_t szUnicodeName2[] = {   // Russian
    0x0420, 0x0443, 0x0441, 0x0441, 0x043A, 0x0438, 0x0439, _T('.'), _T('m'), _T('p'), _T('q'), 0
};

static const wchar_t szUnicodeName3[] = {   // Greek
    0x03B5, 0x03BB, 0x03BB, 0x03B7, 0x03BD, 0x03B9, 0x03BA, 0x03AC, _T('.'), _T('m'), _T('p'), _T('q'), 0
};

static const wchar_t szUnicodeName4[] = {   // Chinese
    0x65E5, 0x672C, 0x8A9E, _T('.'), _T('m'), _T('p'), _T('q'), 0
};

static const wchar_t szUnicodeName5[] = {   // Japanese
    0x7B80, 0x4F53, 0x4E2D, 0x6587, _T('.'), _T('m'), _T('p'), _T('q'), 0
};

static const wchar_t szUnicodeName6[] = {   // Arabic
    0x0627, 0x0644, 0x0639, 0x0639, 0x0631, 0x0628, 0x064A, 0x0629, _T('.'), _T('m'), _T('p'), _T('q'), 0
};

static LPCTSTR PatchList_StarCraft[] =
{
    _T("MPQ_1998_v1_StarCraft.mpq"),
    _T("s1-1998-BroodWar.mpq"),
    NULL
};

static LPCTSTR PatchList_WoW_OldWorld13286[] =
{
    _T("MPQ_2012_v4_OldWorld.MPQ"),
    _T("wow-update-oldworld-13154.MPQ"),
    _T("wow-update-oldworld-13286.MPQ"),
    NULL
};

static LPCTSTR PatchList_WoW_15050[] =
{
    _T("MPQ_2013_v4_world.MPQ"),
    _T("wow-update-13164.MPQ"),
    _T("wow-update-13205.MPQ"),
    _T("wow-update-13287.MPQ"),
    _T("wow-update-13329.MPQ"),
    _T("wow-update-13596.MPQ"),
    _T("wow-update-13623.MPQ"),
    _T("wow-update-base-13914.MPQ"),
    _T("wow-update-base-14007.MPQ"),
    _T("wow-update-base-14333.MPQ"),
    _T("wow-update-base-14480.MPQ"),
    _T("wow-update-base-14545.MPQ"),
    _T("wow-update-base-14946.MPQ"),
    _T("wow-update-base-15005.MPQ"),
    _T("wow-update-base-15050.MPQ"),
    NULL
};

static LPCTSTR PatchList_WoW_16965[] =
{
    _T("MPQ_2013_v4_locale-enGB.MPQ"),
    _T("wow-update-enGB-16016.MPQ"),
    _T("wow-update-enGB-16048.MPQ"),
    _T("wow-update-enGB-16057.MPQ"),
    _T("wow-update-enGB-16309.MPQ"),
    _T("wow-update-enGB-16357.MPQ"),
    _T("wow-update-enGB-16516.MPQ"),
    _T("wow-update-enGB-16650.MPQ"),
    _T("wow-update-enGB-16844.MPQ"),
    _T("wow-update-enGB-16965.MPQ"),
    NULL
};

static LPCTSTR PatchList_SC2_32283[] =
{
    _T("MPQ_2013_v4_Base1.SC2Data"),
    _T("s2-update-base-23258.MPQ"),
    _T("s2-update-base-24540.MPQ"),
    _T("s2-update-base-26147.MPQ"),
    _T("s2-update-base-28522.MPQ"),
    _T("s2-update-base-30508.MPQ"),
    _T("s2-update-base-32283.MPQ"),
    NULL
};

static LPCTSTR PatchList_SC2_34644[] =
{
    _T("MPQ_2013_v4_Base1.SC2Data"),
    _T("s2-update-base-23258.MPQ"),
    _T("s2-update-base-24540.MPQ"),
    _T("s2-update-base-26147.MPQ"),
    _T("s2-update-base-28522.MPQ"),
    _T("s2-update-base-32384.MPQ"),
    _T("s2-update-base-34644.MPQ"),
    NULL
};

static LPCTSTR PatchList_SC2_34644_Maps[] =
{
    _T("MPQ_2013_v4_Base3.SC2Maps"),
    _T("s2-update-base-23258.MPQ"),
    _T("s2-update-base-24540.MPQ"),
    _T("s2-update-base-26147.MPQ"),
    _T("s2-update-base-28522.MPQ"),
    _T("s2-update-base-32384.MPQ"),
    _T("s2-update-base-34644.MPQ"),
    NULL
};

static LPCTSTR PatchList_SC2_32283_enGB[] =
{
    _T("MPQ_2013_v4_Mods#Core.SC2Mod#enGB.SC2Assets"),
    _T("s2-update-enGB-23258.MPQ"),
    _T("s2-update-enGB-24540.MPQ"),
    _T("s2-update-enGB-26147.MPQ"),
    _T("s2-update-enGB-28522.MPQ"),
    _T("s2-update-enGB-30508.MPQ"),
    _T("s2-update-enGB-32283.MPQ"),
    NULL
};

static LPCTSTR PatchList_SC2_36281_enGB[] =
{
    _T("MPQ_2013_v4_Mods#Liberty.SC2Mod#enGB.SC2Data"),
    _T("s2-update-enGB-23258.MPQ"),
    _T("s2-update-enGB-24540.MPQ"),
    _T("s2-update-enGB-26147.MPQ"),
    _T("s2-update-enGB-28522.MPQ"),
    _T("s2-update-enGB-32384.MPQ"),
    _T("s2-update-enGB-34644.MPQ"),
    _T("s2-update-enGB-36281.MPQ"),
    NULL
};

static LPCTSTR PatchList_HS_3604_enGB[] =
{
    _T("MPQ_2014_v4_base-Win.MPQ"),
    _T("hs-0-3604-Win-final.MPQ"),
    NULL
};

static LPCTSTR PatchList_HS_6898_enGB[] =
{
    _T("MPQ_2014_v4_base-Win.MPQ"),
    _T("hs-0-5314-Win-final.MPQ"),
    _T("hs-5314-5435-Win-final.MPQ"),
    _T("hs-5435-5506-Win-final.MPQ"),
    _T("hs-5506-5834-Win-final.MPQ"),
    _T("hs-5834-6024-Win-final.MPQ"),
    _T("hs-6024-6141-Win-final.MPQ"),
    _T("hs-6141-6187-Win-final.MPQ"),
    _T("hs-6187-6284-Win-final.MPQ"),
    _T("hs-6284-6485-Win-final.MPQ"),
    _T("hs-6485-6898-Win-final.MPQ"),
    NULL
};

//-----------------------------------------------------------------------------
// Local file functions

// Definition of the path separator
#ifdef STORMLIB_WINDOWS
static LPCTSTR g_szPathSeparator = _T("\\");
static const TCHAR PATH_SEPARATOR = _T('\\');       // Path separator for Windows platforms
#else
static LPCSTR g_szPathSeparator = "/";
static const TCHAR PATH_SEPARATOR = '/';            // Path separator for Non-Windows platforms
#endif

// This must be the directory where our test MPQs are stored.
// We also expect a subdirectory named
static TCHAR szMpqDirectory[MAX_PATH+1];
size_t cchMpqDirectory = 0;

template <typename XCHAR>
static bool IsFullPath(const XCHAR * szFileName)
{
#ifdef STORMLIB_WINDOWS
    if(('A' <= szFileName[0] && szFileName[0] <= 'Z') || ('a' <= szFileName[0] && szFileName[0] <= 'z'))
    {
        return (szFileName[1] == ':' && szFileName[2] == PATH_SEPARATOR);
    }
#endif

    szFileName = szFileName;
    return false;
}

static bool IsMpqExtension(LPCTSTR szFileName)
{
    LPCTSTR szExtension = _tcsrchr(szFileName, '.');

    if(szExtension != NULL)
    {
        if(!_tcsicmp(szExtension, _T(".mpq")))
            return true;
        if(!_tcsicmp(szExtension, _T(".w3m")))
            return true;
        if(!_tcsicmp(szExtension, _T(".w3x")))
            return true;
        if(!_tcsicmp(szExtension, _T(".asi")))
            return true;
        if(!_tcsicmp(szExtension, _T(".mpqe")))
            return true;
        if(!_tcsicmp(szExtension, _T(".part")))
            return true;
        if(!_tcsicmp(szExtension, _T(".sv")))
            return true;
        if(!_tcsicmp(szExtension, _T(".s2ma")))
            return true;
        if(!_tcsicmp(szExtension, _T(".SC2Map")))
            return true;
        if(!_tcsicmp(szExtension, _T(".SC2Mod")))
            return true;
        if(!_tcsicmp(szExtension, _T(".SC2Replay")))
            return true;
        if(!_tcsicmp(szExtension, _T(".0")))        // .MPQ.0
            return true;
//      if(!_tcsicmp(szExtension, ".link"))
//          return true;
    }

    return false;
}

static void BinaryFromString(LPCSTR szBinary, LPBYTE pbBuffer, DWORD cbBuffer)
{
    LPBYTE pbBufferEnd = pbBuffer + cbBuffer;
    char * szTemp;
    char szHexaDigit[4];

    while(szBinary[0] != 0 && pbBuffer < pbBufferEnd)
    {
        // Get the 2-byte chunk
        szHexaDigit[0] = szBinary[0];
        szHexaDigit[1] = szBinary[1];
        szHexaDigit[2] = 0;

        // Convert to integer
        *pbBuffer++ = (BYTE)strtoul(szHexaDigit, &szTemp, 16);
        szBinary += 2;
    }
}

static void AddStringBeforeExtension(char * szBuffer, LPCSTR szFileName, LPCSTR szExtraString)
{
    LPCSTR szExtension;
    size_t nLength;

    // Get the extension
    szExtension = strrchr(szFileName, '.');
    if(szExtension == NULL)
        szExtension = szFileName + strlen(szFileName);
    nLength = (size_t)(szExtension - szFileName);

    // Copy the part before extension
    memcpy(szBuffer, szFileName, nLength);
    szFileName += nLength;
    szBuffer += nLength;

    // Append the extra data
    if(szExtraString != NULL)
        strcpy(szBuffer, szExtraString);

    // Append the rest of the file name
    strcat(szBuffer, szFileName);
}

static bool CompareBlocks(LPBYTE pbBlock1, LPBYTE pbBlock2, DWORD dwLength, DWORD * pdwDifference)
{
    for(DWORD i = 0; i < dwLength; i++)
    {
        if(pbBlock1[i] != pbBlock2[i])
        {
            pdwDifference[0] = i;
            return false;
        }
    }

    return true;
}

static int GetPathSeparatorCount(LPCSTR szPath)
{
    int nSeparatorCount = 0;

    while(szPath[0] != 0)
    {
        if(szPath[0] == '\\' || szPath[0] == '/')
            nSeparatorCount++;
        szPath++;
    }

    return nSeparatorCount;
}

template <typename XCHAR>
static const XCHAR * FindNextPathPart(const XCHAR * szPath, size_t nPartCount)
{
    const XCHAR * szPathPart = szPath;

    while(szPath[0] != 0 && nPartCount > 0)
    {
        // Is there path separator?
        if(szPath[0] == '\\' || szPath[0] == '/')
        {
            szPathPart = szPath + 1;
            nPartCount--;
        }

        // Move to the next letter
        szPath++;
    }

    return szPathPart;
}

template <typename XCHAR>
size_t StringLength(const XCHAR * szString)
{
    size_t nLength;

    for(nLength = 0; szString[nLength] != 0; nLength++);

    return nLength;
}

template <typename XCHAR>
static const XCHAR * GetShortPlainName(const XCHAR * szFileName)
{
    const XCHAR * szPlainName = FindNextPathPart(szFileName, 1000);
    const XCHAR * szPlainEnd = szFileName + StringLength(szFileName);

    // If the name is still too long, cut it
    if((szPlainEnd - szPlainName) > 50)
        szPlainName = szPlainEnd - 50;

    return szPlainName;
}

static void CopyPathPart(char * szBuffer, LPCSTR szPath)
{
    while(szPath[0] != 0)
    {
        szBuffer[0] = (szPath[0] == '\\' || szPath[0] == '/') ? '/' : szPath[0];
        szBuffer++;
        szPath++;
    }

    *szBuffer = 0;
}

static bool CopyStringAndVerifyConversion(
    LPCTSTR szFoundFile,
    TCHAR * szBufferT,
    char * szBufferA,
    size_t cchMaxChars)
{
    // Convert the TCHAR name to ANSI name
    StringCopy(szBufferA, cchMaxChars, szFoundFile);
    StringCopy(szBufferT, cchMaxChars, szBufferA);

    // Compare both TCHAR strings
    return (_tcsicmp(szBufferT, szFoundFile) == 0) ? true : false;
}

static size_t ConvertSha1ToText(const unsigned char * sha1_digest, TCHAR * szSha1Text)
{
    LPCSTR szTable = "0123456789abcdef";

    for(size_t i = 0; i < SHA1_DIGEST_SIZE; i++)
    {
        *szSha1Text++ = szTable[(sha1_digest[0] >> 0x04)];
        *szSha1Text++ = szTable[(sha1_digest[0] & 0x0F)];
        sha1_digest++;
    }

    *szSha1Text = 0;
    return (SHA1_DIGEST_SIZE * 2);
}

static void CreateFullPathName(TCHAR * szBuffer, size_t cchBuffer, LPCTSTR szSubDir, LPCTSTR szNamePart1, LPCTSTR szNamePart2 = NULL)
{
    TCHAR * szSaveBuffer = szBuffer;
    size_t nPrefixLength = 0;
    size_t nLength;
    DWORD dwProvider = 0;
    bool bIsFullPath = false;
    char chSeparator = PATH_SEPARATOR;

    // Pre-initialize the buffer
    szBuffer[0] = 0;

    // Determine the path prefix
    if(szNamePart1 != NULL)
    {
        nPrefixLength = FileStream_Prefix(szNamePart1, &dwProvider);
        if((dwProvider & BASE_PROVIDER_MASK) == BASE_PROVIDER_HTTP)
        {
            bIsFullPath = true;
            chSeparator = '/';
        }
        else
            bIsFullPath = IsFullPath(szNamePart1 + nPrefixLength);
    }

    // Copy the MPQ prefix, if any
    if(nPrefixLength > 0)
    {
        StringCat(szBuffer, cchBuffer, szNamePart1);
        szBuffer[nPrefixLength] = 0;
        szSaveBuffer += nPrefixLength;
        szNamePart1 += nPrefixLength;
    }

    // If the given name is not a full path, copy the MPQ directory
    if(bIsFullPath == false)
    {
        // Copy the master MPQ directory
        StringCat(szBuffer, cchBuffer, szMpqDirectory);

        // Append the subdirectory, if any
        if(szSubDir != NULL && (nLength = _tcslen(szSubDir)) != 0)
        {
            // No leading or trailing separator are allowed
            assert(szSubDir[0] != '/' && szSubDir[0] != '\\');
            assert(szSubDir[nLength - 1] != '/' && szSubDir[nLength - 1] != '\\');

            // Append the subdirectory
            StringCat(szBuffer, cchBuffer, g_szPathSeparator);
            StringCat(szBuffer, cchBuffer, szSubDir);
        }
    }

    // Copy the file name, if any
    if(szNamePart1 != NULL && (nLength = _tcslen(szNamePart1)) != 0)
    {
        // Path separators are not allowed in the name part
        assert(szNamePart1[0] != '\\' && szNamePart1[0] != '/');
        assert(szNamePart1[nLength - 1] != '/' && szNamePart1[nLength - 1] != '\\');

        // Append file path separator and the name part
        if(bIsFullPath == false)
            StringCat(szBuffer, cchBuffer, g_szPathSeparator);
        StringCat(szBuffer, cchBuffer, szNamePart1);
    }

    // Append the second part of the name
    if(szNamePart2 != NULL && (nLength = _tcslen(szNamePart2)) != 0)
    {
        // Copy the file name
        StringCat(szBuffer, cchBuffer, szNamePart2);
    }

    // Normalize the path separators
    for(; szSaveBuffer[0] != 0; szSaveBuffer++)
    {
        szSaveBuffer[0] = (szSaveBuffer[0] != '/' && szSaveBuffer[0] != '\\') ? szSaveBuffer[0] : chSeparator;
    }
}

#ifdef _UNICODE
static void CreateFullPathName(char * szBuffer, size_t cchBuffer, LPCTSTR szSubDir, LPCTSTR szNamePart1, LPCTSTR szNamePart2 = NULL)
{
    TCHAR szFullPathT[MAX_PATH];

    CreateFullPathName(szFullPathT, _countof(szFullPathT), szSubDir, szNamePart1, szNamePart2);
    StringCopy(szBuffer, cchBuffer, szFullPathT);
}
#endif

static DWORD CalculateFileSha1(TLogHelper * pLogger, LPCTSTR szFullPath, TCHAR * szFileSha1)
{
    TFileStream * pStream;
    unsigned char sha1_digest[SHA1_DIGEST_SIZE];
    LPCTSTR szShortPlainName = GetShortPlainName(szFullPath);
    hash_state sha1_state;
    ULONGLONG ByteOffset = 0;
    ULONGLONG FileSize = 0;
    BYTE * pbFileBlock;
    DWORD cbBytesToRead;
    DWORD cbFileBlock = 0x100000;
    DWORD dwErrCode = ERROR_SUCCESS;

    // Notify the user
    pLogger->PrintProgress(_T("Hashing file %s"), szShortPlainName);
    szFileSha1[0] = 0;

    // Open the file to be verified
    pStream = FileStream_OpenFile(szFullPath, STREAM_FLAG_READ_ONLY);
    if(pStream != NULL)
    {
        // Retrieve the size of the file
        FileStream_GetSize(pStream, &FileSize);

        // Allocate the buffer for loading file parts
        pbFileBlock = STORM_ALLOC(BYTE, cbFileBlock);
        if(pbFileBlock != NULL)
        {
            // Initialize SHA1 calculation
            sha1_init(&sha1_state);

            // Calculate the SHA1 of the file
            while(ByteOffset < FileSize)
            {
                // Notify the user
                pLogger->PrintProgress(_T("Hashing file %s (%I64u of %I64u)"), szShortPlainName, ByteOffset, FileSize);

                // Load the file block
                cbBytesToRead = ((FileSize - ByteOffset) > cbFileBlock) ? cbFileBlock : (DWORD)(FileSize - ByteOffset);
                if(!FileStream_Read(pStream, &ByteOffset, pbFileBlock, cbBytesToRead))
                {
                    dwErrCode = GetLastError();
                    break;
                }

                // Add to SHA1
                sha1_process(&sha1_state, pbFileBlock, cbBytesToRead);
                ByteOffset += cbBytesToRead;
            }

            // Notify the user
            pLogger->PrintProgress(_T("Hashing file %s (%I64u of %I64u)"), szShortPlainName, ByteOffset, FileSize);

            // Finalize SHA1
            sha1_done(&sha1_state, sha1_digest);

            // Convert the SHA1 to ANSI text
            ConvertSha1ToText(sha1_digest, szFileSha1);
            STORM_FREE(pbFileBlock);
        }

        FileStream_Close(pStream);
    }

    // If we calculated something, return OK
    if(dwErrCode == ERROR_SUCCESS && szFileSha1[0] == 0)
        dwErrCode = ERROR_CAN_NOT_COMPLETE;
    return dwErrCode;
}

//-----------------------------------------------------------------------------
// Directory search

static HANDLE InitDirectorySearch(LPCTSTR szDirectory)
{
#ifdef STORMLIB_WINDOWS

    WIN32_FIND_DATA wf;
    HANDLE hFind;
    TCHAR szSearchMask[MAX_PATH];

    // Construct the directory mask
    _stprintf(szSearchMask, _T("%s\\*"), szDirectory);

    // Perform the search
    hFind = FindFirstFile(szSearchMask, &wf);
    return (hFind != INVALID_HANDLE_VALUE) ? hFind : NULL;

#endif

#if defined(STORMLIB_LINUX) || defined(STORMLIB_MAC)

    // Keep compilers happy
    return (HANDLE)opendir(szDirectory);

#endif
}

static bool SearchDirectory(HANDLE hFind, TCHAR * szDirEntry, size_t cchDirEntry, bool & IsDirectory)
{
#ifdef STORMLIB_WINDOWS

    WIN32_FIND_DATA wf;
    TCHAR szDirEntryT[MAX_PATH];
    char szDirEntryA[MAX_PATH];

    __SearchNextEntry:

    // Search for the hnext entry.
    if(FindNextFile(hFind, &wf))
    {
        // Verify if the directory entry is an UNICODE name that would be destroyed
        // by Unicode->ANSI->Unicode conversion
        if(CopyStringAndVerifyConversion(wf.cFileName, szDirEntryT, szDirEntryA, _countof(szDirEntryA)) == false)
            goto __SearchNextEntry;

        IsDirectory = (wf.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
        StringCopy(szDirEntry, cchDirEntry, wf.cFileName);
        return true;
    }

    return false;

#endif

#if defined(STORMLIB_LINUX) || defined(STORMLIB_MAC)

    struct dirent * directory_entry;

    directory_entry = readdir((DIR *)hFind);
    if(directory_entry != NULL)
    {
        IsDirectory = (directory_entry->d_type == DT_DIR) ? true : false;
        strcpy(szDirEntry, directory_entry->d_name);
        return true;
    }

    return false;

#endif
}

static void FreeDirectorySearch(HANDLE hFind)
{
#ifdef STORMLIB_WINDOWS
    FindClose(hFind);
#endif

#if defined(STORMLIB_LINUX) || defined(STORMLIB_MAC)
    closedir((DIR *)hFind);
#endif
}

static DWORD FindFilesInternal(FIND_FILE_CALLBACK pfnTest, TCHAR * szDirectory)
{
    TCHAR * szPlainName;
    HANDLE hFind;
    size_t nLength;
    TCHAR szDirEntry[MAX_PATH];
    bool IsDirectory = false;
    DWORD dwErrCode = ERROR_SUCCESS;

    if(szDirectory != NULL)
    {
        // Initiate directory search
        hFind = InitDirectorySearch(szDirectory);
        if(hFind != NULL)
        {
            // Append slash at the end of the directory name
            nLength = _tcslen(szDirectory);
            szDirectory[nLength++] = PATH_SEPARATOR;
            szPlainName = szDirectory + nLength;

            // Skip the first entry, since it's always "." or ".."
            while(SearchDirectory(hFind, szDirEntry, _countof(szDirEntry), IsDirectory) && dwErrCode == ERROR_SUCCESS)
            {
                // Copy the directory entry name to both names
                _tcscpy(szPlainName, szDirEntry);

                // Found a directory?
                if(IsDirectory)
                {
                    if(szDirEntry[0] != '.')
                    {
                        dwErrCode = FindFilesInternal(pfnTest, szDirectory);
                    }
                }
                else
                {
                    if(pfnTest != NULL)
                    {
                        dwErrCode = pfnTest(szDirectory);
                    }
                }
            }

            FreeDirectorySearch(hFind);
        }
    }

    // Free the path buffer, if any
    return dwErrCode;
}

static DWORD FindFiles(FIND_FILE_CALLBACK pfnFindFile, LPCTSTR szSubDirectory)
{
    TCHAR szWorkBuff[MAX_PATH];

    CreateFullPathName(szWorkBuff, _countof(szWorkBuff), szSubDirectory, NULL);
    return FindFilesInternal(pfnFindFile, szWorkBuff);
}

static DWORD InitializeMpqDirectory(TCHAR * argv[], int argc)
{
    TLogHelper Logger("InitWorkDir");
    TFileStream * pStream;
    TCHAR szFullPath[MAX_PATH];
    LPCTSTR szWhereFrom = _T("default");
    LPCTSTR szDirName = WORK_PATH_ROOT;

    // Retrieve the first argument
    if(argc > 1 && argv[1] != NULL)
    {
        // Check if it's a directory
        pStream = FileStream_OpenFile(argv[1], STREAM_FLAG_READ_ONLY);
        if(pStream == NULL)
        {
            szWhereFrom = _T("command line");
            szDirName = argv[1];
        }
        else
        {
            FileStream_Close(pStream);
        }
    }

    // Copy the name of the MPQ directory.
    StringCopy(szMpqDirectory, _countof(szMpqDirectory), szDirName);
    cchMpqDirectory = _tcslen(szMpqDirectory);

    // Cut trailing slashes and/or backslashes
    while((cchMpqDirectory > 0) && (szMpqDirectory[cchMpqDirectory - 1] == '/' || szMpqDirectory[cchMpqDirectory - 1] == '\\'))
        cchMpqDirectory--;
    szMpqDirectory[cchMpqDirectory] = 0;

    // Print the work directory info
    Logger.PrintMessage(_T("Work directory %s (%s)"), szMpqDirectory, szWhereFrom);

    // Verify if the work MPQ directory is writable
    CreateFullPathName(szFullPath, _countof(szFullPath), NULL, _T("TestFile.bin"));
    pStream = FileStream_CreateFile(szFullPath, 0);
    if(pStream == NULL)
        return Logger.PrintError(_T("MPQ subdirectory doesn't exist or is not writable"));

    // Close the stream
    FileStream_Close(pStream);
    _tremove(szFullPath);

    // Verify if the working directory exists and if there is a subdirectory with the file name
    CreateFullPathName(szFullPath, _countof(szFullPath), szListFileDir, _T("ListFile_Blizzard.txt"));
    pStream = FileStream_OpenFile(szFullPath, STREAM_FLAG_READ_ONLY);
    if(pStream == NULL)
        return Logger.PrintError(_T("The main listfile (%s) was not found. Check your paths"), GetShortPlainName(szFullPath));

    // Close the stream
    FileStream_Close(pStream);
    return ERROR_SUCCESS;
}

static DWORD GetFilePatchCount(TLogHelper * pLogger, HANDLE hMpq, LPCSTR szFileName)
{
    TCHAR * szPatchName;
    HANDLE hFile;
    TCHAR szPatchChain[0x400];
    DWORD dwErrCode = ERROR_SUCCESS;
    int nPatchCount = 0;

    // Open the MPQ file
    if(SFileOpenFileEx(hMpq, szFileName, 0, &hFile))
    {
        // Notify the user
        pLogger->PrintProgress("Verifying patch chain for %s ...", GetShortPlainName(szFileName));

        // Query the patch chain
        if(!SFileGetFileInfo(hFile, SFileInfoPatchChain, szPatchChain, sizeof(szPatchChain), NULL))
            dwErrCode = pLogger->PrintError("Failed to retrieve the patch chain on %s", szFileName);

        // Is there anything at all in the patch chain?
        if(dwErrCode == ERROR_SUCCESS && szPatchChain[0] == 0)
        {
            pLogger->PrintError("The patch chain for %s is empty", szFileName);
            dwErrCode = ERROR_FILE_CORRUPT;
        }

        // Now calculate the number of patches
        if(dwErrCode == ERROR_SUCCESS)
        {
            // Get the pointer to the patch
            szPatchName = szPatchChain;

            // Skip the base name
            for(;;)
            {
                // Skip the current name
                szPatchName = szPatchName + _tcslen(szPatchName) + 1;
                if(szPatchName[0] == 0)
                    break;

                // Increment number of patches
                nPatchCount++;
            }
        }

        SFileCloseFile(hFile);
    }
    else
    {
        pLogger->PrintError("Open failed: %s", szFileName);
    }

    return nPatchCount;
}

static DWORD VerifyFilePatchCount(TLogHelper * pLogger, HANDLE hMpq, LPCSTR szFileName, int nExpectedPatchCount)
{
    int nPatchCount = 0;

    // Retrieve the patch count
    pLogger->PrintProgress(_T("Verifying patch count for %s ..."), szFileName);
    nPatchCount = GetFilePatchCount(pLogger, hMpq, szFileName);

    // Check if there are any patches at all
    if(nExpectedPatchCount != 0 && nPatchCount == 0)
    {
        pLogger->PrintMessage("There are no patches for %s", szFileName);
        return ERROR_FILE_CORRUPT;
    }

    // Check if the number of patches fits
    if(nPatchCount != nExpectedPatchCount)
    {
        pLogger->PrintMessage("Unexpected number of patches for %s", szFileName);
        return ERROR_FILE_CORRUPT;
    }

    return ERROR_SUCCESS;
}

static DWORD CreateEmptyFile(TLogHelper * pLogger, LPCTSTR szPlainName, ULONGLONG FileSize, TCHAR * szBuffer)
{
    TFileStream * pStream;
    TCHAR szFullPath[MAX_PATH];

    // Notify the user
    pLogger->PrintProgress(_T("Creating empty file %s ..."), szPlainName);

    // Construct the full path and crete the file
    CreateFullPathName(szFullPath, _countof(szFullPath), NULL, szPlainName);
    pStream = FileStream_CreateFile(szFullPath, STREAM_PROVIDER_FLAT | BASE_PROVIDER_FILE);
    if(pStream == NULL)
        return pLogger->PrintError(_T("Failed to create file %s"), szBuffer);

    // Write the required size
    FileStream_SetSize(pStream, FileSize);
    FileStream_Close(pStream);

    // Give the caller the full file name
    if(szBuffer != NULL)
        _tcscpy(szBuffer, szFullPath);
    return ERROR_SUCCESS;
}

static DWORD VerifyFilePosition(
    TLogHelper * pLogger,
    TFileStream * pStream,
    ULONGLONG ExpectedPosition)
{
    ULONGLONG ByteOffset = 0;
    DWORD dwErrCode = ERROR_SUCCESS;

    // Retrieve the file position
    if(FileStream_GetPos(pStream, &ByteOffset))
    {
        if(ByteOffset != ExpectedPosition)
        {
            pLogger->PrintMessage(_T("The file position is different than expected (expected: ") I64u_t _T(", current: ") I64u_t, ExpectedPosition, ByteOffset);
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }
    else
    {
        dwErrCode = pLogger->PrintError(_T("Failed to retrieve the file offset"));
    }

    return dwErrCode;
}

static DWORD VerifyFileMpqHeader(TLogHelper * pLogger, TFileStream * pStream, ULONGLONG * pByteOffset)
{
    TMPQHeader Header;
    DWORD dwErrCode = ERROR_SUCCESS;

    memset(&Header, 0xFE, sizeof(TMPQHeader));
    if(FileStream_Read(pStream, pByteOffset, &Header, sizeof(TMPQHeader)))
    {
        if(Header.dwID != g_dwMpqSignature)
        {
            pLogger->PrintMessage(_T("Read error - the data is not a MPQ header"));
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }
    else
    {
        dwErrCode = pLogger->PrintError(_T("Failed to read the MPQ header"));
    }

    return dwErrCode;
}

static DWORD WriteMpqUserDataHeader(
    TLogHelper * pLogger,
    TFileStream * pStream,
    ULONGLONG ByteOffset,
    DWORD dwByteCount)
{
    TMPQUserData UserData;
    DWORD dwErrCode = ERROR_SUCCESS;

    // Notify the user
    pLogger->PrintProgress("Writing user data header...");

    // Fill the user data header
    UserData.dwID = ID_MPQ_USERDATA;
    UserData.cbUserDataSize = dwByteCount;
    UserData.dwHeaderOffs = (dwByteCount + sizeof(TMPQUserData));
    UserData.cbUserDataHeader = dwByteCount / 2;
    if(!FileStream_Write(pStream, &ByteOffset, &UserData, sizeof(TMPQUserData)))
        dwErrCode = GetLastError();
    return dwErrCode;
}

static DWORD WriteFileData(
    TLogHelper * pLogger,
    TFileStream * pStream,
    ULONGLONG ByteOffset,
    ULONGLONG ByteCount)
{
    ULONGLONG SaveByteCount = ByteCount;
    ULONGLONG BytesWritten = 0;
    LPBYTE pbDataBuffer;
    DWORD cbDataBuffer = 0x10000;
    DWORD dwErrCode = ERROR_SUCCESS;

    // Write some data
    pbDataBuffer = new BYTE[cbDataBuffer];
    if(pbDataBuffer != NULL)
    {
        memset(pbDataBuffer, 0, cbDataBuffer);
        strcpy((char *)pbDataBuffer, "This is a test data written to a file.");

        // Perform the write
        while(ByteCount > 0)
        {
            DWORD cbToWrite = (ByteCount > cbDataBuffer) ? cbDataBuffer : (DWORD)ByteCount;

            // Notify the user
            pLogger->PrintProgress("Writing file data (%I64u of %I64u) ...", BytesWritten, SaveByteCount);

            // Write the data
            if(!FileStream_Write(pStream, &ByteOffset, pbDataBuffer, cbToWrite))
            {
                dwErrCode = GetLastError();
                break;
            }

            BytesWritten += cbToWrite;
            ByteOffset += cbToWrite;
            ByteCount -= cbToWrite;
        }

        delete [] pbDataBuffer;
    }
    return dwErrCode;
}

static DWORD CopyFileData(
    TLogHelper * pLogger,
    TFileStream * pStream1,
    TFileStream * pStream2,
    ULONGLONG ByteOffset,
    ULONGLONG ByteCount)
{
    ULONGLONG BytesCopied = 0;
    ULONGLONG EndOffset = ByteOffset + ByteCount;
    LPBYTE pbCopyBuffer;
    DWORD BytesToRead;
    DWORD BlockLength = 0x100000;
    DWORD dwErrCode = ERROR_SUCCESS;

    // Allocate copy buffer
    pbCopyBuffer = STORM_ALLOC(BYTE, BlockLength);
    if(pbCopyBuffer != NULL)
    {
        while(ByteOffset < EndOffset)
        {
            // Read source
            BytesToRead = ((EndOffset - ByteOffset) > BlockLength) ? BlockLength : (DWORD)(EndOffset - ByteOffset);
            if(!FileStream_Read(pStream1, &ByteOffset, pbCopyBuffer, BytesToRead))
            {
                dwErrCode = GetLastError();
                break;
            }

            // Write to the destination file
            if(!FileStream_Write(pStream2, NULL, pbCopyBuffer, BytesToRead))
            {
                dwErrCode = GetLastError();
                break;
            }

            // Increment the byte counts
            BytesCopied += BytesToRead;
            ByteOffset += BytesToRead;

            // Notify the user
            pLogger->PrintProgress("Copying (%I64u of %I64u complete) ...", BytesCopied, ByteCount);
        }

        STORM_FREE(pbCopyBuffer);
    }

    return dwErrCode;
}

// Support function for copying file
static DWORD CreateFileCopy(
    TLogHelper * pLogger,
    LPCTSTR szPlainName,
    LPCTSTR szFileCopy,
    TCHAR * szBuffer = NULL,
    size_t cchBuffer = 0,
    ULONGLONG PreMpqDataSize = 0,
    ULONGLONG UserDataSize = 0)
{
    TFileStream * pStream1;             // Source file
    TFileStream * pStream2;             // Target file
    ULONGLONG ByteOffset = 0;
    ULONGLONG FileSize = 0;
    TCHAR szFileName1[MAX_PATH];
    TCHAR szFileName2[MAX_PATH];
    DWORD dwErrCode = ERROR_SUCCESS;

    // Notify the user
    szPlainName += FileStream_Prefix(szPlainName, NULL);
    pLogger->PrintProgress(_T("Creating copy of %s ..."), szPlainName);

    // Construct both file names. Check if they are not the same
    CreateFullPathName(szFileName1, _countof(szFileName1), szMpqSubDir, szPlainName);
    CreateFullPathName(szFileName2, _countof(szFileName2), NULL, szFileCopy + FileStream_Prefix(szFileCopy, NULL));
    if(!_tcsicmp(szFileName1, szFileName2))
    {
        pLogger->PrintError("Failed to create copy of MPQ (the copy name is the same like the original name)");
        return ERROR_CAN_NOT_COMPLETE;
    }

    // Open the source file
    pStream1 = FileStream_OpenFile(szFileName1, STREAM_FLAG_READ_ONLY);
    if(pStream1 == NULL)
    {
        pLogger->PrintError(_T("Failed to open the source file %s"), szFileName1);
        return ERROR_CAN_NOT_COMPLETE;
    }

    // Create the destination file
    pStream2 = FileStream_CreateFile(szFileName2, 0);
    if(pStream2 != NULL)
    {
        // If we should write some pre-MPQ data to the target file, do it
        if(PreMpqDataSize != 0)
        {
            dwErrCode = WriteFileData(pLogger, pStream2, ByteOffset, PreMpqDataSize);
            ByteOffset += PreMpqDataSize;
        }

        // If we should write some MPQ user data, write the header first
        if(UserDataSize != 0)
        {
            dwErrCode = WriteMpqUserDataHeader(pLogger, pStream2, ByteOffset, (DWORD)UserDataSize);
            ByteOffset += sizeof(TMPQUserData);

            dwErrCode = WriteFileData(pLogger, pStream2, ByteOffset, UserDataSize);
            ByteOffset += UserDataSize;
        }

        // Copy the file data from the source file to the destination file
        FileStream_GetSize(pStream1, &FileSize);
        if(FileSize != 0)
        {
            dwErrCode = CopyFileData(pLogger, pStream1, pStream2, 0, FileSize);
            ByteOffset += FileSize;
        }
        FileStream_Close(pStream2);
    }

    // Close the source file
    FileStream_Close(pStream1);

    // Create the full file name of the target file, including prefix
    if(szBuffer && cchBuffer)
        CreateFullPathName(szBuffer, cchBuffer, NULL, szFileCopy);

    // Report error, if any
    if(dwErrCode != ERROR_SUCCESS)
        pLogger->PrintError("Failed to create copy of MPQ");
    return dwErrCode;
}

static DWORD CreateMasterAndMirrorPaths(
    TLogHelper * pLogger,
    TCHAR * szMirrorPath,
    TCHAR * szMasterPath,
    LPCTSTR szMirrorName,
    LPCTSTR szMasterName,
    bool bCopyMirrorFile)
{
    TCHAR szCopyPath[MAX_PATH];
    DWORD dwErrCode = ERROR_SUCCESS;

    // Always delete the mirror file
    CreateFullPathName(szMasterPath, MAX_PATH, szMpqSubDir, szMasterName);
    CreateFullPathName(szCopyPath, _countof(szCopyPath), NULL, szMirrorName);
    _tremove(szCopyPath + FileStream_Prefix(szCopyPath, NULL));

    // Copy the mirrored file from the source to the work directory
    if(bCopyMirrorFile)
        dwErrCode = CreateFileCopy(pLogger, szMirrorName, szMirrorName);

    // Create the mirror*master path
    if(dwErrCode == ERROR_SUCCESS)
        _stprintf(szMirrorPath, _T("%s*%s"), szCopyPath, szMasterPath);

    return dwErrCode;
}

static void WINAPI AddFileCallback(void * pvUserData, DWORD dwBytesWritten, DWORD dwTotalBytes, bool bFinalCall)
{
    TLogHelper * pLogger = (TLogHelper *)pvUserData;

    // Keep compiler happy
    bFinalCall = bFinalCall;

    pLogger->PrintProgress("Adding file (%s) (%u of %u) (%u of %u) ...", pLogger->UserString,
                                                                         pLogger->UserCount,
                                                                         pLogger->UserTotal,
                                                                         dwBytesWritten,
                                                                         dwTotalBytes);
}

static void WINAPI CompactCallback(void * pvUserData, DWORD dwWork, ULONGLONG BytesDone, ULONGLONG TotalBytes)
{
    TLogHelper * pLogger = (TLogHelper *)pvUserData;
    LPCSTR szWork = NULL;

    switch(dwWork)
    {
        case CCB_CHECKING_FILES:
            szWork = "Checking files in archive";
            break;

        case CCB_CHECKING_HASH_TABLE:
            szWork = "Checking hash table";
            break;

        case CCB_COPYING_NON_MPQ_DATA:
            szWork = "Copying non-MPQ data";
            break;

        case CCB_COMPACTING_FILES:
            szWork = "Compacting files";
            break;

        case CCB_CLOSING_ARCHIVE:
            szWork = "Closing archive";
            break;
    }

    if(szWork != NULL)
    {
        if(pLogger != NULL)
            pLogger->PrintProgress("%s (%I64u of %I64u) ...", szWork, BytesDone, TotalBytes);
        else
            printf("%s (" I64u_a " of " I64u_a ") ...     \r", szWork, BytesDone, TotalBytes);
    }
}

//-----------------------------------------------------------------------------
// MPQ file utilities

#define SEARCH_FLAG_LOAD_FILES      0x00000001      // Test function should load all files in the MPQ
#define SEARCH_FLAG_HASH_FILES      0x00000002      // Test function should load all files in the MPQ
#define SEARCH_FLAG_PLAY_WAVES      0x00000004      // Play extracted WAVE files
#define SEARCH_FLAG_MOST_PATCHED    0x00000008      // Find the most patched file
#define SEARCH_FLAG_IGNORE_ERRORS   0x00000010      // Ignore files that failed to open

struct TFileData
{
    DWORD dwBlockIndex;
    DWORD dwFileSize;
    DWORD dwFlags;
    DWORD dwCrc32;
    BYTE FileData[1];
};

static bool CheckIfFileIsPresent(TLogHelper * pLogger, HANDLE hMpq, LPCSTR szFileName, bool bShouldExist)
{
    HANDLE hFile = NULL;

    if(SFileOpenFileEx(hMpq, szFileName, 0, &hFile))
    {
        if(bShouldExist == false)
            pLogger->PrintMessage("The file %s is present, but it should not be", szFileName);
        SFileCloseFile(hFile);
        return true;
    }
    else
    {
        if(bShouldExist)
            pLogger->PrintMessage("The file %s is not present, but it should be", szFileName);
        return false;
    }
}

static TFileData * LoadLocalFile(TLogHelper * pLogger, LPCTSTR szFileName, bool bMustSucceed)
{
    TFileStream * pStream;
    TFileData * pFileData = NULL;
    ULONGLONG FileSize = 0;
    size_t nAllocateBytes;

    // Notify the user
    if(pLogger != NULL)
        pLogger->PrintProgress("Loading local file ...");

    // Attempt to open the file
    pStream = FileStream_OpenFile(szFileName, STREAM_FLAG_READ_ONLY);
    if(pStream == NULL)
    {
        if(pLogger != NULL && bMustSucceed == true)
            pLogger->PrintError(_T("Open failed: %s"), szFileName);
        return NULL;
    }

    // Verify the size
    FileStream_GetSize(pStream, &FileSize);
    if((FileSize >> 0x20) == 0)
    {
        // Allocate space for the file
        nAllocateBytes = sizeof(TFileData) + (size_t)FileSize;
        pFileData = (TFileData *)STORM_ALLOC(BYTE, nAllocateBytes);
        if(pFileData != NULL)
        {
            // Make sure it;s properly zeroed
            memset(pFileData, 0, nAllocateBytes);
            pFileData->dwFileSize = (DWORD)FileSize;

            // Load to memory
            if(!FileStream_Read(pStream, NULL, pFileData->FileData, pFileData->dwFileSize))
            {
                STORM_FREE(pFileData);
                pFileData = NULL;
            }
        }
    }

    FileStream_Close(pStream);
    return pFileData;
}

static DWORD CompareTwoLocalFilesRR(
    TLogHelper * pLogger,
    TFileStream * pStream1,                         // Master file
    TFileStream * pStream2,                         // Mirror file
    int nIterations)                                // Number of iterations
{
    ULONGLONG RandomNumber = 0x12345678;            // We need pseudo-random number that will repeat each run of the program
    ULONGLONG RandomSeed;
    ULONGLONG ByteOffset;
    ULONGLONG FileSize1 = 1;
    ULONGLONG FileSize2 = 2;
    DWORD BytesToRead;
    DWORD Difference;
    LPBYTE pbBuffer1;
    LPBYTE pbBuffer2;
    DWORD cbBuffer = 0x100000;
    DWORD dwErrCode = ERROR_SUCCESS;

    // Compare file sizes
    FileStream_GetSize(pStream1, &FileSize1);
    FileStream_GetSize(pStream2, &FileSize2);
    if(FileSize1 != FileSize2)
    {
        pLogger->PrintMessage("The files have different size");
        return ERROR_CAN_NOT_COMPLETE;
    }

    // Allocate both buffers
    pbBuffer1 = STORM_ALLOC(BYTE, cbBuffer);
    pbBuffer2 = STORM_ALLOC(BYTE, cbBuffer);
    if(pbBuffer1 && pbBuffer2)
    {
        // Perform many random reads
        for(int i = 0; i < nIterations; i++)
        {
            // Generate psudo-random offsrt and data size
            ByteOffset = (RandomNumber % FileSize1);
            BytesToRead = (DWORD)(RandomNumber % cbBuffer);

            // Show the progress message
            pLogger->PrintProgress("Comparing file: Offset: " I64u_a ", Length: %u", ByteOffset, BytesToRead);

            // Only perform read if the byte offset is below
            if(ByteOffset < FileSize1)
            {
                if((ByteOffset + BytesToRead) > FileSize1)
                    BytesToRead = (DWORD)(FileSize1 - ByteOffset);

                memset(pbBuffer1, 0xEE, cbBuffer);
                memset(pbBuffer2, 0xAA, cbBuffer);

                FileStream_Read(pStream1, &ByteOffset, pbBuffer1, BytesToRead);
                FileStream_Read(pStream2, &ByteOffset, pbBuffer2, BytesToRead);

                if(!CompareBlocks(pbBuffer1, pbBuffer2, BytesToRead, &Difference))
                {
                    pLogger->PrintMessage("Difference at %u (Offset " I64X_a ", Length %X)", Difference, ByteOffset, BytesToRead);
                    dwErrCode = ERROR_FILE_CORRUPT;
                    break;
                }

                // Shuffle the random number
                memcpy(&RandomSeed, pbBuffer1, sizeof(RandomSeed));
                RandomNumber = ((RandomNumber >> 0x11) | (RandomNumber << 0x29)) ^ (RandomNumber + RandomSeed);
            }
        }
    }

    // Free both buffers
    if(pbBuffer2 != NULL)
        STORM_FREE(pbBuffer2);
    if(pbBuffer1 != NULL)
        STORM_FREE(pbBuffer1);
    return dwErrCode;
}

static TFileData * LoadMpqFile(TLogHelper * pLogger, HANDLE hMpq, LPCSTR szFileName, LCID lcLocale = 0, bool bIgnoreOpedwErrCodes = false)
{
    TFileData * pFileData = NULL;
    HANDLE hFile;
    DWORD dwFileSizeHi = 0xCCCCCCCC;
    DWORD dwFileSizeLo = 0;
    DWORD dwBytesRead;
    DWORD dwCrc32 = 0;
    DWORD dwErrCode = ERROR_SUCCESS;

    // Notify the user that we are loading a file from MPQ
    pLogger->PrintProgress("Loading file %s ...", GetShortPlainName(szFileName));

#if defined(_MSC_VER) && defined(_DEBUG)
//  if(!_stricmp(szFileName, "File00000733.wav"))
//      __debugbreak();
#endif

    // Make sure that we open the proper locale file
    SFileSetLocale(lcLocale);

    // Open the file from MPQ
    if(SFileOpenFileEx(hMpq, szFileName, 0, &hFile))
    {
        // Get the CRC32 of the file
        SFileGetFileInfo(hFile, SFileInfoCRC32, &dwCrc32, sizeof(dwCrc32), NULL);

        // Get the size of the file
        if(dwErrCode == ERROR_SUCCESS)
        {
            dwFileSizeLo = SFileGetFileSize(hFile, &dwFileSizeHi);
            if(dwFileSizeLo == SFILE_INVALID_SIZE || dwFileSizeHi != 0)
                dwErrCode = pLogger->PrintError("Failed to query the file size");
        }

        // Spazzler protector: Creates fake files with size of 0x7FFFE7CA
        if(dwErrCode == ERROR_SUCCESS)
        {
            if(dwFileSizeLo > 0x1FFFFFFF)
                dwErrCode = ERROR_FILE_CORRUPT;
        }

        // Allocate buffer for the file content
        if(dwErrCode == ERROR_SUCCESS)
        {
            pFileData = (TFileData *)STORM_ALLOC(BYTE, sizeof(TFileData) + dwFileSizeLo);
            if(pFileData == NULL)
            {
                pLogger->PrintError("Failed to allocate buffer for the file content");
                dwErrCode = ERROR_NOT_ENOUGH_MEMORY;
            }
        }

        // get the file index of the MPQ file
        if(dwErrCode == ERROR_SUCCESS)
        {
            // Store the file size
            memset(pFileData, 0, sizeof(TFileData) + dwFileSizeLo);
            pFileData->dwFileSize = dwFileSizeLo;
            pFileData->dwCrc32 = dwCrc32;

            // Retrieve the block index and file flags
            if(!SFileGetFileInfo(hFile, SFileInfoFileIndex, &pFileData->dwBlockIndex, sizeof(DWORD), NULL))
                dwErrCode = pLogger->PrintError("Failed retrieve the file index of %s", szFileName);
            if(!SFileGetFileInfo(hFile, SFileInfoFlags, &pFileData->dwFlags, sizeof(DWORD), NULL))
                dwErrCode = pLogger->PrintError("Failed retrieve the file flags of %s", szFileName);
        }

        // Load the entire file
        if(dwErrCode == ERROR_SUCCESS)
        {
			//if(!stricmp(szFileName, "replay.game.events"))
			//	__debugbreak();

            // Read the file data
            SFileReadFile(hFile, pFileData->FileData, dwFileSizeLo, &dwBytesRead, NULL);
            if(dwBytesRead != dwFileSizeLo)
                dwErrCode = ERROR_FILE_CORRUPT;
        }

        // If failed, free the buffer
        if(dwErrCode != ERROR_SUCCESS)
        {
            SetLastError(dwErrCode);
            if(pFileData != NULL)
                STORM_FREE(pFileData);
            pFileData = NULL;
        }

        SFileCloseFile(hFile);
    }
    else
    {
        if(bIgnoreOpedwErrCodes == false)
        {
            dwErrCode = pLogger->PrintError("Open failed: %s", szFileName);
        }
    }

    // Return what we got
    return pFileData;
}

static bool CompareTwoFiles(TLogHelper * pLogger, TFileData * pFileData1, TFileData * pFileData2)
{
    // Compare the file size
    if(pFileData1->dwFileSize != pFileData2->dwFileSize)
    {
        pLogger->PrintErrorVa(_T("The files have different size (%u vs %u)"), pFileData1->dwFileSize, pFileData2->dwFileSize);
        SetLastError(ERROR_FILE_CORRUPT);
        return false;
    }

    // Compare the files
    for(DWORD i = 0; i < pFileData1->dwFileSize; i++)
    {
        if(pFileData1->FileData[i] != pFileData2->FileData[i])
        {
            pLogger->PrintErrorVa(_T("Files are different at offset %08X"), i);
            SetLastError(ERROR_FILE_CORRUPT);
            return false;
        }
    }

    // The files are identical
    return true;
}

static DWORD SearchArchive(
    TLogHelper * pLogger,
    HANDLE hMpq,
    DWORD dwSearchFlags = 0,
    DWORD * pdwFileCount = NULL,
    LPBYTE pbFileHash = NULL)
{
    SFILE_FIND_DATA sf;
    TFileData * pFileData;
    HANDLE hFind;
    DWORD dwFileCount = 0;
    hash_state md5state;
    TCHAR szListFile[MAX_PATH] = _T("");
    char szMostPatched[MAX_PATH] = "";
    DWORD dwErrCode = ERROR_SUCCESS;
    bool bIgnoreOpedwErrCodes = (dwSearchFlags & SEARCH_FLAG_IGNORE_ERRORS) ? true : false;
    bool bFound = true;
    int nMaxPatchCount = 0;
    int nPatchCount = 0;

    // Construct the full name of the listfile
    CreateFullPathName(szListFile, _countof(szListFile), szListFileDir, _T("ListFile_Blizzard.txt"));

    // Prepare hashing
    md5_init(&md5state);

    // Initiate the MPQ search
    pLogger->PrintProgress("Searching the archive (initializing) ...");
    hFind = SFileFindFirstFile(hMpq, "*", &sf, szListFile);
    if(hFind == NULL)
    {
        dwErrCode = GetLastError();
        dwErrCode = (dwErrCode == ERROR_NO_MORE_FILES) ? ERROR_SUCCESS : dwErrCode;
        return dwErrCode;
    }

    // Perform the search
    pLogger->PrintProgress("Searching the archive ...");
    while(bFound == true)
    {
        // Increment number of files
        dwFileCount++;

//      if(!_stricmp(sf.cFileName, "war3map.j"))
//          DebugBreak();

        if(dwSearchFlags & SEARCH_FLAG_MOST_PATCHED)
        {
            // Load the patch count
            nPatchCount = GetFilePatchCount(pLogger, hMpq, sf.cFileName);

            // Check if it's greater than maximum
            if(nPatchCount > nMaxPatchCount)
            {
                strcpy(szMostPatched, sf.cFileName);
                nMaxPatchCount = nPatchCount;
            }
        }

        // Load the file to memory, if required
        if(dwSearchFlags & SEARCH_FLAG_LOAD_FILES)
        {
            // Load the entire file to the MPQ
            pFileData = LoadMpqFile(pLogger, hMpq, sf.cFileName, sf.lcLocale, bIgnoreOpedwErrCodes);
            if(pFileData != NULL)
            {
                // Hash the file data, if needed
                if((dwSearchFlags & SEARCH_FLAG_HASH_FILES) && !IsInternalMpqFileName(sf.cFileName))
                    md5_process(&md5state, pFileData->FileData, pFileData->dwFileSize);

                // Play sound files, if required
                if((dwSearchFlags & SEARCH_FLAG_PLAY_WAVES) && strstr(sf.cFileName, ".wav") != NULL)
                {
#ifdef _MSC_VER
                    pLogger->PrintProgress("Playing sound %s", sf.cFileName);
                    PlaySound((LPCTSTR)pFileData->FileData, NULL, SND_MEMORY);
#endif
                }

                STORM_FREE(pFileData);
            }
        }

        bFound = SFileFindNextFile(hFind, &sf);
    }
    SFileFindClose(hFind);

    // Give the file count, if required
    if(pdwFileCount != NULL)
        pdwFileCount[0] = dwFileCount;

    // Give the hash, if required
    if(pbFileHash != NULL && (dwSearchFlags & SEARCH_FLAG_HASH_FILES))
        md5_done(&md5state, pbFileHash);

    return dwErrCode;
}

static DWORD CreateNewArchive(TLogHelper * pLogger, LPCTSTR szPlainName, DWORD dwCreateFlags, DWORD dwMaxFileCount, HANDLE * phMpq)
{
    HANDLE hMpq = NULL;
    TCHAR szMpqName[MAX_PATH];
    TCHAR szFullPath[MAX_PATH];

    // Make sure that the MPQ is deleted
    CreateFullPathName(szFullPath, _countof(szFullPath), NULL, szPlainName);
    _tremove(szFullPath);

    // Create the new MPQ
    StringCopy(szMpqName, _countof(szMpqName), szFullPath);
    if(!SFileCreateArchive(szMpqName, dwCreateFlags, dwMaxFileCount, &hMpq))
        return pLogger->PrintError(_T("Failed to create archive %s"), szMpqName);

    // Shall we close it right away?
    if(phMpq == NULL)
        SFileCloseArchive(hMpq);
    else
        *phMpq = hMpq;

    return ERROR_SUCCESS;
}

static DWORD CreateNewArchive_V2(TLogHelper * pLogger, LPCTSTR szPlainName, DWORD dwCreateFlags, DWORD dwMaxFileCount, HANDLE * phMpq)
{
    SFILE_CREATE_MPQ CreateInfo;
    HANDLE hMpq = NULL;
    TCHAR szMpqName[MAX_PATH];
    TCHAR szFullPath[MAX_PATH];

    // Make sure that the MPQ is deleted
    CreateFullPathName(szFullPath, _countof(szFullPath), NULL, szPlainName);
    StringCopy(szMpqName, _countof(szMpqName), szFullPath);
    _tremove(szFullPath);

    // Fill the create structure
    memset(&CreateInfo, 0, sizeof(SFILE_CREATE_MPQ));
    CreateInfo.cbSize         = sizeof(SFILE_CREATE_MPQ);
    CreateInfo.dwMpqVersion   = (dwCreateFlags & MPQ_CREATE_ARCHIVE_VMASK) >> FLAGS_TO_FORMAT_SHIFT;
    CreateInfo.dwStreamFlags  = STREAM_PROVIDER_FLAT | BASE_PROVIDER_FILE;
//  CreateInfo.dwFileFlags1   = (dwCreateFlags & MPQ_CREATE_LISTFILE)   ? MPQ_FILE_EXISTS : 0;
//  CreateInfo.dwFileFlags2   = (dwCreateFlags & MPQ_CREATE_ATTRIBUTES) ? MPQ_FILE_EXISTS : 0;
    CreateInfo.dwFileFlags1   = (dwCreateFlags & MPQ_CREATE_LISTFILE)   ? MPQ_FILE_DEFAULT_INTERNAL : 0;
    CreateInfo.dwFileFlags2   = (dwCreateFlags & MPQ_CREATE_ATTRIBUTES) ? MPQ_FILE_DEFAULT_INTERNAL : 0;
    CreateInfo.dwFileFlags3   = (dwCreateFlags & MPQ_CREATE_SIGNATURE)  ? MPQ_FILE_DEFAULT_INTERNAL : 0;
    CreateInfo.dwAttrFlags    = (dwCreateFlags & MPQ_CREATE_ATTRIBUTES) ? (MPQ_ATTRIBUTE_CRC32 | MPQ_ATTRIBUTE_FILETIME | MPQ_ATTRIBUTE_MD5) : 0;
    CreateInfo.dwSectorSize   = (CreateInfo.dwMpqVersion >= MPQ_FORMAT_VERSION_3) ? 0x4000 : 0x1000;
    CreateInfo.dwRawChunkSize = (CreateInfo.dwMpqVersion >= MPQ_FORMAT_VERSION_4) ? 0x4000 : 0;
    CreateInfo.dwMaxFileCount = dwMaxFileCount;

    // Create the new MPQ
    if(!SFileCreateArchive2(szMpqName, &CreateInfo, &hMpq))
        return pLogger->PrintError(_T("Failed to create archive %s"), szMpqName);

    // Shall we close it right away?
    if(phMpq == NULL)
        SFileCloseArchive(hMpq);
    else
        *phMpq = hMpq;

    return ERROR_SUCCESS;
}

// Creates new archive with UNICODE name. Adds prefix to the name
static DWORD CreateNewArchiveU(TLogHelper * pLogger, const wchar_t * szPlainName, DWORD dwCreateFlags, DWORD dwMaxFileCount)
{
#ifdef _UNICODE
    HANDLE hMpq = NULL;
    TCHAR szMpqName[MAX_PATH+1];
    TCHAR szFullPath[MAX_PATH];

    // Construct the full UNICODE name
    CreateFullPathName(szFullPath, _countof(szFullPath), NULL, _T("StormLibTest_"));
    StringCopy(szMpqName, _countof(szMpqName), szFullPath);
    StringCat(szMpqName, _countof(szMpqName), szPlainName);

    // Make sure that the MPQ is deleted
    _tremove(szMpqName);

    // Create the archive
    pLogger->PrintProgress("Creating new archive with UNICODE name ...");
    if(!SFileCreateArchive(szMpqName, dwCreateFlags, dwMaxFileCount, &hMpq))
        return pLogger->PrintError(_T("Failed to create archive %s"), szMpqName);

    SFileCloseArchive(hMpq);
#else
    pLogger = pLogger;
    szPlainName = szPlainName;
    dwCreateFlags = dwCreateFlags;
    dwMaxFileCount = dwMaxFileCount;
#endif
    return ERROR_SUCCESS;
}

static DWORD OpenExistingArchive(TLogHelper * pLogger, LPCTSTR szFullPath, DWORD dwFlags, HANDLE * phMpq)
{
    HANDLE hMpq = NULL;
//  bool bReopenResult;
    DWORD dwErrCode = ERROR_SUCCESS;

    // Is it an encrypted MPQ ?
    if(_tcsstr(szFullPath, _T(".MPQE")) != NULL)
        dwFlags |= STREAM_PROVIDER_MPQE;
    if(_tcsstr(szFullPath, _T(".MPQ.part")) != NULL)
        dwFlags |= STREAM_PROVIDER_PARTIAL;
    if(_tcsstr(szFullPath, _T(".mpq.part")) != NULL)
        dwFlags |= STREAM_PROVIDER_PARTIAL;
    if(_tcsstr(szFullPath, _T(".MPQ.0")) != NULL)
        dwFlags |= STREAM_PROVIDER_BLOCK4;

    // Open the copied archive
    pLogger->PrintProgress(_T("Opening archive %s ..."), GetShortPlainName(szFullPath));
    if(!SFileOpenArchive(szFullPath, 0, dwFlags, &hMpq))
    {
        switch(dwErrCode = GetLastError())
        {
//          case ERROR_BAD_FORMAT:  // If the error is ERROR_BAD_FORMAT, try to open with MPQ_OPEN_FORCE_MPQ_V1
//              bReopenResult = SFileOpenArchive(szMpqName, 0, dwFlags | MPQ_OPEN_FORCE_MPQ_V1, &hMpq);
//              dwErrCode = (bReopenResult == false) ? GetLastError() : ERROR_SUCCESS;
//              break;

            case ERROR_AVI_FILE:        // Ignore the error if it's an AVI file or if the file is incomplete
            case ERROR_FILE_INCOMPLETE:
                return dwErrCode;
        }

        // Show the open error to the user
        return pLogger->PrintError(_T("Failed to open archive %s"), szFullPath);
    }

    // Store the archive handle or close the archive
    if(phMpq == NULL)
        SFileCloseArchive(hMpq);
    else
        *phMpq = hMpq;
    return dwErrCode;
}

static DWORD OpenPatchArchive(TLogHelper * pLogger, HANDLE hMpq, LPCTSTR szFullPath)
{
    TCHAR szPatchName[MAX_PATH];
    DWORD dwErrCode = ERROR_SUCCESS;

    pLogger->PrintProgress(_T("Adding patch %s ..."), GetShortPlainName(szFullPath));
    StringCopy(szPatchName, _countof(szPatchName), szFullPath);
    if(!SFileOpenPatchArchive(hMpq, szPatchName, NULL, 0))
        dwErrCode = pLogger->PrintError(_T("Failed to add patch %s ..."), szFullPath);

    return dwErrCode;
}

static DWORD OpenExistingArchiveWithCopy(TLogHelper * pLogger, LPCTSTR szFileName, LPCTSTR szCopyName, HANDLE * phMpq)
{
    DWORD dwFlags = 0;
    TCHAR szFullPath[MAX_PATH];
    DWORD dwErrCode = ERROR_SUCCESS;

    // We expect MPQ directory to be already prepared by InitializeMpqDirectory
    assert(szMpqDirectory[0] != 0);

    // At least one name must be entered
    assert(szFileName != NULL || szCopyName != NULL);

    // If both names entered, create a copy
    if(szFileName != NULL && szCopyName != NULL)
    {
        dwErrCode = CreateFileCopy(pLogger, szFileName, szCopyName, szFullPath, _countof(szFullPath));
        if(dwErrCode != ERROR_SUCCESS)
            return dwErrCode;
    }

    // If only source name entered, open it for read-only access
    else if(szFileName != NULL && szCopyName == NULL)
    {
        CreateFullPathName(szFullPath, _countof(szFullPath), szMpqSubDir, szFileName);
        dwFlags |= MPQ_OPEN_READ_ONLY;
    }

    // If only target name entered, open it directly
    else if(szFileName == NULL && szCopyName != NULL)
    {
        CreateFullPathName(szFullPath, _countof(szFullPath), NULL, szCopyName);
    }

    // Open the archive
    return OpenExistingArchive(pLogger, szFullPath, dwFlags, phMpq);
}

static DWORD OpenPatchedArchive(TLogHelper * pLogger, HANDLE * phMpq, LPCTSTR PatchList[])
{
    HANDLE hMpq = NULL;
    TCHAR szFullPath[MAX_PATH];
    DWORD dwErrCode = ERROR_SUCCESS;

    // The first file is expected to be valid
    assert(PatchList[0] != NULL);

    // Open the primary MPQ
    CreateFullPathName(szFullPath, _countof(szFullPath), szMpqSubDir, PatchList[0]);
    dwErrCode = OpenExistingArchive(pLogger, szFullPath, MPQ_OPEN_READ_ONLY, &hMpq);

    // Add all patches
    if(dwErrCode == ERROR_SUCCESS)
    {
        for(size_t i = 1; PatchList[i] != NULL; i++)
        {
            CreateFullPathName(szFullPath, _countof(szFullPath), szMpqPatchDir, PatchList[i]);
            dwErrCode = OpenPatchArchive(pLogger, hMpq, szFullPath);
            if(dwErrCode != ERROR_SUCCESS)
                break;
        }
    }

    // If anything failed, close the MPQ handle
    if(dwErrCode != ERROR_SUCCESS)
    {
        SFileCloseArchive(hMpq);
        hMpq = NULL;
    }

    // Give the archive handle to the caller
    if(phMpq != NULL)
        *phMpq = hMpq;
    return dwErrCode;
}

static DWORD AddFileToMpq(
    TLogHelper * pLogger,
    HANDLE hMpq,
    LPCSTR szFileName,
    LPCSTR szFileData,
    DWORD dwFlags = 0,
    DWORD dwCompression = 0,
    DWORD dwExpectedError = ERROR_SUCCESS)
{
    HANDLE hFile = NULL;
    DWORD dwFileSize = (DWORD)strlen(szFileData);
    DWORD dwErrCode = ERROR_SUCCESS;

    // Notify the user
    pLogger->PrintProgress("Adding file %s ...", szFileName);

    // Get the default flags
    if(dwFlags == 0)
        dwFlags = MPQ_FILE_COMPRESS | MPQ_FILE_ENCRYPTED;
    if(dwCompression == 0)
        dwCompression = MPQ_COMPRESSION_ZLIB;

    // Create the file within the MPQ
    if(SFileCreateFile(hMpq, szFileName, 0, dwFileSize, 0, dwFlags, &hFile))
    {
        // Write the file
        if(!SFileWriteFile(hFile, szFileData, dwFileSize, dwCompression))
            dwErrCode = pLogger->PrintError("Failed to write data to the MPQ");
        SFileCloseFile(hFile);
    }
    else
    {
        dwErrCode = GetLastError();
    }

    // Check the expected error code
    if(dwExpectedError != ERROR_UNDETERMINED_RESULT && dwErrCode != dwExpectedError)
        return pLogger->PrintError("Unexpected result from SFileCreateFile(%s)", szFileName);
    return dwErrCode;
}

static DWORD AddLocalFileToMpq(
    TLogHelper * pLogger,
    HANDLE hMpq,
    LPCSTR szArchivedName,
    LPCTSTR szLocalFileName,
    DWORD dwFlags = 0,
    DWORD dwCompression = 0,
    bool bMustSucceed = false)
{
    TCHAR szFileName[MAX_PATH];
    DWORD dwVerifyResult;

    // Notify the user
    pLogger->PrintProgress("Adding file %s (%u of %u)...", GetShortPlainName(szLocalFileName), pLogger->UserCount, pLogger->UserTotal);
    pLogger->UserString = szArchivedName;

    // Get the default flags
    if(dwFlags == 0)
        dwFlags = MPQ_FILE_COMPRESS | MPQ_FILE_ENCRYPTED;
    if(dwCompression == 0)
        dwCompression = MPQ_COMPRESSION_ZLIB;

    // Set the notification callback
    SFileSetAddFileCallback(hMpq, AddFileCallback, pLogger);

    // Add the file to the MPQ
    StringCopy(szFileName, _countof(szFileName), szLocalFileName);
    if(!SFileAddFileEx(hMpq, szFileName, szArchivedName, dwFlags, dwCompression, MPQ_COMPRESSION_NEXT_SAME))
    {
        if(bMustSucceed)
            return pLogger->PrintError("Failed to add the file %s", szArchivedName);
        return GetLastError();
    }

    // Verify the file unless it was lossy compression
    if((dwCompression & (MPQ_COMPRESSION_ADPCM_MONO | MPQ_COMPRESSION_ADPCM_STEREO)) == 0)
    {
        // Notify the user
        pLogger->PrintProgress("Verifying file %s (%u of %u) ...", szArchivedName, pLogger->UserCount, pLogger->UserTotal);

        // Perform the verification
        dwVerifyResult = SFileVerifyFile(hMpq, szArchivedName, MPQ_ATTRIBUTE_CRC32 | MPQ_ATTRIBUTE_MD5);
        if(dwVerifyResult & (VERIFY_OPEN_ERROR | VERIFY_READ_ERROR | VERIFY_FILE_SECTOR_CRC_ERROR | VERIFY_FILE_CHECKSUM_ERROR | VERIFY_FILE_MD5_ERROR))
            return pLogger->PrintError("CRC error on %s", szArchivedName);
    }

    return ERROR_SUCCESS;
}

static DWORD RenameMpqFile(TLogHelper * pLogger, HANDLE hMpq, LPCSTR szOldFileName, LPCSTR szNewFileName, DWORD dwExpectedError)
{
    DWORD dwErrCode = ERROR_SUCCESS;

    // Notify the user
    pLogger->PrintProgress("Renaming %s to %s ...", szOldFileName, szNewFileName);

    // Perform the deletion
    if(!SFileRenameFile(hMpq, szOldFileName, szNewFileName))
        dwErrCode = GetLastError();

    if(dwErrCode != dwExpectedError)
        return pLogger->PrintErrorVa("Unexpected result from SFileRenameFile(%s -> %s)", szOldFileName, szNewFileName);
    return ERROR_SUCCESS;
}

static DWORD RemoveMpqFile(TLogHelper * pLogger, HANDLE hMpq, LPCSTR szFileName, DWORD dwExpectedError)
{
    DWORD dwErrCode = ERROR_SUCCESS;

    // Notify the user
    pLogger->PrintProgress("Removing file %s ...", szFileName);

    // Perform the deletion
    if(!SFileRemoveFile(hMpq, szFileName, 0))
        dwErrCode = GetLastError();

    if(dwErrCode != dwExpectedError)
        return pLogger->PrintError("Unexpected result from SFileRemoveFile(%s)", szFileName);
    return ERROR_SUCCESS;
}

static ULONGLONG SFileGetFilePointer(HANDLE hFile)
{
    LONG FilePosHi = 0;
    DWORD FilePosLo;

    FilePosLo = SFileSetFilePointer(hFile, 0, &FilePosHi, FILE_CURRENT);
    return MAKE_OFFSET64(FilePosHi, FilePosLo);
}

//-----------------------------------------------------------------------------
// Tests

static DWORD TestSetFilePointer(
    HANDLE hFile,
    LONGLONG DeltaPos,
    ULONGLONG ExpectedPos,
    DWORD dwMoveMethod,
    bool bUseFilePosHigh,
    DWORD dwErrCode)
{
    ULONGLONG NewPos = 0;
    LONG DeltaPosHi = (LONG)(DeltaPos >> 32);
    LONG DeltaPosLo = (LONG)(DeltaPos);

    // If there was an error before, do nothing
    if(dwErrCode == ERROR_SUCCESS)
    {
        SFileSetFilePointer(hFile, DeltaPosLo, bUseFilePosHigh ? &DeltaPosHi : NULL, dwMoveMethod);
        NewPos = SFileGetFilePointer(hFile);
        if(NewPos != ExpectedPos)
            dwErrCode = ERROR_HANDLE_EOF;
    }

    return dwErrCode;
}

static DWORD TestSetFilePointers(HANDLE hFile, bool bUseFilePosHigh)
{
    LONGLONG FileSize;
    DWORD dwErrCode = ERROR_SUCCESS;

    // We expect the file to be at least 2 pages long
    FileSize = SFileGetFileSize(hFile, NULL);
    if(FileSize < 0x2000)
        return ERROR_NOT_SUPPORTED;

    // Move 0x20 bytes from the beginning. Expected new pos is 0x20
    dwErrCode = TestSetFilePointer(hFile, 0x20, 0x20, FILE_BEGIN, bUseFilePosHigh, dwErrCode);

    // Move 0x20 bytes from the current position. Expected new pos is 0x20
    dwErrCode = TestSetFilePointer(hFile, 0x20, 0x40, FILE_CURRENT, bUseFilePosHigh, dwErrCode);

    // Move 0x40 bytes back. Because the offset can't be moved to negative position, it will be zero
    dwErrCode = TestSetFilePointer(hFile, -64, 0x00, FILE_CURRENT, bUseFilePosHigh, dwErrCode);

    // Move 0x40 bytes before the end of the file
    dwErrCode = TestSetFilePointer(hFile, -64, FileSize-64, FILE_END, bUseFilePosHigh, dwErrCode);

    // Move 0x80 bytes forward. Should be at end of file
    dwErrCode = TestSetFilePointer(hFile, 0x80, FileSize, FILE_CURRENT, bUseFilePosHigh, dwErrCode);

    return dwErrCode;
}


static void TestGetFileInfo(
    TLogHelper * pLogger,
    HANDLE hMpqOrFile,
    SFileInfoClass InfoClass,
    void * pvFileInfo,
    DWORD cbFileInfo,
    DWORD * pcbLengthNeeded,
    bool bExpectedResult,
    DWORD dwExpectedErrCode)
{
    DWORD dwErrCode = ERROR_SUCCESS;
    bool bResult;

    // Call the get file info
    bResult = SFileGetFileInfo(hMpqOrFile, InfoClass, pvFileInfo, cbFileInfo, pcbLengthNeeded);
    if(!bResult)
        dwErrCode = GetLastError();

    if(bResult != bExpectedResult)
        pLogger->PrintMessage("Different result of SFileGetFileInfo.");
    if(dwErrCode != dwExpectedErrCode)
        pLogger->PrintMessage("Different error from SFileGetFileInfo (expected %u, returned %u)", dwExpectedErrCode, dwErrCode);
}

// StormLib is able to open local files (as well as the original Storm.dll)
// I want to keep this for occasional use

static LINE_INFO Lines[] =
{
    {0x000, 18, "accountbilling.url"},
    {0x013, 45, "alternate/character/goblin/male/goblinmale.m2"},
    {0x9ab, 54, "alternate/character/goblin/male/goblinmale0186-00.anim"}
};

static DWORD TestOnLocalListFile_Read(TLogHelper & Logger, HANDLE hFile)
{
    for(size_t i = 0; i < _countof(Lines); i++)
    {
        DWORD dwBytesRead = 0;
        char szFileLine[0x100] = {0};

        SFileSetFilePointer(hFile, Lines[i].nLinePos, NULL, FILE_BEGIN);
        SFileReadFile(hFile, szFileLine, Lines[i].nLineLen, &dwBytesRead, NULL);

        if(dwBytesRead != Lines[i].nLineLen)
        {
            Logger.PrintMessage("Line %u length mismatch", i);
            return false;
        }

        if(strcmp(szFileLine, Lines[i].szLine))
        {
            Logger.PrintMessage("Line %u content mismatch", i);
            return false;
        }
    }

    return true;
}

static DWORD TestOnLocalListFile(LPCTSTR szPlainName)
{
    TLogHelper Logger("LocalListFile", szPlainName);
    SFILE_FIND_DATA sf;
    HANDLE hFile;
    HANDLE hFind;
    DWORD dwFileSizeHi = 0;
    DWORD dwFileSizeLo = 0;
    TCHAR szFullPath[MAX_PATH];
    char szFileName1[MAX_PATH];
    char szFileName2[MAX_PATH];
    int nFileCount = 0;

    // Get the full name of the local file
    CreateFullPathName(szFileName1, _countof(szFileName1), szMpqSubDir, szPlainName);

    // Test opening the local file
    if(SFileOpenFileEx(NULL, szFileName1, SFILE_OPEN_LOCAL_FILE, &hFile))
    {
        // Retrieve the file name. It must match the name under which the file was open
        if(FileStream_Prefix(szPlainName, NULL) == 0)
        {
            SFileGetFileName(hFile, szFileName2);
            if(strcmp(szFileName2, szFileName1))
                Logger.PrintMessage("The retrieved name does not match the open name");
        }

        // Retrieve the file size
        dwFileSizeLo = SFileGetFileSize(hFile, &dwFileSizeHi);
        if(dwFileSizeHi != 0 || dwFileSizeLo != 0x04385a4e)
            Logger.PrintMessage("Local file size mismatch");

        // Read few lines, check their content
        TestOnLocalListFile_Read(Logger, hFile);
        SFileCloseFile(hFile);
    }
    else
        return Logger.PrintError("Failed to open local listfile");

    // We need unicode listfile name
    StringCopy(szFullPath, _countof(szFullPath), szFileName1);

    // Start searching in the listfile
    hFind = SListFileFindFirstFile(NULL, szFullPath, "*", &sf);
    if(hFind != NULL)
    {
        for(;;)
        {
            Logger.PrintProgress("Found file (%04u): %s", nFileCount++, GetShortPlainName(sf.cFileName));
            if(!SListFileFindNextFile(hFind, &sf))
                break;
        }

        SListFileFindClose(hFind);
    }
    else
        return Logger.PrintError("Failed to search local listfile");

    return ERROR_SUCCESS;
}

static void WINAPI TestReadFile_DownloadCallback(
    void * UserData,
    ULONGLONG ByteOffset,
    DWORD DataLength)
{
    TLogHelper * pLogger = (TLogHelper *)UserData;

    if(ByteOffset != 0 && DataLength != 0)
        pLogger->PrintProgress("Downloading data (offset: " I64X_a ", length: %X)", ByteOffset, DataLength);
    else
        pLogger->PrintProgress("Download complete.");
}

// Open a file stream with mirroring a master file
static DWORD TestReadFile_MasterMirror(LPCTSTR szMirrorName, LPCTSTR szMasterName, bool bCopyMirrorFile)
{
    TFileStream * pStream1;                     // Master file
    TFileStream * pStream2;                     // Mirror file
    TLogHelper Logger("OpenMirrorFile", szMirrorName);
    TCHAR szMirrorPath[MAX_PATH + MAX_PATH];
    TCHAR szMasterPath[MAX_PATH];
    DWORD dwProvider = 0;
    int nIterations = 0x10000;
    DWORD dwErrCode;

    // Retrieve the provider
    FileStream_Prefix(szMasterName, &dwProvider);

#ifndef STORMLIB_WINDOWS
    if((dwProvider & BASE_PROVIDER_MASK) == BASE_PROVIDER_HTTP)
        return ERROR_SUCCESS;
#endif

    // Create copy of the file to serve as mirror, keep master there
    dwErrCode = CreateMasterAndMirrorPaths(&Logger, szMirrorPath, szMasterPath, szMirrorName, szMasterName, bCopyMirrorFile);
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Open both master and mirror file
        pStream1 = FileStream_OpenFile(szMasterPath, STREAM_FLAG_READ_ONLY);
        pStream2 = FileStream_OpenFile(szMirrorPath, STREAM_FLAG_READ_ONLY | STREAM_FLAG_USE_BITMAP);
        if(pStream1 && pStream2)
        {
            // For internet based files, we limit the number of operations
            if((dwProvider & BASE_PROVIDER_MASK) == BASE_PROVIDER_HTTP)
                nIterations = 0x80;

            FileStream_SetCallback(pStream2, TestReadFile_DownloadCallback, &Logger);
            dwErrCode = CompareTwoLocalFilesRR(&Logger, pStream1, pStream2, nIterations);
        }

        if(pStream2 != NULL)
            FileStream_Close(pStream2);
        if(pStream1 != NULL)
            FileStream_Close(pStream1);
    }

    return dwErrCode;
}

// Test of the TFileStream object
static DWORD TestFileStreamOperations(LPCTSTR szPlainName, DWORD dwStreamFlags)
{
    TFileStream * pStream = NULL;
    TLogHelper Logger("FileStreamTest", szPlainName);
    ULONGLONG ByteOffset;
    ULONGLONG FileSize = 0;
    TCHAR szFullPath[MAX_PATH];
    DWORD dwRequiredFlags = 0;
    BYTE Buffer[0x10];
    DWORD dwErrCode = ERROR_SUCCESS;

    // Copy the file so we won't screw up
    if((dwStreamFlags & STREAM_PROVIDER_MASK) == STREAM_PROVIDER_BLOCK4)
        CreateFullPathName(szFullPath, _countof(szFullPath), szMpqSubDir, szPlainName);
    else
        dwErrCode = CreateFileCopy(&Logger, szPlainName, szPlainName, szFullPath, _countof(szFullPath));

    // Open the file stream
    if(dwErrCode == ERROR_SUCCESS)
    {
        pStream = FileStream_OpenFile(szFullPath, dwStreamFlags);
        if(pStream == NULL)
            return Logger.PrintError(_T("Open failed: %s"), szFullPath);
    }

    // Get the size of the file stream
    if(dwErrCode == ERROR_SUCCESS)
    {
        if(!FileStream_GetFlags(pStream, &dwStreamFlags))
            dwErrCode = Logger.PrintError("Failed to retrieve the stream flags");

        if(!FileStream_GetSize(pStream, &FileSize))
            dwErrCode = Logger.PrintError("Failed to retrieve the file size");

        // Any other stream except STREAM_PROVIDER_FLAT | BASE_PROVIDER_FILE should be read-only
        if((dwStreamFlags & STREAM_PROVIDERS_MASK) != (STREAM_PROVIDER_FLAT | BASE_PROVIDER_FILE))
            dwRequiredFlags |= STREAM_FLAG_READ_ONLY;
//      if(pStream->BlockPresent)
//          dwRequiredFlags |= STREAM_FLAG_READ_ONLY;

        // Check the flags there
        if((dwStreamFlags & dwRequiredFlags) != dwRequiredFlags)
        {
            Logger.PrintMessage("The stream should be read-only but it isn't");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // After successful open, the stream position must be zero
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = VerifyFilePosition(&Logger, pStream, 0);

    // Read the MPQ header from the current file offset.
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = VerifyFileMpqHeader(&Logger, pStream, NULL);

    // After successful open, the stream position must sizeof(TMPQHeader)
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = VerifyFilePosition(&Logger, pStream, sizeof(TMPQHeader));

    // Now try to read the MPQ header from the offset 0
    if(dwErrCode == ERROR_SUCCESS)
    {
        ByteOffset = 0;
        dwErrCode = VerifyFileMpqHeader(&Logger, pStream, &ByteOffset);
    }

    // After successful open, the stream position must sizeof(TMPQHeader)
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = VerifyFilePosition(&Logger, pStream, sizeof(TMPQHeader));

    // Try a write operation
    if(dwErrCode == ERROR_SUCCESS)
    {
        bool bExpectedResult = (dwStreamFlags & STREAM_FLAG_READ_ONLY) ? false : true;
        bool bResult;

        // Attempt to write to the file
        ByteOffset = 0;
        bResult = FileStream_Write(pStream, &ByteOffset, Buffer, sizeof(Buffer));

        // If the result is not expected
        if(bResult != bExpectedResult)
        {
            Logger.PrintMessage("FileStream_Write result is different than expected");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Move the position 9 bytes from the end and try to read 10 bytes.
    // This must fail, because stream reading functions are "all or nothing"
    if(dwErrCode == ERROR_SUCCESS)
    {
        ByteOffset = FileSize - 9;
        if(FileStream_Read(pStream, &ByteOffset, Buffer, 10))
        {
            Logger.PrintMessage("FileStream_Read succeeded, but it shouldn't");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Try again with 9 bytes. This must succeed, unless the file block is not available
    if(dwErrCode == ERROR_SUCCESS)
    {
        ByteOffset = FileSize - 9;
        if(!FileStream_Read(pStream, &ByteOffset, Buffer, 9))
        {
            Logger.PrintMessage("FileStream_Read from the end of the file failed");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Verify file position - it must be at the end of the file
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = VerifyFilePosition(&Logger, pStream, FileSize);

    // Close the stream
    if(pStream != NULL)
        FileStream_Close(pStream);
    return dwErrCode;
}

static DWORD TestArchive_LoadFiles(TLogHelper * pLogger, HANDLE hMpq, bool bIgnoreOpedwErrCodes, ...)
{
    TFileData * pFileData;
    const char * szFileName;
    va_list argList;
    DWORD dwErrCode = ERROR_SUCCESS;

    va_start(argList, bIgnoreOpedwErrCodes);
    while((szFileName = va_arg(argList, const char *)) != NULL)
    {
        if(SFileHasFile(hMpq, szFileName))
        {
            pFileData = LoadMpqFile(pLogger, hMpq, szFileName);
            if(pFileData == NULL && bIgnoreOpedwErrCodes == false)
            {
                pLogger->PrintError("Error loading the file %s", szFileName);
                dwErrCode = ERROR_FILE_CORRUPT;
                break;
            }
            else
            {
                STORM_FREE(pFileData);
                pFileData = NULL;
            }
        }
    }
    va_end(argList);

    return dwErrCode;
}

static DWORD TestArchive_SetPos(HANDLE hMpq, const char * szFileName)
{
    HANDLE hFile = NULL;
    DWORD dwErrCode = ERROR_SUCCESS;

    if(SFileOpenFileEx(hMpq, szFileName, 0, &hFile))
    {
        // First, use the SFileSetFilePointer WITHOUT the high-dword position
        dwErrCode = TestSetFilePointers(hFile, false);

        // First, use the SFileSetFilePointer WITH the high-dword position
        if(dwErrCode == ERROR_SUCCESS)
            dwErrCode = TestSetFilePointers(hFile, true);

        // Close the file
        SFileCloseFile(hFile);
    }

    return dwErrCode;
}

static DWORD TestArchive(
    LPCTSTR szPlainName,                // Plain name of the MPQ
    LPCTSTR szListFile,                 // Listfile name (NULL if none)
    DWORD  dwFlags,                     // Flags
    LPCSTR szFileName1,
    LPCSTR szFileName2)
{
    TFileData * pFileData1 = NULL;
    TFileData * pFileData2 = NULL;
    TLogHelper Logger("TestMpq", szPlainName);
    LPCSTR szExpectedMD5 = NULL;
    HANDLE hMpq = NULL;
    DWORD dwFileCount = 0;
    DWORD dwSearchFlags = 0;
    DWORD dwErrCode;
    DWORD dwCrc32 = 0;
    DWORD dwExpectedFileCount = 0;
    DWORD dwMpqFlags = 0;
    TCHAR szFullName[MAX_PATH];
    BYTE ExpectedMD5[MD5_DIGEST_SIZE];
    BYTE OverallMD5[MD5_DIGEST_SIZE];
    bool bIgnoreOpedwErrCodes = false;

    // If the file is a partial MPQ, don't load all files
    if(_tcsstr(szPlainName, _T(".MPQ.part")) == NULL)
        dwSearchFlags |= SEARCH_FLAG_LOAD_FILES;

    // If the MPQ is a protected MPQ, do different tests
    if(dwFlags & TEST_FLAG_PROTECTED)
    {
        dwExpectedFileCount = (dwFlags & TEST_FLAG_FILE_COUNT);
        if((szExpectedMD5 = szFileName1) != NULL)
            dwSearchFlags |= SEARCH_FLAG_HASH_FILES;
        szFileName2 = szFileName1 = NULL;
    }

    // Copy the archive so we won't fuck up the original one
    dwErrCode = OpenExistingArchiveWithCopy(&Logger, szPlainName, NULL, &hMpq);
    while(dwErrCode == ERROR_SUCCESS)
    {
        // Check for malformed MPQs
        SFileGetFileInfo(hMpq, SFileMpqFlags, &dwMpqFlags, sizeof(dwMpqFlags), NULL);
        if(dwMpqFlags & MPQ_FLAG_MALFORMED)
        {
            dwSearchFlags |= SEARCH_FLAG_IGNORE_ERRORS;
            bIgnoreOpedwErrCodes = true;
        }

        // If the listfile was given, add it to the MPQ
        if(szListFile && szListFile[0])
        {
            Logger.PrintProgress(_T("Adding listfile %s ..."), szListFile);
            CreateFullPathName(szFullName, _countof(szFullName), szListFileDir, szListFile);
            if((dwErrCode = SFileAddListFile(hMpq, szFullName)) != ERROR_SUCCESS)
                Logger.PrintMessage("Failed to add the listfile to the MPQ");
        }

        // Attempt to open the (listfile), (attributes), (signature)
        dwErrCode = TestArchive_LoadFiles(&Logger, hMpq, bIgnoreOpedwErrCodes, LISTFILE_NAME, ATTRIBUTES_NAME, SIGNATURE_NAME, NULL);
        if(dwErrCode != ERROR_SUCCESS)
            break;

        // If szFileName1 was given, load it and check its CRC
        if(szFileName1 && szFileName1[0])
        {
            // Test setting position
            dwErrCode = TestArchive_SetPos(hMpq, szFileName1);
            if(dwErrCode != ERROR_SUCCESS)
                break;

            // Load the entire file 1
            pFileData1 = LoadMpqFile(&Logger, hMpq, szFileName1);
            if(pFileData1 == NULL)
            {
                dwErrCode = Logger.PrintError("Failed to load the file %s", szFileName1);
                break;
            }

            // Check the CRC of file1, if available
            if(pFileData1->dwCrc32)
            {
                // Compare the CRC32, if available
                dwCrc32 = crc32(0, (Bytef *)pFileData1->FileData, (uInt)pFileData1->dwFileSize);
                if(dwCrc32 != pFileData1->dwCrc32)
                    Logger.PrintError("Warning: CRC32 error on %s", szFileName1);
            }
        }

        // If szFileName2 was given, load it
        if(szFileName2 && szFileName2[0])
        {
            // Load the entire file 2
            pFileData2 = LoadMpqFile(&Logger, hMpq, szFileName2);
            if(pFileData2 == NULL)
            {
                dwErrCode = Logger.PrintError("Failed to load the file %s", szFileName2);
                break;
            }
        }

        // If two files were given, compare them
        if(pFileData1 && pFileData2)
        {
            // Compare both files
            if(!CompareTwoFiles(&Logger, pFileData1, pFileData2))
            {
                dwErrCode = Logger.PrintError("The file has different size/content of files");
                break;
            }
        }

        // Search the archive
        dwErrCode = SearchArchive(&Logger, hMpq, dwSearchFlags, &dwFileCount, OverallMD5);

        // Shall we check the file count and overall MD5?
        if(dwExpectedFileCount != 0)
        {
            if(dwFileCount != dwExpectedFileCount)
            {
                Logger.PrintMessage("File count mismatch(expected: %u, found:%u)", dwExpectedFileCount, dwFileCount);
                dwErrCode = ERROR_CAN_NOT_COMPLETE;
                break;
            }
        }

        // Shall we check overall MD5?
        if(szExpectedMD5 && szExpectedMD5[0])
        {
            BinaryFromString(szExpectedMD5, ExpectedMD5, MD5_DIGEST_SIZE);
            if(memcmp(ExpectedMD5, OverallMD5, MD5_DIGEST_SIZE))
            {
                Logger.PrintMessage("Extracted files MD5 mismatch");
                dwErrCode = ERROR_CAN_NOT_COMPLETE;
                break;
            }
        }
        break;
    }

    // Common cleanup
    if(pFileData2 != NULL)
        STORM_FREE(pFileData2);
    if(pFileData1 != NULL)
        STORM_FREE(pFileData1);
    if(hMpq != NULL)
        SFileCloseArchive(hMpq);
    return dwErrCode;
}

// Open an empty archive (found in WoW cache - it's just a header)
static DWORD TestOpenArchive_WillFail(LPCTSTR szPlainName)
{
    TLogHelper Logger("FailMpqTest", szPlainName);
    HANDLE hMpq = NULL;
    TCHAR szMpqName[MAX_PATH];
    char szFullPath[MAX_PATH];

    // Create the full path name for the archive
    CreateFullPathName(szFullPath, _countof(szFullPath), szMpqSubDir, szPlainName);
    StringCopy(szMpqName, _countof(szFullPath), szFullPath);

    // Try to open the archive. It is expected to fail
    Logger.PrintProgress("Opening archive %s", szPlainName);
    if(!SFileOpenArchive(szMpqName, 0, MPQ_OPEN_READ_ONLY, &hMpq))
        return ERROR_SUCCESS;

    Logger.PrintError(_T("Archive %s succeeded to open, even if it should not."), szPlainName);
    SFileCloseArchive(hMpq);
    return ERROR_CAN_NOT_COMPLETE;
}

static DWORD TestOpenArchive_Corrupt(LPCTSTR szPlainName)
{
    TLogHelper Logger("OpenCorruptMpqTest", szPlainName);
    HANDLE hMpq = NULL;
    TCHAR szFullPath[MAX_PATH];

    // Copy the archive so we won't fuck up the original one
    CreateFullPathName(szFullPath, _countof(szFullPath), szMpqSubDir, szPlainName);
    if(SFileOpenArchive(szFullPath, 0, STREAM_FLAG_READ_ONLY, &hMpq))
    {
        SFileCloseArchive(hMpq);
        Logger.PrintMessage(_T("Opening archive %s succeeded, but it shouldn't"), szFullPath);
        return ERROR_CAN_NOT_COMPLETE;
    }

    return ERROR_SUCCESS;
}


// Opens a patched MPQ archive
static DWORD TestOpenArchive_Patched(LPCTSTR PatchList[], LPCSTR szPatchedFile, int nExpectedPatchCount, bool bExpectedToFail = false)
{
    TLogHelper Logger("OpenPatchedMpqTest", PatchList[0]);
    HANDLE hMpq;
    HANDLE hFile;
    BYTE Buffer[0x100];
    DWORD dwFileCount = 0;
    DWORD BytesRead = 0;
    DWORD dwErrCode;

    // Open a patched MPQ archive
    dwErrCode = OpenPatchedArchive(&Logger, &hMpq, PatchList);
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Check patch count
        if(szPatchedFile != NULL)
            dwErrCode = VerifyFilePatchCount(&Logger, hMpq, szPatchedFile, nExpectedPatchCount);

        // Try to open and read the file
        if(dwErrCode == ERROR_SUCCESS)
        {
            if(SFileOpenFileEx(hMpq, szPatchedFile, 0, &hFile))
            {
                SFileReadFile(hFile, Buffer, sizeof(Buffer), &BytesRead, NULL);
                SFileCloseFile(hFile);
            }
        }

        // Search the archive and load every file
        if(dwErrCode == ERROR_SUCCESS)
            dwErrCode = SearchArchive(&Logger, hMpq, SEARCH_FLAG_LOAD_FILES | SEARCH_FLAG_IGNORE_ERRORS, &dwFileCount);

        // Close the archive
        SFileCloseArchive(hMpq);
    }

    // Clear the error if patch prefix was not found
    if(dwErrCode == ERROR_CANT_FIND_PATCH_PREFIX && bExpectedToFail)
        dwErrCode = ERROR_SUCCESS;
    return dwErrCode;
}

// Open an archive for read-only access
static DWORD TestOpenArchive_ReadOnly(LPCTSTR szPlainName, bool bReadOnly)
{
    TLogHelper Logger("ReadOnlyTest", szPlainName);
    LPCTSTR szCopyName;
    HANDLE hMpq = NULL;
    TCHAR szFullPath[MAX_PATH];
    DWORD dwFlags = bReadOnly ? MPQ_OPEN_READ_ONLY : 0;;
    int nExpectedError;
    DWORD dwErrCode;

    // Copy the fiel so we wont screw up something
    szCopyName = bReadOnly ? _T("StormLibTest_ReadOnly.mpq") : _T("StormLibTest_ReadWrite.mpq");
    dwErrCode = CreateFileCopy(&Logger, szPlainName, szCopyName, szFullPath, _countof(szFullPath));

    // Now open the archive for read-only access
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = OpenExistingArchive(&Logger, szFullPath, dwFlags, &hMpq);

    // Now try to add a file. This must fail if the MPQ is read only
    if(dwErrCode == ERROR_SUCCESS)
    {
        nExpectedError = (bReadOnly) ? ERROR_ACCESS_DENIED : ERROR_SUCCESS;
        AddFileToMpq(&Logger, hMpq, "AddedFile.txt", "This is an added file.", MPQ_FILE_COMPRESS | MPQ_FILE_ENCRYPTED, 0, nExpectedError);
    }

    // Now try to rename a file in the MPQ. This must only succeed if the MPQ is not read only
    if(dwErrCode == ERROR_SUCCESS)
    {
        nExpectedError = (bReadOnly) ? ERROR_ACCESS_DENIED : ERROR_SUCCESS;
        RenameMpqFile(&Logger, hMpq, "spawn.mpq", "spawn-renamed.mpq", nExpectedError);
    }

    // Now try to delete a file in the MPQ. This must only succeed if the MPQ is not read only
    if(dwErrCode == ERROR_SUCCESS)
    {
        nExpectedError = (bReadOnly) ? ERROR_ACCESS_DENIED : ERROR_SUCCESS;
        RemoveMpqFile(&Logger, hMpq, "spawn-renamed.mpq", nExpectedError);
    }

    // Close the archive
    if(hMpq != NULL)
        SFileCloseArchive(hMpq);
    return dwErrCode;
}

static DWORD TestOpenArchive_GetFileInfo(LPCTSTR szPlainName1, LPCTSTR szPlainName4)
{
    TLogHelper Logger("GetFileInfoTest", szPlainName1, szPlainName4);
    HANDLE hFile;
    HANDLE hMpq4;
    HANDLE hMpq1;
    DWORD cbLength;
    BYTE DataBuff[0x400];
    DWORD dwErrCode1;
    DWORD dwErrCode4;

    // Copy the archive so we won't fuck up the original one
    dwErrCode1 = OpenExistingArchiveWithCopy(&Logger, szPlainName1, NULL, &hMpq1);
    dwErrCode4 = OpenExistingArchiveWithCopy(&Logger, szPlainName4, NULL, &hMpq4);
    if(dwErrCode1 == ERROR_SUCCESS && dwErrCode4 == ERROR_SUCCESS)
    {
        // Invalid handle - expected (false, ERROR_INVALID_HANDLE)
        TestGetFileInfo(&Logger, NULL, SFileMpqBetHeader, NULL, 0, NULL, false, ERROR_INVALID_HANDLE);

        // Valid handle but invalid value of file info class (false, ERROR_INVALID_PARAMETER)
        TestGetFileInfo(&Logger, NULL, (SFileInfoClass)0xFFF, NULL, 0, NULL, false, ERROR_INVALID_HANDLE);

        // Invalid archive handle and file info class is for file (false, ERROR_INVALID_HANDLE)
        TestGetFileInfo(&Logger, NULL, SFileInfoNameHash1, NULL, 0, NULL, false, ERROR_INVALID_HANDLE);

        // Valid handle and all parameters NULL
        // Returns (true, ERROR_SUCCESS), if BET table is present, otherwise (false, ERROR_FILE_NOT_FOUND)
        TestGetFileInfo(&Logger, hMpq1, SFileMpqBetHeader, NULL, 0, NULL, false, ERROR_FILE_NOT_FOUND);
        TestGetFileInfo(&Logger, hMpq4, SFileMpqBetHeader, NULL, 0, NULL, false, ERROR_INSUFFICIENT_BUFFER);

        // Now try to retrieve the required size of the BET table header
        TestGetFileInfo(&Logger, hMpq4, SFileMpqBetHeader, NULL, 0, &cbLength, false, ERROR_INSUFFICIENT_BUFFER);

        // When we call SFileInfo with buffer = NULL and nonzero buffer size, it is ignored
        TestGetFileInfo(&Logger, hMpq4, SFileMpqBetHeader, NULL, 3, &cbLength, false, ERROR_INSUFFICIENT_BUFFER);

        // When we call SFileInfo with buffer != NULL and nonzero buffer size, it should return error
        TestGetFileInfo(&Logger, hMpq4, SFileMpqBetHeader, DataBuff, 3, &cbLength, false, ERROR_INSUFFICIENT_BUFFER);

        // Request for bet table header should also succeed if we want header only
        TestGetFileInfo(&Logger, hMpq4, SFileMpqBetHeader, DataBuff, sizeof(TMPQBetHeader), &cbLength, true, ERROR_SUCCESS);

        // Request for bet table header should also succeed if we want header+flag table only
        TestGetFileInfo(&Logger, hMpq4, SFileMpqBetHeader, DataBuff, sizeof(DataBuff), &cbLength, true, ERROR_SUCCESS);

        // Try to retrieve strong signature from the MPQ
        TestGetFileInfo(&Logger, hMpq1, SFileMpqStrongSignature, NULL, 0, NULL, false, ERROR_INSUFFICIENT_BUFFER);
        TestGetFileInfo(&Logger, hMpq4, SFileMpqStrongSignature, NULL, 0, NULL, false, ERROR_FILE_NOT_FOUND);

        // Strong signature is returned including the signature ID
        TestGetFileInfo(&Logger, hMpq1, SFileMpqStrongSignature, NULL, 0, &cbLength, false, ERROR_INSUFFICIENT_BUFFER);
        assert(cbLength == MPQ_STRONG_SIGNATURE_SIZE + 4);

        // Retrieve the signature
        TestGetFileInfo(&Logger, hMpq1, SFileMpqStrongSignature, DataBuff, sizeof(DataBuff), &cbLength, true, ERROR_SUCCESS);
        assert(memcmp(DataBuff, "NGIS", 4) == 0);

        // Check SFileGetFileInfo on
        if(SFileOpenFileEx(hMpq4, LISTFILE_NAME, 0, &hFile))
        {
            // Valid parameters but the handle should be file handle
            TestGetFileInfo(&Logger, hMpq4, SFileInfoFileTime, DataBuff, sizeof(DataBuff), &cbLength, false, ERROR_INVALID_HANDLE);

            // Valid parameters
            TestGetFileInfo(&Logger, hFile, SFileInfoFileTime, DataBuff, sizeof(DataBuff), &cbLength, true, ERROR_SUCCESS);

            SFileCloseFile(hFile);
        }
    }

    if(hMpq4 != NULL)
        SFileCloseArchive(hMpq4);
    if(hMpq1 != NULL)
        SFileCloseArchive(hMpq1);
    return ERROR_SUCCESS;
}

static DWORD TestOpenArchive_MasterMirror(LPCTSTR szMirrorName, LPCTSTR szMasterName, LPCSTR szFileToExtract, bool bCopyMirrorFile)
{
    TFileData * pFileData;
    TLogHelper Logger("OpenServerMirror", szMirrorName);
    HANDLE hFile = NULL;
    HANDLE hMpq = NULL;
    DWORD dwVerifyResult;
    TCHAR szMirrorPath[MAX_PATH + MAX_PATH];   // Combined name
    TCHAR szMasterPath[MAX_PATH];              // Original (server) name
    DWORD dwErrCode;

    // Create both paths
    dwErrCode = CreateMasterAndMirrorPaths(&Logger, szMirrorPath, szMasterPath, szMirrorName, szMasterName, bCopyMirrorFile);

    // Now open both archives as local-server pair
    if(dwErrCode == ERROR_SUCCESS)
    {
        dwErrCode = OpenExistingArchive(&Logger, szMirrorPath, 0, &hMpq);
    }

    // The MPQ must be read-only. Writing to mirrored MPQ is not allowed
    if(dwErrCode == ERROR_SUCCESS)
    {
        if(SFileCreateFile(hMpq, "AddedFile.bin", 0, 0x10, 0, MPQ_FILE_COMPRESS, &hFile))
        {
            SFileCloseFile(hFile);
            Logger.PrintMessage("The archive is writable, although it should not be");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Verify the file
    if(dwErrCode == ERROR_SUCCESS && szFileToExtract != NULL)
    {
        dwVerifyResult = SFileVerifyFile(hMpq, szFileToExtract, SFILE_VERIFY_ALL);
        if(dwVerifyResult & VERIFY_FILE_ERROR_MASK)
        {
            Logger.PrintMessage("File verification failed");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Load the file to memory
    if(dwErrCode == ERROR_SUCCESS && szFileToExtract)
    {
        pFileData = LoadMpqFile(&Logger, hMpq, szFileToExtract);
        if(pFileData != NULL)
            STORM_FREE(pFileData);
    }

    if(hMpq != NULL)
        SFileCloseArchive(hMpq);
    return dwErrCode;
}


static DWORD TestOpenArchive_VerifySignature(LPCTSTR szPlainName, LPCTSTR szOriginalName)
{
    TLogHelper Logger("VerifySignatureTest", szPlainName);
    HANDLE hMpq;
    DWORD dwSignatures = 0;
    DWORD dwErrCode = ERROR_SUCCESS;
    int nVerifyError;

    // We need original name for the signature check
    dwErrCode = OpenExistingArchiveWithCopy(&Logger, szPlainName, szOriginalName, &hMpq);
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Query the signature types
        Logger.PrintProgress("Retrieving signatures ...");
        TestGetFileInfo(&Logger, hMpq, SFileMpqSignatures, &dwSignatures, sizeof(DWORD), NULL, true, ERROR_SUCCESS);

        // Verify any of the present signatures
        Logger.PrintProgress("Verifying archive signature ...");
        nVerifyError = SFileVerifyArchive(hMpq);

        // Verify the result
        if((dwSignatures & SIGNATURE_TYPE_STRONG) && (nVerifyError != ERROR_STRONG_SIGNATURE_OK))
        {
            Logger.PrintMessage("Strong signature verification error");
            dwErrCode = ERROR_FILE_CORRUPT;
        }

        // Verify the result
        if((dwSignatures & SIGNATURE_TYPE_WEAK) && (nVerifyError != ERROR_WEAK_SIGNATURE_OK))
        {
            Logger.PrintMessage("Weak signature verification error");
            dwErrCode = ERROR_FILE_CORRUPT;
        }

        SFileCloseArchive(hMpq);
    }
    return dwErrCode;
}

static DWORD TestOpenArchive_ModifySigned(LPCTSTR szPlainName, LPCTSTR szOriginalName)
{
    TLogHelper Logger("ModifySignedTest", szPlainName);
    HANDLE hMpq = NULL;
    int nVerifyError;
    DWORD dwErrCode = ERROR_SUCCESS;

    // We need original name for the signature check
    dwErrCode = OpenExistingArchiveWithCopy(&Logger, szPlainName, szOriginalName, &hMpq);
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Verify the weak signature
        Logger.PrintProgress("Verifying archive signature ...");
        nVerifyError = SFileVerifyArchive(hMpq);

        // Check the result signature
        if(nVerifyError != ERROR_WEAK_SIGNATURE_OK)
        {
            Logger.PrintMessage("Weak signature verification error");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Add a file and verify the signature again
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Verify any of the present signatures
        Logger.PrintProgress("Modifying signed archive ...");
        dwErrCode = AddFileToMpq(&Logger, hMpq, "AddedFile01.txt", "This is a file added to signed MPQ", 0, 0, ERROR_SUCCESS);
    }

    // Verify the signature again
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Verify the weak signature
        Logger.PrintProgress("Verifying archive signature ...");
        nVerifyError = SFileVerifyArchive(hMpq);

        // Check the result signature
        if(nVerifyError != ERROR_WEAK_SIGNATURE_OK)
        {
            Logger.PrintMessage("Weak signature verification error");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Close the MPQ
    if(hMpq != NULL)
        SFileCloseArchive(hMpq);
    return dwErrCode;
}

static DWORD TestOpenArchive_SignExisting(LPCTSTR szPlainName)
{
    TLogHelper Logger("SignExistingMpq", szPlainName);
    HANDLE hMpq = NULL;
    int nVerifyError;
    DWORD dwErrCode = ERROR_SUCCESS;

    // We need original name for the signature check
    dwErrCode = OpenExistingArchiveWithCopy(&Logger, szPlainName, szPlainName, &hMpq);
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Verify the weak signature
        Logger.PrintProgress("Verifying archive signature ...");
        nVerifyError = SFileVerifyArchive(hMpq);

        // Check the result signature
        if(nVerifyError != ERROR_NO_SIGNATURE)
        {
            Logger.PrintMessage("There already is a signature in the MPQ");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Add a file and verify the signature again
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Verify any of the present signatures
        Logger.PrintProgress("Signing the MPQ ...");
        if(!SFileSignArchive(hMpq, SIGNATURE_TYPE_WEAK))
        {
            Logger.PrintMessage("Failed to create archive signature");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Verify the signature again
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Verify the weak signature
        Logger.PrintProgress("Verifying archive signature ...");
        nVerifyError = SFileVerifyArchive(hMpq);

        // Check the result signature
        if(nVerifyError != ERROR_WEAK_SIGNATURE_OK)
        {
            Logger.PrintMessage("Weak signature verification error");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Close the MPQ
    if(hMpq != NULL)
        SFileCloseArchive(hMpq);
    return dwErrCode;
}

static DWORD TestOpenArchive_CompactArchive(LPCTSTR szPlainName, LPCTSTR szCopyName, bool bAddUserData)
{
    TLogHelper Logger("CompactMpqTest", szPlainName);
	ULONGLONG PreMpqDataSize = (bAddUserData) ? 0x400 : 0;
	ULONGLONG UserDataSize = (bAddUserData) ? 0x531 : 0;
	HANDLE hMpq;
    DWORD dwFileCount1 = 0;
    DWORD dwFileCount2 = 0;
    TCHAR szFullPath[MAX_PATH];
    BYTE FileHash1[MD5_DIGEST_SIZE];
    BYTE FileHash2[MD5_DIGEST_SIZE];
    DWORD dwErrCode;

    // Create copy of the archive, with interleaving some user data
    dwErrCode = CreateFileCopy(&Logger, szPlainName, szCopyName, szFullPath, _countof(szFullPath), PreMpqDataSize, UserDataSize);

    // Open the archive and load some files
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Open the archive
        dwErrCode = OpenExistingArchive(&Logger, szFullPath, 0, &hMpq);
        if(dwErrCode != ERROR_SUCCESS)
            return dwErrCode;

        // Verify presence of (listfile) and (attributes)
        CheckIfFileIsPresent(&Logger, hMpq, LISTFILE_NAME, true);
        CheckIfFileIsPresent(&Logger, hMpq, ATTRIBUTES_NAME, true);

        // Search the archive and load every file
        dwErrCode = SearchArchive(&Logger, hMpq, SEARCH_FLAG_LOAD_FILES | SEARCH_FLAG_HASH_FILES, &dwFileCount1, FileHash1);
        SFileCloseArchive(hMpq);
    }

    // Try to compact the MPQ
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Open the archive again
        dwErrCode = OpenExistingArchive(&Logger, szFullPath, 0, &hMpq);
        if(dwErrCode != ERROR_SUCCESS)
            return dwErrCode;

        // Compact the archive
        Logger.PrintProgress("Compacting archive %s ...", GetShortPlainName(szFullPath));
        if(!SFileSetCompactCallback(hMpq, CompactCallback, &Logger))
            dwErrCode = Logger.PrintError(_T("Failed to compact archive %s"), szFullPath);

        SFileCompactArchive(hMpq, NULL, false);
        SFileCloseArchive(hMpq);
    }

    // Open the archive and load some files
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Open the archive
        dwErrCode = OpenExistingArchive(&Logger, szFullPath, 0, &hMpq);
        if(dwErrCode != ERROR_SUCCESS)
            return dwErrCode;

        // Verify presence of (listfile) and (attributes)
        CheckIfFileIsPresent(&Logger, hMpq, LISTFILE_NAME, true);
        CheckIfFileIsPresent(&Logger, hMpq, ATTRIBUTES_NAME, true);

        // Search the archive and load every file
        dwErrCode = SearchArchive(&Logger, hMpq, SEARCH_FLAG_LOAD_FILES | SEARCH_FLAG_HASH_FILES, &dwFileCount2, FileHash2);
        SFileCloseArchive(hMpq);
    }

    // Compare the file counts and their hashes
    if(dwErrCode == ERROR_SUCCESS)
    {
        if(dwFileCount2 != dwFileCount1)
            Logger.PrintMessage("Different file count after compacting archive: %u vs %u", dwFileCount2, dwFileCount1);

        if(memcmp(FileHash2, FileHash1, MD5_DIGEST_SIZE))
            Logger.PrintMessage("Different file hash after compacting archive");
    }

    return dwErrCode;
}

static DWORD ForEachFile_VerifyFileChecksum(LPCTSTR szFullPath)
{
    TFileData * pFileData;
    TCHAR * szExtension;
    TCHAR szShaFileName[MAX_PATH+1];
    TCHAR szSha1Text[0x40];
    char szSha1TextA[0x40];
    DWORD dwErrCode = ERROR_SUCCESS;

    // Try to load the file with the SHA extension
    StringCopy(szShaFileName, _countof(szShaFileName), szFullPath);
    szExtension = _tcsrchr(szShaFileName, '.');
    if(szExtension == NULL)
        return ERROR_SUCCESS;

    // Skip .SHA and .TXT files
    if(!_tcsicmp(szExtension, _T(".sha")) || !_tcsicmp(szExtension, _T(".txt")))
        return ERROR_SUCCESS;

    // Load the local file to memory
    _tcscpy(szExtension, _T(".sha"));
    pFileData = LoadLocalFile(NULL, szShaFileName, false);
    if(pFileData != NULL)
    {
        TLogHelper Logger("VerifyFileHash");

        // Calculate SHA1 of the entire file
        dwErrCode = CalculateFileSha1(&Logger, szFullPath, szSha1Text);
        if(dwErrCode == ERROR_SUCCESS)
        {
            // Compare with what we loaded from the file
            if(pFileData->dwFileSize >= (SHA1_DIGEST_SIZE * 2))
            {
                // Compare the SHA1
                StringCopy(szSha1TextA, _countof(szSha1TextA), szSha1Text);
                if(_strnicmp(szSha1TextA, (char *)pFileData->FileData, (SHA1_DIGEST_SIZE * 2)))
                {
                    SetLastError(dwErrCode = ERROR_FILE_CORRUPT);
                    Logger.PrintError(_T("File CRC check failed: %s"), szFullPath);
                }
            }
        }

        STORM_FREE(pFileData);
    }

    return dwErrCode;
}

// Opens a found archive
static DWORD ForEachFile_OpenArchive(LPCTSTR szFullPath)
{
    HANDLE hMpq = NULL;
    DWORD dwFileCount = 0;
    DWORD dwErrCode = ERROR_SUCCESS;

    // Check if it's a MPQ file type
    if(IsMpqExtension(szFullPath))
    {
        TLogHelper Logger("OpenEachMpqTest", GetShortPlainName(szFullPath));

        // Open the MPQ name
        dwErrCode = OpenExistingArchive(&Logger, szFullPath, 0, &hMpq);
        if(dwErrCode == ERROR_AVI_FILE || dwErrCode == ERROR_FILE_CORRUPT || dwErrCode == ERROR_BAD_FORMAT)
            return ERROR_SUCCESS;

        // Search the archive and load every file
        if(dwErrCode == ERROR_SUCCESS)
        {
            dwErrCode = SearchArchive(&Logger, hMpq, 0, &dwFileCount);
            SFileCloseArchive(hMpq);
        }

        // Show warning if no files found
        if(dwFileCount == 0)
        {
            Logger.PrintMessage("Warning: no files in the archive");
        }
    }

    // Correct some errors
    if(dwErrCode == ERROR_FILE_CORRUPT || dwErrCode == ERROR_FILE_INCOMPLETE)
        return ERROR_SUCCESS;
    return dwErrCode;
}

// Adding a file to MPQ that had size of the file table equal
// or greater than hash table, but has free entries
static DWORD TestAddFile_FullTable(LPCTSTR szFullMpqName)
{
    TLogHelper Logger("FullMpqTest", szFullMpqName);
    LPCSTR szFileName = "AddedFile001.txt";
    LPCSTR szFileData = "0123456789ABCDEF";
    HANDLE hMpq = NULL;
    DWORD dwErrCode = ERROR_SUCCESS;

    // Copy the archive so we won't fuck up the original one
    dwErrCode = OpenExistingArchiveWithCopy(&Logger, szFullMpqName, szFullMpqName, &hMpq);
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Attempt to add a file
        dwErrCode = AddFileToMpq(&Logger, hMpq, szFileName, szFileData, MPQ_FILE_IMPLODE, MPQ_COMPRESSION_PKWARE, ERROR_SUCCESS);
        SFileCloseArchive(hMpq);
    }

    return dwErrCode;
}

// Adding a file to MPQ that had no (listfile) and no (attributes).
// We expect that neither of these will be present after the archive is closed
static DWORD TestAddFile_ListFileTest(LPCTSTR szSourceMpq, bool bShouldHaveListFile, bool bShouldHaveAttributes)
{
    TLogHelper Logger("ListFileTest", szSourceMpq);
    TFileData * pFileData = NULL;
    LPCTSTR szBackupMpq = bShouldHaveListFile ? _T("StormLibTest_HasListFile.mpq") : _T("StormLibTest_NoListFile.mpq");
    LPCSTR szFileName = "AddedFile001.txt";
    LPCSTR szFileData = "0123456789ABCDEF";
    HANDLE hMpq = NULL;
    DWORD dwFileSize = (DWORD)strlen(szFileData);
    DWORD dwErrCode = ERROR_SUCCESS;

    // Copy the archive so we won't fuck up the original one
    dwErrCode = OpenExistingArchiveWithCopy(&Logger, szSourceMpq, szBackupMpq, &hMpq);

    // Add a file
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Now add a file
        dwErrCode = AddFileToMpq(&Logger, hMpq, szFileName, szFileData, MPQ_FILE_IMPLODE, MPQ_COMPRESSION_PKWARE);
        SFileCloseArchive(hMpq);
        hMpq = NULL;
    }

    // Now reopen the archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = OpenExistingArchiveWithCopy(&Logger, NULL, szBackupMpq, &hMpq);

    // Now the file has been written and the MPQ has been saved.
    // We Reopen the MPQ and check if there is no (listfile) nor (attributes).
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Verify presence of (listfile) and (attributes)
        CheckIfFileIsPresent(&Logger, hMpq, LISTFILE_NAME, bShouldHaveListFile);
        CheckIfFileIsPresent(&Logger, hMpq, ATTRIBUTES_NAME, bShouldHaveAttributes);

        // Try to open the file that we recently added
        pFileData = LoadMpqFile(&Logger, hMpq, szFileName);
        if(pFileData != NULL)
        {
            // Verify if the file size matches
            if(pFileData->dwFileSize == dwFileSize)
            {
                // Verify if the file data match
                if(memcmp(pFileData->FileData, szFileData, dwFileSize))
                {
                    Logger.PrintError("The data of the added file does not match");
                    dwErrCode = ERROR_FILE_CORRUPT;
                }
            }
            else
            {
                Logger.PrintError("The size of the added file does not match");
                dwErrCode = ERROR_FILE_CORRUPT;
            }

            // Delete the file data
            STORM_FREE(pFileData);
        }
        else
        {
            dwErrCode = Logger.PrintError("Failed to open the file previously added");
        }
    }

    // Close the MPQ archive
    if(hMpq != NULL)
        SFileCloseArchive(hMpq);
    return dwErrCode;
}
/*
static DWORD TestCreateArchive_Deprotect(LPCSTR szPlainName)
{
    TLogHelper Logger("DeprotectTest", szPlainName);
    HANDLE hMpq1 = NULL;
    HANDLE hMpq2 = NULL;
    char szMpqName1[MAX_PATH];
    char szMpqName2[MAX_PATH];
    BYTE FileHash1[MD5_DIGEST_SIZE];
    BYTE FileHash2[MD5_DIGEST_SIZE];
    DWORD dwFileCount1 = 0;
    DWORD dwFileCount2 = 0;
    DWORD dwTestFlags = SEARCH_FLAG_LOAD_FILES | SEARCH_FLAG_HASH_FILES;
    DWORD dwErrCode = ERROR_SUCCESS;

    // First copy: The original (untouched) file
    if(dwErrCode == ERROR_SUCCESS)
    {
        AddStringBeforeExtension(szMpqName1, szPlainName, "_original");
        dwErrCode = OpenExistingArchiveWithCopy(&Logger, szPlainName, szMpqName1, &hMpq1);
        if(dwErrCode != ERROR_SUCCESS)
            Logger.PrintMessage("Open failed: %s", szMpqName1);
    }

    // Second copy: Will be deprotected
    if(dwErrCode == ERROR_SUCCESS)
    {
        AddStringBeforeExtension(szMpqName2, szPlainName, "_deprotected");
        dwErrCode = OpenExistingArchiveWithCopy(&Logger, szPlainName, szMpqName2, &hMpq2);
        if(dwErrCode != ERROR_SUCCESS)
            Logger.PrintMessage("Open failed: %s", szMpqName2);
    }

    // Deprotect the second map
    if(dwErrCode == ERROR_SUCCESS)
    {
        SFileSetCompactCallback(hMpq2, CompactCallback, &Logger);
        if(!SFileCompactArchive(hMpq2, NULL, false))
            dwErrCode = Logger.PrintError("Failed to deprotect archive %s", szMpqName2);
    }

    // Calculate number of files and compare their hash (archive 1)
    if(dwErrCode == ERROR_SUCCESS)
    {
        memset(FileHash1, 0, sizeof(FileHash1));
        dwErrCode = SearchArchive(&Logger, hMpq1, dwTestFlags, &dwFileCount1, FileHash1);
    }

    // Calculate number of files and compare their hash (archive 2)
    if(dwErrCode == ERROR_SUCCESS)
    {
        memset(FileHash1, 0, sizeof(FileHash2));
        dwErrCode = SearchArchive(&Logger, hMpq2, dwTestFlags, &dwFileCount2, FileHash2);
    }

    if(dwErrCode == ERROR_SUCCESS)
    {
        if(dwFileCount1 != dwFileCount2)
            Logger.PrintMessage("Different file count (%u in %s; %u in %s)", dwFileCount1, szMpqName1, dwFileCount2, szMpqName2);
        if(memcmp(FileHash1, FileHash2, MD5_DIGEST_SIZE))
            Logger.PrintMessage("Different file hash (%s vs %s)", szMpqName1, szMpqName2);
    }

    // Close both MPQs
    if(hMpq2 != NULL)
        SFileCloseArchive(hMpq2);
    if(hMpq1 != NULL)
        SFileCloseArchive(hMpq1);
    return dwErrCode;
}
*/

static DWORD TestCreateArchive_EmptyMpq(LPCTSTR szPlainName, DWORD dwCreateFlags)
{
    TLogHelper Logger("CreateEmptyMpq", szPlainName);
    HANDLE hMpq = NULL;
    DWORD dwFileCount = 0;
    DWORD dwErrCode;

    // Create the full path name
    dwErrCode = CreateNewArchive(&Logger, szPlainName, dwCreateFlags, 0, &hMpq);
    if(dwErrCode == ERROR_SUCCESS)
    {
        SearchArchive(&Logger, hMpq);
        SFileCloseArchive(hMpq);
    }

    // Reopen the empty MPQ
    if(dwErrCode == ERROR_SUCCESS)
    {
        dwErrCode = OpenExistingArchiveWithCopy(&Logger, NULL, szPlainName, &hMpq);
        if(dwErrCode == ERROR_SUCCESS)
        {
            SFileGetFileInfo(hMpq, SFileMpqNumberOfFiles, &dwFileCount, sizeof(dwFileCount), NULL);

            CheckIfFileIsPresent(&Logger, hMpq, "File00000000.xxx", false);
            CheckIfFileIsPresent(&Logger, hMpq, LISTFILE_NAME, false);
            SearchArchive(&Logger, hMpq);
            SFileCloseArchive(hMpq);
        }
    }

    return dwErrCode;
}

static DWORD TestCreateArchive_TestGaps(LPCTSTR szPlainName)
{
    TLogHelper Logger("CreateGapsTest", szPlainName);
    ULONGLONG ByteOffset1 = 0xFFFFFFFF;
    ULONGLONG ByteOffset2 = 0xEEEEEEEE;
    HANDLE hMpq = NULL;
    HANDLE hFile = NULL;
    TCHAR szFullPath[MAX_PATH];
    DWORD dwErrCode = ERROR_SUCCESS;

    // Create new MPQ
    dwErrCode = CreateNewArchive_V2(&Logger, szPlainName, MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES | MPQ_FORMAT_VERSION_4, 4000, &hMpq);
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Add one file and flush the archive
        dwErrCode = AddFileToMpq(&Logger, hMpq, "AddedFile01.txt", "This is the file data.", MPQ_FILE_COMPRESS);
        SFileCloseArchive(hMpq);
        hMpq = NULL;
    }

    // Reopen the MPQ and add another file.
    // The new file must be added to the position of the (listfile)
    if(dwErrCode == ERROR_SUCCESS)
    {
        CreateFullPathName(szFullPath, _countof(szFullPath), NULL, szPlainName);
        dwErrCode = OpenExistingArchive(&Logger, szFullPath, 0, &hMpq);
        if(dwErrCode == ERROR_SUCCESS)
        {
            // Retrieve the position of the (listfile)
            if(SFileOpenFileEx(hMpq, LISTFILE_NAME, 0, &hFile))
            {
                SFileGetFileInfo(hFile, SFileInfoByteOffset, &ByteOffset1, sizeof(ULONGLONG), NULL);
                SFileCloseFile(hFile);
            }
            else
                dwErrCode = GetLastError();
        }
    }

    // Add another file and check its position. It must be at the position of the former listfile
    if(dwErrCode == ERROR_SUCCESS)
    {
        LPCSTR szAddedFile = "AddedFile02.txt";

        // Add another file
        dwErrCode = AddFileToMpq(&Logger, hMpq, szAddedFile, "This is the second added file.", MPQ_FILE_COMPRESS);

        // Retrieve the position of the (listfile)
        if(SFileOpenFileEx(hMpq, szAddedFile, 0, &hFile))
        {
            SFileGetFileInfo(hFile, SFileInfoByteOffset, &ByteOffset2, sizeof(ULONGLONG), NULL);
            SFileCloseFile(hFile);
        }
        else
            dwErrCode = GetLastError();
    }

    // Now check the positions
    if(dwErrCode == ERROR_SUCCESS)
    {
        if(ByteOffset1 != ByteOffset2)
        {
            Logger.PrintError("The added file was not written to the position of (listfile)");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Close the archive if needed
    if(hMpq != NULL)
        SFileCloseArchive(hMpq);
    return dwErrCode;
}

static DWORD TestCreateArchive_NonStdNames(LPCTSTR szPlainName)
{
    TLogHelper Logger("NonStdNamesTest", szPlainName);
    HANDLE hMpq = NULL;
    DWORD dwErrCode = ERROR_SUCCESS;

    // Create new MPQ
    dwErrCode = CreateNewArchive(&Logger, szPlainName, MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES | MPQ_FORMAT_VERSION_1, 4000, &hMpq);
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Add few files and close the archive
        AddFileToMpq(&Logger, hMpq, "AddedFile000.txt", "This is the file data 000.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "\\/\\/\\/\\AddedFile001.txt", "This is the file data 001.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "\\\\\\\\\\\\\\\\", "This is the file data 002.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "////////////////", "This is the file data 003.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "//\\//\\//\\//\\", "This is the file data 004.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "................", "This is the file data 005.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "//****//****//****//****.***", "This is the file data 006.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "//*??*//*??*//*??*//?**?.?*?", "This is the file data 007.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "\\/\\/File.txt", "This is the file data 008.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "\\/\\/File.txt..", "This is the file data 009.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "Dir1\\Dir2\\Dir3\\File.txt..", "This is the file data 010.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "\\Dir1\\Dir2\\Dir3\\File.txt..", "This is the file data 011.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "\\\\Dir1\\\\Dir2\\\\Dir3\\\\File.txt..", "This is the file data 012.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "/Dir1/Dir2/Dir3/File.txt..", "This is the file data 013.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "////Dir1////Dir2////Dir3////File.txt..", "This is the file data 014.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "\\//\\Dir1\\//\\Dir2\\//\\File.txt..", "This is the file data 015.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "\x10\x11\x12\x13\\\x14\x15\x16\x17\\\x18\x19\x1a\x1b\\\x1c\x1D\x1E\x1F.txt", "This is the file data 016.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "\x09\x20\x09\x20\\\x20\x09\x20\x09\\\x09\x20\x09\x20\\\x20\x09\x20\x09.txt", "This is the file data 017.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "\x80\x91\xA2\xB3\\\xC4\xD5\xE6\xF7\\\x80\x91\xA2\xB3.txt", "This is the file data 018.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "Dir1\x20\x09\x20\\Dir2\x20\x09\x20\\File.txt\x09\x09\x20\x2e", "This is the file data 019.", MPQ_FILE_COMPRESS);
        AddFileToMpq(&Logger, hMpq, "Dir1\x20\x09\x20\\Dir2\x20\x09\x20\\\x09\x20\x2e\x09\x20\x2e", "This is the file data 020.", MPQ_FILE_COMPRESS);

        SFileCloseArchive(hMpq);
    }

    return ERROR_SUCCESS;
}

static DWORD TestCreateArchive_Signed(LPCTSTR szPlainName, bool bSignAtCreate)
{
    TLogHelper Logger("CreateSignedMpq", szPlainName);
    HANDLE hMpq = NULL;
    DWORD dwCreateFlags = MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES | MPQ_FORMAT_VERSION_1;
    DWORD dwSignatures = 0;
    DWORD nVerifyError = 0;
    DWORD dwErrCode = ERROR_SUCCESS;

    // Method 1: Create the archive as signed
    if(bSignAtCreate)
        dwCreateFlags |= MPQ_CREATE_SIGNATURE;

    // Create new MPQ
    dwErrCode = CreateNewArchive_V2(&Logger, szPlainName, dwCreateFlags, 4000, &hMpq);
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Add one file and flush the archive
        dwErrCode = AddFileToMpq(&Logger, hMpq, "AddedFile01.txt", "This is the file data.", MPQ_FILE_COMPRESS);
    }

    // Sign the archive with weak signature
    if(dwErrCode == ERROR_SUCCESS)
    {
        if(!SFileSignArchive(hMpq, SIGNATURE_TYPE_WEAK))
            dwErrCode = ERROR_SUCCESS;
    }

    // Reopen the MPQ and add another file.
    // The new file must be added to the position of the (listfile)
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Query the signature types
        Logger.PrintProgress("Retrieving signatures ...");
        TestGetFileInfo(&Logger, hMpq, SFileMpqSignatures, &dwSignatures, sizeof(DWORD), NULL, true, ERROR_SUCCESS);

        // Verify any of the present signatures
        Logger.PrintProgress("Verifying archive signature ...");
        nVerifyError = SFileVerifyArchive(hMpq);

        // Verify the result
        if((dwSignatures != SIGNATURE_TYPE_WEAK) && (nVerifyError != ERROR_WEAK_SIGNATURE_OK))
        {
            Logger.PrintMessage("Weak signature verification error");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Close the archive
    if(hMpq != NULL)
        SFileCloseArchive(hMpq);
    return dwErrCode;
}

static DWORD TestCreateArchive_MpqEditor(LPCTSTR szPlainName, LPCSTR szFileName)
{
    TLogHelper Logger("CreateMpqEditor", szPlainName);
    HANDLE hMpq = NULL;
    DWORD dwErrCode = ERROR_SUCCESS;

    // Create new MPQ
    dwErrCode = CreateNewArchive_V2(&Logger, szPlainName, MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES, 4000, &hMpq);
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Flush the archive first
        SFileFlushArchive(hMpq);

        // Add one file
        dwErrCode = AddFileToMpq(&Logger, hMpq, szFileName, "This is the file data.", MPQ_FILE_COMPRESS);

        // Flush the archive again
        SFileFlushArchive(hMpq);
        SFileCloseArchive(hMpq);
    }
    else
    {
        dwErrCode = GetLastError();
    }

    return dwErrCode;
}

static DWORD TestCreateArchive_FillArchive(LPCTSTR szPlainName, DWORD dwCreateFlags)
{
    TLogHelper Logger("CreateFullMpq", szPlainName);
    LPCSTR szFileData = "TestCreateArchive_FillArchive: Testing file data";
    char szFileName[MAX_PATH];
    HANDLE hMpq = NULL;
    DWORD dwMaxFileCount = 6;
    DWORD dwCompression = MPQ_COMPRESSION_ZLIB;
    DWORD dwFlags = MPQ_FILE_ENCRYPTED | MPQ_FILE_COMPRESS;
    DWORD dwErrCode;

    //
    // Note that StormLib will round the maxfile count
    // up to hash table size (nearest power of two)
    //
    if((dwCreateFlags & MPQ_CREATE_LISTFILE) == 0)
        dwMaxFileCount++;
    if((dwCreateFlags & MPQ_CREATE_ATTRIBUTES) == 0)
        dwMaxFileCount++;

    // Create the new MPQ archive
    dwErrCode = CreateNewArchive_V2(&Logger, szPlainName, dwCreateFlags, dwMaxFileCount, &hMpq);
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Flush the archive first
        SFileFlushArchive(hMpq);

        // Add all files
        for(unsigned int i = 0; i < dwMaxFileCount; i++)
        {
            sprintf(szFileName, "AddedFile%03u.txt", i);
            dwErrCode = AddFileToMpq(&Logger, hMpq, szFileName, szFileData, dwFlags, dwCompression);
            if(dwErrCode != ERROR_SUCCESS)
                break;
        }

        // Flush the archive again
        SFileFlushArchive(hMpq);
    }

    // Now the MPQ should be full. It must not be possible to add another file
    if(dwErrCode == ERROR_SUCCESS)
    {
        dwErrCode = AddFileToMpq(&Logger, hMpq, "ShouldNotBeHere.txt", szFileData, MPQ_FILE_COMPRESS, MPQ_COMPRESSION_ZLIB, ERROR_DISK_FULL);
        assert(dwErrCode != ERROR_SUCCESS);
        dwErrCode = ERROR_SUCCESS;
    }

    // Close the archive to enforce saving all tables
    if(hMpq != NULL)
        SFileCloseArchive(hMpq);
    hMpq = NULL;

    // Reopen the archive again
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = OpenExistingArchiveWithCopy(&Logger, NULL, szPlainName, &hMpq);

    // The archive should still be full
    if(dwErrCode == ERROR_SUCCESS)
    {
        CheckIfFileIsPresent(&Logger, hMpq, LISTFILE_NAME, (dwCreateFlags & MPQ_CREATE_LISTFILE) ? true : false);
        CheckIfFileIsPresent(&Logger, hMpq, ATTRIBUTES_NAME, (dwCreateFlags & MPQ_CREATE_ATTRIBUTES) ? true : false);
        dwErrCode = AddFileToMpq(&Logger, hMpq, "ShouldNotBeHere.txt", szFileData, MPQ_FILE_COMPRESS, MPQ_COMPRESSION_ZLIB, ERROR_DISK_FULL);
        assert(dwErrCode != ERROR_SUCCESS);
        dwErrCode = ERROR_SUCCESS;
    }

    // The (listfile) and (attributes) must be present
    if(dwErrCode == ERROR_SUCCESS)
    {
        CheckIfFileIsPresent(&Logger, hMpq, LISTFILE_NAME, (dwCreateFlags & MPQ_CREATE_LISTFILE) ? true : false);
        CheckIfFileIsPresent(&Logger, hMpq, ATTRIBUTES_NAME, (dwCreateFlags & MPQ_CREATE_ATTRIBUTES) ? true : false);
        dwErrCode = RemoveMpqFile(&Logger, hMpq, szFileName, ERROR_SUCCESS);
    }

    // Now add the file again. This time, it should be possible OK
    if(dwErrCode == ERROR_SUCCESS)
    {
        dwErrCode = AddFileToMpq(&Logger, hMpq, szFileName, szFileData, dwFlags, dwCompression, ERROR_SUCCESS);
        assert(dwErrCode == ERROR_SUCCESS);
    }

    // Now add the file again. This time, it should fail
    if(dwErrCode == ERROR_SUCCESS)
    {
        dwErrCode = AddFileToMpq(&Logger, hMpq, szFileName, szFileData, dwFlags, dwCompression, ERROR_ALREADY_EXISTS);
        assert(dwErrCode != ERROR_SUCCESS);
        dwErrCode = ERROR_SUCCESS;
    }

    // Now add the file again. This time, it should fail
    if(dwErrCode == ERROR_SUCCESS)
    {
        dwErrCode = AddFileToMpq(&Logger, hMpq, "ShouldNotBeHere.txt", szFileData, dwFlags, dwCompression, ERROR_DISK_FULL);
        assert(dwErrCode != ERROR_SUCCESS);
        dwErrCode = ERROR_SUCCESS;
    }

    // Close the archive and return
    if(hMpq != NULL)
        SFileCloseArchive(hMpq);
    hMpq = NULL;

    // Reopen the archive for the third time to verify that both internal files are there
    if(dwErrCode == ERROR_SUCCESS)
    {
        dwErrCode = OpenExistingArchiveWithCopy(&Logger, NULL, szPlainName, &hMpq);
        if(dwErrCode == ERROR_SUCCESS)
        {
            CheckIfFileIsPresent(&Logger, hMpq, LISTFILE_NAME, (dwCreateFlags & MPQ_CREATE_LISTFILE) ? true : false);
            CheckIfFileIsPresent(&Logger, hMpq, ATTRIBUTES_NAME, (dwCreateFlags & MPQ_CREATE_ATTRIBUTES) ? true : false);
            SFileCloseArchive(hMpq);
        }
    }

    return dwErrCode;
}

static DWORD TestCreateArchive_IncMaxFileCount(LPCTSTR szPlainName)
{
    TLogHelper Logger("IncMaxFileCount", szPlainName);
    LPCSTR szFileData = "TestCreateArchive_IncMaxFileCount: Testing file data";
    char szFileName[MAX_PATH];
    HANDLE hMpq = NULL;
    DWORD dwMaxFileCount = 1;
    DWORD dwErrCode;

    // Create the new MPQ
    dwErrCode = CreateNewArchive(&Logger, szPlainName, MPQ_CREATE_ARCHIVE_V4 | MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES, dwMaxFileCount, &hMpq);

    // Now add exactly one file
    if(dwErrCode == ERROR_SUCCESS)
    {
        dwErrCode = AddFileToMpq(&Logger, hMpq, "AddFile_base.txt", szFileData);
        SFileFlushArchive(hMpq);
        SFileCloseArchive(hMpq);
    }

    // Now add 10 files. Each time we cannot add the file due to archive being full,
    // we increment the max file count
    if(dwErrCode == ERROR_SUCCESS)
    {
        for(unsigned int i = 0; i < 10; i++)
        {
            // Open the archive again
            dwErrCode = OpenExistingArchiveWithCopy(&Logger, NULL, szPlainName, &hMpq);
            if(dwErrCode != ERROR_SUCCESS)
                break;

            // Add one file
            sprintf(szFileName, "AddFile_%04u.txt", i);
            dwErrCode = AddFileToMpq(&Logger, hMpq, szFileName, szFileData, 0, 0, ERROR_UNDETERMINED_RESULT);
            if(dwErrCode != ERROR_SUCCESS)
            {
                // Increment the max file count by one
                dwMaxFileCount = SFileGetMaxFileCount(hMpq) + 1;
                Logger.PrintProgress("Increasing max file count to %u ...", dwMaxFileCount);
                SFileSetMaxFileCount(hMpq, dwMaxFileCount);

                // Attempt to create the file again
                dwErrCode = AddFileToMpq(&Logger, hMpq, szFileName, szFileData, 0, 0, ERROR_SUCCESS);
            }

            // Compact the archive and close it
            SFileSetCompactCallback(hMpq, CompactCallback, &Logger);
            SFileCompactArchive(hMpq, NULL, false);
            SFileCloseArchive(hMpq);
            if(dwErrCode != ERROR_SUCCESS)
                break;
        }
    }

    return dwErrCode;
}

static DWORD TestCreateArchive_UnicodeNames()
{
    TLogHelper Logger("MpqUnicodeName");
    DWORD dwCreateFlags = MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES;
    DWORD dwErrCode = ERROR_SUCCESS;

    dwErrCode = CreateNewArchiveU(&Logger, szUnicodeName1, dwCreateFlags | MPQ_CREATE_ARCHIVE_V1, 15);
    if(dwErrCode != ERROR_SUCCESS)
        return dwErrCode;

    dwErrCode = CreateNewArchiveU(&Logger, szUnicodeName2, dwCreateFlags | MPQ_CREATE_ARCHIVE_V2, 58);
    if(dwErrCode != ERROR_SUCCESS)
        return dwErrCode;

    dwErrCode = CreateNewArchiveU(&Logger, szUnicodeName3, dwCreateFlags | MPQ_CREATE_ARCHIVE_V3, 15874);
    if(dwErrCode != ERROR_SUCCESS)
        return dwErrCode;

    dwErrCode = CreateNewArchiveU(&Logger, szUnicodeName4, dwCreateFlags | MPQ_CREATE_ARCHIVE_V4, 87541);
    if(dwErrCode != ERROR_SUCCESS)
        return dwErrCode;

    dwErrCode = CreateNewArchiveU(&Logger, szUnicodeName5, dwCreateFlags | MPQ_CREATE_ARCHIVE_V3, 87541);
    if(dwErrCode != ERROR_SUCCESS)
        return dwErrCode;

    dwErrCode = CreateNewArchiveU(&Logger, szUnicodeName5, dwCreateFlags | MPQ_CREATE_ARCHIVE_V2, 87541);
    if(dwErrCode != ERROR_SUCCESS)
        return dwErrCode;

    return dwErrCode;
}

static DWORD TestCreateArchive_FileFlagTest(LPCTSTR szPlainName)
{
    TLogHelper Logger("FileFlagTest", szPlainName);
    HANDLE hMpq = NULL;                 // Handle of created archive
    TCHAR szFileName1[MAX_PATH];
    TCHAR szFileName2[MAX_PATH];
    TCHAR szFullPath[MAX_PATH];
    LPCSTR szMiddleFile = "FileTest_10.exe";
    LCID LocaleIDs[] = {0x000, 0x405, 0x406, 0x407, 0xFFFF};
    char szArchivedName[MAX_PATH];
    DWORD dwMaxFileCount = 0;
    DWORD dwFileCount = 0;
    DWORD dwErrCode;

    // Create paths for local file to be added
    CreateFullPathName(szFileName1, _countof(szFileName1), szMpqSubDir, _T("AddFile.exe"));
    CreateFullPathName(szFileName2, _countof(szFileName2), szMpqSubDir, _T("AddFile.bin"));

    // Create an empty file that will serve as holder for the MPQ
    dwErrCode = CreateEmptyFile(&Logger, szPlainName, 0x100000, szFullPath);

    // Create new MPQ archive over that file
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = CreateNewArchive(&Logger, szPlainName, MPQ_CREATE_ARCHIVE_V1 | MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES, 17, &hMpq);

    // Add the same file multiple times
    if(dwErrCode == ERROR_SUCCESS)
    {
        dwMaxFileCount = SFileGetMaxFileCount(hMpq);
        for(size_t i = 0; AddFlags[i] != 0xFFFFFFFF; i++)
        {
            sprintf(szArchivedName, "FileTest_%02u.exe", (unsigned int)i);
            dwErrCode = AddLocalFileToMpq(&Logger, hMpq, szArchivedName, szFileName1, AddFlags[i], 0);
            if(dwErrCode != ERROR_SUCCESS)
                break;

            dwFileCount++;
        }
    }

    // Delete a file in the middle of the file table
    if(dwErrCode == ERROR_SUCCESS)
    {
        Logger.PrintProgress("Removing file %s ...", szMiddleFile);
        dwErrCode = RemoveMpqFile(&Logger, hMpq, szMiddleFile, ERROR_SUCCESS);
        dwFileCount--;
    }

    // Add one more file
    if(dwErrCode == ERROR_SUCCESS)
    {
        dwErrCode = AddLocalFileToMpq(&Logger, hMpq, "FileTest_xx.exe", szFileName1);
        dwFileCount++;
    }

    // Try to decrement max file count. This must succeed
    if(dwErrCode == ERROR_SUCCESS)
    {
        Logger.PrintProgress("Attempting to decrement max file count ...");
        if(SFileSetMaxFileCount(hMpq, 5))
            dwErrCode = Logger.PrintError("Max file count decremented, even if it should fail");
    }

    // Add ZeroSize.txt several times under a different locale
    if(dwErrCode == ERROR_SUCCESS)
    {
        for(size_t i = 0; LocaleIDs[i] != 0xFFFF; i++)
        {
            bool bMustSucceed = ((dwFileCount + 2) < dwMaxFileCount);

            SFileSetLocale(LocaleIDs[i]);
            dwErrCode = AddLocalFileToMpq(&Logger, hMpq, "ZeroSize_1.txt", szFileName2);
            if(dwErrCode != ERROR_SUCCESS)
            {
                if(bMustSucceed == false)
                    dwErrCode = ERROR_SUCCESS;
                break;
            }

            dwFileCount++;
        }
    }

    // Add ZeroSize.txt again several times under a different locale
    if(dwErrCode == ERROR_SUCCESS)
    {
        for(size_t i = 0; LocaleIDs[i] != 0xFFFF; i++)
        {
            bool bMustSucceed = ((dwFileCount + 2) < dwMaxFileCount);

            SFileSetLocale(LocaleIDs[i]);
            dwErrCode = AddLocalFileToMpq(&Logger, hMpq, "ZeroSize_2.txt", szFileName2, 0, 0, bMustSucceed);
            if(dwErrCode != ERROR_SUCCESS)
            {
                if(bMustSucceed == false)
                    dwErrCode = ERROR_SUCCESS;
                break;
            }

            dwFileCount++;
        }
    }

    // Verify how many files did we add to the MPQ
    if(dwErrCode == ERROR_SUCCESS)
    {
        if(dwFileCount + 2 != dwMaxFileCount)
        {
            Logger.PrintErrorVa("Number of files added to MPQ was unexpected (expected %u, added %u)", dwFileCount, dwMaxFileCount - 2);
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Test rename function
    if(dwErrCode == ERROR_SUCCESS)
    {
        Logger.PrintProgress("Testing rename files ...");
        SFileSetLocale(LANG_NEUTRAL);
        if(!SFileRenameFile(hMpq, "FileTest_08.exe", "FileTest_08a.exe"))
            dwErrCode = Logger.PrintError("Failed to rename the file");
    }

    if(dwErrCode == ERROR_SUCCESS)
    {
        if(!SFileRenameFile(hMpq, "FileTest_08a.exe", "FileTest_08.exe"))
            dwErrCode = Logger.PrintError("Failed to rename the file");
    }

    if(dwErrCode == ERROR_SUCCESS)
    {
        if(SFileRenameFile(hMpq, "FileTest_10.exe", "FileTest_10a.exe"))
        {
            Logger.PrintError("Rename test succeeded even if it shouldn't");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    if(dwErrCode == ERROR_SUCCESS)
    {
        if(SFileRenameFile(hMpq, "FileTest_10a.exe", "FileTest_10.exe"))
        {
            Logger.PrintError("Rename test succeeded even if it shouldn't");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    // Close the archive
    if(hMpq != NULL)
        SFileCloseArchive(hMpq);
    hMpq = NULL;

    // Try to reopen the archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = OpenExistingArchive(&Logger, szFullPath, 0, NULL);
    return dwErrCode;
}

static DWORD TestCreateArchive_WaveCompressionsTest(LPCTSTR szPlainName, LPCTSTR szWaveFile)
{
    TLogHelper Logger("CompressionsTest", szPlainName);
    HANDLE hMpq = NULL;                 // Handle of created archive
    TCHAR szFileName[MAX_PATH];          // Source file to be added
    char szArchivedName[MAX_PATH];
    DWORD dwCmprCount = sizeof(WaveCompressions) / sizeof(DWORD);
    DWORD dwAddedFiles = 0;
    DWORD dwFoundFiles = 0;
    DWORD dwErrCode;

    // Create paths for local file to be added
    CreateFullPathName(szFileName, _countof(szFileName), szMpqSubDir, szWaveFile);

    // Create new archive
    dwErrCode = CreateNewArchive(&Logger, szPlainName, MPQ_CREATE_ARCHIVE_V1 | MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES, 0x40, &hMpq);

    // Add the same file multiple times
    if(dwErrCode == ERROR_SUCCESS)
    {
        Logger.UserTotal = dwCmprCount;
        for(unsigned int i = 0; i < dwCmprCount; i++)
        {
            sprintf(szArchivedName, "WaveFile_%02u.wav", i + 1);
            dwErrCode = AddLocalFileToMpq(&Logger, hMpq, szArchivedName, szFileName, MPQ_FILE_COMPRESS | MPQ_FILE_ENCRYPTED | MPQ_FILE_SECTOR_CRC, WaveCompressions[i]);
            if(dwErrCode != ERROR_SUCCESS)
                break;

            Logger.UserCount++;
            dwAddedFiles++;
        }

        SFileCloseArchive(hMpq);
    }

    // Reopen the archive extract each WAVE file and try to play it
    if(dwErrCode == ERROR_SUCCESS)
    {
        dwErrCode = OpenExistingArchiveWithCopy(&Logger, NULL, szPlainName, &hMpq);
        if(dwErrCode == ERROR_SUCCESS)
        {
            SearchArchive(&Logger, hMpq, SEARCH_FLAG_LOAD_FILES | SEARCH_FLAG_PLAY_WAVES, &dwFoundFiles, NULL);
            SFileCloseArchive(hMpq);
        }

        // Check if the number of found files is the same like the number of added files
        // DOn;t forget that there will be (listfile) and (attributes)
        if(dwFoundFiles != (dwAddedFiles + 2))
        {
            Logger.PrintError("Number of found files does not match number of added files.");
            dwErrCode = ERROR_FILE_CORRUPT;
        }
    }

    return dwErrCode;
}

static DWORD TestCreateArchive_ListFilePos(LPCTSTR szPlainName)
{
    TFileData * pFileData;
    LPCSTR szReaddedFile = "AddedFile_##.txt";
    LPCSTR szFileMask = "AddedFile_%02u.txt";
    TLogHelper Logger("ListFilePos", szPlainName);
    HANDLE hMpq = NULL;                 // Handle of created archive
    char szArchivedName[MAX_PATH];
    DWORD dwMaxFileCount = 0x0E;
    DWORD dwFileCount = 0;
    size_t i;
    DWORD dwErrCode;

    // Create a new archive with the limit of 0x20 files
    dwErrCode = CreateNewArchive(&Logger, szPlainName, MPQ_CREATE_ARCHIVE_V4 | MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES, dwMaxFileCount, &hMpq);

    // Add maximum files files
    if(dwErrCode == ERROR_SUCCESS)
    {
        for(i = 0; i < dwMaxFileCount; i++)
        {
            sprintf(szArchivedName, szFileMask, i);
            dwErrCode = AddFileToMpq(&Logger, hMpq, szArchivedName, "This is a text data.", 0, 0, ERROR_SUCCESS);
            if(dwErrCode != ERROR_SUCCESS)
                break;

            dwFileCount++;
        }
    }

    // Delete few middle files
    if(dwErrCode == ERROR_SUCCESS)
    {
        for(i = 0; i < (dwMaxFileCount / 2); i++)
        {
            sprintf(szArchivedName, szFileMask, i);
            dwErrCode = RemoveMpqFile(&Logger, hMpq, szArchivedName, ERROR_SUCCESS);
            if(dwErrCode != ERROR_SUCCESS)
                break;

            dwFileCount--;
        }
    }

    // Close the archive
    if(hMpq != NULL)
        SFileCloseArchive(hMpq);
    hMpq = NULL;

    // Reopen the archive to catch any asserts
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = OpenExistingArchiveWithCopy(&Logger, NULL, szPlainName, &hMpq);

    // Check that (listfile) is at the end
    if(dwErrCode == ERROR_SUCCESS)
    {
        pFileData = LoadMpqFile(&Logger, hMpq, LISTFILE_NAME);
        if(pFileData != NULL)
        {
            if(pFileData->dwBlockIndex < dwFileCount)
                Logger.PrintMessage("Unexpected file index of %s", LISTFILE_NAME);
            STORM_FREE(pFileData);
        }

        pFileData = LoadMpqFile(&Logger, hMpq, ATTRIBUTES_NAME);
        if(pFileData != NULL)
        {
            if(pFileData->dwBlockIndex <= dwFileCount)
                Logger.PrintMessage("Unexpected file index of %s", ATTRIBUTES_NAME);
            STORM_FREE(pFileData);
        }

        // Add new file to the archive. It should be added to the last position
        dwErrCode = AddFileToMpq(&Logger, hMpq, szReaddedFile, "This is a re-added file.", 0, 0, ERROR_SUCCESS);
        if(dwErrCode == ERROR_SUCCESS)
        {
            // Force update of the tables
            SFileFlushArchive(hMpq);

            // Load the file
            pFileData = LoadMpqFile(&Logger, hMpq, szReaddedFile);
            if(pFileData != NULL)
            {
                if(pFileData->dwBlockIndex != dwFileCount)
                    Logger.PrintMessage("Unexpected file index of %s", szReaddedFile);
                STORM_FREE(pFileData);
            }
        }

        SFileCloseArchive(hMpq);
    }

    return dwErrCode;
}

static DWORD TestCreateArchive_BigArchive(LPCTSTR szPlainName)
{
    TLogHelper Logger("BigMpqTest", szPlainName);
    HANDLE hMpq = NULL;                 // Handle of created archive
    TCHAR szLocalFileName[MAX_PATH];
    char szArchivedName[MAX_PATH];
    DWORD dwMaxFileCount = 0x20;
    DWORD dwAddedCount = 0;
    size_t i;
    DWORD dwErrCode;

    // Create a new archive with the limit of 0x20 files
    dwErrCode = CreateNewArchive(&Logger, szPlainName, MPQ_CREATE_ARCHIVE_V3 | MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES, dwMaxFileCount, &hMpq);
    if(dwErrCode == ERROR_SUCCESS)
    {
        LPCSTR szFileMask = "AddedFile_%02u.txt";

        // Now add few really big files
        CreateFullPathName(szLocalFileName, _countof(szLocalFileName), szMpqSubDir, _T("MPQ_1997_v1_Diablo1_DIABDAT.MPQ"));
        Logger.UserTotal = (dwMaxFileCount / 2);

        for(i = 0; i < dwMaxFileCount / 2; i++)
        {
            sprintf(szArchivedName, szFileMask, i + 1);
            dwErrCode = AddLocalFileToMpq(&Logger, hMpq, szArchivedName, szLocalFileName, 0, 0, true);
            if(dwErrCode != ERROR_SUCCESS)
                break;

            Logger.UserCount++;
            dwAddedCount++;
        }
    }

    // Close the archive
    if(hMpq != NULL)
        SFileCloseArchive(hMpq);
    hMpq = NULL;

    // Reopen the archive to catch any asserts
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = OpenExistingArchiveWithCopy(&Logger, NULL, szPlainName, &hMpq);

    // Check that (listfile) is at the end
    if(dwErrCode == ERROR_SUCCESS)
    {
        CheckIfFileIsPresent(&Logger, hMpq, LISTFILE_NAME, true);
        CheckIfFileIsPresent(&Logger, hMpq, ATTRIBUTES_NAME, true);

        SFileCloseArchive(hMpq);
    }

    return dwErrCode;
}

// "MPQ_2014_v4_Heroes_Replay.MPQ", "AddFile-replay.message.events"
static DWORD TestModifyArchive_ReplaceFile(LPCTSTR szMpqPlainName, LPCTSTR szFileName)
{
    TLogHelper Logger("ModifyTest", szMpqPlainName);
    HANDLE hMpq = NULL;
    TCHAR szLocalFileName[MAX_PATH];
    char szArchivedName[MAX_PATH];
    size_t nOffset = 0;
    DWORD dwErrCode;

    // Open an existing archive
    dwErrCode = OpenExistingArchiveWithCopy(&Logger, szMpqPlainName, szMpqPlainName, &hMpq);

    // Add the given file
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Get the name of archived file
        if(!_tcsnicmp(szFileName, _T("AddFile-"), 8))
            nOffset = 8;
        StringCopy(szArchivedName, _countof(szArchivedName), szFileName + nOffset);

        // Create the local file name
        CreateFullPathName(szLocalFileName, _countof(szLocalFileName), szMpqSubDir, szFileName);

        // Add the file to MPQ
        dwErrCode = AddLocalFileToMpq(&Logger, hMpq,
                                               szArchivedName,
                                               szLocalFileName,
                                               MPQ_FILE_REPLACEEXISTING | MPQ_FILE_COMPRESS | MPQ_FILE_SINGLE_UNIT,
                                               MPQ_COMPRESSION_ZLIB,
                                               true);
    }

    // Reopen the MPQ and compact it
    if(dwErrCode == ERROR_SUCCESS)
    {
        // Compact the archive
        Logger.PrintProgress("Compacting archive %s ...", szMpqPlainName);
        if(!SFileSetCompactCallback(hMpq, CompactCallback, &Logger))
            dwErrCode = Logger.PrintError(_T("Failed to compact archive %s"), szMpqPlainName);

        if(!SFileCompactArchive(hMpq, NULL, 0))
            dwErrCode = GetLastError();

        SFileCloseArchive(hMpq);
    }

    // Try to open the archive again
    if(dwErrCode == ERROR_SUCCESS)
    {
        CreateFullPathName(szLocalFileName, _countof(szLocalFileName), NULL, szMpqPlainName);
        dwErrCode = OpenExistingArchive(&Logger, szLocalFileName, 0, &hMpq);
        if(dwErrCode == ERROR_SUCCESS)
            SFileCloseArchive(hMpq);
    }

    return dwErrCode;
}

//-----------------------------------------------------------------------------
// Tables

static const TEST_INFO TestList_StreamOps[] =
{
    {_T("MPQ_2013_v4_alternate-original.MPQ"),         NULL, 0},
    {_T("MPQ_2013_v4_alternate-original.MPQ"),         NULL, STREAM_FLAG_READ_ONLY},
    {_T("MPQ_2013_v4_alternate-complete.MPQ"),         NULL, STREAM_FLAG_USE_BITMAP},
    {_T("part-file://MPQ_2009_v2_WoW_patch.MPQ.part"), NULL, 0},
    {_T("blk4-file://streaming/model.MPQ.0"),          NULL, STREAM_PROVIDER_BLOCK4},
    {_T("mpqe-file://MPQ_2011_v2_EncryptedMpq.MPQE"),  NULL, STREAM_PROVIDER_MPQE}
};

static const TEST_INFO TestList_MasterMirror[] =
{
    {_T("part-file://MPQ_2009_v1_patch-created.MPQ.part"),  _T("MPQ_2009_v1_patch-original.MPQ"),       0},
    {_T("part-file://MPQ_2009_v1_patch-partial.MPQ.part"),  _T("MPQ_2009_v1_patch-original.MPQ"),       1},
    {_T("part-file://MPQ_2009_v1_patch-complete.MPQ.part"), _T("MPQ_2009_v1_patch-original.MPQ"),       1},
    {_T("MPQ_2013_v4_alternate-created.MPQ"),               _T("MPQ_2013_v4_alternate-original.MPQ"),   0},
    {_T("MPQ_2013_v4_alternate-incomplete.MPQ"),            _T("MPQ_2013_v4_alternate-incomplete.MPQ"), 1},
    {_T("MPQ_2013_v4_alternate-complete.MPQ"),              _T("MPQ_2013_v4_alternate-original.MPQ"),   1},

    // Takes hell a lot of time!!!
//  {_T("MPQ_2013_v4_alternate-downloaded.MPQ"),            _T("http://www.zezula.net\\mpqs\\alternate.zip"), 0}
};

static const TEST_INFO Test_Mpqs[] =
{

    // Correct or damaged archives
    {_T("MPQ_1997_v1_Diablo1_DIABDAT.MPQ"),    NULL, 0, "music\\dintro.wav", "File00000023.xxx"},
    {_T("MPQ_2016_v1_D2XP_IX86_1xx_114a.mpq"), NULL, 0, "waitingroombkgd.dc6"},                   // Update MPQ from Diablo II (patch 2016)
    {_T("MPQ_2018_v1_icon_error.w3m"),         NULL, 0, "file00000002.blp"},
    {_T("MPQ_1997_v1_Diablo1_STANDARD.SNP"),   _T("ListFile_Blizzard.txt")},                      // File whose archive's (signature) file has flags = 0x90000000
    {_T("MPQ_2012_v2_EmptyMpq.MPQ") },                                                            // Empty archive (found in WoW cache - it's just a header)
    {_T("MPQ_2013_v4_EmptyMpq.MPQ") },                                                            // Empty archive (created artificially - it's just a header)
    {_T("MPQ_2013_v4_patch-base-16357.MPQ") },                                                    // Empty archive (found in WoW cache - it's just a header)
    {_T("MPQ_2011_v4_InvalidHetEntryCount.MPQ") },                                                // Empty archive (A buggy MPQ with invalid HET entry count)
    {_T("MPQ_2002_v1_BlockTableCut.MPQ") },                                                       // Truncated archive
    {_T("MPQ_2010_v2_HasUserData.s2ma") },                                                        // MPQ that actually has user data
    {_T("MPQ_2014_v1_AttributesOneEntryLess.w3x") },                                              // Warcraft III map whose "(attributes)" file has (BlockTableSize-1) entries
    {_T("MPQ_2020_v1_AHF04patch.mix") },                                                          // MIX file
    {_T("MPQ_2010_v3_expansion-locale-frFR.MPQ") },                                               // MPQ archive v 3.0
    {_T("mpqe-file://MPQ_2011_v2_EncryptedMpq.MPQE") },                                           // Encrypted archive from Starcraft II installer
    {_T("MPx_2013_v1_LongwuOnline.mpk") },                                                        // MPK archive from Longwu online
    {_T("MPx_2013_v1_WarOfTheImmortals.sqp"), _T("ListFile_WarOfTheImmortals.txt") },             // SQP archive from War of the Immortals
    {_T("part-file://MPQ_2010_v2_HashTableCompressed.MPQ.part") },                                // Partial MPQ with compressed hash table
    {_T("blk4-file://streaming/model.MPQ.0")},                                                      // Archive that is merged with multiple files
    {_T("MPQ_2002_v1_ProtectedMap_InvalidUserData.w3x")},
    {_T("MPQ_2002_v1_ProtectedMap_InvalidMpqFormat.w3x")},
    {_T("MPQ_2002_v1_ProtectedMap_Spazzler.w3x")},                                                  // Warcraft III map locked by the Spazzler protector
    {_T("MPQ_2014_v1_ProtectedMap_Spazzler2.w3x")},                                                 // Warcraft III map locked by the Spazzler protector
    {_T("MPQ_2014_v1_ProtectedMap_Spazzler3.w3x")},                                                 // Warcraft III map locked by the Spazzler protector
    {_T("MPQ_2002_v1_ProtectedMap_BOBA.w3m")},                                                      // Warcraft III map locked by the BOBA protector
    {_T("MPQ_2015_v1_ProtectedMap_KangTooJee.w3x")},
    {_T("MPQ_2015_v1_ProtectedMap_Somj2hM16.w3x")},
    {_T("MPQ_2015_v1_ProtectedMap_Spazy.w3x")},                                                     // Warcraft III map locked by Spazy protector
    {_T("MPQ_2015_v1_MessListFile.mpq")},
    {_T("MPQ_2016_v1_ProtectedMap_TableSizeOverflow.w3x")},
    {_T("MPQ_2016_v1_ProtectedMap_HashOffsIsZero.w3x")},
    {_T("MPQ_2016_v1_ProtectedMap_Somj2.w3x")},                                                     // Something like Somj 2.0
    {_T("MPQ_2016_v1_WME4_4.w3x")},                                                                 // Protector from China (2016-05-27)
    {_T("MPQ_2016_v1_SP_(4)Adrenaline.w3x")},
    {_T("MPQ_2016_v1_ProtectedMap_1.4.w3x")},
    {_T("MPQ_2016_v1_KoreanFile.w3m")},
    {_T("MPQ_2017_v1_Eden_RPG_S2_2.5J.w3x")},                                                       // Load map protected by PG1.11.973
    {_T("MPQ_2017_v1_BigDummyFiles.w3x")},
    {_T("MPQ_2017_v1_TildeInFileName.mpq")},
    {_T("MPQ_2018_v1_EWIX_v8_7.w3x"), NULL, 0, "BlueCrystal.mdx"},
    {_T("MPQ_2020_v4_FakeMpqHeaders.SC2Mod")},                                                      // Archive that has two fake headers before the real one
    {_T("MPQ_2020_v4_NP_Protect_1.s2ma")},                                                          // SC2 map that is protected by the NP_Protect
    {_T("MPQ_2020_v4_NP_Protect_2.s2ma")},                                                          // SC2 map that is protected by the NP_Protect

    // Protected archives
    {_T("MPQ_2015_v1_flem1.w3x"), NULL, TEST_FLAG_PROTECTED | 20, "1c4c13e627658c473e84d94371e31f37"},
    {_T("MPQ_2002_v1_ProtectedMap_HashTable_FakeValid.w3x"), NULL, TEST_FLAG_PROTECTED | 114, "5250975ed917375fc6540d7be436d4de"},
};

//-----------------------------------------------------------------------------
// Main

int _tmain(int argc, TCHAR * argv[])
{
    DWORD dwErrCode = ERROR_SUCCESS;

#if defined(_MSC_VER) && defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif  // defined(_MSC_VER) && defined(_DEBUG)

    // Initialize storage and mix the random number generator
    printf("==== Test Suite for StormLib version %s ====\n", STORMLIB_VERSION_STRING);
    dwErrCode = InitializeMpqDirectory(argv, argc);

    //
    // Tests on a local listfile
    //
    /*
    if(dwErrCode == ERROR_SUCCESS)
    {
        TestOnLocalListFile(_T("FLAT-MAP:ListFile_Blizzard.txt"));
        dwErrCode = TestOnLocalListFile(_T("ListFile_Blizzard.txt"));
    }
    */
    //
    // Open all files from the command line
    //

	TestArchive(_T("ProblemMpqArchive.SC2Replay"), NULL, 0, "replay.game.events", NULL);
    for(int i = 1; i < argc; i++)
    {
        TestOpenArchive_CompactArchive(argv[i], _T("TestArchiveCopy.mpq"), false);
    }

    //
    // Search all testing archives and verify their SHA1 hash
    //

    if(dwErrCode == ERROR_SUCCESS)
    {
        dwErrCode = FindFiles(ForEachFile_VerifyFileChecksum, szMpqSubDir);
    }

    //
    // Test file stream operations
    //

    if(dwErrCode == ERROR_SUCCESS)
    {
        for(size_t i = 0; i < _countof(TestList_StreamOps); i++)
        {
            dwErrCode = TestFileStreamOperations(TestList_StreamOps[i].szMpqName1, TestList_StreamOps[i].dwFlags);
            if(dwErrCode != ERROR_SUCCESS)
                break;
        }
    }

    //
    // Test master-mirror reading operations
    //

    if(dwErrCode == ERROR_SUCCESS)
    {
        for(size_t i = 0; i < _countof(TestList_MasterMirror); i++)
        {
            dwErrCode = TestReadFile_MasterMirror(TestList_MasterMirror[i].szMpqName1,
                                                  TestList_MasterMirror[i].szMpqName2,
                                                  TestList_MasterMirror[i].dwFlags != 0);
            if(dwErrCode != ERROR_SUCCESS)
                break;
        }
    }

    //
    // Test opening various archives - correct, damaged, protected
    //

    if(dwErrCode == ERROR_SUCCESS)
    {
        for(size_t i = 0; i < _countof(Test_Mpqs); i++)
        {
            dwErrCode = TestArchive(Test_Mpqs[i].szMpqName1,         // Plain archive name
                                    Test_Mpqs[i].szMpqName2,         // List file (NULL if none)
                                    Test_Mpqs[i].dwFlags,            // What exactly to do
                                    Test_Mpqs[i].szFileName1,        // The first name of the open file
                                    Test_Mpqs[i].szFileName2);       // The second name of the open file
            if(dwErrCode != ERROR_SUCCESS)
                break;
        }
    }

    // Open the multi-file archive with wrong prefix to see how StormLib deals with it
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_WillFail(_T("flat-file://streaming/model.MPQ.0"));

    // Open every MPQ that we have in the storage
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = FindFiles(ForEachFile_OpenArchive, NULL);

    // Test on an archive that has been invalidated by extending an old valid MPQ
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_Corrupt(_T("MPQ_2013_vX_Battle.net.MPQ"));

    // Open a patched archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_Patched(PatchList_StarCraft, "music\\terran1.wav", 0);

    // Open a patched archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_Patched(PatchList_WoW_OldWorld13286, "OldWorld\\World\\Model.blob", 2);

    // Open a patched archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_Patched(PatchList_WoW_15050, "World\\Model.blob", 8);

    // Open a patched archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_Patched(PatchList_WoW_16965, "DBFilesClient\\BattlePetNPCTeamMember.db2", 0);

    // Open a patched archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_Patched(PatchList_SC2_32283, "TriggerLibs\\natives.galaxy", 6);

    // Open a patched archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_Patched(PatchList_SC2_34644, "TriggerLibs\\GameData\\GameData.galaxy", 2);

    // Open a patched archive with new format of BSDIFF patch
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_Patched(PatchList_SC2_34644_Maps, "Maps\\Campaign\\THorner03.SC2Map\\BankList.xml", 3);

    // Open a patched archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_Patched(PatchList_SC2_32283_enGB, "Assets\\Textures\\startupimage.dds", 0, true);

    // Open a patched archive where the "StreamingBuckets.txt" in the patch doesn't contain MPQ_FILE_PATCH_FILE
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_Patched(PatchList_SC2_36281_enGB, "LocalizedData\\GameHotkeys.txt", 6);

    // Open a patched archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_Patched(PatchList_HS_3604_enGB, "Hearthstone.exe", 1);

    // Open a patched archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_Patched(PatchList_HS_6898_enGB, "Hearthstone_Data\\Managed\\Assembly-Csharp.dll", 10);

    // Check the opening archive for read-only
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_ReadOnly(_T("MPQ_1997_v1_Diablo1_DIABDAT.MPQ"), true);

    // Check the opening archive for read-only
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_ReadOnly(_T("MPQ_1997_v1_Diablo1_DIABDAT.MPQ"), false);

    // Check the SFileGetFileInfo function
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_GetFileInfo(_T("MPQ_2002_v1_StrongSignature.w3m"), _T("MPQ_2013_v4_SC2_EmptyMap.SC2Map"));

    // Downloadable MPQ archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_MasterMirror(_T("part-file://MPQ_2009_v1_patch-partial.MPQ.part"), _T("MPQ_2009_v1_patch-original.MPQ"), "world\\Azeroth\\DEADMINES\\PASSIVEDOODADS\\GOBLINMELTINGPOT\\DUST2.BLP", false);

    // Downloadable MPQ archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_MasterMirror(_T("MPQ_2013_v4_alternate-downloaded.MPQ"), _T("MPQ_2013_v4_alternate-original.MPQ"), "alternate\\DUNGEONS\\TEXTURES\\ICECROWN\\GATE\\jlo_IceC_Floor_Thrown.blp", false);

    // Check archive signature
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_VerifySignature(_T("MPQ_1997_v1_Diablo1_STANDARD.SNP"), _T("STANDARD.SNP"));

    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_VerifySignature(_T("MPQ_1999_v1_WeakSignature.exe"), _T("War2Patch_202.exe"));

    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_VerifySignature(_T("MPQ_2003_v1_WeakSignatureEmpty.exe"), _T("WoW-1.2.3.4211-enUS-patch.exe"));

    // Check archive signature
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_VerifySignature(_T("MPQ_2002_v1_StrongSignature.w3m"), _T("(10)DustwallowKeys.w3m"));

    // Compact the archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_CompactArchive(_T("MPQ_2010_v3_expansion-locale-frFR.MPQ"), _T("StormLibTest_CraftedMpq1_v3.mpq"), true);

    // Compact the archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_CompactArchive(_T("MPQ_2016_v1_00000.pak"), _T("MPQ_2016_v1_00000.pak"), false);

    // Open a MPQ (add custom user data to it)
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_CompactArchive(_T("MPQ_2013_v4_SC2_EmptyMap.SC2Map"), _T("StormLibTest_CraftedMpq2_v4.mpq"), true);

    // Open a MPQ (add custom user data to it)
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_CompactArchive(_T("MPQ_2013_v4_expansion1.MPQ"), _T("StormLibTest_CraftedMpq3_v4.mpq"), true);

    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestAddFile_FullTable(_T("MPQ_2014_v1_out1.w3x"));

    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestAddFile_FullTable(_T("MPQ_2014_v1_out2.w3x"));

    // Test modifying file with no (listfile) and no (attributes)
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestAddFile_ListFileTest(_T("MPQ_1997_v1_Diablo1_DIABDAT.MPQ"), false, false);

    // Test modifying an archive that contains (listfile) and (attributes)
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestAddFile_ListFileTest(_T("MPQ_2013_v4_SC2_EmptyMap.SC2Map"), true, true);

    // Create an empty archive v2
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_EmptyMpq(_T("StormLibTest_EmptyMpq_v2.mpq"), MPQ_CREATE_ARCHIVE_V2 | MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES);

    // Create an empty archive v4
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_EmptyMpq(_T("StormLibTest_EmptyMpq_v4.mpq"), MPQ_CREATE_ARCHIVE_V4 | MPQ_CREATE_LISTFILE | MPQ_CREATE_ATTRIBUTES);

    // Test creating of an archive the same way like MPQ Editor does
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_TestGaps(_T("StormLibTest_GapsTest.mpq"));

    // Test creating of an archive with non standard file names
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_NonStdNames(_T("StormLibTest_NonStdNames.mpq"));

    // Sign an existing non-signed archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_SignExisting(_T("MPQ_1998_v1_StarDat.mpq"));

    // Open a signed archive, add a file and verify the signature
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestOpenArchive_ModifySigned(_T("MPQ_1999_v1_WeakSignature.exe"), _T("War2Patch_202.exe"));

    // Create new archive and sign it
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_Signed(_T("MPQ_1999_v1_WeakSigned1.mpq"), true);

    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_Signed(_T("MPQ_1999_v1_WeakSigned2.mpq"), false);

    // Test creating of an archive the same way like MPQ Editor does
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_MpqEditor(_T("StormLibTest_MpqEditorTest.mpq"), "AddedFile.exe");

    // Create an archive and fill it with files up to the max file count
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_FillArchive(_T("StormLibTest_FileTableFull.mpq"), 0);

    // Create an archive and fill it with files up to the max file count
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_FillArchive(_T("StormLibTest_FileTableFull.mpq"), MPQ_CREATE_LISTFILE);

    // Create an archive and fill it with files up to the max file count
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_FillArchive(_T("StormLibTest_FileTableFull.mpq"), MPQ_CREATE_ATTRIBUTES);

    // Create an archive and fill it with files up to the max file count
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_FillArchive(_T("StormLibTest_FileTableFull.mpq"), MPQ_CREATE_ATTRIBUTES | MPQ_CREATE_LISTFILE);

    // Create an archive, and increment max file count several times
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_IncMaxFileCount(_T("StormLibTest_IncMaxFileCount.mpq"));

    // Create a MPQ archive with UNICODE names
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_UnicodeNames();

    // Create a MPQ file, add files with various flags
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_FileFlagTest(_T("StormLibTest_FileFlagTest.mpq"));

    // Create a MPQ file, add a mono-WAVE file with various compressions
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_WaveCompressionsTest(_T("StormLibTest_AddWaveMonoTest.mpq"), _T("AddFile-Mono.wav"));

    // Create a MPQ file, add a mono-WAVE with 8 bits per sample file with various compressions
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_WaveCompressionsTest(_T("StormLibTest_AddWaveMonoBadTest.mpq"), _T("AddFile-MonoBad.wav"));

    // Create a MPQ file, add a stereo-WAVE file with various compressions
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_WaveCompressionsTest(_T("StormLibTest_AddWaveStereoTest.mpq"), _T("AddFile-Stereo.wav"));

    // Check if the listfile is always created at the end of the file table in the archive
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_ListFilePos(_T("StormLibTest_ListFilePos.mpq"));

    // Open a MPQ (add custom user data to it)
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestCreateArchive_BigArchive(_T("StormLibTest_BigArchive_v4.mpq"));

    // Test replacing a file with zero size file
    if(dwErrCode == ERROR_SUCCESS)
        dwErrCode = TestModifyArchive_ReplaceFile(_T("MPQ_2014_v4_Base.StormReplay"), _T("AddFile-replay.message.events"));

#ifdef _MSC_VER
    _CrtDumpMemoryLeaks();
#endif  // _MSC_VER

    return dwErrCode;
}
