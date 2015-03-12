#include "DropMatchChecker.h"

static const int UNMARKED = -1;
static const int UNMATCHED_MARK = 0;

DropMatchChecker::DropMatchChecker(Drop* drops[][COLUMN_COUNT])
: _drops(drops)
, _groupId(0)
{
	// 解析結果テーブル（_resultTable）の初期化
	for (int row = 0; row < ROW_COUNT; row++)
	{
		for (int column = 0; column < COLUMN_COUNT; column++)
		{
			_resultTable[row][column] = UNMARKED;
		}
	}
}

std::vector<std::vector<Drop*>> DropMatchChecker::analyze(Drop* drops[][COLUMN_COUNT])
{
	auto instance = new DropMatchChecker(drops);

	return instance->analyze();
}

std::vector<std::vector<Drop*>> DropMatchChecker::analyze()
{
	// マスを解析していく。
	// 同時に消せるマスには同じ番号が入る。
	// 消せないマスには0が入る。
	for (int row = 0; row < ROW_COUNT; row++)
	{
		for (int column = 0; column < COLUMN_COUNT; column++)
		{
			_groupId++;

			auto index = PositionIndex(row, column);

			// 解析済みの場合はスキップする
			if (isAlreadyMarked(index))
				continue;

			_currentAttribute = _drops[row][column]->getAttribute();

			markAndDig(index);
		}
	}

	std::vector<std::vector<Drop*>> matchedDrops;

	// _resultTableから同じ番号のドロップを拾っていき、
	// 3個以上あれば、最終結果のvectorに追加する。
	for (int id = 1; id < _groupId; id++)
	{
		std::vector<Drop*> aSetOfDrops;

		for (int row = 0; row < ROW_COUNT; row++)
		{
			for (int column = 0; column < COLUMN_COUNT; column++)
			{
				if (_resultTable[row][column] == id)
				{
					aSetOfDrops.push_back(_drops[row][column]);
				}
			}
		}

		if (aSetOfDrops.size() >= REQUIED_MATCH_NUM)
		{
			matchedDrops.push_back(aSetOfDrops);
		}
	}

	return matchedDrops;
}


void DropMatchChecker::markAndDig(const PositionIndex& index)
{
	if (_resultTable[index.row][index.column] != UNMARKED)
	{
		// 既にチェック済みであるマスであるため、何もしない
		return;
	}
	else if (_resultTable[index.row][index.column] == UNMARKED)
	{
		// とりあえずチェック済みとする
		_resultTable[index.row][index.column] = UNMATCHED_MARK;
	}

	static const PositionIndex directions[4] =
	{
		PositionIndex::LEFT,
		PositionIndex::UP,
		PositionIndex::RIGHT,
		PositionIndex::DOWN,
	};

	// 自分の周囲4方向が同じ属性の場合、掘り進める（1マス進んでまた同じ調査を繰り返す）。
	for (auto direction : directions)
	{
		const auto nextIndex = index + direction;

		if (canDig(nextIndex))
			markAndDig(nextIndex);
	}

	// 右方向に同属性のドロップが揃っているか調べ、揃っていたら現在のgroupIdを書き込む
	checkAndMark(index, PositionIndex::RIGHT, REQUIED_MATCH_NUM);

	// 上方向に同属性のドロップが揃っているか調べ、揃っていたら現在のgroupIdを書き込む
	checkAndMark(index, PositionIndex::UP, REQUIED_MATCH_NUM);
}

bool DropMatchChecker::isDropSameAttribute(const PositionIndex& index)
{
	return (_drops[index.row][index.column]->getAttribute() == _currentAttribute);
}

bool DropMatchChecker::isAlreadyMarked(const PositionIndex& index)
{
	return (_resultTable[index.row][index.column] >= 0);
}

bool DropMatchChecker::canDig(const PositionIndex& index)
{
	if (index.isInvalid())
		return false;

	if (!isDropSameAttribute(index))
		return false;

	if (isAlreadyMarked(index))
		return false;

	return true;
}

void DropMatchChecker::checkAndMark(const PositionIndex& origin, const PositionIndex& direction, int requiredNum)
{
	Attribute attribute = _drops[origin.row][origin.column]->getAttribute();
	
	// 調査範囲のマスに、以下のものが含まれていたら、揃っていないということなので調査終了
	// ・無効なマス
	// ・異なる属性のマス
	for (int i = 0; i < requiredNum; i++)
	{
		const auto posIndex = (origin + (direction * i));

		if (posIndex.isInvalid())
		{
			return;
		}

		if (!isDropSameAttribute(posIndex))
		{
			return;
		}
	}

	// ここまで来たら揃っているということなので、groupIdを書き込む
	for (int i = 0; i < requiredNum; i++)
	{
		const auto posIndex = (origin + (direction * i));

		_resultTable[posIndex.row][posIndex.column] = _groupId;
	}
}

void DropMatchChecker::mark(const PositionIndex& index)
{
	_resultTable[index.row][index.column] = _groupId;
}
