#include "ClearScene.h"
#include "Input.h"
#include "Config.h"
#include "SceneManager.h"

void ClearScene::Init() {
	endFlag = false;
	waitTimer_ = 0;
	ClearImage      = LoadGraph("image/Clear.png");
    Clear_XboxImage = LoadGraph("image/Clear_Xbox.png");

	displayScore = score_ + timeBonus + lifeBonus + bodyBonus /*+ itemBonus*/;
    phase; timer = 0.0f; rank.clear();
}

//リザルト表示用
void ClearScene::SetBonuses(int time, int lives, int body /*, int items*/) {
	timeBonus = time;
	lifeBonus = lives;
	bodyBonus = body;
	//itemBonus = items;
}


void ClearScene::Update() {
	waitTimer_ ++;
    timer += 1.0f;
   
    // コントローラー接続確認
    controllerConnected = (GetJoypadNum() > 0);

	// 約1秒待機してからキー入力を受付
	if (waitTimer_ > ClearConfig::WAIT_FRAMES && Input::IsDecide()) {
		endFlag = true;
	}

    switch (phase) {
    // 基礎スコア表示フェーズ
    case ScorePhase::DasisScore :
        if (timer > ClearConfig::PHASE0_SEC) { 
            phase = ScorePhase::TimeBonus;  // タイムボーナスフェーズへ移行
            timer = Time_Reset;
        }
        break;
    // 残り時間加算フェーズ
    case ScorePhase::TimeBonus :
        if (timer > ClearConfig::PHASE1_SEC) {
            int add = min(ClearConfig::TIME_ADD_PER_STEP, timeBonus); // 50点ずつ加算演出
            displayScore += add;
            timeBonus    -= add;
        }
        else {
            if (timer > ClearConfig::PHASE_NEXT_SEC) {
                phase = ScorePhase::LifeBonus;  // 残機ボーナスフェーズへ移行
                timer = Time_Reset;
            }
        }
        break;
    // 残機加算フェーズ
    case ScorePhase::LifeBonus :
        if (timer > ClearConfig::PHASE2_SEC) {
           displayScore += ClearConfig::LIFE_ADD_SCORE;
           lifeBonus--;
        }
        else {
            if (timer > ClearConfig::PHASE_NEXT_SEC) {
                phase = ScorePhase::BodyBonus;  // 体型ボーナスフェーズへ移行
                timer = Time_Reset;
            }
        }
        break;
    // 体型評価加算フェーズ
    case ScorePhase::BodyBonus :
        if (timer > ClearConfig::PHASE3_SEC) {
            displayScore += bodyBonus;
            bodyBonus = 0;
        }
        if (timer > ClearConfig::PHASE_NEXT_SEC) {
            phase = ScorePhase::TotalScore;  // 総合計・ランクフェーズへ移行
            timer = Time_Reset;
        }
        break;
    // アイテム加算
    //case ScorePhase::ItemBonus :
    //    if (timer > ClearConfig::PHASE4_SEC) {
    //        displayScore += itemBonus;
    //        itemBonus = 0;
    //    }
    //    if (timer > ClearConfig::PHASE_NEXT_SEC) {
    //        phase = ScorePhase::TotalScore; 
    //        timer = Time_Reset; 
    //    }
    //    break;

    // 最終スコアを算出してランク決定
    case ScorePhase::TotalScore :
        int finalScore = displayScore + timeBonus + lifeBonus *
            ClearConfig::LIFE_ADD_SCORE + bodyBonus /*+ itemBonus*/;
        if (rank.empty())      { DecideRank(finalScore); }
        if (Input::IsDecide()) { endFlag = true; }
        break;
    }
}

void ClearScene::Draw() {
	// 画像が読み込まれていたら表示
    if (controllerConnected && Clear_XboxImage >= 0) {
        // コントローラー接続時
        DrawGraph(0, 0, Clear_XboxImage, TRUE);
    }
    else if (!controllerConnected && ClearImage >= 0) {
        // キーボード用
        DrawGraph(0, 0, ClearImage, TRUE);
    }

    // 表示フォントのサイズ
	SetFontSize(ClearConfig::FONT_SIZE); 

    // 基礎スコア
    if (phase >= ScorePhase::DasisScore) DrawFormatString( x_a, y_a, ColorConfig::Black, "%06d", score_);
    // 残り時間ボーナス
    if (phase >= ScorePhase::TimeBonus)  DrawFormatString( x_a, y_b, ColorConfig::Black, "%06d", timeBonus);
    // 残機ボーナス
    if (phase >= ScorePhase::LifeBonus)  DrawFormatString( x_a, y_c, ColorConfig::Black, "%06d", lifeBonus);
    // 体型評価
    if (phase >= ScorePhase::BodyBonus)  DrawFormatString( x_a, y_d, ColorConfig::Black, "%06d", bodyBonus);
   
    //やりこみアイテム
    //if (phase >= ScorePhase::ItemBonus) DrawFormatString( x, y + 280, cloor/*GetColor(255, 0, 255)*/, " %d / 3", itemBonus);

    if (phase == ScorePhase::TotalScore) {
        // スコア総合計
        DrawFormatString( x_a, y_e, ColorConfig::Cyan, "%06d", displayScore);
        // ランク評価
        DrawFormatString( x_b, y_d, ColorConfig::Black, "%s", rank.c_str());
    }
}

void ClearScene::DecideRank(int totalScore) {

    totalScore = displayScore;    // 総合計を直接受け渡す

    if      (totalScore <= ClearConfig::Up_Score_Rank_C) rank = "D"; // Dランク：スコア 5000以下
    else if (totalScore <= ClearConfig::Up_Score_Rank_B) rank = "C"; // Cランク：スコア 5001以上～スコア8000以下
    else if (totalScore <= ClearConfig::Up_Score_Rank_A) rank = "B"; // Bランク：スコア 8001以上～スコア12000以下
    else if (totalScore <= ClearConfig::Up_Score_Rank_S) rank = "A"; // Aランク：スコア12001以上～スコア15000以下
    else                                                 rank = "S"; // Sランク：スコア15001以上
}

bool ClearScene::IsEnd() {
	return endFlag;
}

int ClearScene::NextScene()
{
    if (sceneManager->GetCurrentStage() < sceneManager->GetMaxStage()) {
        sceneManager->IncrementStage();      // ステージを進める
        return (int)SceneState::Ready_Scene; // 準備画面のIDを渡す
    }
    else {
        return (int)SceneState::Title_Scene; // タイトル画面のIDを渡す
    }
}