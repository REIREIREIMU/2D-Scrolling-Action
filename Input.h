#pragma once
#include <DxLib.h>

// 入力設定
class Input {
public:
    static void Update();

    // 左右移動
    static int GetAxisX(); // -1: 左, 1: 右, 0: 無し

	static int GetAxisY(); // -1: 上, 1: 下, 0: 無し 梯子の時のみ使用

    // ボタン入力
    static bool IsJump();   // ジャンプ（ B / SPACE）
    static bool IsDecide(); // 決定    （ A / ENTER）

    // デバッグ表示
    //static void DebugPrint();

protected:
    static int padState; // ジョイパッド状態
};