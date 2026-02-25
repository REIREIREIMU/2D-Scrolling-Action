#pragma once
#include "SceneBase.h"
#include <string>
#include <DxLib.h>

// クリア画面のスコア演出設定
namespace ClearConfig {
	const int WAIT_FRAMES    = 60;  // 入力受付まで

	const int PHASE0_SEC     = 60;  // 基礎スコア表示の時間
	const int PHASE1_SEC     = 90;  // 時間ボーナス加算表示の開始時間
	const int PHASE2_SEC     = 120; // 残機ボーナス加算表示の開始時間
	const int PHASE3_SEC     = 150; // 体型ボーナス加算表示の開始時間
	//const int PHASE4_SEC     = 180; // アイテムボーナス加算表示の開始時間
	const int PHASE_NEXT_SEC = 60;  // 次のフェースに進むまで

	const int TIME_ADD_PER_STEP = 10;    // 一秒 10点加算
	const int LIFE_ADD_SCORE    = 1000;  // 残機ボーナスの点数

	const int FONT_SIZE = 60;		// フォントサイズ

	const int Up_Score_Rank_C =  5000;  // このスコアでランク：C に上がる
	const int Up_Score_Rank_B =  8000;  // このスコアでランク：B に上がる
	const int Up_Score_Rank_A = 12000;  // このスコアでランク：A に上がる
	const int Up_Score_Rank_S = 15000;  // このスコアでランク：S に上がる
}

// 各スコア表示フェーズのステータスを定義
enum class ScorePhase {
	DasisScore,   // 0: 基本スコアフェーズ
	TimeBonus,    // 1: タイムボーナスフェーズ
	LifeBonus,	  // 2: 残機ボーナスフェーズ
	BodyBonus,	  // 3: 体型ボーナスフェーズ
	ItemBonus,    // 4: アイテムボーナスフェーズ
	TotalScore    // 5: 総合計・ランクフェーズ
};

class SceneManager;

class ClearScene : public SceneBase {
private:
	bool endFlag = false;
	int ClearImage		= -1; // 背景画像のID
	int Clear_XboxImage = -1;
	int waitTimer_		= 0;  // 入力受付の待機タイマー（フレーム単位）

	int score_;			 // ゲーム終了時のスコア
	int displayScore;    // 表示用スコア（加算演出に使う）
	int timeBonus;       // 残り時間ボーナス
	int lifeBonus;       // 残機ボーナス
	int bodyBonus;       // 体型評価ボーナス
	//int itemBonus;     // やりこみアイテムボーナス

	// 文字の表示座標(X)
	const int x_a =  880, x_b = 1150;

	// 文字の表示座標(Y)
	const int y_a = 300, y_b = 370, y_c = 440, y_d = 510, y_e = 660;

	ScorePhase phase;				// 今どのフェーズか（0=基礎スコア,1=時間…）
	float timer			   = 0.0f;  // フェーズ切替用タイマー
	const float Time_Reset = 0.0f;  // フェーズタイマーのリセット
	std::string rank;				// ランク評価

	// コントローラー接続フラグ
	bool controllerConnected = false;

	// SceneManager 参照
	SceneManager* sceneManager = nullptr; 

	void DecideRank(int totalScore);

public:
	ClearScene(SceneManager* mgr) : sceneManager(mgr) {} // コンストラクタで受け取る

	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEnd() override;
	int  NextScene() override;

	// GameSceneからスコアを渡すための関数
	void SetScore(int score) { score_ = score; }

	//リザルト画面用の関数
	void SetBonuses(int time, int lives, int body /*, int items*/);
};