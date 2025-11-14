#include "GameOverScene.h"
#include "DxLib.h"
#include "Input.h"

void GameOverScene::Init() {
	endFlag = false;
	waitTimer_ = 0;

	GameOverImage = LoadGraph("image/GameOver.png");
	GameOver_XboxImage = LoadGraph("image/GameOver_Xbox.png");

	GameOverSound = LoadSoundMem("sound/Gameover.mp3");
	soundvolume_ = 128;

	//ゲームオーバーSEを再生
	PlaySoundMem(GameOverSound, DX_PLAYTYPE_BACK);
	ChangeVolumeSoundMem(soundvolume_, GameOverSound);

}

void GameOverScene::Update() {

	waitTimer_++;

	// コントローラー接続確認
	controllerConnected = (GetJoypadNum() > 0);

	// 約1秒待機してからキー入力を受付
	if (waitTimer_ > 60 && Input::IsDecide()) {
		endFlag = true;
	}
}

void GameOverScene::Draw() {
	//DrawString(500, 300, "ゲームオーバー", GetColor(255, 0, 0));
	//DrawString(480, 360, "SPACEキーでタイトルに戻る", GetColor(255, 255, 255));

	// 画像が読み込まれていたら表示
	if (GameOverImage >= 0)
	{
		DrawGraph(0, 0, GameOverImage, TRUE); 

		//SetFontSize(40);
		//DrawString(260, 600, "死因：病名はありません。栄養失調です。", GetColor(255, 0, 0));
		//SetFontSize(20);
	}

	if (controllerConnected && GameOver_XboxImage >= 0) {
		
		// コントローラー接続時
		DrawGraph(0, 0, GameOver_XboxImage, TRUE);

	}
	else if (!controllerConnected && GameOverImage >= 0) {
		
		// キーボード用
		DrawGraph(0, 0, GameOverImage, TRUE);
		
	}
}

bool GameOverScene::IsEnd() {
	return endFlag;
}

int GameOverScene::NextScene() {
	return 0; // タイトル画面へ
}