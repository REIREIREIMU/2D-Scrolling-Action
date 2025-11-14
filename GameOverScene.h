#pragma once
#include "SceneBase.h"


class GameOverScene : public SceneBase {
private:
	bool endFlag = false;
	int GameOverImage = -1; // 背景画像のID
	int GameOver_XboxImage = -1;

	int GameOverSound;

	int waitTimer_ = 0; // 入力受付の待機タイマー（フレーム単位）

	//サウンドの音量
	int soundvolume_ = 0;

	// コントローラー接続フラグ
	bool controllerConnected = false;

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEnd() override;
	int NextScene() override;
};
