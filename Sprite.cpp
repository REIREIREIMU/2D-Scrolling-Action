#include "Sprite.h"
#include <cstdarg>
#include <cstdio>

SpriteFont& SpriteFont::Instance() {
    static SpriteFont inst;
    return inst;
}

bool SpriteFont::Load(const char* path, int glyphW, int glyphH, int cols,
    const std::vector<std::string>& rows)
{
    Unload();
    handle_ = LoadGraph(path);
    if (handle_ < 0) return false;

    glyphW_ = glyphW;
    glyphH_ = glyphH;
    cols_ = cols;

    table_.clear();
    for (int r = 0; r < (int)rows.size(); ++r) {
        registerRow(r, rows[r]);
    }
    return true;
}

void SpriteFont::Unload() {
    if (handle_ >= 0) {
        DeleteGraph(handle_);
        handle_ = -1;
    }
    table_.clear();
}

void SpriteFont::registerRow(int rowIndex, const std::string& chars) {
    for (int i = 0; i < (int)chars.size(); ++i) {
        char c = chars[i];
        int col = i % cols_;
        int sx = col * glyphW_;
        int sy = rowIndex * glyphH_;
        table_[c] = Glyph{ sx, sy, glyphW_, glyphH_ };
    }
    // ƒXƒy[ƒX‚ª rows “à‚É–³‚¢ê‡‚É”õ‚¦‚Ä“o˜^i•‚¾‚¯‹ó‚¯‚éj
    if (table_.find(' ') == table_.end()) {
        int col = 0;
        int sx = col * glyphW_;
        int sy = rowIndex * glyphH_;
        table_[' '] = Glyph{ sx, sy, glyphW_, glyphH_ };
    }
}

void SpriteFont::drawOneGlyph(int x, int y, const Glyph& g, bool transparent) const {
    DrawRectGraph(x, y, g.sx, g.sy, g.sw, g.sh, handle_, transparent ? TRUE : FALSE);
}

void SpriteFont::drawOneGlyphScaled(int x, int y, const Glyph& g, float scale, bool transparent) const {
    int w = (int)(g.sw * scale);
    int h = (int)(g.sh * scale);
    DrawRectExtendGraph(x, y, x + w, y + h, g.sx, g.sy, g.sw, g.sh, handle_, transparent ? TRUE : FALSE);
}

void SpriteFont::DrawText(int x, int y, const std::string& text,
    int letterSpacing, int lineSpacing, bool transparent) const
{
    if (handle_ < 0) return;
    int cx = x, cy = y;
    for (char c : text) {
        if (c == '\n') {
            cx = x;
            cy += glyphH_ + lineSpacing;
            continue;
        }
        auto it = table_.find(c);
        if (it == table_.end()) {
            cx += glyphW_ + letterSpacing; // –¢“o˜^‚Í‹ó”’ˆµ‚¢
            continue;
        }
        drawOneGlyph(cx, cy, it->second, transparent);
        cx += glyphW_ + letterSpacing;
    }
}

void SpriteFont::DrawFormatText(int x, int y, int letterSpacing, int lineSpacing, bool transparent,
    const char* fmt, ...) const
{
    char buf[1024];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    DrawText(x, y, buf, letterSpacing, lineSpacing, transparent);
}

int SpriteFont::MeasureTextWidth(const std::string& text, int letterSpacing) const {
    int maxW = 0, curW = 0;
    for (char c : text) {
        if (c == '\n') {
            if (curW > maxW) maxW = curW;
            curW = 0;
        }
        else {
            curW += glyphW_ + letterSpacing;
        }
    }
    if (curW > maxW) maxW = curW;
    return maxW > 0 ? (maxW - letterSpacing) : 0;
}

int SpriteFont::MeasureTextHeight(const std::string& text, int lineSpacing) const {
    int lines = 1;
    for (char c : text) if (c == '\n') ++lines;
    return lines * glyphH_ + (lines - 1) * lineSpacing;
}

void SpriteFont::DrawTextScaled(int x, int y, float scale,
    const std::string& text,
    int letterSpacing, int lineSpacing, bool transparent) const
{
    if (handle_ < 0) return;
    int advW = (int)(glyphW_ * scale) + letterSpacing;
    int advH = (int)(glyphH_ * scale) + lineSpacing;

    int cx = x, cy = y;
    for (char c : text) {
        if (c == '\n') {
            cx = x;
            cy += advH;
            continue;
        }
        auto it = table_.find(c);
        if (it == table_.end()) {
            cx += advW;
            continue;
        }
        drawOneGlyphScaled(cx, cy, it->second, scale, transparent);
        cx += advW;
    }
}