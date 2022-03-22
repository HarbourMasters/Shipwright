#include "fs-windows.h"

#ifdef _WIN32
#include <direct.h>
#include <Windows.h>
#include <ShObjIdl.h>
#include <stdio.h>
#include "utils/mutils.h"

void WindowsBridge::InitBridge() {}

ProcessResult WindowsBridge::LaunchProcess(std::string cmd) {
    cmd = MoonUtils::normalize(cmd);
    std::cout << "Trying to launch: " << cmd << std::endl;
    ProcessResult result = { };
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD exit_code;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char cwd[256];
    getcwd(cwd, 256);

    // Start the child process.
    if (!CreateProcess(NULL,   // No module name (use command line)
        LPSTR(cmd.c_str()),   // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi
    )) {
        result.exitCode = GetLastError();
        return result;
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);
    GetExitCodeProcess(pi.hProcess, &exit_code);

    result.exitCode = static_cast<int>(exit_code);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return result;
}


std::string WindowsBridge::LaunchFileExplorer(LaunchType type) {
    std::string path = "None";
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (!SUCCEEDED(hr)) return path;
    IFileOpenDialog* pFileOpen;

    // Create the FileOpenDialog object.
    hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
        IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
    if (type == LaunchType::FOLDER) {
        DWORD dwOptions;
        pFileOpen->GetOptions(&dwOptions);
        pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);
    }
    if (!SUCCEEDED(hr)) {
        CoUninitialize();
        return path;
    }

    hr = pFileOpen->Show(nullptr);

    if (!SUCCEEDED(hr)) {
        pFileOpen->Release();
        return path;
    }

    IShellItem* pItem;
    hr = pFileOpen->GetResult(&pItem);
    if (!SUCCEEDED(hr)) return path;

    PWSTR pszFilePath;
    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

    // Display the file name to the user.
    if (SUCCEEDED(hr)) {
        path = MoonUtils::narrow(std::wstring(pszFilePath));
        CoTaskMemFree(pszFilePath);
    }
    pItem->Release();
    return path;
}
#endif