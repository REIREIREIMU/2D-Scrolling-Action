#include "Enemy.h"
#include "Config.h"
#include <DxLib.h>
#include <cmath>
#include <algorithm>

// 敵の設定値
namespace EnemyConfig {
	float Jump_Power		  = -80.0f;	 // 敵のジャンプ力
	float Gravity			  = 12.0f;	 // 敵の重力加速度
	float Bullet_Speed		  = 200.0f;	 // 弾の速度（遅めに設定）
	float Bullet_Cooldown	  = 3.0f;    // 弾のクールダウン
	int   Bullet_Size		  = 16;		 // 弾のサイズ
	float Jump_Interval		  = 5.0f;    // ジャンプ間隔
	float Bullet_Angle_Offset = 0.0f;	 // 弾の角度分散（15度 = 0.26f）
	float Bullet_Range		  = 450.0f;	 // 画面外でも生きてる最大距離
}

// コンストラクタ（速度付き）
Enemy::Enemy(Rect r, int t, int v)
	: rect(r), type(t), vx(v), speed(1.0f)
{
	posX_f = (float)r.x;
	posY_f = (float)r.y;
	isFacingRight = (vx < 0);
}

// コンストラクタ（速度なし → 速度1.0として初期化）
Enemy::Enemy(Rect r, int t, int v, float s)
	: rect(r), type(t), vx(v), speed(s)
{
	posX_f = (float)r.x;
	posY_f = (float)r.y;
	isFacingRight = (vx < 0);
}

void Enemy::SetAnimImages(const std::vector<int>& images) {
	animImages_ = images;
	currentAnimFrame_ = 0;
	animTimer_ = 0.0f;
}

void Enemy::Update(const std::vector<Block>& blocks, std::vector<Enemy>& enemies, 
								const Rect& playerRect, int scrollX, double deltaTime)
{
	// 敵の移動処理（速度反映）
	float dx = vx * speed * (float)deltaTime;

	// ここで dx を次の位置用に再利用する
	Rect nextRect = rect;
	nextRect.x += (int)dx;

	// 壁に当たったら反転
	bool reversed = false;

	//  壁に当たったら反転
	for (const auto& block : blocks)
	{
		if (!block.HasCollision()) continue;
		if (nextRect.Intersects(block.GetRect())) {
			vx = -vx;                       // 方向を反転
			isFacingRight = (vx < 0);       // 画像も反転
			reversed = true;
			break;		// 反転だけして抜ける
		}
	}

	// 他の敵との衝突判定
	for (auto& other : enemies) {
		if (&other == this) continue;

		//敵タイプCは他の敵との接触判定を無視する
		if (type == 2 || other.type == 2) continue;
		if (nextRect.Intersects(other.rect)) {
			vx = -vx;                       // 方向を反転
			isFacingRight = (vx < 0);       // 画像も反転
			reversed = true;	
			break;		// 反転だけして抜ける	
		}
	}

	// 通常移動
	posX_f += vx * speed * (float)deltaTime;
	rect.x = (int)posX_f;

	// 足元に地面がなければ落下
	if (!isOnGround(blocks)) {
		verticalVelocity += EnemyConfig::Gravity * (float)deltaTime;
		posY_f += verticalVelocity * (float)deltaTime;					// 落下処理
		rect.y = (int)posY_f;
	}
	else
	{
		if (verticalVelocity > 0.0f)
		{
			verticalVelocity = 0.0f; // 地面にいるなら速度リセット
		}
	}

	// Enemy2（type == 1）だけは穴の前で反転
	if (type == 1 && willFallAhead(blocks)) {
		vx = -vx;
		isFacingRight = (vx < 0);	// 画像も反転
	}

	// Enemy3（type == 2）：ジャンプ＋弾幕
	if (type == 2) {

		//Enemy3のアニメーション
		bulletCooldown -= (float)deltaTime;

		// onGround 状態を確認
		bool onGround = isOnGround(blocks);

		// 地面にいるときだけジャンプタイマー進行
		if (onGround) {
			jumpTimer += (float)deltaTime; // 地面にいるときだけジャンプタイマー進行
		}

		// プレイヤーの方向を向く（x座標比較）
		if (playerRect.x + playerRect.w / 2 < rect.x + rect.w / 2) {
			vx = GlobalConfig::LEFT;  // プレイヤーが左にいる → 左向き
			isFacingRight = true;

		}
		else {
			vx = GlobalConfig::RIGHT;  // プレイヤーが右にいる → 右向き
			isFacingRight = false;
		}

		// 定期的にジャンプ
		if (jumpTimer > EnemyConfig::Jump_Interval && onGround) {
			verticalVelocity = EnemyConfig::Jump_Power;				// 上向きにジャンプ
			jumpTimer = 0.0f;
		}

		// 重力適用
		verticalVelocity += EnemyConfig::Gravity * (float)deltaTime;
		posY_f += verticalVelocity * (float)deltaTime;
		rect.y = (int)posY_f;

		// 地面にぶつかったら着地
		for (const auto& block : blocks) {
			if (!block.HasCollision()) continue;
			if (rect.Intersects(block.GetRect())) {
				if (verticalVelocity > 0.0f) {
					// 落下中にのみ補正
					rect.y -= (int)(verticalVelocity * deltaTime);
					posY_f = (float)rect.y;
					verticalVelocity = 0.0f;
				}
				break;
			}
		}

		// 敵3のアニメ状態＆射撃制御
		switch (enemy3State_) {
		case Enemy3AnimState::Normal:
			if (bulletCooldown <= 1.5f) { // 攻撃準備状態に移行（クールダウン残り1.5秒から）
				enemy3State_ = Enemy3AnimState::Ready;
				enemy3AnimTimer_ = 0.0f;
			}
			break;

		case Enemy3AnimState::Ready:
			enemy3AnimTimer_ += (float)deltaTime;
			if (enemy3AnimTimer_ >= 1.0f) { // 1秒 Ready 表示後に Attack 状態へ
				enemy3State_ = Enemy3AnimState::Attack;
				enemy3AnimTimer_ = 0.0f;

				// 弾発射処理
				int dx = (playerRect.x + playerRect.w / 2) - (rect.x + rect.w / 2);
				int dy = (playerRect.y + playerRect.h / 2) - (rect.y + rect.h / 2);
				float baseAngle = std::atan2(dy, dx);

				for (int i = -1; i <= 1; ++i) {
					float angle = baseAngle + i * EnemyConfig::Bullet_Angle_Offset;
					float bvx = std::cos(angle) * EnemyConfig::Bullet_Speed;
					float bvy = std::sin(angle) * EnemyConfig::Bullet_Speed;

					EnemyBullet bullet;
					bullet.x = rect.x + rect.w / GlobalConfig::Break_Number;
					bullet.y = rect.y + rect.h / GlobalConfig::Break_Number;
					bullet.vx = bvx;
					bullet.vy = bvy;
					bullet.w = EnemyConfig::Bullet_Size;
					bullet.h = EnemyConfig::Bullet_Size;
					bullets.push_back(bullet);
				}
			}
			break;

		case Enemy3AnimState::Attack:
			enemy3AnimTimer_ += (float)deltaTime;
			if (enemy3AnimTimer_ >= 0.5f) { // 攻撃モーション終了
				enemy3State_ = Enemy3AnimState::Normal;
				enemy3AnimTimer_ = 0.0f;

				// 攻撃後はクールダウン再セット
				bulletCooldown = EnemyConfig::Bullet_Cooldown;
			}
			break;
		}

		// 弾移動
		for (auto& bullet : bullets) {
			bullet.x += bullet.vx * (float)deltaTime;
			bullet.y += bullet.vy * (float)deltaTime;
		}

		bullets.erase(
			std::remove_if(bullets.begin(), bullets.end(), [&](const EnemyBullet& b) {
				Rect r = b.GetRect();

				// 壁に衝突したら削除
				for (const auto& block : blocks) {
					if (!block.HasCollision()) continue;
					if (r.Intersects(block.GetRect())) return true;
				}

				// 画面外に行ったら削除
				float dx = b.x - (rect.x + rect.w / GlobalConfig::Break_Number);
				float dy = b.y - (rect.y + rect.h / GlobalConfig::Break_Number);
				float distSq = dx * dx + dy * dy;
				float maxRange = EnemyConfig::Bullet_Range;
				return distSq > maxRange * maxRange || r.y + r.h < 0 || r.y > GlobalConfig::SCREEN_HEIGHT;
				}),
			bullets.end()
		);
	}

	//  歩行アニメーション更新
	if (!animImages_.empty()) {
		animTimer_ += (float)deltaTime;
		if (animTimer_ >= animFrameDuration_) {
			animTimer_ = 0.0f;
			currentAnimFrame_++;
			if (currentAnimFrame_ >= (int)animImages_.size()) {
				currentAnimFrame_ = 0;
			}
		}
	}
}

