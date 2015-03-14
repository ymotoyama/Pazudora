#include "BattleCountLabel.h"
#include "Consts.h"

void BattleCountLabel::show(int currentCount, int maxCount, Node* scene)
{
	auto moveDistance = Vec2(-200, 0);
	auto position = Vec2(RESOLUTION_X / 2, RESOLUTION_Y / 2 + 280);

	auto label = Label::createWithTTF(
		StringUtils::format("Battle %d/%d", currentCount, maxCount),
		"fonts/Marker Felt.ttf",
		40,
		Size::ZERO,
		TextHAlignment::CENTER,
		TextVAlignment::CENTER);
	label->setColor(Color3B::WHITE);
	label->setPosition(position - moveDistance);

	auto config = label->getTTFConfig();
	config.outlineSize = 3;
	label->setTTFConfig(config);

	scene->addChild(label, Z_BATTLE_COUNT_LABEL);
	
	auto move1 = EaseQuarticActionOut::create(MoveTo::create(0.3f, position));
	auto fedein = FadeIn::create(0.2f);
	auto spawn1 = Spawn::create(move1, fedein, nullptr);

	auto wait = DelayTime::create(0.8f);

	auto move2 = EaseQuarticActionIn::create(MoveBy::create(0.3f, moveDistance));
	auto delay = DelayTime::create(0.1f);
	auto fadeout = FadeOut::create(0.2f);
	auto spawn2 = Spawn::create(move2, Sequence::create(delay, fadeout, nullptr), nullptr);

	auto remove = RemoveSelf::create();

	auto seq = Sequence::create(spawn1, wait, spawn2, remove, nullptr);

	label->runAction(seq);
}

