#include "Drop.h"
#include "Consts.h"
#include "MotoUtil.h"
#include "GameScene.h"
#include "MotoUtil.h"

static const float SWAP_DURATION = 0.13f;

Drop::Drop(Attribute attribute, int row, int column)
	: _attribute(attribute)
	, _positionIndex(row, column)
{
}

Drop* Drop::create(int row, int column)
{
	Attribute kind = (Attribute)(RAND_INT(0, ATTRIBUTE_NUM - 1));
	return create(kind, row, column);
}

Drop* Drop::create(Attribute attribute, int row, int column)
{
	auto drop = new Drop(attribute, row, column);

	drop->init();
	drop->autorelease();

	return drop;
}

bool Drop::init()
{
	Sprite::initWithFile(getImageFileName(_attribute));

	setTag(TAG_DROP);
	setScale(DROP_SIZE / getContentSize().width);

	return true;
}

const std::string& Drop::getImageFileName(Attribute attribute)
{
	// 属性と画像ファイル名の対応表
	static const std::map<Attribute, std::string> imageFileNameTable =
	{
		{ Attribute::FIRE, "fire.png" },
		{ Attribute::AQUA, "aqua.png" },
		{ Attribute::WOOD, "wood.png" },
		{ Attribute::LIGHT, "light.png" },
		{ Attribute::DARK, "dark.png" },
		{ Attribute::LIFE, "life.png" },
	};

	return imageFileNameTable.at(attribute);
}


Attribute Drop::getAttribute()
{
	return _attribute;
}

const PositionIndex& Drop::getPositionIndex()
{
	return _positionIndex;
}

void Drop::setPositionIndex(const PositionIndex& position)
{
	_positionIndex = position;
}

void Drop::moveTo(const PositionIndex& dest)
{
	// 現在一つ前の移動処理中かもしれないので、直ちに理想の位置まで移動する
	setPositionToIdeal();

	auto srcCoord = GameScene::calcDropPosition(_positionIndex);
	auto destCoord = GameScene::calcDropPosition(dest);

	auto direction = dest - _positionIndex;

	Vec2 pivot = (destCoord + srcCoord) / 2;
	float fromAngle = (srcCoord - pivot).getAngle();
	float toAngle = fromAngle + 3.1415f;
	float radius = (destCoord - srcCoord).length() / 2;

	float fromAngleDeg = CC_RADIANS_TO_DEGREES(fromAngle);
	float toAngleDeg = CC_RADIANS_TO_DEGREES(toAngle);
	
	auto move = CustomAction::create(SWAP_DURATION, fromAngle, toAngle, [=](float angle){
		auto position = pivot + Vec2(std::cos(angle), std::sin(angle)) * radius;
		setPosition(position);
	});

	runAction(move);

	setPositionIndex(dest);

	setLocalZOrder(Z_SWAPPING_DROP);

	EXEC_DELAY(SWAP_DURATION, [this](){setPositionToIdeal(); }, this);
}

void Drop::setDragging(bool isDragging)
{
	if (isDragging)
	{
		setOpacity(120);
	}
	else
	{
		setOpacity(255);
	}
}

void Drop::scheduleRemove(float delayTime)
{
	setPositionToIdeal();
	runAction(SEQUENCE(DELAY(delayTime), FADEOUT_AND_REMOVE(0.15f)));
}

void Drop::setPositionToIdeal()
{
	stopAllActions();
	auto srcCoord = GameScene::calcDropPosition(_positionIndex);
	setPosition(srcCoord);

	setLocalZOrder(Z_DROP);
}