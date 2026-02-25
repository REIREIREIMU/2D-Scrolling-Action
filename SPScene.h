#pragma once
#include "SceneManager.h"
#include "SceneBase.h"
#include <DxLib.h>
#include "ClearScene.h"


namespace SPConfig
{
	const int WAIT_FRAMES = 60;  // 入力受付まで

};

class SPScene : public SceneBase 
{

private:

	int SPscore_;			 // ゲーム終了時のスコア
	bool SPendFlag = false;
	bool SPquitFlag = false;    //終了用フラグ
	int ClearImage = -1; // 背景画像のID
	int Clear_XboxImage = -1;
	int waitTimer_ = 0;  // 入力受付の待機タイマー（フレーム単位）
	int displayScore;    // 表示用スコア（加算演出に使う）
	

	SceneManager* sceneManager = nullptr;

public:
	SPScene(SceneManager* mgr) : sceneManager(mgr) {}
	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEnd() override;
	int NextScene() override;

	// GameSceneからスコアを渡すための関数
	void SPSetScore(int SPscore) { SPscore_ = SPscore; }
	int SPScore;		  // ステージ2限定のスコアを保持する変数

};

