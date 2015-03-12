#ifndef GameScene_h__
#define GameScene_h__

#include "cocos2d.h"
#include "Consts.h"
#include "Drop.h"
#include "ComboLabel.h"
#include "Party.h"
#include "EnemyParty.h"

USING_NS_CC;

namespace{
	// ゲーム状態種別
	enum class State
	{
		GAME_START,
		WAITING_INPUT,
		TOUCHING,
		ERASING,
		CHARGE_ATTACK_POWER,
		PARTY_ATTACK,
		AFTER_PARTY_ATTACK,
		ENEMY_ATTACK,
		AFTER_ENEMY_ATTACK,
		GAME_OVER,
		GAME_CLEAR,
	};
}

class GameScene : public Scene
{
public:
	//============================================================
	// public メンバー関数
	//============================================================
	static GameScene* create();
	bool init();
	void update(float delta) override;
	
	// 行,列から座標を計算し、返却する。
	// row : 行番号。画面下が0。
	// column : 列番号。画面左が0。
	static Vec2 calcDropPosition(const PositionIndex& positionIndex);

private:
	//============================================================
	//メンバー変数
	//============================================================

	// ドラッグ中の場所
	PositionIndex _touchingPositionIndex;
	
	// ゲームの状態
	State _state;

	// ドロップを移動可能な残り時間
	float _remainingTouchTime;
	
	// 全ドロップの配列
	Drop* _drops[ROW_COUNT][COLUMN_COUNT];

	// コンボ数
	int _comboCount;

	// コンボのラベルを詰めておくキュー
	std::queue<ComboLabel*> _comboLabels;

	// 味方パーティ
	Party* _party;

	// 敵パーティ
	EnemyParty* _enemyParty;

	// ドラッグ中のドロップ
	Drop* _draggingDrop;

	// ドラッグ中のドロップ（表示用）
	Sprite* _draggingDropSprite;

	//============================================================
	// private メンバー関数
	//============================================================

	void changeState(State newState);

	// タッチイベントを登録する
	void registerTouchEvent();

	// 盤面の初期化。盤面をドロップで満たす
	void initDrops();

	// 空いている場所にドロップを追加する
	void fillEmpty();

	// ドロップを落として空いてるマスを詰める
	void fallDrops();

	// パーティ初期化
	void initParty();
	
	// ドロップが消せるかチェックし、消せるなら消す。
	// 消した場合は、新しいドロップを出現させる。
	// そしてまた自身（matchTest）を呼ぶ。
	void matchTest();
	
	// 指定した座標に存在するドロップを取得する。
	// 指定した座標にドロップが存在しない場合はnullptrを返却する。
	Drop* getDrop(const Vec2& position);

	// 2つのドロップの位置を交換する
	void swapDrop(PositionIndex a, PositionIndex b);
	
	// 画面座標からPositionIndex（ドロップの行/列）を計算する
	// 座標が範囲外である場合はPositionIndex::INVALID_INDEXを返却する
	PositionIndex getPositionIndexFromPosition(const Vec2& position);

	// 画面座標からPositionIndex（ドロップの行/列）を計算する。
	// 座標が範囲外である場合は、補正して範囲内にしたものを返却する。
	PositionIndex getPositionIndexFromPositionClamp(const Vec2& position);

	// 複数のドロップの中心座標を計算し、返却する
	Vec2 calcCenterOfDrops(const std::vector<Drop*>& drops);

	// コンボ表示を生成し、SceneにAddする
	void createComboDisplay(const Vec2& position, int comboCount, float delay);

	// ドラッグ中のドロップの表示用スプライトを作成
	void createDraggingDropSpriteAndAdd(const Vec2& position);

	// ドロップ消去時の効果音を再生
	void playEraseDropSE(int comboCount);

	// 敵パーティ初期化
	void initEnemyParty();
};

#endif // GameScene_h__
