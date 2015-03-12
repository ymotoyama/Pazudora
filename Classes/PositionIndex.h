#ifndef Position_h__
#define Position_h__

// ドロップの位置（行/列）用構造体
class PositionIndex
{
public:
	int row;
	int column;

	PositionIndex();
	PositionIndex(int aRow, int aColumn);

	// 無効なインデックスの場合はtrueを返却する
	bool isInvalid() const;
	// 二つの距離を返却する。1マス離れている場合は1、斜めは2
	static int distance(const PositionIndex& a, const PositionIndex& b);

	PositionIndex operator+(const PositionIndex& p) const;
	PositionIndex operator-(const PositionIndex& p) const;
	PositionIndex operator*(int amount) const;
	bool operator==(const PositionIndex& p) const;

	// 無効な座標を表す
	static const PositionIndex INVALID_INDEX;

	// 左方向を表す
	static const PositionIndex LEFT;
	// 上方向を表す
	static const PositionIndex UP;
	// 右方向を表す
	static const PositionIndex RIGHT;
	// 下方向を表す
	static const PositionIndex DOWN;
};

#endif // Position_h__
