#pragma once
#include "SceneBase.h"
#include <string>
#include <DxLib.h>
#include "SceneManager.h"
#include "Config.h" // ColorConfig / ClearConfig を使います

// 表示・進行設定（SP 専用）
namespace SPConfig {
    constexpr int WAIT_FRAMES = 60;   // 入力受付開始まで（約1秒）
    constexpr int SHOW_DURATION_FRAMES = 600;  // 自動遷移: 約10秒（60fps想定）
}

class SPScene : public SceneBase
{
private:
    // 受け取り値
    int SPscore_ = 0; // 獲得した SP 専用スコア
    int itemCount_ = 0; // 予備（必要なら表示に使ってください）

    // 進行・制御
    bool endFlag = false;
    bool quitFlag = false;
    bool controllerConnected = false;
	int  waitTimer_ = 0;// 入力受付開始までの待機フレームカウンタ
	//int SPtimer_ = 0;// SPステージ限定のフレームカウンタ（必要なら）
    // 表示資源
    int ClearImage = -1; // 背景画像（キーボード）
    int Clear_XboxImage = -1; // 背景画像（コントローラー）
   

    // 表示値（今回は固定表示：アニメ演出なし）
    int displayScore = 0;

    // レイアウト（ClearScene と同じ座標系を流用）
    const int x_a = 880, x_b = 1150;
    const int y_a = 300, y_b = 370, y_c = 440, y_d = 510, y_e = 660;

    // シーン管理
    SceneManager* sceneManager = nullptr;

public:
    explicit SPScene(SceneManager* mgr) : sceneManager(mgr) {}

    void Init() override;
    void Update() override;
    void Draw() override;
    bool IsEnd() override;
    int  NextScene() override;

    // GameScene から SP スコアを渡す（表示だけに使う）
    void SPSetScore(int spScore, int count) { SPscore_ = spScore; itemCount_ = count; }
};