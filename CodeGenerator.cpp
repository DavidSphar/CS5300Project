/*
 * CodeGenerator.cpp
 *
 *  Created on: Feb 27, 2016
 *      Author: tamnguyen
 */

#include <fstream>
#include <iostream>
#include "CodeGenerator.h"
#include "Register.h"
#include "StringTable.h"

using namespace std;
extern ofstream fout;
extern void yyerror(const char *msg);


shared_ptr<SymbolTable> CodeGenerator::symbolTable;

shared_ptr<SymbolTable> CodeGenerator::getSymbolTable() {
	if (!symbolTable) symbolTable = getPredefinedTable();
	return symbolTable;
}

shared_ptr<LabelManager> CodeGenerator::labelManager;

shared_ptr<LabelManager> CodeGenerator::getLabelManager() {
	if (!labelManager) labelManager = make_shared<LabelManager>();
	return labelManager;
}

shared_ptr<SymbolTable> CodeGenerator::getPredefinedTable() {
	auto table = std::make_shared<SymbolTable>(nullptr, GLOBAL);
	table->addType("integer", BuiltInType::getInt());
	table->addType("INTEGER", BuiltInType::getInt());
	table->addType("boolean", BuiltInType::getBool());
	table->addType("BOOLEAN", BuiltInType::getBool());
	table->addType("char", BuiltInType::getChar());
	table->addType("CHAR", BuiltInType::getChar());
	table->addType("string", BuiltInType::getString());
	table->addType("STRING", BuiltInType::getString());

	auto trueExpr = make_shared<Expression>(BuiltInType::getBool(), true);
	table->addConstant("true", trueExpr);
	table->addConstant("TRUE", trueExpr);

	auto falseExpr = make_shared<Expression>(BuiltInType::getBool(), false);
	table->addConstant("false", falseExpr);
	table->addConstant("FALSE", falseExpr);

	Register::init();
	return table;
}

std::vector<std::string>* CodeGenerator::identList(std::vector<std::string>* list, char* ident) {
	if (list == nullptr) {
		list = new vector<std::string>();
	}
	list->push_back(ident);
	return list;
}

Type* CodeGenerator::simpleType(char* text) {
	auto type = getSymbolTable()->lookupType(text);
	if ( type == nullptr) {
		yyerror("ERROR: Type is undefined");
		return nullptr;
	}
	return type.get();
}

void CodeGenerator::addVariables(std::vector<std::string>* idList, Type* type) {
	if (idList == nullptr) {
		yyerror("ERROR: Empty identifier list");
		return;
	}
	if (type == nullptr) {
		yyerror("ERROR: Empty type");
		return;
	}
	if (type->name().length() < 1) { //new array or record type
		if (dynamic_cast<ArrayType*>(type)) {
			auto arrayType = dynamic_cast<ArrayType*>(type);
			arrayType->setName("_TempArrayType");
			auto arrayTypeSharedPtr = make_shared<ArrayType>(*arrayType);
			getSymbolTable()->addType(arrayTypeSharedPtr->name(), arrayTypeSharedPtr);
		} else if (dynamic_cast<RecordType*>(type)) {
			auto recordType = dynamic_cast<RecordType*>(type);
			recordType->setName("_TempRecordType");
			auto recordTypeSharedPtr = make_shared<RecordType>(*recordType);
			getSymbolTable()->addType(recordTypeSharedPtr->name(), recordTypeSharedPtr);
		}
	}
	auto identType = getSymbolTable()->lookupType(type->name());
	for (const auto& ident: *idList) {
		getSymbolTable()->addVariable(ident, identType);
		// cout << *getSymbolTable()->lookupVariable(ident) << endl;
	}
	if (getSymbolTable()->getMemoryLocation() == STACK) {
		fout << "\t" << "addi $sp, $sp, -" << idList->size()*identType->size() << endl;
	}
	// delete the identList after used
	delete idList;
}


Expression* CodeGenerator::lvalueExpression(LValue* lvalue) {
	if (lvalue == nullptr) {
		yyerror("ERROR: Empty lvalue");
		return nullptr;
	}
	if (lvalue->isConst()) {
		auto expression = new Expression(lvalue->type(), lvalue->value()->getValue());
		delete lvalue;
		return expression;
	} else {
		auto expression = new Expression(shared_ptr<LValue>(lvalue));
		if (dynamic_pointer_cast<BuiltIn>(lvalue->type())) {
			fout << "\t" << "lw " << expression->getRegister() << ", 0(" << lvalue->address()->getRegister() << ")" << endl;
		} else {
			fout << "\t" << "move " << expression->getRegister() << ", " << lvalue->address()->getRegister() << endl;
		}
		return expression;
	}
}

Expression* CodeGenerator::charExpression(char c) {
	auto expression = new Expression(BuiltInType::getChar(), c);
	return expression;
}

