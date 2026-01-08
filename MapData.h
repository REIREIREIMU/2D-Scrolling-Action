#pragma once
#include <vector>
#include <string>
#include "Types.h"
#include "Enemy.h"
#include "Block.h" 

// アイテムデータ構造
struct Item {
    Rect rect;  // 位置
    int  type;  // 種類
};

// マップデータ全体を保持するクラス
class MapData {           
public:
    // マップ文字列からマップを構築する
    void LoadFromString(const std::vector<std::string>& mapLines, const int* blockImages);

    //CSVからマップを構築する
    void LoadMapFromCsv(const std::string& filename, const int* blockImages);

    std::vector<Block> blocks;           // ブロック一覧
    std::vector<Item>  items;            // アイテム一覧
    std::vector<Enemy> enemies;          // 敵一覧
    Rect               playerStart;      // プレイヤー初期位置

    std::vector<Rect> fallTriggers;//Lの座標を保存する
    std::vector<Rect> UpTriggers;//Uの座標を保存する
private:
    std::vector<std::string> rawMap;     // 元のマップ文字列 
};