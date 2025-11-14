#pragma once
#include "SceneBase.h"

class ReadyScene : public SceneBase {
private:
	bool endFlag = false;
	int waitTimer_ = 0;
	int backgroundImage = -1;  // 背景画像
	int readyImage = -1;
	int LivesImage = -1;	   //残機の画像

	//int Stage_1 = -1;
	int stageImage = -1;       // ステージ番号表示用
	int stageNumber = 1;       // 現在のステージ番号

	//int nextSceneID = 4; // ReadyScene終了後はGameSceneに遷移

	int playerLives = 3;       // プレイヤー残機を受け取る

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEnd() override;
	int NextScene() override;

	// 残機を渡す関数
	void SetLives(int lives) { playerLives = lives; }  // 残機セット

	// 外部からステージ番号を渡す関数
	void SetStage(int stage) { stageNumber = stage; }
};