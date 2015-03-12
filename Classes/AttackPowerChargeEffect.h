#ifndef AttackPowerChargeEffect_h__
#define AttackPowerChargeEffect_h__

#include "cocos2d.h"
USING_NS_CC;

class AttackPowerChargeEffect : public Sprite
{
public:
	static AttackPowerChargeEffect* create(const Vec2& start, const Vec2& end);
	bool init(const Vec2& start, const Vec2& end);

private:
};

#endif // AttackPowerChargeEffect_h__
