#include "GameScene.h"
#include "ClearScene.h"
#include "DxLib.h"
#include "SceneManager.h"
#include "Input.h"
#include <algorithm>
#include "CsvReader.h"
#include "MapData.h"
#include "Player.h"
#include "SPScene.h"

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
	 "     P                //    4    //              A         //        //    A   //   B                     ///          ///        A   B   A       ///      3      ///     A     ///#####///       A     //           A    A    A       //// C ///    ##   ##      A    A    A       ///   ",
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

	blockImages[(int)BlockType::GroundA] = LoadGraph("image/GroundA.png");
	blockImages[(int)BlockType::GroundB] = LoadGraph("image/GroundB.png");
	blockImages[(int)BlockType::GroundC] = LoadGraph("image/GroundC.png");
	blockImages[(int)BlockType::Brick] = LoadGraph("image/Block.png");
	blockImages[(int)BlockType::Question] = LoadGraph("image/Question_Block_1.png");
	blockImages[(int)BlockType::Question_Empty] = LoadGraph("image/Question_Empty.png");
	blockImages[(int)BlockType::Goal] = LoadGraph("image/Goal.png");
	blockImages[(int)BlockType::Ladder] = LoadGraph("image/Ladder.png");
	blockImages[(int)BlockType::FallBrick] = LoadGraph("image/Fall_Brick.png");

	// ？ブロック用は3枚の連番画像を読み込む
	int questionImgs[3] = {
		LoadGraph("image/Question_Block_1.png"),
		LoadGraph("image/Question_Block_2.png"),
		LoadGraph("image/Question_Block_3.png")
	};
	Block::SetQuestionBlockImages(questionImgs, 3);



	if (brickPieceImages[0] == -1)
	{
		brickPieceImages[0] = LoadGraph("image/brick_piece1.png");
		brickPieceImages[1] = LoadGraph("image/brick_piece2.png");
	}



	// サウンドの読み込み
	Eat_Sound = LoadSoundMem("sound/Eat.mp3");
	BreakBrick_Sound = LoadSoundMem("sound/BreakBrick.mp3");
	Activate_Sound = LoadSoundMem("sound/Activate.mp3");
	Kill_Sound_1 = LoadSoundMem("sound/collapse.mp3");
	Kill_Sound_2 = LoadSoundMem("sound/collapse.mp3");
	Main_Bgm = LoadSoundMem("sound/MainBGM.mp3");
	

	// ★BGMは毎回再生OK
	PlaySoundMem(Main_Bgm, DX_PLAYTYPE_LOOP);
	ChangeVolumeSoundMem(soundvolume_, Main_Bgm);

	// ★ステージは毎回初期化OK
	if (stageNo == 1 && isMap5Clear)
	{
		StageSet(map.returnPoint);
	}
	else
	{
		StageSet(map.playerStart);
	}

	// ★タイマー復元（そのままでOK）
	if (carryOverTimePending_ >= 0)
	{
		timeLimit = (double)carryOverTimePending_;
		carryOverTimePending_ = -1;
	}

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
	backgroundImage = LoadGraph("image/NewBackGround.png");
	SPUI_Image = LoadGraph("image/SP_UI.png");            // SP スコア表示用の UI 画像（必要なら）
	bonusBackGroundImg = LoadGraph("image/BonusBackGround.png");
	SP_BonusStart_Image = LoadGraph("image/BonusStart.png");
	NotFrool_Image = LoadGraph("image/NotFrool.png");
	Warning_Sound = LoadSoundMem("sound/Warning.mp3");
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
			enemy.enemy3ReadyImg_ = enemyImages[5];  // Enemy3_Ready.png
			enemy.enemy3AttackImg_ = enemyImages[6];  // Enemy3_Attack.png
		}
	}

	//UI関係
	UI_Score = LoadGraph("image/Score.png");		    //スコアの画像
	UI_Score_SP = -1;										// 「SPScore」ラベル画像用
	UI_Timer = LoadGraph("image/Timer.png");		    //タイマーの画像
	UI_Player_Lives = LoadGraph("image/Player_Lives.png");	//残機表示の画像
	UI_WARNING_BD = LoadGraph("image/WARNING.png");
	UI_WARNING = LoadGraph("image/Danger.png");

	UI_Thin = LoadGraph("image/Thin.png");			//Thinの画像
	UI_SlightlyThin = LoadGraph("image/SlightlyThin.png");	//SlightlyThin画像
	UI_Normal = LoadGraph("image/Normal.png");		//Normalの画像
	UI_SlightlyFat = LoadGraph("image/SlightlyFat.png");	//SlightlyFatの画像


}


