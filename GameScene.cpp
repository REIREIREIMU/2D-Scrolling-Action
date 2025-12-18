#include "GameScene.h"
#include "ClearScene.h"
#include "DxLib.h"
#include "SceneManager.h"
#include "Input.h"
#include <algorithm>

// アイテム種類をランダムで返す
int GetRandomItemType() { return GetRand(4); }  // 0〜4 のアイテム番号

// 体型ごとの評価スコア
int GetFatScore(FatState fat) {
	switch (fat) {
	case FatState::Thin:
	case FatState::SlightlyThin:
	case FatState::Normal:		return GameConfig::BODY_SCORE_NORMAL;
	case FatState::SlightlyFat: return GameConfig::BODY_SCORE_SLIGHTLY_FAT;
	case FatState::Fat1:		return GameConfig::BODY_SCORE_FAT_1;
	case FatState::Fat2:		return GameConfig::BODY_SCORE_FAT_2;
	case FatState::Fat3:		return GameConfig::BODY_SCORE_FAT_3;
	case FatState::Fat4:		return GameConfig::BODY_SCORE_FAT_4;
	}
	return 0;
}

/*
   マップ記号の例
   '0'～'4': アイテム
   'w''=':   地面ブロック
   '?':      ?ブロック
   '#':      レンガブロック
   'A'～'C': 敵
   'P':      プレイヤー初期位置
*/

extern const std::vector<std::string> mapText_stage1 = {

	//βステージ1(難易度調整板)
	 "                                                                                                                                                                                                                                                                                           ",
	 "                                                                                                                                                                                                                                                                                           ",
	 "                                                                                                                                                                                                                                                                                           ",
	 "                                                                                                                                                                                                                                                                                           ",
	 "                                                                                                                                                                                                                                                                                           ",
	 "                                                                                                                                                                                                                                                                                           ",
	 "                                                                                                                                                                                                                                                                                      $    ",
	 "                            2                                                                                              2  1  3  2                                                                                      C                                            1             $    ",
	 "              1            3 1                                      3    4                                                ###########?                                                          1 2                      ?###?                                     2                  $    ",
	 "                                                                                                                                                        4       5                                                                            2                3            C          $    ",
	 "                                                               1              5                                 ?    ?                                                                                                                                   4                ##          $    ",
	 "                                                     5                                        2                                                             C            2                                              B     B                                  B   ##   ##          $    ",
	 "            #?#?#          ###                    ?#####?             //                   1     4            1  2  3  4             ##?##                 ###           ####?       3 1       ?????          2   3   ?#########?                               ##   ##   ##          $    ",
	 "                      //         //                                   //                                     /##########/                            /             /               /#####/               //                               //   /           ##   ##   ##   ##          $    ",
	 "      　　          　//         //                         //        //        //                          //          //                          //             //             //#####//              //                              ///   //     ##   ##   ##   ##   ##          $    ",
	 "     P                //    4    //               A         //        //    A   //   B                     ///          ///        A   B   A       ///      3      ///     A     ///#####///       A     //           A    A    A       //// C ///    ##   ##      A    A    A       ///   ",
	 "wwwwwwwwwwwwwwwwwwwwwwwwwww   wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww     wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww###wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww                 wAw",//Aタイプの敵は最後の個体を倒すと警告文が出るためここに置く
	 "===========================   ==============================================================     ==========================================================   ==============================================================================================================================                 ==="
	 //0   P     1         2         3         4         5         6         7         8         9         A         B         C         D         E         F        10        11        12        13        14        15        16        17        18        19        1A        1B        1C        1D        1E        1F
};

