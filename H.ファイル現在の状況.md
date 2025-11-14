### **・h.ファイル**



##### **//Block.h**

\#pragma once

\#include "Types.h"

\#include <DxLib.h>



// ブロックの種類

enum class BlockType {

 	GroundA,   // 地面ブロックA

 	GroundB,   // 地面ブロックB

 	Brick,     // レンガブロック

 	Question,  // ？ブロック



 	Goal,

};



class Block {

public:

 	void SetType(BlockType newType);       // 追加

 	Block(Rect rect, BlockType type);      // コンストラクタ



 	void Draw(int scrollX) const;          // 描画

 	const Rect\& GetRect() const;           // 当たり判定領域取得

 	BlockType GetType() const;             // 種類取得



 	bool HasCollision() const;             // 当たり判定の有無（今後用）



 	void SetImage(int imageID);           // 画像をセット

 	void SetBlockImages(const int\* images);



 	bool IsActivated() const;

 	void Activate();



private:

 	bool activated = false;

 	Rect rect;         // ブロックの位置・大きさ

 	BlockType type;    // ブロックの種類

 	int imageID = -1;                     // -1なら画像未使用

 	const int\* blockImages = nullptr;     // ブロック画像配列の参照用

};





##### **//ClearScene.h**

\#pragma once

\#include "SceneBase.h"



class ClearScene : public SceneBase {

private:

 	bool endFlag = false;



public:

 	void Init() override;

 	void Update() override;

 	void Draw() override;

 	bool IsEnd() override;

 	int NextScene() override;

};



##### **//Enemy.h**

\#pragma once

\#include "Types.h"

\#include <vector>

\#include "Block.h"



// 弾の速度を保持する構造体

struct EnemyBullet {

&nbsp;	float x, y;     // float位置

&nbsp;	int w, h;       // 幅・高さ

&nbsp;	float vx, vy;   // 速度



&nbsp;	Rect GetRect() const {

&nbsp;		return { (int)x, (int)y, w, h };

&nbsp;	}

};



// 敵のクラス

class Enemy {

public:

&nbsp;	Rect rect;     // 位置とサイズ

&nbsp;	int type;      // 敵の種類 (0: 落下, 1: 穴回避, 2以降: その他)

&nbsp;	int vx;        // 移動方向（-1 or 1）

&nbsp;	float speed;   // 移動速度（例：0.5f など）



&nbsp;	std::vector<EnemyBullet> bullets; // 弾の速度一覧（同じ数）



&nbsp;	// コンストラクタ

&nbsp;	Enemy(Rect r, int t, int v);           // 速度はデフォルトで 1.0f

&nbsp;	Enemy(Rect r, int t, int v, float s);  // 速度を明示指定



&nbsp;	// 敵のロジック更新（移動・落下・反転）

&nbsp;	//void Update(const std::vector<Block>\& blocks, std::vector<Enemy>\& enemies, const Point\& playerPos);

&nbsp;	//void Update(const std::vector<Block>\& blocks, std::vector<Enemy>\& enemies, const Rect\& playerRect);

&nbsp;	void Update(const std::vector<Block>\& blocks, std::vector<Enemy>\& enemies, const Rect\& playerRect, int scrollX, double deltaTime);



&nbsp;	// 描画（アニメーション対応）

&nbsp;	void Draw(int scrollX, int ImageID) const;



&nbsp;	// アニメーション画像セット

&nbsp;	void SetAnimImages(const std::vector<int>\& images); 



private:

&nbsp;	// 地面に立っているか判定（落下の有無に使用）

&nbsp;	bool isOnGround(const std::vector<Block>\& blocks) const;



&nbsp;	// 進行方向の下に地面があるか判定（落下前の反転に使用）

&nbsp;	bool willFallAhead(const std::vector<Block>\& blocks) const;



&nbsp;	bool isFacingRight = false; // 左向き（初期値）true



&nbsp;	// ジャンプ用

&nbsp;	float jumpTimer = 0.0f;

&nbsp;	float bulletCooldown = 0.0f;

&nbsp;	float verticalVelocity = 0.0f;



&nbsp;	float posX\_f = 0.0f;

&nbsp;	float posY\_f = 0.0f;



&nbsp;	//std::vector<EnemyBullet> shots;



&nbsp;	// アニメーション管理

&nbsp;	std::vector<int> animImages\_; // Enemy1.png, Enemy2.png

&nbsp;	int currentAnimFrame\_ = 0;

&nbsp;	float animTimer\_ = 0.0f;

&nbsp;	float animFrameDuration\_ = 0.2f; // 0.2秒ごとに切り替え



};





