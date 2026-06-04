#include "SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "GameOverScene.h"
#include "ClearScene.h"
#include "ReadyScene.h"
#include "SPScene.h"

// static 変数の実体定義
int SceneManager::resultScore = 0;
int SceneManager::timeBonus   = 0;
int SceneManager::lifeBonus   = 0;
int SceneManager::bodyBonus   = 0;
int SceneManager::SPBonus     = 0;
int SceneManager::itemCount   = 0;
bool SceneManager::isBonusClear = false;
int SceneManager::currentStageNo = 1;

namespace 
{
	int         g_currentStage = 1;
	ResetReason g_resetReason = ResetReason::None;
}

SceneManager::SceneManager() {
	currentScene = nullptr;
	sceneID      = -1;
	PlayerLives  = SceneFlowConfig::START_LIVES; // 初期残機数
	CurrentStage = 1; // 最初のステージ設定
	ChangeScene(0);   // 最初はタイトル画面
}

SceneManager::~SceneManager() {
	delete currentScene;
}

// =========================================
// シーン切り替え処理
// 現在のシーンを破棄し、新しいシーンを生成する
// =========================================
void SceneManager::ChangeScene(int id)
{
	// 既存シーンを削除
	if (currentScene) {
		delete currentScene;
		currentScene = nullptr;
	}

	sceneID = id;

	switch (sceneID) {
	// =========================================
	// タイトル
	// =========================================
	case (int)SceneState::Title_Scene:
	{
		ResetStageAndLives(); // タイトルに戻るときはステージと残機初期化
		currentScene = new TitleScene();
		break;
	}
	// =========================================
	// 準備画面
	// =========================================
	case (int)SceneState::Ready_Scene:
	{
		auto* ready = new ReadyScene();
		ready->SetStage(CurrentStage); // 現在のステージを渡す
		ready->SetLives(PlayerLives);  // 残機を渡す
		currentScene = ready; // 現在のシーンに設定
		break;
	}
	// =========================================
	// ゲームオーバー
	// =========================================
	case (int)SceneState::GameOver_Scene:
	{
	//	PlayerLives = SceneFlowConfig::START_LIVES; // この時残機初期化
	//	CurrentStage = 1;                           // ステージリセット
		currentScene = new GameOverScene();
		break;
	}
	// =========================================
	// クリア
	// =========================================
	case (int)SceneState::Clear_Scene:
	{
		auto* clear = new ClearScene(this); // SceneManager ポインタを渡す
		clear->SetScore(resultScore);       // スコアを渡す
		clear->SetBonuses(timeBonus, lifeBonus, bodyBonus, SPBonus);
		currentScene = clear; // 現在のシーンに設定
		break;
	}
	// =========================================
	// ボーナスステージ
	// =========================================
	case (int)SceneState::SP_Scene:
	{
		auto* SPclear = new SPScene(this);      // SceneManager ポインタを渡す
		SPclear->SPSetScore(SPBonus,itemCount); //スコアを渡す
		currentScene = SPclear; //現在のシーンに設定
		break;
	}
	// =========================================
	// ゲーム本編
	// =========================================
	case (int)SceneState::Game_Scene:
	{
		// 引数でステージを指定
		auto* game = new GameScene(CurrentStage,isBonusClear);
		// 残機を渡す
		game->SetLives(PlayerLives);
		// 純スコア・ボーナスステージスコアを渡す
		game->SetScore(resultScore,SPBonus);

		// 前シーンからのデータ引き継ぎ
		if (GetCarryOverScore() >= 0) {
			game->SetScore(GetCarryOverScore(), SPBonus);
			ClearCarryOverScore();
		}

		if (GetCarryOverTimeSec() >= 0)
		{
			game->SetCarryOverTime(GetCarryOverTimeSec());
			ClearCarryOverTime();
		}
		currentScene = game;
		break;
	}
	default:
		currentScene = new TitleScene();
		break;
	}
	// 初期化処理
	if (currentScene) currentScene->Init();
}

// =========================================
// 更新処理
// シーン更新 + 遷移判定
// =========================================
void SceneManager::Update(float deltaTime)
{
	if (!currentScene) return;

	// deltaTimeを受け取れるシーンには渡す
	if (auto* game = dynamic_cast<GameScene*>(currentScene)) {
		game->SetDeltaTime(deltaTime);
	}

	// GameScene のような deltaTime を使うシーンに渡す
	currentScene->Update();

	// シーン終了時、次のシーンに遷移
	if (currentScene->IsEnd()) {

		int next = currentScene->NextScene();

		// GameScene → SP_Scene に遷移する直前に残り時間を退避
		if (sceneID == (int)SceneState::Game_Scene && next == (int)SceneState::SP_Scene)
		{
			if (auto* game = dynamic_cast<GameScene*>(currentScene))
			{
				// 後述 2) で GameScene に Getter を追加します
				SetCarryOverTimeSec((int)game->GetTimeLimitSec());
				SetCarryOverScore(game->GetCurrentScore());
			}
		}

		// =========================================
		// プレイヤー死亡処理
		// =========================================
		if (sceneID == (int)SceneState::Game_Scene && next == (int)SceneState::Ready_Scene) { 
			PlayerLives--; // 残機を減らす
			if (PlayerLives <= 0) {
				ChangeScene((int)SceneState::GameOver_Scene); // 残機が無い → GameOver
				return;
			}
		}
		ChangeScene(next);
	}
}

void SceneManager::SetStage(int stage)
{
	currentStageNo = stage;
}

int SceneManager::GetStage()
{
	return currentStageNo;
}

void SceneManager::Draw() {
	if (!currentScene) return;
	currentScene->Draw();
}

SceneBase* SceneManager::GetCurrentScene() 
{
	return currentScene;
}

void SceneManager::RequestReset(ResetReason reason) 
{
	g_resetReason = reason;
}
ResetReason SceneManager::ConsumeResetReason() 
{
	const ResetReason r = g_resetReason;
	g_resetReason = ResetReason::None;   // 一度だけ有効
	return r;
}

void SceneManager::NextStage()
{
	CurrentStage++;
	//最大ステージまで行けたら
	if (CurrentStage > SceneFlowConfig::MAX_STAGE) {
		// 全部クリアしたらタイトルに戻す
		ChangeScene((int)SceneState::Title_Scene);
	}
	else {
		// そうでなかったら準備画面に戻す
		ChangeScene((int)SceneState::Ready_Scene);
	}
}
