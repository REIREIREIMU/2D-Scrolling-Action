#include "SPScene.h"
#include "Input.h"
#include "Config.h"

// SP はステージ2のみで獲得したスコア（SPscore_）を表示してから画面遷移する
void SPScene::Init()
{
    endFlag = false;
    quitFlag = false;
    waitTimer_ = 0;

    // 背景画像（存在しなくても致命ではないため、ロード失敗は無視）
    ClearImage = LoadGraph("image/BonusClear.png");      // キーボード用
    Clear_XboxImage = LoadGraph("image/BonusClear_Xbox.png"); // コントローラー用

    // 今回は「獲得した SPScore をそのまま見せる」仕様なのでアニメはしない
    displayScore = SPscore_;
}

void SPScene::Update()
{
    // 入力受付までの待機
    if (waitTimer_ > SPConfig::WAIT_FRAMES && Input::IsDecide()) {
        endFlag = true;
    }

    // 一定時間で自動遷移（10秒）
    if (waitTimer_ >= SPConfig::SHOW_DURATION_FRAMES) {
        endFlag = true;
    }

    // 経過フレームを進める
    waitTimer_++;

    // コントローラー接続確認
    controllerConnected = (GetJoypadNum() > 0);
}

void SPScene::Draw()
{
    // 背景
    if (controllerConnected && Clear_XboxImage >= 0) {
        DrawGraph(0, 0, Clear_XboxImage, TRUE);
    }
    else if (!controllerConnected && ClearImage >= 0) {
        DrawGraph(0, 0, ClearImage, TRUE);
    }

    // フォントサイズは ClearScene に合わせる（Config に依存）
    SetFontSize(GlobalConfig::FONT_SIZE);

    // 見出し
    DrawFormatString(x_a, y_a - 60, ColorConfig::Black, "SP SCORE");

    // SP スコア（6桁ゼロ埋め）
    DrawFormatString(x_a, y_a, ColorConfig::Black, "%06d", displayScore);

    // 必要なら SP で取得したアイテム数などを表示（コメントアウト例）
    // DrawFormatString(x_a, y_b, ColorConfig::Black, "ITEMS: %d", itemCount_);

    // 操作ガイダンス（背景にテキストがある場合は省略してもOK）
    // 1秒経過後は押下でスキップできることを示す
    if (waitTimer_ > SPConfig::WAIT_FRAMES) {
        DrawFormatString(x_a, y_e, ColorConfig::Black, "Press ENTER / A to continue");
    }
}

bool SPScene::IsEnd()
{
    return endFlag || quitFlag;
}

int SPScene::NextScene()
{
    // ここではゲーム本編に戻す（== ステージ1の初期位置復帰は SceneManager 側で対応）
    // 例：SceneManager に ResetToStage(1) や SetStage(1) があるなら、
    //     遷移前または遷移直後に呼び出してください。
    return (int)SceneState::Game_Scene;
}