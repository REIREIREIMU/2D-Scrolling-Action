#include "ReadyScene.h"
#include <string>

void ReadyScene::Init() {
	endFlag = false;
	waitTimer_ = 0;

	backgroundImage = LoadGraph("image/Background.png");
	ReadyImage      = LoadGraph("image/Ready_BG.png");
	LivesImage      = LoadGraph("image/Ready_Player.png");

	// ステージ番号に応じた画像を切り替え
	switch (CurrentStage) {
	case 1:  StageImage = LoadGraph("image/Stage_1.png"); break;
	case 2:  StageImage = LoadGraph("image/Stage_2.png"); break;
	case 3:  StageImage = LoadGraph("image/Stage_3.png"); break;
	default: StageImage = LoadGraph("image/Stage_1.png"); break;
	}
}

void ReadyScene::Update() {
	waitTimer_++;

	// 約2秒待機後に自動終了
	if (waitTimer_ > ReadySceneConfig::AUTO_END_FRAMES) {
		endFlag = true;
	}
}

void ReadyScene::Draw() {

	// 背景描画
	if (backgroundImage >= 0) DrawGraph(0, 0, backgroundImage, TRUE);

	//薄黒の帯画像
	if (ReadyImage		>= 0) DrawGraph(0, 0, ReadyImage, TRUE);

	//残機のプレイヤー
	if (LivesImage		>= 0) DrawGraph(0, 0, LivesImage, TRUE);

	//ステージ表示画像
	if (StageImage		>= 0) DrawGraph(0, 0, StageImage, TRUE);

	// 残機数を表示
	SetFontSize(40);
	std::string text = "×" + std::to_string(PlayerLives);
	DrawString(620, 350, text.c_str(), GetColor(255, 255, 255));
}

bool ReadyScene::IsEnd() {
	return endFlag;
}

int ReadyScene::NextScene() {
	return (int)SceneState::Game_Scene;	 // プレイ画面のIDを渡す
}
