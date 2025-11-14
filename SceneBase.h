#pragma once

class SceneBase {
private:
protected:
public:
	//virtual ~SceneBase() = default;

	virtual ~SceneBase() {}
	virtual void Init() = 0;                    // 初期化
	virtual void Update(/*float deltaTime*/) = 0;   // ロジック更新
	virtual void Draw() = 0;                    // 描画
	virtual bool IsEnd() = 0;                   // 終了判定（画面遷移条件）
	virtual int NextScene() = 0;                // 次のシーンID
};
