#include "Party.h"
#include "Consts.h"
#include "AttackPowerChargeEffect.h"
#include "MotoUtil.h"

static const int MEMBER_COUNT = 6;
static const float WIDTH = 100;
static const float HEIGHT = 100;

static const Vec2 LIFE_GAUGE_POSITION = Vec2(0, -50);
static const float LIFE_GAUGE_WIDTH = 570;

Party* Party::create()
{
	auto party = new Party();

	party->init();
	party->autorelease();

	return party;
}

bool Party::init()
{
	Node::init();

	_maxLife = 2000;// TODO (仮)
	_life = _maxLife;

	loadMembers();
	initLifeGauge();
	initCurePowerLabel();
	initMembersAttackPower();

	return true;
}

void Party::addPowerByEraseDrop(Attribute attribute, int dropNum)
{
	if (attribute == Attribute::LIFE)
	{
		addCurePowerByEraseDrop(dropNum);
	}
	else
	{
		addAttackPowerByEraseDrop(attribute, dropNum);
	}
}

void Party::addAttackPowerByEraseDrop(Attribute attribute, int dropNum)
{
	for (auto member : _members)
	{
		if (member->getAttribute() == attribute)
		{
			member->addAttackPowerByEraseDrop(dropNum);
		}
	}
}

void Party::addComboRate(float amount)
{
	for (auto member : _members)
	{
		member->addAttackPowerComboRate(amount);
	}

	addCurePowerRate(amount);
}

void Party::addCurePowerByEraseDrop(int dropNum)
{
	CC_ASSERT(dropNum >= 3);

	float rate = 1.0f + ((dropNum - 3) * 0.25f);

	_curePower += (_curePowerBase * rate);

	updateCurePowerLabel(_curePower);
}

void Party::initMembersAttackPower()
{
	for (auto member : _members)
	{
		member->initAttackPower();
	}

	_curePower = 0;
	_cureComboRate = 1;
	_curePowerLabel->setVisible(false);
}

void Party::loadMembers()
{
	// 仮データ
	auto member1 = PartyMember::create("01.png", Attribute::FIRE, 100, 10);
	_members.push_back(member1);

	auto member2 = PartyMember::create("02.png", Attribute::AQUA, 200, 20);
	_members.push_back(member2);

	auto member3 = PartyMember::create("03.png", Attribute::DARK, 300, 30);
	_members.push_back(member3);

	auto member4 = PartyMember::create("04.png", Attribute::LIGHT, 400, 40);
	_members.push_back(member4);

	auto member5 = PartyMember::create("05.png", Attribute::WOOD, 500, 50);
	_members.push_back(member5);

	auto member6 = PartyMember::create("06.png", Attribute::FIRE, 600, 60);
	_members.push_back(member6);

	for (unsigned int i = 0; i < _members.size(); i++)
	{
		auto member = _members.at(i);
		member->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		member->setPosition(WIDTH / 2 + WIDTH * i - ((int)(WIDTH * _members.size()) / 2), 0);
		addChild(member);
	}

	_curePowerBase = 0;

	for (auto member : _members)
	{
		_curePowerBase += member->getCurePower();
	}
}

void Party::applyDamage(int damage)
{
	_life -= damage;

	_life = std::max(_life, 0);

	updateLifeGauge(_life, _maxLife);
}

void Party::initLifeGauge()
{
	auto sprite = Sprite::create("ProgressBar2.png");

	_lifeGauge = ProgressTimer::create(sprite);
	_lifeGauge->setScaleX(LIFE_GAUGE_WIDTH / sprite->getContentSize().width);
	_lifeGauge->setType(ProgressTimer::Type::BAR);
	_lifeGauge->setMidpoint(Vec2(0, 0));
	_lifeGauge->setBarChangeRate(Vec2(1, 0));
	_lifeGauge->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_lifeGauge->setPosition(LIFE_GAUGE_POSITION);
	_lifeGauge->setPercentage(100);

	addChild(_lifeGauge, Z_LIFE_GAUGE);
}

void Party::updateLifeGauge(int newValue, int maxValue)
{
	// 既に振動中の場合、それを止める（元の位置に戻す）
	forceFinishActionByTag(this, TAG_SHAKE_ACTION);

	// ゲージ減少中である場合、それを止める
	_lifeGauge->stopAllActions();

	// 揺らす
	auto shake = Shake::create(0.3f, Vec2(5, 5), 0.8f);
	shake->setTag(TAG_SHAKE_ACTION);
	_lifeGauge->runAction(shake);	

	float newPercentage = ((float)newValue) / maxValue * 100;

	auto action = CustomAction::create(PARTY_APPLY_DAMAGE_DURATION, _lifeGauge->getPercentage(), newPercentage,
		[this](float value){_lifeGauge->setPercentage(value); });
	auto ease = EaseQuadraticActionOut::create(action);

	runAction(ease);
}

void Party::updateCurePowerLabel(int newValue)
{
	_curePowerLabel->setVisible(true);

	_curePowerLabel->setString(StringUtils::format("%d", newValue));

	_curePowerLabel->setScale(2.5f);

	float duration = 0.2f;
	auto scaleto = ScaleTo::create(duration, 1.0f);
	auto jump = JumpBy::create(duration, Vec2::ZERO, 50, 1);
	auto spawn = Spawn::create(scaleto, jump, nullptr);
	_curePowerLabel->runAction(spawn);
}