Expression* CodeGenerator::intExpression(int value) {
	auto expression = new Expression(BuiltInType::getInt(), value);
	return expression;
}

Expression* CodeGenerator::stringExpression(char *text) {
	auto stringID = StringTable::getInstance()->put(text);
	auto expression = new Expression(BuiltInType::getString(), stringID);
	return expression;
}

Expression* CodeGenerator::trueExpression() {
	auto expression = new Expression(BuiltInType::getBool(), 1);
	return expression;
}

Expression* CodeGenerator::falseExpression() {
	auto expression = new Expression(BuiltInType::getBool(), 0);
	return expression;
}

Expression* CodeGenerator::eval(Expression* first, Expression* second, std::string op) {
	if (first == nullptr) {
		yyerror("ERROR: Empty expression");
		return nullptr;
	}
	if (second == nullptr) {
		yyerror("ERROR: Empty expression");
		return nullptr;
	}
	if (first->isConst() && second->isConst()) {
		return evalConstant(first, second, op);
	}
	Expression* expression = nullptr;
	if (op == "or" || op == "and" || op == "seq" || op == "sne" || op == "sle" || op == "slt" ||  op == "sgt") {
		expression = new Expression(BuiltInType::getBool());
	} else {
		expression = new Expression(BuiltInType::getInt());
	}
	if (first->isConst() && (!second->isConst())) {
		auto tempReg = Register::allocate();
		fout << "\t" << "li " << tempReg->getName() << ", " << first->getValue() << endl;
		fout << "\t" << op << " " << expression->getRegister() << ", " << tempReg->getName() << ", " << second->getRegister() << endl;
	} else if ((!first->isConst()) && second->isConst()) {
		auto tempReg = Register::allocate();
		fout << "\t" << "li " << tempReg->getName() << ", " << second->getValue() << endl;
		fout << "\t" << op << " " << expression->getRegister() << ", " << first->getRegister() << ", " << tempReg->getName() << endl;
	} else {
		fout << "\t" << op << " " << expression->getRegister() << ", " << first->getRegister() << ", " << second->getRegister() << endl;
	}
	delete first;
	delete second;
	return expression;
}

Expression* CodeGenerator::evalConstant(Expression* first, Expression* second, std::string op) {
	if (first == nullptr) {
		yyerror("ERROR: Empty expression");
		return nullptr;
	}
	if (second == nullptr) {
		yyerror("ERROR: Empty expression");
		return nullptr;
	}
	Expression *ret = nullptr;
	if (op == "or") {
		ret = new Expression(BuiltInType::getBool(), first->getValue() | second->getValue());
	}
	if (op == "and") {
		ret = new Expression(BuiltInType::getBool(), first->getValue() & second->getValue());
	}
	if (op == "seq") {
		ret = new Expression(BuiltInType::getBool(), first->getValue() == second->getValue());
	}
	if (op == "sne") {
		ret = new Expression(BuiltInType::getBool(), first->getValue() != second->getValue());
	}
	if (op == "sle") {
		ret = new Expression(BuiltInType::getBool(), first->getValue() <= second->getValue());
	}
	if (op == "sge") {
		ret = new Expression(BuiltInType::getBool(), first->getValue() >= second->getValue());
	}
	if (op == "slt") {
		ret = new Expression(BuiltInType::getBool(), first->getValue() < second->getValue());
	}
	if (op == "sgt") {
		ret = new Expression(BuiltInType::getBool(), first->getValue() > second->getValue());
	}
	if (op == "add") {
		ret = new Expression(BuiltInType::getInt(), first->getValue() + second->getValue());
	}
	if (op == "sub") {
		ret = new Expression(BuiltInType::getInt(), first->getValue() - second->getValue());
	}
	if (op == "mul") {
		ret = new Expression(BuiltInType::getInt(), first->getValue() * second->getValue());
	}
	if (op == "div") {
		ret = new Expression(BuiltInType::getInt(), first->getValue() / second->getValue());
	}
	if (op == "rem") {
		ret = new Expression(BuiltInType::getInt(), first->getValue() % second->getValue());
	}
	delete first;
	delete second;
	return ret;
}

Expression* CodeGenerator::evalUnary(Expression* first, std::string op) {
	if (first == nullptr) {
		yyerror("ERROR: Empty expression");
		return nullptr;
	}
	if (first->isConst()) {
		return evalUnaryConstant(first,op);
	}
	Expression* expression = nullptr;
	if (op == "not") {
		expression = new Expression(BuiltInType::getBool());
		auto tempReg = Register::allocate();
		fout << "\t" << "addi " << first->getRegister() << ", " << first->getRegister() << ", 1" << endl;
		fout << "\t" << "li " << tempReg->getName() << ", 2" << endl;
		fout << "\t" << "rem " <<  expression->getRegister() << ", " << first->getRegister() << ", " << tempReg->getName() << endl;
	} else{
		expression = new Expression(BuiltInType::getInt());
		fout << "\t" << "sub " << " " << expression->getRegister() << ", $zero, " << first->getRegister() << endl;
	}

	delete first;
	return expression;
}