extern const std::vector<std::string> mapText_stage2 = {
	//βステージ2
	"                                                                                                                                                                                                                             ",
	"                                                                                                                                                                                                                             ",
	"                                                                                                                                                                                                                             ",
	"                                                                                                                                                                                                                             ",
	"                                                                                                                                                                                                                             ",
	"                                                                                                                                                                                                                             ",
	"                                                                                                                                                         ??                                                            $     ",
	"                 B                                                                                                                                                                                                     $     ",
	"               ?###?            1   5                                                                                                                                                                                  $     ",
	"                                                                                             2      1            1       2                             1    2                                           1              $     ",
	"                                                                                                                                                       ######                                                          $     ",
	"                                                  1     4                   5                                                                   B      ######   1              1    2    4                             $     ",
	"           ?###?   ?###?    3    / /             ###   ###               ?#####?                    B          ?###?   ?###?                 #######           ###     ?     /////     /////   2 3 1   ?       ?##     $     ",
	"                               / / / /     /                                              /#####//#####/                         /     ###   #######    1    4 ###                                    /         ##     $     ",
	"                             / / / / /     /                   //                        //     //     //                       //     ###        ##   ########                                      //         ##     $     ",
	"     P                     / / / / / /  A  /  B   4  C  5   B  //        A  A  A        ///     //     //            C         ///     ### 1    2 ## C ########     B           B   C   B           ///    A    ##    ///    ",
	"wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww wwwwwwwwwwwwww###www###wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww     ww     wwwwwwwwwwww#####wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww   wwwwwwwwwwwwwww   wwwwwwwwwwwwwwwwwwwwwwwwwwwwww                   wAw",//Aタイプの敵は最後の個体を倒すと警告文が出るためここに置く
	"================================== ==============   ===   =================================     ==     ============     ==================================================   ===============   ==============================                   ==="
	//0   P     1         2         3         4         5         6         7         8         9         A         B         C         D         E         F        10        11        12        13        14        15        16        17        18        19        1A        1B        1C        1D        1E        1F    
};

extern const std::vector<std::string> mapText_stage3 = {
	//βステージ3
	"                                                                                                                                                                                                                                                                             ",
	"                                                                                                                                                                                                                                                                             ",
	"                                                    1   3   4   4   1   2   5                                                                                                                                                                                                ",
	"                                            ?       /###/###/###/###/###/###/                                                                                                                                                                                                ",
	"                             1                                                                               ? ? ?                                                                                                                                                           ",
	"                                                                                                                                                                                                                                                                             ",
	"                                                              2                                                                                                               2                                                                                        $     ",
	"                                           ?#?                                                                                                                          1    5 3    4                                  C                                               $     ",
	"                            www                                           ?                                 ?#####?                                                                                                  ?###?                                  2 B 1      $     ",
	"                            ===                                   1                                                                                                                                                                                         /////      $     ",
	"                    ?        =              B           wwww                                                                              4      2                                                   3                                   1      1   2                  $     ",
	"                                       5  wwwww  2      ====              B          1   5   3              B     B                                                   ?###?       ?###?                   1         B     B         3           wwwww                  $     ",
	"            /                        wwwww=====wwwww              C     wwwww                            #wwwwwwwwwww                                    ?##?                                                  3  ?#########?  4                =====                  $     ",
	"     P     //                        ===============             www    =====                       www### ==========wwww                                                                       www                                     www      ===                   $     ",
	"wwwwwwwwww///     wwwww      B        ===       ===              ===     ===                     www===#                =wwww         ww     ww     ww                  B     B     B           ===wwwww  B                            w===                 #####      $     ",
	"==========www     =====     www                                                   wwwwwwwwwwwwwww======                  ====wwww    w==w C w==w C w==w            wwwwwwwwwwwwwwwwwwwwwww       =======wwwww     A    A    A     wwwww====                           ///    ",
	"=============      ===      ===                                                   ======================= 1 2A3 4A5 =============wwww====www====www====wwwwwwww    =======================          =========wwwwwwwwwwwwwwwwwwwww=========             C             wwwwwwww                        wAw",//Aタイプの敵は最後の個体を倒すと警告文が出るためここに置く
	"=============                =                                                    =============================================================================     =====================                ==================================             #             ========                        ==="
	//0   P     1         2         3         4         5         6         7         8         9         A         B         C         D         E         F        10        11        12        13        14        15        16        17        18        19        1A        1B        1C        1D        1E        1F
};

