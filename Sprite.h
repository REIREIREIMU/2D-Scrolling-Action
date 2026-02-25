#pragma once
#include <DxLib.h>
#include <string>
#include <unordered_map>
#include <vector>

class SpriteFont {
public:
    bool Load(const char* path, int glyphW, int glyphH, int cols,
        const std::vector<std::string>& rows);
    void Unload();

    bool IsLoaded() const { return handle_ >= 0; }

    void DrawText(int x, int y, const std::string& text,
        int letterSpacing = 0, int lineSpacing = 0, bool transparent = true) const;

    void DrawFormatText(int x, int y, int letterSpacing, int lineSpacing, bool transparent,
        const char* fmt, ...) const;

    void DrawTextScaled(int x, int y, float scale, const std::string& text,
        int letterSpacing = 0, int lineSpacing = 0, bool transparent = true) const;

    int MeasureTextWidth(const std::string& text, int letterSpacing = 0) const;
    int MeasureTextHeight(const std::string& text, int lineSpacing = 0) const;

    int GlyphW() const { return glyphW_; }
    int GlyphH() const { return glyphH_; }

    static SpriteFont& Instance();

private:
    struct Glyph { int sx, sy, sw, sh; };

    int handle_ = -1;
    int glyphW_ = 0;
    int glyphH_ = 0;
    int cols_ = 0;
    std::unordered_map<char, Glyph> table_;

    void registerRow(int rowIndex, const std::string& chars);
    void drawOneGlyph(int x, int y, const Glyph& g, bool transparent) const;
    void drawOneGlyphScaled(int x, int y, const Glyph& g, float scale, bool transparent) const;
};