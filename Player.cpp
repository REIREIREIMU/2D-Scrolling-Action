#include "Player.h"
#include <DxLib.h>
#include "Input.h"

const float TERMINAL_VELOCITY = 20.0f; // 終端速度

void Player::Init(const Rect& start) {
	x = start.x + WIDTH_MIN / 2;
	y = start.y + start.h - HEIGHT / 2;

	width = WIDTH_NOR;

	fixedStartY = y;

	width = WIDTH_NOR;      // 普通体型
	speed = SPEED_MAX;
	jumpPower = JUMP_MAX;
	velocityY = 0;
	//imageID = LoadGraph("image/Player.png");

	walkedDistance = 0.0f;
	isDead = false;             //プレイヤーは死亡判定
	isJumping = false;          //ジャンプ中か
	onGround = false;           //地面の上にいるか
	jumpCooldownTimer = 0.0f;

	startFreezeTimer = FREEZE_DURATION;

	// 画像ロード
	LoadImages();

	UI_Bar_Case = LoadGraph("image/Bar_Case.png");		//バーの枠の画像

	// 満腹度バー画像ロード
	UI_Bar_Green = LoadGraph("image/Green_Bar.png");
	UI_Bar_Yellow = LoadGraph("image/Yellow_Bar.png");
	UI_Bar_Red = LoadGraph("image/Red_Bar.png");

	//体型表示
	UI_Thin = LoadGraph("image/Thin.png");					//Thinの画像
	UI_SlightlyThin = LoadGraph("image/SlightlyThin.png");	//SlightlyThin画像
	UI_Normal = LoadGraph("image/Normal.png");				//Normalの画像
	UI_SlightlyFat = LoadGraph("image/SlightlyFat.png");	//SlightlyFatの画像
	UI_Fat1 = LoadGraph("image/Fat1.png");					//Fat1の画像
	UI_Fat2 = LoadGraph("image/Fat2.png");					//Fat2の画像
	UI_Fat3 = LoadGraph("image/Fat3.png");					//Fat3の画像
	UI_Fat4 = LoadGraph("image/Fat4.png");					//Fat4の画像

	//警告用
	UI_WARNING_BD = LoadGraph("image/WARNING.png");
	UI_WARNING = LoadGraph("image/Danger.png");

	//ジャンプの効果音
	Jump_Sound = LoadSoundMem("sound/Jumping.mp3");

	//警告音
	Warning_Sound = LoadSoundMem("sound/Warning.mp3");

	soundvolume_ = 128 * 4;
}


