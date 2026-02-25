// SpriteText.h
#pragma once
#include "Sprite.h"   // ← SpriteFont の宣言が入っているヘッダ名（= Sprite.h）

#include <cstdarg>
#include <cstdio>
#include <string>


inline void BmpDrawStringScaled(int x, int y, float scale, const char* str,
    int letterSpacing = 0, int lineSpacing = 0, bool transparent = true)
{
    SpriteFont::Instance().DrawTextScaled(
        x, y, scale,
        str ? std::string(str) : std::string(),
        letterSpacing, lineSpacing, transparent
    );
}


inline void BmpDrawFormatString(int x, int y, int /*color*/, const char* fmt, ...)
{
    char buf[1024];
    va_list ap; va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    SpriteFont::Instance().DrawText(x, y, buf);
}