#pragma once
#include "SceneBase.h"
#include "MapData.h"
#include "Player.h"
#include "Enemy.h"
#include "Block.h"
#include "Config.h"
#include <vector>
#include <array>

// ゲーム画面設定
namespace GameConfig {
	const int   UI_FONT_SCORE = 70;	 // スコアUIのサイズ
	const int   UI_FONT_TIMER = 50;  // タイムUIのサイズ

	const float AUTO_WAIT_TIME = 2.0f;   // 一定時間
	const float TIME_LIMIT_SEC = 300.0f; // 制限時間(5分)
	const int   FADE_STEP	   = 4;	     // 1フレームごとのフェード増加

	const int   BREAK_SCORE    =  50;    // ブロック破壊時加算される点数
	const int   ITEM_SCORE     = 100;    // アイテム取得時加算される点数
	const int   ENEMY_SCORE    = 200;    // 敵を倒した時加算される点数

	const int   BODY_SCORE_NORMAL		= 200;
	const int   BODY_SCORE_SLIGHTLY_FAT = 600;
	const int   BODY_SCORE_FAT_1        = 1000;
	const int   BODY_SCORE_FAT_2        = 1500;
	const int   BODY_SCORE_FAT_3        = 3000;
	const int   BODY_SCORE_FAT_4        = 8000;

	const int   DEATH_COLS		= 3; // 爆発スプライトの横コマ
	const int   DEATH_ROWS		= 4; // 爆発スプライトの縦コマ
	const int   DEATH_FRAMES_X5 = 5; // 5フレームごとに次コマ

	const int FONT_SIZE = 20;		 // フォントサイズ

	const float ALPHA		   = 255.0f;   // 透明度
	const int   ALPHA_CONSTANT = 128;      // アルファー定数
}

class GameScene : public SceneBase {
public:
	// ステージ関連
	explicit GameScene(int stage) : stageNo(stage) {}
	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEnd() override;
	int NextScene() override;

	void SetDeltaTime(float dt);

	void SetLives(int lives) { playerLives = lives; } // 残機を受け取る
	
private:
	void Update(/*float*/ double deltaTime);
	bool CheckCollision(const Rect& a, const Rect& b);

	int scrollX     = 0;         // スクロール位置（整数型に修正）
	bool endFlag    = false;     // シーン終了フラグ
	int nextSceneID = 0;         // 次のシーンID（0:タイトル, 2:ゲームオーバー, 3:クリア）

	std::vector<Block> blocks;
	std::vector<Item> items;
	std::vector<bool> itemCollected;
	std::vector<Enemy> enemies;
	std::vector<int> enemyImages;
	std::vector<int> itemImages;

	std::vector<Rect> fallTriggers;//Lの位置を記憶しゲームシーンへ送る
	std::vector<Rect> UpTriggers;//Uの位置を記憶しゲームシーンへ送る

	Player player;
	MapData map;

	// ブロック画像
	int blockImages[(int)BlockType::Question + 1];
	
	// レンガ破片画像(2つ)
	std::array<int, 2> brickPieceImages = { -1, -1 };
	int backgroundImage = -1; // 背景画像用

	// 死亡時のエフェクトの処理
	int deathEffectImg = -1;	// 爆発エフェクト画像
	int deathEffectCols = GameConfig::DEATH_COLS;
	int deathEffectRows = GameConfig::DEATH_ROWS;

	int deathEffectFrameW = 0;
	int deathEffectFrameH = 0;
	int deathEffectFrame  = 0;  // アニメーション用フレーム

	bool isDeathEffectActive = false;
	int effectX = 0, effectY = 0; // エフェクト表示位置

	float deltaTimeForUpdate = 0.0f;

	int playerLives  = 3;							// 現在の残機
	int score		 = 0;							// スコア
	double timeLimit = GameConfig::TIME_LIMIT_SEC;	// 制限時間（秒）

	// 文字の表示座標(X)
	const int x_a = 120, x_b = 560, x_c = 1180;

	// 文字の表示座標(Y)
	const int y_a =  20, y_b =  15, y_c =   15;

	// フェードアウト用
	bool isGoalFade = false;   // ゴール後の暗転中フラグ
	int fadeAlpha = 0;         // 透明度 (0〜255)

	// サウンド系管理
	int Main_Bgm		 = -1;	// メインBGM
	int Death_Sound		 = -1;	// 死亡時の効果音
	int Eat_Sound		 = -1;	// アイテム取得
	int BreakBrick_Sound = -1;	// レンガブロック破壊の効果音
	int Activate_Sound	 = -1;  // 木箱破壊の効果音
	int Kill_Sound_1	 = -1;	// 敵を踏んだ時の効果音 その1
	int Kill_Sound_2	 = -1;	// 敵を踏んだ時の効果音 その2
	
	// サウンドの音量
	int soundvolume_ = 128;

	// ゲームUI画像
	int UI_Score = -1;			// スコアの画像
	int UI_Timer = -1;			// タイマーの画像
	int UI_Player_Lives = -1;	// 残機表示の画像

	// 当たり判定関係
	int Head_R_h = 5;  // 頭の高さ
	int	Head_R_y = 5;  // 頭の上
	int Foot_R_h = 5;  // 足の高さ
	int pixel    = 22; // 微調整用

	int stageNo;  // 現在ステージ
};
