#include <DxLib.h>
#include <chrono>
#include "SceneManager.h"
#include "Config.h"
#include "Input.h"

// ウィンドウモード管理
//bool g_isWindowMode = false;  // 初期はフルスクリーン表示
bool g_isWindowMode = true;     // 初期はウィンドウ表示

// DxLib を初期化する関数（切り替え対応）
void InitDxLib(bool windowMode) {
    DxLib_End();  // 再初期化対応

    ChangeWindowMode(windowMode ? TRUE : FALSE);             //フルスクリーンモードかウィンドウモード
    SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_NATIVE); //モニタ解像度そのまま
    SetGraphMode(GlobalConfig::SCREEN_WIDTH,                 // 幅1280px
                 GlobalConfig::SCREEN_HEIGHT,                // 高さ720px
                 GlobalConfig::BIT_COLOR);                   // 32bitカラー
    SetWindowSizeChangeEnableFlag(FALSE, FALSE);             // 自動リサイズ禁止

    if (DxLib_Init() == -1) {
        // 失敗した場合は終了
        exit(-1);
    }
    SetDrawScreen(DX_SCREEN_BACK);  // 裏画面に描画
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{
    InitDxLib(g_isWindowMode);
    SceneManager manager;

    using clock = std::chrono::steady_clock;
    auto lastTime = clock::now();

    while (ProcessMessage() == 0) {
        ClearDrawScreen();

        auto now = clock::now();
        std::chrono::duration<float> delta = now - lastTime;
        lastTime = now;
        float deltaTime = delta.count();                // 秒単位の経過時間
        if (deltaTime > GlobalConfig::MAX_DELTA_TIME) {
            deltaTime = GlobalConfig::MAX_DELTA_TIME;   // 最大50msまでに制限（=20FPS）
        }

        //F1キーでフルスクリーン / ウィンドウ切り替え
        if (CheckHitKey(KEY_INPUT_F1)) {
            g_isWindowMode = !g_isWindowMode;
            InitDxLib(g_isWindowMode);
        }

        manager.Update(deltaTime);
        manager.Draw();

        Input::Update();

        ScreenFlip();
    }
    DxLib_End();                // 終了処理
    return 0;
}