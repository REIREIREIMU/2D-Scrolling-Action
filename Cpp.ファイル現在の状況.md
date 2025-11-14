### **・Cpp.ファイル**



##### **//Block.cpp**

\#include "Block.h"



Block::Block(Rect rect, BlockType type)

 	: rect(rect), type(type) {

}



void Block::SetImage(int imageID\_) {

 	imageID = imageID\_;

}



void Block::Draw(int scrollX) const {

 	if (imageID >= 0) {

 		//画像が設定されているなら画像を描画

 		DrawExtendGraph(

 			rect.x - scrollX, rect.y,

 			rect.x + rect.w - scrollX, rect.y + rect.h,

 			imageID,

 			TRUE

 		);

 	}

 	else {

 		// 画像なしなら色付きボックスで描画

 		int color = GetColor(200, 200, 200);  // デフォルト色



 		switch (type) {

 		case BlockType::GroundA:

 			color = GetColor(139, 69, 19);   // 茶色

 			break;



 		case BlockType::GroundB:

 			color = GetColor(160, 82, 45);   // 明るい茶色

 			break;



 		case BlockType::Brick:

 			color = GetColor(255, 100, 100); // 赤っぽいレンガ

 			break;



 		case BlockType::Question:

 			color = GetColor(255, 255, 0);   // 黄色

 			break;



 		case BlockType::Goal:

 			color = GetColor( 0, 0, 0);   // 黒

 			break;

 		}



 		DrawBox(rect.x - scrollX, rect.y, rect.x + rect.w - scrollX, rect.y + rect.h, color, TRUE);

 	}

}



const Rect\& Block::GetRect() const {

 	return rect;

}



BlockType Block::GetType() const {

 	return type;

}



bool Block::HasCollision() const {

 	//return true;



 	return type == BlockType::GroundA ||

 		type == BlockType::GroundB ||

 		type == BlockType::Brick ||

 		type == BlockType::Question;

}



bool Block::IsActivated() const {

 	return activated;

}



void Block::Activate() {

 	activated = true;

 	type = BlockType::GroundB; // 使い終わったブロックに見た目変更



 	// 画像IDを GroundB の画像に変更

 	if (blockImages != nullptr) {

 		imageID = blockImages\[(int)BlockType::GroundB];

 	}

}



void Block::SetType(BlockType newType) {

 	type = newType;

}



void Block::SetBlockImages(const int\* images) {

 	blockImages = images;

}



##### **//ClearScene.cpp**

\#include "ClearScene.h"

\#include "DxLib.h"



void ClearScene::Init() {

 	endFlag = false;

}



void ClearScene::Update() {

 	// スペースキーでタイトルに戻る

 	if (CheckHitKey(KEY\_INPUT\_SPACE)) {

 		endFlag = true;

 	}

}



void ClearScene::Draw() {

 	DrawString(500, 300, "ステージクリア！", GetColor(0, 255, 0));

 	DrawString(460, 360, "SPACEキーでタイトルに戻る", GetColor(255, 255, 255));

}



bool ClearScene::IsEnd() {

 	return endFlag;

}



int ClearScene::NextScene() {

 	return 0; // タイトル画面に戻る

}



##### **//Enemy.cpp**

\#include "Enemy.h"

\#include <DxLib.h>

\#include <cmath>

\#include <algorithm>



// 敵の行動に関するパラメータ設定

namespace EnemyConfig {

&nbsp;	// ジャンプ関連

&nbsp;	float GetJumpPower() { return -80.0f; }    // 敵のジャンプ力	(-80.0f)

&nbsp;	float GetGravity() { return 12.0f; }     // 敵の重力加速度	(12.0f)



&nbsp;	// 弾関連

&nbsp;	float GetBulletSpeed() { return 200.0f; }  // 弾の速度（遅めに設定）(1.0f)



&nbsp;	// 弾のクールダウン

&nbsp;	float GetBulletCooldown() { return 3.0f; }  //3秒ごとに発射(320)



&nbsp;	// 弾のサイズ

&nbsp;	int GetBulletSize() { return 16; }			//(16)



&nbsp;	// ジャンプ間隔

&nbsp;	float GetJumpInterval() { return 5.0f; }	//5秒ごとにジャンプ(380.0f)



&nbsp;	// 弾の角度分散（15度 = 0.26f）

&nbsp;	float GetBulletAngleOffset() { return 0.52f; }//(30度)



&nbsp;	float GetBulletRange() { return 600.0f; } // 画面外でも生きてる最大距離

}



// 画面サイズを定義

const int SCREEN\_WIDTH = 1280;

const int SCREEN\_HEIGHT = 720;



// コンストラクタ（速度付き）

Enemy::Enemy(Rect r, int t, int v)

&nbsp;	: rect(r), type(t), vx(v), speed(1.0f)

{

&nbsp;	posX\_f = (float)r.x;

&nbsp;	posY\_f = (float)r.y;

&nbsp;	isFacingRight = (vx < 0);

&nbsp;	animImages\_ = {};

&nbsp;	currentAnimFrame\_ = 0;

&nbsp;	animTimer\_ = 0.0f;

}



// コンストラクタ（速度なし → 速度1.0として初期化）

Enemy::Enemy(Rect r, int t, int v, float s)

&nbsp;	: rect(r), type(t), vx(v), speed(s)

{

&nbsp;	posX\_f = (float)r.x;

&nbsp;	posY\_f = (float)r.y;

&nbsp;	isFacingRight = (vx < 0);

&nbsp;	animImages\_ = {};

&nbsp;	currentAnimFrame\_ = 0;

&nbsp;	animTimer\_ = 0.0f;

}



void Enemy::SetAnimImages(const std::vector<int>\& images) {

&nbsp;	animImages\_ = images;

&nbsp;	currentAnimFrame\_ = 0;

&nbsp;	animTimer\_ = 0.0f;



}



//void Enemy::Update(const std::vector<Block>\& blocks, std::vector<Enemy>\& enemies, const Point\& playerPos)

void Enemy::Update(const std::vector<Block>\& blocks, std::vector<Enemy>\& enemies, const Rect\& playerRect, int scrollX, double deltaTime)

