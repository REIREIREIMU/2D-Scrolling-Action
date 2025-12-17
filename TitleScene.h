#pragma once
#include "SceneManager.h"
#include "SceneBase.h"
#include <DxLib.h>

// Title画面の設定
namespace TitleSceneConfig {
	const int   SCROLL_SPEED = 2;		// スクロール速度
	const int   WAIT_FRAMES  = 60;		// 60フレーム = 1秒
	const float ALPHA_MIN	 = 50.0f;	// 透明度(最低)
	const float ALPHA_MAX	 = 255.0f;  // 透明度(最大)
	const float ALPHA_STEP	 = 5.0f;	// 点滅速度
}

class TitleScene : public SceneBase {
private:
	bool endFlag  = false;
	bool quitFlag = false;    //終了用フラグ

	int backgroundImage = -1; // 背景画像のID
	int TitleImage		= -1; // タイトル画像のID
	int StartImage		= -1; // スタートボタン画像のID
	int Start_XboxImage = -1; // スタートボタン(コントローラ用)画像のID

	int waitTimer_ = 0;		  // 入力受付の待機タイマー（フレーム単位）
	
	// 背景スクロール用
	int scrollX = 0;									//X座標
	int scrollSpeed = TitleSceneConfig::SCROLL_SPEED;	//背景のスクロール速度

	// コントローラー接続フラグ
	bool controllerConnected = false;

	// 点滅演出
	float startAlpha_ =  TitleSceneConfig::ALPHA_MAX;   // スタート画像の透明度
	float alphaSpeed_ = -TitleSceneConfig::ALPHA_STEP;  // アルファ変化速度（点滅スピード）

public:
	void Init() override;
	void Update() override;
	void Draw() override;
	bool IsEnd() override;
	int NextScene() override;
};
