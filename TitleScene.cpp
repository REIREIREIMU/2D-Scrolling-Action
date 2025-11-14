#include "TitleScene.h"
#include "DxLib.h"
#include "Input.h"


void TitleScene::Init() {
	endFlag = false;
	waitTimer_ = 0;
	scrollX = 0;

	backgroundImage = LoadGraph("image/background.png"); // ← 画像のパスに合わせてください
	TitleImage = LoadGraph("image/Title.png");
	StartImage = LoadGraph("image/Start.png");
	Start_XboxImage = LoadGraph("image/Start_Xbox.png");

	// キー入力をクリアして、前シーンのキー入力を無効化
	char keys[256];
	GetHitKeyStateAll(keys);  // 全キーの状態を取得して無視することでリセット

	startAlpha_ = 255.0f; // 初期は不透明
	alphaSpeed_ = -5.0f;  // 徐々に消えていく
}

void TitleScene::Update(/*float deltaTime*/) {
	waitTimer_++;

	// コントローラー接続確認
	controllerConnected = (GetJoypadNum() > 0);

	// 背景スクロール処理
	scrollX -= scrollSpeed;
	int bgWidth, bgHeight;
	GetGraphSize(backgroundImage, &bgWidth, &bgHeight);
	if (scrollX <= -bgWidth) {
		scrollX = 0;
	}

	// 約1秒待機してからキー入力を受付
	if (waitTimer_ > 60 && Input::IsDecide()) {
		endFlag = true;		// GameScene に遷移
	}

	// 点滅処理（アルファ値を上下させる）
	startAlpha_ += alphaSpeed_;
	if (startAlpha_ <= 50.0f) {    // 最小値で反転
		startAlpha_ = 50.0f;
		alphaSpeed_ = +5.0f;
	}
	if (startAlpha_ >= 255.0f) {   // 最大値で反転
		startAlpha_ = 255.0f;
		alphaSpeed_ = -5.0f;
	}

	// Escキーでゲーム終了
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		quitFlag = true;
	}
}

void TitleScene::Draw(){
	// 背景塗りつぶし（背景画像が無いときの安全策）
	DrawBox(0, 0, 1280, 720, GetColor(50, 150, 170), TRUE);

	// 背景画像が読み込まれていたら表示
	if (backgroundImage >= 0) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);

		int bgWidth, bgHeight;
		GetGraphSize(backgroundImage, &bgWidth, &bgHeight);

		// ループするように2枚描画
		DrawGraph(scrollX, 0, backgroundImage, TRUE);
		DrawGraph(scrollX + bgWidth, 0, backgroundImage, TRUE);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		//for (int x = 0; x < 1280; x += bgWidth) {
		//	DrawGraph(x, 0/* -100*/, backgroundImage, TRUE); // Yは少し上にずらしてます
		//}

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 文字列と中央位置
	//const char* text = "Press SPACE to Start";
	//int textWidth = GetDrawStringWidth(text, strlen(text));
	//int drawX = (1280 - textWidth) / 2;
	//int drawY = 400;

	// 画像が読み込まれていたら表示
	if (TitleImage >= 0) {

		//int bgWidth, bgHeight;
		//GetGraphSize(backgroundImage, &bgWidth, &bgHeight);

		//for (int x = 0; x < 1280; x += bgWidth) {
			DrawGraph(0, 0, TitleImage, TRUE); // Yは少し上にずらしてます
		//}

	}

	//スタートボタン画像
	//画像が読み込まれていたら表示
	if (controllerConnected && Start_XboxImage >= 0) {
		//点滅処理つき
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)startAlpha_);
		// コントローラー接続時
		DrawGraph(0, 0, Start_XboxImage, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else if (!controllerConnected && StartImage >= 0) {
		//点滅処理つき
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)startAlpha_);
		// キーボード用
		DrawGraph(0, 0, StartImage, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// タイトル文字など描画
	//SetFontSize(40);
	//DrawString(drawX - 80 , drawY + 120, "Press Enter to Start", GetColor(255, 255, 255));
	//SetFontSize(20);

	//DrawString(500, 300, "タイトル画面", GetColor(255, 255, 255));
	//DrawString(500, 340, "Enterキーでスタート", GetColor(255, 255, 0));
}

bool TitleScene::IsEnd() {

	return endFlag || quitFlag;
}

int TitleScene::NextScene() {

	//ソフト終了
	if (quitFlag) {
		DxLib_End();
		return -1; 
	}

	return 1; // GameSceneのID
}