##### **//GameOverScene.h**

\#pragma once

\#include "SceneBase.h"



class GameOverScene : public SceneBase {

private:

 	bool endFlag = false;



public:

 	void Init() override;

 	void Update() override;

 	void Draw() override;

 	bool IsEnd() override;

 	int NextScene() override;

};





##### **//GameScene.h**

\#pragma once

\#include "SceneBase.h"

\#include "MapData.h"

\#include "Player.h"

\#include "Enemy.h"

\#include "Block.h"

\#include <vector>



class GameScene : public SceneBase {

public:

 	void Init() override;

 	void Update() override;

 	void Draw() override;

 	bool IsEnd() override;

 	int NextScene() override;



 	void SetDeltaTime(float dt);



private:

 	void Update(float deltaTime);

 	bool CheckCollision(const Rect\& a, const Rect\& b);



 	int scrollX = 0;                     // スクロール位置（整数型に修正）

 	bool endFlag = false;               // シーン終了フラグ

 	int nextSceneID = 0;                // 次のシーンID（0:タイトル, 2:ゲームオーバー, 3:クリア）



 	std::vector<Block> blocks;

 	std::vector<Item> items;

 	std::vector<bool> itemCollected;

 	std::vector<Enemy> enemies;

 	std::vector<int> enemyImages;

 	std::vector<int> itemImages;



 	Player player;

 	MapData map;



 	// ブロック画像

 	int blockImages\[(int)BlockType::Question + 1];



 	int backgroundImage = -1; // 背景画像用



 	const int SCREEN\_WIDTH = 1280;

 	const int SCREEN\_HEIGHT = 720;



 	float deltaTimeForUpdate = 0.0f;

};





##### **//MapData.h**

\#pragma once

\#include <vector>

\#include <string>

\#include "Types.h"

\#include "Enemy.h"

\#include "Block.h"



// 敵データ構造（位置＋種類）

//struct Enemy {

//    Rect rect;

//    int type; // 0:トマト, 1:たまねぎ, 2:にんじん

//};



// アイテムデータ構造（位置＋種類）

struct Item {

    Rect rect;

    int type; // 0:肉, 1:ドーナツ, ... 4:ピザ

};



//enum TileType {

//    TILE\_EMPTY,

//    TILE\_WALL,

//};



// マップデータ全体を保持するクラス

class MapData {

private:

    std::vector[std::string](std::string) rawMap;     // 元のマップ文字列

    int tileSize = 40;                   // タイル1マスのピクセルサイズ

protected:

public:

    // マップ文字列からマップを構築する

    void LoadFromString(const std::vector[std::string](std::string)\& mapLines, const int\* blockImages);



    std::vector<Block> blocks;                  // ブロック一覧

    std::vector<Item> items;                    // アイテム一覧

    std::vector<Enemy> enemies;                 // 敵一覧

    Rect playerStart;                           // プレイヤー初期位置



    //const int tileSize = 40;        // タイル1マスのピクセルサイズ

    //std::vector[std::string](std::string) rawMap;    // 元のマップ文字列

};



##### **//Player.h**

\#pragma once

\#include "Types.h"

\#include "Block.h"

\#include <vector>



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

 	void Init(const Rect\& start);

 	void Update(std::vector<Block>\& blocks, double deltaTime);

 	void Draw(int scrollX) const;



 	Rect GetRect() const;



 	void Grow(std::vector<Block>\& blocks);      // 太る処理

 	void Bounce();								// 敵を踏んだ時少し飛ぶ処理



 	int GetFatLevel() const;         // 数値型の段階

 	FatState GetFatState() const;    // enum型の体型



 	void SetBlockImages(const int\* images);

 	int GetX() const;

 	int GetY() const;

 	bool IsDead() const;             //プレイヤーの生死

 	bool IsJumping() const;