{

&nbsp;	// 敵の移動処理（速度反映）

&nbsp;	float dx = vx \* speed \* (float)deltaTime;



&nbsp;	// ここで dx を次の位置用に再利用する

&nbsp;	Rect nextRect = rect;



&nbsp;	nextRect.x += (int)dx;



&nbsp;	// 壁に当たったら反転

&nbsp;	bool reversed = false;



&nbsp;	//  壁に当たったら反転

&nbsp;	for (const auto\& block : blocks)

&nbsp;	{

&nbsp;		if (!block.HasCollision()) continue;

&nbsp;		if (nextRect.Intersects(block.GetRect())) {

&nbsp;			vx = -vx;                       // 方向を反転

&nbsp;			isFacingRight = (vx < 0);       // 画像も反転

&nbsp;			reversed = true;

&nbsp;			break;							// 反転だけしてループ抜ける

&nbsp;		}

&nbsp;	}



&nbsp;	// 他の敵との衝突判定

&nbsp;	for (auto\& other : enemies) {

&nbsp;		if (\&other == this) continue;



&nbsp;		if (nextRect.Intersects(other.rect)) {

&nbsp;			vx = -vx;                       // 方向を反転

&nbsp;			isFacingRight = (vx < 0);       // 画像も反転

&nbsp;			reversed = true;

&nbsp;			break;							// 反転だけしてループ抜ける

&nbsp;		}

&nbsp;	}



&nbsp;	// 通常移動

&nbsp;	posX\_f += vx \* speed \* (float)deltaTime;

&nbsp;	rect.x = (int)posX\_f;



&nbsp;	// 足元に地面がなければ落下

&nbsp;	if (!isOnGround(blocks)) {

&nbsp;		verticalVelocity += EnemyConfig::GetGravity() \* (float)deltaTime;

&nbsp;		posY\_f += verticalVelocity \* (float)deltaTime; // 落下処理

&nbsp;		rect.y = (int)posY\_f;

&nbsp;	}

&nbsp;	else

&nbsp;	{

&nbsp;		if (verticalVelocity > 0.0f)

&nbsp;		{

&nbsp;			verticalVelocity = 0.0f; // 地面にいるなら速度リセット

&nbsp;		}

&nbsp;	}



&nbsp;	// Enemy2（type == 1）だけは穴の前で反転

&nbsp;	if (type == 1 \&\& willFallAhead(blocks)) {

&nbsp;		vx = -vx;

&nbsp;		isFacingRight = (vx < 0);	// 画像も反転

&nbsp;	}



&nbsp;	// Enemy3（type == 2）：ジャンプ＋弾幕

&nbsp;	if (type == 2) {



&nbsp;		//Enemy3のアニメーション

&nbsp;		bulletCooldown -= (float)deltaTime;



&nbsp;		// onGround 状態を確認

&nbsp;		bool onGround = isOnGround(blocks);



&nbsp;		// 地面にいるときだけジャンプタイマー進行

&nbsp;		if (onGround) {

&nbsp;			jumpTimer += (float)deltaTime; // 地面にいるときだけジャンプタイマー進行

&nbsp;		}



&nbsp;		// プレイヤーの方向を向く（x座標比較）

&nbsp;		if (playerRect.x + playerRect.w / 2 < rect.x + rect.w / 2) {

&nbsp;			vx = -1;  // プレイヤーが左にいる → 左向き

&nbsp;			isFacingRight = true;



&nbsp;		}

&nbsp;		else {

&nbsp;			vx = 1;   // プレイヤーが右にいる → 右向き

&nbsp;			isFacingRight = false;

&nbsp;		}



&nbsp;		// 距離を測定（プレイヤー位置狙い）

&nbsp;		//int dx = (playerRect.x + playerRect.w / 2) - (rect.x + rect.w / 2);

&nbsp;		//int dy = (playerRect.y + playerRect.h / 2) - (rect.y + rect.h / 2);

&nbsp;		//float distance = std::sqrt(dx \* dx + dy \* dy);



&nbsp;		// 定期的にジャンプ

&nbsp;		if (jumpTimer > EnemyConfig::GetJumpInterval() \&\& onGround) {

&nbsp;			verticalVelocity = EnemyConfig::GetJumpPower();   // 上向きにジャンプ

&nbsp;			jumpTimer = 0.0f;

&nbsp;		}



&nbsp;		// 重力適用

&nbsp;		verticalVelocity += EnemyConfig::GetGravity() \* (float)deltaTime;

&nbsp;		posY\_f += verticalVelocity \* (float)deltaTime;

&nbsp;		rect.y = (int)posY\_f;



&nbsp;		// 地面にぶつかったら着地

&nbsp;		for (const auto\& block : blocks) {

&nbsp;			if (!block.HasCollision()) continue;

&nbsp;			if (rect.Intersects(block.GetRect())) {

&nbsp;				if (verticalVelocity > 0.0f) {

&nbsp;					// 落下中にのみ補正

&nbsp;					rect.y -= (int)(verticalVelocity \* deltaTime);

&nbsp;					posY\_f = (float)rect.y;

&nbsp;					verticalVelocity = 0.0f;

&nbsp;				}

&nbsp;				break;

&nbsp;			}

&nbsp;		}



&nbsp;		//bulletCooldown -= (float)deltaTime;// 弾のクールダウンは常に減らす



&nbsp;		// 敵が画面内にいるかどうか判定

&nbsp;		//bool isOnScreen =

&nbsp;		//	rect.x + rect.w > scrollX \&\&

&nbsp;		//	rect.x < scrollX + SCREEN\_WIDTH \&\&

&nbsp;		//	rect.y + rect.h > 0 \&\&

&nbsp;		//	rect.y < SCREEN\_HEIGHT;



&nbsp;		// 弾発射（画面内にいるときだけ）

&nbsp;		//if (bulletCooldown <= 1.0f)

&nbsp;		//{

&nbsp;		//	float baseAngle = std::atan2(dy, dx);



&nbsp;		//	// 3方向弾（-15°, 0°, +15°）分散

&nbsp;		//	for (int i = -1; i <= 1; ++i) {

&nbsp;		//		float angle = baseAngle + i \* EnemyConfig::GetBulletAngleOffset();

&nbsp;		//		float bvx = std::cos(angle) \* EnemyConfig::GetBulletSpeed();

&nbsp;		//		float bvy = std::sin(angle) \* EnemyConfig::GetBulletSpeed();



&nbsp;		//		EnemyBullet bullet;

&nbsp;		//		bullet.x = rect.x + rect.w / 2.0f;

&nbsp;		//		bullet.y = rect.y + rect.h / 2.0f;

&nbsp;		//		bullet.vx = bvx;

&nbsp;		//		bullet.vy = bvy;

&nbsp;		//		bullet.w = EnemyConfig::GetBulletSize();

&nbsp;		//		bullet.h = EnemyConfig::GetBulletSize();

&nbsp;		//		bullets.push\_back(bullet);  // ← 弾ごとの速度追加

&nbsp;		//	}



&nbsp;		//	bulletCooldown = EnemyConfig::GetBulletCooldown(); // クールダウン

&nbsp;		//}





&nbsp;		switch (enemy3State\_) {

&nbsp;		case Enemy3AnimState::Normal:

&nbsp;			if (bulletCooldown <= 1.5f) { // 攻撃準備状態に移行（クールダウン残り1.5秒から）

&nbsp;				enemy3State\_ = Enemy3AnimState::Ready;

&nbsp;				enemy3AnimTimer\_ = 0.0f;

&nbsp;			}

&nbsp;			break;



&nbsp;		case Enemy3AnimState::Ready:

&nbsp;			enemy3AnimTimer\_ += (float)deltaTime;

&nbsp;			if (enemy3AnimTimer\_ >= 1.0f) { // 1秒 Ready 表示後に Attack 状態へ

&nbsp;				enemy3State\_ = Enemy3AnimState::Attack;

&nbsp;				enemy3AnimTimer\_ = 0.0f;



&nbsp;				// 弾発射処理

&nbsp;				int dx = (playerRect.x + playerRect.w / 2) - (rect.x + rect.w / 2);

&nbsp;				int dy = (playerRect.y + playerRect.h / 2) - (rect.y + rect.h / 2);

&nbsp;				float baseAngle = std::atan2(dy, dx);



&nbsp;				for (int i = -1; i <= 1; ++i) {

&nbsp;					float angle = baseAngle + i \* EnemyConfig::GetBulletAngleOffset();

&nbsp;					float bvx = std::cos(angle) \* EnemyConfig::GetBulletSpeed();

&nbsp;					float bvy = std::sin(angle) \* EnemyConfig::GetBulletSpeed();



&nbsp;					EnemyBullet bullet;

&nbsp;					bullet.x = rect.x + rect.w / 2.0f;

&nbsp;					bullet.y = rect.y + rect.h / 2.0f;

&nbsp;					bullet.vx = bvx;

&nbsp;					bullet.vy = bvy;

&nbsp;					bullet.w = EnemyConfig::GetBulletSize();

&nbsp;					bullet.h = EnemyConfig::GetBulletSize();

&nbsp;					bullets.push\_back(bullet);

&nbsp;				}

&nbsp;			}

&nbsp;			break;



&nbsp;		case Enemy3AnimState::Attack:

&nbsp;			enemy3AnimTimer\_ += (float)deltaTime;

&nbsp;			if (enemy3AnimTimer\_ >= 0.5f) { // 攻撃モーション終了

&nbsp;				enemy3State\_ = Enemy3AnimState::Normal;

&nbsp;				enemy3AnimTimer\_ = 0.0f;



&nbsp;				// 攻撃後はクールダウン再セット

&nbsp;				bulletCooldown = EnemyConfig::GetBulletCooldown();

&nbsp;			}

&nbsp;			break;

&nbsp;		}



&nbsp;		// 弾移動

&nbsp;		for (auto\& bullet : bullets) {

&nbsp;			bullet.x += bullet.vx \* deltaTime;

&nbsp;			bullet.y += bullet.vy \* deltaTime;

&nbsp;		}



&nbsp;		bullets.erase(

&nbsp;			std::remove\_if(bullets.begin(), bullets.end(), \[\&](const EnemyBullet\& b) {

&nbsp;				Rect r = b.GetRect();



&nbsp;				// 壁に衝突したら削除

&nbsp;				for (const auto\& block : blocks) {

&nbsp;					if (!block.HasCollision()) continue;

&nbsp;					if (r.Intersects(block.GetRect())) return true;

&nbsp;				}



&nbsp;				float dx = b.x - (rect.x + rect.w / 2.0f);

&nbsp;				float dy = b.y - (rect.y + rect.h / 2.0f);

&nbsp;				float distSq = dx \* dx + dy \* dy;

&nbsp;				float maxRange = EnemyConfig::GetBulletRange();

&nbsp;				return distSq > maxRange \* maxRange || r.y + r.h < 0 || r.y > SCREEN\_HEIGHT;

&nbsp;				}),

&nbsp;			bullets.end()

&nbsp;		);

&nbsp;	}



&nbsp;	// アニメーション更新

&nbsp;	if (!animImages\_.empty()) {

&nbsp;		animTimer\_ += (float)deltaTime;

&nbsp;		if (animTimer\_ >= animFrameDuration\_) {

&nbsp;			animTimer\_ = 0.0f;

&nbsp;			currentAnimFrame\_++;

&nbsp;			if (currentAnimFrame\_ >= (int)animImages\_.size()) {

&nbsp;				currentAnimFrame\_ = 0;

&nbsp;			}

&nbsp;		}

&nbsp;	}

}



