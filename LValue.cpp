/*
 * LValue.cpp
 *
 *  Created on: Apr 20, 2016
 *      Author: tamnguyen
 */



#include <fstream>
#include <iostream>
#include "LValue.h"
using namespace std;
extern ofstream fout;
extern void yyerror(const char *msg);


std::shared_ptr<Expression> IdAccess::address() const {
	auto variable = symbolTable->lookupVariable(id);
	if (!variable) { yyerror("Invalid variable lookup"); return nullptr; }
	auto location = variable->getMemoryLocation();
	auto offset = variable->getMemoryOffset();
	auto tempRegister = Register::allocate();
	if (location == GLOBAL) {
		fout << "\t" << "move " << tempRegister->getName() << ", $gp" << endl;
		fout << "\t" << "addi " << tempRegister->getName() << ", " << tempRegister->getName() << ", " << offset << endl;
	} else if (location == STACK) {
		auto finalOffset = offset + variable->getType()->size();
		fout << "\t" << "move " << tempRegister->getName() << ", $fp" << endl;
		fout << "\t" << "addi " << tempRegister->getName() << ", " << tempRegister->getName() << ", -" << finalOffset << endl;
	} else {
		fout << "\t" << "move " << tempRegister->getName() << ", $fp" << endl;
		fout << "\t" << "addi " << tempRegister->getName() << ", " << tempRegister->getName() << ", " << offset << endl;
	}

	auto addressExpression = make_shared<Expression>(BuiltInType::getInt());
	if (dynamic_pointer_cast<ReferenceType>(variable->getType())) {
		fout << "\t" << "lw " << addressExpression->getRegister() << ", 0(" << tempRegister->getName() << ")" << endl;
		return addressExpression;
	} else {
		fout << "\t" << "move " << addressExpression->getRegister() << ", " << tempRegister->getName() << endl;
		return addressExpression;
	}
}

std::shared_ptr<Type> IdAccess::type() const {
	if (isConst()) return symbolTable->lookupConstant(id)->getType();
	auto variable = symbolTable->lookupVariable(id);
	if (!variable) { yyerror("Invalid variable lookup"); return nullptr; }
	auto refType = dynamic_pointer_cast<ReferenceType>(variable->getType());
	if (refType) return refType->getBaseType();
	return variable->getType();
}

bool IdAccess::isConst() const {
	return (symbolTable->lookupConstant(id) != nullptr);
}

std::shared_ptr<Expression> IdAccess::value() const {
	return symbolTable->lookupConstant(id);
}


std::shared_ptr<Expression> MemberAccess::address() const {
	auto baseAddress = base->address();
	auto offset = btype()->getFieldOffset(field);
	auto addressExpression = make_shared<Expression>(BuiltInType::getInt());
	fout << "\t" << "addi " << addressExpression->getRegister() << ", " << baseAddress->getRegister() << ", " << offset << endl;
	return addressExpression;
}

std::shared_ptr<RecordType> MemberAccess::btype() const {
	auto refType = dynamic_pointer_cast<ReferenceType>(base->type());
	if (refType) return dynamic_pointer_cast<RecordType>(refType->getBaseType());
	return dynamic_pointer_cast<RecordType>(base->type());
}

std::shared_ptr<Type> MemberAccess::type() const {
	return btype()->getFieldType(field);
}

bool MemberAccess::isConst() const {
	return false;
}

std::shared_ptr<Expression> MemberAccess::value() const {
	return nullptr;
}

std::shared_ptr<Expression> ArrayAccess::address() const {
	auto baseAddress = base->address();
	auto arrayType = atype();
	if (expr->isConst()) {
		auto offset = arrayType->getBaseType()->size() * (expr->getValue() - arrayType->getLowerBound());
		if (offset == 0) return baseAddress;
		else {
			auto addressExpression = make_shared<Expression>(BuiltInType::getInt());
			fout << "\t" << "addi " << addressExpression->getRegister() << ", " << baseAddress->getRegister() << ", " << offset << endl;
			return addressExpression;
		}
	} else {
		auto tempRegister1 = Register::allocate();
		fout << "\t" << "li " << tempRegister1->getName() << ", " << arrayType->getLowerBound() << endl;
		auto tempRegister2 = Register::allocate();
		fout << "\t" << "sub " << tempRegister2->getName() << ", " << expr->getRegister() << ", " << tempRegister1->getName() << endl;
		fout << "\t" << "li " << tempRegister1->getName() << ", " << arrayType->getBaseType()->size() << endl;
		auto tempRegister3 = Register::allocate();
		fout << "\t" << "mul " << tempRegister3->getName() << ", " << tempRegister2->getName() << ", " << tempRegister1->getName() << endl;
		auto addressExpression = make_shared<Expression>(BuiltInType::getInt());
		fout << "\t" << "add " << addressExpression->getRegister() << ", " << baseAddress->getRegister() << ", " << tempRegister3->getName() << endl;
		return addressExpression;
	}
}

std::shared_ptr<ArrayType> ArrayAccess::atype() const {
	auto refType = dynamic_pointer_cast<ReferenceType>(base->type());
	if (refType) return dynamic_pointer_cast<ArrayType>(refType->getBaseType());
	return dynamic_pointer_cast<ArrayType>(base->type());
}

std::shared_ptr<Type> ArrayAccess::type() const {
	return atype()->getBaseType();
}

bool ArrayAccess::isConst() const {
	return false;
}

std::shared_ptr<Expression> ArrayAccess::value() const {
	return nullptr;
}