void Party::initCurePowerLabel()
{
	_curePowerLabel = Label::createWithTTF(
		"CURE POWER",
		"fonts/Marker Felt.ttf",
		30,
		Size::ZERO,
		TextHAlignment::CENTER,
		TextVAlignment::CENTER);
	_curePowerLabel->setColor(Color3B::WHITE);
	_curePowerLabel->setPosition(getContentSize().width / 2, -50);

	auto config = _curePowerLabel->getTTFConfig();
	config.outlineSize = 3;
	_curePowerLabel->setTTFConfig(config);


	addChild(_curePowerLabel, Z_CURE_POWER_LABEL);
}

void Party::addCurePowerRate(float amount)
{
	_cureComboRate += amount;

	if (_curePower > 0)
	{
		updateCurePowerLabel(getCurePower());
	}
}

int Party::getCurePower()
{
	return std::ceil(((float)_curePowerBase) * _cureComboRate);
}

void Party::fireChargeEffect(const Vec2& startPosition, Attribute attribute)
{
	if (attribute == Attribute::LIFE)
	{
		auto position = LIFE_GAUGE_POSITION + getPosition(); // Sceneにおける座標

		auto effect = AttackPowerChargeEffect::create(startPosition, position);

		getParent()->addChild(effect); // Sceneに追加
	}
	else
	{
		for (auto member : _members)
		{
			if (member->getAttribute() != attribute)
				continue;

			auto memberPosition = member->getPosition() + getPosition(); // Sceneにおける座標

			auto effect = AttackPowerChargeEffect::create(startPosition, memberPosition);

			getParent()->addChild(effect); // Sceneに追加
		}
	}
}

const std::vector<PartyMember*> Party::getMembers()
{
	return _members;
}


float Party::doAttackSequence(EnemyParty* enemyParty)
{
	auto enemies = enemyParty->getActiveEnemies();

	CC_ASSERT(enemies.size() > 0);

	EnemyLifeTable enemyLifeTable;

	for (auto enemy : enemies)
	{
		enemyLifeTable[enemy] = enemy->getLife();
	}

	float delayTime = 0;

	for (unsigned int i = 0; i < _members.size(); i++)
	{
		auto member = _members.at(i);

		if (member->getAttackPower() <= 0)
			continue;

		if (member->isAttackToAll())
		{
			for (auto target : enemies)
			{
				auto damage = target->calcDamage(member->getAttackPower(), member->getAttribute());
				enemyLifeTable[target] -= damage;
			}
			member->attackToAll(enemies, delayTime);
		}
		else
		{
			auto target = decideAttackTarget(enemies, enemyLifeTable, member);

			auto damage = target->calcDamage(member->getAttackPower(), member->getAttribute());
			enemyLifeTable[target] -= damage;
			member->attack(target, delayTime);
		}

		// 死んだ敵はvectorから除去する
		// ただし、最後の一体は消さない（集中砲火でオーバーキルとする）
		for (auto itr = enemies.begin(); itr != enemies.end() && enemies.size() > 1; ++itr)
		{
			if (enemyLifeTable.at(*itr) <= 0)
			{
				itr = enemies.erase(itr);
				break;
			}
		}

		delayTime += MEMBERS_ATTACK_INTERVAL;
	}

	return delayTime + ((delayTime > 0) ? ATTACK_EFFECT_DURATION : 0);
}

Enemy* Party::decideAttackTarget(std::vector<Enemy*> enemies, EnemyLifeTable enemyLifeTable, PartyMember* member)
{
	// 一撃で殺せる敵をリストアップし、最大HPの高い順に対象とする
	std::vector<Enemy*> enemiesCouldBeKill;

	for (auto enemy : enemies)
	{
		if (canKill(enemy, enemyLifeTable[enemy], member))
		{
			enemiesCouldBeKill.push_back(enemy);
		}
	}

	if (enemiesCouldBeKill.size() > 0)
	{
		// 最大HPの昇順で並び替え
		std::sort(enemiesCouldBeKill.begin(), enemiesCouldBeKill.end(), [](Enemy* a, Enemy* b){return (a->getMaxLife() < b->getMaxLife()); });

		// 最大HPが多い敵を返却する
		return enemiesCouldBeKill.back();
	}

	// 一撃で殺せる敵がいない場合、最大HPが最も少ない敵を選ぶ
	std::vector<Enemy*> enemiesCouldNotBeKill = enemies;

	std::sort(enemiesCouldNotBeKill.begin(), enemiesCouldNotBeKill.end(), [](Enemy* a, Enemy* b){return (a->getMaxLife() < b->getMaxLife()); });

	return enemiesCouldNotBeKill.front();
}

bool Party::isAlive()
{
	return (_life > 0);
}

bool Party::canKill(Enemy* target, int remainingLife, PartyMember* member)
{
	int damage = target->calcDamage(member->getAttackPower(), member->getAttribute());

	return (damage >= remainingLife);
}