Expression* CodeGenerator::evalUnaryConstant(Expression* first, std::string op) {
	if (first == nullptr) {
		yyerror("ERROR: Empty expression");
		return nullptr;
	}
	Expression *ret = nullptr;
	if (op == "not") {
		ret = new Expression(BuiltInType::getBool(), !first->getValue());
	}
	if (op == "neg") {
		ret = new Expression(BuiltInType::getInt(), -first->getValue());
	}
	delete first;
	return ret;
}

Expression* CodeGenerator::evalChr(Expression* first) {
	if (first == nullptr) {
		yyerror("ERROR: Empty expression");
		return nullptr;
	}
	if (first->getType() == BuiltInType::getInt() || first->getType() == BuiltInType::getChar()) {
		first->setType(BuiltInType::getChar());
		return first;
	}
	yyerror("ERROR: CHR only converts int to char");
	return nullptr;
}

Expression* CodeGenerator::evalOrd(Expression* first) {
	if (first == nullptr) {
		yyerror("ERROR: Empty expression");
		return nullptr;
	}
	if (first->getType() == BuiltInType::getChar() || first->getType() == BuiltInType::getInt()) {
		first->setType(BuiltInType::getInt());
		return first;
	}
	yyerror("ERROR: CHR only converts char to int");
	return nullptr;
}

Expression* CodeGenerator::evalPred(Expression* first) {
	if (first == nullptr) {
		yyerror("ERROR: Empty expression");
		return nullptr;
	}
	if (first->getType() == BuiltInType::getBool()) {
		if (first->isConst()) {
			bool value = first->getValue() != 0;
			value = !value;
			first->setValue(value);
		} else {
			auto tempReg = Register::allocate();
			fout << "\t" << "addi " << first->getRegister() << ", " << first->getRegister() << ", 1" << endl;
			fout << "\t" << "li " << tempReg->getName() << ", 2" << endl;
			fout << "\t" << "rem " <<  first->getRegister() << ", " << first->getRegister() << ", " << tempReg->getName() << endl;
		}
		return first;
	} else if (first->getType() == BuiltInType::getInt() || first->getType() == BuiltInType::getChar()) {
		if (first->isConst()) {
			int value = first->getValue();
			first->setValue(value-1);
		} else {
			fout << "\t" << "addi " << first->getRegister() << ", " << first->getRegister() << ", -1" << endl;
		}
		return first;
	} else {
		yyerror("ERROR: PRED is only defined for bool, int and char");
		return nullptr;
	}
}

Expression* CodeGenerator::evalSucc(Expression* first) {
	if (first == nullptr) {
		yyerror("ERROR: Empty expression");
		return nullptr;
	}
	if (first->getType() == BuiltInType::getBool()) {
		if (first->isConst()) {
			bool value = first->getValue() != 0;
			value = !value;
			first->setValue(value);
		} else {
			auto tempReg = Register::allocate();
			fout << "\t" << "addi " << first->getRegister() << ", " << first->getRegister() << ", 1" << endl;
			fout << "\t" << "li " << tempReg->getName() << ", 2" << endl;
			fout << "\t" << "rem " <<  first->getRegister() << ", " << first->getRegister() << ", " << tempReg->getName() << endl;
		}
		return first;
	} else if (first->getType() == BuiltInType::getInt() || first->getType() == BuiltInType::getChar()) {
		if (first->isConst()) {
			int value = first->getValue();
			first->setValue(value-1);
		} else {
			fout << "\t" << "addi " << first->getRegister() << ", " << first->getRegister() << ", 1" << endl;
		}
		return first;
	} else {
		yyerror("ERROR: SUCC is only defined for bool, int and char");
		return nullptr;
	}
}

void CodeGenerator::assignment(LValue* lvalue, Expression* expression) {
	if (lvalue == nullptr) {
		yyerror("ERROR: Empty lvalue");
		return;
	}
	if (expression == nullptr) {
		yyerror("ERROR: Empty expression");
		return;
	}
	if (lvalue->isConst()) {
		yyerror("ERROR: Cannot assign to a constant");
		return;
	}

	if (dynamic_pointer_cast<BuiltIn>(lvalue->type())) {
		if (expression->isConst()) {
			auto tempReg = Register::allocate();
			if (expression->isString()) { // if string, load address
				fout << "\t" << "la " << tempReg->getName() << ", STR" << expression->getValue() << endl;
			} else {
				fout << "\t" << "li " << tempReg->getName() << ", " << expression->getValue() << endl;
			}
			fout << "\t" << "sw " << tempReg->getName() << ", 0(" << lvalue->address()->getRegister() << ")" << endl;
		} else {
			fout << "\t" << "sw " << expression->getRegister() << ", 0(" << lvalue->address()->getRegister() << ")" << endl;
		}
	} else {
		CodeGenerator::copy(expression->getRegister(), lvalue->address()->getRegister(), lvalue->type());
	}
	delete expression; // delete expression after done
	delete lvalue; // delete lvalue after done

}