void Enemy::Draw(int scrollX, int ImageID) const

{

&nbsp;	int left = rect.x - scrollX;

&nbsp;	int right = rect.x + rect.w - scrollX;



&nbsp;	if (!isFacingRight) {

&nbsp;		std::swap(left, right); // 左右反転

&nbsp;	}



&nbsp;	// アニメーション画像を使う。画像がなければ dummyImageID を使う

&nbsp;	int img = ImageID;

&nbsp;	//if (!animImages\_.empty()) {

&nbsp;	//	img = animImages\_\[currentAnimFrame\_];

&nbsp;	//}

&nbsp;	if (type == 2) {

&nbsp;		switch (enemy3State\_)

&nbsp;		{

&nbsp;		case Enemy3AnimState::Normal:

&nbsp;			if (enemy3NormalImg\_ != -1) img = enemy3NormalImg\_;

&nbsp;			break;

&nbsp;		case Enemy3AnimState::Ready:

&nbsp;			if (enemy3ReadyImg\_ != -1) img = enemy3ReadyImg\_;

&nbsp;			break;

&nbsp;		case Enemy3AnimState::Attack:

&nbsp;			if (enemy3AttackImg\_ != -1) img = enemy3AttackImg\_;

&nbsp;			break;

&nbsp;		}

&nbsp;	}

&nbsp;	else if (!animImages\_.empty()) {

&nbsp;		img = animImages\_\[currentAnimFrame\_];

&nbsp;	}



&nbsp;	// 敵画像

&nbsp;	DrawExtendGraph(left, rect.y, right, rect.y + rect.h, img, TRUE);



&nbsp;	// 弾（赤い四角）

&nbsp;	for (const auto\& b : bullets) {

&nbsp;		Rect r = b.GetRect();

&nbsp;		int bx = r.x - scrollX;

&nbsp;		int by = r.y;

&nbsp;		DrawBox(bx, by, bx + r.w, by + r.h, GetColor(255, 0, 0), TRUE);

&nbsp;	}

}



////敵の弾幕

//void Enemy::DrawBullets(int scrollX) const {

//	for (const auto\& b : shots) {

//		DrawCircle((int)b.x - scrollX, (int)b.y, 5, GetColor(255, 100, 100), TRUE);

//	}

//}

//

//// プレイヤーの当たり判定チェック

//bool Enemy::CheckBulletCollision(const Rect\& playerRect) {

//	for (auto it = shots.begin(); it != shots.end();) {

//		Rect bulletRect = { (int)(it->x - 3), (int)(it->y - 3), 6, 6 };

//

//		if (bulletRect.Intersects(playerRect)) {

//			it = shots.erase(it);

//			return true; // 当たった

//		}

//		else {

//			++it;

//		}

//	}

//	return false;

//}



// 地面に乗っているか判定

bool Enemy::isOnGround(const std::vector<Block>\& blocks) const {

&nbsp;	Rect foot = rect;

&nbsp;	foot.y += 1; // 少し下の位置を確認



&nbsp;	for (const auto\& block : blocks)

&nbsp;	{

&nbsp;		if (!block.HasCollision()) continue;

&nbsp;		if (foot.Intersects(block.GetRect())) return true;

&nbsp;	}



&nbsp;	return false;

}



// 進行方向に地面があるか調べる（Enemy2 専用）

bool Enemy::willFallAhead(const std::vector<Block>\& blocks) const {

&nbsp;	// 足の先端を確認

&nbsp;	int checkX = (vx > 0) ? rect.x + rect.w + 1 : rect.x - 1;

&nbsp;	int checkY = rect.y + rect.h + 1;



&nbsp;	Rect probe = { checkX, checkY, 1, 1 }; // 小さなチェック点



&nbsp;	for (const auto\& block : blocks) {

&nbsp;		if (!block.HasCollision()) continue;

&nbsp;		if (probe.Intersects(block.GetRect())) return false; // 地面あり

&nbsp;	}

&nbsp;	return true; // 地面なし → 落ちる

}



##### **//GameOverScene.cpp**

\#include "GameOverScene.h"

\#include "DxLib.h"



void GameOverScene::Init() {

 	endFlag = false;

}



void GameOverScene::Update() {

 	// スペースキーでタイトルに戻る

 	if (CheckHitKey(KEY\_INPUT\_SPACE)) {

 		endFlag = true;

 	}

}



void GameOverScene::Draw() {

 	DrawString(500, 300, "ゲームオーバー", GetColor(255, 0, 0));

 	DrawString(480, 360, "SPACEキーでタイトルに戻る", GetColor(255, 255, 255));

}



bool GameOverScene::IsEnd() {

 	return endFlag;

}



int GameOverScene::NextScene() {

 	return 0; //nextSceneID を返す

}





##### **//GameScene.cpp**

\#include "GameScene.h"

\#include "DxLib.h"



// アイテム種類をランダムで返す

int GetRandomItemType() {

 	return GetRand(4);  // 0〜4 のアイテム番号

}



/\*

   '#' = 壁

   'I' = アイテム

   'E' = 敵

   'P' = プレイヤー初期位置

\*/



