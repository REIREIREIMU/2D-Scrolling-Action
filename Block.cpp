#include "Block.h"
#include "DxLib.h"
#include <algorithm>

//ランダム使うよ(出てくるレンガの破片のために)
#include <cstdlib> 
#include <ctime>

// 静的メンバの定義
int Block::brickFragmentImages[2] = { -1, -1 };

int Block::questionImages[3] = { -1, -1, -1 };
int Block::questionImageCount = 0;

Block::Block(Rect rect, BlockType type)
	: rect(rect), type(type) {
}

void Block::SetImage(int imageID_) {
	imageID = imageID_;
}

void Block::SetBlockImages(const int* images) {
	blockImages = images;
}

void Block::SetType(BlockType newType) {
	type = newType;
}

//当たり判定のブロック
bool Block::HasCollision() const {
	//return true;

	return
		type == BlockType::GroundA ||
		type == BlockType::GroundB ||
		type == BlockType::Brick ||
		type == BlockType::Question||
		type == BlockType::Question_Empty;

	//type == BlockType::Goal;
}

const Rect& Block::GetRect() const {
	return rect;
}

BlockType Block::GetType() const {
	return type;
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
	activated = true;
	type = BlockType::Question_Empty; // 使い終わったブロックに見た目変更

	// 画像IDを Question_Empty の画像に変更
	if (blockImages != nullptr) {
		imageID = blockImages[(int)BlockType::Question_Empty];
	}
}

void Block::BreakBrick() {
	//if (type != BlockType::Brick) return;
	if (type != BlockType::Brick || isBroken) return;

	// 今破壊されたら true に
	//brokenThisFrame = true;

	isBroken = true;

	type = BlockType::None;

	imageID = -1;

	//時々レンガの破片が表示されないバグが起きる
	//fragments.clear();

	// 破片の数を 2〜4 のランダム化
	int pieceCount = 2 + rand() % 3; // 2から4つ表示

	// 左右に破片を飛ばす
	for (int i = 0; i < pieceCount; i++) {
		BrickPiece frag;
		frag.x = rect.x + rect.w / 2.0f;
		frag.y = rect.y + rect.h / 2.0f;

		// ランダムな速度で飛び散る
		frag.vx = (rand() % 2 == 0 ? -1.0f : 1.0f)
			* (120.0f + rand() % 80);		// 左右に飛ぶ
		frag.vy = (-250.0f + rand() % -150);  // 上に散らばる

		// 破片表示画像もランダム
		frag.imageID = brickFragmentImages[rand() % 2];

		frag.lifetime = 3.0f;         // 3秒間表示
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
		//if (animTimer >= 0.2f) {
		//	animTimer = 0.0f;
		//	animFrame = (animFrame + 1) % 3; // 0,1,2,3ループ
		//
		//	if (blockImages != nullptr) {
		//		// ？ブロック画像は3枚連番確保想定
		//		imageID = blockImages[(int)BlockType::Question * 3 + animFrame];
		//	}
		//}

		if (animTimer >= 0.2f) {
			animTimer = 0.0f;
			animFrame = (animFrame + 1) % questionImageCount; // 3枚ループ

			/*if (blockImages != nullptr) {
				imageID = blockImages[(int)BlockType::Question * 3 + animFrame];
			}*/
		}

	}

	// レンガ破片のアニメーション処理
	//// 破壊された瞬間に破片を出す
	//if (brokenThisFrame) {
	//
	//	//時々レンガの破片が表示されないバグが起きる
	//	//fragments.clear();
	//
	//	 // 破片の数を 2〜4 のランダム化
	//	int pieceCount = 2 + rand() % 3; // 2から4つ表示
	//
	//	// 左右に破片を飛ばす
	//	for (int i = 0; i < pieceCount; i++) {
	//		BrickPiece frag;
	//		frag.x = rect.x + rect.w / 2.0f;
	//		frag.y = rect.y + rect.h / 2.0f;
	//
	//		// ランダムな速度で飛び散る
	//		frag.vx = (rand() % 2 == 0 ? -1.0f : 1.0f)
	//			* (120.0f + rand() % 80);		// 左右に飛ぶ
	//		frag.vy = (-250.0f + rand() % -150);  // 上に散らばる
	//
	//		// 破片表示画像もランダム
	//		frag.imageID = brickFragmentImages[rand() % 2];
	//
	//		frag.lifetime = 3.0f;         // 3秒間表示
	//		fragments.push_back(frag);
	//	}
	//
	//	brokenThisFrame = false; // フラグをリセット
	//}
	//
	//// 破片の重力
	//for (auto it = fragments.begin(); it != fragments.end();) {
	//	it->x += it->vx * deltaTime;
	//	it->y += it->vy * deltaTime;
	//	it->vy += 750.0f * deltaTime; // 重力
	//	it->lifetime -= deltaTime;
	//
	//	if (it->lifetime <= 0) {
	//		it = fragments.erase(it);
	//	}
	//	else {
	//		++it;
	//	}
	//
	//}

	for (auto& frag : fragments) {
		frag.x += frag.vx * deltaTime;
		frag.y += frag.vy * deltaTime;
		frag.vy += 800.0f * deltaTime; // 重力
		frag.lifetime -= deltaTime;
		//frag.lifetime -= static_cast<float>(deltaTime);
	}

	//prevType = type;

	// 時間が過ぎたら破片を削除
	fragments.erase(
		std::remove_if(fragments.begin(), fragments.end(),
			[](const BrickPiece& f) { return f.lifetime <= 0.0f; }),
		fragments.end()
	);

}

