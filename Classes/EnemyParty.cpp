#include "EnemyParty.h"
#include "Consts.h"
#include "MotoUtil.h"

EnemyParty* EnemyParty::create()
{
	auto ep = new EnemyParty();

	ep->init();
	ep->autorelease();

	return ep;
}

bool EnemyParty::init()
{
	_enemies.push_back(Enemy::create(EnemyKind::ENEMY1));
	_enemies.push_back(Enemy::create(EnemyKind::ENEMY2));
	_enemies.push_back(Enemy::create(EnemyKind::ENEMY3));

	alignCenter();

	for (unsigned int i = 0; i < _enemies.size(); i++)
	{
		auto enemy = _enemies.at(i);
		addChild(enemy);
		
		enemy->setOpacity(0);
		auto delay = DelayTime::create(0.12 * i);
		auto fadein = FadeIn::create(0.3f);
		auto seq = Sequence::create(delay, fadein, nullptr);
		enemy->runAction(seq);
	}

	return true;
}

void EnemyParty::alignCenter()
{
	float totalWidth = 0;

	for (auto enemy : _enemies)
	{
		totalWidth += enemy->getContentSize().width;
	}

	float x = (RESOLUTION_X / 2) - (totalWidth / 2);

	for (auto enemy : _enemies)
	{
		float width = enemy->getContentSize().width;
		enemy->setPosition(x + width / 2, 0);
		x += width;
	}
}

const std::vector<Enemy*> EnemyParty::getActiveEnemies()
{
	std::vector<Enemy*> activeEnemies;
	
	for (auto enemy : _enemies)
	{
		if (enemy->isAlive())
		{
			activeEnemies.push_back(enemy);
		}
	}

	return activeEnemies;
}

void EnemyParty::incrementTurn()
{
	for (auto enemy : _enemies)
	{
		enemy->incrementTurn();
	}
}

void EnemyParty::attack(Party* party, std::function<void()> onComplete)
{
	float delayTime = 0;

	for (auto enemy : _enemies)
	{
		if (enemy->getRemainingTurnForAttack() <= 0)
		{
			EXEC_DELAY(delayTime, [=](){enemy->attack(party); }, enemy);

			delayTime += ENEMY_ATTACK_INTERVAL;
		}
	}

	delayTime += (delayTime > 0) ? (ENEMY_ATTACK_EFFECT_DURATION + ENEMY_ATTACK_DAMAGE_DURATION) : 0;

	// UŒ‚Š®—¹Žž‚Ìˆ—
	EXEC_DELAY(delayTime, [=]()
	{
		for (auto enemy : _enemies)
		{
			if (enemy->getRemainingTurnForAttack() <= 0)
			{
				enemy->resetRemainingTurnForAttack();
			}
		}

		onComplete();
	}, this);
}

bool EnemyParty::isWipedOut()
{
	for (auto enemy : _enemies)
	{
		if (enemy->isAlive())
		{
			return false;
		}
	}

	return true;
}