void CodeGenerator::copy(std::string sourceReg, std::string destReg, std::shared_ptr<Type> type) {
	auto tempReg =  Register::allocate();
	auto i = 0;
	while (i < type->size()) {
		fout << "\t" << "lw " << tempReg->getName() << ", " << i << "(" << sourceReg << ")" << endl;
		fout << "\t" << "sw " << tempReg->getName() << ", " << i << "(" << destReg << ")" << endl;
		i += 4;
	}
}

void CodeGenerator::copy(std::string sourceReg, std::string destReg, int index, std::shared_ptr<Type> type) {
	auto tempReg = Register::allocate();
	auto i = 0;
	while (i < type->size()) {
		fout << "\t" << "lw " << tempReg->getName() << ", " << i << "(" << sourceReg << ")" << endl;
		fout << "\t" << "sw " << tempReg->getName() << ", " << i+index << "(" << destReg << ")" << endl;
		i += 4;
	}
}

void CodeGenerator::stop() {
	fout << "\t" << "li $v0, 10" << endl;
	fout << "syscall" << endl;
}

void CodeGenerator::read(std::vector<LValue*>* list) {
	if (list == nullptr) {
		yyerror("ERROR: Empty lvalue list");
		return;
	}
	for (auto& lvalue: *list) {
		if (lvalue->isConst()) {
			yyerror("ERROR: Cannot read value to a constant");
			return;
		} else {
			if (lvalue->type() == BuiltInType::getInt()) {
				fout << "\t" << "li $v0, 5" << endl;
				fout << "\t" << "syscall" << endl;
				fout << "\t" << "sw $v0, 0(" << lvalue->address()->getRegister() << ")" << endl;
			} else if (lvalue->type() == BuiltInType::getChar()) {
				fout << "\t" << "li $v0, 12" << endl;
				fout << "\t" << "syscall" << endl;
				fout << "\t" << "sw $v0, 0(" << lvalue->address()->getRegister() << ")" << endl;
			} else {
				yyerror("ERROR: Only read integer and character");
			}
		}
		delete lvalue;
	}
	delete list;
}

void CodeGenerator::write(std::vector<Expression*>* list) {
	if (list == nullptr) {
		yyerror("ERROR: Empty expression list");
		return;
	}
	for (auto& expr: *list) {
		if (expr->getType() == BuiltInType::getString()) {
			if (expr->isConst()) {
				fout << "\t" << "la $a0, STR" << expr->getValue() << endl;
			} else {
				fout << "\t" << "move $a0, " << expr->getRegister() << endl;
			}
			fout << "\t" << "li $v0, 4" << endl;
		} else if (expr->getType() == BuiltInType::getChar()) {
			if (expr->isConst()) {
				fout << "\t" << "la $a0, " << expr->getValue() << endl;
			} else {
				fout << "\t" << "move $a0, " << expr->getRegister() << endl;
			}
			fout << "\t" << "li $v0, 11" << endl;
		} else {
			if (expr->isConst()) {
				fout << "\t" << "la $a0, " << expr->getValue() << endl;
			} else {
				fout << "\t" << "move $a0, " << expr->getRegister() << endl;
			}
			fout << "\t" << "li $v0, 1" << endl;
		}
		fout << "\t" << "syscall" << endl;
		delete expr;
	}
	delete list;
	// fout << "\t" << "la $a0, NEWLINE" << endl;
	// fout << "\t" << "li $v0, 4" << endl;
	// fout << "\t" << "syscall" << endl;
}

std::vector<Expression*>* CodeGenerator::exprList(std::vector<Expression*>* list, Expression *expression) {
	if (expression == nullptr) {
		yyerror("ERROR: Empty lvalue");
		return list;
	}
	if (list == nullptr) {
		list = new vector<Expression*>();
	}
	list->push_back(expression);
	return list;
}

std::vector<LValue*>* CodeGenerator::lvalueList(std::vector<LValue*> *list, LValue *lvalue) {
	if (lvalue == nullptr) {
		yyerror("ERROR: Empty lvalue");
		return list;
	}
	if (list == nullptr) {
		list = new vector<LValue*>();
	}
	list->push_back(lvalue);
	return list;
}

