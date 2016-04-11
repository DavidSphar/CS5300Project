/*
 * Expression.h
 *
 *  Created on: Feb 26, 2016
 *      Author: tamnguyen
 */

#ifndef EXPRESSION_H_
#define EXPRESSION_H_

#include <memory>
#include "Type.h"
#include "Register.h"

class LValue;

class Expression {
public:
    Expression(std::shared_ptr<Type> type); // runtime value
    Expression(std::shared_ptr<Type> type, int value); // const value
    Expression(std::shared_ptr<LValue> lvalue);
	Expression(const Expression& rhs) = delete;
	Expression& operator=(const Expression& rhs) = delete;
	~Expression() = default;

    void setType(std::shared_ptr<Type> t) { type = t; }
    void setValue(int v) { value = v; }
	std::shared_ptr<Type> getType() const { return type; }
	int getValue() const { return value; }
	std::string getRegister() const { return reg->getName(); }
	bool isConst() const { return constCheck; }
	bool isString() const { return type == BuiltInType::getString(); }
	bool isLValue() const { return lvalue != nullptr; }
	std::shared_ptr<LValue> getLValue() const { return lvalue; }
private:
	std::shared_ptr<Type> type = nullptr;
	std::unique_ptr<Register> reg = nullptr;
	int value = 0; // integer | boolean | char | string id 
	bool constCheck = false;
	std::shared_ptr<LValue> lvalue = nullptr;
};



#endif /* EXPRESSION_H_ */
