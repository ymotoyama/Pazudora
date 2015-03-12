#ifndef DropMatchChecker_h__
#define DropMatchChecker_h__

#include "cocos2d.h"
USING_NS_CC;
#include "Consts.h"
#include "Drop.h"
#include "PositionIndex.h"

// ドロップの盤面解析君
class DropMatchChecker
{
public:
	// ドロップの盤面を解析し、消せるドロップの集合の集合を返却する。
	// 消せるドロップが無い場合は長さゼロのvectorを返却する。
	// 結果の並び順は本家パズドラ準拠。
	//   左下→右下
	//   一段上に行って左→右
	//   一段上に行って左→右
	//   という順にチェックし、見つかった順となっている。
	static std::vector<std::vector<Drop*>> analyze(Drop* drops[][COLUMN_COUNT]);

private:
	// 盤面のポインタ
	Drop* (*_drops)[COLUMN_COUNT];
	// 解析結果テーブル。
	// 同時に消せる場所は同じ番号(groupId)が振られる。
	// 未解析: -1
	// 解析済み（消せない）: 0
	// 解析済み（消せる）: 1以上の値
	int _resultTable[ROW_COUNT][COLUMN_COUNT];
	// 解析結果に振るための番号。
	int _groupId;
	// 現在調査中の属性
	Attribute _currentAttribute;

	// コンストラクタ（private）
	DropMatchChecker(Drop* drops[][COLUMN_COUNT]);
	// 解析実行
	std::vector<std::vector<Drop*>> analyze();
	// 指定したマスを解析し、結果を解析結果テーブルに書き込む。
	// 再帰的に隣接するマスも解析していく。
	// 同属性が3個以上並んでいる場合、同じ番号が振られる。
	// 消せない場合、0を書き込む。
	void markAndDig(const PositionIndex& index);
	// 指定した場所のドロップと同じ属性か？
	bool isDropSameAttribute(const PositionIndex& index);
	// 指定した場所は既に解析済みか？
	bool isAlreadyMarked(const PositionIndex& index);
	// 指定した場所は掘り進めることが可能か？（解析可能か？）
	bool canDig(const PositionIndex& index);
	// 同じ色のドロップが続いているかどうかをチェックし、
	// 続いていれば、現在のgroupIdを書き込む。
	// origin : 起点
	// direction : 調べる方向。PositionIndex::RIGHTまたはPositionIndex::UPを指定する。
	// requiredNum : 「揃った」とする最低個数。
	void checkAndMark(const PositionIndex& origin, const PositionIndex& direction, int requiredNum);
	// 指定した場所に現在のgroupIdを書き込む
	void mark(const PositionIndex& index);
};


#endif // DropMatchChecker_h__
