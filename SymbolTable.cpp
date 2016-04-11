/*
 * SymbolTable.cpp
 *
 *  Created on: Feb 26, 2016
 *      Author: tamnguyen
 */

#include "SymbolTable.h"
#include <iostream>

using namespace std;

void SymbolTable::addConstant(std::string id, std::shared_ptr<Expression> value) {
	auto found = constants.find(id);
	if (found == constants.end()) constants[id] = value;
}

void SymbolTable::addType(std::string id, std::shared_ptr<Type> type) {
	auto found = types.find(id);
	if (found == types.end()) types[id] = type;
}

void SymbolTable::addVariable(std::string id, std::shared_ptr<Type> type) {
	auto found = variables.find(id);
	if (found == variables.end()) {
		variables[id] = std::make_shared<Symbol>(id, type, memoryLocation, memoryOffset);
		memoryOffset += type->size();
	}
}

void SymbolTable::addFunction(std::string id, std::shared_ptr<Function> function) {
	auto found = functions.find(id);
	if (found == functions.end()) {
		functions[id] = function;
	} else {
		if ((!found->second->isDefined()) && function->isDefined()) {
			found->second->setDefined(true);
		} else {
			cout << "Function already defined" << endl;
		}
	}
}

void SymbolTable::addParameter(std::string id, std::shared_ptr<Type> type) {
	auto found = variables.find(id);
	if (found == variables.end()) {
		variables[id] = std::make_shared<Symbol>(id, type, FRAME, parameterOffset);
		parameterOffset += type->size();
	}
}

std::shared_ptr<Expression> SymbolTable::lookupConstant(std::string id) {
	auto found = constants.find(id);
	if (found != constants.end()) return found->second;
	if (parent) return parent->lookupConstant(id);
	return nullptr;
}
std::shared_ptr<Type> SymbolTable::lookupType(std::string id) {
	auto found = types.find(id);
	if (found != types.end()) return found->second;
	if (parent) return parent->lookupType(id);
	return nullptr;
}
std::shared_ptr<Symbol> SymbolTable::lookupVariable(std::string id) {
	auto found = variables.find(id);
	if (found != variables.end()) return found->second;
	if (parent) return parent->lookupVariable(id);
	return nullptr;
}

std::shared_ptr<Function> SymbolTable::lookupFunction(std::string id) {
	auto found = functions.find(id);
	if (found != functions.end()) return found->second;
	if (parent) return parent->lookupFunction(id);
	return nullptr;
}
