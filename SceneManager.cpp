#include "SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "GameOverScene.h"
#include "ClearScene.h"
#include "ReadyScene.h"

// static 変数の実体定義
int SceneManager::resultScore = 0;
int SceneManager::timeBonus	  = 0;
int SceneManager::lifeBonus   = 0;
int SceneManager::bodyBonus   = 0;
//int SceneManager::itemBonus   = 0;

SceneManager::SceneManager() {
	currentScene = nullptr;
	sceneID		 = -1;
	PlayerLives  = SceneFlowConfig::START_LIVES;    // 初期残機数
	CurrentStage = 1;								// 最初のステージ設定
	
	ChangeScene(0);		// 最初はタイトル画面
}

SceneManager::~SceneManager() {
	delete currentScene;
}

void SceneManager::ChangeScene(int id) {
	if (currentScene) {
		delete currentScene;
		currentScene = nullptr;
	}
	sceneID = id;

	switch (sceneID) {
	case (int)SceneState::Title_Scene:
	{
		ResetStageAndLives();  // タイトルに戻るときはステージと残機初期化
		currentScene = new TitleScene();
		break;
	}
	case (int)SceneState::Ready_Scene:
	{
		auto* ready = new ReadyScene();
		ready->SetStage(CurrentStage);    // 現在のステージを渡す
		ready->SetLives(PlayerLives);	  // 残機を渡す
		currentScene = ready;
		break;
	}
	case (int)SceneState::GameOver_Scene:
	{
		PlayerLives = SceneFlowConfig::START_LIVES;	//この時残機初期化
		CurrentStage = 1;							// ステージリセット
		currentScene = new GameOverScene();
		break;
	}
	case (int)SceneState::Clear_Scene:
	{
		auto* clear = new ClearScene(this);  // SceneManager ポインタを渡す
		clear->SetScore(resultScore);		 //スコアを渡す
		clear->SetBonuses(timeBonus,
						  lifeBonus,
					      bodyBonus
						  /*, itemBonus*/);
		currentScene = clear;				 //現在のシーンに設定
		break;
	}
	case (int)SceneState::Game_Scene:
	{
		auto* game = new GameScene(CurrentStage); // 引数でステージを指定
		game->SetLives(PlayerLives);			  // 残機を渡す
		currentScene = game;
		break;
	}
	default:
		currentScene = new TitleScene();
		break;
	}
	if (currentScene) currentScene->Init();
}

void SceneManager::Update(float deltaTime) {
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

		// GameScene → プレイヤー死亡時の処理
		if (sceneID == (int)SceneState::Game_Scene &&
			// GameScene終了 → ReadyScene
			next == (int)SceneState::Ready_Scene) { 
			PlayerLives--;										// 残機を減らす
			if (PlayerLives <= 0) {
				ChangeScene((int)SceneState::GameOver_Scene);	// 残機が無い → GameOver
				return;
			}
		}
		ChangeScene(next);
	}
}

void SceneManager::Draw() {
	if (!currentScene) return;
	currentScene->Draw();
}

SceneBase* SceneManager::GetCurrentScene() {
	return currentScene;
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
