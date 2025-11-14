#pragma once
#include "SceneBase.h"
#include "MapData.h"
#include "Player.h"
#include "Enemy.h"
#include "Block.h"
#include <vector>
#include <array>

class GameScene : public SceneBase {
public:
	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEnd() override;
	int NextScene() override;

	void SetDeltaTime(float dt);
	void SetLives(int lives) { playerLives = lives; } // 残機を受け取る

	// 残機を外部から取得できるようにする
	//int GetPlayerLives() const { return player.GetLives(); }
	
	//ステージ関連
	GameScene(int stage) : stageNo(stage) {}
	
private:
	void Update(/*float*/ double deltaTime);
	bool CheckCollision(const Rect& a, const Rect& b);

	int scrollX = 0;                     // スクロール位置（整数型に修正）
	bool endFlag = false;               // シーン終了フラグ
	int nextSceneID = 0;                // 次のシーンID（0:タイトル, 2:ゲームオーバー, 3:クリア）

	std::vector<Block> blocks;
	std::vector<Item> items;
	std::vector<bool> itemCollected;
	std::vector<Enemy> enemies;
	std::vector<int> enemyImages;
	std::vector<int> itemImages;

	Player player;
	MapData map;

	// ブロック画像
	int blockImages[(int)BlockType::Question + 1];
	
	// レンガ破片画像(2つ)
	std::array<int, 2> brickPieceImages = { -1, -1 };

	int backgroundImage = -1; // 背景画像用

	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;

	//死亡時のエフェクトの処理
	int deathEffectImg = -1;	// 爆発エフェクト画像
	int deathEffectCols = 3;	// 横3コマ
	int deathEffectRows = 4;	// 縦4コマ

	int deathEffectFrameW = 0;
	int deathEffectFrameH = 0;

	int deathEffectFrame = 0;  // アニメーション用フレーム

	bool isDeathEffectActive = false;
	int effectX = 0, effectY = 0; // エフェクト表示位置

	float deltaTimeForUpdate = 0.0f;
	int playerLives = 3; // 現在の残機

	int score = 0;          // スコア
	double timeLimit = 0;	// 制限時間（秒）

	// フェードアウト用
	bool isGoalFade = false;   // ゴール後の暗転中フラグ
	int fadeAlpha = 0;         // 透明度 (0〜255)


	//サウンド系管理
	int Main_Bgm;//メインBGM

	int Death_Sound;		//死亡時の効果音
	int Eat_Sound;			//アイテム取得
	int BreakBrick_Sound;	//レンガブロック破壊の効果音
	int Activate_Sound;		//木箱破壊の効果音
	int Kill_Sound_1;		//敵を踏んだ時の効果音 その1
	int Kill_Sound_2;		//敵を踏んだ時の効果音 その2
	
	//サウンドの音量
	int soundvolume_ = 0;


	//ゲームUI画像
	int UI_Score = -1;			//スコアの画像
	//int UI_Bar_Case = -1;		//バーの枠の画像
	int UI_Timer = -1;			//タイマーの画像
	int UI_Player_Lives = -1;	//残機表示の画像

	int stageNo;  // どのステージか

};