const std::vector[std::string](std::string) mapText = {

 	// "                                                                                                                                                                                                                             ",

 	// "                                                                                                                                                                                                                             ",

 	// "                                                                                                                                                                                                                             ",

 	// "                                                                                                                                                                                                                             ",

 	// "                                                                                                                                                                                                                             ",

 	// "                                                                                                                                                                                                                             ",

 	// "                                                                                                                                                                                                                             ",

 	// "                                                                                                                                                                                                                             ",

 	// "                                                                                                                                                                                                                             ",

 	// "                                                                                                                                                                                                                             ",

 	// "																																																							   ",

 	// "																																																							   ",

 	// "											///																																									   ",

 	// "/										    ///                                         /																															   ",

 	// "/    　       　　　　																/																																   ",

 	// "/  											 								        /                                                                                                                                      ",

 	// "=============================================================================================================================================   =============================================================================",

 	// "=============================================================================================================================================   ============================================================================="

 	////0         1         2         3         4         5         6         7         8         9         A         B         C         D         E         F        10        11        12        13        14        15        16         7         8         9

 	//上の数は10マスごとに16進数で数えられている



 	//α(拡張版)ステージ

 	 "                                                                                                                                                                                                                                                                                                                       ",

 	 "                                                                                                                                                                                                                                                                                                                       ",

 	 "                                                                                                                                                                                                                                                                                                                       ",

 	 "                                                                                                                                                                                                                                                                                                                       ",

 	 "                                                                                                                                                                                                                                                                                                                       ",

 	 "                                                                                                                                                                                                                                                                                                                       ",

 	 "                                                                                                                                                                                                                                                                                                                   $   ",

 	 "                                                                                                    4 1 3 5 2                                                                                                                                         C                                            1    5          $   ",

 	 "                      ?                                                                           ###########?                                            ?                                                        4      5                         ?###?                                     2                    $   ",

 	 "                                    2    4      3   1                                                                                                                                                                                                                                    3            C            $   ",

 	 "                                                                                        ?    ?                                        3                                                                                                                                 2           4                ##            $   ",

 	 "                                                                    2 3 4                                                                                                                      1                                                  B      B                                  B   ##   ##            $   ",

 	 "           1 2 1    #?#?#                   //               ?     1     5            2  3  1  5               3  4                                     ?###?                21                ###?            ##     ##     ##          2   3   ?#########?                               ##   ##   ##            $   ",

 	 "                                //          //         //                            /##########/                            /   /         //   /                 //       3    5       //                                          //                               //   /           ##   ##   ##   ##            $   ",

 	 "      　　       　　　         //          //         //                           //          //                          //   //       ///   //                //                    //              //                          //                              ///   //     ##   ##   ##   ##   ##            $   ",

 	 "    P                           //          //    A    //    B                     ///          ///        A   B   A       ///   ///     ////   ///     A   A     //   B            B   //        A     //  B 1  5 C 3  2 C 2  5  B //           A    A    A       //// C ///    ##   ##      A    A    A         ///  ",

 	 "====================================================================     ====================================================================   ============================    ==============================####===####===####=======================================================================================",

 	 "====================================================================     ====================================================================   ============================    ==============================    ===    ===    ======================================================================================="

 	 //0   P     1         2         3         4         5         6         7         8         9         A         B         C         D         E         F        10        11        12        13        14        15        16        17        18        19        1A        1B        1C        1D        1E        1F

 	 //上の数は10マスごとに16進数で数えられている

};



void GameScene::Init() {

 	//speed = 5;

 	scrollX = 0;

 	endFlag = false;



 	// ブロック画像の読み込み

 	blockImages\[(int)BlockType::GroundA] = LoadGraph("image/GroundA.png");

 	blockImages\[(int)BlockType::GroundB] = LoadGraph("image/Ground\_B.png");

 	blockImages\[(int)BlockType::Brick] = LoadGraph("image/Block.png");

 	blockImages\[(int)BlockType::Question] = LoadGraph("image/Question\_Block.png");





 	// マップデータから各オブジェクトリストを取得



 	// 1. マップ読み込み

 	map.LoadFromString(mapText, blockImages);



 	// 2. ブロック配置（プレイヤーより先）

 	blocks = map.blocks;    //壁のオブジェクトリスト



 	// 3. プレイヤー初期化

 	player.Init(map.playerStart);



 	items = map.items;      //アイテムのオブジェクトリスト

 	enemies = map.enemies;  //敵のオブジェクトリスト

 	itemCollected = std::vector<bool>(items.size(), false);



 	enemyImages =       //敵の描画ファイルの読み込み

 	{

 		 LoadGraph("image/Enemy1\_Walk\_L.png"),

 		 LoadGraph("image/Enemy1\_Walk\_R.png"),

 		 LoadGraph("image/Enemy2\_Walk\_L.png"),

 		 LoadGraph("image/Enemy2\_Walk\_R.png"),

 		 LoadGraph("image/Enemy3\_Normal.png"),

 		 LoadGraph("image/Enemy3\_Ready.png"),

 		 LoadGraph("image/Enemy3\_Attack.png"),

 		 LoadGraph("image/Enemy4.png")

 	};



 	itemImages = {       //アイテムがの描画ファイルの読み込み

 	   LoadGraph("image/Food1.png"),

 	   LoadGraph("image/Food2.png"),

 	   LoadGraph("image/Food3.png"),

 	   LoadGraph("image/Food4.png"),

 	   LoadGraph("image/Food5.png")

 	};



 	//ステージの描画ファイルの読み込み

 	backgroundImage = LoadGraph("image/background.png");



 	player.SetBlockImages(blockImages);



 	// 敵アニメーション画像をセット

 	std::vector<int> enemyWalkAnim = { enemyImages\[0], enemyImages\[1] };



 	for (auto\& enemy : enemies) {

 		if (enemy.type == 0) {

 			enemy.SetAnimImages({ enemyImages\[0], enemyImages\[1] });  // Enemy1

 		}

 		else if (enemy.type == 1) {

 			enemy.SetAnimImages({ enemyImages\[2], enemyImages\[3] });  // Enemy2

 		}

 		else if (enemy.type == 2)

 		{

 			enemy.SetAnimImages({ enemyImages\[4] });

 		}

 	}

}



// SceneBase から呼ばれる Update（引数なし）

void GameScene::Update() {

 	Update(deltaTimeForUpdate); // 内部で渡す

}



// deltaTime付きの本来のUpdate

