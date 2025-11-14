#pragma once
#include "Types.h"
#include <DxLib.h>
#include <vector>

// ブロックの種類
enum class BlockType {
	None,      // 完全に消えた状態
	GroundA,   // 地面ブロックA
	GroundB,   // 地面ブロックB
	Brick,     // レンガブロック
	Question,  // ？ブロック
	Question_Empty,  // ？ブロック(空)
	Goal           // ゴールブロック

};

// レンガ破片用
struct BrickPiece {
	float x, y;      // 位置
	float vx, vy;    // 速度
	//int size;        // 描画サイズ
	int imageID;	 // 使用する破片画像
	float lifetime;  // 残り寿命
};

class Block {
public:
	Block(Rect rect, BlockType type);      // コンストラクタ

	void Update(double deltaTime);         // アニメーション更新
	void Draw(int scrollX) const;          // 描画

	const Rect& GetRect() const;           // 当たり判定領域取得
	BlockType GetType() const;             // 種類取得

	bool HasCollision() const;             // 当たり判定の有無

	void SetImage(int imageID);            // 画像をセット
	void SetBlockImages(const int* images);// ブロック画像をセット

	// レンガ破片用画像を外部から登録する
	static void SetBrickPieceImages(const int* images, int count); // 破片画像をセット

	bool IsActivated() const;              // ？ブロックが叩かれたかどうか
	void Activate();                       // ？ブロックを叩いた後の処理

	void SetType(BlockType newType);       // 種類を変更
	void BreakBrick(); // レンガ破壊処理

	// 破壊フラグ
	//bool WasBrokenThisFrame() const { return brokenThisFrame; }  // レンガブロックが破壊されたか
	//void ResetBrokenFlag() { brokenThisFrame = false; }			 // レンガ破壊時に呼ぶ

	// ？ブロックアニメーション画像を登録
	static void SetQuestionBlockImages(const int* images, int count);

private:
	bool activated = false;
	bool isBroken = false;                // レンガ破壊フラグ

	Rect rect;         // ブロックの位置・大きさ
	BlockType type;    // ブロックの種類

	int imageID = -1;                     // -1なら画像未使用
	const int* blockImages = nullptr;     // ブロック画像配列の参照用

	// ？ブロック用アニメーション
	int animFrame = 0;                     // 現在のフレーム
	double animTimer = 0.0;                // 経過時間（秒）
	static int questionImages[3];         // アニメーション用画像
	static int questionImageCount;

	// レンガ破片用
	std::vector<BrickPiece> fragments;  // 飛び散る破片
	static int brickFragmentImages[2];  // レンガ破片用画像（2種類）

	//bool brokenThisFrame = false;  // このフレームで壊れたか

};