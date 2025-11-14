#include "SceneManager.h"
#include "TitleScene.h"
#include "GameScene.h"
#include "GameOverScene.h"
#include "ClearScene.h"
#include "ReadyScene.h"
#include "Player.h"

// static 変数の実体定義
int SceneManager::resultScore = 0;

int SceneManager::timeBonus = 0;
int SceneManager::lifeBonus = 0;
int SceneManager::bodyBonus = 0;
int SceneManager::itemBonus = 0;


SceneManager::SceneManager() {
	currentScene = nullptr;
	sceneID = -1;
	playerLives = 3; // 初期残機数

	currentStage = 1;   // 最初のステージ設定
	
	ChangeScene(0);  // 最初はタイトル画面

	//printfDx("SceneManager 初期化完了\n");

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
	case 0:
		ResetStageAndLives();  // タイトルに戻るときはステージと残機初期化

		currentScene = new TitleScene();
		break;

	case 1:
	{
		auto* ready = new ReadyScene();
		ready->SetStage(currentStage);    // 現在のステージを渡す

		ready->SetLives(playerLives); // 残機を渡す
		currentScene = ready;
	}
		break;

	case 2:
		playerLives = 3;
		currentStage = 1; // ステージリセット
		currentScene = new GameOverScene();
		break;

	case 3:
	{
		//playerLives = 3;
		//currentScene = new ClearScene();
		auto* clearScene = new ClearScene(this);  // SceneManager ポインタを渡す

		clearScene->SetScore(resultScore);     //スコアを渡す
		
		clearScene->SetBonuses(timeBonus, lifeBonus, bodyBonus, itemBonus);
		
		currentScene = clearScene;             //現在のシーンに設定
	}
		break;

	case 4:
	{
		auto* game = new GameScene(currentStage); // ★引数でステージを指定
		//auto* game = new GameScene();

		game->SetLives(playerLives); // 残機を渡す
		currentScene = game;
	}
		break;

	default:
		currentScene = new TitleScene();
		break;
	}

	if (currentScene) currentScene->Init();
}

//void SceneManager::Update()
//{
//}

void SceneManager::Update(float deltaTime) {
	if (!currentScene) return;

	// deltaTimeを受け取れるシーンには渡す
	if (auto* game = dynamic_cast<GameScene*>(currentScene)) {
		game->SetDeltaTime(deltaTime);
	}

	// GameScene のような deltaTime を使うシーンに渡す
	currentScene->Update();  // 仮にSceneBaseがfloatを受け取らない場合はこのまま

	// シーン終了時、次のシーンに遷移
	if (currentScene->IsEnd()) {
		int next = currentScene->NextScene();

		// GameScene → プレイヤー死亡時の処理
		if (sceneID == 4 && next == 1) { // GameScene終了 → ReadyScene
			playerLives--; // 残機を減らす
			if (playerLives <= 0) {
				ChangeScene(2); // 残機0 → GameOver
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
	currentStage++;

	//3ステージまで
	if (currentStage > 3) {
		// 全部クリアしたらタイトルに戻す
		ChangeScene(0);
	}
	else {
		ChangeScene(1); // Ready → GameScene(currentStage)
	}
}