void GameScene::Update(float deltaTime) {



 	// deltaTime をセット

 	deltaTimeForUpdate = deltaTime;



 	// プレイヤーの更新

 	player.Update(blocks, deltaTime);



 	// プレイヤーの矩形を取得（敵に渡す）

 	Rect playerRect = player.GetRect();



 	// 敵の更新（移動処理）

 	for (auto\& enemy : enemies) {

 		bool isOnScreen =

 			enemy.rect.x + enemy.rect.w > scrollX \&\&

 			enemy.rect.x < scrollX + SCREEN\_WIDTH \&\&

 			enemy.rect.y + enemy.rect.h > 0 \&\&

 			enemy.rect.y < SCREEN\_HEIGHT;



 		if (!isOnScreen \&\& enemy.type != 2) continue; // 画面外の敵は更新しない



 		enemy.Update(blocks, enemies, playerRect, scrollX, deltaTime);

 	}



 	// プレイヤーの頭位置の矩形

 	Rect footRect = playerRect;

 	footRect.y += 1;  // 足元をわずかに下にオフセット

 	footRect.h = 5;   // 足の高さ（ヒット判定用）



 	// プレイヤーの頭部矩形を作る

 	Rect headRect = playerRect;

 	headRect.h = 5;             // 頭の高さは小さめ

 	headRect.y -= 5;            // 頭の上に少しオフセット



 	// 敵全員更新

 	//enemy.Update(blocks, enemies, player.GetRect());



 	for (auto\& block : blocks) {

 		if (!block.HasCollision()) continue;



 		const Rect\& b = block.GetRect();



 		// 上向きジャンプ中に？ブロックと接触

 		if (headRect.Intersects(b) \&\& block.GetType() == BlockType::Question \&\& !block.IsActivated()) {

 			block.Activate();



 			// ブロックの種類を地面Bに変更

 			block.SetType(BlockType::GroundB);

 			block.SetImage(blockImages\[(int)BlockType::GroundB]);



 			// アイテムを出す

 			items.push\_back({ { b.x, b.y - b.h, b.w, b.h }, GetRandomItemType() });

 			itemCollected.push\_back(false);

 		}

 	}



 	// アイテム取得処理

 	for (size\_t i = 0; i < items.size(); ++i) {

 		if (!itemCollected\[i] \&\& CheckCollision(playerRect, items\[i].rect)) {

 			itemCollected\[i] = true;

 			player.Grow(blocks);

 		}

 	}



 	bool stomped = false;     //このフレームで1回でも踏んだらtrueにする



 	// アイテムとの当たり判定などで使用

 	for (size\_t i = 0; i < enemies.size();)

 	{

 		const Rect\& enemyRect = enemies\[i].rect;

 		int enemyType = enemies\[i].type;



 		// プレイヤー矩形の取得

 		Rect playerRect = player.GetRect();



 		// プレイヤーの底と敵の上辺の差

 		int playerBottom = playerRect.y + playerRect.h;



 		// 敵の頭（上辺）のY座標

 		int enemyTop = enemyRect.y;



 		// 上から踏んでいる判定（プレイヤーの足部分が敵の頭付近に位置）

 		bool isVerticallyAbove =

 			playerBottom > enemyTop \&\&               // プレイヤーが敵より下に入り込んでいて

 			playerBottom < enemyTop + 22;            // (12px) 以内なら踏んだと判定



 		// 横方向の重なりチェック

 		bool isHorizontallyOverlapping =

 			playerRect.x + playerRect.w > enemyRect.x \&\&

 			playerRect.x < enemyRect.x + enemyRect.w;



 		// 敵の上に乗って倒す（Enemy1 or Enemy2 or Enemy3限定）

 		if ((enemyType == 0 || enemyType == 1 || enemyType == 2) \&\& isHorizontallyOverlapping \&\& isVerticallyAbove)

 		{

 			enemies.erase(enemies.begin() + i);  // 敵消滅

 			player.Bounce();                     // プレイヤーが跳ねる

 			stomped = true;                      // 踏んだら true

 			continue;                            // erase したので i は増やさない

 		}



 		// まだ踏んでいなければ、衝突判定

 		if (!stomped \&\& CheckCollision(playerRect, enemyRect)) {

 			endFlag = true;

 			nextSceneID = 2; // ゲームオーバーに移動

 		}



 		++i;

 	}



 	// 敵の弾とプレイヤーの当たり判定チェック

 	bool hitByBullet = false;



 	for (const auto\& enemy : enemies) {

 		for (const auto\& bullet : enemy.bullets) {

 			if (bullet.GetRect().Intersects(playerRect)) {

 				hitByBullet = true;

 				break;

 			}

 		}

 		if (hitByBullet) break;

 	}



 	// 弾が当たったらプレイヤー死亡処理

 	if (hitByBullet) {

 		nextSceneID = 2;  // GameOver

 		endFlag = true;	  // プレイヤーを死亡状態にする

 		return;

 	}



 	// スクロール位置更新

 	scrollX = player.GetX() - SCREEN\_WIDTH / 2;

 	if (scrollX < 0) scrollX = 0;



 	// プレイヤー死亡時

 	if (player.IsDead()) {

 		nextSceneID = 2;  // GameOver

 		endFlag = true;

 		return;

 	}



 	// 仮ゴール判定

 	for (const Block\& block : blocks) {

 		if (block.GetType() == BlockType::Goal) {

 			if (playerRect.Intersects(block.GetRect())) {

 				// ゴールに触れたらクリアフラグ設定

 				endFlag = true;

 				nextSceneID = 3; // 3 = クリアシーン（ClearScene）

 				return;

 			}

 		}

 	}

 	//if (player.GetX() >= 8700) {

 	//	endFlag = true;

 	//	nextSceneID = 3; //クリアに移動

 	//}





 	// ESCで強制終了

 	if (CheckHitKey(KEY\_INPUT\_ESCAPE)) {

 		endFlag = true;

 		nextSceneID = 2;  // ゲームオーバーに移動

 	}

}



void GameScene::Draw() {

 	DrawString(10, 10, "←→:移動 SPACE:ジャンプ ESC:終了", GetColor(255, 255, 255));

 	DrawString(10, 50, ("壁の数: " + std::to\_string(blocks.size())).c\_str(), GetColor(255, 255, 255));



 	// 背景描画

 	if (backgroundImage >= 0) {



 		DrawBox(0, 0, SCREEN\_WIDTH, SCREEN\_HEIGHT, GetColor(50, 150, 170), TRUE);



 		int bgOffsetY = -100;  // 上に100pxオフセット



 		// 背景をスクロールに応じて描画（ループ表示例）

 		int bgWidth, bgHeight;



 		SetDrawBlendMode(DX\_BLENDMODE\_ALPHA, 128);



 		GetGraphSize(backgroundImage, \&bgWidth, \&bgHeight);



 		int bgX = -(int(scrollX) % bgWidth);

 		for (int x = bgX; x < SCREEN\_WIDTH; x += bgWidth) {

 			DrawGraph(x, bgOffsetY, backgroundImage, TRUE);

 		}

 		SetDrawBlendMode(DX\_BLENDMODE\_NOBLEND, 0);

 	}



 	// 敵描画

 	for (const auto\& enemy : enemies) {

 		bool isOnScreen =

 			enemy.rect.x + enemy.rect.w > scrollX \&\&

 			enemy.rect.x < scrollX + SCREEN\_WIDTH \&\&

 			enemy.rect.y + enemy.rect.h > 0 \&\&

 			enemy.rect.y < SCREEN\_HEIGHT;



 		if (!isOnScreen \&\& enemy.type != 2) continue;



 		enemy.Draw(scrollX, enemyImages\[enemy.type]);

 		//enemy.Update(blocks, enemies, player.GetRect(), scrollX);

 	}



 	// ブロック描画

 	for (const auto\& block : blocks) {

 		block.Draw(scrollX);

 	}



 	// アイテム描画

 	for (size\_t i = 0; i < items.size(); ++i) {

 		if (!itemCollected\[i]) {

 			const auto\& item = items\[i];

 			DrawExtendGraph(

 				item.rect.x - scrollX, item.rect.y,

 				item.rect.x + item.rect.w - scrollX, item.rect.y + item.rect.h,

 				itemImages\[item.type], TRUE);

 		}

 	}



 	// プレイヤー描画（当たり判定と一致させる）

 	player.Draw(scrollX);



}



bool GameScene::IsEnd() {

 	return endFlag;

}



int GameScene::NextScene() {

 	return nextSceneID; //nextSceneID を返す

}



void GameScene::SetDeltaTime(float dt)

{

 	deltaTimeForUpdate = dt;

}



bool GameScene::CheckCollision(const Rect\& a, const Rect\& b) {

 	return a.x < b.x + b.w \&\& a.x + a.w > b.x \&\& a.y < b.y + b.h \&\& a.y + a.h > b.y;

}





##### **//Main.cpp**

\#include "DxLib.h"

\#include "SceneManager.h"

\#include <chrono>



const int WIN\_WIDTH = 1280;

const int WIN\_HEIGHT = 720;



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    ChangeWindowMode(TRUE);         // ウィンドウモードに変更

    SetGraphMode(WIN\_WIDTH, WIN\_HEIGHT, 32);    // 幅1280px, 高さ720px, 32bitカラー

    DxLib\_Init();                   // DxLib初期化

    SetDrawScreen(DX\_SCREEN\_BACK);  // 裏画面に描画



    //GameScene scene;

    //scene.Init();



    SceneManager manager;



    using clock = std::chrono::steady\_clock;

    auto lastTime = clock::now();



    //SceneManager manager;

    //manager.ChangeScene(0);   // ゲームシーン開始



    while (ProcessMessage() == 0) {

        ClearDrawScreen();



        auto now = clock::now();

        std::chrono::duration<float> delta = now - lastTime;

        lastTime = now;

        float deltaTime = delta.count();   // 秒単位の経過時間

        if (deltaTime > 0.05f) deltaTime = 0.05f; // 最大50msまでに制限（=20FPS）



        // GameSceneのために deltaTime を保存

        //SceneBase\* scene = manager.GetCurrentScene();

        //if (auto\* game = dynamic\_cast<GameScene\*>(scene)) {

        //    game->SetDeltaTime(deltaTime);

        //}



        manager.Update(deltaTime);

        //manager.Update();

        manager.Draw();



        ScreenFlip();

    }



    DxLib\_End();                // 終了処理

    return 0;

}



##### **//MapData.cpp**

\#include "MapData.h"



