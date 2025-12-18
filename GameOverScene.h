#pragma once
#include "SceneBase.h"
#include <DxLib.h>

// GameOver画面の設定
namespace GameOverConfig {
	const int WAIT_FRAMES = 60; // 入力受付までの待機
}

class GameOverScene : public SceneBase {
private:
	bool endFlag = false;
	int GameOverImage	   = -1; // 背景画像のID(キーボード時)
	int GameOver_XboxImage = -1; // 背景画像のID(コントローラー時)
	int GameOverSound	   = -1; // ゲームオーバーのSE

	int waitTimer_   = 0;    // 入力受付の待機タイマー（フレーム単位）
	int soundvolume_ = 128;  //サウンドの音量

	bool controllerConnected = false;  // コントローラー接続フラグ

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEnd() override;
	int NextScene() override;
};
