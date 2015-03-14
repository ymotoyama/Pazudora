#include "Enemy.h"
#include "MotoUtil.h"
#include "PartyMember.h"
#include "MotoUtil.h"
#include "Consts.h"
#include "Party.h"

static const Vec2 LIFE_GAUGE_POSITION = Vec2(0, -30);
static const float APPLY_DAMAGE_DURATION = 0.2f;

struct EnemyData
{
	std::string imageFileName;
	int life;
	int attackPower;
	Attribute attribute;
	int attackInterval;

	EnemyData(std::string aImageFileName, int aLife, int aAttackPower, Attribute aAttribute, int aAttackInterval)
	: imageFileName(aImageFileName)
	, life(aLife)
	, attackPower(aAttackPower)
	, attribute(aAttribute)
	, attackInterval(aAttackInterval)
	{
	}
};

Enemy* Enemy::create(EnemyKind kind)
{
	Enemy* enemy = new Enemy();

	enemy->init(kind);
	enemy->autorelease();

	return enemy;
}

bool Enemy::init(EnemyKind kind)
{
	static const std::map<EnemyKind, EnemyData> enemyDataTable = 
	{
		/* ‰æ‘œƒtƒ@ƒCƒ‹–¼, ƒ‰ƒCƒt, UŒ‚—Í, ‘®«, UŒ‚ŠÔŠu(ƒ^[ƒ“) */
		{ EnemyKind::ENEMY1, EnemyData("01.png", 800, 100, Attribute::LIGHT, 3) },
		{ EnemyKind::ENEMY2, EnemyData("02.png", 1200, 200, Attribute::LIGHT, 4) },
		{ EnemyKind::ENEMY3, EnemyData("03.png", 1500, 300, Attribute::LIGHT, 5) },
	};

	auto enemyData = enemyDataTable.at(kind);
	
	auto sprite = Sprite::create(enemyData.imageFileName);
	sprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
	addChild(sprite);

	// “G‚Ì‘å‚«‚³‚Í‰æ‘œƒvƒ‰ƒXƒAƒ‹ƒtƒ@‚Æ‚·‚é
	auto size = sprite->getContentSize();
	size.width += 50;
	setContentSize(size);

	_maxLife = enemyData.life;
	_life = enemyData.life;
	_displayedLife = _life;
	_attackPower = enemyData.attackPower;
	_attribute = enemyData.attribute;
	_attackInterval = enemyData.attackInterval;
	_remainingTurnForAttack = enemyData.attackInterval;

	initLifeGauge(sprite->getContentSize().width);
	initRemainingTurnForAttackLabel();

	setCascadeOpacityEnabled(true);
	setCascadeColorEnabled(true);

	return true;
}

void Enemy::initLifeGauge(float width)
{
	auto sprite = Sprite::create("ProgressBar2.png");

	_lifeGauge = ProgressTimer::create(sprite);
	_lifeGauge->setScaleX(width / sprite->getContentSize().width);
	_lifeGauge->setType(ProgressTimer::Type::BAR);
	_lifeGauge->setMidpoint(Vec2(0, 0));
	_lifeGauge->setBarChangeRate(Vec2(1, 0));
	_lifeGauge->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_lifeGauge->setPosition(LIFE_GAUGE_POSITION);
	_lifeGauge->setPercentage(100);

	addChild(_lifeGauge);
}

void Enemy::applyDamage(int damage)
{
	if (!isAlive())
		return;

	_life = std::max(0, (_life - damage));

	updateLifeGauge();

	// Šù‚ÉU“®’†‚Ìê‡A‚»‚ê‚ğ~‚ß‚é
	forceFinishActionByTag(this, TAG_SHAKE_ACTION);

	// —h‚ç‚·
	auto shake = Shake::create(0.3f, Vec2(8, 8), 0.8f);
	shake->setTag(TAG_SHAKE_ACTION);
	runAction(shake);

	// €‚Ê‚È‚çÁ–Åˆ—
	if (!isAlive())
	{
		EXEC_DELAY(APPLY_DAMAGE_DURATION, [this](){ die(); }, this);
	}
}

