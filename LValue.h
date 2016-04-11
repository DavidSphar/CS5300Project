/*
 * LValue.h
 *
 *  Created on: Feb 27, 2016
 *      Author: tamnguyen
 */

#ifndef LVALUE_H_
#define LVALUE_H_
#include "Symbol.h"
#include <memory>

class LValue {
public:
	LValue(std::shared_ptr<Type> type, MemoryLocation memoryLocation, int memoryOffset): type(type), memoryLocation(memoryLocation), memoryOffset(memoryOffset) {}
	LValue(std::shared_ptr<Type> type, std::shared_ptr<Expression> constEx): type(type), constEx(constEx) { constCheck = true; }
	~LValue() = default;
	MemoryLocation getMemoryLocation() { return memoryLocation; }
	int getMemoryOffset() { return memoryOffset; }
	std::shared_ptr<Type> getType() { return type; }
	std::shared_ptr<Expression> getConstEx() const { return constEx; }
	bool isConst() const { return constCheck; }
	std::string getAddress() {
		if (memoryLocation == GLOBAL) {
			return std::to_string(memoryOffset)+"($gp)";
		}
		if (memoryLocation == STACK) {
			auto finalOffset = memoryOffset + getType()->size();
			return "-" + std::to_string(finalOffset) + "($fp)";
		}
		if (memoryLocation == FRAME) {
			return std::to_string(memoryOffset)+"($fp)";
		}
		return "";
	}
	bool isRef() const { return std::dynamic_pointer_cast<ReferenceType>(type) != nullptr; }
private:
	std::shared_ptr<Type> type;
	MemoryLocation memoryLocation = GLOBAL;
	int memoryOffset = 0;
	std::shared_ptr<Expression> constEx;
	bool constCheck = false;
};

#endif /* LVALUE_H_ */
