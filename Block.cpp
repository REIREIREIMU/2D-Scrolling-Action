#include "Block.h"
#include "DxLib.h"
#include "Config.h"
#include <algorithm>
#include <cstdlib> 
#include <ctime>

// 静的メンバの定義
int Block::brickFragmentImages[2] = { -1, -1 };
int Block::questionImages[3]	  = { -1, -1, -1 };
int Block::questionImageCount	  = 0;

Block::Block(Rect r, BlockType t):
	rect(r), type(t) {}

void Block::SetImage(int imageID_) {
	imageID = imageID_;
}

void Block::SetBlockImages(const int* images) {
	blockImages = images;
}

void Block::SetType(BlockType newType) {
	type = newType;
}

//当たり判定の有効のブロック
bool Block::HasCollision() const {
	return
		type == BlockType::GroundA ||
		type == BlockType::GroundB ||
		type == BlockType::Brick   ||
		type == BlockType::Question||
		type == BlockType::Question_Empty;
}

void Block::SetBrickPieceImages(const int* images, int count)
{
	for (int i = 0; i < count && i < 2; i++) {
		brickFragmentImages[i] = images[i];
	}
}

bool Block::IsActivated() const {
	return activated;
}

void Block::Activate() {

	// 今叩かれたら true
	activated = true;
	// 「使い終わった？ブロック」に見た目変更
	type = BlockType::Question_Empty;

	// 画像IDを Question_Empty の画像に変更
	if (blockImages != nullptr) {
		imageID = blockImages[(int)BlockType::Question_Empty];
	}
}

void Block::BreakBrick() {
	if (type != BlockType::Brick || isBroken) return;

	// 今破壊されたら true
	isBroken = true;
	// 破壊されたら「何も無い」に
	type = BlockType::None; 
	// 画像読み込み
	imageID = -1;

	// 破片の数を 2〜4 のランダム化
	int pieceCount = BlockConfig::PieceCount_MIN + rand() % BlockConfig::PieceCount_MAX; // 2から4つ表示

	// 左右に破片を飛ばす
	for (int i = 0; i < pieceCount; i++) {
		BrickPiece frag;
		frag.x = rect.x + rect.w / GlobalConfig::Break_Number;
		frag.y = rect.y + rect.h / GlobalConfig::Break_Number;

		// 左右にばらける初速（120〜200）
		float speedX = BlockConfig::frag_MAX_X + rand() % BlockConfig::frag_MIN_X;

		frag.vx = rand() % 2 == 0 ? -speedX : speedX;

		// 上向きの初速（-250〜-150）
		frag.vy = BlockConfig::frag_MIN_Y + rand() % BlockConfig::frag_MAX_Y;

		// 破片表示画像もランダム
		frag.imageID = brickFragmentImages[rand() % 2];

		frag.lifetime = BlockConfig::Piece_LiffTime;  // 3秒間表示
		fragments.push_back(frag);
	}
}

void Block::SetQuestionBlockImages(const int* images, int count)
{
	questionImageCount = count > 3 ? 3 : count;
	for (int i = 0; i < questionImageCount; i++) {
		questionImages[i] = images[i];
	}
}

void Block::Update(double deltaTime) {
	// ？ブロックのアニメーション処理
	if (type == BlockType::Question && !activated && questionImageCount > 0) {
		animTimer += deltaTime;
		// 0.2秒ごとに切り替え
		if (animTimer >= BlockConfig::Question_AnimTime) {
			animTimer = 0.0f;
			animFrame = (animFrame + 1) % questionImageCount; // 3枚ループ
		}
	}

	// 破片の移動・寿命
	for (auto& frag : fragments) {
		frag.x		  += frag.vx * (float)deltaTime;						// X座標移動
		frag.y		  += frag.vy * (float)deltaTime;						// Y座標移動
		frag.vy		  += BlockConfig::Piece_Gravity * (float)deltaTime;		// 重力
		frag.lifetime -= (float)deltaTime;									// 破片寿命（減算）
	}
	// 時間が過ぎたら破片を削除
	fragments.erase(
		std::remove_if(fragments.begin(), fragments.end(),
			[](const BrickPiece& f) { return f.lifetime <= 0.0f; }),
		fragments.end()
	);
}

void Block::Draw(int scrollX) const {

	// 画像優先で描画
	int drawX1 = rect.x - scrollX;
	int drawY1 = rect.y;
	int drawX2 = rect.x + rect.w - scrollX;
	int drawY2 = rect.y + rect.h;

	// ？ブロックアニメーション
	if (type == BlockType::Question && !activated && questionImageCount > 0) {
		DrawExtendGraph(drawX1, drawY1, drawX2, drawY2, questionImages[animFrame], TRUE);
	}
	else {
		if (imageID >= 0) {
			DrawExtendGraph(drawX1, drawY1, drawX2, drawY2, imageID, TRUE);
		}
	}

	// 通常ブロック描画(空ブロックは対象外)
	if (imageID >= 0) {
		//画像が設定されているなら画像を描画
		DrawExtendGraph(drawX1, drawY1, drawX2, drawY2,imageID, TRUE );
	}
	else {
		// 画像なしなら色付きボックスで描画
		switch (type) {
		case BlockType::GroundA:	// 茶色
			DrawBox(drawX1, drawY1, drawX2, drawY2, ColorConfig::Brown, TRUE);
			break;

		case BlockType::GroundB:	// 明るい茶色 
			DrawBox(drawX1, drawY1, drawX2, drawY2, ColorConfig::Light_Brown, TRUE);
			break;

		case BlockType::Brick:		// 赤っぽいレンガ
			DrawBox(drawX1, drawY1, drawX2, drawY2, ColorConfig::Red, TRUE);
			break;

		case BlockType::Question:	// 黄色
			DrawBox(drawX1, drawY1, drawX2, drawY2, ColorConfig::Yellow, TRUE);
			break;

		case BlockType::Goal:		// 黒
			DrawBox(drawX1, drawY1, drawX2, drawY2, ColorConfig::Black, TRUE);
			break;
		}
		//空ブロック以外は仮ブロックを表示する
		if(type != BlockType::None)
			DrawBox(drawX1, drawY1, drawX2, drawY2, ColorConfig::Default_Color, TRUE);
	}

	// レンガ破片の描画
	for (const auto& frag : fragments) {
		if (frag.imageID >= 0) {
			int w, h;
			GetGraphSize(frag.imageID, &w, &h);
			DrawExtendGraph(
				(int)frag.x - scrollX,
				(int)frag.y,
				(int)frag.x - scrollX + w / BlockConfig::Reduction,
				(int)frag.y + h / BlockConfig::Reduction,
				frag.imageID,
				TRUE
			);
		}
	}
}