void MapData::LoadFromString(const std::vector[std::string](std::string)\& mapLines, const int\* blockImages) {

    rawMap = mapLines;

    blocks.clear();

    items.clear();

    enemies.clear();



    for (int y = 0; y < mapLines.size(); ++y) {

        for (int x = 0; x < mapLines\[y].size(); ++x) {

            char ch = mapLines\[y]\[x];

            int px = x \* tileSize;

            int py = y \* tileSize;



            // 各文字に対応する処理（ブロックや敵の生成）

            switch (ch)

            {

            case '=': {  // 地面ブロックA

                Block block({ px, py, tileSize, tileSize }, BlockType::GroundA);

                block.SetImage(blockImages\[(int)BlockType::GroundA]);

                block.SetBlockImages(blockImages);

                blocks.push\_back(block);

                break;

            }



            case '/': {  // 地面ブロックB

                Block block({ px, py, tileSize, tileSize }, BlockType::GroundB);

                block.SetImage(blockImages\[(int)BlockType::GroundB]);

                block.SetBlockImages(blockImages);

                blocks.push\_back(block);

                break;

            }



            case '#': {  // レンガブロック

                Block block({ px, py, tileSize, tileSize }, BlockType::Brick);

                block.SetImage(blockImages\[(int)BlockType::Brick]);

                block.SetBlockImages(blockImages);

                blocks.push\_back(block);

                break;

            }



            case '?': {  // ？ブロック

                Block block({ px, py, tileSize, tileSize }, BlockType::Question);

                block.SetImage(blockImages\[(int)BlockType::Question]);

                block.SetBlockImages(blockImages);

                blocks.push\_back(block);

                break;

            }



            case '$': {  // ゴールブロック（仮）

                Block block({ px, py, tileSize, tileSize }, BlockType::Goal); // Goal を使う

                //block.SetImage(blockImages\[(int)BlockType::GroundB]); // ←仮の画像（GroundBで代用）

                block.SetBlockImages(blockImages);

                blocks.push\_back(block);

                break;

            }



            case 'P': {  // プレイヤーの開始位置

                playerStart = { px, py, tileSize, tileSize };



                //playerStart = { px, py - tileSize / 2, tileSize, tileSize };



                break;

            }



                    // アイテム '1' ～ '5'

            case '1': case '2': case '3': case '4': case '5': {

                int type = ch - '1'; // 例: '3'なら type = 2

                items.push\_back({ { px, py, tileSize, tileSize }, type });

                break;

            }

                    // 敵 'A' ～ 'D'

            case 'A': {   // Enemy type 0（穴に落ちる）

                // 敵画像の見た目に比べて当たり判定は小さく、下に寄せる（例：高さ30、下に10ずらす）

 

                Enemy e1({ px, py, tileSize, tileSize }, 0, -100);// 下部に合わせる

                enemies.push\_back(e1);

                break;



            }



            case 'B': {  // Enemy type 1（穴で反転）

 

                Enemy e2({ px, py, tileSize, tileSize }, 1, -100); // ← 同じ速度に統一

                enemies.push\_back(e2);

                break;

            }



            case 'C': {  // Enemy type 2（ジャンプ弾幕系など）

                Enemy e3({ px, py, tileSize, tileSize }, 2, 0, 0.0f);

                enemies.push\_back(e3);

                break;

            }



            case 'D': {  // Enemy type 4（将来用）

                Enemy e({ px, py, tileSize, tileSize }, 3, -1, 1.0f);

                enemies.push\_back(e);

                break;

            }



            default:

                // 空白やその他の文字は無視

                break;

            }

        }

    }

}



##### **//Player.cpp**

\#include "Player.h"

\#include <DxLib.h>



const float TERMINAL\_VELOCITY = 20.0f; // 終端速度



void Player::Init(const Rect\& start) {

 	x = start.x + WIDTH\_MIN / 2;

 	y = start.y + start.h - HEIGHT / 2;



 	width = WIDTH\_NOR;



 	fixedStartY = y;



 	width = WIDTH\_NOR;      // 普通体型

 	speed = SPEED\_MAX;

 	jumpPower = JUMP\_MAX;

 	velocityY = 0;

 	imageID = LoadGraph("image/Player.png");



 	walkedDistance = 0.0f;

 	isDead = false;             //プレイヤーは死亡判定

 	isJumping = false;          //ジャンプ中か

 	onGround = false;           //地面の上にいるか

 	jumpCooldownTimer = 0.0f;



 	startFreezeTimer = FREEZE\_DURATION;

}



void Player::Update(/\*const\*/ std::vector<Block>\& blocks, double deltaTime) {

 

 	// 開始直後の固定処理

 	bool duringFreeze = (startFreezeTimer > 0.0f);

 	if (duringFreeze) {

 		startFreezeTimer -= deltaTime;



 		y = fixedStartY;        // y座標を固定



 		velocityY = 0;          // 落下も止める



 		// ここでジャンプ入力を受け付けてもいい

 		if (CheckHitKey(KEY\_INPUT\_SPACE) \&\& onGround \&\& jumpCooldownTimer <= 0)

 		{

 			velocityY = jumpPower;

 			isJumping = true;

 			startFreezeTimer = 0;  // フリーズ解除してジャンプ開始

 		}



 		// フリーズ中は他の更新処理は飛ばすかも

 		//return;

 	}



 	// 速度を時間ベースに補正

 	int dx = 0;

 	bool moved = false;



 	//float deltaSpeed = speed \* deltaTime \* SPEED\_CORRECTION;  // 通常速度に補正



 	if (CheckHitKey(KEY\_INPUT\_RIGHT))

 	{

 		dx = (int)(speed \* deltaTime \* SPEED\_CORRECTION);

 		moved = true;

 		isFacingRight = true;  // 右向き

 	}



 	if (CheckHitKey(KEY\_INPUT\_LEFT)) {

 		dx = -(int)(speed \* deltaTime \* SPEED\_CORRECTION);

 		moved = true;

 		isFacingRight = false; // 左向き

 	}



 	if (moved) {                    //移動中の距離の加算

 		walkedDistance += abs(dx);

 	}



 	// 横方向の移動・当たり判定

 	int step = (dx > 0) ? 1 : -1;

 	int movedX = 0;



 	while (movedX != dx) {

 		int nextX = x + step;

 		Rect testRect = GetRect();

 		testRect.x = nextX - width / 2;



 		bool hit = false;



 		// 上方向の衝突判定（ジャンプ中にブロックにぶつかった時）

 		for (const auto\& block : blocks) {

 			if (!block.HasCollision()) continue;



 			const Rect\& wall = block.GetRect();



 			// 上下方向にある程度重なっているときだけ横衝突とみなす

 			int margin = 2; // この値で調整（小さすぎると壁をすり抜ける、大きすぎると登る）



 			bool verticalOverlap =

 				testRect.y + testRect.h > wall.y + margin \&\&

 				testRect.y < wall.y + wall.h - margin;



 			if (testRect.Intersects(wall) \&\& verticalOverlap) {

 				hit = true;

 				break;

 			}

 		}



 		if (hit) break;



 		x += step;                // x方向だけ更新

 		movedX += step;

 		//playerRect = GetRect(); // ← xが変わった後の矩形を再取得

 	}



 	// ジャンプ

 	if (CheckHitKey(KEY\_INPUT\_SPACE) \&\& onGround \&\& jumpCooldownTimer <= 0)

 		//if (CheckHitKey(KEY\_INPUT\_SPACE) \&\& !isJumping \&\& jumpCooldownTimer <= 0)

 	{

 		velocityY = jumpPower;

 		isJumping = true;

 		startFreezeTimer = 0;  // フリーズ解除してジャンプ開始

 		//jumpCooldownTimer = JUMP\_COOLDOWN\_DURATION;

 	}



 	// 重力

 	velocityY += GRAVITY \* deltaTime \* SPEED\_CORRECTION;



 	// 終端速度制限

 	if (velocityY > TERMINAL\_VELOCITY)

 		velocityY = TERMINAL\_VELOCITY;



 	// Y座標に反映

 	y += (int)(velocityY \* deltaTime \* SPEED\_CORRECTION);



 	// 着地判定

 	//bool onGround = false;

 	onGround = (startFreezeTimer > 0.0f);



 	Rect footRect = GetRect();



 	int dy = (int)(velocityY \* deltaTime \* SPEED\_CORRECTION);

 	dy = (dy > 1) ? dy : 1;

 	footRect.y += dy;



 	//footRect.y += (int)(velocityY \* deltaTime \* SPEED\_CORRECTION);

 	//footRect.y += 1;  // 足元1ピクセル下をチェック

 	//footRect.h += 4;  // ← これ追加して判定強化（例：+4ピクセル）



 	for (int i = (int)blocks.size() - 1; i >= 0; --i) {

 		Block\& block = blocks\[i];

 		if (!block.HasCollision()) continue;



 		const Rect\& rect = block.GetRect();



 		//横に少しでもかぶっているかも確認（特に空中で）

 		bool horizontalOverlap =

 			footRect.x + footRect.w > rect.x + COLLISION\_MARGIN \&\&

 			footRect.x < rect.x + rect.w - COLLISION\_MARGIN;



 		// プレイヤーがブロックに接地しているか判定

 		if (velocityY >= 0 \&\& footRect.Intersects(rect) \&\& horizontalOverlap) {



 			y = rect.y - HEIGHT / 2;  // めり込まないよう補正



 			velocityY = 0;



 			onGround = true;



 			if (isJumping)

 			{

 				isJumping = false;

 				jumpCooldownTimer = JUMP\_COOLDOWN\_DURATION;  // クールタイム開始

 			}



 			// 肥満状態でレンガの上なら削除

 			FatState fat = GetFatState();

 			if ((fat == FatState::Fat1 || fat == FatState::Fat2 || fat == FatState::Fat3 || fat == FatState::Fat4) \&\&

 				block.GetType() == BlockType::Brick)

 			{

 				blocks.erase(blocks.begin() + i);

 				continue;

 			}



 			break;

 		}

 	}



 	// 天井にぶつかる判定

 	Rect headRect = GetRect();

 	for (const auto\& block : blocks) {

 		if (!block.HasCollision()) continue;

 		const Rect\& rect = block.GetRect();



 		bool horizontalOverlap =

 			headRect.x + headRect.w > rect.x + COLLISION\_MARGIN \&\&

 			headRect.x < rect.x + rect.w - COLLISION\_MARGIN;



 		if (velocityY < 0 \&\& headRect.Intersects(rect) \&\& horizontalOverlap) {

 			y = rect.y + rect.h + HEIGHT / 2;

 			velocityY = 0;

 			break;

 		}

 	}



 	// ジャンプクールタイムのカウント

 	if (jumpCooldownTimer > 0.0f)

 	{

 		jumpCooldownTimer -= deltaTime;

 	}



 	// 落下死判定

 	if (startFreezeTimer <= 0.0f \&\& y > FALL\_LIMIT\_Y \&\& !onGround) {

 		isDead = true;

 	}



 	ShrinkByDistance(blocks);



}