void Enemy::Draw(int scrollX, int ImageID) const
{
	int left = rect.x - scrollX;
	int right = rect.x + rect.w - scrollX;

	if (!isFacingRight) {
		std::swap(left, right); // 左右反転
	}

	// アニメーション画像を使う。画像がなければ dummyImageID を使う
	int img = ImageID;
	if (type == 2) {
		switch (enemy3State_)
		{
		case Enemy3AnimState::Normal:		// 通常時のアニメーション画像
			if (enemy3NormalImg_ != -1) img = enemy3NormalImg_;
			break;
		case Enemy3AnimState::Ready:		// 攻撃準備時のアニメーション画像
			if (enemy3ReadyImg_  != -1) img = enemy3ReadyImg_;
			break;
		case Enemy3AnimState::Attack:		// 攻撃時のアニメーション画像
			if (enemy3AttackImg_ != -1) img = enemy3AttackImg_;
			break;
		}
	}
	else if (!animImages_.empty()) {
		img = animImages_[currentAnimFrame_];
	}

	// 敵画像
	DrawExtendGraph(left, rect.y, right, rect.y + rect.h, img, TRUE);

	// 弾（赤い四角）
	for (const auto& b : bullets) {
		Rect r = b.GetRect();
		int bx = r.x - scrollX;
		int by = r.y;
		DrawBox(bx, by, bx + r.w, by + r.h, GetColor(255, 0, 0), TRUE);
	}
}

// 地面に乗っているか判定
bool Enemy::isOnGround(const std::vector<Block>& blocks) const {
	Rect foot = rect;
	foot.y ++;	      // 少し下の位置を確認

	for (const auto& block : blocks)
	{
		if (!block.HasCollision()) continue;
		if (foot.Intersects(block.GetRect())) return true;
	}

	return false;
}

// 進行方向に地面があるか調べる（Enemy2 専用）
bool Enemy::willFallAhead(const std::vector<Block>& blocks) const {
	// 足の先端を確認
	int checkX = (vx > 0) ? rect.x + rect.w + 1 : rect.x - 1;
	int checkY = rect.y + rect.h + 1;

	Rect probe = { checkX, checkY, 1, 1 }; // 小さなチェック点

	for (const auto& block : blocks) {
		if (!block.HasCollision()) continue;
		if (probe.Intersects(block.GetRect())) return false; // 地面あり
	}
	return true; // 地面なし → 落ちる
}
