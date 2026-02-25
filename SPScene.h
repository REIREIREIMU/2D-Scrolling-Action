#include "SceneManager.h"
#include "SceneBase.h"
#include <iostream> 
#include <DxLib.h>


namespace SPConfig
{
	const int WAIT_FRAMES = 60;  // 入力受付まで
	const int PHASE_NEXT_SEC = 60;  // 次のフェースに進むまで
};





class SPScene : public SceneBase
{

private:

	int SPscore_;			 // ゲーム終了時のスコア
	int itemCount_;
	bool endFlag = false;
	bool quitFlag = false;    //終了用フラグ
	int ClearImage = -1; // 背景画像のID
	int Clear_XboxImage = -1;
	int waitTimer_ = 0;  // 入力受付の待機タイマー（フレーム単位）
	int displayScore;    // 表示用スコア（加算演出に使う）

	// コントローラー接続フラグ
	bool controllerConnected = false;

	// 文字の表示座標(X)
	const int x_a = 880, x_b = 1150;

	// 文字の表示座標(Y)
	const int y_a = 300, y_b = 370, y_c = 440, y_d = 510, y_e = 660;

	float timer = 0.0f;  // フェーズ切替用タイマー
	const float Time_Reset = 0.0f;  // フェーズタイマーのリセット
	std::string rank;				// ランク評価



	SceneManager* sceneManager = nullptr;

	void DecideRank(int totalScore);

public:
	SPScene(SceneManager* mgr) : sceneManager(mgr) {}
	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEnd() override;
	int NextScene() override;

	// GameSceneからスコアを渡すための関数
	void SPSetScore(int SPscore, int count) { SPscore_ = SPscore, itemCount_ = count; }

};

