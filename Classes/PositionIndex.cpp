#include "PositionIndex.h"
#include "Consts.h"
#include <math.h>
#include <complex>

PositionIndex::PositionIndex()
{
	row = -1;
	column = -1;
}

PositionIndex::PositionIndex(int aRow, int aColumn)
{
	row = aRow;
	column = aColumn;
}

bool PositionIndex::isInvalid() const
{
	if (*this == INVALID_INDEX)
		return true;

	return (row < 0 || row >= ROW_COUNT || column < 0 || column >= COLUMN_COUNT);
}

PositionIndex PositionIndex::operator+(const PositionIndex& p) const
{
	return PositionIndex(row + p.row, column + p.column);
}

PositionIndex PositionIndex::operator-(const PositionIndex& p) const
{
	return PositionIndex(row - p.row, column - p.column);
}

PositionIndex PositionIndex::operator*(int amount) const
{
	return PositionIndex(row * amount, column * amount);
}

bool PositionIndex::operator==(const PositionIndex& p) const
{
	return (row == p.row && column == p.column);
}

int PositionIndex::distance(const PositionIndex& a, const PositionIndex& b)
{
	int rowDistance = std::abs(a.row - b.row);
	int columnDistance = std::abs(a.column - b.column);

	return (rowDistance + columnDistance);
}


const PositionIndex PositionIndex::INVALID_INDEX = PositionIndex(-1, -1);
const PositionIndex PositionIndex::LEFT = PositionIndex(0, -1);
const PositionIndex PositionIndex::UP = PositionIndex(1, 0);
const PositionIndex PositionIndex::RIGHT = PositionIndex(0, 1);
const PositionIndex PositionIndex::DOWN = PositionIndex(-1, 0);