void CodeGenerator::start() {
	fout << "prog:" << endl;
}

void CodeGenerator::end() {
	fout << "\t" << "li $v0, 10" << endl;
	fout << "\t" << "syscall" << endl;
	fout << ".data" << endl;
	for (auto& elem: StringTable::getInstance()->getTable()) {
		fout << "\t" << "STR" << elem.second << ": .asciiz " << elem.first << endl;
	}
}

void CodeGenerator::addConstant(char *text, Expression* expression) {
	if (expression == nullptr) {
		yyerror("ERROR: Empty expression");
		return;
	}
	if (!expression->isConst()) {
		yyerror("ERROR: rhs is not a constant");
		return;
	}
	getSymbolTable()->addConstant(text, make_shared<Expression>(expression->getType(), expression->getValue()));
	delete expression;
}


void CodeGenerator::newIf() {
	getLabelManager()->ifLabelStack.push_back(0);
	getLabelManager()->controlLabelStack.push_back(getLabelManager()->controlLabels++);
}

void CodeGenerator::ifBranch(Expression *condition) {
	int ifLabelCount = getLabelManager()->ifLabelStack.back();
	int controlLabelCount = getLabelManager()->controlLabelStack.back();
	if (condition->isConst()) {
		auto tempReg = Register::allocate();
		fout << "\t" << "li " << tempReg->getName() << ", " << condition->getValue() << endl;
		fout << "\t" << "beq " << tempReg->getName() << ", $zero, _IF" << controlLabelCount << "L" << ifLabelCount << endl;
	} else {
		fout << "\t" << "beq " << condition->getRegister() << ", $zero, _IF" << controlLabelCount << "L" << ifLabelCount << endl;
	}
	delete condition;
}

void CodeGenerator::ifBranchEnd() {
	int controlLabelCount = getLabelManager()->controlLabelStack.back();
	fout << "\t" << "j _IF" << controlLabelCount << "END" << endl;
}

void CodeGenerator::labelElseIfBranch() {
	int ifLabelCount = getLabelManager()->ifLabelStack.back()++;
	int controlLabelCount = getLabelManager()->controlLabelStack.back();
	fout << "_IF" << controlLabelCount << "L" << ifLabelCount << ":" << endl;
}

void CodeGenerator::endIf() {
	int ifLabelCount = getLabelManager()->ifLabelStack.back();
	int controlLabelCount = getLabelManager()->controlLabelStack.back();
	fout << "_IF" << controlLabelCount << "L" << ifLabelCount << ":" << endl;
	fout << "_IF" << controlLabelCount << "END:" << endl;
	getLabelManager()->ifLabelStack.pop_back();
	getLabelManager()->controlLabelStack.pop_back();
}

void CodeGenerator::newLoop() {
	getLabelManager()->controlLabelStack.push_back(getLabelManager()->controlLabels++);
	int controlLabelCount = getLabelManager()->controlLabelStack.back();
	fout << "_LOOP" << controlLabelCount << ":" << endl;
}

void CodeGenerator::loopCheck(Expression* condition, bool boolValue) {
	int controlLabelCount = getLabelManager()->controlLabelStack.back();
	string cmd = "beq";
	if (!boolValue) cmd = "bne";
	if (condition->isConst()) {
		auto tempReg = Register::allocate();
		fout << "\t" << "li " << tempReg->getName() << ", " << condition->getValue() << endl;
		fout << "\t" << cmd << " " << tempReg->getName() << ", $zero, _LOOP" << controlLabelCount << "END" << endl;
	} else {
		fout << "\t" << cmd << " " << condition->getRegister() << ", $zero, _LOOP" << controlLabelCount << "END" << endl;
	}
	delete condition;
}

void CodeGenerator::endLoop() {
	int controlLabelCount = getLabelManager()->controlLabelStack.back();
	fout << "\t" << "j _LOOP" << controlLabelCount << endl;
	fout << "_LOOP" << controlLabelCount << "END:" << endl;
	getLabelManager()->controlLabelStack.pop_back();
}

void CodeGenerator::repeatCheck(Expression* condition) {
	int controlLabelCount = getLabelManager()->controlLabelStack.back();
	if (condition->isConst()) {
		auto tempReg = Register::allocate();
		fout << "\t" << "li " << tempReg->getName() << ", " << condition->getValue() << endl;
		fout << "\t" << "beq " << tempReg->getName() << ", $zero, _LOOP" << controlLabelCount << endl;
	} else {
		fout << "\t" << "beq " << condition->getRegister() << ", $zero, _LOOP" << controlLabelCount << endl;
	}
	getLabelManager()->controlLabelStack.pop_back();
	delete condition;
}

