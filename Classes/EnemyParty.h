#ifndef EnemyParty_h__
#define EnemyParty_h__

#include "cocos2d.h"
#include "Enemy.h"

USING_NS_CC;

class Party;

class EnemyParty : public Node
{
public:
	//============================================================
	// public メンバー関数
	//============================================================

	// インスタンス生成
	static EnemyParty* create();
	bool init();

	// 生きているすべての敵を返却する。
	const std::vector<Enemy*> getActiveEnemies();

	// ターンを進める（攻撃までの残りターン数を減らす）
	void incrementTurn();

	// 各メンバーの攻撃。
	// 攻撃までの残りターン数がゼロのモンスターに攻撃をさせる。
	// onComplete : 敵の攻撃完了時に実施する処理を渡す。
	void attack(Party* party, std::function<void()> onComplete);

	// 全滅したか？
	bool isWipedOut();

private:
	//============================================================
	//メンバー変数
	//============================================================

	std::vector<Enemy*> _enemies;

	//============================================================
	// private メンバー関数
	//============================================================

	void alignCenter();

};


#endif // EnemyParty_h__