private:

 	void ShrinkByDistance(std::vector<Block>\& blocks);   // 移動に応じて瘦せる



 	int x, y;

 	int width;



 	float speed;

 	float jumpPower;

 	float velocityY;



 	int imageID;



 	bool isJumping = false;

 	bool isDead = false;



 	bool onGround;

 	float jumpCooldownTimer = 0.0f; // ジャンプクールタイムのカウント（フレーム単位）



 	bool isFacingRight = true;  // プレイヤーが右を向いているか



 	// float fallTime = 0.0f;  // 地面にいない時間のカウント

 	float startFreezeTimer = 3.0f;

 	const float FREEZE\_DURATION = 3.0f;  // 開始3秒間固定

 	int fixedStartY = 0;                // 初期y座標を覚えておく



 	float walkedDistance = 0.0f;    // 移動距離の統計



 	const int\* blockImages = nullptr;



 	const int HEIGHT = 40;          //高さ(身長)

 	const int WIDTH\_MIN = 10;        //横幅(最低値)

 	const int WIDTH\_NOR = 40;       //横幅(普通)

 	const int WIDTH\_MAX = 360;      //横幅(最高値)



 	const float SPEED\_MAX = 8.0f;   //移動速度(最低値)(3.0f)

 	const float SPEED\_MIN = 2.0f;   //移動速度(最高値)(1.0f)



 	const float JUMP\_MAX = -24.0f;  //ジャンプ力(最低値)(-12.0f)//-24

 	const float JUMP\_MIN = -8.0f;    //ジャンプ力(最高値)(-4.0f)//-8



 	const float GRAVITY = 1.00f;     //重力(0.25f)->(1.00f)

 	const int FALL\_LIMIT\_Y = 800;



 	const float DISTANCE\_TO\_SHRINK = 500.0f; // この距離移動すると痩せる



 	const float JUMP\_COOLDOWN\_DURATION = 0.10f;  // ジャンプ後のクールタイム

 	const int COLLISION\_MARGIN = 4;             // 衝突判定用マージン

 	const float SPEED\_CORRECTION = 60.0f;       // deltaTimeの補正

 	const float FAT\_ADJUST = 2.00f;             //太った時のペナルティー



 	// サイズ変更後の壁埋まり対応

 	void ResolveStuckAfterResize(const std::vector<Block>\& blocks);



 	void FixStuckInBlock(const std::vector<Block>\& blocks);

};



##### **//SceneBase.h**

\#pragma once



class SceneBase {

private:

protected:

public:

 	//virtual ~SceneBase() = default;

 	virtual ~SceneBase() {}

 	virtual void Init() = 0;                    // 初期化

 	virtual void Update(/\*float deltaTime\*/) = 0;   // ロジック更新

 	virtual void Draw() = 0;                    // 描画

 	virtual bool IsEnd() = 0;                   // 終了判定（画面遷移条件）

 	virtual int NextScene() = 0;                // 次のシーンID

};



##### **//SceneManager.h**

\#pragma once

\#include "SceneBase.h"



class SceneManager {

private:

 	int sceneID = 0;

 	SceneBase\* currentScene = nullptr;



protected:

public:

 	SceneManager();

 	~SceneManager();

 	void ChangeScene(int id);

 	void Update();

 	void Update(float deltaTime);

 	void Draw();

 	SceneBase\* GetCurrentScene();

};



##### **//TitleScene.h**

\#pragma once

\#include "SceneBase.h"



class TitleScene : public SceneBase {

private:

 	bool endFlag = false;

 	int backgroundImage = -1; // 背景画像のID

 	int TitleImage = -1; // 背景画像のID



protected:

public:

 	void Init() override;

 	void Update(/\*float deltaTime\*/) override;

 	void Draw() override;

 	bool IsEnd() override;

 	int NextScene() override;



};



##### **//Types.h**

\#pragma once



struct Rect {

 	int x, y, w, h;



 	// 衝突判定関数を宣言

 	bool Intersects(const Rect\& other) const {

 		return x < other.x + other.w \&\&

 			x + w > other.x \&\&

 			y < other.y + other.h \&\&

 			y + h > other.y;

 	}

};

