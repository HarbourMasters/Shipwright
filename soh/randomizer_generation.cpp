#include <Lib/spdlog/include/spdlog/spdlog.h>

void GenerateRandomizer() {
    int ret = Playthrough::Playthrough_Init(std::hash<std::string>{}(Settings::seed));

    if (ret < 0) {
        if (ret == -1) { // Failed to generate after 5 tries
            SPDLOG_ERROR(
                "\n\nFailed to generate after 5 tries.\nPress B to go back to the menu.\nA different seed might be "
                "successful.");
            return;
        } else {
            SPDLOG_ERROR("\n\nError %d with fill.\nPress Select to exit or B to go back to the menu.\n", ret);
            return;
        }
    }

    const auto& randomizerHash = GetRandomizerHash();
}