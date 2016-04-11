/*
 * StringTable.h
 *
 *  Created on: Feb 26, 2016
 *      Author: Tam Nguyen
 */

#ifndef STRINGTABLE_H_
#define STRINGTABLE_H_

#include <memory>
#include <map>
#include <iostream>

class StringTable {
public:
	static std::shared_ptr<StringTable> getInstance();

	StringTable() = default;
	StringTable(const StringTable& rhs) = delete;
	StringTable& operator=(const StringTable& rhs) = delete;

	int put(std::string value);
	std::string get(int id);
	int size() { return table.size(); }
	std::map<std::string, int> getTable() const { return table; }

	friend std::ostream& operator<<(std::ostream& os, const StringTable& rhs);
private:
	static std::shared_ptr<StringTable> instance;
	std::map<std::string, int> table;
	std::map<int, std::string> idTable;
};

#endif /* STRINGTABLE_H_ */