void GameScene::Init() {
	// ステージごとにmapTextを切り替える
	const std::vector<std::string>* stageMap = nullptr;

	if		(stageNo == 1) stageMap = &mapText_stage1;
	else if (stageNo == 2) stageMap = &mapText_stage2;
	else if (stageNo == 3) stageMap = &mapText_stage3;
	
	// ブロック画像の読み込み
	blockImages[(int)BlockType::GroundA]        = LoadGraph("image/GroundA.png");
	blockImages[(int)BlockType::GroundB]        = LoadGraph("image/GroundB.png");
	blockImages[(int)BlockType::Brick]          = LoadGraph("image/Block.png");
	blockImages[(int)BlockType::Question]       = LoadGraph("image/Question_Block_1.png");
	blockImages[(int)BlockType::Question_Empty] = LoadGraph("image/Question_Empty.png");
	blockImages[(int)BlockType::Goal]			= LoadGraph("image/Goal.png");

	// ？ブロック用は3枚の連番画像を読み込む
	int questionImgs[3] = {
		LoadGraph("image/Question_Block_1.png"),
		LoadGraph("image/Question_Block_2.png"),
		LoadGraph("image/Question_Block_3.png")
	};
	Block::SetQuestionBlockImages(questionImgs, 3);

	// サウンドの読み込み
	Eat_Sound		 = LoadSoundMem("sound/Eat.mp3");
	BreakBrick_Sound = LoadSoundMem("sound/BreakBrick.mp3");
	Activate_Sound	 = LoadSoundMem("sound/Activate.mp3");
	Kill_Sound_1	 = LoadSoundMem("sound/collapse.mp3");
	Kill_Sound_2	 = LoadSoundMem("sound/collapse.mp3");
	Main_Bgm		 = LoadSoundMem("sound/MainBGM.mp3");

	// BGMを再生
	PlaySoundMem(Main_Bgm, DX_PLAYTYPE_LOOP);
	ChangeVolumeSoundMem(soundvolume_, Main_Bgm);

	// マップ読み込み
	map.LoadFromString(*stageMap, blockImages);

	// ブロック配置（プレイヤーより先）
	{
		blocks = map.blocks;    // 壁のオブジェクトリスト

		// レンガ破片画像を読み込み
		int brickPieceImages[2] = {
			LoadGraph("image/brick_piece1.png"),
			LoadGraph("image/brick_piece2.png")
		};
		Block::SetBrickPieceImages(brickPieceImages, 2);
	}

	// プレイヤー初期化
	player.Init(map.playerStart);
	player.SetBlockImages(blockImages);

	items = map.items;      // アイテムのオブジェクトリスト
	enemies = map.enemies;  // 敵のオブジェクトリスト
	itemCollected = std::vector<bool>(items.size(), false);

	enemyImages =       // 敵の描画ファイルの読み込み
	{ 
		 LoadGraph("image/Enemy1_Walk_L.png"),
		 LoadGraph("image/Enemy1_Walk_R.png"),
		 LoadGraph("image/Enemy2_Walk_L.png"),
		 LoadGraph("image/Enemy2_Walk_R.png"),
		 LoadGraph("image/Enemy3_Normal.png"),
		 LoadGraph("image/Enemy3_Ready.png"),
		 LoadGraph("image/Enemy3_Attack.png"),
		 LoadGraph("image/Enemy4.png")
	};

	itemImages = {       // アイテムがの描画ファイルの読み込み
	   LoadGraph("image/Food1.png"),
	   LoadGraph("image/Food2.png"),
	   LoadGraph("image/Food3.png"),
	   LoadGraph("image/Food4.png"),
	   LoadGraph("image/Food5.png")
	};

	// ステージの描画ファイルの読み込み
	backgroundImage = LoadGraph("image/background.png");

	//player.SetBlockImages(blockImages);

	// 死亡演出（爆発スプライト）
	deathEffectImg = LoadGraph("image/Explosion.png");// 爆発画像1枚 or アニメシート
	
	// 爆発スプライトのコマサイズを取得
	int imgW, imgH;
	GetGraphSize(deathEffectImg, &imgW, &imgH);
	deathEffectCols = GameConfig::DEATH_COLS;    // 横3コマ
	deathEffectRows = GameConfig::DEATH_ROWS;    // 縦4コマ
	deathEffectFrameW = imgW / deathEffectCols;
	deathEffectFrameH = imgH / deathEffectRows;

	// 死亡音
	Death_Sound = LoadSoundMem("sound/Explosion.mp3");
	
	// 敵ごとにアニメ画像設定
	for (auto& enemy : enemies) {
		if (enemy.type == 0) {
			enemy.SetAnimImages({ enemyImages[0], enemyImages[1] });  // Enemy1
		}
		else if (enemy.type == 1) {
			enemy.SetAnimImages({ enemyImages[2], enemyImages[3] });  // Enemy2
		}
		else if (enemy.type == 2) {
			// type 2 は個別画像をセット
			enemy.enemy3NormalImg_ = enemyImages[4];  // Enemy3_Normal.png
			enemy.enemy3ReadyImg_  = enemyImages[5];  // Enemy3_Ready.png
			enemy.enemy3AttackImg_ = enemyImages[6];  // Enemy3_Attack.png
		}
	}
	//UI関係
	UI_Score		= LoadGraph("image/Score.png");		    //スコアの画像
	UI_Timer		= LoadGraph("image/Timer.png");		    //タイマーの画像
	UI_Player_Lives = LoadGraph("image/Player_Lives.png");	//残機表示の画像
}