void Player::Draw(int scrollX) const {



 	int left = x - width / 2 - scrollX;

 	int right = x + width / 2 - scrollX;



 	if (!isFacingRight) std::swap(left, right);  // 左向きなら左右を入れ替えて描画



 	//DrawExtendGraph(x - width / 2 - scrollX, y - HEIGHT / 2, x + width / 2 - scrollX, y + HEIGHT / 2, imageID, TRUE);

 	DrawExtendGraph(

 		left,

 		y - HEIGHT / 2,

 		right,

 		y + HEIGHT / 2,

 		imageID,

 		TRUE

 	);



 	// 当たり判定を赤枠で描画

 	Rect rect = GetRect();

 	DrawBox(

 		rect.x - scrollX,

 		rect.y,

 		rect.x + rect.w - scrollX,

 		rect.y + rect.h,

 		GetColor(255, 0, 0),

 		FALSE

 	);



 	// 体型表示

 	int fatRatio = GetFatLevel();

 	const char\* status = "";

 	switch (GetFatState()) {

 	case FatState::Thin:          status = "瘦せ"; break;

 	case FatState::SlightlyThin:  status = "やや瘦せ"; break;

 	case FatState::Normal:        status = "普通"; break;

 	case FatState::SlightlyFat:   status = "やや肥満"; break;

 	case FatState::Fat1:          status = "肥満(1度)"; break;

 	case FatState::Fat2:          status = "肥満(2度)"; break;

 	case FatState::Fat3:          status = "肥満(3度)"; break;

 	case FatState::Fat4:          status = "肥満(4度)"; break;

 	}





 	DrawString(10, 100, status, GetColor(255, 255, 255));



 	// 残り距離表示

 	int remaining = (int)(DISTANCE\_TO\_SHRINK - walkedDistance);

 	if (remaining < 0) remaining = 0;

 	DrawFormatString(10, 130, GetColor(255, 255, 255), "あと %d px で瘦せます", remaining);

}



//プレイヤーの描画

Rect Player::GetRect() const {

 	return { x - width / 2, y - HEIGHT / 2, width, HEIGHT };

}



//食べたときの処理(大雑把に分けて8段階太る)

void Player::Grow(std::vector<Block>\& blocks) {

 	FatState state = GetFatState();



 	if (width >= WIDTH\_MAX) {

 		isDead = true;  // 太りすぎで死亡

 		return;

 	}



 	// 瘦せ または やや瘦せ のときは +15

 	if (state == FatState::Thin || state == FatState::SlightlyThin) {

 		if (width + 15 <= WIDTH\_NOR) {

 			width += 15;

 			if (jumpPower < JUMP\_MIN) jumpPower += FAT\_ADJUST;

 		}

 		else {

 			width = WIDTH\_NOR; // 普通の最大に調整

 		}

 	}

 	else {

 		// 普通以上は従来通り +40

 		if (width < WIDTH\_MAX) width += 40;

 		if (speed > SPEED\_MIN) speed -= 0.75f;//(0.5)

 		if (jumpPower < JUMP\_MIN) jumpPower += FAT\_ADJUST;

 	}



 	walkedDistance = 0.0f;         // カウントリセット



 	ResolveStuckAfterResize(blocks);  // ここでプレイヤーの位置を調整



 	// 体型変化後にブロックに埋まっていないか確認

 	FixStuckInBlock(blocks);

}



//動いているとき

void Player::ShrinkByDistance(std::vector<Block>\& blocks) {



 	if (walkedDistance < DISTANCE\_TO\_SHRINK)return;



 	FatState state = GetFatState();



 	//「瘦せ状態」で歩ききったら死亡

 	if (state == FatState::Thin) {

 		isDead = true;

 		return;

 	}





 	// 普通より太っている → 大きく減少

 	if (walkedDistance >= DISTANCE\_TO\_SHRINK \&\& width > WIDTH\_NOR) {



 		if (width > WIDTH\_NOR) {

 			if (width > WIDTH\_MIN) width -= 40;

 			if (speed < SPEED\_MAX) speed += 0.75f;//(0.5)

 			if (jumpPower > JUMP\_MAX) jumpPower -= FAT\_ADJUST;

 		}

 	}

 	// 普通から痩せる → 少し減らす

 	else if (walkedDistance >= DISTANCE\_TO\_SHRINK \&\& width == WIDTH\_NOR)

 	{

 		if (width > WIDTH\_MIN) width -= 15;

 		if (jumpPower > JUMP\_MAX) jumpPower -= FAT\_ADJUST;

 	}

 	// やや瘦せ〜瘦せ → 少しずつ減少

 	else if (walkedDistance >= DISTANCE\_TO\_SHRINK \&\& width > WIDTH\_MIN)

 	{

 		if (width > WIDTH\_MIN) width -= 15;

 		if (jumpPower > JUMP\_MAX) jumpPower -= FAT\_ADJUST;

 	}



 	walkedDistance = 0.0f; // カウントリセット



 	ResolveStuckAfterResize(blocks);  // ここでプレイヤーの位置を調整



 	// 体型変化後にブロックに埋まっていないか確認

 	FixStuckInBlock(blocks);

}



