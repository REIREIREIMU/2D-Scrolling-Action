#pragma once
#include "Types.h"
#include <vector>
#include "Block.h"

// 弾の速度を保持する構造体
struct EnemyBullet {
public:
	float x, y;     // float位置
	int w, h;       // 幅・高さ
	float vx, vy;   // 速度

	Rect GetRect() const {
		return { (int)x, (int)y, w, h };
	}
};

// 敵C（type==2）のアニメーション状態
enum class Enemy3AnimState {
	Normal,
	Ready,
	Attack
};

// 敵のクラス
class Enemy {
public:
	Rect rect;     // 位置とサイズ
	int type;      // 敵の種類 (0: 落下, 1: 穴回避, 2以降: その他)
	int vx;        // 移動方向（-1 or 1）
	float speed;   // 移動速度（例：0.5f など）

	std::vector<EnemyBullet> bullets; // 弾の速度一覧（同じ数）

	// コンストラクタ
	Enemy(Rect r, int t, int v);           // 速度はデフォルトで 1.0f
	Enemy(Rect r, int t, int v, float s);  // 速度を明示指定

	// 敵のロジック更新（移動・落下・反転）
	//void Update(const std::vector<Block>& blocks, std::vector<Enemy>& enemies, const Point& playerPos);
	//void Update(const std::vector<Block>& blocks, std::vector<Enemy>& enemies, const Rect& playerRect);
	void Update(const std::vector<Block>& blocks, std::vector<Enemy>& enemies,
		const Rect& playerRect, int scrollX, double deltaTime);

	// 描画（アニメーション対応）
	void Draw(int scrollX, int ImageID) const;

	// アニメーション画像セット
	void SetAnimImages(const std::vector<int>& images); 

	// 弾リストを外部から参照するための関数
	const std::vector<EnemyBullet>& GetBullets() const { return bullets; }

	// type==2専用アニメーション状態
	Enemy3AnimState enemy3State_ = Enemy3AnimState::Normal;
	float enemy3AnimTimer_ = 0.0f;

	// type==2専用の画像IDを保持
	int enemy3NormalImg_ = -1;
	int enemy3ReadyImg_ = -1;
	int enemy3AttackImg_ = -1;

	bool isDead = false;      // 死亡フラグ
private:
	// 地面に立っているか判定（落下の有無に使用）
	bool isOnGround(const std::vector<Block>& blocks) const;

	// 進行方向の下に地面があるか判定（落下前の反転に使用）
	bool willFallAhead(const std::vector<Block>& blocks) const;

	bool isFacingRight = false; // 左向き（初期値）true

	// ジャンプ用
	float jumpTimer = 0.0f;
	float bulletCooldown = 0.0f;
	float verticalVelocity = 0.0f;

	float posX_f = 0.0f;
	float posY_f = 0.0f;

	//std::vector<EnemyBullet> shots;

	// アニメーション管理
	std::vector<int> animImages_; // Enemy1.png, Enemy2.png
	int currentAnimFrame_ = 0;
	float animTimer_ = 0.0f;
	float animFrameDuration_ = 0.2f; // 0.2秒ごとに切り替え

};
