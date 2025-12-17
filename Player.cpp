#include "Player.h"
#include "Config.h"
#include "Input.h"
#include <cmath>

void Player::Init(const Rect& start) {
	x = start.x + PlayerConfig:: WIDTH_NORMAL / GlobalConfig::Break_Number;
	y = start.y + start.h - PlayerConfig::HEIGHT / GlobalConfig::Break_Number;

	width = PlayerConfig:: WIDTH_NORMAL;

	fixedStartY = y;

	width	  =	PlayerConfig:: WIDTH_NORMAL; // 普通体型
	speed	  =	PlayerConfig::SPEED_MAX;     // 移動速度(最高値)
	jumpPower = PlayerConfig::JUMP_MAX;      // 跳躍力　(最高値)
	velocityY = 0.0f;

	walkedDistance = 0.0f;
	isDead    = false;          //プレイヤーは死亡判定
	isJumping = false;          //ジャンプ中か
	onGround  = false;          //地面の上にいるか
	jumpCooldownTimer = 0.0f;
	startFreezeTimer = PlayerConfig::START_FREEZE_SEC;

	// 画像ロード
	LoadImages();
	UI_Bar_Case = LoadGraph("image/Bar_Case.png");		//バーの枠の画像

	// 満腹度バー画像ロード
	UI_Bar_Green  = LoadGraph("image/Green_Bar.png");
	UI_Bar_Yellow = LoadGraph("image/Yellow_Bar.png");
	UI_Bar_Red    = LoadGraph("image/Red_Bar.png");

	//体型表示
	UI_Thin         = LoadGraph("image/Thin.png");			//Thinの画像
	UI_SlightlyThin = LoadGraph("image/SlightlyThin.png");	//SlightlyThin画像
	UI_Normal       = LoadGraph("image/Normal.png");		//Normalの画像
	UI_SlightlyFat  = LoadGraph("image/SlightlyFat.png");	//SlightlyFatの画像
	UI_Fat1         = LoadGraph("image/Fat1.png");			//Fat1の画像
	UI_Fat2         = LoadGraph("image/Fat2.png");			//Fat2の画像
	UI_Fat3         = LoadGraph("image/Fat3.png");			//Fat3の画像
	UI_Fat4         = LoadGraph("image/Fat4.png");			//Fat4の画像

	//警告用
	UI_WARNING_BD = LoadGraph("image/WARNING.png");
	UI_WARNING    = LoadGraph("image/Danger.png");

	//ジャンプの効果音
	Jump_Sound = LoadSoundMem("sound/Jumping.mp3");

	//警告音
	Warning_Sound = LoadSoundMem("sound/Warning.mp3");

	//soundvolume_ = 128 * 4;
}


