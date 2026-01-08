#pragma once
#include "Types.h"
#include <DxLib.h>
#include <vector>

// ブロック系の設定値
namespace BlockConfig {
	const float Question_AnimTime = 0.2f;	   // ？ブロックのフレーム切替秒
	const float Piece_Gravity = 800.0f;    // 破片の落下加速度
	const float Piece_LiffTime = 3.0f;	   // 破片寿命（秒）
	const int   PieceCount_MIN = 2;		   // 破片の最小数
	const int   PieceCount_MAX = 4;		   // 破片の最大数

	const int   frag_MIN_X = 80;	// 左右の初速の最小数
	const int   frag_MAX_X = 120;  // 左右の初速の最大数
	const int   frag_MIN_Y = -250;	// 上の初速の最小数
	const int   frag_MAX_Y = 100;  // 上の初速の最大数

	const int   Reduction = 18;	//レンガの破片をサイズ調整用
}

// ブロックの種類
enum class BlockType {
	None,			// 完全に消えた状態
	GroundA,		// 地面ブロックA
	GroundB,		// 地面ブロックB
	Brick,			// レンガブロック
	Question,		// ？ブロック
	Question_Empty, // ？ブロック(空)
	FallBrick,      // 落下レンガブロック
	Goal            // ゴールブロック
};

// レンガ破片(演習用)
struct BrickPiece {
	float x, y;      // 位置
	float vx, vy;    // 速度
	float lifetime;  // 残り寿命（秒）
	int	  imageID;	 // 使用する破片画像
};

// ブロック系の設定
class Block {
public:
	Block(Rect rect, BlockType type);			  // コンストラクタ
	void Update(double deltaTime);			      // アニメーション更新
	void Draw(int scrollX) const;			      // 描画

	const Rect& GetRect() const { return rect; }  // 当たり判定領域取得
	BlockType	GetType() const { return type; }  // 種類取得
	bool HasCollision() const;                    // 当たり判定の有無

	void SetImage(int imageID);              // 画像をセット
	void SetBlockImages(const int* images);  // ブロック画像をセット
	bool IsActivated() const;                // ？ブロックが叩かれたかどうか
	void Activate();                         // ？ブロックを叩いた後の処理
	void SetType(BlockType newType);         // ブロックの種類を変化

	void BreakBrick();					     // レンガ破壊処理

	// ？ブロックアニメーション画像を登録
	static void SetQuestionBlockImages(const int* images, int count);

	// レンガ破片用画像を外部から登録する
	static void SetBrickPieceImages(const int* images, int count);

private:
	bool activated = false;				  // ？ブロック変化フラグ
	bool isBroken = false;                // レンガ破壊フラグ

	Rect	  rect;						  // ブロックの位置・大きさ
	BlockType type;						  // ブロックの種類

	int imageID = -1;                     // -1なら画像未使用
	const int* blockImages = nullptr;     // ブロック画像配列の参照用

	// ？ブロック用アニメーション
	int	   animFrame = 0;                 // 現在のフレーム
	double animTimer = 0.0;               // 経過時間（秒）
	static int questionImages[3];         // アニメーション用画像
	static int questionImageCount;        // アニメーションのカウントダウン用

	// レンガ破片用
	std::vector<BrickPiece> fragments;  // 飛び散る破片
	static int brickFragmentImages[2];  // レンガ破片用画像（2種類）

	//bool brokenThisFrame = false;     // このフレームで壊れたか
};