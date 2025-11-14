#pragma once
#include <vector>
#include <string>
#include "Types.h"
#include "Enemy.h"
#include "Block.h" 

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
//    TILE_EMPTY,
//    TILE_WALL,
//};

// マップデータ全体を保持するクラス
class MapData {
private:
    std::vector<std::string> rawMap;     // 元のマップ文字列
    int tileSize = 40;                   // タイル1マスのピクセルサイズ               
protected:
public:
    // マップ文字列からマップを構築する
    void LoadFromString(const std::vector<std::string>& mapLines, const int* blockImages);

    std::vector<Block> blocks;                  // ブロック一覧
    std::vector<Item> items;                    // アイテム一覧
    std::vector<Enemy> enemies;                 // 敵一覧
    Rect playerStart;                           // プレイヤー初期位置

    //const int tileSize = 40;        // タイル1マスのピクセルサイズ
    //std::vector<std::string> rawMap;    // 元のマップ文字列
};