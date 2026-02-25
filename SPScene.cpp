#include "SPScene.h"
#include "Input.h"
//SPはステージ2のみで獲得したスコアを記録・表示してから画面遷移する内容を想定して作成
void SPScene::Init()
{
    SPendFlag = false;
    waitTimer_ = 0;
    ClearImage = LoadGraph("image/Clear.png");//SPのみのScore内容(普通にスコアに加算される)
    //SPステージが終わったときに別でSPScoreのみを表示する内容
    //SPScore+ScoreでScoreを増やす⇒10秒経過してボーナスタイムが終わる⇒獲得したSPScoreのみを表示する(このクラスで実装を想定した)
    //獲得したSPScoreを表示したらステージ1に戻る(可能ならステージ1の初期位置希望)

    Clear_XboxImage = LoadGraph("image/Clear_Xbox.png");
    displayScore = SPscore_;

}

void SPScene::Update()
{

    // 約1秒待機してからキー入力を受付
    if (waitTimer_ > SPConfig::WAIT_FRAMES && Input::IsDecide()) {
        SPendFlag = true;
    }
}

void SPScene::Draw()
{
    // 表示フォントのサイズ
    SetFontSize(ClearConfig::FONT_SIZE);


}

bool SPScene::IsEnd()
{
    return SPendFlag || SPquitFlag;
}

int SPScene::NextScene()
{
    return 0;
}
