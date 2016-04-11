/*
 * StringTable.cpp
 *
 *  Created on: Feb 26, 2016
 *      Author: Tam Nguyen
 */

#include "StringTable.h"

using namespace std;


shared_ptr<StringTable> StringTable::instance;

shared_ptr<StringTable> StringTable::getInstance() {
	if (!instance) {
		instance = shared_ptr<StringTable>(new StringTable());
        instance->put("\"\""); // default string 
    }
	return instance;
}


int StringTable::put(string value) {
	auto it = table.find(value);
	if (it != table.end()) return it->second;
	auto newId = table.size();
	table[value] = newId;
	idTable[newId] = value;
	return newId;
}

string StringTable::get(int id) {
	return idTable[id];
}


std::ostream& operator<<(std::ostream& os, const StringTable& rhs) {
	os << "String Table: " << endl;
	for (auto& it: rhs.getTable()) {
		cout << '"' << it.first << '"' << ": " << it.second << endl;
	}
	return os;
}
