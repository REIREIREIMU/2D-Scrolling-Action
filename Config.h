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
