#include "DxLib.h"
#include "SceneManager.h"
#include <chrono>
#include "Input.h"

const int WIN_WIDTH = 1280;
const int WIN_HEIGHT = 720;

// グローバルでウィンドウモード管理
bool g_isWindowMode = false;  // 初期はウィンドウ表示
//bool g_isWindowMode = true;  // 初期はウィンドウ表示

// DxLib を初期化する関数（切り替え対応）
void InitDxLib(bool windowMode) {
    DxLib_End();  // 一度終了（既に初期化済みなら）

    ChangeWindowMode(windowMode ? TRUE : FALSE);             //フルスクリーンモードかウィンドウモード
    SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_NATIVE); //モニタ解像度そのまま
    SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);    // 幅1280px, 高さ720px, 32bitカラー
   
    SetWindowSizeChangeEnableFlag(FALSE, FALSE);             // 自動リサイズ禁止

    if (DxLib_Init() == -1) {
        // 失敗した場合は終了
        exit(-1);
    }
    SetDrawScreen(DX_SCREEN_BACK);  // 裏画面に描画

  //  DxLib_Init();                   // DxLib初期化
   
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{
    InitDxLib(g_isWindowMode);

    SceneManager manager;

    using clock = std::chrono::steady_clock;
    auto lastTime = clock::now();

    //SceneManager manager;
    //manager.ChangeScene(0);   // ゲームシーン開始

    while (ProcessMessage() == 0) {
        ClearDrawScreen();

        auto now = clock::now();
        std::chrono::duration<float> delta = now - lastTime;
        lastTime = now;
        float deltaTime = delta.count();   // 秒単位の経過時間
        if (deltaTime > 0.05f) deltaTime = 0.05f; // 最大50msまでに制限（=20FPS）

        // GameSceneのために deltaTime を保存
        //SceneBase* scene = manager.GetCurrentScene();
        //if (auto* game = dynamic_cast<GameScene*>(scene)) {
        //    game->SetDeltaTime(deltaTime);
        //}
        
        //F1キーでフルスクリーン / ウィンドウ切り替え
        if (CheckHitKey(KEY_INPUT_F1)) {
            g_isWindowMode = !g_isWindowMode;
            InitDxLib(g_isWindowMode);
        }

        manager.Update(deltaTime);
        //manager.Update();
        manager.Draw();

        Input::Update();

        // デバッグ表示
        // Input::DebugPrint();

        ScreenFlip();
    }

    DxLib_End();                // 終了処理
    return 0;
}




/*
                -メモ-

    6/22    敵、壁追加した

    6/23    絵を付けたよ
        追記:次回プレイヤーのジャンプ力
                をいじるところからスタート

    6/29    ジャンプ調整したよ

    9/4     そろそろ本気出す、次回ゲームの仕様追加

*/