void Player::Update(/*const*/ std::vector<Block>& blocks, double deltaTime) {

	if (isDead) {
		// 死亡中は操作・移動無効
		deathTimer += (float)deltaTime;
		return; // 何もせず終了
	}

	// 暗転中などで操作禁止の場合
	if (!canControl) {
		// 入力処理を無効化
		velocityY = 0;
		return;
	}

	// 入力判定
	int moveX = Input::GetAxisX();  // -1:左, 1:右
	bool jumpPressed = Input::IsJump();
	bool decidePressed = Input::IsDecide();

	// アニメーション状態切り替え
	if (!onGround) {
		animState = AnimState::Jump;
	}
	else if (moveX != 0) {
		animState = AnimState::Move;
	}
	else {
		animState = AnimState::Idle;
	}

	// 移動中ならアニメーション進行
	if (animState == AnimState::Move) {
		animTimer += (float)deltaTime;
		if (animTimer > animSpeed) {
			currentFrame = (currentFrame + 1) % 3;
			animTimer = 0.0f;
		}
	}
	else {
		currentFrame = 0;
		animTimer = 0.0f;
	}

	// 開始直後の固定処理
	bool duringFreeze = (startFreezeTimer > 0.0f);
	if (duringFreeze) {
		startFreezeTimer -= deltaTime;

		y = fixedStartY;        // y座標を固定

		velocityY = 0;          // 落下も止める

		// ここでジャンプ入力を受け付けてもいい
		//if (CheckHitKey(KEY_INPUT_SPACE) && onGround && jumpCooldownTimer <= 0)
		if (jumpPressed && /*onGround &&*/ jumpCooldownTimer <= 0)
		{
			velocityY = jumpPower;
			isJumping = true;
			startFreezeTimer = 0;  // フリーズ解除してジャンプ開始
			onGround = false;

			//ジャンプ音を再生
			PlaySoundMem(Jump_Sound, DX_PLAYTYPE_BACK);
			ChangeVolumeSoundMem(soundvolume_, Jump_Sound);
		}

		// フリーズ中は他の更新処理は飛ばすかも
		//return;
	}



	// 横移動処理 
	int dx = (int)(moveX * speed * deltaTime * SPEED_CORRECTION);
	if (dx != 0) {
		walkedDistance += abs(dx);
		isFacingRight = (dx > 0);
	}

	int step = (dx > 0) ? 1 : -1;
	int movedX = 0;
	while (movedX != dx) {
		int nextX = x + step;
		Rect testRect = GetRect();
		testRect.x = nextX - width / 2;

		bool hit = false;
		for (const auto& block : blocks) {
			if (!block.HasCollision()) continue;
			const Rect& wall = block.GetRect();

			int margin = 2;
			bool verticalOverlap =
				testRect.y + testRect.h > wall.y + margin &&
				testRect.y < wall.y + wall.h - margin;

			if (testRect.Intersects(wall) && verticalOverlap) {
				hit = true;
				break;
			}
		}
		if (hit) break;

		x += step;
		movedX += step;
	}

	// 速度を時間ベースに補正
	//int dx = 0;
	//bool moved = false;

	////float deltaSpeed = speed * deltaTime * SPEED_CORRECTION;  // 通常速度に補正

	//if (CheckHitKey(KEY_INPUT_RIGHT))
	//{
	//	dx = (int)(speed * deltaTime * SPEED_CORRECTION);
	//	moved = true;
	//	isFacingRight = true;  // 右向き
	//}

	//if (CheckHitKey(KEY_INPUT_LEFT)) {
	//	dx = -(int)(speed * deltaTime * SPEED_CORRECTION);
	//	moved = true;
	//	isFacingRight = false; // 左向き
	//}

	//if (moved) {                    //移動中の距離の加算
	//	walkedDistance += abs(dx);
	//}

	//// 横方向の移動・当たり判定
	//int step = (dx > 0) ? 1 : -1;
	//int movedX = 0;

	//while (movedX != dx) {
	//	int nextX = x + step;
	//	Rect testRect = GetRect();
	//	testRect.x = nextX - width / 2;

	//	bool hit = false;

	//	// 上方向の衝突判定（ジャンプ中にブロックにぶつかった時）
	//	for (const auto& block : blocks) {
	//		if (!block.HasCollision()) continue;

	//		const Rect& wall = block.GetRect();

	//		// 上下方向にある程度重なっているときだけ横衝突とみなす
	//		int margin = 2; // この値で調整（小さすぎると壁をすり抜ける、大きすぎると登る）

	//		bool verticalOverlap =
	//			testRect.y + testRect.h > wall.y + margin &&
	//			testRect.y < wall.y + wall.h - margin;

	//		if (testRect.Intersects(wall) && verticalOverlap) {
	//			hit = true;
	//			break;
	//		}
	//	}

	//	if (hit) break;

	//	x += step;                // x方向だけ更新
	//	movedX += step;
	//	//playerRect = GetRect(); // ← xが変わった後の矩形を再取得
	//}

	// ジャンプ
	//if (CheckHitKey(KEY_INPUT_SPACE) && onGround && jumpCooldownTimer <= 0)
	if (jumpPressed && onGround && jumpCooldownTimer <= 0)
	{
		velocityY = jumpPower;
		isJumping = true;
		startFreezeTimer = 0;  // フリーズ解除してジャンプ開始
		//jumpCooldownTimer = JUMP_COOLDOWN_DURATION;

		//ジャンプ音を再生
		PlaySoundMem(Jump_Sound, DX_PLAYTYPE_BACK);

	}

	// 重力
	velocityY += GRAVITY * deltaTime * SPEED_CORRECTION;

	// 終端速度制限
	if (velocityY > TERMINAL_VELOCITY)
		velocityY = TERMINAL_VELOCITY;

	// Y座標に反映
	y += (int)(velocityY * deltaTime * SPEED_CORRECTION);

	// 着地判定
	//bool onGround = false;
	onGround = (startFreezeTimer > 0.0f);

	Rect footRect = GetRect();

	int dy = (int)(velocityY * deltaTime * SPEED_CORRECTION);
	dy = (dy > 1) ? dy : 1;
	footRect.y += dy;

	//footRect.y += (int)(velocityY * deltaTime * SPEED_CORRECTION);
	//footRect.y += 1;  // 足元1ピクセル下をチェック
	//footRect.h += 4;  // ← これ追加して判定強化（例：+4ピクセル）

	for (int i = (int)blocks.size() - 1; i >= 0; --i) {
		Block& block = blocks[i];
		if (!block.HasCollision()) continue;

		const Rect& rect = block.GetRect();

		//横に少しでもかぶっているかも確認（特に空中で）
		bool horizontalOverlap =
			footRect.x + footRect.w > rect.x + COLLISION_MARGIN &&
			footRect.x < rect.x + rect.w - COLLISION_MARGIN;

		// プレイヤーがブロックに接地しているか判定
		if (velocityY >= 0 && footRect.Intersects(rect) && horizontalOverlap) {

			y = rect.y - HEIGHT / 2;  // めり込まないよう補正

			velocityY = 0;

			onGround = true;

			if (isJumping)
			{
				isJumping = false;
				jumpCooldownTimer = JUMP_COOLDOWN_DURATION;  // クールタイム開始
			}

			// 肥満状態でレンガの上なら削除
			/*FatState fat = GetFatState();
			if ((fat == FatState::Fat1 || fat == FatState::Fat2 || fat == FatState::Fat3 || fat == FatState::Fat4) &&
				block.GetType() == BlockType::Brick)
			{

				blocks.erase(blocks.begin() + i);
				continue;
			}*/

			break;
		}
	}

	// 天井にぶつかる判定
	Rect headRect = GetRect();
	for (const auto& block : blocks) {
		if (!block.HasCollision()) continue;
		const Rect& rect = block.GetRect();

		bool horizontalOverlap =
			headRect.x + headRect.w > rect.x + COLLISION_MARGIN &&
			headRect.x < rect.x + rect.w - COLLISION_MARGIN;

		if (velocityY < 0 && headRect.Intersects(rect) && horizontalOverlap) {
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
	if (startFreezeTimer <= 0.0f && y > FALL_LIMIT_Y && !onGround) {
		isDead = true;
	}

	ShrinkByDistance(blocks);


	// 満腹度バー減少処理
	switch (GetFatState()) {
	case FatState::Thin:
		shrinkDistance = 1500.0f;
		break;
	case FatState::SlightlyThin:
	case FatState::Normal:
		shrinkDistance = 1000.0f;
		break;
	case FatState::SlightlyFat:
	case FatState::Fat1:
	case FatState::Fat2:
	case FatState::Fat3:
	case FatState::Fat4:
		shrinkDistance = 500.0f;
		break;
	}

	// 移動距離に応じて満腹度減少
	//if (walkedDistance >= shrinkDistance) {
	//	satiety -= 0.1f;   // 1割減少（必要に応じて調整）
	//	if (satiety < 0.0f) satiety = 0.0f;
	//	walkedDistance = 0.0f;
	//}
	int remaining = (int)(shrinkDistance - walkedDistance);
	if (remaining < 0) remaining = 0;


	//瘦せ状態の警告点滅処理
	if (GetFatState() == FatState::Thin) {
		thinWarningTimer += (float)deltaTime;

		// 点滅切り替え
		if (fmod(thinWarningTimer, THIN_WARNING_BLINK_INTERVAL * 2) < THIN_WARNING_BLINK_INTERVAL) {
			thinWarningVisible = true;
		}
		else {
			thinWarningVisible = false;
		}

		// 警告表示の有効期間を超えたら非表示にする
		if (thinWarningTimer > THIN_WARNING_DURATION) {
			thinWarningVisible = false;
		}
	}
	else {
		// 瘦せ以外ならリセット
		thinWarningTimer = 0.0f;
		thinWarningVisible = true;
	}

}

void Player::Draw(int scrollX) const {

	if (isDead) {
		// 死亡時は描画しない
		return;
	}

	//int drawX = x - scrollX;
	//int drawY = y;

	//int drawX = x - width / 2 - scrollX;
	//int drawY = x + width / 2 - scrollX;

	int handle = -1;
	switch (animState) {
	case AnimState::Idle:
		handle = idleImage;
		break;
	case AnimState::Move:
		handle = moveImages[currentFrame];
		break;
	case AnimState::Jump:
		handle = jumpImage;
		break;
	}

	// プレイヤーの当たり判定矩形を取得
	Rect rect = GetRect();

	// プレイヤーの幅・高さに合わせて拡縮描画
	int left = rect.x - scrollX;
	int top = rect.y;
	int right = rect.x + rect.w - scrollX;
	int bottom = rect.y + rect.h;

	// 左右反転
	if (isFacingRight) {
		DrawExtendGraph(left, top, right, bottom, handle, TRUE); // 通常
	}
	else {
		// 左右反転
		DrawExtendGraph(right, top, left, bottom, handle, TRUE);
	}

	//int left = x - width / 2 - scrollX;
	//int right = x + width / 2 - scrollX;

	//if (!isFacingRight) std::swap(left, right);  // 左向きなら左右を入れ替えて描画

	////DrawExtendGraph(x - width / 2 - scrollX, y - HEIGHT / 2, x + width / 2 - scrollX, y + HEIGHT / 2, imageID, TRUE);
	//// 通常描画処理
	//DrawExtendGraph(
	//	left,
	//	y - HEIGHT / 2,
	//	right,
	//	y + HEIGHT / 2,
	//	imageID,
	//	TRUE
	//);

	// 当たり判定を赤枠で描画
	//Rect rect = GetRect();
	//DrawBox(
	//	rect.x - scrollX,
	//	rect.y,
	//	rect.x + rect.w - scrollX,
	//	rect.y + rect.h,
	//	GetColor(255, 0, 0),
	//	FALSE
	//);

	// 体型表示
	/*int fatRatio = GetFatLevel();
	const char* status = "";
	switch (GetFatState()) {
	case FatState::Thin:          status = "瘦せ"; break;
	case FatState::SlightlyThin:  status = "やや瘦せ"; break;
	case FatState::Normal:        status = "普通"; break;
	case FatState::SlightlyFat:   status = "やや肥満"; break;
	case FatState::Fat1:          status = "肥満(1度)"; break;
	case FatState::Fat2:          status = "肥満(2度)"; break;
	case FatState::Fat3:          status = "肥満(3度)"; break;
	case FatState::Fat4:          status = "肥満(4度)"; break;
	}*/

	// 残り距離表示
	int uiHandle = -1;

	//警告用
	int ui_Warning_bg = -1;
	int ui_Warning = -1;

	//int shrinkDistance = 500;
	switch (GetFatState()) {
	case FatState::Thin:         // shrinkDistance = 1000; break;

		//警告用
		ui_Warning_bg = UI_WARNING_BD;
		ui_Warning = UI_WARNING;

		uiHandle = UI_Thin; break;

	case FatState::SlightlyThin:
		uiHandle = UI_SlightlyThin; break;
	case FatState::Normal:       // shrinkDistance = 750; break;
		uiHandle = UI_Normal; break;
	case FatState::SlightlyFat:
		uiHandle = UI_SlightlyFat; break;
	case FatState::Fat1:
		uiHandle = UI_Fat1; break;
	case FatState::Fat2:
		uiHandle = UI_Fat2; break;
	case FatState::Fat3:
		uiHandle = UI_Fat3; break;
	case FatState::Fat4:         // shrinkDistance = 500; break;
		uiHandle = UI_Fat4; break;
	}

	int remaining = (int)(shrinkDistance - walkedDistance);
	//if (remaining < 0) remaining = 0;

	if (uiHandle >= 0) {
		// 瘦せ状態のときは点滅させる
		if (GetFatState() == FatState::Thin &&
			ui_Warning_bg >= 0 && ui_Warning >= 0)
		{
			if (thinWarningVisible) {

				//警告音を再生
				PlaySoundMem(Warning_Sound, DX_PLAYTYPE_BACK);
				ChangeVolumeSoundMem(soundvolume_ / 2, Warning_Sound);

				// 通常は常時表示
				DrawGraph(0, 0, uiHandle, TRUE);

				DrawGraph(0, 0, ui_Warning_bg, TRUE);  //画面中央
				DrawGraph(0, 0, ui_Warning, TRUE);
			}
			else
			{
				// 通常は常時表示
				DrawGraph(0, 0, uiHandle, TRUE);
			}
		}
		else {
			StopSoundMem(Warning_Sound);

			// 通常は常時表示
			DrawGraph(0, 0, uiHandle, TRUE);
		}

	}

	//DrawString(10, 100, status, GetColor(255, 255, 255));

	// 残り距離表示
	//int remaining = (int)(DISTANCE_TO_SHRINK - walkedDistance);
	//if (remaining < 0) remaining = 0;
	//DrawFormatString(10, 130, GetColor(255, 255, 255), "あと %d px で瘦せます", remaining);

	// 満腹度バー描画
	int barX = 0;
	int barY = 0;
	int barWidth = 278;   // 枠画像に合わせた横幅
	int barHeight = 720;  // 枠画像に合わせた縦幅

	// remaining を割合に変換（例: 最大100なら 0.0～1.0 に正規化）
	//float ratio = (float)remaining / DISTANCE_TO_SHRINK;
	float ratio = (float)remaining / (float)shrinkDistance;
	if (ratio < 0.0f) ratio = 0.0f;
	if (ratio > 1.0f) ratio = 1.0f;

	// 色判定（緑 → 黄 → 赤）
	int barHandle = UI_Bar_Green;
	if (ratio <= 0.33f) {
		barHandle = UI_Bar_Red;
	}
	else if (ratio <= 0.5f) {
		barHandle = UI_Bar_Yellow;
	}

	// 中身の「ソース画像」のサイズを取得
	int srcW = 0, srcH = 0;
	if (barHandle >= 0) {
		GetGraphSize(barHandle, &srcW, &srcH);
	}

	// 描画する幅（左から ratio 倍だけ切り出す）
	int srcFilledW = (int)(srcW * ratio);
	int destFilledW = (int)(barWidth * ratio);

	int destLeft = barX;      // 左固定
	int destTop = barY;

	// 枠を描画
	if (UI_Bar_Case >= 0) {
		DrawGraph(barX, barY, UI_Bar_Case, TRUE);
	}

	// 中身を左から描画
	if (barHandle >= 0 && srcFilledW > 0) {
		DrawRectGraph(destLeft, destTop, 0, 0, srcFilledW, srcH, barHandle, TRUE);
	}

	//// 残り距離表示
	//int remaining = (int)(DISTANCE_TO_SHRINK - walkedDistance);
	//if (remaining < 0) remaining = 0;
	//DrawFormatString(10, 130, GetColor(255, 255, 255), "あと %d px で瘦せます", remaining);
}

//プレイヤーの描画
Rect Player::GetRect() const {
	return { x - width / 2, y - HEIGHT / 2, width, HEIGHT };
}

//食べたときの処理(大雑把に分けて8段階太る)
void Player::Grow(std::vector<Block>& blocks) {
	FatState state = GetFatState();

	if (width >= WIDTH_MAX) {
		isDead = true;  // 太りすぎで死亡
		return;
	}

	// 瘦せ または やや瘦せ のときは +15
	if (state == FatState::Thin || state == FatState::SlightlyThin) {
		if (width + 15 <= WIDTH_NOR) {
			width += 15;
			if (jumpPower < JUMP_MIN) jumpPower += FAT_ADJUST;
		}
		else {
			width = WIDTH_NOR; // 普通の最大に調整
		}
	}
	else {
		// 普通以上は従来通り +40
		if (width < WIDTH_MAX) width += 40;
		if (speed > SPEED_MIN) speed -= 0.65f;//(0.5)
		if (jumpPower < JUMP_MIN) jumpPower += FAT_ADJUST;
	}

	walkedDistance = 0.0f;         // カウントリセット

	ResolveStuckAfterResize(blocks);  // ここでプレイヤーの位置を調整

	// 体型変化後にブロックに埋まっていないか確認
	FixStuckInBlock(blocks);
}

void Player::LoadImages() {
	idleImage = LoadGraph("image/player_idle.png");
	jumpImage = LoadGraph("image/player_jump.png");
	moveImages[0] = LoadGraph("image/player_move1.png");
	moveImages[1] = LoadGraph("image/player_move2.png");
	moveImages[2] = LoadGraph("image/player_move3.png");
}

//動いているとき
void Player::ShrinkByDistance(std::vector<Block>& blocks) {

	if (walkedDistance < shrinkDistance) return;
	//if (walkedDistance < DISTANCE_TO_SHRINK)return;

	FatState state = GetFatState();

	//「瘦せ状態」で歩ききったら死亡
	if (state == FatState::Thin) {
		isDead = true;
		return;
	}


	// 普通より太っている → 大きく減少
	if (walkedDistance >= DISTANCE_TO_SHRINK && width > WIDTH_NOR) {

		if (width > WIDTH_NOR) {
			if (width > WIDTH_MIN) width -= 40;
			if (speed < SPEED_MAX) speed += 0.65f;//(0.5)
			if (jumpPower > JUMP_MAX) jumpPower -= FAT_ADJUST;
		}
	}
	// 普通から痩せる → 少し減らす
	else if (walkedDistance >= DISTANCE_TO_SHRINK && width == WIDTH_NOR)
	{
		if (width > WIDTH_MIN) width -= 15;
		if (jumpPower > JUMP_MAX) jumpPower -= FAT_ADJUST;
	}
	// やや瘦せ〜瘦せ → 少しずつ減少
	else if (walkedDistance >= DISTANCE_TO_SHRINK && width > WIDTH_MIN)
	{
		if (width > WIDTH_MIN) width -= 15;
		if (jumpPower > JUMP_MAX) jumpPower -= FAT_ADJUST;
	}

	walkedDistance = 0.0f; // カウントリセット

	ResolveStuckAfterResize(blocks);  // ここでプレイヤーの位置を調整

	// 体型変化後にブロックに埋まっていないか確認
	FixStuckInBlock(blocks);
}

void Player::Bounce() {
	velocityY = JUMP_MAX / 2;  // 通常ジャンプの半分くらいの反発力
	isJumping = true;
	jumpCooldownTimer = JUMP_COOLDOWN_DURATION;
}

int Player::GetFatLevel() const {
	if (width <= WIDTH_MIN)       return 0; // 瘦せ
	if (width <= WIDTH_NOR - 15)  return 1; // やや瘦せ
	if (width == WIDTH_NOR)       return 2; // 普通
	if (width <= WIDTH_NOR + 40)  return 3; // やや肥満
	if (width <= WIDTH_NOR + 80)  return 4; // 肥満(1度)
	if (width <= WIDTH_NOR + 200) return 5; // 肥満(2度)
	if (width <= WIDTH_NOR + 280) return 6; // 肥満(3度)
	if (width <= WIDTH_MAX)       return 7; // 肥満(4度以上)
}

FatState Player::GetFatState() const {
	if (width <= WIDTH_MIN)       return FatState::Thin;
	if (width <= WIDTH_NOR - 15)  return FatState::SlightlyThin;
	if (width == WIDTH_NOR)       return FatState::Normal;
	if (width <= WIDTH_NOR + 40)  return FatState::SlightlyFat;
	if (width <= WIDTH_NOR + 80)  return FatState::Fat1;
	if (width <= WIDTH_NOR + 120) return FatState::Fat2;
	if (width <= WIDTH_NOR + 280) return FatState::Fat3;
	if (width <= WIDTH_MAX)       return FatState::Fat4;
}

void Player::SetBlockImages(const int* images)
{
	blockImages = images;
}

//int Player::GetX() const { return x; }
//int Player::GetY() const { return y; }
//bool Player::IsDead() const { return isDead; }

//bool Player::IsJumping() const
//{
//	return isJumping;  // 地面に接していない間はジャンプ中とみなす
//}

// サイズ変更後に壁や床に埋まっていたら少しずつ補正
void Player::ResolveStuckAfterResize(const std::vector<Block>& blocks)
{
	Rect playerRect = GetRect();

	for (const auto& block : blocks) {
		if (!block.HasCollision()) continue;

		const Rect& blockRect = block.GetRect();
		if (playerRect.Intersects(blockRect)) {
			// 右側がめり込んでいる
			if (playerRect.x < blockRect.x && playerRect.x + playerRect.w > blockRect.x) {
				x -= 1;  // 左にずらす
			}
			// 左側がめり込んでいる
			else if (playerRect.x >= blockRect.x && blockRect.x + blockRect.w > playerRect.x) {
				x += 1;  // 右にずらす
			}

			// 確実に更新された座標で再チェック
			playerRect = GetRect();
		}
	}
}

//追加
void Player::FixStuckInBlock(const std::vector<Block>& blocks)
{
	Rect playerRect = GetRect();

	// 最大32ドット上にずらしてみる
	const int maxOffset = 32;
	for (int offset = 1; offset <= maxOffset; ++offset)
	{
		bool hit = false;
		Rect testRect = playerRect;
		testRect.y -= offset;

		for (const auto& block : blocks)
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