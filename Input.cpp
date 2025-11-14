#include "Input.h"

int Input::padState = 0;

void Input::Update() {
    padState = GetJoypadInputState(DX_INPUT_PAD1);
}

// 左右移動（左スティック or キーボード）
int Input::GetAxisX() {
    int x = 0, y = 0;
    GetJoypadAnalogInput(&x, &y, DX_INPUT_PAD1);

    // アナログスティック
    if (x < -200) return -1;
    if (x > 200) return 1;

    // キーボード
    //if (CheckHitKey(KEY_INPUT_LEFT)) return -1;
    //if (CheckHitKey(KEY_INPUT_RIGHT)) return 1;
    if (CheckHitKey(KEY_INPUT_LEFT)||CheckHitKey(KEY_INPUT_A)) return -1;
    if (CheckHitKey(KEY_INPUT_RIGHT)||CheckHitKey(KEY_INPUT_D)) return 1;

    return 0;
}

// ジャンプ（Xbox=A, Keyboard=SPACE/W）
bool Input::IsJump() {
    //if (padState & PAD_INPUT_2) return true; // PS4=× / Xbox=B
    //if (CheckHitKey(KEY_INPUT_SPACE) || CheckHitKey(KEY_INPUT_W)) return true;
    //return false;

    // コントローラー
    if (padState & PAD_INPUT_1) return true;    // Xbox=A
    // if (padState & PAD_INPUT_2) return true; // Xbox=B

    // キーボード
    if (CheckHitKey(KEY_INPUT_SPACE) || CheckHitKey(KEY_INPUT_W)) return true;

    return false;

}

// 決定（Xbox=B, Keyboard=ENTER）
bool Input::IsDecide() {
    //if (padState & PAD_INPUT_1) return true; // PS4=○ / Xbox=A
    //if (CheckHitKey(KEY_INPUT_RETURN)) return true;
    //return false;

    // コントローラー
    // if (padState & PAD_INPUT_1) return true; // Xbox=A
    if (padState & PAD_INPUT_2) return true;    // Xbox=B

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
