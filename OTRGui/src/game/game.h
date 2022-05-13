#ifndef MarsGameH
#define MarsGameH

#include "raylib.h"
#include <map>
#include <string>

class OTRGame {
public:
    std::map<std::string, Font>       Fonts;
    std::map<std::string, Model>      Models;
    std::map<std::string, Texture2D>  Textures;
    float ModelRotation = 0.0f;
    Camera camera;

    void preload();
    void init();
    void imgui();
    void update();
    void draw();
    void exit();

    inline bool CloseRequested() { return closeRequested; }
protected:
    void LoadTexture(const std::string& name, const std::string& path) {
        const Image tmp = LoadImage(path.c_str());
        Textures[name] = LoadTextureFromImage(tmp);
        UnloadImage(tmp);
    }

    void LoadFont(const std::string& name, const std::string& path) {
        Font font = LoadFontEx(path.c_str(), 16, nullptr, 0);
        GenTextureMipmaps(&font.texture);
        SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
        Fonts[name] = font;
    }

private:
    bool closeRequested = false;
};

extern OTRGame* Game;

#endif