/*
 * LValue.h
 *
 *  Created on: Feb 27, 2016
 *      Author: tamnguyen
 */

#ifndef LVALUE_H_
#define LVALUE_H_
#include "Symbol.h"
#include "SymbolTable.h"
#include <memory>

class LValue {
public:
	LValue(std::shared_ptr<SymbolTable> t): symbolTable(t) {}
	virtual ~LValue() = default;
	virtual std::shared_ptr<Expression> address() const = 0;
	virtual std::shared_ptr<Type> type() const = 0;
	virtual bool isConst() const = 0;
	virtual std::shared_ptr<Expression> value() const = 0;
protected:
	std::shared_ptr<SymbolTable> symbolTable;
};

class IdAccess: public LValue {
public:
	IdAccess(std::string n, std::shared_ptr<SymbolTable> t): LValue(t), id(n) { }
	std::shared_ptr<Type> type() const;
	std::shared_ptr<Expression> address() const;
	bool isConst() const;
	std::shared_ptr<Expression> value() const;
private:
	std::string id;
};

class MemberAccess: public LValue {
public:
	MemberAccess(std::shared_ptr<LValue> base, std::string field, std::shared_ptr<SymbolTable> t): LValue(t), base(base), field(field) { }
	std::shared_ptr<RecordType> btype() const;
	std::shared_ptr<Type> type() const;
	std::shared_ptr<Expression> address() const;
	bool isConst() const;
	std::shared_ptr<Expression> value() const;
private:
	std::shared_ptr<LValue> base;
	std::string field;
};


class ArrayAccess: public LValue {
public:
	ArrayAccess(std::shared_ptr<LValue> base, std::shared_ptr<Expression> e, std::shared_ptr<SymbolTable> t): LValue(t), base(base), expr(e) { }
	std::shared_ptr<Expression> address() const;
	std::shared_ptr<ArrayType> atype() const;
	std::shared_ptr<Type> type() const;
	bool isConst() const;
	std::shared_ptr<Expression> value() const;
private:
	std::shared_ptr<LValue> base;
	std::shared_ptr<Expression> expr;
};


#endif /* LVALUE_H_ */
