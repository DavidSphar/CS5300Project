/*
 * SymbolTable.h
 *
 *  Created on: Feb 26, 2016
 *      Author: tamnguyen
 */

#ifndef SYMBOLTABLE_H_
#define SYMBOLTABLE_H_

#include <memory>
#include <map>
#include "Symbol.h"
#include "Type.h"
#include "Expression.h"
#include "Function.h"

class SymbolTable {
public:
	SymbolTable(std::shared_ptr<SymbolTable> parent, MemoryLocation memoryLocation):
		parent(parent), memoryLocation(memoryLocation), memoryOffset(0), parameterOffset(0) {}

	void addConstant(std::string id, std::shared_ptr<Expression> value);
	void addType(std::string id, std::shared_ptr<Type> type);
	void addVariable(std::string id, std::shared_ptr<Type> type);
	void addFunction(std::string id, std::shared_ptr<Function> function);
	void addParameter(std::string id, std::shared_ptr<Type> type);

	std::shared_ptr<Expression> lookupConstant(std::string id);
	std::shared_ptr<Type> lookupType(std::string id);
	std::shared_ptr<Symbol> lookupVariable(std::string id);
	std::shared_ptr<Function> lookupFunction(std::string id);

	std::shared_ptr<SymbolTable> getParent() const { return parent; }
	MemoryLocation getMemoryLocation() const { return memoryLocation; }
	int getMemoryOffset() const { return memoryOffset; }
	void setMemoryOffset(int offset) { memoryOffset = offset; }
private:
	std::shared_ptr<SymbolTable> parent;
	MemoryLocation memoryLocation;
	int memoryOffset;
	int parameterOffset;
	std::map<std::string, std::shared_ptr<Type>> types;
	std::map<std::string, std::shared_ptr<Symbol>> variables;
	std::map<std::string, std::shared_ptr<Expression>> constants;
	std::map<std::string, std::shared_ptr<Function>> functions;
};

#endif /* SYMBOLTABLE_H_ */
