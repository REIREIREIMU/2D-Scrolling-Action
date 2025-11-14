#include "MapData.h"

void MapData::LoadFromString(const std::vector<std::string>& mapLines, const int* blockImages) {
    rawMap = mapLines;
    blocks.clear();
    items.clear();
    enemies.clear();

    for (int y = 0; y < mapLines.size(); ++y) {
        for (int x = 0; x < mapLines[y].size(); ++x) {
            char ch = mapLines[y][x];
            int px = x * tileSize;
            int py = y * tileSize;

            // 各文字に対応する処理（ブロックや敵の生成）
            switch (ch)
            {
            case 'w': {  // 地面ブロックA
                Block block({ px, py, tileSize, tileSize }, BlockType::GroundA);
                block.SetImage(blockImages[(int)BlockType::GroundA]);
                block.SetBlockImages(blockImages);
                blocks.push_back(block);
                break;
            }

            case '=': {  // 地面ブロックB
                Block block({ px, py, tileSize, tileSize }, BlockType::GroundB);
                block.SetImage(blockImages[(int)BlockType::GroundB]);
                block.SetBlockImages(blockImages);
                blocks.push_back(block);
                break;
            }

            case '#': {  // レンガブロック
                Block block({ px, py, tileSize, tileSize }, BlockType::Brick);
                block.SetImage(blockImages[(int)BlockType::Brick]);
                block.SetBlockImages(blockImages);
                blocks.push_back(block);
                break;
            }

            case '?': {  // ？ブロック
                Block block({ px, py, tileSize, tileSize }, BlockType::Question);
                block.SetImage(blockImages[(int)BlockType::Question]);
                block.SetBlockImages(blockImages);
                blocks.push_back(block);
                break;
            }

            case '/': {  // ？ブロック(空)
                Block block({ px, py, tileSize, tileSize }, BlockType::Question_Empty);
                block.SetImage(blockImages[(int)BlockType::Question_Empty]);
                block.SetBlockImages(blockImages);
                blocks.push_back(block);
                break;
            }

            case '$': {  // ゴールブロック（仮）
                Block block({ px, py, tileSize, tileSize }, BlockType::Goal); // Goal を使う
                block.SetImage(blockImages[(int)BlockType::Goal]);
                block.SetBlockImages(blockImages);
                blocks.push_back(block);
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
                items.push_back({ { px, py, tileSize, tileSize }, type });
                break;
            }
                    // 敵 'A' ～ 'D'
            case 'A': {   // Enemy type 0（穴に落ちる）
                // 敵画像の見た目に比べて当たり判定は小さく、下に寄せる（例：高さ30、下に10ずらす）
            
                Enemy e1({ px, py, tileSize, tileSize }, 0, -100);// 下部に合わせる
                enemies.push_back(e1);
                break;

            }

            case 'B': {  // Enemy type 1（穴で反転）
                
                Enemy e2({ px, py, tileSize, tileSize }, 1, -100); // ← 同じ速度に統一
                enemies.push_back(e2);
                break;
            }

            case 'C': {  // Enemy type 2（ジャンプ弾幕系など）
                Enemy e3({ px, py, tileSize, tileSize }, 2, 0, 0.0f);
                enemies.push_back(e3);
                break;
            }

            case 'D': {  // Enemy type 3（将来用）
                Enemy e({ px, py, tileSize, tileSize }, 3, -1, 1.0f);
                enemies.push_back(e);
                break;
            }

            default:
                // 空白やその他の文字は無視
                break;
            }
        }
    }
}