void Block::Draw(int scrollX) const {

	// 通常ブロック描画(空ブロックは対象外)
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
		// デバッグ用の色描画
		//if (type == BlockType::Brick) return;

		// 画像なしなら色付きボックスで描画
		int color = GetColor(200, 200, 200);  // デフォルト色

		switch (type) {
		case BlockType::GroundA:
			color = GetColor(139, 69, 19);   // 茶色
			DrawBox(rect.x - scrollX, rect.y, rect.x + rect.w - scrollX, rect.y + rect.h, color, TRUE);
			break;

		case BlockType::GroundB:
			color = GetColor(160, 82, 45);   // 明るい茶色
			DrawBox(rect.x - scrollX, rect.y, rect.x + rect.w - scrollX, rect.y + rect.h, color, TRUE);
			break;

		case BlockType::Brick:
			color = GetColor(255, 100, 100); // 赤っぽいレンガ
			DrawBox(rect.x - scrollX, rect.y, rect.x + rect.w - scrollX, rect.y + rect.h, color, TRUE);
			break;

		case BlockType::Question:
			color = GetColor(255, 255, 0);   // 黄色
			DrawBox(rect.x - scrollX, rect.y, rect.x + rect.w - scrollX, rect.y + rect.h, color, TRUE);
			break;

		case BlockType::Goal:
			color = GetColor(0, 0, 0);   // 黒
			DrawBox(rect.x - scrollX, rect.y, rect.x + rect.w - scrollX, rect.y + rect.h, color, TRUE);
			break;

			//case BlockType::None:
			//	
			//	break;
		}

		//空ブロック以外は仮ブロックを表示する
		if(type != BlockType::None)
		DrawBox(rect.x - scrollX, rect.y, rect.x + rect.w - scrollX, rect.y + rect.h, color, TRUE);
	
	}

	//？ブロックの表示座標
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

	// レンガ破片の描画
	for (const auto& frag : fragments) {
		if (frag.imageID >= 0) {
			int w, h;
			GetGraphSize(frag.imageID, &w, &h);
			/*DrawExtendGraph(
				(int)frag.x - scrollX,
				(int)frag.y,
				(int)frag.x - scrollX + w / 2,
				(int)frag.y + h / 2,
				frag.imageID,
				TRUE
			);*/
			DrawExtendGraph(
				(int)frag.x - scrollX,
				(int)frag.y,
				(int)frag.x - scrollX + w / 18,
				(int)frag.y + h / 18,
				frag.imageID,
				TRUE
			);
		}
	}
}