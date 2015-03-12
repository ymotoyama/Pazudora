#ifndef PartyMember_h__
#define PartyMember_h__

#include "cocos2d.h"
USING_NS_CC;
#include "Enemy.h"

#include "Attribute.h"

class PartyMember : public Sprite
{
public:
	static PartyMember* create(std::string imageFileName, Attribute attribute, float attack, float cure);
	bool init(std::string imageFileName, Attribute attribute, float attack, float cure);
	Attribute getAttribute();
	int getCurePower();
	void initAttackPower();
	// ドロップ消去による攻撃力の加算
	// dropNum : 消したドロップの数
	// 3個消しの場合は、攻撃力の素点が加算される。
	// 4個消しの場合は、攻撃力の素点×1.25倍が加算される。
	// 5個消しの場合は、攻撃力の素点×1.5倍が加算される。
	// 6個消しの場合は、攻撃力の素点×1.75倍が加算される。
	// 以下同様
	void addAttackPowerByEraseDrop(int dropNum);
	// コンボによる攻撃力の加算
	// 1コンボの攻撃力は1倍である。
	// 2コンボの場合は1.25倍、
	// 3コンボの場合は1.5倍、
	// 4コンボの場合は1.75倍…
	// 以下同様
	void addAttackPowerComboRate(float amount);
	// 積み立てた攻撃力を返却する
	// （小数点以下繰り上げ）
	int getAttackPower();
	// 敵に攻撃を行う
	void attack(Enemy* enemy, float delay);
	// 敵全体に攻撃を行う
	void attackToAll(std::vector<Enemy*> eneimes, float delay);
	// 攻撃は全体攻撃か？
	bool isAttackToAll();

private:
	Attribute _attribute;
	// 攻撃力素点
	float _attackBase;
	// 回復力素点
	float _cure;
	// 現在の（積立中の）攻撃力
	float _attackPower;
	// コンボによる倍率
	float _comboRate;
	// 攻撃力ラベル
	Label* _attackPowerLabel;
	// 全体攻撃か
	bool _attackToAll;

	// 攻撃力（ダメージ量）の文字の更新（アニメーション付き）
	void updateAttackPowerLabel(int power);
};

#endif // PartyMember_h__