void GameScene::StageSet(const Rect &position)
{


	const char* stageMap = nullptr;//ファイルパス用
	//文字列からCSV形式に変更
	//(もともとの位置だと常にロードされてマップの切り替えができなかった)
	if (stageNo == 1) stageMap = "map/mapData1.csv";
	else if (stageNo == 2) stageMap = "map/mapData2.csv";
	else if (stageNo == 3) stageMap = "map/mapData3.csv";
	else (int)SceneState::Title_Scene;
	
	
	
	
	if (stageNo == 5)
	{
		stageMap = "map/mapDataSP.csv";//SPステージのマップ

	}


	//対応したマップ読み込み




	if (lastLoadedStageNo_ != stageNo)
	{
		map.LoadMapFromCsv(stageMap, blockImages);
		lastLoadedStageNo_ = stageNo;
	}

		blocks = map.blocks;
		items = map.items;
		enemies = map.enemies;

		itemCollected = std::vector<bool>(items.size(), false);

		fallPointTriggers = map.fallPointTriggers;
		fallTriggers = map.fallTriggers;
		UpTriggers = map.UpTriggers;

		lastLoadedStageNo_ = stageNo;

	Block::SetBrickPieceImages(brickPieceImages.data(), 2);

	// プレイヤー初期化
	player.Init(position);
	player.SetBlockImages(blockImages);

	items = map.items;      // アイテムのオブジェクトリスト
	enemies = map.enemies;  // 敵のオブジェクトリスト
	itemCollected = std::vector<bool>(items.size(), false);

	fallPointTriggers = map.fallPointTriggers;//Hの位置受け取り
	fallTriggers = map.fallTriggers;//Lの位置受け取り
	UpTriggers = map.UpTriggers;//Uの位置受け取り

	for (auto& block : blocks)
	{
		block.SetImage(blockImages[(int)block.GetType()]);
		block.SetBlockImages(blockImages);
	}

}



void GameScene::SetDeltaTime(float dt) 
{
	
	deltaTimeForUpdate = dt;

}



