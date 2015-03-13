#ifndef Party_h__
#define Party_h__

#include "cocos2d.h"
USING_NS_CC;

#include "PartyMember.h"
#include "EnemyParty.h"
#include "Enemy.h"

class Party : public Node
{
private:
	// 敵の残りHP一時保存用テーブル。
	// 攻撃対象を決める時に使う。
	// key=敵, value=残りHP
	typedef std::map<Enemy*, int> EnemyLifeTable;

public:
	static Party* create();
	bool init();

	// ドロップ消去による攻撃力および回復量の加算
	// dropNum : 消したドロップの数
	// 3個消しの場合は、攻撃力の素点が加算される。
	// 4個消しの場合は、攻撃力の素点×1.25倍が加算される。
	// 5個消しの場合は、攻撃力の素点×1.5倍が加算される。
	// 6個消しの場合は、攻撃力の素点×1.75倍が加算される。
	// 以下同様。回復についても同様。
	void addPowerByEraseDrop(Attribute attribute, int dropNum);

	// コンボによる攻撃力の加算
	// 1コンボの攻撃力は1倍である。
	// 2コンボの場合は1.25倍、
	// 3コンボの場合は1.5倍、
	// 4コンボの場合は1.75倍…
	// 以下同様。回復についても同様。
	void addComboRate(float amount);

	// 各メンバーの蓄積された攻撃力を初期化する
	void initMembersAttackPower();

	// パーティにダメージを与える（食らう）
	void applyDamage(int damage);
	
	// ドロップ消去により、攻撃力（または回復力）がチャージされる時のエフェクトを発生させる
	void fireChargeEffect(const Vec2& startPosition, Attribute attribute);

	// 全メンバーを返却する
	const std::vector<PartyMember*> getMembers();

	// 味方パーティが敵パーティを攻撃する一連の流れを実施する。
	// （攻撃対象の決定、攻撃エフェクトの発生、ダメージ反映、敵消滅）
	// 戻り値として、全攻撃が完了するのに要する時間（秒）を返却する。
	float doAttackSequence(EnemyParty* enemyParty);

	// どの敵に攻撃を行えばよいかを決定し、攻撃対象の敵を返却する。
	// 以下、上にあるほど優先度高
	// ・一撃で倒せる高HPの敵
	// ・一撃で倒せる低HPの敵
	// ・一撃では倒せない低HPの敵
	// ・一撃では倒せない高HPの敵
	Enemy* decideAttackTarget(std::vector<Enemy*> enemies, EnemyLifeTable enemyLifeTable, PartyMember* member);

	// 生きているか？
	bool isAlive();

private:
	// パーティーメンバーの配列
	std::vector<PartyMember*> _members;
	// ライフゲージ
	ProgressTimer* _lifeGauge;
	int _maxLife;
	int _life;
	// 回復量素点
	int _curePowerBase;
	// 積み立てた回復量
	int _curePower;
	// 回復量コンボ率
	float _cureComboRate;
	// 回復量ラベル
	Label* _curePowerLabel;

	void loadMembers();
	// ライフゲージを更新する
	void updateLifeGauge(int newValue, int maxValue);
	void addAttackPowerByEraseDrop(Attribute attribute, int dropNum);
	void addCurePowerByEraseDrop(int dropNum);
	void addCurePowerRate(float amount);
	void initCurePowerLabel();
	void updateCurePowerLabel(int newValue);
	void initLifeGauge();
	int getCurePower();

	// 指定した敵を一撃で殺せるか？
	// 味方メンバーの累積攻撃力と属性などを考慮する。
	static bool canKill(Enemy* target, int remainingLife, PartyMember* member);
};

#endif // Party_h__
