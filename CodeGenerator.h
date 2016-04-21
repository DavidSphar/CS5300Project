/*
 * CodeGenerator.h
 *
 *  Created on: Feb 27, 2016
 *      Author: tamnguyen
 */

#ifndef CODEGENERATOR_H_
#define CODEGENERATOR_H_

#include <memory>
#include <vector>
#include <string>
#include "SymbolTable.h"
#include "LValue.h"
#include "LabelManager.h"


class CodeGenerator {
public:
	static std::shared_ptr<SymbolTable> getSymbolTable();
	static std::shared_ptr<LabelManager> getLabelManager();
    
    static std::vector<std::string>* identList(std::vector<std::string>* list, char* ident);
    static Type* simpleType(char* text);
    static void addVariables(std::vector<std::string>* idList, Type* type);
    static Expression* lvalueExpression(LValue* lvalue);
    static Expression* charExpression(char c);
    static Expression* intExpression(int value);
    static Expression* stringExpression(char *text);
    static Expression* trueExpression();
    static Expression* falseExpression();
    static Expression* eval(Expression* first, Expression* second, std::string op);
    static Expression* evalConstant(Expression* first, Expression* second, std::string op);
    static Expression* evalUnary(Expression* first, std::string op);
    static Expression* evalUnaryConstant(Expression* first, std::string op);
    static Expression* evalChr(Expression* first);
    static Expression* evalOrd(Expression* first);
    static Expression* evalPred(Expression* first);
    static Expression* evalSucc(Expression* first);
    
    static void assignment(LValue* rhs, Expression* lhs);
    static void stop();
    static void read(std::vector<LValue*>* list);
    static void write(std::vector<Expression*>* list);
    static std::vector<Expression*>* exprList(std::vector<Expression*>* list, Expression *expression);
    static std::vector<LValue*>* lvalueList(std::vector<LValue*>* list, LValue *lvalue);
    static void start();
    static void end();
    static void addConstant(char *text, Expression* expression);

    //if statement
    static void newIf();
    static void ifBranch(Expression* condition);
    static void ifBranchEnd();
    static void labelElseIfBranch();
    static void endIf();
    //while statement
    static void newLoop();
    static void loopCheck(Expression* condition, bool boolValue);
    static void endLoop();
    //repeat statement
    static void repeatCheck(Expression* condition);
    //for statement
    static char* setupForLoop(char*text, Expression* rhs);
    static char* forToHead(char* lhs, Expression* rhs);
    static char* forDowntoHead(char* lhs, Expression* rhs);
    static void endForTo(char* text);
    static void endForDownto(char* text);

    //function
    static std::vector<std::pair<std::string, std::shared_ptr<Type>>>* parameter(int ref, std::vector<std::string>* identList, Type* type);
    static std::vector<std::pair<std::string, std::shared_ptr<Type>>>* parameterList(std::vector<std::pair<std::string, std::shared_ptr<Type>>>* finalList, std::vector<std::pair<std::string, std::shared_ptr<Type>>>* list);
    static Function* createFunction(char* name, std::vector<std::pair<std::string, std::shared_ptr<Type>>>* paramList, Type* returnType);
    static void functionForward(Function* func);
    static void functionDeclare(Function* func);
    static void endFunction();
    static Expression* functionCall(char*name, std::vector<Expression*>* expressionList);
    static void functionReturn(Expression* expression);

    //array and record
    static Type* recordType(std::vector<std::pair<std::string, std::shared_ptr<Type>>>* fields);
    static std::vector<std::pair<std::string, std::shared_ptr<Type>>>* fieldList(std::vector<std::pair<std::string, std::shared_ptr<Type>>>* finalList, std::vector<std::pair<std::string, std::shared_ptr<Type>>>* list);
    static std::vector<std::pair<std::string, std::shared_ptr<Type>>>* field(std::vector<std::string>* identList, Type* type);
    static Type* arrayType(Expression* lowExpr, Expression* highExpr, Type* type);
    static void addType(char* text, Type* type);
    static LValue* loadMember(LValue* recordLvalue, char* memberText);
    static LValue* loadArray(LValue* arrayLvalue, Expression* indexExpression);
    static LValue* loadId(char* text);
    static void copy(std::string sourceReg, std::string destReg, std::shared_ptr<Type> type);
    static void copy(std::string sourceReg, std::string destReg, int index, std::shared_ptr<Type> type);
private:
	static std::shared_ptr<SymbolTable> symbolTable;
	static std::shared_ptr<SymbolTable> getPredefinedTable();
	static std::shared_ptr<LabelManager> labelManager;
};

#endif /* CODEGENERATOR_H_ */
