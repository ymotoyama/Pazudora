#include "PartyMemberAttackEffect.h"
#include "Consts.h"
#include "MotoUtil.h"

void PartyMemberAttackEffect::fire(Node* src, Node* dest, Attribute attribute)
{
	fire(GET_SCREEN_POS(src), GET_SCREEN_POS(dest), attribute);
}

void PartyMemberAttackEffect::fire(const Vec2& start, const Vec2& end, Attribute attribute)
{
	create(start, end, attribute);
}

PartyMemberAttackEffect* PartyMemberAttackEffect::create(const Vec2& start, const Vec2& end, Attribute attribute)
{
	auto p = new PartyMemberAttackEffect();

	p->init(start, end, attribute);
	p->autorelease();

	return p;
}

bool PartyMemberAttackEffect::init(const Vec2& start, const Vec2& end, Attribute attribute)
{
	Sprite::initWithFile("particle_texture.png");

	// 属性→色テーブル
	static const std::map<Attribute, Color3B> colorTable =
	{
		{ Attribute::FIRE,  Color3B(255, 0, 0) },
		{ Attribute::AQUA,  Color3B(0, 0, 255) },
		{ Attribute::WOOD,  Color3B(0, 255, 0) },
		{ Attribute::DARK,  Color3B(150, 0, 150) },
		{ Attribute::LIGHT, Color3B(255, 255, 0) },
	};

	setColor(colorTable.at(attribute));
	setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	setBlendFunc(BlendFunc::ADDITIVE);
	setPosition(start);

	auto move = MoveTo::create(ATTACK_EFFECT_DURATION, end);
	auto remove = RemoveSelf::create();
	auto seq = SEQUENCE(move, remove);

	runAction(seq);

	Director::getInstance()->getRunningScene()->addChild(this, Z_PARTY_ATTACK_EFFECT);

	return true;
}
