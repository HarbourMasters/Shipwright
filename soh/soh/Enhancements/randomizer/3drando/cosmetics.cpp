#include "cosmetics.hpp"
#include "random.hpp"
#include <sstream>

namespace Cosmetics {

  bool ValidHexString(std::string_view hexStr) {
    return hexStr.find_first_not_of("0123456789ABCDEFabcdef") == std::string_view::npos && hexStr.length() == 6;
  }

  Color_RGBAf HexStrToColorRGBAf(const std::string& hexStr) {
    uint32_t hex = std::stoi(hexStr, nullptr, 16);

    return Color_RGBAf{
      .r = ((hex & 0xFF0000) >> 16) / 255.0f,
      .g = ((hex & 0x00FF00) >> 8)  / 255.0f,
      .b = (hex & 0x0000FF) / 255.0f,
      .a = 1.0f,
    };
  }

  Color_RGBA8 HexStrToColorRGBA8(const std::string& hexStr) {
      uint32_t hex = std::stoi(hexStr, nullptr, 16);

    return Color_RGBA8{
      .r = (uint8_t)((hex & 0xFF0000) >> 16),
      .g = (uint8_t)((hex & 0x00FF00) >> 8),
      .b = (uint8_t)(hex & 0x0000FF),
      .a = 0xFF,
    };
  }

  std::string CustomColorOptionText(std::string_view color) {
    return std::string(CUSTOM_COLOR_STR.substr(0, CUSTOM_COLOR_STR.length() - 6)).append(color);
  }

  std::string GetCustomColor(const std::string& str) {
    return str.substr(str.length() - 6);
  }

  const std::array<std::string_view, 13> gauntletColors = {
    "FFFFFF", //Silver
    "FECF0F", //Gold
    "000006", //Black
    "025918", //Green
    "06025A", //Blue
    "600602", //Bronze
    "FF0000", //Red
    "025DB0", //Sky Blue
    "FA6A90", //Pink
    "FF00FF", //Magenta
    "D83A00", //Orange
    "5B8A06", //Lime
    "800080", //Purple
  };
  const std::array<std::string_view, 28> tunicColors = {
    "168A0E", //Kokiri Green
    "96000E", //Goron Red
    "002A8E", //Zora Blue
    "202020", //Black
    "FFFFFF", //White
    "FF4A0A", //Orange
    "86FF23", //Yellow
    "0094B0", //Cyan
    "362076", //Indigo
    "7B0080", //Purple
    "F04F7D", //Pink
    "323232", //Dark Gray
    "E44B4B", //Salmon
    "5A1A20", //Wine Red
    "7E705B", //Beige
    "583923", //Brown
    "72602B", //Sand
    "6b7E5D", //Tea Green
    "3C5800", //Dark Green
    "6CFFF8", //Powder Blue
    "005A4B", //Teal
    "0259FF", //Sky Blue
    "33405E", //Faded Blue
    "635AD2", //Lavender
    "9C1B4B", //Magenta
    "52314F", //Mauve
    "505A59", //Silver
    "F16F16", //Gold
  };
  const std::array<std::string_view, 20> naviInnerColors = {
    "FFFFFF", //White
    "00FF00", //Green
    "9696FF", //Light Blue
    "FFFF00", //Yellow
    "FF0000", //Red
    "FF00FF", //Magenta
    "FECC3C", //Gold
    "000000", //Black
    "FFFFFF", //Tatl
    "49146C", //Tael
    "2C9EC4", //Fi
    "E6DE83", //Ciela
    "D14902", //Epona
    "629C5F", //Ezlo
    "A83317", //KoRL
    "032660", //Linebeck
    "D62E31", //Loftwing
    "192426", //Midna
    "977A6C", //Phantom Zelda
    "FF0000", //Rainbow (starts at red)
  };
  const std::array<std::string_view, 20> naviOuterColors = {
    "0000FF", //White
    "00FF00", //Green
    "9696FF", //Light Blue
    "C89B00", //Yellow
    "FF0000", //Red
    "C8009B", //Magenta
    "FEC007", //Gold
    "000000", //Black
    "C89800", //Tatl
    "FF0000", //Tael
    "2C1983", //Fi
    "C6BE5B", //Ciela
    "551F08", //Epona
    "3F5D37", //Ezlo
    "DED7C5", //KoRL
    "EFFFFF", //Linebeck
    "FDE6CC", //Loftwing
    "D28330", //Midna
    "6F4667", //Phantom Zelda
    "FF0000", //Rainbow (starts at red)
  };
  const std::array<std::string_view, 13> weaponTrailColors = {
    "FFFFFF", //White
    "000000", //Black
    "FF0000", //Red
    "00FF00", //Green
    "0000FF", //Blue
    "FFFF00", //Yellow
    "00FFFF", //Cyan
    "FF00FF", //Magenta
    "FFA500", //Orange
    "FFD700", //Gold
    "800080", //Purple
    "FF69B4", //Pink
    "FF0000", //Rainbow (starts at red)
  };

  //Generate random hex color
  std::string RandomColor() {
    std::ostringstream color;
    color << std::hex << (rand() % 0x1000000); //use default rand to not interfere with main settings
    return color.str();
  }
} //namespace Cosmetics
