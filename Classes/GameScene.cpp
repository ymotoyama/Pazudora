#include "GameScene.h"
#include "Attribute.h"
#include "ComboLabel.h"
#include "DropMatchChecker.h"
#include "MotoUtil.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

#define ERASE_DROP_SE "power38.wav"
#define SWAP_DROP_SE "kachi33.wav"
#define CHARGE_SE "pyoro56_a.wav"

GameScene* GameScene::create()
{
	auto scene = new GameScene();

	scene->init();
	scene->autorelease();

	return scene;
}

bool GameScene::init()
{
	srand(time(nullptr));

	SimpleAudioEngine::getInstance()->preloadEffect(ERASE_DROP_SE);
	SimpleAudioEngine::getInstance()->preloadEffect(SWAP_DROP_SE);
	SimpleAudioEngine::getInstance()->preloadEffect(CHARGE_SE);
	
	for (int row = 0; row < ROW_COUNT; row++)
	{
		for (int column = 0; column < COLUMN_COUNT; column++)
		{
			_drops[row][column] = nullptr;
		}
	}

	initParty();
	initEnemyParty();
	initDrops();
	
	for (int row = 0; row < ROW_COUNT; row++)
	{
		for (int column = 0; column < COLUMN_COUNT; column++)
		{
			auto drop = _drops[row][column];
			drop->setPosition(calcDropPosition(PositionIndex(row, column)));
			addChild(drop, Z_DROP);
		}
	}

	registerTouchEvent();

	changeState(State::WAITING_INPUT);

	scheduleUpdate();
		
	// DEBUG CODE
	std::vector<Enemy*> enemies =
	{
		Enemy::create(EnemyKind::ENEMY1),
		Enemy::create(EnemyKind::ENEMY2),
		Enemy::create(EnemyKind::ENEMY3),
	};

	auto member = PartyMember::create("01.png", Attribute::DARK, 600, 100);
	member->addAttackPowerByEraseDrop(3);
	//member->addAttackPowerComboRate(1.0f);
			
	return true;
}

void GameScene::update(float delta)
{
	switch (_state)
	{
	case State::GAME_START:
		break;
	case State::WAITING_INPUT:
		break;
	case State::TOUCHING: {
		_remainingTouchTime -= delta;
		if (_remainingTouchTime <= 0)
		{
			changeState(State::ERASING);
		}
	} break;
	case State::ERASING:
		break;
	case State::CHARGE_ATTACK_POWER:
		break;
	case State::PARTY_ATTACK:
		break;
	case State::AFTER_PARTY_ATTACK:
		break;
	case State::ENEMY_ATTACK:
		break;
	case State::AFTER_ENEMY_ATTACK:
		break;
	case State::GAME_OVER:
		break;
	case State::GAME_CLEAR:
		break;
	default:
		break;
	}
}

void GameScene::changeState(State newState)
{
	_state = newState;

	switch (_state)
	{
	case State::GAME_START:
		break;
	case State::WAITING_INPUT:
		break;
	case State::TOUCHING:
		_remainingTouchTime = MAX_TOUCH_DURATION;
		_comboCount = 0;
		break;
	case State::ERASING:{
		_draggingDrop->setDragging(false);
		_draggingDropSprite->removeFromParent();
		matchTest();
	} break;
	case State::CHARGE_ATTACK_POWER:{
		for (unsigned int i = 0; i < _comboLabels.size(); i++)
		{
			auto func = [this](){
				_party->addComboRate(0.25f);
				_comboLabels.front()->hide();
				_comboLabels.pop();
				SimpleAudioEngine::getInstance()->playEffect(CHARGE_SE);
			};

			EXEC_DELAY(CHARGE_BY_COMBO_DURATION * i, func, this);
		}

		EXEC_DELAY(CHARGE_BY_COMBO_DURATION * _comboLabels.size(),
			[this](){changeState(State::PARTY_ATTACK); },
			this);
	} break;
	case State::PARTY_ATTACK: {
		float attackDuration = _party->doAttackSequence(_enemyParty);

		EXEC_DELAY(attackDuration, [this](){
			changeState(State::AFTER_PARTY_ATTACK);
		}, this);
	} break;
	case State::AFTER_PARTY_ATTACK: {
		_party->initMembersAttackPower();

		if (_enemyParty->isWipedOut())
		{
			changeState(State::GAME_CLEAR);
		}
		else
		{
			changeState(State::ENEMY_ATTACK);
		}
	} break;
	case State::ENEMY_ATTACK: {
		_enemyParty->incrementTurn();
		_enemyParty->attack(_party, ([this](){ changeState(State::AFTER_ENEMY_ATTACK); }));
	} break;
	case State::AFTER_ENEMY_ATTACK: {
		if (_party->isAlive())
		{
			changeState(State::WAITING_INPUT);
		}
		else
		{
			changeState(State::GAME_OVER);
		}
	} break;
	case State::GAME_OVER: {
		CCLOG("Game Over!!");
	} break;
	case State::GAME_CLEAR: {
		CCLOG("Game Clear!!");
	} break;
	default:
		break;
	}
}

