#pragma once
#include "SceneBase.h"


enum class ResetReason
{
	None,
	StageAdvance,
	NewGame
};

// シーン遷移/スコア共有の設定
namespace SceneFlowConfig {
	const int START_LIVES = 3; // 初期残機
	const int MAX_STAGE   = 3; // ステージ最大数
}




// 各シーンのステータスを定義
enum class SceneState {
	Title_Scene,     // 0: タイトル画面
	Ready_Scene,     // 1: 準備画面
	GameOver_Scene,  // 2: ゲームオーバー画面
	Clear_Scene,     // 3: クリア画面
	Game_Scene,      // 4: プレイ画面
	SP_Scene         // 5: ステージ2限定のスペシャルシーン（例：ミニゲームやボーナスステージ）
};

class SceneManager {
private:
	SceneBase* currentScene = nullptr;
	static int resultScore;   // クリア後に渡す純スコア(ボーナスなどが加えられたいないスコア)
	static int timeBonus;	  // タイムボーナス
	static int lifeBonus;	  // 残機ボーナス
	static int bodyBonus;	  // 体型ボーナス
	static int SPBonus;	  // ステージ2限定のスペシャルボーナス
	static int itemCount;//ボーナスステージのアイテム取得カウント
	static bool isBonusClear;//ボーナスステージから戻った時の座標を決めるために使うk
	//static int itemBonus;	  // アイテムボーナス
	int carryOverTimeSec = -1;// タイムボーナス計算用の残り時間（秒単位）
	int carryOverScore = -1;// タイムボーナス計算用のスコア（必要なら）



	int	 sceneID      = -1;
	int  CurrentStage =  1;   // 現在のステージ番号を追加 (1,2,3など)
	int  PlayerLives  = SceneFlowConfig::START_LIVES;

public:
	SceneManager();
	~SceneManager();

	void ChangeScene(int id);
	void Update(float deltaTime);
	void Draw();
	SceneBase* GetCurrentScene();


	static void        RequestReset(ResetReason reason);
	static ResetReason ConsumeResetReason();


	// 残り時間の退避＆取得＆クリア
	void SetCarryOverTimeSec(int sec) { carryOverTimeSec = sec; }//	GameScene から残り時間を受け取るための Setter
	int  GetCarryOverTimeSec() const { return carryOverTimeSec; }//	GameScene から受け取った残り時間を SPScene で利用するための Getter
	void ClearCarryOverTime() { carryOverTimeSec = -1; }




	void SetCarryOverScore(int sc) { carryOverScore = sc; }// GameScene からスコアを受け取るための Setter
	int  GetCarryOverScore() const { return carryOverScore; }// GameScene から受け取ったスコアを SPScene で利用するための Getter
	void ClearCarryOverScore() { carryOverScore = -1; }// GameScene から受け取ったスコアをクリアするためのメソッド（SPScene で呼び出す想定）




	//次のステージへ
	void NextStage();
	int  GetCurrentStage() const { return CurrentStage; }
	void IncrementStage()		 { CurrentStage++; }						// 異なるステージでも残機は維持
	int  GetMaxStage() const	 { return SceneFlowConfig::MAX_STAGE; }
	void ResetStageAndLives()	 { CurrentStage = 1;
								   PlayerLives  = SceneFlowConfig::START_LIVES; }
	
	static void SetResultScore(int s) { resultScore = s; }		// 純スコアをセット
	static int GetResultScore()		  { return resultScore; }	// 純スコアの受け渡し

	static void SetTimeBonus(int t)   { timeBonus = t; }		// タイムボーナスをセット
	static int GetTimeBonus()		  { return timeBonus; }		// タイムボーナスの受け渡し
	
	static void SetLifeBonus(int l)   { lifeBonus = l; }		// 残機ボーナスをセット
	static int GetLifeBonus()		  { return lifeBonus; }		// 残機の受け渡し
	
	static void SetBodyBonus(int b)   { bodyBonus = b; }		// 体型ボーナスをセット
	static int GetBodyBonus()		  { return bodyBonus; }		// 体型の受け渡し

	static void SetSPScore(int s)	  { SPBonus = s; }			// ステージ2限定のスコアをセット
	static int GetSPScore()			  { return SPBonus; }		// ステージ2限定のスコアの受け渡し

	static void SetItemCount(int s)   { itemCount = s; }
	static int GetItemCount()		  { return itemCount; }

	static void SetBonusClear(bool c) { isBonusClear = c; }
	static bool GetBonusClear()       { return isBonusClear; }


	//static void SetItemBonus(int i) { itemBonus = i; }		// アイテムボーナスをセット
	//static int GetItemBonus()		  { return itemBonus; }		// アイテムの受け渡し

};
