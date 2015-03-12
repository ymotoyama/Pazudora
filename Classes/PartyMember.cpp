#include "PartyMember.h"
#include "Consts.h"
#include "MotoUtil.h"
#include "PartyMemberAttackEffect.h"

PartyMember* PartyMember::create(std::string imageFileName, Attribute attribute, float attack, float cure)
{
	auto member = new PartyMember();

	member->init(imageFileName, attribute, attack, cure);
	member->autorelease();

	return member;
}

bool PartyMember::init(std::string imageFileName, Attribute attribute, float attack, float cure)
{
	Sprite::initWithFile(imageFileName);

	_attribute = attribute;
	_attackBase = attack;
	_cure = cure;

	_attackPowerLabel = Label::createWithTTF(
		"attack",
		"fonts/Marker Felt.ttf",
		30,
		Size::ZERO,
		TextHAlignment::CENTER,
		TextVAlignment::CENTER);
	_attackPowerLabel->setColor(Color3B::WHITE);
	_attackPowerLabel->setPosition(getContentSize().width / 2, 30);

	auto config = _attackPowerLabel->getTTFConfig();
	config.outlineSize = 3;
	_attackPowerLabel->setTTFConfig(config);

	addChild(_attackPowerLabel);

	initAttackPower();

	return true;
}

Attribute PartyMember::getAttribute()
{
	return _attribute;
}

void PartyMember::initAttackPower()
{
	_attackPower = 0;
	_comboRate = 1;
	_attackPowerLabel->setVisible(false);
	_attackToAll = false;
}

void PartyMember::addAttackPowerByEraseDrop(int dropNum)
{
	CC_ASSERT(dropNum >= 3);

	float rate = 1.0f + ((dropNum - 3) * 0.25f);

	_attackPower += (_attackBase * rate);

	updateAttackPowerLabel(getAttackPower());

	if (dropNum >= ATTACK_TO_ALL_NUM)
	{
		_attackToAll = true;
	}
}

void PartyMember::addAttackPowerComboRate(float amount)
{
	_comboRate += amount;

	if (_attackPower > 0)
	{
		updateAttackPowerLabel(getAttackPower());
	}
}

int PartyMember::getAttackPower()
{
	return std::ceil(_attackPower * _comboRate);
}

void PartyMember::attack(Enemy* enemy, float delay)
{
	float damage = enemy->calcDamage(this);

	// 攻撃エフェクト
	EXEC_DELAY(delay, [=](){
		PartyMemberAttackEffect::fire(this, enemy, _attribute);
	}, this);

	// 敵にダメージ反映
	EXEC_DELAY(delay + ATTACK_EFFECT_DURATION, [=](){enemy->applyDamage(damage);}, this);
}

void PartyMember::attackToAll(std::vector<Enemy*> eneimes, float delay)
{
	for (auto enemy : eneimes)
	{
		float damage = enemy->calcDamage(this);
		// 攻撃エフェクト
		EXEC_DELAY(delay, [=](){
			PartyMemberAttackEffect::fire(GET_SCREEN_POS(this), Vec2(RESOLUTION_X / 2, ENEMY_PARTY_Y), _attribute);
		}, this);

		// 敵にダメージ反映
		EXEC_DELAY(delay + ATTACK_EFFECT_DURATION, [=](){enemy->applyDamage(damage); }, this);
	}
}

bool PartyMember::isAttackToAll()
{
	return _attackToAll;
}

void PartyMember::updateAttackPowerLabel(int power)
{
	_attackPowerLabel->setVisible(true);

	_attackPowerLabel->setString(StringUtils::format("%d", power));

	_attackPowerLabel->setScale(2.5f);

	float duration = 0.2f;
	auto scaleto = ScaleTo::create(duration, 1.0f);
	auto jump = JumpBy::create(duration, Vec2::ZERO, 50, 1);
	auto spawn = Spawn::create(scaleto, jump, nullptr);
	_attackPowerLabel->runAction(spawn);
}

int PartyMember::getCurePower()
{
	return _cure;
}
