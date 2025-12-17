#pragma once
#include "Types.h"
#include "Block.h"
#include <vector>
#include <algorithm>
#include <DxLib.h>

// プレイヤー設定値
namespace PlayerConfig {

	const int HEIGHT			  = 40;   //高さ(身長)
	const int WIDTH_THIN		  = 10;   //横幅(瘦せ時)
	const int WIDTH_SLIGHTLY_THIN = 25;   //横幅(やや瘦せ時)
	const int WIDTH_NORMAL		  = 40;   //横幅(普通時)
	const int WIDTH_SLIGHTLY_FAT  = 80;   //横幅(やや肥満時)
	const int WIDTH_FAT_1		  = 120;  //横幅(肥満１度時)
	const int WIDTH_FAT_2		  = 160;  //横幅(肥満２度時)
	const int WIDTH_FAT_3		  = 280;  //横幅(肥満３度時)
	const int WIDTH_FAT_4		  = 320;  //横幅(肥満４度時)

	const float SPEED_MIN = 2.0f;   //移動速度(最高値)(1.0f)
	const float SPEED_MAX = 8.0f;   //移動速度(最低値)(3.0f)
	
	const float JUMP_MIN = -8.0f;  //ジャンプ力(最高値)(- 8.0f)
	const float JUMP_MAX  = -24.0f; //ジャンプ力(最低値)(-24.0f)
	
	const float GRAVITY           = 1.00f; //重力(0.25f)->(1.00f)
	const float TERMINAL_VELOCITY = 20.0f; // 終端速度

	const int   FALL_LIMIT_Y         = 800;
	const float DISTANCE_SHRINK_MIN  = 750.0f;   // この距離移動すると痩せる(肥満時)500
	const float DISTANCE_SHRINK_NOR  = 1000.0f;  // この距離移動すると痩せる(通常時)1000
	const float DISTANCE_SHRINK_MAX  = 1500.0f;  // この距離移動すると痩せる(瘦せ時)1500
	const float DISTANCE_TO_SHRINK   = 1.0f ;    // ゲージの表示管理
	const int   DISTANCE_FIXED_VALUE = 500;      // ゲージの表示の固定値
	
	const float JUMP_COOLDOWN    = 0.10f;     // ジャンプ後のクールタイム
	
	const int   COLLISION_MARGIN = 4;         // 衝突判定用マージン
	const float SPEED_CORRECTION = 60.0f;     // deltaTimeの補正

	const float CONVERSION_JUMP      = 2.00f;   //体型変化時の変化値(ジャンプ力)
	const int   CONVERSION_WIDTH_MIN = 15;		//体型変化時の変化値(小幅)
	const int   CONVERSION_WIDTH_MAX = 40;		//体型変化時の変化値(大幅)
	const float CONVERSION_SPEED	 = 0.5f;    //体型変化時の変化値(移動速度)

	const float START_FREEZE_SEC = 3.0f; // 開始直後の固定時間
	const float THIN_WARN_SEC    = 3.0f; // 警告表示時間
	const float THIN_BLINK_INT   = 0.6f; // 警告点滅間隔

