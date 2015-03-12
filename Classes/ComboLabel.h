#ifndef ComboLabel_h__
#define ComboLabel_h__

#include "cocos2d.h"
USING_NS_CC;

// コンボ数表示ラベル
class ComboLabel : public Node
{
public:
	static ComboLabel* create(const Vec2& position, int count);
	bool init(const Vec2& position, int count);
	int getComboCount();
	void hide();
private:
	int _comboCount;
};

#endif // ComboLabel_h__
