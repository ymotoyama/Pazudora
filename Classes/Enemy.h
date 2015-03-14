#ifndef Enemy_h__
#define Enemy_h__

#include "cocos2d.h"
#include "Attribute.h"

USING_NS_CC;

// 敵種別
enum class EnemyKind
{
	ENEMY1,
	ENEMY2,
	ENEMY3,
};

// 循環してるので前方宣言
class Party;
class PartyMember;

// 敵クラス
class Enemy : public Node
{
public:

	//============================================================
	// public メンバー関数
	//============================================================

	// インスタンス生成
	static Enemy* create(EnemyKind kind);

	// 初期化
	bool init(EnemyKind kind);

	// 受けるダメージを計算する
	int calcDamage(PartyMember* partyMember);

	// 受けるダメージを計算する
	int calcDamage(int attackPower, Attribute attribute);

	// 不透明度変更（HPゲージの不透明度変更のため）
	void setOpacity(GLubyte opacity) override;

	// ダメージを与える
	void applyDamage(int damage);

	// ターンを進める（攻撃までの残りターン数を減らす）
	void incrementTurn();

	// 属性を返却する。
	Attribute getAttribute();

	// 現在のHPを返却する。
	int getLife();

	// 最大HPを返却する
	int getMaxLife();

	// 指定した属性に弱いか？（ダメージ倍増）
	bool isValnerableTo(Attribute attribute);

	// 指定した属性に強いか？（ダメージ半減）
	bool hasResistanceTo(Attribute attribute);

	// 生きているか？
	bool isAlive();

	// 攻撃までの残りターン数を返却する。
	int getRemainingTurnForAttack();

	// プレイヤーを攻撃する
	void attack(Party* party);

	// 攻撃までの残りターン数をリセットする
	void resetRemainingTurnForAttack();

private:
	//============================================================
	//メンバー変数
	//============================================================

	// 最大ライフ（HP初期値）
	int _maxLife;
	// ライフ（HP）
	int _life;
	// ゲージに表示されているライフ量
	int _displayedLife;
	// 攻撃力
	int _attackPower;
	// 属性
	Attribute _attribute;
	// 攻撃間隔（ターン数）
	int _attackInterval;
	// 攻撃までの残りターン数
	int _remainingTurnForAttack;
	// 攻撃までの残りターン数ラベル
	Label* _remainingTurnForAttackLabel;
	// ライフゲージ
	ProgressTimer* _lifeGauge;

	//============================================================
	// private メンバー関数
	//============================================================

	// 攻撃までの残りターン数ラベルを準備する
	void initRemainingTurnForAttackLabel();

	// 攻撃までの残りターン数ラベルを更新する
	void updateRemainingTurnForAttackLabel();
	
	// ライフゲージを準備する
	void initLifeGauge(float width);

	// ライフゲージ更新
	void updateLifeGauge();

	// 死亡時の処理
	void die();
};

#endif // Enemy_h__