// SceneBase から呼ばれる Update（引数なし）
void GameScene::Update() {
	Update(deltaTimeForUpdate); // 内部で渡す
}

// deltaTime付きの本来のUpdate
void GameScene::Update(/*float*/ double deltaTime) {

	// deltaTime をセット
	deltaTimeForUpdate = (float)deltaTime;

	// 制限時間を減少
	timeLimit -= deltaTime;
	// 時間が 0になるとの死亡
	if (timeLimit <= 0) {
		player.isDead = true;
		timeLimit = 0;
	}

	// プレイヤーの更新
	player.Update(blocks, deltaTime);

	// プレイヤーの矩形を取得（敵に渡す）
	Rect playerRect = player.GetRect();

	// 死亡後、2秒経過でゲームオーバーへ
	if (player.IsDead()) {

		//BGM止める
		StopSoundMem(Main_Bgm);

		// エフェクト開始（1回だけ）
		if (!isDeathEffectActive) {
			isDeathEffectActive = true;

			deathEffectFrame = 0;
			effectX = playerRect.x + playerRect.w / GlobalConfig::Break_Number - deathEffectFrameW / GlobalConfig::Break_Number;
			effectY = playerRect.y + playerRect.h / GlobalConfig::Break_Number - deathEffectFrameH / GlobalConfig::Break_Number;

			//死亡効果音を再生
			PlaySoundMem(Death_Sound, DX_PLAYTYPE_BACK);
			ChangeVolumeSoundMem(soundvolume_, Death_Sound);
		}

		// エフェクト進行
		int totalFrames = deathEffectCols * deathEffectRows; // 9コマ
		if (deathEffectFrame < totalFrames * GameConfig::DEATH_FRAMES_X5 - 1) {
			deathEffectFrame++;
		}

		//一定時間待つ
		if (player.deathTimer > GameConfig::AUTO_WAIT_TIME) {
			player.LoseLife();			  // 残機を減らす
			if (player.GetLives() > 0) {  // 残機がある場合
				nextSceneID = (int)SceneState::Ready_Scene;     // 準備画面 に遷移
			}
			else {						  // 残機がない場合
				nextSceneID = (int)SceneState::GameOver_Scene;  // ゲームオーバー に遷移		
			}
			endFlag = true;
		}
	}

	//敵の更新（移動処理）
	for (auto& enemy : enemies) {
		//画面内の敵だけ更新
		bool isOnScreen =
			enemy.rect.x + enemy.rect.w > scrollX &&
			enemy.rect.x < scrollX + GlobalConfig::SCREEN_WIDTH &&
			enemy.rect.y + enemy.rect.h > 0 &&
			enemy.rect.y < GlobalConfig::SCREEN_HEIGHT;

		if (!isOnScreen && enemy.type != 2) continue;

		enemy.Update(blocks, enemies, playerRect, scrollX, deltaTime);

		// 敵弾との接触
		for (const auto& bullet : enemy.GetBullets()) {
			if (playerRect.Intersects(bullet.GetRect())) {
				player.isDead = true;
			}
		}
	}
	
	Rect headRect    = playerRect;   // プレイヤーの頭部矩形を作る
	     headRect.h  = Head_R_h;     // 頭の高さは小さめ
	     headRect.y -= Head_R_y;     // 頭の上に少しオフセット

	//ブロック更新と判定
	for (auto& block : blocks) {
		//各ブロックの内部更新
		block.Update(deltaTimeForUpdate);

		//衝突のあるブロックのみ処理
		if (!block.HasCollision()) continue;

		//上向きジャンプ中に？ブロックと接触
		if (headRect.Intersects(block.GetRect()) &&
			block.GetType() == BlockType::Question &&
			!block.IsActivated())
		{
			block.Activate();

			// ブロックの種類を地面Bに変更
			block.SetType(BlockType::Question_Empty);
			block.SetImage(blockImages[(int)BlockType::Question_Empty]);

			// アイテムを出す
			items.push_back({ { block.GetRect().x, block.GetRect().y - block.GetRect().h, block.GetRect().w, block.GetRect().h }, GetRandomItemType() });
			itemCollected.push_back(false);

			//木箱破壊時の効果音を再生
			PlaySoundMem(Activate_Sound, DX_PLAYTYPE_BACK);
			ChangeVolumeSoundMem(soundvolume_,Activate_Sound);
		}

		// レンガブロック破壊判定
		FatState fat = player.GetFatState();

		//肥満体型をboolで一括り
		bool isFatEnough = (fat == FatState::Fat1 || fat == FatState::Fat2|| 
							fat == FatState::Fat3 || fat == FatState::Fat4);
		
		Rect footRect    = playerRect;		// プレイヤーの頭位置の矩形
		     footRect.y += playerRect.h;	// 足元をわずかに下にオフセット
		     footRect.h  = Foot_R_h;	    // 足の高さ（ヒット判定用）

		if (isFatEnough && block.GetType() == BlockType::Brick)
		{
			// レンガブロックの上を歩いて or 上から乗っかったで破壊(予備)
			if (footRect.Intersects(block.GetRect()) ||
				playerRect.Intersects(block.GetRect()))
			{
				block.BreakBrick();
				score += GameConfig::BREAK_SCORE; // スコア加算

				//レンガ破壊時の効果音を再生
				PlaySoundMem(BreakBrick_Sound, DX_PLAYTYPE_BACK);
				ChangeVolumeSoundMem(soundvolume_, BreakBrick_Sound);
			}
		}
	}

	// アイテム取得処理
	for (size_t i = 0; i < items.size(); ++i) {
		if (!itemCollected[i] && CheckCollision(playerRect, items[i].rect)) {
			itemCollected[i] = true;
			player.Grow(blocks);
			score += GameConfig::ITEM_SCORE; // スコア加算

			//取得効果音を再生
			PlaySoundMem(Eat_Sound, DX_PLAYTYPE_BACK);
			ChangeVolumeSoundMem(soundvolume_, Eat_Sound);
		}
	}

	// スクロール位置更新（プレイヤー中心）
	scrollX = player.GetX() - GlobalConfig::SCREEN_WIDTH / GlobalConfig::Break_Number;
	if (scrollX < 0) scrollX = 0;

	// 敵との当たり判定
	bool stomped = false;     //このフレームで1回でも踏んだらtrueにする

	// アイテムとの当たり判定などで使用
	for (size_t i = 0; i < enemies.size();)
	{
		const Rect& enemyRect = enemies[i].rect;

		// プレイヤーの底と敵の上辺の差
		int playerBottom = playerRect.y + playerRect.h;

		// 敵の頭（上辺）のY座標
		int enemyTop = enemyRect.y;

		// 上から踏んでいる判定（プレイヤーの足部分が敵の頭付近に位置）
		bool isVerticallyAbove =
			playerBottom > enemyTop &&               // プレイヤーが敵より下に入り込んでいて
			playerBottom < enemyTop + pixel;         // (22px) 以内なら踏んだと判定

		// 横方向の重なりチェック
		bool isHorizontallyOverlapping =
			playerRect.x + playerRect.w > enemyRect.x &&
			playerRect.x < enemyRect.x + enemyRect.w;

		// 敵の上に乗って倒す（Enemy1 or Enemy2 or Enemy3限定）
		if ((enemies[i].type == 0 || enemies[i].type == 1 || enemies[i].type == 2) &&
			isHorizontallyOverlapping && isVerticallyAbove)
		{
			// 消す前に型を保存しておく（erase 後に参照しないように）
			int killedType = enemies[i].type;

			enemies.erase(enemies.begin() + i);	// 敵消滅
			player.Bounce();					// 敵を踏んだらプレイヤーが跳ねる
			stomped = true;						// 踏んだら true
			score += GameConfig::ENEMY_SCORE;	// スコア加算

			// 音の再生は保存した killedType を使う
			if (killedType == 0 || killedType == 1)
			{
				PlaySoundMem(Kill_Sound_1, DX_PLAYTYPE_BACK);
				ChangeVolumeSoundMem(soundvolume_, Kill_Sound_1);
			}
			else if (killedType == 2)
			{
				PlaySoundMem(Kill_Sound_2, DX_PLAYTYPE_BACK);
				ChangeVolumeSoundMem(soundvolume_, Kill_Sound_2);
			}
			// erase したので i は増やさない（continueして次ループへ）
			continue;
		}

		// まだ踏んでいなければ、衝突判定（プレイヤー死亡）
		if (!stomped && CheckCollision(playerRect, enemyRect)) {
			player.isDead = true;
		}
		++i;
	}

	// ゴール判定
	for (const auto& block : blocks) {
		if (block.GetType() == BlockType::Goal &&
			playerRect.Intersects(block.GetRect())) {
			if (!isGoalFade) {			// 初回だけ処理
				StopSoundMem(Main_Bgm);

				// ゲーム中のスコア
				SceneManager::SetResultScore(score);
				// 残り時間ボーナス
				SceneManager::SetTimeBonus((int)timeLimit * ClearConfig::TIME_ADD_PER_STEP);
				// 残機ボーナス
				SceneManager::SetLifeBonus(playerLives * ClearConfig::LIFE_ADD_SCORE);
				// 体型評価ボーナス
				SceneManager::SetBodyBonus(GetFatScore(player.GetFatState()));
				//ClearSceneに渡すスコアをSceneManagerに保存
				SceneManager::SetResultScore(score);

				isGoalFade = true;   // フェード開始
				fadeAlpha = 0;
			}
		}
	}

	// フェード処理
	if (isGoalFade) {

		player.SetInvincible(true);
		player.SetControllable(false);

		fadeAlpha += GameConfig::FADE_STEP; // 1フレームで暗くなる量（調整可）
		if (fadeAlpha >= GameConfig::ALPHA) {
			fadeAlpha = GameConfig::ALPHA;
			endFlag = true;
			// クリアシーン に遷移
			nextSceneID = (int)SceneState::Clear_Scene;
		}
	}

	// ESCで強制終了
	if (CheckHitKey(KEY_INPUT_ESCAPE)) {
		StopSoundMem(Main_Bgm);
		endFlag = true;
		// ゲームオーバー　に遷移
		nextSceneID = (int)SceneState::GameOver_Scene;
	}
}

