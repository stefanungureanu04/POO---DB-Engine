#include "Column.h"

Column::Column(const std::string& name, const std::string& type)
{
	this->name = name;
	this->type = type;
}

void Column::setPrimaryKey()
{
	isPrimaryKey = true;
}

void Column::setForeignKey(const std::string& foreignTable, const std::string& foreignColumn)
{
	isForeignKey = true;
	this->foreignTable = foreignTable;
	this->foreignColumn = foreignColumn;
}

std::string Column::getName() const
{
	return name;
}

std::string Column::getType() const
{
	return type;
}

bool Column::isPK() const
{
	return isPrimaryKey;
}

bool Column::isFK() const
{
	return isForeignKey;
}

std::string Column::getFKTable() const
{
	return foreignTable;
}

std::string Column::getFKColumn() const
{
	return foreignColumn;
}
