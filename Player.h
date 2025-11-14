#pragma once
#include "Types.h"
#include "Block.h"
#include <vector>
#include <algorithm>

// プレイヤーの体型ステータスを定義
enum class FatState {
	Thin,           // 0: 瘦せ
	SlightlyThin,   // 1: やや瘦せ
	Normal,         // 2: 普通
	SlightlyFat,    // 3: やや肥満
	Fat1,           // 4: 肥満(1度)
	Fat2,           // 5: 肥満(2度)
	Fat3,           // 6: 肥満(3度)
	Fat4            // 7: 肥満(4度)
};

class Player {
public:
	// 初期化
	void Init(const Rect& start);

	// 毎フレーム更新（ブロック参照 + deltaTime）
	void Update(std::vector<Block>& blocks, double deltaTime);

	// 描画（スクロールXオフセット込み）
	void Draw(int scrollX) const;

	// 当たり判定矩形
	Rect GetRect() const;

	void Grow(std::vector<Block>& blocks);      // 太る処理
	void Bounce();								// 敵を踏んだ時少し飛ぶ処理

	int GetFatLevel() const;         // 数値型の段階
	FatState GetFatState() const;    // enum型の体型

	void SetBlockImages(const int* images);
	int GetX() const { return x; }
	int GetY() const { return y; }

	bool IsDead() const { return isDead; }          // 生死判定
	bool IsJumping() const { return isJumping; }    // ジャンプ中判定

	// 残機関連
	void LoseLife() { lives = (lives > 0 ? lives - 1 : 0); }
	void GainLife() { lives++; }
	int GetLives() const { return lives; }
	bool HasLives() const { return lives > 0; }

	// 暗転中などで操作不能＆無敵にするフラグ
	void SetInvincible(bool value) { isInvincible = value; }
	void SetControllable(bool value) { canControl = value; }
	bool IsInvincible() const { return isInvincible; }
	bool CanControl() const { return canControl; }

	bool isDead = false;      // 死亡フラグ
	float deathTimer = 0.0f;  // 死亡後の経過時間（秒）

	// アニメーション関連
	void LoadImages();

	// 満腹度関連
	int UI_Bar_Green = -1;	//緑バーの画像
	int UI_Bar_Yellow = -1; //黄バーの画像
	int UI_Bar_Red = -1;	//赤バーの画像

	int UI_Bar_Case = -1;		//バーの枠の画像

	//体系状態UI
	int UI_Thin = -1;			//Thinの画像
	int UI_SlightlyThin = -1;	//SlightlyThin画像
	int UI_Normal = -1;			//Normalの画像
	int UI_SlightlyFat = -1;	//SlightlyFatの画像
	int UI_Fat1 = -1;			//Fat1の画像
	int UI_Fat2 = -1;			//Fat2の画像
	int UI_Fat3 = -1;			//Fat3の画像
	int UI_Fat4 = -1;			//Fat4の画像

	//警告用
	int UI_WARNING_BD = -1;
	int UI_WARNING = -1;

	float satiety = 1.0f;   // 満腹度(1.0 = 最大, 0.0 = 空腹)

private:
	void ShrinkByDistance(std::vector<Block>& blocks);   // 移動に応じて瘦せる
	
	// サイズ変更後の壁埋まり対応
	void ResolveStuckAfterResize(const std::vector<Block>& blocks);
	void FixStuckInBlock(const std::vector<Block>& blocks);

	int x, y;      // 位置
	int width;     // 幅

	int lives = 3; // 初期残機3

	float speed;
	float jumpPower;
	float velocityY;

	int imageID;	//画像
	
	// 瘦せ警告表示用
	float thinWarningTimer = 0.0f;   // 点滅用の累積時間
	bool thinWarningVisible = true;  // 表示/非表示切り替え
	const float THIN_WARNING_DURATION = 3.0f; // 警告を出す秒数
	const float THIN_WARNING_BLINK_INTERVAL = 0.6f; // 点滅間隔

	int Warning_Sound;			//警告音

	int Jump_Sound;
	int soundvolume_ = 0;

	float shrinkDistance = 500.0f; // 現在の体型ごとのやせ距離

	bool isJumping = false;
	//bool isDead = false;

	bool onGround;
	float jumpCooldownTimer = 0.0f; // ジャンプクールタイムのカウント（フレーム単位）

	bool isFacingRight = true;  // プレイヤーが右を向いているか

	// float fallTime = 0.0f;  // 地面にいない時間のカウント
	float startFreezeTimer = 3.0f;
	const float FREEZE_DURATION = 3.0f;  // 開始3秒間固定
	int fixedStartY = 0;                // 初期y座標を覚えておく

	float walkedDistance = 0.0f;    // 移動距離の統計

	const int* blockImages = nullptr;

	// 無敵＆操作フラグ
	bool isInvincible = false;
	bool canControl = true;

	// 定数群
	const int HEIGHT = 40;          //高さ(身長)
	const int WIDTH_MIN = 10;        //横幅(最低値)
	const int WIDTH_NOR = 40;       //横幅(普通)
	const int WIDTH_MAX = 360;      //横幅(最高値)

	const float SPEED_MAX = 8.0f;   //移動速度(最低値)(3.0f)
	const float SPEED_MIN = 2.0f;   //移動速度(最高値)(1.0f)

	const float JUMP_MAX = -24.0f;  //ジャンプ力(最低値)(-12.0f)//-24
	const float JUMP_MIN = -8.0f;    //ジャンプ力(最高値)(-4.0f)//-8

	const float GRAVITY = 1.00f;     //重力(0.25f)->(1.00f)
	const int FALL_LIMIT_Y = 800;

	const float DISTANCE_TO_SHRINK = 500.0f /*+ 250.0f */ ; // この距離移動すると痩せる

	const float JUMP_COOLDOWN_DURATION = 0.10f;  // ジャンプ後のクールタイム
	const int COLLISION_MARGIN = 4;             // 衝突判定用マージン
	const float SPEED_CORRECTION = 60.0f;       // deltaTimeの補正
	const float FAT_ADJUST = 2.00f;             //太った時のペナルティー

	// サイズ変更後の壁埋まり対応
	//void ResolveStuckAfterResize(const std::vector<Block>& blocks);

	//void FixStuckInBlock(const std::vector<Block>& blocks);

	//アニメーション関連
	int idleImage;            // 待機
	int jumpImage;            // ジャンプ
	int moveImages[3];        // 移動
	int currentFrame = 0;     // 現在のフレーム
	float animTimer = 0.0f;   // アニメーション用タイマー
	float animSpeed = 0.15f;  // フレーム切り替え間隔

	enum class AnimState {
		Idle,
		Move,
		Jump
	};
	AnimState animState = AnimState::Idle;

};