void Player::Update(std::vector<Block>& blocks, double deltaTime) {

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
	int moveX		   = Input::GetAxisX();  // -1:左, 1:右
	bool jumpPressed   = Input::IsJump();
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
		startFreezeTimer -= (float)deltaTime;

		y = fixedStartY;        // y座標を固定

		velocityY = 0.0f;       // 落下も止める

		// ここでジャンプ入力を受け付けてもいい
		if (jumpPressed && jumpCooldownTimer <= 0.0f)
		{
			velocityY = jumpPower;
			isJumping = true;
			startFreezeTimer = 0.0f;  // フリーズ解除してジャンプ開始
			onGround = false;

			//ジャンプ音を再生
			PlaySoundMem(Jump_Sound, DX_PLAYTYPE_BACK);
			ChangeVolumeSoundMem(soundvolume_, Jump_Sound);
		}
	}

	// 横移動処理ベータタイム
	int dx = (int)(moveX * speed * deltaTime * PlayerConfig::SPEED_CORRECTION);
	if (dx != 0) {
		walkedDistance += abs(dx);
		isFacingRight = (dx > 0);
	}

	//左右判定
	int step = (dx > 0) ? GlobalConfig::RIGHT : GlobalConfig::LEFT;
	
	int movedX = 0;
	while (movedX != dx) {
		int nextX = x + step;
		Rect testRect = GetRect();
		testRect.x = nextX - width / GlobalConfig::Break_Number;

		bool hit = false;
		for (const auto& block : blocks) {
			if (!block.HasCollision()) continue;
			const Rect& wall = block.GetRect();

			//ブロックとの当たり判定のだから触らないで
			bool verticalOverlap =
				testRect.y + testRect.h > wall.y + PlayerConfig::COLLISION_MARGIN &&
				testRect.y < wall.y + wall.h - PlayerConfig::COLLISION_MARGIN;

			if (testRect.Intersects(wall) && verticalOverlap) {
				hit = true;
				break;
			}
		}
		if (hit) break;
		x += step;
		movedX += step;
	}

	// 通常ジャンプ（地面上）
	if (jumpPressed && onGround && jumpCooldownTimer <= 0.0f)
	{
		velocityY = jumpPower;
		isJumping = true;
		startFreezeTimer = 0.0f;  // フリーズ解除してジャンプ開始
		//ジャンプ音を再生
		PlaySoundMem(Jump_Sound, DX_PLAYTYPE_BACK);
	}

	// 重力
	velocityY += PlayerConfig::GRAVITY * deltaTime *
		PlayerConfig::SPEED_CORRECTION;

	// 終端速度制限
	if (velocityY > PlayerConfig::TERMINAL_VELOCITY)
		velocityY = PlayerConfig::TERMINAL_VELOCITY;

	// Y座標に反映
	y += (int)(velocityY * deltaTime * PlayerConfig::SPEED_CORRECTION);

	// 着地判定
	onGround = (startFreezeTimer > 0.0f);
	Rect footRect = GetRect();

	int dy = (int)(velocityY * deltaTime * PlayerConfig::SPEED_CORRECTION);
	dy = (dy > 1) ? dy : 1;
	footRect.y += dy;

	for (int i = (int)blocks.size() - 1; i >= 0; --i) {
		Block& block = blocks[i];
		if (!block.HasCollision()) continue;

		const Rect& rect = block.GetRect();

		//横に少しでもかぶっているかも確認（特に空中で）
		bool horizontalOverlap =
			footRect.x + footRect.w > rect.x + PlayerConfig::COLLISION_MARGIN &&
			footRect.x < rect.x + rect.w - PlayerConfig::COLLISION_MARGIN;

		// プレイヤーがブロックに接地しているか判定
		if (velocityY >= 0 && footRect.Intersects(rect) && horizontalOverlap) {

			y = rect.y - PlayerConfig::HEIGHT / GlobalConfig::Break_Number;  // めり込まないよう補正
			velocityY = 0.0f;
			onGround = true;
			if (isJumping)
			{
				isJumping = false;
				jumpCooldownTimer = PlayerConfig::JUMP_COOLDOWN;  // クールタイム開始
			}
			break;
		}
	}

	// 天井にぶつかる判定
	Rect headRect = GetRect();
	for (const auto& block : blocks) {
		if (!block.HasCollision()) continue;
		const Rect& rect = block.GetRect();

		bool horizontalOverlap =
			headRect.x + headRect.w > rect.x + PlayerConfig::COLLISION_MARGIN &&
			headRect.x < rect.x + rect.w - PlayerConfig::COLLISION_MARGIN;

		if (velocityY < 0 && headRect.Intersects(rect) && horizontalOverlap) {
			y = rect.y + rect.h + PlayerConfig::HEIGHT / GlobalConfig::Break_Number;
			velocityY = 0;
			break;
		}
	}

	// ジャンプクールタイムのカウント
	if (jumpCooldownTimer > 0.0f)
	{
		jumpCooldownTimer -= (float)deltaTime;
	}

	// 落下死判定
	if (startFreezeTimer <= 0.0f && y > PlayerConfig::FALL_LIMIT_Y && !onGround) {
		isDead = true;
	}

	// 歩行距離による体型変化
	ShrinkByDistance(blocks);

	// 満腹度バー減少処理
	switch (GetFatState()) {
	case FatState::Thin:
		shrinkDistance = PlayerConfig::DISTANCE_SHRINK_MAX;
		break;
	case FatState::SlightlyThin: 
	case FatState::Normal:
		shrinkDistance = PlayerConfig::DISTANCE_SHRINK_NOR;
		break;
	case FatState::SlightlyFat:
	case FatState::Fat1:
	case FatState::Fat2:
	case FatState::Fat3:
	case FatState::Fat4:
		shrinkDistance = PlayerConfig::DISTANCE_SHRINK_MIN;
		break;
	}
	
	// 移動距離に応じて満腹度減少
	int remaining = (int)(shrinkDistance - walkedDistance);
	if (remaining < 0) remaining = 0;

	//瘦せ状態の警告点滅処理
	if (GetFatState() == FatState::Thin) {
		thinWarningTimer += (float)deltaTime;

		// 点滅切り替え
		if (fmod(thinWarningTimer, PlayerConfig::THIN_BLINK_INT * (int)GlobalConfig::Break_Number) < PlayerConfig::THIN_BLINK_INT) {
			thinWarningVisible = true;
		}
		else {
			thinWarningVisible = false;
		}
	
		// 警告表示の有効期間を超えたら非表示にする
		if (thinWarningTimer > PlayerConfig::THIN_WARN_SEC) {
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

	if (isDead) return;// 死亡時は描画しない
	
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
	int left   = rect.x - scrollX;
	int top    = rect.y;
	int right  = rect.x + rect.w - scrollX;
	int bottom = rect.y + rect.h;

	// プレイヤーの描画の左右反転
	if (isFacingRight) {
		// 右向き
		DrawExtendGraph(left, top, right, bottom, handle, TRUE);
	}
	else {
		// 左向き
		DrawExtendGraph(right, top, left, bottom, handle, TRUE);
	}

	// 残り距離表示
	int uiHandle = Image;

	//警告用
	int ui_Warning_bg = UI_WARNING_BD;
	int ui_Warning    = UI_WARNING;

	switch (GetFatState()) {
	case FatState::Thin:
		uiHandle = UI_Thin;			break;
	case FatState::SlightlyThin:
		uiHandle = UI_SlightlyThin;	break;
	case FatState::Normal:
		uiHandle = UI_Normal;		break;
	case FatState::SlightlyFat:
		uiHandle = UI_SlightlyFat;	break;
	case FatState::Fat1:
		uiHandle = UI_Fat1;			break;
	case FatState::Fat2:
		uiHandle = UI_Fat2;			break;
	case FatState::Fat3:
		uiHandle = UI_Fat3;			break;
	case FatState::Fat4:
		uiHandle = UI_Fat4;			break;
	}

	// 移動距離に応じて満腹度減少
	int remaining = (int)(shrinkDistance - walkedDistance);
	if (uiHandle >= 0) {
		// 瘦せ状態のときは点滅させる
		if (GetFatState() == FatState::Thin &&
			ui_Warning_bg >= 0 && ui_Warning >= 0)
		{
			if (thinWarningVisible) {
				//警告音を再生
				PlaySoundMem(Warning_Sound, DX_PLAYTYPE_BACK);
				ChangeVolumeSoundMem(soundvolume_ / GlobalConfig::Break_Number, Warning_Sound);
				// 通常は常時表示(画面中央)
				DrawGraph(0, 0, uiHandle,      TRUE);
				DrawGraph(0, 0, ui_Warning_bg, TRUE);
				DrawGraph(0, 0, ui_Warning,    TRUE);
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

	// 満腹度バー描画
	int barX = 0, barY = 0;

	// remaining を割合に変換（例: 最大100なら 0.0～1.0 に正規化）
	float ratio = (float)remaining / (float)shrinkDistance;
	if (ratio < 0.0f) ratio = 0.0f;
	if (ratio > 1.0f) ratio = 1.0f;

	// 色判定（緑 → 黄 → 赤）
	int barHandle = UI_Bar_Green;

	if (ratio <= PlayerConfig::DIS_BAR_RED_ZONE)
		barHandle = UI_Bar_Red;

	else if (ratio <= PlayerConfig::DIS_BAR_YELLOW_ZONE)
		barHandle = UI_Bar_Yellow;
	
	// 中身の「ソース画像」のサイズを取得
	int srcW = 0, srcH = 0;
	if (barHandle >= 0) GetGraphSize(barHandle, &srcW, &srcH);

	// 描画する幅（左から ratio 倍だけ切り出す）
	int srcFilledW  = (int)(srcW * ratio);
	
	// 枠を描画
	if (UI_Bar_Case >= 0) {
		DrawGraph(barX, barY, UI_Bar_Case, TRUE);
	}

	// 中身を左から描画
	if (barHandle >= 0 && srcFilledW > 0) {
		DrawRectGraph(barX, barY, 0, 0, srcFilledW, srcH, barHandle, TRUE);
	}
}

//プレイヤーの描画
Rect Player::GetRect() const {
	return { 
		x - width / (int)GlobalConfig::Break_Number,
		y - PlayerConfig::HEIGHT / (int)GlobalConfig::Break_Number,
		width,
		PlayerConfig::HEIGHT 
	};
}

//食べたときの処理(大雑把に分けて8段階太る)
void Player::Grow(std::vector<Block>& blocks) {
	FatState state = GetFatState();
	if (width >=PlayerConfig::WIDTH_FAT_4) {
		isDead = true;  // 太りすぎで死亡
		return;
	}

	// 瘦せ または やや瘦せ のときは +15
	if (state == FatState::Thin || state == FatState::SlightlyThin) {
		if (width +  PlayerConfig::CONVERSION_WIDTH_MIN <= PlayerConfig:: WIDTH_NORMAL) {
			width +=  PlayerConfig::CONVERSION_WIDTH_MIN;
			if (jumpPower < PlayerConfig::JUMP_MIN) {
				jumpPower += PlayerConfig::CONVERSION_JUMP;
			}
		}
		else {
			width = PlayerConfig:: WIDTH_NORMAL; // 普通の最大に調整
		}
	}
	else {
		// 普通以上は従来通り +40
		if (width <PlayerConfig::WIDTH_FAT_4) width +=  PlayerConfig::CONVERSION_WIDTH_MAX;
		if (speed > PlayerConfig::SPEED_MIN) speed -= PlayerConfig::CONVERSION_SPEED;
		if (jumpPower < PlayerConfig::JUMP_MIN) {
			jumpPower += PlayerConfig::CONVERSION_JUMP;
		}
	}

	walkedDistance = 0.0f;			  // カウントリセット
	ResolveStuckAfterResize(blocks);  // ここでプレイヤーの位置を調整
	FixStuckInBlock(blocks);		  // 体型変化後にブロックに埋まっていないか確認
}

void Player::LoadImages() {
	idleImage     = LoadGraph("image/player_idle.png");
	jumpImage     = LoadGraph("image/player_jump.png");
	moveImages[0] = LoadGraph("image/player_move1.png");
	moveImages[1] = LoadGraph("image/player_move2.png");
	moveImages[2] = LoadGraph("image/player_move3.png");
}

//動いているとき
void Player::ShrinkByDistance(std::vector<Block>& blocks) {
	if (walkedDistance < shrinkDistance) return;
	FatState state = GetFatState();

	//「瘦せ状態」で歩ききったら死亡
	if (state == FatState::Thin) {
		isDead = true;
		return;
	}

	// 普通より太っている → 大きく減少
	if (walkedDistance >= PlayerConfig::DISTANCE_TO_SHRINK * PlayerConfig::DISTANCE_FIXED_VALUE
			&& width > PlayerConfig:: WIDTH_NORMAL) {

		if (width > PlayerConfig:: WIDTH_NORMAL) {
			if (width > PlayerConfig::WIDTH_THIN) width -=  PlayerConfig::CONVERSION_WIDTH_MAX;
			if (speed < PlayerConfig::SPEED_MAX) speed += PlayerConfig::CONVERSION_SPEED;
			if (jumpPower > PlayerConfig::JUMP_MAX) {
				jumpPower -= PlayerConfig::CONVERSION_JUMP;
			}
		}
	}
	// 普通から痩せる → 少し減らす
	else if (walkedDistance >= PlayerConfig::DISTANCE_TO_SHRINK * PlayerConfig::DISTANCE_FIXED_VALUE
				&& width == PlayerConfig:: WIDTH_NORMAL)
	{
		if (width > PlayerConfig::WIDTH_THIN) width -=  PlayerConfig::CONVERSION_WIDTH_MIN;
		if (jumpPower > PlayerConfig::JUMP_MAX) {
			jumpPower -= PlayerConfig::CONVERSION_JUMP;
		}
	}
	// やや瘦せ〜瘦せ → 少しずつ減少
	else if (walkedDistance >= PlayerConfig::DISTANCE_TO_SHRINK * PlayerConfig::DISTANCE_FIXED_VALUE
				&& width > PlayerConfig::WIDTH_THIN)
	{
		if (width > PlayerConfig::WIDTH_THIN) width -=  PlayerConfig::CONVERSION_WIDTH_MIN;
		if (jumpPower > PlayerConfig::JUMP_MAX) {
			jumpPower -= PlayerConfig::CONVERSION_JUMP;
		}
	}

	walkedDistance = 0.0f;			  // カウントリセット
	ResolveStuckAfterResize(blocks);  // ここでプレイヤーの位置を調整
	FixStuckInBlock(blocks);		  // 体型変化後にブロックに埋まっていないか確認
}

void Player::Bounce() {
	velocityY = PlayerConfig::JUMP_MAX / GlobalConfig::Break_Number;  // 通常ジャンプの半分くらいの反発力
	isJumping = true;
	jumpCooldownTimer = PlayerConfig::JUMP_COOLDOWN;
}

FatState Player::GetFatState() const {
	if (width <= PlayerConfig::WIDTH_THIN)			return FatState::Thin;			// 瘦せ
	if (width <= PlayerConfig::WIDTH_SLIGHTLY_THIN) return FatState::SlightlyThin;	// やや瘦せ
	if (width == PlayerConfig::WIDTH_NORMAL)        return FatState::Normal;		// 普通
	if (width <= PlayerConfig::WIDTH_SLIGHTLY_FAT)  return FatState::SlightlyFat;	// やや肥満
	if (width <= PlayerConfig::WIDTH_FAT_1)         return FatState::Fat1;			// 肥満(1度)
	if (width <= PlayerConfig::WIDTH_FAT_2)         return FatState::Fat2;			// 肥満(2度)
	if (width <= PlayerConfig::WIDTH_FAT_3)         return FatState::Fat3;			// 肥満(3度)
	if (width <= PlayerConfig::WIDTH_FAT_4)         return FatState::Fat4;			// 肥満(4度以上)
}

void Player::SetBlockImages(const int* images)
{
	blockImages = images;
}

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
				x -= GlobalConfig::LEFT;   // 左にずらす
			}
			// 左側がめり込んでいる
			else if (playerRect.x >= blockRect.x && blockRect.x + blockRect.w > playerRect.x) {
				x += GlobalConfig::RIGHT;  // 右にずらす
			}

			// 確実に更新された座標で再チェック
			playerRect = GetRect();
		}
	}
}

void Player::FixStuckInBlock(const std::vector<Block>& blocks)
{
	Rect playerRect = GetRect();

	// 最大32ドット上にずらしてみる
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