void Player::Bounce() {

 	velocityY = JUMP\_MAX / 2;  // 通常ジャンプの半分くらいの反発力

 	isJumping = true;

 	jumpCooldownTimer = JUMP\_COOLDOWN\_DURATION;

}



int Player::GetFatLevel() const {

 	if (width <= WIDTH\_MIN)       return 0; // 瘦せ

 	if (width <= WIDTH\_NOR - 15)  return 1; // やや瘦せ

 	if (width == WIDTH\_NOR)       return 2; // 普通

 	if (width <= WIDTH\_NOR + 40)  return 3; // やや肥満

 	if (width <= WIDTH\_NOR + 80)  return 4; // 肥満(1度)

 	if (width <= WIDTH\_NOR + 200) return 5; // 肥満(2度)

 	if (width <= WIDTH\_NOR + 280) return 6; // 肥満(3度)

 	if (width <= WIDTH\_MAX)       return 7; // 肥満(4度以上)

}



FatState Player::GetFatState() const {

 	if (width <= WIDTH\_MIN)       return FatState::Thin;

 	if (width <= WIDTH\_NOR - 15)  return FatState::SlightlyThin;

 	if (width == WIDTH\_NOR)       return FatState::Normal;

 	if (width <= WIDTH\_NOR + 40)  return FatState::SlightlyFat;

 	if (width <= WIDTH\_NOR + 80)  return FatState::Fat1;

 	if (width <= WIDTH\_NOR + 120) return FatState::Fat2;

 	if (width <= WIDTH\_NOR + 280) return FatState::Fat3;

 	if (width <= WIDTH\_MAX)       return FatState::Fat4;

}



void Player::SetBlockImages(const int\* images)

{

 	blockImages = images;

}



int Player::GetX() const { return x; }

int Player::GetY() const { return y; }

bool Player::IsDead() const { return isDead; }



bool Player::IsJumping() const

{

 	return isJumping;  // 地面に接していない間はジャンプ中とみなす

}



// サイズ変更後に壁や床に埋まっていたら少しずつ補正

void Player::ResolveStuckAfterResize(const std::vector<Block>\& blocks)

{

 	Rect playerRect = GetRect();



 	for (const auto\& block : blocks) {

 		if (!block.HasCollision()) continue;



 		const Rect\& blockRect = block.GetRect();

 		if (playerRect.Intersects(blockRect)) {

 			// 右側がめり込んでいる

 			if (playerRect.x < blockRect.x \&\& playerRect.x + playerRect.w > blockRect.x) {

 				x -= 1;  // 左にずらす

 			}

 			// 左側がめり込んでいる

 			else if (playerRect.x >= blockRect.x \&\& blockRect.x + blockRect.w > playerRect.x) {

 				x += 1;  // 右にずらす

 			}



 			// 確実に更新された座標で再チェック

 			playerRect = GetRect();

 		}

 	}

}



//追加

void Player::FixStuckInBlock(const std::vector<Block>\& blocks)

{

 	Rect playerRect = GetRect();



 	// 最大32ドット上にずらしてみる

 	const int maxOffset = 32;

 	for (int offset = 1; offset <= maxOffset; ++offset)

 	{

 		bool hit = false;

 		Rect testRect = playerRect;

 		testRect.y -= offset;



 		for (const auto\& block : blocks)

 		{

 			if (!block.HasCollision()) continue;

 			if (testRect.Intersects(block.GetRect()))

 			{

 				hit = true;

 				break;

 			}

 		}



 		if (!hit)

 		{

 			y -= offset;  // 衝突しない位置まで上に移動

 			return;

 		}

 	}

}





##### **//SceneManager.cpp**

\#include "SceneManager.h"

\#include "TitleScene.h"

\#include "GameScene.h"

\#include "GameOverScene.h"

\#include "ClearScene.h"



SceneManager::SceneManager() {

 	currentScene = nullptr;

 	sceneID = -1;

 	ChangeScene(0);  // 最初はタイトル画面



 	//printfDx("SceneManager 初期化完了\\n");



}



SceneManager::~SceneManager() {

 	delete currentScene;

}



void SceneManager::ChangeScene(int id) {

 	delete currentScene;

 	sceneID = id;



 	switch (id) {

 	case 0:

 		currentScene = new TitleScene();

 		break;



 	case 1:

 		currentScene = new GameScene();

 		break;



 	case 2:

 		currentScene = new GameOverScene();

 		break;



 	case 3:

 		currentScene = new ClearScene();

 		break;



 	default:

 		currentScene = new TitleScene();

 		break;

 	}



 	currentScene->Init();

}



void SceneManager::Update()

{

}



void SceneManager::Update(float deltaTime) {

 	if (!currentScene) return;



 	// deltaTimeを受け取れるシーンには渡す

 	if (auto\* game = dynamic\_cast<GameScene\*>(currentScene)) {

 		game->SetDeltaTime(deltaTime);

 	}



 	// GameScene のような deltaTime を使うシーンに渡す

 	currentScene->Update();  // 仮にSceneBaseがfloatを受け取らない場合はこのまま



 	// シーン終了時、次のシーンに遷移

 	if (currentScene->IsEnd()) {

 		ChangeScene(currentScene->NextScene());

 	}

}



void SceneManager::Draw() {

 	if (!currentScene) return;

 	currentScene->Draw();

}



SceneBase\* SceneManager::GetCurrentScene() {

 	return currentScene;

}



##### **//TitleScene.cpp**

\#include "TitleScene.h"

\#include "DxLib.h"



void TitleScene::Init() {

 	endFlag = false;



 	backgroundImage = LoadGraph("image/background.png"); // ← 画像のパスに合わせてください

 	TitleImage = LoadGraph("image/Title.png");



 	// キー入力をクリアして、前シーンのキー入力を無効化

 	char keys\[256];

 	GetHitKeyStateAll(keys);  // 全キーの状態を取得して無視することでリセット

}



void TitleScene::Update(/\*float deltaTime\*/) {

 

 	if (CheckHitKey(KEY\_INPUT\_RETURN)) {

 		endFlag = true;

 		//nextSceneID = 1;	// GameScene に遷移

 	}

}



void TitleScene::Draw(){

 	// 背景塗りつぶし（背景画像が無いときの安全策）

 	DrawBox(0, 0, 1280, 720, GetColor(50, 150, 170), TRUE);



 	// 背景画像が読み込まれていたら表示

 	if (backgroundImage >= 0) {

 		SetDrawBlendMode(DX\_BLENDMODE\_ALPHA, 128);



 		int bgWidth, bgHeight;

 		GetGraphSize(backgroundImage, \&bgWidth, \&bgHeight);



 		for (int x = 0; x < 1280; x += bgWidth) {

 			DrawGraph(x, -100, backgroundImage, TRUE); // Yは少し上にずらしてます

 		}



 		SetDrawBlendMode(DX\_BLENDMODE\_NOBLEND, 0);

 	}



 	// 文字列と中央位置

 	const char\* text = "Press SPACE to Start";

 	int textWidth = GetDrawStringWidth(text, strlen(text));

 	int drawX = (1280 - textWidth) / 2;

 	int drawY = 400;



 	// 画像が読み込まれていたら表示

 	if (TitleImage >= 0) {



 		//int bgWidth, bgHeight;

 		//GetGraphSize(backgroundImage, \&bgWidth, \&bgHeight);



 		//for (int x = 0; x < 1280; x += bgWidth) {

 			DrawGraph(-50, -250, TitleImage, TRUE); // Yは少し上にずらしてます

 		//}



 	}



 	// タイトル文字など描画

 	DrawString(drawX , drawY + 100, "Press Enter to Start", GetColor(255, 255, 255));

 	//DrawString(500, 300, "タイトル画面", GetColor(255, 255, 255));

 	//DrawString(500, 340, "Enterキーでスタート", GetColor(255, 255, 0));

}



bool TitleScene::IsEnd() {

 	return endFlag;

}



int TitleScene::NextScene() {

 	return 1; // GameSceneのID

}