char* CodeGenerator::setupForLoop(char* text, Expression* rhs) {
	// make new scope to introduce the new variable in the for loop
	auto newST = make_shared<SymbolTable>(getSymbolTable(), getSymbolTable()->getMemoryLocation());
	newST->setMemoryOffset(getSymbolTable()->getMemoryOffset());
	symbolTable = newST;
	getSymbolTable()->addVariable(text, BuiltInType::getInt());
	if (symbolTable->getMemoryLocation() == STACK) {
		fout << "\t" << "addi $sp, $sp, -4" << endl;
	}
	auto lhs = loadId(text);
	assignment(lhs, rhs); // delete parameters
	newLoop();
	return text;
}

char* CodeGenerator::forToHead(char* text, Expression* rhs) {
	auto lhs = loadId(text);
	auto lhsExpression = lvalueExpression(lhs);
	auto resultExpression = eval(lhsExpression, rhs, "sgt");
	loopCheck(resultExpression, false);
	return text;
}

char* CodeGenerator::forDowntoHead(char* text, Expression* rhs) {
	auto lhs = loadId(text);
	auto lhsExpression = lvalueExpression(lhs);
	auto resultExpression = eval(lhsExpression, rhs, "slt");
	loopCheck(resultExpression, false);
	return text;
}

void CodeGenerator::endForTo(char* text) {
	auto lhs = loadId(text);
	auto lhsExpression = lvalueExpression(lhs);
	auto rhsExpression = new Expression(BuiltInType::getInt(), 1);
	auto rhs = eval(lhsExpression, rhsExpression, "add");
	lhs = loadId(text);
	assignment(lhs, rhs);
	endLoop();
	symbolTable = symbolTable->getParent(); //clear the new variable introduced in the for loop
	if (symbolTable->getMemoryLocation() == STACK) {
		fout << "\t" << "addi $sp, $sp, 4" << endl;
	}
}

void CodeGenerator::endForDownto(char* text) {
	auto lhs = loadId(text);
	auto lhsExpression = lvalueExpression(lhs);
	auto rhsExpression = new Expression(BuiltInType::getInt(), 1);
	auto rhs = eval(lhsExpression, rhsExpression, "sub");
	lhs = loadId(text);
	assignment(lhs, rhs);
	endLoop();
	symbolTable = symbolTable->getParent(); //clear the new variable introduced in the for loop
}

vector<pair<string, shared_ptr<Type>>>* CodeGenerator::parameter(int ref, vector<string>* identList, Type* type) {
	auto result = new vector<pair<string, shared_ptr<Type>>>();
	auto t = CodeGenerator::getSymbolTable()->lookupType(type->name());
	if (ref) t = std::make_shared<ReferenceType>(t);
	for (const auto& elem: *identList) {
		result->push_back(std::make_pair(elem, t));
	}
	delete identList;
	return result;
}

vector<pair<string, shared_ptr<Type>>>* CodeGenerator::parameterList(vector<pair<string, shared_ptr<Type>>>* finalList, vector<pair<string, shared_ptr<Type>>>* list) {
	if (finalList == nullptr) return list;
	for (const auto& elem: *list) {
		finalList->push_back(elem);
	}
	delete list;
	return finalList;
}

Function* CodeGenerator::createFunction(char* name, vector<pair<string, shared_ptr<Type>>>* paramList, Type* returnType) {
	shared_ptr<Type> rt = nullptr;
	if (returnType != nullptr) { // procedure
		rt = CodeGenerator::getSymbolTable()->lookupType(returnType->name());
	}
	vector<pair<string, shared_ptr<Type>>> pl;
	if (paramList != nullptr) {
		for (auto& elem: *paramList) {
			pl.push_back(elem);
		}
		delete paramList;
	}
	auto result = new Function(string(name), pl, rt);
	return result;
}

void CodeGenerator::functionForward(Function* func) {
	auto newFunc = make_shared<Function>(func->getName(), func->getArgs(), func->getReturnType(), false); // create shared ptr
	CodeGenerator::getSymbolTable()->addFunction(newFunc->getName(), newFunc); // add function to symbol table
	delete func;
}

void CodeGenerator::functionDeclare(Function* func) {
	auto newFunc = make_shared<Function>(func->getName(), func->getArgs(), func->getReturnType(), true); // create shared ptr
	CodeGenerator::getSymbolTable()->addFunction(newFunc->getName(), newFunc); // add function to symbol table
	delete func; //delete pointer after done
	auto newST = make_shared<SymbolTable>(getSymbolTable(), STACK);
	symbolTable = newST;
	for (const auto& elem: newFunc->getArgs()) { // add parameters to symbol table
		symbolTable->addParameter(elem.first, elem.second);
	}
	fout << "__" + newFunc->getName() << "__:" << endl;
}

