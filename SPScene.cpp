#include "SPScene.h"
#include "Input.h"
//SPはステージ2のみで獲得したスコアを記録・表示してから画面遷移する内容を想定して作成
void SPScene::Init()
{
    endFlag = false;
    waitTimer_ = 0;
    ClearImage = LoadGraph("image/BonusClear.png");//SPのみのScore内容(普通にスコアに加算される)
    //SPステージが終わったときに別でSPScoreのみを表示する内容
    //SPScore+ScoreでScoreを増やす⇒10秒経過してボーナスタイムが終わる⇒獲得したSPScoreのみを表示する(このクラスで実装を想定した)
    //獲得したSPScoreを表示したらステージ1に戻る(可能ならステージ1の初期位置希望)

    Clear_XboxImage = LoadGraph("image/BonusClear_Xbox.png");
    displayScore = SPscore_;

}





void SPScene::Update()
{

    // 約1秒待機してからキー入力を受付
    if (waitTimer_ > SPConfig::WAIT_FRAMES && Input::IsDecide()) {
        endFlag = true;
    }

    waitTimer_++;

    // コントローラー接続確認
    controllerConnected = (GetJoypadNum() > 0);
}

void SPScene::Draw()
{
    // 画像が読み込まれていたら表示
    if (controllerConnected && Clear_XboxImage >= 0) {
        // コントローラー接続時
        DrawGraph(0, 0, Clear_XboxImage, TRUE);
    }
    else if (!controllerConnected && ClearImage >= 0) {
        // キーボード用
        DrawGraph(0, 0, ClearImage, TRUE);
    }

}

bool SPScene::IsEnd()
{
    return endFlag || quitFlag;
}

int SPScene::NextScene()
{
    return (int)SceneState::Game_Scene; 
}