	const float DIS_BAR_YELLOW_ZONE = 0.5f;  // 満腹ゲージがここ位で色が変わる（黄色）
	const float DIS_BAR_RED_ZONE    = 0.35f; // 満腹ゲージがここ位で色が変わる（赤）
}

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

	FatState GetFatState() const;    // enum型の体型

	void SetBlockImages(const int* images);
	int GetX() const { return x; }
	int GetY() const { return y; }

	bool IsDead() const { return isDead; }          // 生死判定
	bool IsJumping() const { return isJumping; }    // ジャンプ中判定

	// 残機関連
	void LoseLife() { lives = (lives > 0 ? lives - 1 : 0); }
	void GainLife() { lives++; }
	int GetLives() const  { return lives; }
	bool HasLives() const { return lives > 0; }

	// 暗転中などで操作不能＆無敵にするフラグ
	void SetInvincible(bool value) { isInvincible = value; }
	void SetControllable(bool value) { canControl = value; }
	bool IsInvincible() const { return isInvincible; }
	bool CanControl()   const { return canControl; }

	bool isDead = false;      // 死亡フラグ
	float deathTimer = 0.0f;  // 死亡後の経過時間（秒）

	// アニメーション関連
	void LoadImages();
	const int Image = -1, Sound = -1;

	// 満腹度関連
	int UI_Bar_Green   = Image;		//緑バーの画像
	int UI_Bar_Yellow  = Image;		//黄バーの画像
	int UI_Bar_Red     = Image;		//赤バーの画像
	int UI_Bar_Case    = Image;		//バーの枠の画像

	//体系状態UI
	int UI_Thin         = Image;	//Thinの画像
	int UI_SlightlyThin = Image;	//SlightlyThin画像
	int UI_Normal       = Image;	//Normalの画像
	int UI_SlightlyFat  = Image;	//SlightlyFatの画像
	int UI_Fat1         = Image;	//Fat1の画像
	int UI_Fat2         = Image;	//Fat2の画像
	int UI_Fat3         = Image;	//Fat3の画像
	int UI_Fat4         = Image;	//Fat4の画像

	//警告用
	int UI_WARNING_BD   = Image;
	int UI_WARNING      = Image;

private:
	void ShrinkByDistance(std::vector<Block>& blocks);   // 移動に応じて瘦せる
	
	// サイズ変更後の壁埋まり対応
	void ResolveStuckAfterResize(const std::vector<Block>& blocks);
	void FixStuckInBlock(const std::vector<Block>& blocks);

	int x = 0, y = 0;					     // 中心座標
	int width = PlayerConfig::WIDTH_NORMAL;  // 幅
	int lives = 3;						     // 初期残機

	float speed;
	float jumpPower;
	float velocityY;

	//アニメーション関連
	int idleImage    = Image;  // 待機
	int jumpImage    = Image;  // ジャンプ
	int moveImages[3];         // 移動
	int currentFrame = 0;      // 現在のフレーム
	float animTimer  = 0.0f;   // アニメーション用タイマー
	float animSpeed  = 0.15f;  // フレーム切り替え間隔

	enum class AnimState { Idle, Move, Jump }; // アイドル、移動、ジャンプ
	AnimState animState = AnimState::Idle;     // 基本状態

	float satiety = 1.0f;		// 満腹度(1.0 = 最大, 0.0 = 空腹)

	const int maxOffset = 32;	//32ドット

	int soundvolume_  = 512;	//音量
	int Warning_Sound = Sound;	//警告音
	int Jump_Sound    = Sound;	//ジャンプ音

	// 瘦せ警告表示用
	float thinWarningTimer = 0.0f;    // 点滅用の累積時間
	bool  thinWarningVisible = true;  // 表示/非表示切り替え

	float shrinkDistance = 0.0f;	  // 現在の体型ごとのやせ距離
	bool isJumping = false;
	bool  onGround = false;
	float jumpCooldownTimer = 0.0f;   // ジャンプクールタイムのカウント（フレーム単位）

	bool isFacingRight = true;        // プレイヤーが右を向いているか
	
	float startFreezeTimer = PlayerConfig::START_FREEZE_SEC;
	int fixedStartY = 0;              // 初期y座標を覚えておく
	
	float walkedDistance   = PlayerConfig::DISTANCE_TO_SHRINK;    // 移動距離の統計
	const int* blockImages = nullptr;

	// 無敵＆操作フラグ
	bool isInvincible = false;
	bool canControl = true;

	// サイズ変更後の壁埋まり対応
	//void ResolveStuckAfterResize(const std::vector<Block>& blocks);

	//void FixStuckInBlock(const std::vector<Block>& blocks);

};
