#pragma once
#include "SceneBase.h"
#include <string>

// クリア画面のスコア演出設定
namespace ClearConfig {
	const int WAIT_FRAMES    = 60;  // 入力受付まで
	const int PHASE0_SEC     = 60;  // 基礎スコア表示
	const int PHASE1_SEC     = 90;  // 時間ボーナス加算開始
	const int PHASE2_SEC     = 120; // 残機ボーナス
	const int PHASE3_SEC     = 150; // 体型ボーナス
	const int PHASE4_SEC     = 180; // アイテムボーナス
	const int PHASE_NEXT_SEC = 60;

	const int TIME_ADD_PER_STEP = 50;
	const int LIFE_ADD_SCORE = 1000;
}

class SceneManager;

class ClearScene : public SceneBase {
private:
	bool endFlag = false;
	int ClearImage = -1; // 背景画像のID
	int Clear_XboxImage = -1;

	int waitTimer_ = 0; // 入力受付の待機タイマー（フレーム単位）

	//int score_ = 0;        // スコア保持用

	int score_;			 // ゲーム終了時のスコア
	int displayScore;    // 表示用スコア（加算演出に使う）
	int timeBonus;       // 残り時間ボーナス
	int lifeBonus;       // 残機ボーナス
	int bodyBonus;       // 体型評価ボーナス
	int itemBonus;       // やりこみアイテムボーナス

	int phase;           // 今どのフェーズか（0=基礎スコア,1=時間…）
	float timer;         // フェーズ切替用タイマー

	std::string rank;    // ランク評価

	void DecideRank(int totalScore);

	// コントローラー接続フラグ
	bool controllerConnected = false;

	//SceneManager* sceneManager = nullptr; // SceneManager 参照
	SceneManager* sceneManager;

public:

	//ClearScene(SceneManager* mgr) : sceneManager(mgr) {}
	ClearScene(SceneManager* mgr) : sceneManager(mgr) {} // コンストラクタで受け取る
	//int NextScene() override;

	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEnd() override;
	int NextScene() override;

	// GameSceneからスコアを渡すための関数
	void SetScore(int score) { score_ = score; }

	//リザルト画面用の関数
	void SetBonuses(int time, int lives, int body /*, int items*/);

};