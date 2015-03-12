#include "AttackPowerChargeEffect.h"
#include "Consts.h"

AttackPowerChargeEffect* AttackPowerChargeEffect::create(const Vec2& start, const Vec2& end)
{
	auto a = new AttackPowerChargeEffect();

	a->init(start, end);
	a->autorelease();

	return a;
}

bool AttackPowerChargeEffect::init(const Vec2& start, const Vec2& end)
{
	Sprite::initWithFile("particle_texture.png");

	setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	setPosition(start);
	setLocalZOrder(Z_CHAGE_ATTACK_EFFECT);
	setBlendFunc(BlendFunc::ADDITIVE);

	auto jump = JumpTo::create(ATTACK_CHAGE_EFFECT_DURATION, end, 130, 1);
	auto remove = RemoveSelf::create();
	auto seq = Sequence::create(jump, remove, nullptr);
	runAction(seq);

	return true;
}
