#pragma once

namespace GlobalConfig {
    // 画面サイズ（全体共通）
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;

    //カラー(32bit)
    const int BIT_COLOR = 32;

    // グローバルなタイルサイズ（マップ系）
    const int TILE_SIZE = 40;

    // メインループで許容する最大デルタタイム（秒）
    const float MAX_DELTA_TIME = 0.05f;

    const float Break_Number = 2.0f;    // 割り算で使うやつ
    const int LEFT  = -1;               // 左
    const int RIGHT = +1;               // 右

    // フルスクリーン切替キー（F1）はコード内で使用
}

// 配色の管理
namespace ColorConfig {
    const int Default_Color = GetColor(200, 200, 200);  // デフォルト色
    const int Brown         = GetColor(139,  69,  19);  // 茶色
    const int Light_Brown   = GetColor(160,  82,  45);  // 明るい茶色
    const int Red           = GetColor(255, 100, 100);  // 赤っぽいレンガ
    const int Yellow        = GetColor(255, 255,   0);  // 黄色
    const int Cyan          = GetColor(  0, 255, 255);  // 水色
    const int Light_Cyan    = GetColor( 50, 150, 170);  // 明るい水色
    const int Green         = GetColor(  0, 255,   0);  // 緑
    const int Black         = GetColor(  0,   0,   0);	// 黒
    const int White         = GetColor(255, 255, 255);  // 白
}