// SceneBase から呼ばれる Update（引数なし）
void GameScene::Update()
{
	Update(deltaTimeForUpdate);
	
	//経過時間を進める（SceneManager->SetDeltaTime で受け取った dt が入っている前提）
	elapsedSec += deltaTimeForUpdate;



	//Update(deltaTimeForUpdate); // 内部で渡す
	// --- ステージ2固定（10秒間）を監視し、期間内は毎フレーム 体型3 を保証 ---
	if (stageNo == 5 && stage5TimeSec >= 0.0f)
	{
		const float t = elapsedSec - stage5TimeSec;

		if (t >= 0.0f && t < STAGE5_SteatTime)
		{


			//10秒以内 → 体型3を毎フレーム再適用（保険）
			if (player.GetFatState() != FatState::Normal)
			{
				player.SetWidthAndFix(PlayerConfig::WIDTH_NORMAL, blocks);
			}


		}
		else if (t >= STAGE5_SteatTime)
		{

			// ★ 次回のために差分基準化フラグを落とす
			spCountActive = false;


		}
	}

	else
	{
		// ステージ2以外、または監視無効時は安全のためフラグを落とす
		spCountActive = false;
	}
	showNotFool_ = false; // 毎フレーム初期化（デフォルト非表示）

	{
		const FatState fat = player.GetFatState();
		const bool isEnough = (fat == FatState::Fat3 || fat == FatState::Fat4);

		// 足元で接触判定
		Rect playerRect = player.GetRect();
		Rect footRect = playerRect;
		footRect.y += playerRect.h - 1;
		footRect.h = 2;                 

		for (const auto& trigger : fallPointTriggers)
		{
			if (footRect.Intersects(trigger))
			{
				// Fat3/Fat4は非表示、それ以外は表示
				showNotFool_ = !isEnough;
				break;
			}
		}

	

		for (const auto& trigger : fallTriggers)//Lから触れた処理を受け取ったとき
		{
			if (player.GetRect().Intersects(trigger))//プレイヤーの当たり判定とLの位置が重なったとき
			{
				stageNo = 5;//落下先のマップ
				StageSet(map.playerStart);//マップをロードする


				player.SetWidthAndFix(PlayerConfig::WIDTH_FAT_1, blocks);//体系を3に固定する


				// ★ BonusStart（イントロ）開始
				spIntroActive_ = true;
				spIntroRemain_ = SP_INTRO_SEC;
				if (SP_BonusStart_Image < 0) {
					SP_BonusStart_Image = LoadGraph("image/BonusStart.png"); // 画像がある場合
				}
				player.SetControllable(false);     // 入力禁止
				player.SetInvincible(true);        // 念のため無敵

				// ★ ここでは SP タイマーを開始しない（本開始はイントロ終了時）
				// spTimerActive_ = true;                   ← 消す
				// spTimeRemain_  = STAGE5_SteatTime;       ← 消す

				//isMap5Start = true;                // 背景やUIはSPモードに
				//return;

				// ここから10秒間固定カウント開始
				stage5TimeSec = elapsedSec;

				// --- ★ SPタイマー開始（見た目用） ---
				//spTimerActive_ = true;
				//spTimeRemain_ = STAGE5_SteatTime;   // 10.0f（既存の定数）

				// ★ ここでSP専用UIを遅延ロード（未ロード時だけ）
				if (SPUI_Image < 0)
				{
					//SPUI_Image = LoadGraph("image/SP_UI.png");
				}

				isMap5Start = true;
				return;
			}
		}

		showNotFool_ = false; // 毎フレーム初期化（デフォルト非表示）
		{

			Rect playerRect = player.GetRect();
			Rect footRect = playerRect;
			footRect.y += playerRect.h - 1;
			footRect.h = 2;

			for (const auto& trigger : fallTriggers) // L
			{
				if (footRect.Intersects(trigger)
					&& spEntryArmed_            // ← H で武装済み
					&& !isMap5Start             // ← 既にSP中ではない
					&& stageNo != 5)            // ← 念のため
				{
					stageNo = 5;                       // SPマップへ
					StageSet(map.playerStart);         // マップロード

					// SP入場時のSPカウントを初期化
					SPitemGetCount = 0;

					// 体系を固定したい幅に
					player.SetWidthAndFix(PlayerConfig::WIDTH_FAT_1, blocks);

					// イントロ開始
					spIntroActive_ = true;
					spIntroRemain_ = SP_INTRO_SEC;
					if (SP_BonusStart_Image < 0) {
						SP_BonusStart_Image = LoadGraph("image/BonusStart.png");
					}
					player.SetControllable(false);
					player.SetInvincible(true);

					// 入場時刻を記録
					stage5TimeSec = elapsedSec;

					isMap5Start = true;

					// 再入場防止
					spEntryArmed_ = false;
					spArmTimeout_ = 0.0f;

					return;
				}
			}


			bool onH = false;
			for (const auto& trigger : fallPointTriggers) // H
			{
				if (footRect.Intersects(trigger))
				{
					// Fat3/Fat4は非表示、それ以外は警告
					showNotFool_ = !isEnough;
					onH = true;
					break;
				}
			}

			//Hに触れているかつ特定の体系なら移動
			if (onH && isEnough)
			{
				spEntryArmed_ = true;
				spArmTimeout_ = SP_ARM_WINDOW_SEC;
			}
			else
			{
				// 離れている間は猶予を減らす／切れたら解除
				if (spArmTimeout_ > 0.0f)
				{
					spArmTimeout_ -= deltaTimeForUpdate;
					if (spArmTimeout_ <= 0.0f)
					{
						spArmTimeout_ = 0.0f;
						spEntryArmed_ = false;
					}
				}
				else
				{
					spEntryArmed_ = false;
				}
			}
		}

		for (const auto& trigger : UpTriggers)//Uから触れた処理を受け取ったとき
		{

			if (player.GetRect().Intersects(trigger))
			{
				isMap5Clear = true;
				SceneManager::SetSPScore(SPscore);
				SceneManager::SetItemCount(SPitemGetCount);
				SceneManager::SetBonusClear(isMap5Clear);
				StopSoundMem(Main_Bgm);
				endFlag = true;
				nextSceneID = (int)SceneState::SP_Scene;
			}
			
		}
		

		



	}
}