void CodeGenerator::endFunction() {
	symbolTable = symbolTable->getParent(); // restore symbol table
	// procedure may not have return statement, guarantee to return
	fout << "\t" << "move $sp, $fp" << endl;
	fout << "\t" << "jr $ra" << endl;
}

Expression* CodeGenerator::functionCall(char*name, std::vector<Expression*>* expressionList) {
	auto function = getSymbolTable()->lookupFunction(name);

	if (function == nullptr) { // if function is not defined, print error message and return null expression
		yyerror("Function is not defined");
		return nullptr;
	}

	//store $ra and $fp
	fout << "\t" << "addi $sp, $sp, -8" << endl;
	fout << "\t" << "sw $ra, 0($sp)" << endl; // store $ra
	fout << "\t" << "sw $fp, 4($sp)" << endl; // store $fp

	// steal registers
	vector<string> usedRegisters;
	auto registerSpace = Register::getUsed().size()*4;
	if (registerSpace > 0) {
		fout << "\t" << "addi $sp, $sp, -" << registerSpace << endl;
		int index = 0;
		for (auto& elem: Register::getUsed()) {
			fout << "\t" << "sw " << elem << ", " << index << "($sp)" << endl;
			usedRegisters.push_back(elem);
			index += 4;
		}
	}

	// compute total size of parameters
	int parameterSize = 0;
	for (auto& elem: function->getArgs()) {
		parameterSize += elem.second->size();
	}
	// check size of argument list vs function definition
	int argSize = 0;
	if (expressionList != nullptr) argSize = expressionList->size();
	if (argSize != function->getArgs().size()) {
		yyerror("Arguments do not match function definition");
		return nullptr;
	}

	if (parameterSize > 0) {	// if the function has parameters, copy parameters to activation record
		fout << "\t" << "addi $sp, $sp, -" << parameterSize << endl;
		int index = 0;
		for (int i = 0; i < function->getArgs().size(); i++) {
			if (dynamic_pointer_cast<ReferenceType>(function->getArgs()[i].second)) { // pass by reference
				if (!(*expressionList)[i]->isLValue()) {
					yyerror("Argument is expected to be an lvalue");
					return nullptr;
				}
				auto lvalue = (*expressionList)[i]->getLValue();
				fout << "\t" << "sw " << lvalue->address()->getRegister() << ", " << index << "($sp)" << endl;
			} else { // pass by value
				if (dynamic_pointer_cast<BuiltIn>((*expressionList)[i]->getType())) { // built in type
					auto tempReg = Register::allocate();
					if ((*expressionList)[i]->isConst()) {
						fout << "\t" << "li " << tempReg->getName() << ", " << (*expressionList)[i]->getValue() << endl;
					} else {
						fout << "\t" << "move " << tempReg->getName() << ", " << (*expressionList)[i]->getRegister() << endl;
					}
					fout << "\t" << "sw " << tempReg->getName() << ", " << index << "($sp)" << endl;
				} else {
					CodeGenerator::copy((*expressionList)[i]->getLValue()->address()->getRegister(), "$sp", index, (*expressionList)[i]->getType());
				}

			}
			index += function->getArgs()[i].second->size();
		}
		// delete pointers after done
		for (auto& elem: *expressionList) {
			delete elem;
		}
		delete expressionList;
	}

	// move frame pointer to right position
	fout << "\t" << "move $fp, $sp" << endl;

	// call function
	fout << "\t" << "jal __" << function->getName() << "__" << endl;

	// clear parameters
	if (parameterSize > 0) {
		fout << "\t" << "addi $sp, $sp, " << parameterSize << endl;
	}

	// restore registers
	if (registerSpace > 0) {
		int index = 0;
		for (auto& elem: usedRegisters) {
			fout << "\t" << "lw " << elem << ", " << index << "($sp)" << endl;
			index += 4;
		}
		fout << "\t" << "addi $sp, $sp, " << registerSpace << endl;
	}

	// restore $ra, $fp
	fout << "\t" << "lw $ra, 0($sp)" << endl;
	fout << "\t" << "lw $fp, 4($sp)" << endl;
	fout << "\t" << "addi $sp, $sp, 8" << endl;

	if (function->getReturnType() == nullptr) return nullptr; // if procedure, return null expression
	auto expr = new Expression(function->getReturnType()); // create an expression represent return value of a function call
	fout << "\t" << "move " << expr->getRegister() << ", $v0" << endl; // move value from $v0 to the register of expression
	return expr;

}
void CodeGenerator::functionReturn(Expression* expression) {
	if (expression == nullptr) { // return void in procedure, just return
		fout << "\t" << "move $sp, $fp" << endl;
		fout << "\t" << "jr $ra" << endl;
		return;
	}
	if (dynamic_pointer_cast<BuiltIn>(expression->getType())) {
		if (expression->isConst()) { // expression is a constant, string <> (int, char, bool)
			if (expression->isString()) {
				fout << "\t" << "la $v0, STR" << expression->getValue() << endl;
			} else {
				fout << "\t" << "li $v0, " << expression->getValue() << endl;
			}
			fout << "\t" << "move $sp, $fp" << endl;
			fout << "\t" << "jr $ra" << endl;
			delete expression;
			return;
		} else { // expression is not a constant, move value of expression to $v0
			fout << "\t" << "move $v0, " << expression->getRegister() << endl;
			fout << "\t" << "move $sp, $fp" << endl;
			fout << "\t" << "jr $ra" << endl;
			delete expression;
			return;
		}
	} else {
		string retValName = "__" + expression->getType()->name();
		auto retLvalue = new IdAccess(retValName, getSymbolTable());
		assignment(retLvalue, expression);
		retLvalue = new IdAccess(retValName, getSymbolTable());
		fout << "\t" << "move $v0, " << retLvalue->address()->getRegister() << endl;
		fout << "\t" << "move $sp, $fp" << endl;
		fout << "\t" << "jr $ra" << endl;
		delete retLvalue;
		return;
	}
}

