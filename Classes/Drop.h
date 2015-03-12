#ifndef Drop_h__
#define Drop_h__

#include "cocos2d.h"
#include "Attribute.h"
#include "PositionIndex.h"

USING_NS_CC;

class Drop : public Sprite
{
public:
	Drop(Attribute attribute, int row, int column);
	// ランダムな属性のドロップを生成
	static Drop* create(int row, int column);
	static Drop* create(Attribute attribute, int row, int column);
	bool init();

	// 属性を返却する
	Attribute getAttribute();

	// 行/列インデックスを返却する
	const PositionIndex& getPositionIndex();

	// 行/列インデックスを設定する
	void setPositionIndex(const PositionIndex& position);

	// 隣（斜め含む）の座標に移動する。
	// （本家のようにクルっと半回転して移動する）
	void moveTo(const PositionIndex& dest);

	// ドラッグ中かどうかを設定する（見た目に影響する）
	void setDragging(bool isDragging);

	// 指定時間後に消滅する
	void scheduleRemove(float delayTime);

private:
	// 指定された属性の画像ファイル名を取得する。
	const std::string& getImageFileName(Attribute attribute);

	// 属性
	Attribute _attribute;

	// 行/列インデックス
	PositionIndex _positionIndex;

	// アクションをキャンセルして理想位置に移動する
	void setPositionToIdeal();
};

#endif