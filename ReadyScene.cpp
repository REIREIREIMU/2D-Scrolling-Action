#include "ReadyScene.h"
#include "DxLib.h"
#include <string>

void ReadyScene::Init() {
	endFlag = false;
	waitTimer_ = 0;

	backgroundImage = LoadGraph("image/Background.png");
	readyImage = LoadGraph("image/Ready_BG.png");
	LivesImage = LoadGraph("image/Ready_Player.png");

	// ステージ番号に応じた画像を切り替え
	//Stage_1 = LoadGraph("image/Stage_1.png");
	switch (stageNumber) {
	case 1: stageImage = LoadGraph("image/Stage_1.png"); break;
	case 2: stageImage = LoadGraph("image/Stage_2.png"); break;
	case 3: stageImage = LoadGraph("image/Stage_3.png"); break;
	default: stageImage = LoadGraph("image/Stage_1.png"); break;
	}

}

void ReadyScene::Update() {
	waitTimer_++;

	// 約2秒待機後に自動終了
	if (waitTimer_ > 120) {
		endFlag = true;
	}
}

void ReadyScene::Draw() {

	// 背景描画
	if (backgroundImage >= 0) 
	{
		DrawGraph(0, 0, backgroundImage, TRUE);
	}

	//薄黒の帯画像
	if (readyImage >= 0)
	{
		DrawGraph(0, 0, readyImage, TRUE);
	}

	//残機のプレイヤー
	if (LivesImage >= 0)
	{
		DrawGraph(0, 0, LivesImage, TRUE);
	}

	//ステージ表示画像
	//if (Stage_1 >= 0)
	if (stageImage >= 0)
	{
		//DrawGraph(0, 0, Stage_1, TRUE);
		DrawGraph(0, 0, stageImage, TRUE);
	}



	// READY文字
	//SetFontSize(40);
	//DrawString(400, 300, "READY!", GetColor(255, 255, 0));

	SetFontSize(40);
	//DrawString(600, 300, "1-1", GetColor(255, 255, 0));

	// 残機数を表示
	std::string text = "×" + std::to_string(playerLives);
	DrawString(620, 350, text.c_str(), GetColor(255, 255, 255));
}

bool ReadyScene::IsEnd() {
	return endFlag;
}

int ReadyScene::NextScene() {
	// GameScene の ID を返す想定（例: 4）
	return 4;
}
