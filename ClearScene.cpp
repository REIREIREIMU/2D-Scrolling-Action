#include "ClearScene.h"
#include "DxLib.h"
#include "Input.h"
#include "SceneManager.h"

void ClearScene::Init() {
	endFlag = false;
	waitTimer_ = 0;

	ClearImage = LoadGraph("image/Clear.png");
    Clear_XboxImage = LoadGraph("image/Clear_Xbox.png");

	displayScore = score_ + timeBonus + lifeBonus + bodyBonus + itemBonus;
	phase = 0;
	timer = 0.0f;

    rank = "";
}

//リザルト表示用
void ClearScene::SetBonuses(int time, int lives, int body /*, int items*/) {
	timeBonus = time;
	lifeBonus = lives;
	bodyBonus = body;
	//itemBonus = items;
}


void ClearScene::Update() {
	waitTimer_++;

    // コントローラー接続確認
    controllerConnected = (GetJoypadNum() > 0);

	// 約1秒待機してからキー入力を受付
	if (waitTimer_ > 60 && Input::IsDecide()) {
		endFlag = true;
	}

    timer += 1.0f;

    switch (phase) {
    case 0: // 基礎スコア表示
        if (timer > 60) { phase = 1; timer = 0; }
        break;

    case 1: // 残り時間加算
        if (/*timeBonus > 0*/timer > 90) {
            int add = min(50, timeBonus); // 50点ずつ加算演出
            displayScore += add;
            timeBonus -= add;
        }
        else {
            if (timer > 60) { phase = 2; timer = 0; }
        }
        break;

    case 2: // 残機加算
        if (/*lifeBonus > 0*/timer > 120) {
           displayScore += 1000;
           lifeBonus--;

        }
        else {
            if (timer > 60) { phase = 3; timer = 0; }
        }
        break;

    case 3: // 体型評価加算
        if (/*bodyBonus > 0*/timer > 150) {
            displayScore += bodyBonus;
            bodyBonus = 0;
        }
        if (timer > 60) { phase = 4; timer = 0; }
        break;

    case 4: // アイテム加算
        if (/*itemBonus > 0*/timer > 180) {
            displayScore += itemBonus;
            itemBonus = 0;
        }
        if (timer > 60) { phase = 5; timer = 0; }
        break;

    case 5: // 全て終了 → キー入力待ち
    {
        // 最終スコアを算出してランク決定
        int finalScore = displayScore + timeBonus + lifeBonus * 1000 + bodyBonus + itemBonus;
        if (rank.empty()) {
            DecideRank(finalScore);
        }
        if (Input::IsDecide()) {
            endFlag = true;
        }
        break;
    }
    }

}

void ClearScene::Draw() {
//	DrawString(500, 300, "ステージクリア！", GetColor(0, 255, 0));
//	DrawString(460, 360, "SPACEキーでタイトルに戻る", GetColor(255, 255, 255));

	// 画像が読み込まれていたら表示
    if (controllerConnected && Clear_XboxImage >= 0) {

        // コントローラー接続時
        DrawGraph(0, 0, Clear_XboxImage, TRUE);

    }
    else if (!controllerConnected && ClearImage >= 0) {

        // キーボード用
        DrawGraph(0, 0, ClearImage, TRUE);

    }

	SetFontSize(60);
	//DrawString(740, 220, "2025/09/16", GetColor(0, 0, 0));

	//DrawString(880, 300, "162.5", GetColor(0, 0, 0));
	//DrawString(880, 370, "61.5(仮)", GetColor(0, 0, 0));
	//DrawString(880, 440, "58.1(仮)", GetColor(0, 0, 0));
	//DrawString(700, 510, "普通　23.3(仮)", GetColor(0, 0, 0));
	
    int x = 880;
    int y = 300;

    int cloor = GetColor( 20, 20, 20);

    //基礎スコア
    if (phase >= 0) DrawFormatString( x, y, cloor/*GetColor(255, 255, 0)*/, "%06d", score_);
   
    //残り時間ボーナス
    if (phase >= 1) DrawFormatString( x, y + 70, cloor/*GetColor(0, 255, 255)*/, "%06d", timeBonus);
   
    //残機ボーナス
    if (phase >= 2) DrawFormatString( x, y + 140, cloor/*GetColor(0, 255, 0)*/, "%06d", lifeBonus);
   
    //体型評価
    if (phase >= 3) DrawFormatString( x, y + 210, cloor/*GetColor(255, 128, 0)*/, "%06d", bodyBonus);
    
    //やりこみアイテム
    //if (phase >= 4) DrawFormatString( x, y + 280, cloor/*GetColor(255, 0, 255)*/, " %d / 3", itemBonus);

	//DrawString(880, 580, " XXX(仮)", GetColor(0, 0, 0));
	//DrawFormatString(880, 580, GetColor(255, 255, 0), "%06d", score_);

    if (phase == 5) {
        DrawFormatString( x, y + 360, GetColor( 0, 255, 255), "%06d", displayScore /*+ timeBonus + lifeBonus + bodyBonus + itemBonus*/);
        //DrawString(100, 450, "ENTERで次へ", GetColor(255, 255, 255));
        
        //ランク評価
        DrawFormatString(1150, 510, cloor/*GetColor(255, 255, 0)*/, "%s", rank.c_str());
    }

	//DrawString(1150, 510, "A", GetColor(0, 0, 0));
	SetFontSize(20);
}

void ClearScene::DecideRank(int totalScore) {
    if (displayScore <= 5000) rank = "D";
    else if (5001 <= displayScore && displayScore <= 8000) rank = "C";
    else if (8001 <= displayScore && displayScore <= 12000) rank = "B";
    else if (12001 <= displayScore && displayScore <= 15000) rank = "A";
    else if (15001 <= displayScore) rank = "S";
}

bool ClearScene::IsEnd() {
	return endFlag;
}

int ClearScene::NextScene()
{
    //if (sceneManager == nullptr) return 0; // 念のため

    //if (sceneManager->GetCurrentStage() < sceneManager->GetMaxStage()) {
    //    sceneManager->NextStage();  // currentStage++ と ReadyScene への遷移
    //    return -1;                  // SceneManager 内で遷移済み
    //}
    //else {
    //    return 0; // タイトルに戻す
    //}

    if (sceneManager->GetCurrentStage() < sceneManager->GetMaxStage()) {
        sceneManager->IncrementStage(); // ステージを進める
        return 1; // ReadyScene
    }
    else {
        return 0; // TitleScene
    }
    
	//return 0; // タイトル画面へ
}