void GameScene::registerTouchEvent()
{
	auto listener = EventListenerTouchOneByOne::create();

	// タッチ開始時
	listener->onTouchBegan = [this](Touch* touch, Event* e){
		if (_state != State::WAITING_INPUT)
			return false;

		_touchingPositionIndex = getPositionIndexFromPosition(touch->getLocation());

		if (_touchingPositionIndex.isInvalid())
			return false;

		_draggingDrop = getDrop(touch->getLocation());
		_draggingDrop->setDragging(true);

		createDraggingDropSpriteAndAdd(touch->getLocation());

		changeState(State::TOUCHING);
		// TODO 移動してドロップ交換時に初めて残り時間をカウントし始める

		return true;
	};
	
	// タッチ移動時
	listener->onTouchMoved = [this](Touch* touch, Event* e){
		if (_state != State::TOUCHING)
			return;

		if (_touchingPositionIndex.isInvalid())
			return;

		_draggingDropSprite->setPosition(touch->getLocation());

		auto newPositionIndex = getPositionIndexFromPositionClamp(touch->getLocation());
		
		if ((newPositionIndex.isInvalid()) || (newPositionIndex == _touchingPositionIndex))
			return;

		swapDrop(_touchingPositionIndex, newPositionIndex);

		_touchingPositionIndex = newPositionIndex;
	};

	// タッチ終了時
	listener->onTouchEnded = [this](Touch* touch, Event* e){
		if (_state != State::TOUCHING)
			return;

		changeState(State::ERASING);
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void GameScene::initDrops()
{
	// ランダムでドロップを生成する。
	// いきなり揃ってしまう場合は決め直す。
	while (true)
	{
		for (int row = 0; row < ROW_COUNT; row++)
		{
			for (int column = 0; column < COLUMN_COUNT; column++)
			{
				_drops[row][column] = Drop::create(row, column);
			}
		}

		// 揃ってなければOK。揃ってたらもう一度生成しなおし。
		if (DropMatchChecker::analyze(_drops).size() <= 0)
			break;
	}
}

void GameScene::fillEmpty()
{
	for (int column = 0; column < COLUMN_COUNT; column++)
	{
		int emptyCount = 0;

		for (int row = 0; row < ROW_COUNT; row++)
		{
			if (_drops[row][column] == nullptr)
			{
				emptyCount++;
			}
		}

		for (int row = ROW_COUNT - emptyCount; row < ROW_COUNT; row++)
		{
			auto drop = Drop::create(row, column);
			drop->setOpacity(0);
			drop->setPosition(calcDropPosition(PositionIndex(row + emptyCount, column)));
			addChild(drop, Z_DROP);

			_drops[row][column] = drop;

			auto move = MoveTo::create(FALL_DROP_DURATION, calcDropPosition(drop->getPositionIndex()));
			auto fadein = FadeIn::create(FALL_DROP_DURATION);
			auto spawn = Spawn::create(move, fadein, nullptr);
			drop->runAction(spawn);
		}
	}
}

void GameScene::fallDrops()
{
	for (int column = 0; column < COLUMN_COUNT; column++)
	{
		int fallCount = 0;

		for (int row = 0; row < ROW_COUNT; row++)
		{
			if (_drops[row][column] == nullptr)
			{
				fallCount++;
			}
			else if (fallCount > 0)
			{
				auto drop = _drops[row][column];
				_drops[row - fallCount][column] = drop;
				_drops[row][column] = nullptr;
				drop->setPositionIndex(PositionIndex(row - fallCount, column));
				auto move = MoveTo::create(FALL_DROP_DURATION, calcDropPosition(drop->getPositionIndex()));
				drop->runAction(move);
			}
		}
	}
}

void GameScene::matchTest()
{
	auto matched = DropMatchChecker::analyze(_drops);

	if (matched.size() <= 0)
	{
		changeState(State::CHARGE_ATTACK_POWER);
		return;
	}
	
	// 揃ったドロップを一定間隔ごとに消去する
	for (unsigned int i = 0; i < matched.size(); i++)
	{
		_comboCount++;

		auto drops = matched.at(i);
		auto attribute = drops.front()->getAttribute();
		auto dropNum = drops.size();
		auto delayTimeForRemove = REMOVE_DROP_INTERVAL * i;

		for (auto drop : drops)
		{
			auto positionIndex = drop->getPositionIndex();
			_drops[positionIndex.row][positionIndex.column] = nullptr;

			drop->scheduleRemove(delayTimeForRemove);

			auto tmpComboCount = _comboCount;
			EXEC_DELAY(delayTimeForRemove, [=](){playEraseDropSE(tmpComboCount); }, this);
		}

		const Vec2& centerOfDrops = calcCenterOfDrops(drops);

		// 攻撃力をチャージするエフェクトの生成と、チャージの予約	
		auto delay = DELAY(delayTimeForRemove);
		auto chargeEffect = CallFunc::create([=](){_party->fireChargeEffect(centerOfDrops, attribute); });
		auto delay2 = DELAY(ATTACK_CHAGE_EFFECT_DURATION);
		auto addAttackPower = CallFunc::create([=](){_party->addPowerByEraseDrop(attribute, dropNum); });
		auto seq = SEQUENCE(delay, chargeEffect, delay2, addAttackPower);
		runAction(seq);

		// コンボを生成
		if (_comboCount > 1)
		{
			createComboDisplay(centerOfDrops, _comboCount, REMOVE_DROP_INTERVAL * i);
		}
	}

	// 揃ったドロップが全て消えた頃に上から新しいドロップを落とし、
	// 再度揃ったかどうかのチェックを行う。
	auto delay1 = DELAY(REMOVE_DROP_INTERVAL * matched.size());
	auto fallAndCreateNewDrops = CallFunc::create([this](){
		fallDrops();
		fillEmpty(); });
	auto delay2 = DELAY(FALL_DROP_DURATION + DELAY_AFTER_FALL_DROP);
	auto check = CallFunc::create([this](){
		matchTest();
	});
	auto seq = Sequence::create(delay1, fallAndCreateNewDrops, delay2, check, nullptr);
	runAction(seq);
}

Drop* GameScene::getDrop(const Vec2& position)
{
	auto children = getChildren();

	for (auto child : children)
	{
		if ((child->getTag() == TAG_DROP) && child->getBoundingBox().containsPoint(position))
		{
			return (Drop*)child;
		}
	}

	return nullptr;
}

void GameScene::swapDrop(PositionIndex a, PositionIndex b)
{
	auto tmp = _drops[a.row][a.column];
	_drops[a.row][a.column] = _drops[b.row][b.column];
	_drops[b.row][b.column] = tmp;

	_drops[a.row][a.column]->moveTo(a);
	_drops[b.row][b.column]->moveTo(b);

	SimpleAudioEngine::getInstance()->playEffect(SWAP_DROP_SE);
}

cocos2d::Vec2 GameScene::calcDropPosition(const PositionIndex& positionIndex)
{
	float centerX = RESOLUTION_X / 2;
	float originX = centerX - DROP_SIZE * (((float)COLUMN_COUNT) / 2 - 0.5f);
	float originY = DROP_BOTTOM_MARGIN + DROP_SIZE / 2;
	float y = originY + DROP_SIZE * positionIndex.row;
	float x = originX + DROP_SIZE * positionIndex.column;

	return Vec2(x, y);
}

PositionIndex GameScene::getPositionIndexFromPosition(const Vec2& position)
{
	float centerX = RESOLUTION_X / 2;
	float originX = centerX - DROP_SIZE * (((float)COLUMN_COUNT) / 2);
	float originY = DROP_BOTTOM_MARGIN;

	auto positionIndex = PositionIndex(
		((int)(position.y - originY) / DROP_SIZE),
		((int)(position.x - originX) / DROP_SIZE));

	if (positionIndex.row < 0 || positionIndex.row >= ROW_COUNT
		|| positionIndex.column < 0 || positionIndex.column >= COLUMN_COUNT)
	{
		return PositionIndex::INVALID_INDEX;
	}

	return positionIndex;
}

PositionIndex GameScene::getPositionIndexFromPositionClamp(const Vec2& position)
{
	float centerX = RESOLUTION_X / 2;
	float originX = centerX - DROP_SIZE * (((float)COLUMN_COUNT) / 2);
	float originY = DROP_BOTTOM_MARGIN;

	auto positionIndex = PositionIndex(
		((int)(position.y - originY) / DROP_SIZE),
		((int)(position.x - originX) / DROP_SIZE));

	if (positionIndex.row >= ROW_COUNT)
		positionIndex.row = ROW_COUNT - 1;

	if (positionIndex.row < 0)
		positionIndex.row = 0;

	if (positionIndex.column >= COLUMN_COUNT)
		positionIndex.column = COLUMN_COUNT - 1;
	
	if (positionIndex.column < 0)
		positionIndex.column = 0;
	
	return positionIndex;
}

cocos2d::Vec2 GameScene::calcCenterOfDrops(const std::vector<Drop*>& drops)
{
	float minX = RESOLUTION_X;
	float maxX = 0;
	float minY = RESOLUTION_Y;
	float maxY = 0;

	for (auto drop : drops)
	{
		const auto& dropPos = drop->getPosition();
		minX = std::min(minX, dropPos.x);
		maxX = std::max(maxX, dropPos.x);
		minY = std::min(minY, dropPos.y);
		maxY = std::max(maxY, dropPos.y);
	}

	return Vec2((minX + maxX) / 2, (minY + maxY) / 2);
}

void GameScene::createComboDisplay(const Vec2& position, int comboCount, float delay)
{
	EXEC_DELAY(delay, [=](){
		auto comboLabel = ComboLabel::create(position, comboCount);
		addChild(comboLabel);
		_comboLabels.push(comboLabel);
	}, this);
}

void GameScene::createDraggingDropSpriteAndAdd(const Vec2& position)
{
	auto touchingDrop = getDrop(position);

	_draggingDropSprite = Sprite::createWithTexture(touchingDrop->getTexture());
	_draggingDropSprite->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_draggingDropSprite->setPosition(position);
	_draggingDropSprite->setScale(DROP_SIZE / _draggingDropSprite->getContentSize().width * DRAGGING_DROP_SIZE_RATIO);
	_draggingDropSprite->setOpacity(180);

	addChild(_draggingDropSprite, Z_DRAGGING_DROP);
}

void GameScene::playEraseDropSE(int comboCount)
{
	float pitch = 1.0f + (0.06f * (comboCount - 1));
	SimpleAudioEngine::getInstance()->playEffect(ERASE_DROP_SE, false, pitch, 0.0f, 1.0f);
}

void GameScene::initParty()
{
	_party = Party::create();
	_party->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_party->setPosition(RESOLUTION_X / 2, 600);
	addChild(_party, Z_PARTY);
}

void GameScene::initEnemyParty()
{
	_enemyParty = EnemyParty::create();
	_enemyParty->setPosition(0, ENEMY_PARTY_Y);
	addChild(_enemyParty, Z_ENEMY);
}

