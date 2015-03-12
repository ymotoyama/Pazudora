#include "MotoUtil.h"
#include <random>


//
// Shake
//

Shake* Shake::create(float duration, const Vec2& range, float speedRate)
{
	Shake *ret = new Shake();
	ret->initWithDuration(duration, range, speedRate);
	ret->autorelease();

	return ret;
}

bool Shake::initWithDuration(float duration, const Vec2& range, float speedRate)
{
	if (ActionInterval::initWithDuration(duration))
	{
		_range = range;
		_speedRate = speedRate;

		_rotation = RAND_INT(0, 3);
		_count = 0;
		_nextRotationCount = (int)RAND_ND(16, 5);

		for (int i = 0; i < 4; i++){
			_theta[i] = RAND_FLOAT(0, 3.1415f);
		}

		return true;
	}

	return false;
}

Shake* Shake::clone() const
{
	// no copy constructor
	auto a = new Shake();
	a->initWithDuration(_duration, _range, _speedRate);
	a->autorelease();
	return a;
}

void Shake::startWithTarget(Node *target)
{
	ActionInterval::startWithTarget(target);
	_startPosition = target->getPosition();
}

Shake* Shake::reverse() const
{
	return Shake::create(_duration, _range, _speedRate);
}

void Shake::update(float t)
{
	if (_target == nullptr) return;

	_count++;

	_theta[(_rotation + 0) % 4] += (1.04f + RAND_ND(0, 0.2f)) * _speedRate;
	_theta[(_rotation + 1) % 4] += (0.90f + RAND_ND(0, 0.2f)) * _speedRate;
	_theta[(_rotation + 2) % 4] += (1.18f + RAND_ND(0, 0.2f)) * _speedRate;
	_theta[(_rotation + 3) % 4] += (0.98f + RAND_ND(0, 0.2f)) * _speedRate;

	Vec2 newPosition = _startPosition;
	newPosition.x += (sin(_theta[0]) + sin(_theta[1])) * _range.x / 2;
	newPosition.y += (sin(_theta[2]) + sin(_theta[2])) * _range.y / 2;

	float rangeRate = clampf((10.0f - t * 10), 0.0f, 1.0f);

	newPosition.x = newPosition.x * rangeRate + _startPosition.x * (1.0f - rangeRate);
	newPosition.y = newPosition.y * rangeRate + _startPosition.y * (1.0f - rangeRate);

	_target->setPosition(newPosition);

	if (_count == _nextRotationCount) {
		_rotation++;
		_nextRotationCount += (int)RAND_ND(16, 5);
	}
}



//
// CustomAction
//

CustomAction* CustomAction::create(float duration, float from, float to, std::function<void(float)> callbackLambda)
{
	CustomAction *ret = new CustomAction();
	ret->initWithDuration(duration, from, to, callbackLambda);
	ret->autorelease();

	return ret;
}

bool CustomAction::initWithDuration(float duration, float from, float to, std::function<void(float)> callbackLambda)
{
	if (ActionInterval::initWithDuration(duration))
	{
		_duration = duration;
		_from = from;
		_to = to;
		_callback = callbackLambda;

		return true;
	}

	return false;
}

CustomAction* CustomAction::clone() const
{
	// no copy constructor
	auto a = new CustomAction();
	a->initWithDuration(_duration, _from, _to, _callback);
	a->autorelease();
	return a;
}

void CustomAction::startWithTarget(Node *target)
{
	ActionInterval::startWithTarget(target);
}

CustomAction* CustomAction::reverse() const
{
	return CustomAction::create(_duration, _to, _from, _callback);
}

void CustomAction::update(float t)
{
	if (_target == nullptr) return;
	
	float value = _from + (_to - _from) * t;

	_callback(value);
}

//
// Random
//

Random* Random::s_instance;

Random::Random()
{
	std::random_device rd;
	_seed = rd();
	CCLOG("Random seed=%ld", _seed);
	_mt = new std::mt19937(_seed);
}

Random::Random(long seed)
{
	CCLOG("Random seed(specified)=%ld", _seed);
	_seed = seed;
	_mt = new std::mt19937(_seed);
}

Random::~Random()
{
	delete _mt;
}

Random* Random::getInstance()
{
	if (s_instance == nullptr) s_instance = new Random();

	return Random::s_instance;
}

void Random::init(long seed)
{
	if (s_instance != nullptr) delete s_instance;

	s_instance = new Random(seed);
}

long Random::getSeed()
{
	return _seed;
}

int Random::getInt()
{
	return (*_mt)();
}

int Random::getInt(int min, int max)
{
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(*_mt);
}

float Random::get0_1()
{
	std::uniform_real_distribution<float> distribution(0, 1.0f);
	return distribution(*_mt);
}

float Random::getMinus1_1()
{
	return getFloat(-1, 1);
}

float Random::getFloat(float min, float max)
{
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(*_mt);
}

float Random::getNormalDistribution(float average, float sigma)
{
	std::normal_distribution<float> distribution(average, sigma);
	return distribution(*_mt);
}

bool Random::getBool()
{
	return ((*_mt)() % 2 == 0);
}

bool Random::probability(float percent)
{
	if (percent <= 0) return false;
	if (percent >= 100) return true;

	return (getFloat(0, 100) <= percent);
}

int clamp(int value, int min, int max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}

void replaceSceneFade(Scene* scene, float duration)
{
	auto transition = TransitionFade::create(duration, scene);
	DIRECTOR->replaceScene(transition);
}


bool isIOS()
{
	return (CC_TARGET_PLATFORM == CC_PLATFORM_IOS);
}

bool isAndroid()
{
	return (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID);
}

bool isWin32()
{
	return (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32);
}

void addChildToCenter(Node* parent, Node* child)
{
	const float x = parent->getContentSize().width / 2;
	const float y = parent->getContentSize().height / 2;

	child->setPosition(x, y);
	parent->addChild(child);
}

void forceFinishActionByTag(Node* target, int tag)
{
	auto action = target->getActionManager()->getActionByTag(tag, target);

	if (action) {
		action->update(1.0f);
		target->stopAction(action);
	}
}