void GameScene::Draw() {

	// 背景描画
	if (backgroundImage >= 0) {
		DrawBox(0, 0, GlobalConfig::SCREEN_WIDTH, GlobalConfig::SCREEN_HEIGHT, ColorConfig::Light_Cyan, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, GameConfig::ALPHA_CONSTANT);

		// 背景をスクロールに応じて描画（ループ表示例）
		int bgWidth, bgHeight;
		GetGraphSize(backgroundImage, &bgWidth, &bgHeight);
		int bgX = -(int(scrollX) % bgWidth);
		for (int x = bgX; x < GlobalConfig::SCREEN_WIDTH; x += bgWidth) {
			DrawGraph(x, 0, backgroundImage, TRUE);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 敵描画
	for (const auto& enemy : enemies) {
		bool isOnScreen =
			enemy.rect.x + enemy.rect.w > scrollX &&
			enemy.rect.x < scrollX + GlobalConfig::SCREEN_WIDTH &&
			enemy.rect.y + enemy.rect.h > 0 &&
			enemy.rect.y < GlobalConfig::SCREEN_HEIGHT;
		if (!isOnScreen && enemy.type != 2) continue;
		enemy.Draw(scrollX, enemyImages[enemy.type]);
	}

	// ブロック描画
	for (const auto& block : blocks) { block.Draw(scrollX); }

	// アイテム描画
	for (size_t i = 0; i < items.size(); ++i) {
		if (!itemCollected[i]) {
			const auto& item = items[i];
			DrawExtendGraph( item.rect.x - scrollX, item.rect.y,
							 item.rect.x + item.rect.w - scrollX, item.rect.y + item.rect.h,
							 itemImages[item.type], TRUE);
		}
	}

	// プレイヤーが生きていれば描画（当たり判定と一致させる）
	if (!player.IsDead()) { player.Draw(scrollX); }

	// フェードアウト描画
	if (isGoalFade) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, fadeAlpha);
		DrawBox(0, 0, GlobalConfig::SCREEN_WIDTH, GlobalConfig::SCREEN_HEIGHT, ColorConfig::Black, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// 死亡演出
	if (isDeathEffectActive) {
		// 1フレームごとにアニメーション進行
		int totalFrames  = deathEffectCols * deathEffectRows;								// 9コマ
		int currentFrame = (deathEffectFrame / GameConfig::DEATH_FRAMES_X5) % totalFrames;  // 5フレームごと切替
		int srcX = (currentFrame % deathEffectCols) * deathEffectFrameW;
		int srcY = (currentFrame / deathEffectCols) * deathEffectFrameH;
		DrawRectGraph(
			effectX - scrollX, effectY,
			srcX, srcY,
			deathEffectFrameW, deathEffectFrameH,
			deathEffectImg, TRUE
		);
	}

	SetFontSize(GameConfig::UI_FONT_SCORE);
	DrawFormatString( x_a, y_a, ColorConfig::White, " %06d", score);
	
	SetFontSize(GameConfig::UI_FONT_TIMER);
	DrawFormatString( x_b, y_b, ColorConfig::Yellow, " %d", (int)timeLimit);
	
	DrawFormatString( x_c, y_c, ColorConfig::Green, "×%d", playerLives);
	
	SetFontSize(GameConfig::FONT_SIZE);
	DrawGraph(0, 0, UI_Score, TRUE);		// スコア表示
	DrawGraph(0, 0, UI_Player_Lives, TRUE);	// 残機表示
	DrawGraph(0, 0, UI_Timer, TRUE);		// タイマー表示
}

bool GameScene::IsEnd() { return endFlag; }

int GameScene::NextScene() { return nextSceneID; } // nextSceneID を返す

void GameScene::SetDeltaTime(float dt) { deltaTimeForUpdate = dt; }

bool GameScene::CheckCollision(const Rect& a, const Rect& b) {
	return !(a.x + a.w <= b.x ||
		a.x >= b.x + b.w ||
		a.y + a.h <= b.y ||
		a.y >= b.y + b.h);
}   