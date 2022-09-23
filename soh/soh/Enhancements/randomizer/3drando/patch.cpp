#include "patch.hpp"

#include "cosmetics.hpp"
#include "custom_messages.hpp"
#include "music.hpp"
#include "sound_effects.hpp"
#include "shops.hpp"
#include "spoiler_log.hpp"
#include "entrance.hpp"
#include "hints.hpp"

#include <array>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

// For specification on the IPS file format, visit: https://zerosoft.zophar.net/ips.php

using FILEPtr = std::unique_ptr<FILE, decltype(&std::fclose)>;

void WriteFloatToBuffer(std::vector<char>& buffer, float f, size_t offset) {
    memcpy(buffer.data() + offset, &f, sizeof(float));
}