void Enemy::updateLifeGauge()
{
	_lifeGauge->stopAllActions();
		
	auto action = CustomAction::create(
		APPLY_DAMAGE_DURATION,
		_displayedLife,
		_life,
		[this](float displayedLife)
	{
		_displayedLife = displayedLife;
		float percentage = ((float)_displayedLife) / _maxLife * 100;
		_lifeGauge->setPercentage(percentage);
	});

	runAction(action);

}

void Enemy::die()
{
	auto fadeout = FadeOut::create(0.2f);
	runAction(fadeout);
}

void Enemy::incrementTurn()
{
	if (!isAlive())
		return;

	_remainingTurnForAttack--;

	updateRemainingTurnForAttackLabel();
}

void Enemy::initRemainingTurnForAttackLabel()
{
	_remainingTurnForAttackLabel = Label::createWithTTF(
		"",
		"fonts/Marker Felt.ttf",
		30,
		Size::ZERO,
		TextHAlignment::LEFT,
		TextVAlignment::CENTER);
	_remainingTurnForAttackLabel->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
	_remainingTurnForAttackLabel->setColor(Color3B::YELLOW);
	_remainingTurnForAttackLabel->setPosition(- getContentSize().width / 2 + 10, getContentSize().height + 10);

	auto config = _remainingTurnForAttackLabel->getTTFConfig();
	config.outlineSize = 3;
	_remainingTurnForAttackLabel->setTTFConfig(config);
	
	addChild(_remainingTurnForAttackLabel);

	updateRemainingTurnForAttackLabel();
}

void Enemy::updateRemainingTurnForAttackLabel()
{
	auto text = StringUtils::toString(_remainingTurnForAttack);
	_remainingTurnForAttackLabel->setString(text);
}

Attribute Enemy::getAttribute()
{
	return _attribute;
}

int Enemy::getLife()
{
	return _life;
}

int Enemy::getMaxLife()
{
	return _maxLife;
}

bool Enemy::isValnerableTo(Attribute attribute)
{
	return (
		((_attribute == Attribute::FIRE)  && (attribute == Attribute::AQUA))  ||
		((_attribute == Attribute::WOOD)  && (attribute == Attribute::FIRE))  ||
		((_attribute == Attribute::AQUA)  && (attribute == Attribute::WOOD))  ||
		((_attribute == Attribute::DARK)  && (attribute == Attribute::LIGHT)) ||
		((_attribute == Attribute::LIGHT) && (attribute == Attribute::DARK)));
}

bool Enemy::hasResistanceTo(Attribute attribute)
{
	return (
		((_attribute == Attribute::FIRE)  && (attribute == Attribute::WOOD))  ||
		((_attribute == Attribute::WOOD)  && (attribute == Attribute::AQUA))  ||
		((_attribute == Attribute::AQUA)  && (attribute == Attribute::FIRE)));
}

bool Enemy::isAlive()
{
	return (_life > 0);
}

int Enemy::getRemainingTurnForAttack()
{
	return _remainingTurnForAttack;
}

void Enemy::attack(Party* party)
{
	if (!isAlive())
		return;

	auto jump = JumpBy::create(0.14f, Vec2::ZERO, -20, 1);
	runAction(jump);

	EXEC_DELAY(0.07f, [=](){party->applyDamage(_attackPower); }, this);
}

void Enemy::resetRemainingTurnForAttack()
{
	_remainingTurnForAttack = _attackInterval;

	updateRemainingTurnForAttackLabel();
}

int Enemy::calcDamage(PartyMember* partyMember)
{
	return (calcDamage(partyMember->getAttackPower(), partyMember->getAttribute()));
}

int Enemy::calcDamage(int attackPower, Attribute attribute)
{
	// “G‚Ìã“_‚ÅUŒ‚
	if (isValnerableTo(attribute))
	{
		attackPower *= 2;
	}

	// “G‚Ì‘Ï«‚Ì‚ ‚é‘®«‚ÅUŒ‚
	if (hasResistanceTo(attribute))
	{
		attackPower /= 2;
	}

	return attackPower;
}

void Enemy::setOpacity(GLubyte opacity)
{
	Node::setOpacity(opacity);

	_lifeGauge->setOpacity(opacity);
}