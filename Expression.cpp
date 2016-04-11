/*
 * Expression.cpp
 *
 *  Created on: Feb 26, 2016
 *      Author: tamnguyen
 */

#include "Expression.h"
#include "LValue.h"


Expression::Expression(std::shared_ptr<Type> type): type(type) {
	reg = Register::allocate();
    constCheck = false;
}

Expression::Expression(std::shared_ptr<Type> type, int value): type(type), value(value) {
    constCheck = true;
}

Expression::Expression(std::shared_ptr<LValue> lvalue): lvalue(lvalue) {
	reg = Register::allocate();
	type = lvalue->getType();
	auto refType = std::dynamic_pointer_cast<ReferenceType>(lvalue->getType());
	if (refType) type = refType->getBaseType();
	constCheck = false;
}

