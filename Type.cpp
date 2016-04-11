/*
 * Type.cpp
 *
 *  Created on: Feb 27, 2016
 *      Author: tamnguyen
 */


#include "Type.h"
#include <iostream>

using namespace std;

std::shared_ptr<Type> BuiltInType::intType;
std::shared_ptr<Type> BuiltInType::charType;
std::shared_ptr<Type> BuiltInType::boolType;
std::shared_ptr<Type> BuiltInType::stringType;

std::shared_ptr<Type> BuiltInType::getInt() {
	if (!intType) intType = std::make_shared<IntType>();
	return intType;
}

std::shared_ptr<Type> BuiltInType::getChar() {
	if (!charType) charType = std::make_shared<CharType>();
	return charType;
}

std::shared_ptr<Type> BuiltInType::getBool() {
	if (!boolType) boolType = std::make_shared<BoolType>();
	return boolType;
}

std::shared_ptr<Type> BuiltInType::getString() {
	if (!stringType) stringType = std::make_shared<StringType>();
	return stringType;
}

int RecordType::size() {
	int sum = 0;
	for (auto& elem: fieldMap) {
		sum += elem.second->size();
	}
	return sum;
}

void RecordType::addField(std::string fieldName, std::shared_ptr<Type> fieldType) {
	fieldMap[fieldName] = fieldType;
	fieldOffsetMap[fieldName] = currentOffset;
	currentOffset += fieldType->size();
}

std::shared_ptr<Type> RecordType::getFieldType(std::string fieldName) const {
	auto found = fieldMap.find(fieldName);
	if (found == fieldMap.end()) {
		cout << "Cannot find member " + fieldName << endl;
		return nullptr;
	}
	return found->second;
}

int RecordType::getFieldOffset(std::string fieldName) const {
	auto found = fieldOffsetMap.find(fieldName);
	if (found == fieldOffsetMap.end()) {
		cout << "Cannot find member " + fieldName << endl;
		return -1;
	}
	return found->second;
}
