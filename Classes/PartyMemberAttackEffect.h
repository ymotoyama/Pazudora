#ifndef PartyMemberAttackEffect_h__
#define PartyMemberAttackEffect_h__

#include "cocos2d.h"
USING_NS_CC;

#include "Attribute.h"
#include "PartyMember.h"
#include "Enemy.h"

class PartyMemberAttackEffect : protected Sprite
{
public:
	static void fire(Node* src, Node* dest, Attribute attribute);
	static void fire(const Vec2& start, const Vec2& end, Attribute attribute);

private:
	static PartyMemberAttackEffect* create(const Vec2& start, const Vec2& end, Attribute attribute);
	bool init(const Vec2& start, const Vec2& end, Attribute attribute);
};

#endif // PartyMemberAttackEffect_h__
