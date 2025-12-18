#include "GameOverScene.h"
#include "SceneManager.h"
#include "DxLib.h"
#include "Input.h"

void GameOverScene::Init() {
	endFlag = false;
	waitTimer_ = 0;

	GameOverImage      = LoadGraph("image/GameOver.png");
	GameOver_XboxImage = LoadGraph("image/GameOver_Xbox.png");
	GameOverSound	   = LoadSoundMem("sound/Gameover.mp3");

	//ゲームオーバーSEを再生
	PlaySoundMem(GameOverSound, DX_PLAYTYPE_BACK);
	ChangeVolumeSoundMem(soundvolume_, GameOverSound);

}

void GameOverScene::Update() {
	waitTimer_++;

	// コントローラー接続確認
	controllerConnected = (GetJoypadNum() > 0);

	// 約1秒待機してからキー入力を受付
	if (waitTimer_ > GameOverConfig::WAIT_FRAMES && Input::IsDecide()) {
		endFlag = true;
	}
}

void GameOverScene::Draw() {
	// 画像が読み込まれていたら表示
	if (GameOverImage >= 0) DrawGraph(0, 0, GameOverImage, TRUE); 

	// コントローラー接続時
	if (controllerConnected && GameOver_XboxImage >= 0)  DrawGraph(0, 0, GameOver_XboxImage, TRUE);
	// キーボード接続時
	else if (!controllerConnected && GameOverImage >= 0) DrawGraph(0, 0, GameOverImage, TRUE);	
}

bool GameOverScene::IsEnd() {
	return endFlag;
}

int GameOverScene::NextScene() {
	return (int)SceneState::Title_Scene; // タイトル画面のIDを渡す
}