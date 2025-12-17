#include "MapData.h"
#include "Config.h"

void MapData::LoadFromString(const std::vector<std::string>& mapLines, const int* blockImages) {
    rawMap = mapLines;
    blocks.clear(); items.clear(); enemies.clear();  // ブロック、アイテム、敵 等の一覧をクリア

    for (int y = 0; y < mapLines.size(); ++y) {
        for (int x = 0; x < mapLines[y].size(); ++x) {
            char ch = mapLines[y][x];
            int px = x * GlobalConfig::TILE_SIZE;
            int py = y * GlobalConfig::TILE_SIZE;

            // 各文字に対応する処理（ブロックや敵の生成）
            switch (ch)
            {
            case 'w': {  // 地面ブロックA
                Block block({ px, py, GlobalConfig::TILE_SIZE, GlobalConfig::TILE_SIZE }, BlockType::GroundA);
                block.SetImage(blockImages[(int)BlockType::GroundA]);
                block.SetBlockImages(blockImages);
                blocks.push_back(block);
                break;
            }

            case '=': {  // 地面ブロックB
                Block block({ px, py, GlobalConfig::TILE_SIZE, GlobalConfig::TILE_SIZE }, BlockType::GroundB);
                block.SetImage(blockImages[(int)BlockType::GroundB]);
                block.SetBlockImages(blockImages);
                blocks.push_back(block);
                break;
            }

            case '#': {  // レンガブロック
                Block block({ px, py, GlobalConfig::TILE_SIZE, GlobalConfig::TILE_SIZE }, BlockType::Brick);
                block.SetImage(blockImages[(int)BlockType::Brick]);
                block.SetBlockImages(blockImages);
                blocks.push_back(block);
                break;
            }

            case '?': {  // ？ブロック
                Block block({ px, py, GlobalConfig::TILE_SIZE, GlobalConfig::TILE_SIZE }, BlockType::Question);
                block.SetImage(blockImages[(int)BlockType::Question]);
                block.SetBlockImages(blockImages);
                blocks.push_back(block);
                break;
            }

            case '/': {  // ？ブロック(空)
                Block block({ px, py, GlobalConfig::TILE_SIZE, GlobalConfig::TILE_SIZE }, BlockType::Question_Empty);
                block.SetImage(blockImages[(int)BlockType::Question_Empty]);
                block.SetBlockImages(blockImages);
                blocks.push_back(block);
                break;
            }

            case '$': {  // ゴールブロック
                Block block({ px, py, GlobalConfig::TILE_SIZE, GlobalConfig::TILE_SIZE }, BlockType::Goal);
                block.SetImage(blockImages[(int)BlockType::Goal]);
                block.SetBlockImages(blockImages);
                blocks.push_back(block);
                break;
            }

            case 'P': {  // プレイヤーの開始位置
                playerStart = { px, py, GlobalConfig::TILE_SIZE, GlobalConfig::TILE_SIZE };
                break;
            }

                    // アイテム '1' ～ '5'
            case '1': case '2': case '3': case '4': case '5': {
                int type = ch - '1'; // 例: '3'なら type = 2
                items.push_back({ { px, py, GlobalConfig::TILE_SIZE, GlobalConfig::TILE_SIZE }, type });
                break;
            }

            case 'A': {   // Enemy type 0（穴に落ちる）
                Enemy e1 ({ px, py, GlobalConfig::TILE_SIZE, GlobalConfig::TILE_SIZE }, 0, GlobalConfig::LEFT, 100.0f);
                enemies.push_back(e1);
                break;
            }

            case 'B': {   // Enemy type 1（穴で反転）
                Enemy e2 ({ px, py, GlobalConfig::TILE_SIZE, GlobalConfig::TILE_SIZE }, 1, GlobalConfig::LEFT, 100.0f);
                enemies.push_back(e2);
                break;
            }

            case 'C': {   // Enemy type 2（ジャンプ弾幕系など）
                Enemy e3 ({ px, py, GlobalConfig::TILE_SIZE, GlobalConfig::TILE_SIZE }, 2, GlobalConfig::LEFT,   0.0f);
                enemies.push_back(e3);
                break;
            }

                    //case 'D': {   // Enemy type 3（将来用）
                    //    Enemy e4({ px, py, Config::TILE_SIZE, Config::TILE_SIZE }, 3, -1, 1.0f);
                    //    enemies.push_back(e4);
                    //    break;
                    //}
 
            default:
                // 空白やその他の文字は無視
                break;
            }
        }
    }
}