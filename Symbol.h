/*
 * Symbol.h
 *
 *  Created on: Feb 26, 2016
 *      Author: tamnguyen
 */

#ifndef SYMBOL_H_
#define SYMBOL_H_

#include <string>
#include <memory>
#include <iostream>
#include "Type.h"

enum MemoryLocation { GLOBAL, STACK, FRAME };

class Symbol {
public:
	Symbol(std::string id,
		   std::shared_ptr<Type> type,
		   MemoryLocation memoryLocation,
		   int memoryOffset): id(id), type(type),
				   memoryLocation(memoryLocation), memoryOffset(memoryOffset) {}
	virtual ~Symbol() = default;
	std::string getId() { return id; }
	std::shared_ptr<Type> getType() { return type; }
	MemoryLocation getMemoryLocation() { return memoryLocation; }
	int getMemoryOffset() { return memoryOffset; }
	friend std::ostream& operator<<(std::ostream& os, const Symbol& rhs) {
		os << "VAR {";
		os << "Id: " << rhs.id << ", ";
		os << "Type: " << rhs.type->name() << ", ";
		os << "Scope: " << rhs.memoryLocation << ", ";
		os << "Offset: " << rhs.memoryOffset << "}";
		return os;
	}
private:
	std::string id;
	std::shared_ptr<Type> type;
	MemoryLocation memoryLocation;
	int memoryOffset;
};

#endif /* SYMBOL_H_ */
