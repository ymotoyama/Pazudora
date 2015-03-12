#include "ComboLabel.h"
#include "Consts.h"

static const int FONT_SIZE = 30;
static const int OUTLINE_SIZE = 3;

ComboLabel* ComboLabel::create(const Vec2& position, int count)
{
	auto comboLabel = new ComboLabel();

	comboLabel->init(position, count);
	comboLabel->autorelease();

	return comboLabel;
}

bool ComboLabel::init(const Vec2& position, int count)
{
	Node::init();

	_comboCount = count;

	setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	setLocalZOrder(Z_COMBO);
	setTag(TAG_COMBO_LABEL);
	setCascadeOpacityEnabled(true);
	
	auto label = Label::createWithTTF(
		StringUtils::format("COMBO %d", _comboCount),
		"fonts/Marker Felt.ttf",
		FONT_SIZE,
		Size::ZERO,
		TextHAlignment::CENTER,
		TextVAlignment::CENTER);

	auto ttfConfig = label->getTTFConfig();
	ttfConfig.outlineSize = OUTLINE_SIZE;
	label->setTTFConfig(ttfConfig);

	Vec2 offset = Vec2(0, -40);

	label->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	label->setColor(Color3B(255, 0, 0));
	addChild(label);

	// 色アニメーション（黄⇔赤 繰り返し）
	auto tint1 = TintTo::create(0.1f, 255, 255, 0);
	auto tint2 = TintTo::create(0.1f, 255, 0, 0);
	auto seq = Sequence::create(tint1, tint2, nullptr);
	auto repeat = RepeatForever::create(seq);
	label->runAction(repeat);

	// 出現アニメーション
	setPosition(position + offset);
	setScale(1.8f);
	float duration = 0.2f;
	auto jump = JumpBy::create(duration, -offset, 55, 1);
	auto scale = ScaleTo::create(duration, 1.0f);
	auto spawn = Spawn::create(jump, scale, nullptr);
	runAction(spawn);

	return true;
}

int ComboLabel::getComboCount()
{
	return _comboCount;
}

void ComboLabel::hide()
{
	auto scale = ScaleBy::create(0.5f, 2.0f);
	auto delay = DelayTime::create(0.25f);
	auto fade = FadeOut::create(0.2f);
	auto seq = Sequence::create(delay, fade, nullptr);
	auto spawn = Spawn::create(scale, seq, nullptr);
	auto remove = RemoveSelf::create();
	auto seq2 = Sequence::create(spawn, remove, nullptr);
	runAction(seq2);	
}