#include "TitleScene.h"
#include "Input.h"
#include "Config.h"

void TitleScene::Init() {
	endFlag  = false; 
	quitFlag = false;
	waitTimer_ = 0;
	scrollX    = 0;

	backgroundImage = LoadGraph("image/background.png"); 
	TitleImage		= LoadGraph("image/Title.png");
	StartImage		= LoadGraph("image/Start.png");
	Start_XboxImage = LoadGraph("image/Start_Xbox.png");

	// キー入力初期化（直前シーンの押下状態をクリア）
	char keys[256];
	GetHitKeyStateAll(keys);  // 全キーの状態を取得して無視することでリセット

	startAlpha_ = TitleSceneConfig::ALPHA_MAX;	  // 初期は不透明
	alphaSpeed_ = -TitleSceneConfig::ALPHA_STEP;  // 徐々に消えていく
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
	if (waitTimer_ > TitleSceneConfig::WAIT_FRAMES && Input::IsDecide()) {
		endFlag = true;		// GameScene に遷移
	}

	// 点滅処理（アルファ値を上下させる）
	startAlpha_ += alphaSpeed_;
	if (startAlpha_ <= TitleSceneConfig::ALPHA_MIN) {    // 最小値で反転
		startAlpha_ =  TitleSceneConfig::ALPHA_MIN;
		alphaSpeed_ = +TitleSceneConfig::ALPHA_STEP;
	}
	if (startAlpha_ >= TitleSceneConfig::ALPHA_MAX) {    // 最大値で反転
		startAlpha_ =  TitleSceneConfig::ALPHA_MAX;
		alphaSpeed_ = -TitleSceneConfig::ALPHA_STEP;
	}

	// ESCで終了
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		quitFlag = true;
	}
}

void TitleScene::Draw(){
	// 背景塗りつぶし（背景画像が無いときの安全策）
	DrawBox(0, 0, GlobalConfig::SCREEN_WIDTH,
			GlobalConfig::SCREEN_HEIGHT, GetColor(50, 150, 170), TRUE);

	// 背景画像が読み込まれていたら表示
	if (backgroundImage >= 0) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);

		int bgWidth, bgHeight;
		GetGraphSize(backgroundImage, &bgWidth, &bgHeight);

		// ループするように2枚描画
		DrawGraph(scrollX,           0, backgroundImage, TRUE);
		DrawGraph(scrollX + bgWidth, 0, backgroundImage, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 画像が読み込まれていたら表示
	if (TitleImage >= 0) {
		DrawGraph(0, 0, TitleImage, TRUE); // タイトルロゴ
	}

	//点滅処理
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)startAlpha_);

	//画像が読み込まれていたら表示
	if (controllerConnected && Start_XboxImage >= 0) {
		// コントローラー接続時のスタートボタン画像
		DrawGraph(0, 0, Start_XboxImage, TRUE);
	}
	else if (!controllerConnected && StartImage >= 0) {
		// キーボード時のスタートボタン画像
		DrawGraph(0, 0, StartImage, TRUE);
	}
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

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

	return (int)SceneState::Ready_Scene; // 準備画面のIDを渡す
}