#pragma once
#include "SceneBase.h"

class TitleScene : public SceneBase {
private:
	bool endFlag = false;
	int backgroundImage = -1; // 背景画像のID
	int TitleImage = -1;	  // タイトル画像のID
	int StartImage = -1;	  // スタートボタン画像のID
	int Start_XboxImage = -1; // スタートボタン(コントローラ用)画像のID

	int waitTimer_ = 0; // 入力受付の待機タイマー（フレーム単位）
	bool quitFlag = false;      //終了用フラグ

	// 背景スクロール用
	int scrollX = 0;
	int scrollSpeed = 2;        // 背景スクロール速度

	// コントローラー接続フラグ
	bool controllerConnected = false;

	float startAlpha_ = 255.0f; // スタート画像の透明度
	float alphaSpeed_ = -10.0f;  // アルファ変化速度（点滅スピード）

protected:
public:
	void Init() override;
	void Update(/*float deltaTime*/) override;
	void Draw() override;
	bool IsEnd() override;
	int NextScene() override;

};
