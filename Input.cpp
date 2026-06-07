#include "Input.h"
#include "Config.h"

const int Analog_DeadZone = 200; // アナログスティックのデッドゾーン
int Input::padState = 0;         // コントローラー入力状態

// =========================================
// 入力状態の更新（毎フレーム呼び出し）
// =========================================
void Input::Update() {
    padState = GetJoypadInputState(DX_INPUT_PAD1);
}

// =========================================
// 左右入力（左スティック＋キーボード対応）
// =========================================
int Input::GetAxisX() {
    int x = 0, y = 0;

    // スティック取得
    GetJoypadAnalogInput(&x, &y, DX_INPUT_PAD1);

    // アナログスティックによる左右判定（デッドゾーン適用）
    if (x < -Analog_DeadZone) return GlobalConfig::LEFT;
    if (x >  Analog_DeadZone) return GlobalConfig::RIGHT;

    // キーボード入力
    if (CheckHitKey(KEY_INPUT_LEFT) ||CheckHitKey(KEY_INPUT_A)) return GlobalConfig::LEFT;
    if (CheckHitKey(KEY_INPUT_RIGHT)||CheckHitKey(KEY_INPUT_D)) return GlobalConfig::RIGHT;

    return 0;
}

// =========================================
// 上下入力（主に梯子用）
// =========================================
int Input::GetAxisY()
{
    if (CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_W)) return GlobalConfig::LEFT;
    if (CheckHitKey(KEY_INPUT_DOWN) || CheckHitKey(KEY_INPUT_S)) return GlobalConfig::RIGHT;

    return 0;
}

// =========================================
// ジャンプ入力（Xbox=A, Keyboard=SPACE/W）
// =========================================
bool Input::IsJump() {
    // コントローラ
    if (padState & PAD_INPUT_1) return true;
 
    // キーボード
    if (CheckHitKey(KEY_INPUT_SPACE) || CheckHitKey(KEY_INPUT_W)) return true;

    return false;
}

// =========================================
// 決定（Xbox=B, Keyboard=ENTER）
// =========================================
bool Input::IsDecide() {
    // コントローラー
    if (padState & PAD_INPUT_2) return true;

    // キーボード
    if (CheckHitKey(KEY_INPUT_RETURN)) return true;

    return false;
}

// デバッグ表示（押されたボタンを出す）
//void Input::DebugPrint() {
//    if (padState & PAD_INPUT_1) {
//        DrawString(50, 100, "PAD_INPUT_1: (Xbox=A)", GetColor(255, 255, 255));
//    }
//    if (padState & PAD_INPUT_2) {
//        DrawString(50, 120, "PAD_INPUT_2: (Xbox=B)", GetColor(255, 255, 255));
//    }
//}