// deltaTime付きの本来のUpdate
void GameScene::Update(/*float*/ double deltaTime) {



	// ===== SPイントロの進行（BonusStart）=====
	if (spIntroActive_) {
		spIntroRemain_ -= (float)deltaTime;
		if (spIntroRemain_ <= 0.0f) {
			// イントロ終了 → SP本開始
			spIntroActive_ = false;
			spTimerActive_ = true;
			spTimeRemain_ = STAGE5_SteatTime;  // 例：10.0f
			player.SetControllable(true);
			player.SetInvincible(false);
		}
		else {
			// イントロ中はゲーム進行を止める（描画だけ行う）
			return;
		}
	}


	// deltaTime をセット
	deltaTimeForUpdate = (float)deltaTime;



// 制限時間（通常）を減少：SP中は止める
	if (!isMap5Start) {
		timeLimit -= deltaTime;
		if (timeLimit <= 0) {
			player.isDead = true;
			timeLimit = 0;
		}
	}


	//SP専用タイマーを動かす
	if (spTimerActive_) {
		spTimeRemain_ -= (float)deltaTime;
		if (spTimeRemain_ < 0.0f) spTimeRemain_ = 0.0f;
	}

	if (spTimerActive_ && spTimeRemain_ <= 0.0f && isMap5Start && !isMap5Clear)
	{
		isMap5Clear = true;
		SceneManager::SetSPScore(SPscore);
		SceneManager::SetItemCount(SPitemGetCount);
		SceneManager::SetBonusClear(isMap5Clear);
		StopSoundMem(Main_Bgm);
		endFlag = true;
		nextSceneID = (int)SceneState::SP_Scene;
		return;
	}

		// --- ★ 通常タイマー停止／SPタイマー動作 ---

	if (!isMap5Start) { // ← SPでない通常プレイ中
		if (timeLimitRemainFrames_ > 0) {
			--timeLimitRemainFrames_;
		}
		if (timeLimitRemainFrames_ <= 0) {
			player.isDead = true; // 0 になったら死亡
		}
	}
	else
	{


	}


	// プレイヤーの更新
	player.Update(blocks, deltaTime);



	if (!isMap5Start) // ★ステージ5では処理しない
	{
		// ===== 瘦せ警告（点滅＋音トリガー）=====

		// Thin or SlightlyThin 判定
		bool isThin = (player.GetFatState() == FatState::Thin);

		showThinWarning_ = isThin;

		// 点滅処理
		static float warningTimer = 0.0f;
		warningTimer += (float)deltaTime;

		// 点滅切り替え
		bool blink = ((int)(warningTimer / PlayerConfig::THIN_BLINK_INT) % 2) == 0;

		if (!blink) showThinWarning_ = false;

		// 音（変化時だけ）
		if (isThin && !wasThinWarning_)
		{
			PlaySoundMem(Warning_Sound, DX_PLAYTYPE_BACK);
		}
		else if (!isThin && wasThinWarning_)
		{
			StopSoundMem(Warning_Sound);
		}

		wasThinWarning_ = isThin;
	}
	else
	{
		// ★ステージ5に入った瞬間にリセット（重要）
		if (wasThinWarning_)
		{
			StopSoundMem(Warning_Sound);
		}

		showThinWarning_ = false;
		wasThinWarning_ = false;
	}


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

				isMap5Clear = false;   // ← ★これ追加
				isMap5Start = false;

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
		bool FallBlock = (fat == FatState::Fat3 || fat == FatState::Fat4);//3以上で壊れる


		//肥満体型をboolで一括り
		bool isFatEnough = (fat == FatState::Fat1 || fat == FatState::Fat2|| 
							fat == FatState::Fat3 || fat == FatState::Fat4);
		
		Rect footRect    = playerRect;		// プレイヤーの頭位置の矩形
		     footRect.y += playerRect.h;	// 足元をわずかに下にオフセット
		     footRect.h  = Foot_R_h;	    // 足の高さ（ヒット判定用）


		

		if (FallBlock && block.GetType() == BlockType::FallBrick)
		{

		 // レンガブロックの上を歩いて or 上から乗っかったで破壊(予備)
			if (footRect.Intersects(block.GetRect()) ||
			 playerRect.Intersects(block.GetRect()))
			{
				block.BreakBrick();


				if (!isMap5Start) {
					score += GameConfig::BREAK_SCORE;    // 通常時のみ score に加算
				}
				else {
					// （必要なら）SPscore += GameConfig::BREAK_SCORE;
				}

				//score += GameConfig::BREAK_SCORE; // スコア加算

				 //レンガ破壊時の効果音を再生
				PlaySoundMem(BreakBrick_Sound, DX_PLAYTYPE_BACK);
				ChangeVolumeSoundMem(soundvolume_, BreakBrick_Sound);
			}
		}

		if (isFatEnough && block.GetType() == BlockType::Brick)
		{
			// レンガブロックの上を歩いて or 上から乗っかったで破壊(予備)
			if (footRect.Intersects(block.GetRect()) ||
				playerRect.Intersects(block.GetRect()))
			{
				block.BreakBrick();


				if (!isMap5Start) {
					score += GameConfig::BREAK_SCORE;    // 通常時のみ score に加算
				}
				else {
					// （必要なら）SPscore += GameConfig::BREAK_SCORE;
				}


				//score += GameConfig::BREAK_SCORE; // スコア加算

				//レンガ破壊時の効果音を再生
				PlaySoundMem(BreakBrick_Sound, DX_PLAYTYPE_BACK);
				ChangeVolumeSoundMem(soundvolume_, BreakBrick_Sound);
			}
		}

	}

	// アイテム取得処理
	for (size_t i = 0; i < items.size(); ++i) 
	{
		
		
		if (!itemCollected[i] && CheckCollision(playerRect, items[i].rect)) 
		{

			if (!itemCollected[i]) 
			{
				const auto& item = items[i];


			}



			if (!isMap5Start)
			{
				itemCollected[i] = true;
				player.Grow(blocks);
				score += GameConfig::ITEM_SCORE; // スコア加算
			}
			else
			{
				itemCollected[i] = true;
				//score += GameConfig::ITEM_SCORE; // スコア加算
				SPscore += GameConfig::ITEM_SCORE; // ボーナススコア加算
				SPitemGetCount++;
			}

			

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

			FatState fat = player.GetFatState();

			if (FatState::Normal < fat)
			{
				enemies.erase(enemies.begin() + i);	// 敵消滅
				player.Bounce();


				if (!isMap5Start) {
					score += GameConfig::ENEMY_SCORE;     // 通常時のみ score に加算
				}
				else {
					// （必要なら）SPscore += GameConfig::ENEMY_SCORE;
				}


				//score += GameConfig::ENEMY_SCORE;	// スコア加算
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
			else
			{
				player.Bounce();// 敵を踏んだらプレイヤーが跳ねる
			}
			
			stomped = true;// 踏んだら true
			
		}

		// まだ踏んでいなければ、衝突判定
		if (!stomped && CheckCollision(playerRect, enemyRect)) {
			int fat = (int)player.GetFatState();
			if (lastHitTime <= 0.0f)
			{
				if (fat == (int)FatState::Thin)
				{
					player.isDead = true;
				}
				else
				{
					fat--;
					player.SetFatState((FatState)fat, blocks);
					lastHitTime = damageCooldown;
				}
			}
		}
		++i;
	}

	//当たった際の無敵時間を計測
	if (lastHitTime > 0.0f)
	{
		lastHitTime -= deltaTime;
	}
	else if(lastHitTime<0.0f)
	{
		lastHitTime = 0.0f;
	}


	// 梯子判定
	bool onLadder = false;
	for (const auto& block : blocks) {
		if (block.GetType() != BlockType::Ladder) {
			continue;
		}
		if (playerRect.Intersects(block.GetRect())&&FatState::SlightlyFat>=player.GetFatState()) {
			onLadder = true;
			break;
		}
	}
	player.SetOnLadder(onLadder);


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
		if (fadeAlpha >= GlobalConfig::ALPHA) {
			fadeAlpha = GlobalConfig::ALPHA;
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

void GameScene::Draw() 
{

	// 背景描画
	if (backgroundImage >= 0&&!isMap5Start) 
	{
		DrawBox(0, 0, GlobalConfig::SCREEN_WIDTH, GlobalConfig::SCREEN_HEIGHT, ColorConfig::White, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, GlobalConfig::ALPHA_CONSTANT);

		// 背景をスクロールに応じて描画（ループ表示例）
		int bgWidth, bgHeight;
		GetGraphSize(backgroundImage, &bgWidth, &bgHeight);
		int bgX = -(int(scrollX) % bgWidth);
		for (int x = bgX; x < GlobalConfig::SCREEN_WIDTH; x += bgWidth) 
		{
			DrawGraph(x, 0, backgroundImage, TRUE);
		}

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
	else if(isMap5Start)
	{
		DrawGraph(0, 0, bonusBackGroundImg, TRUE);
	}


		





	// 敵描画
	for (const auto& enemy : enemies) {
		bool isOnScreen =
			enemy.rect.x + enemy.rect.w > scrollX &&
			enemy.rect.x < scrollX + GlobalConfig::SCREEN_WIDTH &&
			enemy.rect.y + enemy.rect.h > 0 &&
			enemy.rect.y < GlobalConfig::SCREEN_HEIGHT;
		if (!isOnScreen && enemy.type != 2) continue;

		if (enemy.type >= 0 && enemy.type < enemyImages.size())
		{
			enemy.Draw(scrollX, enemyImages[enemy.type]);
		}

	}

	// ブロック描画
	for (const auto& block : blocks) { block.Draw(scrollX); }

	// アイテム描画
	for (size_t i = 0; i < items.size(); ++i) 
	{
		if (i >= itemCollected.size()) break; // ★これ追加

		if (!itemCollected[i]) 
		{
			const auto& item = items[i];

			if (item.type >= 0 && item.type < itemImages.size()) // ★これも追加
			{
				DrawExtendGraph(
					item.rect.x - scrollX, item.rect.y,
					item.rect.x + item.rect.w - scrollX, item.rect.y + item.rect.h,
					itemImages[item.type], TRUE);
			}

		}
	}

	// プレイヤーが生きていれば描画（当たり判定と一致させる）

	if(!player.IsDead()) 
	{
		int hitTime = (int)(lastHitTime*10);
		if (hitTime  % 2 == 0)
		{
			player.Draw(scrollX);
		}
	
		player.DrawUI(scrollX);
	}





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



// === ここから UI 出力ブロック ===
	SetFontSize(GameConfig::UI_FONT_SCORE);

	// --- 1) ラベル画像の切り替え ---
	if (isMap5Start) 
	{
		if (UI_Score_SP >= 0) 
		{
			DrawGraph(0, 0, UI_Score_SP, TRUE);// SP中: SPScore.png を (0,0)
			//printfDx("LABEL: SPScore IMAGE DRAWN (SP中にSPラベルを表示)\n");
		}
		else 
		{
			// 画像が無いときは描かない（必要なら仮テキストをここで）
			// DrawString(x_a, y_a, "SPScore:", ColorConfig::White);
			//printfDx("LABEL: NONE (SP中だがSPScore画像なし → ラベル未表示)\n");
		}
	}
	else 
	{
		if (UI_Score >= 0) 
		{
			DrawGraph(0, 0, UI_Score, TRUE);// 通常: Score.png を (0,0)
			//printfDx("LABEL: Score IMAGE DRAWN (通常にScoreラベルを表示)\n");
		}
		else 
		{
			//printfDx("LABEL: NONE (通常だがScore画像なし → ラベル未表示)\n");
		}
	}






	// --- 2) 数値の描画 ---
	SetFontSize(GameConfig::UI_FONT_SCORE);
	if (isMap5Start) {
		// SP中に数値を出したいなら SPscore を描く
		DrawFormatString(x_a, y_a, ColorConfig::Cyan, " %06d", SPscore);
		//printfDx("VALUE: SPscore drawn = %d\n", SPscore);
	}
	else {
		// 通常は通常スコアを描く
		DrawFormatString(x_a, y_a, ColorConfig::White, " %06d", score + SPscore);
		//printfDx("VALUE: score drawn = %d\n", score+SPscore);
	}

	
	SetFontSize(GameConfig::FONT_SIZE);
	//DrawGraph(0, 0, UI_Score, TRUE);		// スコア表示
	//DrawGraph(0, 0, UI_Player_Lives, TRUE);	// 残機表示
	
	


	//printfDx("=== 画像ロードチェック ===\n");

	//// Block画像
	//printfDx("GroundA: %d\n", blockImages[(int)BlockType::GroundA]);
	//printfDx("GroundB: %d\n", blockImages[(int)BlockType::GroundB]);
	//printfDx("GroundC: %d\n", blockImages[(int)BlockType::GroundC]);
	//printfDx("Brick: %d\n", blockImages[(int)BlockType::Brick]);
	//printfDx("Question: %d\n", blockImages[(int)BlockType::Question]);
	//printfDx("Question_Empty: %d\n", blockImages[(int)BlockType::Question_Empty]);
	//printfDx("Goal: %d\n", blockImages[(int)BlockType::Goal]);
	//printfDx("Ladder: %d\n", blockImages[(int)BlockType::Ladder]);
	//printfDx("FallBrick: %d\n", blockImages[(int)BlockType::FallBrick]);


	//// 敵
	//for (int i = 0; i < enemyImages.size(); ++i)
	//{
	//	printfDx("Enemy[%d]: %d\n", i, enemyImages[i]);
	//}

	//// アイテム
	//for (int i = 0; i < itemImages.size(); ++i)
	//{
	//	printfDx("Item[%d]: %d\n", i, itemImages[i]);
	//}

	//// UI
	//printfDx("UI_Score: %d\n", UI_Score);
	//printfDx("UI_Timer: %d\n", UI_Timer);
	//printfDx("UI_Lives: %d\n", UI_Player_Lives);
	//printfDx("UI_WARNING_BD: %d\n", UI_WARNING_BD);
	//printfDx("UI_WARNING: %d\n", UI_WARNING);

	//printfDx("UI_Thin: %d\n", UI_Thin);
	//printfDx("UI_SlightlyThin: %d\n", UI_SlightlyThin);
	//printfDx("UI_Normal: %d\n", UI_Normal);
	//printfDx("UI_SlightlyFat: %d\n", UI_SlightlyFat);

	//// 背景
	//printfDx("Background: %d\n", backgroundImage);
	//printfDx("SP_UI: %d\n", SPUI_Image);
	//printfDx("BonusBG: %d\n", bonusBackGroundImg);
	//printfDx("BonusStart: %d\n", SP_BonusStart_Image);
	//printfDx("NotFrool: %d\n", NotFrool_Image);


	//static int loadCount = 0;
	//loadCount++;

	//printfDx("LoadGraph呼ばれた回数: %d\n", loadCount);



	if (isMap5Start && SPUI_Image >= 0) {
		// ★ SPステージ中のみSP用UIパネルを描画（座標は例。必要に応じて調整）
		DrawGraph(0, 0, SPUI_Image, TRUE);
	}

	// ===== 最前面：SPUI(SP中のみ)=====
	if (isMap5Start && SPUI_Image >= 0 && !spIntroActive_) {
		DrawGraph(0, 0, SPUI_Image, TRUE);
	}

	SetFontSize(GameConfig::UI_Player_Lives);
	if (!isMap5Start)
	{
		if (UI_Player_Lives >= 0) DrawGraph(0, 0, UI_Player_Lives, TRUE);
		DrawFormatString(x_c, y_c, ColorConfig::White, "×%d", playerLives);
	}


	DrawGraph(0, 0, UI_Timer, TRUE);		// タイマー表示
	if (!isMap5Start)
	{
		DrawFormatString(x_b, y_b, ColorConfig::White, " %d", (int)timeLimit);
	}
	else
	{
		DrawFormatString(x_b, y_b, ColorConfig::White, " %d", (int)(spTimeRemain_ + 0.999f));
	}



	//プレイヤー体系別
	SetFontSize(UI_Thin);
	if (!isMap5Start) // ステージ5ではない
	{
		if (player.GetFatState() == FatState::Thin)
		{
			if (UI_Thin >= 0) DrawGraph(0, 0, UI_Thin, TRUE);
		}
		
		//DrawFormatString(x_c, y_c, ColorConfig::White, "×%d", UI_Thin);
	}
	else
	{
		
	}


	SetFontSize(UI_SlightlyThin);
	if (!isMap5Start)//ステージ5ではない
	{
		if ((player.GetFatState() == FatState::SlightlyThin))
		{
			if (UI_SlightlyThin >= 0) DrawGraph(0, 0, UI_SlightlyThin, TRUE);
		}
		
		//DrawFormatString(x_c, y_c, ColorConfig::White, "×%d", UI_SlightlyThin );
	}
	else
	{
		
	}

	SetFontSize(UI_Normal);
	if (!isMap5Start )//ステージ5ではない
	{
		if (player.GetFatState() == FatState::Normal)
		{
			if (UI_Normal >= 0) DrawGraph(0, 0, UI_Normal, TRUE);
		}
		
		//DrawFormatString(x_c, y_c, ColorConfig::White, "×%d", playerLives);
	}
	else
	{
		
	}

	SetFontSize(UI_SlightlyFat);
	if (!isMap5Start)//ステージ5ではない
	{
		if ((player.GetFatState() == FatState::SlightlyFat))
		{
			if (UI_SlightlyFat >= 0) DrawGraph(0, 0, UI_SlightlyFat, TRUE);
		}
		
		//DrawFormatString(x_c, y_c, ColorConfig::White, "×%d", playerLives);
	}
	else
	{
		
	}


	// ===== 最前面：BonusStart=====
	if (spIntroActive_) {
		// 半透明オーバレイ
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 160);
		DrawBox(0, 0, GlobalConfig::SCREEN_WIDTH, GlobalConfig::SCREEN_HEIGHT, GetColor(0, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		if (SP_BonusStart_Image >= 0) {
			// 画像がある場合は中央に
			int w = 0, h = 0;
			GetGraphSize(SP_BonusStart_Image, &w, &h);
			int cx = (GlobalConfig::SCREEN_WIDTH - w) / 2;
			int cy = (GlobalConfig::SCREEN_HEIGHT - h) / 2;
			DrawGraph(cx, cy, SP_BonusStart_Image, TRUE);
		}
		else {
			// 画像が無い場合はテキストで代用
			SetFontSize(72);
			const char* msg = "BONUS START!";
			int tw = GetDrawStringWidthToHandle(msg, strlen(msg), -1);
			int tx = (GlobalConfig::SCREEN_WIDTH - tw) / 2;
			int ty = (GlobalConfig::SCREEN_HEIGHT - 72) / 2;
			DrawString(tx, ty, msg, GetColor(255, 255, 0));
			SetFontSize(GameConfig::FONT_SIZE);
		}
	}


	if (showNotFool_) {
		if (NotFrool_Image >= 0) {
			int w = 0, h = 0;

			GetGraphSize(NotFrool_Image, &w, &h);
			const int cx = (GlobalConfig::SCREEN_WIDTH - w) / 2;
			const int cy = (GlobalConfig::SCREEN_HEIGHT - h) / 3;
			DrawGraph(cx, cy, NotFrool_Image, TRUE);
		}
		else {
			// 画像が無い場合
			SetFontSize(48);
			DrawFormatString(120, 60, ColorConfig::Black, "NotFool");
		}
	}


	// ★一番最後に追加
	if (showThinWarning_)
	{
		DrawGraph(0, 0, UI_WARNING_BD, TRUE);
		DrawGraph(0, 0, UI_WARNING, TRUE);
	}


}


bool GameScene::IsEnd()
{
	if (endFlag)
	{
		ReleaseResources();
	}
	return endFlag;
}

int GameScene::NextScene()
{ 

	if (endFlag)
	{
		ReleaseResources();
	}
	return nextSceneID;

} // nextSceneID を返す


bool GameScene::CheckCollision(const Rect& a, const Rect& b) {
	return !(a.x + a.w <= b.x ||
		a.x >= b.x + b.w ||
		a.y + a.h <= b.y ||
		a.y >= b.y + b.h);
}   


void GameScene::ReleaseResources()
{
	// --- blockImages ---
	for (int i = 0; i < (int)BlockType::Max; i++)
	{
		if (blockImages[i] != -1)
		{
			DeleteGraph(blockImages[i]);
			blockImages[i] = -1;
		}
	}

	// --- enemies ---
	for (auto& img : enemyImages)
	{
		if (img != -1)
		{
			DeleteGraph(img);
			img = -1;
		}
	}

	// --- items ---
	for (auto& img : itemImages)
	{
		if (img != -1)
		{
			DeleteGraph(img);
			img = -1;
		}
	}

	// --- UI ---
	if (UI_Score != -1) DeleteGraph(UI_Score);
	if (UI_Timer != -1) DeleteGraph(UI_Timer);
	if (UI_Player_Lives != -1) DeleteGraph(UI_Player_Lives);
	if (UI_WARNING_BD != -1) DeleteGraph(UI_WARNING_BD);
	if (UI_WARNING != -1) DeleteGraph(UI_WARNING);
	if (UI_Thin != -1) DeleteGraph(UI_Thin);
	if (UI_SlightlyThin != -1) DeleteGraph(UI_SlightlyThin);
	if (UI_Normal != -1) DeleteGraph(UI_Normal);
	if (UI_SlightlyFat != -1) DeleteGraph(UI_SlightlyFat);

	// --- 背景 ---
	if (backgroundImage != -1) DeleteGraph(backgroundImage);
	if (SPUI_Image != -1) DeleteGraph(SPUI_Image);
	if (bonusBackGroundImg != -1) DeleteGraph(bonusBackGroundImg);
	if (SP_BonusStart_Image != -1) DeleteGraph(SP_BonusStart_Image);
	if (NotFrool_Image != -1) DeleteGraph(NotFrool_Image);

	// --- エフェクト ---
	if (deathEffectImg != -1) DeleteGraph(deathEffectImg);

	// --- サウンド ---
	if (Eat_Sound != -1) DeleteSoundMem(Eat_Sound);
	if (BreakBrick_Sound != -1) DeleteSoundMem(BreakBrick_Sound);
	if (Activate_Sound != -1) DeleteSoundMem(Activate_Sound);
	if (Kill_Sound_1 != -1) DeleteSoundMem(Kill_Sound_1);
	if (Kill_Sound_2 != -1) DeleteSoundMem(Kill_Sound_2);
	if (Main_Bgm != -1) DeleteSoundMem(Main_Bgm);
	if (Warning_Sound != -1) DeleteSoundMem(Warning_Sound);
	if (Death_Sound != -1) DeleteSoundMem(Death_Sound);
}
