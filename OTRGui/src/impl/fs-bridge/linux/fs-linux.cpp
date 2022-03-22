#include "fs-linux.h"

#ifndef _WIN32
#include <stdio.h>
#include "utils/mutils.h"
#include "portable-file-dialogs.h"

#define DEFAULT_PATH "~"

void LinuxBridge::InitBridge() {}

ProcessResult LinuxBridge::LaunchProcess(std::string cmd) {
    cmd = MoonUtils::normalize(cmd);
    std::cout << "Trying to launch: " << cmd << std::endl;
    ProcessResult result = { };
    result.exitCode = WEXITSTATUS(system(cmd.c_str()));
    return result;
}

std::string LinuxBridge::LaunchFileExplorer(LaunchType type) {
    if(type == LaunchType::FILE){
        auto file = pfd::open_file("Open OoT Baserom", DEFAULT_PATH,
            { "Rom Files (.z64 .n64)", "*.z64 *.n64", "All Files", "*" },
            pfd::opt::force_path
        );
        std::vector<std::string> files = file.result();
        if(!files.empty()) return files[0];
    }

    if(type == LaunchType::FOLDER){
        auto dir = pfd::select_folder("Select any directory", DEFAULT_PATH).result();
        if(!dir.empty()) return dir;
    }

    return "None";
}
#endif