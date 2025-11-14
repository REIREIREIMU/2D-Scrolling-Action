#pragma once
//#include <vector>
//#include <memory>
#include "SceneBase.h"

class SceneManager {
private:
	//std::vector<std::unique_ptr<SceneBase>> scenes;

	SceneBase* currentScene = nullptr;
	static int resultScore; // クリア後に渡すスコア

	int sceneID;

	int currentStage;   // 現在のステージ番号を追加 (1,2,3など)

	int playerLives;
	//SceneBase* currentScene = nullptr;

	static int timeBonus;
	static int lifeBonus;
	static int bodyBonus;
	static int itemBonus; // 将来用

protected:
public:
	SceneManager();
	~SceneManager();
	void ChangeScene(int id);
	//void Update();
	void Update(float deltaTime);
	void Draw();
	SceneBase* GetCurrentScene();

	//次のステージへ
	void NextStage();

	// スコアの受け渡し
	static void SetResultScore(int score) { resultScore = score; }
	static int GetResultScore() { return resultScore; }

	static void SetTimeBonus(int t) { timeBonus = t; }
	static int GetTimeBonus() { return timeBonus; }

	static void SetLifeBonus(int l) { lifeBonus = l; }
	static int GetLifeBonus() { return lifeBonus; }

	static void SetBodyBonus(int b) { bodyBonus = b; }
	static int GetBodyBonus() { return bodyBonus; }

	static void SetItemBonus(int i) { itemBonus = i; }
	static int GetItemBonus() { return itemBonus; }

	// 追加
	int GetCurrentStage() const { return currentStage; }
	void IncrementStage() { currentStage++; } // 残機は維持
	int GetMaxStage() const { return 3; }

	void ResetStageAndLives() { currentStage = 1; playerLives = 3; }
};