Type* CodeGenerator::recordType(std::vector<std::pair<std::string, std::shared_ptr<Type>>>* fields) {
	auto record = new RecordType();
	for (auto& elem: *fields) {
		record->addField(elem.first, elem.second);
	}
	delete fields;
	return record;
}

std::vector<std::pair<std::string, std::shared_ptr<Type>>>* CodeGenerator::fieldList(std::vector<std::pair<std::string, std::shared_ptr<Type>>>* finalList, std::vector<std::pair<std::string, std::shared_ptr<Type>>>* list) {
	if (finalList == nullptr) return list;
	for (auto& elem: *list) {
		finalList->push_back(elem);
	}
	delete list;
	return finalList;
}

std::vector<std::pair<std::string, std::shared_ptr<Type>>>* CodeGenerator::field(std::vector<std::string>* identList, Type* type) {
	auto result = new vector<pair<string, shared_ptr<Type>>>();
	auto typeSharedPtr = CodeGenerator::getSymbolTable()->lookupType(type->name());
	for (auto& elem: *identList) {
		result->push_back(std::make_pair(elem, typeSharedPtr));
	}
	return result;
}

Type* CodeGenerator::arrayType(Expression* lowExpr, Expression* highExpr, Type* type) {
	if ((!lowExpr->isConst())||(!highExpr->isConst())) {
		yyerror("Error in array declaration");
		return nullptr;
	}
	auto lower = lowExpr->getValue();
	auto upper = highExpr->getValue();
	auto typeSharedPtr = CodeGenerator::getSymbolTable()->lookupType(type->name());
	return new ArrayType(lower, upper, typeSharedPtr);
}

void CodeGenerator::addType(char* text, Type* type) {
	if (dynamic_cast<ArrayType*>(type)) {
		auto arrayType = dynamic_cast<ArrayType*>(type);
		auto arrayTypeSharedPtr = make_shared<ArrayType>(*arrayType);
		arrayTypeSharedPtr->setName(string(text));
		getSymbolTable()->addType(string(text), arrayTypeSharedPtr);
		if(getSymbolTable()->getMemoryLocation() == GLOBAL) {
			auto arrayTypeRetId = "__" + arrayTypeSharedPtr->name();
			getSymbolTable()->addVariable(arrayTypeRetId, arrayTypeSharedPtr);
		}
		return;
	} else if (dynamic_cast<RecordType*>(type)) {
		auto recordType = dynamic_cast<RecordType*>(type);
		auto recordTypeSharedPtr = make_shared<RecordType>(*recordType);
		recordTypeSharedPtr->setName(string(text));
		getSymbolTable()->addType(string(text), recordTypeSharedPtr);
		if(getSymbolTable()->getMemoryLocation() == GLOBAL) {
			auto arrayTypeRetId = "__" + recordTypeSharedPtr->name();
			getSymbolTable()->addVariable(arrayTypeRetId, recordTypeSharedPtr);
		}
		return;
	} else {
		auto builtInType = getSymbolTable()->lookupType(type->name());
		getSymbolTable()->addType(string(text), builtInType);
	}
}


LValue* CodeGenerator::loadMember(LValue* recordLvalue, char* memberText) {
	return new MemberAccess(shared_ptr<LValue>(recordLvalue), string(memberText), getSymbolTable());
}

LValue* CodeGenerator::loadArray(LValue* arrayLvalue, Expression* indexExpression) {
	return new ArrayAccess(shared_ptr<LValue>(arrayLvalue), shared_ptr<Expression>(indexExpression), getSymbolTable());
}

LValue* CodeGenerator::loadId(char* text) {
	return new IdAccess(string(text), getSymbolTable());
}


