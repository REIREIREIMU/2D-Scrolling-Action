#pragma once
#include "SceneManager.h"
#include "SceneBase.h"
#include <DxLib.h>

// Ready画面の設定
namespace ReadySceneConfig {
	const int  AUTO_END_FRAMES = 120; // 2秒で自動終了（60FPS）
}

class ReadyScene : public SceneBase{
private:
	bool endFlag = false;
	int waitTimer_ = 0;

	int backgroundImage = -1;  // 背景画像
	int ReadyImage      = -1;  // 黒い帯状の画像
	int LivesImage      = -1;  // 残機の画像
	int StageImage      = -1;  // ステージ番号表示用

	int CurrentStage = 1;							     // 現在のステージ番号
	int PlayerLives  = SceneFlowConfig::START_LIVES;     // プレイヤー残機を受け取る

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEnd() override;
	int NextScene() override;

	// 残機を渡す関数
	void SetLives(int lives) { PlayerLives = lives; }  // 残機セット

	// 外部からステージ番号を渡す関数
	void SetStage(int stage) { CurrentStage = stage; } // ステージセット
};